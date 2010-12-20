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

#include "anh/database/database_manager.h"

#ifdef WIN32
#include <regex>
#else
#include <boost/regex.hpp>
#endif

#include "anh/database/connection.h"
#include "anh/database/mysql/connection.h"


#ifdef WIN32
using std::regex;
using std::smatch;
using std::regex_match;
#else
using boost::regex;
using boost::smatch;
using boost::regex_match;
#endif

using namespace anh::database;

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() {}

std::shared_ptr<IConnection> DatabaseManager::connect(const std::string& dsn, const std::string& username, const std::string& password) {
    std::shared_ptr<IConnection> connection = nullptr;
    return connection;
}

void DatabaseManager::process() {

}

std::shared_ptr<IConnection> DatabaseManager::connection(const std::string& schema) const {
    return nullptr;
}


std::tuple<std::string, std::string, std::string> DatabaseManager::parseDsn(const std::string& dsn) const {
    std::tuple<std::string, std::string, std::string> ret;

    const regex r("([a-z]+):(dbname|host)=([a-z0-9]+);(dbname|host)=([a-z0-9]+)");
    smatch m;

    if (regex_match(dsn, m, r)) {
        if (m[2].str() == "dbname") {
            ret = std::tuple<std::string, std::string, std::string>(m[1].str(), m[5].str(), m[3].str());
        } else {
            ret = std::tuple<std::string, std::string, std::string>(m[1].str(), m[3].str(), m[5].str());
        }
    }

    return ret;
}

