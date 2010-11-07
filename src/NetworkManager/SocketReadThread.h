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

#ifndef ANH_NETWORKMANAGER_SOCKETREADTHREAD_H
#define ANH_NETWORKMANAGER_SOCKETREADTHREAD_H

#include <cstdint>
#include <list>
#include <map>
#include <string>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/future.hpp>

#include "Utils/ActiveObject.h"
#include "Utils/typedefs.h"

class SocketWriteThread;
class PacketFactory;
class SessionFactory;
class MessageFactory;
class CompCryptor;
class Session;
class Service;
class Packet;

typedef std::list<Session*>			SessionList;
typedef std::map<uint64,Session*>	AddressSessionMap;

struct NewConnection {
public:
    int8              mAddress[256];
    uint16            mPort;
    Session*          mSession;
};

class SocketReadThread {
public:
    SocketReadThread(SOCKET socket, SocketWriteThread* writeThread, Service* service,uint32 mfHeapSize, bool serverservice);
    ~SocketReadThread();

    void run();

    boost::shared_future<Session*> createOutgoingConnection(const std::string& address, uint16_t port);

    void RemoveAndDestroySession(Session* session);

    NewConnection* getNewConnectionInfo(void) {
        return &mNewConnection;
    }

    bool getIsRunning(void) {
        return mIsRunning;
    }

    void requestExit() {
        mExit = true;
    }

protected:
    void handleIncomingMessage_(struct sockaddr_in from, uint16_t recvLen, Packet* incoming_message);

    void _startup();
    void _shutdown();
    
    utils::ActiveObject active_;

    uint16 mMessageMaxSize;
    SessionFactory* mSessionFactory;
    SocketWriteThread* mSocketWriteThread;
    PacketFactory* mPacketFactory;
    MessageFactory* mMessageFactory;
    CompCryptor* mCompCryptor;
    NewConnection mNewConnection;

    SOCKET mSocket;

    bool mIsRunning;

    uint32 mSessionResendWindowSize;

    boost::thread mThread;
    boost::mutex mSocketReadMutex;
    AddressSessionMap mAddressSessionMap;

    bool mExit;
};

#endif //ANH_NETWORKMANAGER_SOCKETREADTHREAD_H
