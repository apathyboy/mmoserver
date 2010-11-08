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
#include <vector>

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/future.hpp>

#include "Utils/ActiveObject.h"
#include "Utils/typedefs.h"

#define SEND_BUFFER_SIZE 8192

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

class SocketReadThread {
public:
    SocketReadThread(boost::asio::io_service& io_service, uint16_t port, SocketWriteThread* write_thread, Service* service, uint32_t mf_heap_size, bool server_service);
    
    ~SocketReadThread();
    
    boost::shared_future<Session*> createOutgoingConnection(const std::string& address, uint16_t port);
            
    void sendPacket(Packet* packet, Session* session);

    void RemoveAndDestroySession(Session* session);
    
    bool getIsRunning(void) {
        return mIsRunning;
    }

    void requestExit() {
        mExit = true;
    }

protected:
    void asyncReceive_();
    void handleIncomingMessage_(const boost::system::error_code& error, size_t bytes_received);
    void handleIncomingMessage_(const std::string& address, uint16_t port, uint16_t recvLen, Packet* incoming_message);

    void _startup();
    void _shutdown();
    
    utils::ActiveObject active_;
    
    boost::asio::ip::udp::endpoint remote_endpoint_;   //Storage for Current Client
    boost::asio::ip::udp::socket socket_;
    std::vector<int8> receive_buffer_;
    
    int8				mSendBuffer[SEND_BUFFER_SIZE];
    uint16 mMessageMaxSize;
    SessionFactory* mSessionFactory;
    SocketWriteThread* mSocketWriteThread;
    PacketFactory* mPacketFactory;
    MessageFactory* mMessageFactory;
    CompCryptor* mCompCryptor;

    SOCKET mSocket;

    bool mIsRunning;

    uint32 mSessionResendWindowSize;

    boost::thread thread_;
    boost::mutex mSocketReadMutex;
    AddressSessionMap mAddressSessionMap;

    bool mExit;
};

#endif //ANH_NETWORKMANAGER_SOCKETREADTHREAD_H
