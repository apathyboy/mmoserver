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

#include <gtest/gtest.h>

#include <driver/mysql_public_iface.h>

#include "anh/server_directory/server_directory.h"

using namespace anh::server_directory;
using namespace std;

// using namespace anh::server_directory;

class ServerDirectoryTest : public testing::Test {
protected:
    virtual void SetUp() {
        database_connection_ = std::shared_ptr<sql::Connection>(sql::mysql::get_driver_instance()->connect(
            "localhost", "root", "swganh"
            ));
        database_connection_->setSchema("config");
    }
    
    // virtual void TearDown() {}

    std::shared_ptr<sql::Connection> database_connection_;
};

/// Creating and using an instance of ServerDirectory requires a valid cluster
/// so the process of constructing should join with a cluster.
TEST_F(ServerDirectoryTest, ByDefaultServerDirectoryHasNoCluster) {
    ServerDirectory(database_connection_, "test_cluster");

//    ServerDirectory server_directory(database_manager, "test_cluster");
//    EXPECT_FALSE(server_directory.getActiveCluster());
}
