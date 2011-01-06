/*
 This file is part of MMOServer. For more information, visit http://swganh.com
 
 Copyright (c) 2006 - 2010 The SWG:ANH Team

 MMOServer is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 MMOServer is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with MMOServer.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ANH_LOGINSERVER_ZONESERVER_H
#define ANH_LOGINSERVER_ZONESERVER_H

#include "Common/Server.h"

namespace anh {
namespace database {
    class DatabaseManager;
}  // namespace database

namespace server_directory {
    class ServerDirectory;
}  // namespace server_directory
}  // namespace anh

class NetworkManager;
class Service;
class LoginManager;
class DatabaseManager;
class Database;


//======================================================================================================================
class LoginServer : public common::BaseServer
{
public:
    LoginServer(int argc, char* argv[]);
    ~LoginServer(void);

    void	Process(void);

private:
    std::shared_ptr<anh::database::DatabaseManager> database_manager_;
    std::shared_ptr<anh::server_directory::ServerDirectory> server_directory_;
    NetworkManager*									mNetworkManager;
    Service*										mService;
    ::DatabaseManager*								mDatabaseManager;
    Database*										mDatabase;
    LoginManager*									mLoginManager;
};



#endif // ANH_LOGINSERVER_ZONESERVER_H


