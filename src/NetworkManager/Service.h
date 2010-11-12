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

#ifndef ANH_NETWORKMANAGER_SERVICE_H
#define ANH_NETWORKMANAGER_SERVICE_H

#include <cstdint>
#include <functional>
#include <list>

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include <tbb/concurrent_queue.h>

#include "Utils/ActiveObject.h"
#include "Utils/typedefs.h"

#define SEND_BUFFER_SIZE 8192

class CompCryptor;
class MessageFactory;
class NetworkClient;
class Packet;
class PacketFactory;
class Session;
class SessionFactory;
class SocketReadThread;
class SocketWriteThread;
class NetworkManager;
class NetworkCallback;

typedef std::map<uint64_t, Session*> AddressSessionMap;
typedef tbb::concurrent_queue<Session*>	SessionQueue;

class Service {
public:
    Service(NetworkManager* networkManager, bool server_service, uint32 id, int8* localAddress, uint16 localPort,uint32 mfHeapSize);
    ~Service();

    void sendPacket(Packet* packet, Session* session);

    void Process();

    void Connect(NetworkClient* client, int8* address, uint16 port);

    void AddSessionToProcessQueue(Session* session);

    void AddNetworkCallback(NetworkCallback* callback);

    int8* getLocalAddress();
    
    uint16 getLocalPort();
    
    uint32 getId();

    void setId(uint32 id);

    void setQueued(bool b);

    bool isQueued();

    void RemoveAndDestroySession(Session* session);

private:
    void startAsyncReceive_();
    void handleIncomingSocketMessage_(const boost::system::error_code& error, size_t bytes_received);
    void handleIncomingMessage_(const std::string& address, uint16_t port, uint16_t recvLen, Packet* incoming_message);
    
    boost::asio::ip::udp::endpoint remote_endpoint_;   //Storage for Current Client
    boost::asio::ip::udp::socket socket_;
    std::vector<int8> receive_buffer_;
        
    int8 mSendBuffer[SEND_BUFFER_SIZE];
    
    AddressSessionMap mAddressSessionMap;

    NetworkCallback* mCallBack;
    SessionQueue mSessionProcessQueue;

    int8 mLocalAddressName[256];
    NetworkManager* mNetworkManager;
    CompCryptor* mCompCryptor;
    MessageFactory* mMessageFactory;
    PacketFactory* mPacketFactory;
    SessionFactory* mSessionFactory;
    SocketWriteThread* mSocketWriteThread;
    uint32 mId;
    uint32 mLocalAddress;
    uint32 mSessionResendWindowSize;
    uint16 mLocalPort;
    bool mQueued;
    bool mServerService;	//marks us as the serverservice / clientservice
};

#endif //ANH_NETWORKMANAGER_SERVICE_
