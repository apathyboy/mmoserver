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

#ifndef ANH_DATABASE_DATABASE_MANAGER_H_
#define ANH_DATABASE_DATABASE_MANAGER_H_

#include <list>
#include <memory>
#include <string>
#include <tuple>

#include <boost/noncopyable.hpp>

namespace anh {
namespace database {

class IConnection;

class IDatabaseManager {
public:
    virtual ~IDatabaseManager() = 0 {}
    
    /*! Processes all current database connections.
    */
    virtual void process() = 0;
};

/*! Manages multiple database connections.
*/
class DatabaseManager : public IDatabaseManager , boost::noncopyable {
public:
    DatabaseManager();
    ~DatabaseManager();

    /*! Connects to a specified database.
    *
    * \param dsn The data source name, contains the information needed to connect to a database.
    * \param username The username for accessing the requested schema.
    * \param password The password for accessing the requested schema.
    *
    * \return The instance of the database connection.
    */
    std::shared_ptr<IConnection> connect(const std::string& dsn, const std::string& username = "", const std::string& password = "");

    /*! Processes all current database connections.
    */
    void process();

    std::shared_ptr<IConnection> connection(const std::string& schema) const;

    /*! Parses a dsn string into it's 3 components, the database engine type 
    * and the host and schema to connect to.
    *
    * \param dsn The dsn string to process.
    * \return A tuple containing the individual components from a dsn string.
    */
    std::tuple<std::string, std::string, std::string> parseDsn(const std::string& dsn) const;

private:
    typedef std::list<std::pair<std::string, std::shared_ptr<IConnection>>> ConnectionList;
    ConnectionList connection_list_;
};

}  // namespace database
}  // namespace anh

#endif  // ANH_DATABASE_DATABASE_MANAGER_H_
