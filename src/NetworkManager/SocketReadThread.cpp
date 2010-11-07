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

#include <glog/logging.h>

#include "CompCryptor.h"
#include "NetworkClient.h"
#include "Packet.h"
#include "PacketFactory.h"
#include "Service.h"
#include "Session.h"
#include "SessionFactory.h"
#include "Socket.h"
#include "SocketWriteThread.h"

#include "NetworkManager/MessageFactory.h"

#include <boost/thread/thread.hpp>

#if defined(__GNUC__)
// GCC implements tr1 in the <tr1/*> headers. This does not conform to the TR1
// spec, which requires the header without the tr1/ prefix.
#include <tr1/functional>
#else
#include <functional>
#endif

#if defined(_MSC_VER)
#define socklen_t int
#else
#include <sys/socket.h>
#include <arpa/inet.h>

#define INVALID_SOCKET	-1
#define SOCKET_ERROR	-1
#define closesocket		close
#endif

//======================================================================================================================

SocketReadThread::SocketReadThread(SOCKET socket, SocketWriteThread* writeThread, Service* service,uint32 mfHeapSize, bool serverservice) 
    : mSessionFactory(0)
    , mPacketFactory(0)
    , mCompCryptor(0)
    , mSocket(0)
    , mIsRunning(false)
{
    if(serverservice)
    {
        mMessageMaxSize = gNetConfig->getServerServerReliableSize();
        mSessionResendWindowSize = gNetConfig->getServerPacketWindow();
    }
    else
    {
        mMessageMaxSize = gNetConfig->getServerClientReliableSize();
        mSessionResendWindowSize = gNetConfig->getClientPacketWindow();
    }

    mSocket = socket;
    mSocketWriteThread = writeThread;

    // Init our NewConnection object
    memset(mNewConnection.mAddress, 0, sizeof(mNewConnection.mAddress));
    mNewConnection.mPort = 0;
    mNewConnection.mSession = 0;

    // Startup our factories
    mMessageFactory = new MessageFactory(mfHeapSize,service->getId());
    mPacketFactory	= new PacketFactory(serverservice);
    mSessionFactory = new SessionFactory(writeThread, service, mPacketFactory, mMessageFactory, serverservice);

    mCompCryptor = new CompCryptor();
    
    // start our thread
    boost::thread t(std::tr1::bind(&SocketReadThread::run, this));
    mThread = boost::move(t);

#ifdef _WIN32
    HANDLE th =  mThread.native_handle();
    SetPriorityClass(th,REALTIME_PRIORITY_CLASS);
#endif
    //SetPriorityClass(th,NORMAL_PRIORITY_CLASS);
}

//======================================================================================================================

SocketReadThread::~SocketReadThread()
{
    mExit = true;

    mThread.interrupt();
    mThread.join();

    delete mPacketFactory;
    delete mSessionFactory;

    delete mMessageFactory;

    delete mCompCryptor;
}

//======================================================================================================================

void SocketReadThread::run(void)
{
    struct sockaddr_in  from;
    uint32              address, fromLen = sizeof(from), count;
    int16               recvLen = 0;
    uint16              port = 0;
    uint16              decompressLen = 0;
    fd_set              socketSet;
    struct              timeval tv;

    FD_ZERO(&socketSet);

    // Call our internal _startup method
    _startup();

    while(!mExit) {
        // Reset our internal members so we can use the packet again.
        // Build a new fd_set structure
        FD_SET(mSocket, &socketSet);

        // We're going to block for 250ms.
        tv.tv_sec   = 0;
        tv.tv_usec  = 250;

        count = select(mSocket+1, &socketSet, 0, 0, &tv);

        if(count && FD_ISSET(mSocket, &socketSet)) {
            LOG(WARNING) << "Message received on port " << port;
            // Read any incoming packets.
            Packet* incoming_message = mPacketFactory->CreatePacket();
            recvLen = recvfrom(mSocket, incoming_message->getData(),(int) mMessageMaxSize, 0, (sockaddr*)&from, reinterpret_cast<socklen_t*>(&fromLen));

            if(recvLen <= 0) {
#if(ANH_PLATFORM == ANH_PLATFORM_WIN32)

                int errorNr = 0;
                errorNr = WSAGetLastError();

                char errorMsg[512];

                if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorNr, MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPTSTR)errorMsg, (sizeof(errorMsg) / sizeof(TCHAR)) - 1, NULL)) {
                    LOG(WARNING) << "Error(recvFrom): " << errorMsg;
                } else {
                    LOG(WARNING) << "Error(recvFrom): " << errorNr;
                }
#endif
                continue;
            }

            if(recvLen > mMessageMaxSize) {
                LOG(INFO) << "Socket Read Thread Received Size > mMessageMaxSize: " << recvLen;
            }

            // Get our remote Address and port
            address		= from.sin_addr.s_addr;
            port		= from.sin_port;
            
            // Grab our packet type
            incoming_message->Reset();           // Reset our internal members so we can use the packet again.
            incoming_message->setSize(recvLen); // crc is subtracted by the decryption

            handleIncomingMessage_(from, recvLen, incoming_message);
        }

        boost::this_thread::sleep(boost::posix_time::microseconds(10));
    }

    // Shutdown internally
    _shutdown();
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


//======================================================================================================================

void SocketReadThread::RemoveAndDestroySession(Session* session)
{
    if (! session) {
        return;
    }

    // Find and remove the session from the address map.
    uint64 hash = session->getAddress() | (((uint64)session->getPort()) << 32);

    LOG(INFO) << "Service " << mSessionFactory->getService()->getId() << ": Removing Session("	<< inet_ntoa(*((in_addr*)(&hash))) 
    <<  ", " << ntohs(session->getPort()) << "), AddressMap: " << mAddressSessionMap.size() - 1 << " hash " << hash;

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
        <<  ", " << ntohs(session->getPort()) << "), AddressMap: " << mAddressSessionMap.size() - 1 << " hash " << hash;
    }
}



void SocketReadThread::handleIncomingMessage_(struct sockaddr_in from, uint16_t recvLen, Packet* incoming_message) {
    Session* session;
    uint16_t decompressLen = 0;
    
    // Get our remote Address and port
    uint32_t address = from.sin_addr.s_addr;
    uint16_t port    = from.sin_port;

    uint64 hash = address | (((uint64)port) << 32);
    
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
            session->setAddress(address);  // Store the address and port in network order so we don't have to
            session->setPort(port);  // convert them all the time.  Only convert for humans.
            session->setResendWindowSize(mSessionResendWindowSize);

            // Insert the session into our address map and process list
            mAddressSessionMap.insert(std::make_pair(hash, session));
            mSocketWriteThread->NewSession(session);
            session->mHash = hash;

            LOG(INFO) << "Added Service " << mSessionFactory->getService()->getId() << ": New Session(" 
            <<inet_ntoa(from.sin_addr) << ", " << ntohs(session->getPort()) << "), AddressMap: " << mAddressSessionMap.size();
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





