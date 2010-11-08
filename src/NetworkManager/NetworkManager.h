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

#ifndef ANH_NETWORKMANAGER_NETWORKMANAGER_H
#define ANH_NETWORKMANAGER_NETWORKMANAGER_H

#include <memory>
#include <queue>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/thread/thread.hpp>

#include <tbb/concurrent_queue.h>

#include "Utils/concurrent_queue.h"
#include "Utils/typedefs.h"
#include "Service.h"

//======================================================================================================================

class Client;
class LogManager;
class NetworkCallback;
class Session;

//======================================================================================================================

typedef tbb::concurrent_queue<Service*> ServiceQueue;

//======================================================================================================================

class NetworkManager {
public:

    NetworkManager(void);
    ~NetworkManager(void);
    
    boost::asio::io_service& io_service();

    void		Process();

    Service*	GenerateService(int8* address, uint16 port,uint32 mfHeapSize, bool serverservice);
    void		DestroyService(Service* service);
    Client*		Connect();

    void		RegisterCallback(NetworkCallback* callback);
    void		UnregisterCallback(NetworkCallback* callback);

    void		AddServiceToProcessQueue(Service* service);
    
private:    
    boost::asio::io_service io_service_;
    std::unique_ptr<boost::asio::io_service::work> io_work_;

    std::vector<boost::thread> worker_threads_;

    ServiceQueue		mServiceProcessQueue;

    uint32			mServiceIdIndex;
};

#endif // ANH_NETWORKMANAGER_NETWORKMANAGER_H
