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

#include "NetworkManager.h"

#include <algorithm>
#include <functional>

#include "NetConfig.h"
#include "Service.h"


#include "Utils/typedefs.h"


//======================================================================================================================

NetworkManager::NetworkManager() 
    : io_service_()
    , io_work_(new boost::asio::io_service::work(io_service_))
    , worker_threads_(std::min<uint32_t>(boost::thread::hardware_concurrency()!=0?boost::thread::hardware_concurrency():2,8))
    , mServiceIdIndex(1)
{
    // for safety, in case someone forgot to init previously
    NetConfig::Init();
    
    std::for_each(worker_threads_.begin(), worker_threads_.end(), [=] (boost::thread& thread) {
        thread = boost::thread(std::bind(&boost::asio::io_service::run, &io_service_));
    });
}

//======================================================================================================================

NetworkManager::~NetworkManager() {
    io_work_.reset();

    std::for_each(worker_threads_.begin(), worker_threads_.end(), [=] (boost::thread& thread) {
        if (thread.joinable()) {
            thread.join();
        }
    });
}


boost::asio::io_service& NetworkManager::io_service() {
    return io_service_;
}

//======================================================================================================================

void NetworkManager::Process() {
    Service*	service = 0;
    uint32		serviceCount = mServiceProcessQueue.unsafe_size();

    for(uint32 i = 0; i < serviceCount; i++) {
        // Grab our next Service to process
        if (mServiceProcessQueue.try_pop(service)) {
            if(service) {
                service->Process();
                service->setQueued(false);
            }
        }
    }
}


//======================================================================================================================

Service* NetworkManager::GenerateService(int8* address, uint16 port,uint32 mfHeapSize,  bool serverservice)
{
    Service* newService = 0;

    newService = new Service(this, serverservice, mServiceIdIndex++, address, port,mfHeapSize);

    return newService;
}

//======================================================================================================================

void NetworkManager::DestroyService(Service* service)
{
    delete(service);
}

//======================================================================================================================

Client* NetworkManager::Connect()
{
    Client* newClient = 0;

    return newClient;
}

//======================================================================================================================

void NetworkManager::RegisterCallback(NetworkCallback* callback)
{
}

//======================================================================================================================

void NetworkManager::UnregisterCallback(NetworkCallback* callback)
{
}

//======================================================================================================================

void NetworkManager::AddServiceToProcessQueue(Service* service) {
    if(!service->isQueued()) {
        service->setQueued(true);
        mServiceProcessQueue.push(service);
    }
}

