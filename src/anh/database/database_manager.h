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

#ifndef DATABASE_MANAGER_DATABASE_MANAGER_H_
#define DATABASE_MANAGER_DATABASE_MANAGER_H_

#include <cstdint>
#include <memory>
#include <queue>
#include <string>

#include <boost/noncopyable.hpp>

#include <tbb/concurrent_hash_map.h>
#include <tbb/concurrent_queue.h>

#include "anh/hash_string.h"

namespace sql {
    class Connection;
    class Driver;
}

namespace anh {
namespace database {
    
struct ConnectionData {
    ConnectionData(std::string schema_, std::string host_, std::string username_, std::string password_)
        : schema(std::move(schema_))
        , host(std::move(host_))
        , username(std::move(username_))
        , password(std::move(password_))
    {}

    std::string schema;
    std::string host;
    std::string username;
    std::string password;
};

typedef anh::HashString StorageType;
typedef tbb::concurrent_hash_map<StorageType, std::shared_ptr<ConnectionData>> ConnectionDataMap;
typedef std::queue<std::shared_ptr<sql::Connection>> ConnectionPool;
typedef tbb::concurrent_hash_map<StorageType, ConnectionPool> ConnectionPoolMap;

/*! Manages multithreaded database query processing.
*/
class DatabaseManager : private boost::noncopyable {
public:
	/**
	 * \brief Default constructor.
	 *
	 * \param db_config Database configuration options.
	 * \see DatabaseConfig
	 */
	explicit DatabaseManager(sql::Driver* driver);

    ~DatabaseManager();

    bool hasStorageType(StorageType storage_type) const;

    bool registerStorageType(StorageType storage_type, const std::string& schema, const std::string& host, const std::string& username, const std::string& password);

    bool hasConnection(StorageType storage_type) const;

    std::shared_ptr<sql::Connection> requestConnection(StorageType storage_type);
    
private:
    DatabaseManager();

    void recycleConnection_(StorageType storage_type, sql::Connection* connection);

    sql::Driver* driver_;
    
    ConnectionDataMap connection_data_;
    ConnectionPoolMap connections_;
};

}  // namespace database
}  // namespace anh

#endif  // DATABASE_MANAGER_DATABASE_MANAGER_H_
