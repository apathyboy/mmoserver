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

 You should have received a copy of the GNU aGeneral Public License
 along with MMOServer.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include "anh/database/database_manager.h"
#include "anh/database/mysql/statement.h"

using namespace anh::database;
using namespace std;

std::string some_func() {
    static std::string some_string("some_string");
    return some_string;
}

/// This test shows how to use the DatabaseManager can parse a DSN string.
TEST(DatabaseManagerTest, CanParseDsnFromString) {
    DatabaseManager db_manager;

    std::string type;
    std::string host;
    std::string dbname;

    std::tie(type, host, dbname) = db_manager.parseDsn("mysql:dbname=testdb;host=localhost");

    EXPECT_EQ(std::string("mysql"), type);
    EXPECT_EQ(std::string("localhost"), host);
    EXPECT_EQ(std::string("testdb"), dbname);

    std::tie(type, host, dbname) = db_manager.parseDsn("mysql:host=otherhost;dbname=testdb2");
    
    EXPECT_EQ(std::string("mysql"), type);
    EXPECT_EQ(std::string("otherhost"), host);
    EXPECT_EQ(std::string("testdb2"), dbname);

    mysql::Statement statement;
    statement.bindParam("something", std::bind(some_func));
    statement.execute();
}
