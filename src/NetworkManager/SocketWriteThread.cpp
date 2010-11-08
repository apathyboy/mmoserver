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

#include "SocketWriteThread.h"
#include "SocketReadThread.h"

#ifdef ERROR
#undef ERROR
#endif
#include <glog/logging.h>

#include "CompCryptor.h"
#include "NetConfig.h"
#include "Packet.h"
#include "Service.h"
#include "Session.h"

#include "Utils/rand.h"

#include <functional>

#include <boost/thread/thread.hpp>

//======================================================================================================================

SocketWriteThread::SocketWriteThread(Service* service, bool serverservice) :
    mService(0),
    mCompCryptor(0),
    mIsRunning(false)
{
    mService = service;

    if(serverservice)
    {

        mServerService = true;
        mMessageMaxSize = gNetConfig->getServerServerReliableSize();

    }
    else
    {
        mServerService = false;
        mMessageMaxSize = gNetConfig->getServerClientReliableSize();
    }


    // We do have a global clock object, don't use seperate clock and times for every process.
    // mClock = new Anh_Utils::Clock();

    // Create our CompCryptor object.
    mCompCryptor = new CompCryptor();

    // start our thread
    mThread = boost::thread(std::tr1::bind(&SocketWriteThread::run, this));

#ifdef _WIN32
    HANDLE mtheHandle = mThread.native_handle();
    SetPriorityClass(mtheHandle,REALTIME_PRIORITY_CLASS);
#endif


    //our thread load values
    //mThreadTime = mLastThreadTime = 0;
    mLastTime =   Anh_Utils::Clock::getSingleton()->getLocalTime();
    //lastThreadProcessingTime = threadProcessingTime = 0;

    unCount = 	reCount = 0;
}

SocketWriteThread::~SocketWriteThread()
{
    LOG(INFO) << "Socket Write Thread Ended.";

    // shutdown our thread
    mExit = true;

    mThread.interrupt();
    mThread.join();

    delete mCompCryptor;

    // delete(mClock);
}

//======================================================================================================================
void SocketWriteThread::run()
{
    Session*            session;
    Packet*             packet;

    // Call our internal _startup method
    _startup();

    uint32 packets = 50;
    if(mServerService)
        packets = 1000;


    // Main loop
    while(!mExit)
    {

        uint32 sessionCount = mSessionQueue.size();

        for(uint32 i = 0; i < sessionCount; i++)
        {
            uint32 packetCount = 0;
            session = mSessionQueue.pop();

            if(!session)
                continue;

            // Process our session
            session->ProcessWriteThread();

            // Send any outgoing reliable packets
            //uint32 rcount = 0;

            while (session->getOutgoingReliablePacketCount())
            {
                packetCount++;
                if(packetCount > packets)
                    break;

                LOG(INFO) << "Reliable packet sent";
                packet = session->getOutgoingReliablePacket();
                _sendPacket(packet, session);
            }


            packetCount = 0;

            // Send any outgoing unreliable packets
            //uint32 ucount = 0;
            while (session->getOutgoingUnreliablePacketCount())
            {
                LOG(INFO) << "Unreliable packet sent";
                packet = session->getOutgoingUnreliablePacket();
                _sendPacket(packet, session);
                session->DestroyPacket(packet);
            }


            // If the session is still in a connected state, Put us back in the queue.
            if (session->getStatus() != SSTAT_Disconnected)
            {
                mSessionQueue.push(session);
            }
            else
            {
                DLOG(INFO) << "Socket Write Thread: Destroy Session";

                session->setStatus(SSTAT_Destroy);
                mService->AddSessionToProcessQueue(session);
            }
        }


        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }

    // Shutdown internally
    _shutdown();
}

//======================================================================================================================

void SocketWriteThread::_startup(void)
{
    // Initialization is done.  All of it.  :)
    mIsRunning = true;
    mExit = false;
}

//======================================================================================================================

void SocketWriteThread::_shutdown(void)
{
    // Shutting down
    mIsRunning = false;
}

//======================================================================================================================

void SocketWriteThread::_sendPacket(Packet* packet, Session* session) {
    socket_thread_->sendPacket(packet, session);
}

//======================================================================================================================

void SocketWriteThread::NewSession(Session* session)
{
    //using concurrent queue that has a recursive mutex
    mSessionQueue.push(session);
}

//======================================================================================================================



void SocketWriteThread::setSocket(SocketReadThread* socket_thread) {
    socket_thread_ = socket_thread;
}
