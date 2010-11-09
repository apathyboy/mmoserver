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

#include "NetworkManager/Service.h"

#include <cassert>

#ifdef ERROR
#undef ERROR
#endif
#include <glog/logging.h>

#include "NetworkManager/Message.h"
#include "NetworkManager/NetworkCallback.h"
#include "NetworkManager/NetworkClient.h"
#include "NetworkManager/NetworkManager.h"
#include "NetworkManager/Packet.h"
#include "NetworkManager/Session.h"
#include "NetworkManager/SocketReadThread.h"
#include "NetworkManager/SocketWriteThread.h"

Service::Service(NetworkManager* networkManager, bool serverservice, uint32 id, int8* localAddress, uint16 localPort,uint32 mfHeapSize) 
    : mNetworkManager(networkManager)
    , mSocketReadThread(0)
    , mSocketWriteThread(0)
    , mLocalAddress(0)
    , mLocalPort(0)
    , mQueued(false)
    , mServerService(serverservice)
{
    mCallBack = NULL;
    mId = id;

    assert(strlen(localAddress) < 256 && "Address length should be less than 256");
    strcpy(mLocalAddressName, localAddress);
    mLocalAddress = inet_addr(localAddress);
    mLocalPort = htons(localPort);

    // Create our read/write socket classes
    mSocketWriteThread = new SocketWriteThread(this, mServerService);
    mSocketReadThread = new SocketReadThread(mNetworkManager->io_service(), localPort, mSocketWriteThread, this, mfHeapSize, mServerService);
    mSocketWriteThread->setSocket(mSocketReadThread);
}


Service::~Service() {
    Session* session = 0;

    while(mSessionProcessQueue.size()) {
        session = mSessionProcessQueue.pop();

        if(session) {
            mSocketReadThread->RemoveAndDestroySession(session);
        }
    }

    delete mSocketWriteThread;
    delete mSocketReadThread;
}


void Service::Process() {
    //we only ever get here with a connected session

    // Get the current count of Sessions to be processed.  We can't just check to see if the queue is empty, since
    // the other threads could keep placing more Packets in the queue, and this could cause a stall in the
    // main thread.
    Session* session = 0;
    //Message* message = 0;
    NetworkClient* newClient = 0;
    uint32 sessionCount = mSessionProcessQueue.size();

    for(uint32 i = 0; i < sessionCount; i++) {
        // Grab our next Service to process
        session = mSessionProcessQueue.pop();

        if(!session)
            continue;

        session->setInIncomingQueue(false);

        // Check to see if we're in the process of connecting or disconnecting.
        if(session->getStatus() == SSTAT_Connecting) {
            newClient = mCallBack->handleSessionConnect(session, this);

            // They returned a client to us, so keep the session.
            if(newClient) {
                session->setClient(newClient);
                newClient->setSession(session);
                session->setStatus(SSTAT_Connected);
            } else {
                // Remove the session, they don't want it.
                session->setCommand(SCOM_Disconnect);
            }
        } else if(session->getStatus() == SSTAT_Disconnecting) {
            if(session->getClient()) {
                mCallBack->handleSessionDisconnect(session->getClient());
                session->setClient(0);
            }

            // We're now dis connected.
            session->setStatus(SSTAT_Disconnected);
            continue;
        } else if(session->getStatus() == SSTAT_Destroy) {
            mSocketReadThread->RemoveAndDestroySession(session);
            continue;
        }

        // Now send up any messages waiting.

        // Iterate through our priority queue's looking for messages.
        uint32 messageCount = session->getIncomingQueueMessageCount();

        if(!session->getClient()) {
            for(uint32 j = 0; j < messageCount; j++) {
                Message* message = session->getIncomingQueueMessage();
                message->setPendingDelete(true);
            }
        } else {
            for(uint32 j = 0; j < messageCount; j++) {
                Message* message = session->getIncomingQueueMessage();

                message->ResetIndex();

                // At this point we can assume we have a client object, so send the data up.
                mCallBack->handleSessionMessage(session->getClient(), message);
            }
        }

        session->setInIncomingQueue(false);
    }
}


void Service::Connect(NetworkClient* client, int8* address, uint16 port) {
    LOG(INFO) << "New connection to " << address << " on port " << port;

    boost::shared_future<Session*> result = mSocketReadThread->createOutgoingConnection(address, port);
    Session* session = result.get();

    while(session->getStatus() != SSTAT_Connected) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
    }

    client->setSession(session);
    session->setClient(client);
}


void Service::AddSessionToProcessQueue(Session* session) {
    if(!session->getInIncomingQueue()) {
        session->setInIncomingQueue(true);
        mSessionProcessQueue.push(session);

    }

    mNetworkManager->AddServiceToProcessQueue(this);
}


void Service::AddNetworkCallback(NetworkCallback* callback) {
    assert((mCallBack == NULL) && "dammit");
    mCallBack = callback;
}


int8* Service::getLocalAddress() {
    return inet_ntoa(*(struct in_addr *)&mLocalAddress);
}


uint16 Service::getLocalPort() {
    return ntohs(mLocalPort);
}


uint32 Service::getId(void) {
    return mId;
}


void Service::setId(uint32 id) {
    mId = id;
}


void Service::setQueued(bool b) {
    mQueued = b;
}


bool Service::isQueued() {
    return mQueued;
}
