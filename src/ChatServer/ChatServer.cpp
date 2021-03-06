/*
---------------------------------------------------------------------------------------
This source file is part of SWG:ANH (Star Wars Galaxies - A New Hope - Server Emulator)

For more information, visit http://www.swganh.com

Copyright (c) 2006 - 2010 The SWG:ANH Team
---------------------------------------------------------------------------------------
Use of this source code is governed by the GPL v3 license that can be found
in the COPYING file or at http://www.gnu.org/licenses/gpl-3.0.html

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
---------------------------------------------------------------------------------------
*/

#include "ChatServer.h"

#include <glog/logging.h>
// External references
#include "ChatManager.h"
#include "CSRManager.h"
#include "GroupManager.h"
#include "TradeManagerChat.h"
#include "StructureManagerChat.h"
#include "CharacterAdminHandler.h"
#include "PlanetMapHandler.h"

#include "NetworkManager/NetworkManager.h"
#include "NetworkManager/Service.h"

#include "DatabaseManager/DataBinding.h"
#include "DatabaseManager/Database.h"
#include "DatabaseManager/DatabaseManager.h"
#include "DatabaseManager/DatabaseResult.h"

#include "NetworkManager/DispatchClient.h"
#include "NetworkManager/MessageDispatch.h"
#include "NetworkManager/MessageFactory.h"
#include "Common/ConfigManager.h"

#include "Utils/utils.h"
#include "Utils/clock.h"

#include <boost/thread/thread.hpp>
#include <cstring>

//======================================================================================================================

ChatServer* gChatServer;

//======================================================================================================================

ChatServer::ChatServer() : mNetworkManager(0),mDatabaseManager(0),mRouterService(0),mDatabase(0), mLastHeartbeat(0)
{
    Anh_Utils::Clock::Init();
    //gLogger->printSmallLogo();
    LOG(WARNING) << "Chat Server Startup";

    // Create and startup our core services.
    mDatabaseManager = new DatabaseManager();

    mNetworkManager = new NetworkManager();

    // Connect to the DB and start listening for the RouterServer.
    mDatabase = mDatabaseManager->connect(DBTYPE_MYSQL,
                                          (char*)(gConfig->read<std::string>("DBServer")).c_str(),
                                          gConfig->read<int>("DBPort"),
                                          (char*)(gConfig->read<std::string>("DBUser")).c_str(),
                                          (char*)(gConfig->read<std::string>("DBPass")).c_str(),
                                          (char*)(gConfig->read<std::string>("DBName")).c_str());

    mDatabase->executeProcedureAsync(0, 0, "CALL sp_ServerStatusUpdate('chat', NULL, NULL, NULL);");

    mRouterService = mNetworkManager->GenerateService((char*)gConfig->read<std::string>("BindAddress").c_str(), gConfig->read<uint16>("BindPort"),gConfig->read<uint32>("ServiceMessageHeap")*1024,true);

    // We need to register our IP and port in the DB so the connection server can connect to us.
    // Status:  0=offline, 1=loading, 2=online
    _updateDBServerList(1);

    // Instant the messageFactory. It will also run the Startup ().
    (void)MessageFactory::getSingleton();		// Use this a marker of where the factory is instanced.
    // The code itself here is not needed, since it will instance itself at first use.

    // Connect to the ConnectionServer;
    _connectToConnectionServer();

    // Place all startup code here.
    mMessageDispatch = new MessageDispatch(mRouterService);

    // load up our ChatManager
    mChatManager = ChatManager::Init(mDatabase,mMessageDispatch);
    mTradeManagerChatHandler = TradeManagerChatHandler::Init(mDatabase,mMessageDispatch,mChatManager);
    mStructureManagerChatHandler = StructureManagerChatHandler::Init(mDatabase,mMessageDispatch,mChatManager);
    mCSRManager = CSRManager::Init(mDatabase, mMessageDispatch, mChatManager);

    // load up GroupManager
    mGroupManager = GroupManager::Init(mMessageDispatch);

    mCharacterAdminHandler = new CharacterAdminHandler(mDatabase, mMessageDispatch);

    mPlanetMapHandler = new PlanetMapHandler(mDatabase,mMessageDispatch);

    ChatMessageLib::Init(mClient);

    // We're done initializing.
    _updateDBServerList(2);

    LOG(WARNING) << "Chat Server startup complete";
    //gLogger->printLogo();
    // std::string BuildString(GetBuildString());

    LOG(WARNING) << "Chat Server - Build " << ConfigManager::getBuildString().c_str();
    LOG(WARNING) << "Welcome to your SWGANH Experience!";
}

//======================================================================================================================

