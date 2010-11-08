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

#include "SocketReadThread.h"

#ifdef ERROR
#undef ERROR
#endif
#include <glog/logging.h>

#include <boost/thread/thread.hpp>

#include <functional>

#if defined(_MSC_VER)
#else
#include <sys/socket.h>
#include <arpa/inet.h>

#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#define closesocket		close
#endif

#include "Service.h"
#include "CompCryptor.h"
#include "NetworkClient.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Session.h"
#include "SessionFactory.h"
#include "Socket.h"
#include "SocketWriteThread.h"

#include "NetworkManager/MessageFactory.h"

#define RECEIVE_BUFFER 4096
//======================================================================================================================

SocketReadThread::SocketReadThread(
    boost::asio::io_service& io_service,
    uint16_t port,
    SocketWriteThread* write_thread,
    Service* service,
    uint32_t mf_heap_size,
    bool server_service)
    : socket_(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
    , receive_buffer_(RECEIVE_BUFFER)
    , mSessionFactory(nullptr)
    , mSocketWriteThread(write_thread)
    , mPacketFactory(nullptr)
    , mCompCryptor(nullptr)
    , mSocket(0)
    , mIsRunning(false)
{   
    if(server_service) {
        mMessageMaxSize = gNetConfig->getServerServerReliableSize();
        mSessionResendWindowSize = gNetConfig->getServerPacketWindow();
    } else {
        mMessageMaxSize = gNetConfig->getServerClientReliableSize();
        mSessionResendWindowSize = gNetConfig->getClientPacketWindow();
    } 
    
    // Startup our factories
    mMessageFactory = new MessageFactory(mf_heap_size,service->getId());
    mPacketFactory	= new PacketFactory(server_service);
    mSessionFactory = new SessionFactory(write_thread, service, mPacketFactory, mMessageFactory, server_service);

    mCompCryptor = new CompCryptor();

    LOG(WARNING) << "Listening on port [" << port << "]";
    
    _startup();
    asyncReceive_();
}

//======================================================================================================================

SocketReadThread::~SocketReadThread()
{
    _shutdown();
    mExit = true;

    delete mPacketFactory;
    delete mSessionFactory;

    delete mMessageFactory;

    delete mCompCryptor;
}


boost::shared_future<Session*> SocketReadThread::createOutgoingConnection(const std::string& address, uint16_t port) {
    auto p = std::make_shared<boost::promise<Session*>>();
    boost::shared_future<Session*> ret(p->get_future());

    active_.Send([=] {
        LOG(INFO) << "Connecting to remote server";

        Session* newSession = mSessionFactory->CreateSession();
        newSession->setCommand(SCOM_Connect);
        newSession->setAddress(inet_addr(address.c_str()));
        newSession->setPort(htons(port));
        newSession->setResendWindowSize(mSessionResendWindowSize);

        uint64 hash = newSession->getAddress() | (((uint64)newSession->getPort()) << 32);
        mAddressSessionMap.insert(std::make_pair(hash,newSession));

        mSocketWriteThread->NewSession(newSession);

        p->set_value(newSession);
    });

    return ret;
}


void SocketReadThread::sendPacket(Packet* packet, Session* session) { active_.Send([=] {
    struct sockaddr     toAddr;
    uint32              toLen = sizeof(toAddr), outLen;

    packet->setReadIndex(0);
    uint16 packetType = packet->getUint16();
    uint8  packetTypeLow = *(packet->getData());
    //uint8  packetTypeHigh = *(packet->getData()+1);

    // Set our TimeSent
    packet->setTimeSent(Anh_Utils::Clock::getSingleton()->getStoredTime());

    // Setup our to address
    toAddr.sa_family = AF_INET;
    *((unsigned int*)&toAddr.sa_data[2]) = session->getAddress();     // Ports and addresses are stored in network order.
    *((unsigned short*)&(toAddr.sa_data[0])) = session->getPort();    // Only need to convert for humans.

    // Copy our 2 byte header.
    *((uint16*)mSendBuffer) = *((uint16*)packet->getData());

    // Compress the packet if needed.
    if(packet->getIsCompressed())
    {
        if(packetTypeLow == 0)
        {
            // Compress our packet, but not the header
            outLen = mCompCryptor->Compress(packet->getData() + 2, packet->getSize() - 2, mSendBuffer + 2, sizeof(mSendBuffer));
        }
        else
        {
            outLen = mCompCryptor->Compress(packet->getData() + 1, packet->getSize() - 1, mSendBuffer + 1, sizeof(mSendBuffer));
        }

        // If we compressed it, place a 1 at the end of the buffer.
        if(outLen)
        {
            if(packetTypeLow == 0)
            {
                mSendBuffer[outLen + 2] = 1;
                outLen += 3;  //thats 2 (uncompressed) headerbytes plus the encryption flag
            }
            else
            {
                mSendBuffer[outLen + 1] = 1;
                outLen += 2;
            }
        }
        // else a 0 - so no compression
        else
        {
            memcpy(mSendBuffer, packet->getData(), packet->getSize());
            outLen = packet->getSize();

            mSendBuffer[outLen] = 0;
            outLen += 1;
        }
    }
    else if(packetType == SESSIONOP_SessionResponse || packetType == SESSIONOP_CriticalError)
    {
        memcpy(mSendBuffer, packet->getData(), packet->getSize());
        outLen = packet->getSize();
    }
    else
    {
        memcpy(mSendBuffer, packet->getData(), packet->getSize());
        outLen = packet->getSize();

        mSendBuffer[outLen] = 0;
        outLen += 1;
    }

    // Encrypt the packet if needed.
    if(packet->getIsEncrypted())
    {
        if(packetTypeLow == 0)
        {
            mCompCryptor->Encrypt(mSendBuffer + 2, outLen - 2, session->getEncryptKey()); // -2 header is not encrypted
        }
        else if(packetTypeLow < 0x0d)
        {
            mCompCryptor->Encrypt(mSendBuffer + 1, outLen - 1, session->getEncryptKey()); // - 1 header is not encrypted
        }

        packet->setCRC(mCompCryptor->GenerateCRC(mSendBuffer, outLen, session->getEncryptKey()));


        mSendBuffer[outLen] = (uint8)(packet->getCRC() >> 8);
        mSendBuffer[outLen + 1] = (uint8)packet->getCRC();
        outLen += 2;
    }

    boost::asio::ip::udp::endpoint dest(boost::asio::ip::address::from_string(session->getAddressString()), session->getPortHost());
    socket_.send_to(
        boost::asio::buffer(mSendBuffer, outLen),
        dest);
    //sent = sendto(mSocket, mSendBuffer, outLen, 0, &toAddr, toLen);

   // if (sent < 0)
   // {
   //     LOG(WARNING) << "Unkown Error from socket sendto: " << errno;
   // }
}); }

//======================================================================================================================

void SocketReadThread::RemoveAndDestroySession(Session* session) { active_.Send([=] {
    if (! session) {
        return;
    }

    // Find and remove the session from the address map.
    uint64 hash = session->getAddress() | (((uint64)session->getPort()) << 32);

    LOG(INFO) << "Service " << mSessionFactory->getService()->getId() << ": Removing Session("	<< inet_ntoa(*((in_addr*)(&hash))) 
    <<  ", " << session->getPortHost() << "), AddressMap: " << mAddressSessionMap.size() - 1 << " hash " << hash;

    boost::mutex::scoped_lock lk(mSocketReadMutex);

    AddressSessionMap::iterator iter = mAddressSessionMap.find(hash);

    if(iter != mAddressSessionMap.end())
    {
        mAddressSessionMap.erase(iter);

        mSessionFactory->DestroySession(session);
    }
    else
    {
        LOG(INFO) << "Service " << mSessionFactory->getService()->getId() << ": Removing Session FAILED("	<< inet_ntoa(*((in_addr*)(&hash))) 
        <<  ", " << session->getPortHost() << "), AddressMap: " << mAddressSessionMap.size() - 1 << " hash " << hash;
    }
}); }


void SocketReadThread::asyncReceive_() {
    socket_.async_receive_from(
        boost::asio::buffer(receive_buffer_),
        remote_endpoint_,
        std::bind(&SocketReadThread::handleIncomingMessage_, this, 
            std::placeholders::_1, 
            std::placeholders::_2));
}


void SocketReadThread::handleIncomingMessage_(const boost::system::error_code& error, size_t bytes_received) {    
    if (error && error != boost::asio::error::message_size && error != boost::asio::error::connection_refused) {
        LOG(WARNING) << "Error reading from socket: " << error.message().c_str();
        LOG(WARNING) << "Bytes received: " << bytes_received;
        asyncReceive_();
        return;
    }
    
    if (bytes_received) {
        Packet* incoming_message = mPacketFactory->CreatePacket();
        incoming_message->addData(&receive_buffer_[0], bytes_received);
        handleIncomingMessage_(remote_endpoint_.address().to_string(), 
                               remote_endpoint_.port(), 
                               bytes_received,
                               incoming_message);
    }

    asyncReceive_();
}


void SocketReadThread::handleIncomingMessage_(const std::string& address, uint16_t port, uint16_t recvLen, Packet* incoming_message) {
    Session* session;
    uint16_t decompressLen = 0;
    
    uint64 hash = inet_addr(address.c_str()) | (((uint64)htons(port)) << 32);
    
    uint8  packetTypeLow	= incoming_message->peekUint8();
    uint16 packetType		= incoming_message->getUint16();

    boost::mutex::scoped_lock lk(mSocketReadMutex);

    AddressSessionMap::iterator i = mAddressSessionMap.find(hash);

    if(i != mAddressSessionMap.end())
    {
        session = (*i).second;
    }
    else
    {
        // We should only be creating a new session if it's a session request packet
        if(packetType == SESSIONOP_SessionRequest)
        {
            session = mSessionFactory->CreateSession();
            session->setSocketReadThread(this);
            session->setPacketFactory(mPacketFactory);
            session->setAddress(inet_addr(address.c_str()));  // Store the address and port in network order so we don't have to
            session->setPort(htons(port));  // convert them all the time.  Only convert for humans.
            session->setResendWindowSize(mSessionResendWindowSize);

            // Insert the session into our address map and process list
            mAddressSessionMap.insert(std::make_pair(hash, session));
            mSocketWriteThread->NewSession(session);
            session->mHash = hash;

            LOG(INFO) << "Added Service " << mSessionFactory->getService()->getId() << ": New Session(" 
            << address << ", " << port << "), AddressMap: " << mAddressSessionMap.size();
        }
        else
        {
            LOG(WARNING) << "Socket Read Thread Session not found. Type:0x" << packetType;
            return;
        }
    }

    lk.unlock();

    // I don't like any of the code below, but it's going to take me a bit to work out a good way to handle decompression
    // and decryption.  It's dependent on session layer protocol information, which should not be looked at here.  Should
    // be placed in Session, though I'm not sure how or where yet.
    // Set the size of the packet

    // Validate our date header.  If it's not a valid header, drop it.
    if(packetType > 0x00ff && (packetType & 0x00ff) == 0 && session != NULL)
    {
        switch(packetType)
        {
        case SESSIONOP_Disconnect:
        case SESSIONOP_DataAck1:
        case SESSIONOP_DataAck2:
        case SESSIONOP_DataAck3:
        case SESSIONOP_DataAck4:
        case SESSIONOP_DataOrder1:
        case SESSIONOP_DataOrder2:
        case SESSIONOP_DataOrder3:
        case SESSIONOP_DataOrder4:
        case SESSIONOP_Ping:
        {
            // Before we do anything else, check the CRC.
            uint32 packetCrc = mCompCryptor->GenerateCRC(incoming_message->getData(), recvLen - 2, session->getEncryptKey());  // - 2 crc

            uint8 crcLow  = (uint8)*(incoming_message->getData() + recvLen - 1);
            uint8 crcHigh = (uint8)*(incoming_message->getData() + recvLen - 2);

            if (crcLow != (uint8)packetCrc || crcHigh != (uint8)(packetCrc >> 8))
            {
                // CRC mismatch.  Dropping packet.
                //gLogger->hexDump(mReceivePacket->getData(),mReceivePacket->getSize());
                DLOG(INFO) << "DIS/ACK/ORDER/PING dropped.";
                return;
            }

            // Decrypt the packet
            mCompCryptor->Decrypt(incoming_message->getData() + 2, recvLen - 4, session->getEncryptKey());

            // Send the packet to the session.
            session->HandleSessionPacket(incoming_message);
        }
        break;

        case SESSIONOP_MultiPacket:
        case SESSIONOP_NetStatRequest:
        case SESSIONOP_NetStatResponse:
        case SESSIONOP_DataChannel1:
        case SESSIONOP_DataChannel2:
        case SESSIONOP_DataChannel3:
        case SESSIONOP_DataChannel4:
        case SESSIONOP_DataFrag1:
        case SESSIONOP_DataFrag2:
        case SESSIONOP_DataFrag3:
        case SESSIONOP_DataFrag4:
        {
            // Before we do anything else, check the CRC.
            uint32 packetCrc = mCompCryptor->GenerateCRC(incoming_message->getData(), recvLen - 2, session->getEncryptKey());

            uint8 crcLow  = (uint8)*(incoming_message->getData() + recvLen - 1);
            uint8 crcHigh = (uint8)*(incoming_message->getData() + recvLen - 2);

            if (crcLow != (uint8)packetCrc || crcHigh != (uint8)(packetCrc >> 8)) {
                // CRC mismatch.  Dropping packet.
                LOG(INFO) << "Socket Read Thread: Reliable Packet dropped." << packetType << " CRC mismatch.";
                return;
            }

            // Decrypt the packet
            mCompCryptor->Decrypt(incoming_message->getData() + 2, recvLen - 4, session->getEncryptKey());  // don't hardcode the header buffer or CRC len.

            // Decompress the packet
            Packet* decompressed_packet = mPacketFactory->CreatePacket();
            decompressLen = mCompCryptor->Decompress(incoming_message->getData() + 2, recvLen - 5, decompressed_packet->getData() + 2, decompressed_packet->getMaxPayload() - 5);

            if(decompressLen > 0)
            {
                decompressed_packet->setIsCompressed(true);
                decompressed_packet->setSize(decompressLen + 2); // add the packet header size
                *((uint16*)(decompressed_packet->getData())) = *((uint16*)incoming_message->getData());
                session->HandleSessionPacket(decompressed_packet);

                break;
            }
            else
            {
                // we have to remove comp/crc
                incoming_message->setSize(incoming_message->getSize() - 3);
            }
        }

        case SESSIONOP_SessionRequest:
        case SESSIONOP_SessionResponse:
        case SESSIONOP_FatalError:
        case SESSIONOP_FatalErrorResponse:
            //case SESSIONOP_Reset:
        {
            // Send the packet to the session.

            session->HandleSessionPacket(incoming_message);
        }
        break;

        default:
        {
            DLOG(INFO) << "SocketReadThread: Dont know what todo with this packet! --tmr <3";
        }
        break;

        } //end switch(sessionOp)
    }
    // Validate that our data is actually fastpath
    else if(packetTypeLow < 0x0d && session != NULL) // highest fastpath I've seen is 0x0b -tmr
    {
        // Before we do anything else, check the CRC.
        uint32	packetCrc	= mCompCryptor->GenerateCRC(incoming_message->getData(), recvLen - 2, session->getEncryptKey());
        uint8	crcLow		= (uint8)*(incoming_message->getData() + recvLen - 1);
        uint8	crcHigh		= (uint8)*(incoming_message->getData() + recvLen - 2);

        if(crcLow != (uint8)packetCrc || crcHigh != (uint8)(packetCrc >> 8)) {
            // CRC mismatch.  Dropping packet.
            LOG(INFO) << "Packet dropped.  CRC mismatch.";
            return;
        }

        // It's a 'fastpath' packet.  Send it directly up the data channel
        mCompCryptor->Decrypt(incoming_message->getData() + 1, recvLen - 3, session->getEncryptKey());  // don't hardcode the header buffer or CRc len.

        // Decompress the packet
        decompressLen	= 0;
        uint8 compFlag	= (uint8)*(incoming_message->getData() + recvLen - 3);
        Packet* decompressed_packet = mPacketFactory->CreatePacket();

        if(compFlag == 1) {
            decompressLen = mCompCryptor->Decompress(incoming_message->getData() + 1, recvLen - 4, decompressed_packet->getData() + 1, decompressed_packet->getMaxPayload() - 4);
        }

        if(decompressLen > 0) {
            decompressed_packet->setIsCompressed(true);
            decompressed_packet->setSize(decompressLen + 1); // add the packet header size

            *((uint8*)(decompressed_packet->getData())) = *((uint8*)incoming_message->getData());

            // send the packet up the stack
            session->HandleFastpathPacket(decompressed_packet);
        } else {
            // send the packet up the stack, remove comp/crc
            incoming_message->setSize(incoming_message->getSize() - 3);
            session->HandleFastpathPacket(incoming_message);
        }
    }
}


void SocketReadThread::_startup(void)
{
    // Initialization is done.  All of it.  :)
    mIsRunning = true;
    mExit = false;
}

//======================================================================================================================

void SocketReadThread::_shutdown(void)
{
    // Shutting down
    mIsRunning = false;
}

//======================================================================================================================





