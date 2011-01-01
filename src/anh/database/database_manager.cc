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

#define NOMINMAX

#include "anh/database/database_manager.h"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>

#include "anh/memory.h"

using namespace anh::database;
using namespace boost::posix_time;
using namespace std;

DatabaseManager::DatabaseManager(sql::Driver* driver)   
: driver_(driver)
{}

DatabaseManager::~DatabaseManager() {
    std::for_each(connections_.begin(), connections_.end(), [] (ConnectionPoolMap::value_type& conn) {
        if (conn.second.empty()) {
            return;
        }

        do {
            // close each connection and pop it from the list
            auto connection = conn.second.front();
            connection->close();
            conn.second.pop();
        } while (!conn.second.empty());
    });
}


bool DatabaseManager::hasStorageType(StorageType storage_type) const {
    ConnectionDataMap::const_accessor a;
    return connection_data_.find(a, storage_type);
}

bool DatabaseManager::registerStorageType(StorageType storage_type, const std::string& schema, const std::string& host, const std::string& username, const std::string& password) {
    if (hasStorageType(storage_type)) {
        return false;
    }

    // create a valid connection to verify the integrity of the data passed in
    auto connection = std::shared_ptr<sql::Connection>(driver_->connect(host, username, password), 
        std::bind(&DatabaseManager::recycleConnection_, this, storage_type, std::placeholders::_1));
    connection->setSchema(schema);

    // insert the data
    ConnectionDataMap::accessor data_accessor;    
    connection_data_.insert(data_accessor, storage_type);
    data_accessor->second = make_shared<ConnectionData>(schema, host, username, password);

    // add the created connection to the connection pool
    ConnectionPoolMap::accessor pool_accessor;
    connections_.insert(pool_accessor, storage_type);
    pool_accessor->second.push(connection);

    return true;
}

bool DatabaseManager::hasConnection(StorageType storage_type) const {
    ConnectionPoolMap::const_accessor a;

    // return false if unable to find a connection pool for the requested type
    if (!connections_.find(a, storage_type)) {
        return false;
    }

    // return whether or not the connection pool for this storage type is empty
    return !a->second.empty();
}

shared_ptr<sql::Connection> DatabaseManager::requestConnection(StorageType storage_type) {
    ConnectionPoolMap::accessor a;

    // return false if unable to find a connection pool for the requested type
    if (!connections_.find(a, storage_type) || a->second.empty()) {
        // lookup the connection data for the requested storage type and fail out
        // if not found
        ConnectionDataMap::accessor data_accessor;
        if (!connection_data_.find(data_accessor, storage_type)) {
            assert(false && "Requested a storage type that has not been registered");
            return nullptr;
        }
        
        // create a valid connection to verify the integrity of the data passed in
        auto connection_data = data_accessor->second;
        auto connection = std::shared_ptr<sql::Connection>(driver_->connect(connection_data->host, connection_data->username, connection_data->password), 
            std::bind(&DatabaseManager::recycleConnection_, this, storage_type, std::placeholders::_1));
        connection->setSchema(connection_data->schema);

        return connection;
    }
    
    auto connection = a->second.front();
    a->second.pop();

    return connection;
}

void DatabaseManager::recycleConnection_(StorageType storage_type, sql::Connection* connection) {
    if (connection->isClosed()) {
        delete connection;
        return;
    }
    
    ConnectionPoolMap::accessor pool_accessor;
    connections_.insert(pool_accessor, storage_type);
    pool_accessor->second.push(std::shared_ptr<sql::Connection>(connection, std::bind(&DatabaseManager::recycleConnection_, this, storage_type, std::placeholders::_1)));
}