ChatServer::~ChatServer()
{
    LOG(WARNING) << "ChatServer shutting down...";

    // We're shutting down, so update the DB again.
    _updateDBServerList(0);

    // Shutdown the various handlers
    delete mCharacterAdminHandler;

    delete mPlanetMapHandler;

    delete (mChatManager);
    delete (mCSRManager);
    mTradeManagerChatHandler->Shutdown();
    delete (mTradeManagerChatHandler);

    delete mMessageDispatch;

    // Shutdown and delete our core services.
    mNetworkManager->DestroyService(mRouterService);
    delete mNetworkManager;

    MessageFactory::getSingleton()->destroySingleton();	// Delete message factory and call shutdown();

    delete mDatabaseManager;

    LOG(WARNING) << "ChatServer Shutdown Complete";
}

//======================================================================================================================

void ChatServer::Process()
{
    // Process our game modules
    mMessageDispatch->Process();
    gMessageFactory->Process();

    //  Process our core services
    mDatabaseManager->process();
    mNetworkManager->Process();
    mCharacterAdminHandler->Process();
    mPlanetMapHandler->Process();
    mTradeManagerChatHandler->Process();
    mStructureManagerChatHandler->Process();


    // Heartbeat once in awhile
    if (Anh_Utils::Clock::getSingleton()->getLocalTime() - mLastHeartbeat > 180000)//main loop every 10ms
    {
        mLastHeartbeat = static_cast<uint32>(Anh_Utils::Clock::getSingleton()->getLocalTime());
        DLOG(INFO) << "ChatServer Heartbeat.";
    }
}


//======================================================================================================================

void ChatServer::_updateDBServerList(uint32 status)
{
    // Update the DB with our status.  This must be synchronous as the connection server relies on this data.
    mDatabase->executeProcedureAsync(0, 0, "CALL sp_ServerStatusUpdate('chat', %u, '%s', %u);", status, mRouterService->getLocalAddress(), mRouterService->getLocalPort()); // SQL - Update server status
 
}

//======================================================================================================================

void ChatServer::_connectToConnectionServer()
{
    ProcessAddress processAddress;
    memset(&processAddress, 0, sizeof(ProcessAddress));

    // Query the DB to find out who this is.
    // setup our databinding parameters.
    DataBinding* binding = mDatabase->createDataBinding(5);
    binding->addField(DFT_uint32, offsetof(ProcessAddress, mType), 4);
    binding->addField(DFT_bstring, offsetof(ProcessAddress, mAddress), 16);
    binding->addField(DFT_uint16, offsetof(ProcessAddress, mPort), 2);
    binding->addField(DFT_uint32, offsetof(ProcessAddress, mStatus), 4);
    binding->addField(DFT_uint32, offsetof(ProcessAddress, mActive), 4);

    // Setup our statement
    DatabaseResult* result = mDatabase->executeSynchSql("SELECT id, address, port, status, active FROM config_process_list WHERE name='connection';");
    
    uint64 count = result->getRowCount();

    // If we found them
    if (count == 1)
    {
        // Retrieve our routes and add them to the map.
        result->getNextRow(binding, &processAddress);
    }

    // Delete our DB objects.
    mDatabase->destroyDataBinding(binding);
    mDatabase->destroyResult(result);

    // Now connect to the ConnectionServer
    mClient = new DispatchClient();

	LOG(INFO) << "New connection to " << processAddress.mAddress.getAnsi() << " on port " << processAddress.mPort;
    mRouterService->Connect(mClient, processAddress.mAddress.getAnsi(), processAddress.mPort);
}

//======================================================================================================================

void handleExit()
{
    delete gChatServer;
}

//======================================================================================================================

int main(int argc, char* argv[])
{
    // Initialize the google logging.
    google::InitGoogleLogging(argv[0]);

#ifndef _WIN32
    google::InstallFailureSignalHandler();
#endif

    FLAGS_log_dir = "./logs";
    FLAGS_stderrthreshold = 1;

    try {
        ConfigManager::Init("ChatServer.cfg");
    } catch (file_not_found) {
        std::cout << "Unable to find configuration file: " << CONFIG_DIR << "ChatServer.cfg" << std::endl;
        exit(-1);
    }

    //set stdout buffers to 0 to force instant flush
    setvbuf( stdout, NULL, _IONBF, 0);

    bool exit = false;

    gChatServer = new ChatServer();

    // Since startup completed successfully, now set the atexit().  Otherwise we try to gracefully shutdown a failed startup, which usually fails anyway.
    //atexit(handleExit);

    // Main loop
    while (!exit)
    {
        gChatServer->Process();

        if(Anh_Utils::kbhit())
        {
            if(std::cin.get() == 'q')
                break;
        }

        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }

    // Shutdown things
    delete gChatServer;

    delete ConfigManager::getSingletonPtr();

    return 0;
}


