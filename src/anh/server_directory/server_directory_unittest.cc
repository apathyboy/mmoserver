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
#include <gmock/gmock.h>

#include "anh/server_directory/cluster.h"
#include "anh/server_directory/datastore.h"
#include "anh/server_directory/process.h"
#include "anh/server_directory/server_directory.h"

using namespace anh::server_directory;
using namespace std;
using namespace testing;

// Test harness for the ServerDirectory unit tests, provides a database_connection_
// to use for testing.
class ServerDirectoryTest : public testing::Test {
protected:
    virtual void SetUp() {
        test_cluster_ = make_shared<Cluster>(getTestCluster());
    }

    Cluster getTestCluster() {
        Cluster cluster(1, 1, "test_cluster", Cluster::OFFLINE, "", "");
        return cluster;
    }

    std::shared_ptr<Cluster> test_cluster_;
};

class MockDatastore : public DatastoreInterface {
public:
    MOCK_CONST_METHOD1(findClusterByName, Cluster(const std::string& name));
};

/// Creating and using an instance of ServerDirectory requires a valid cluster
/// so the process of constructing should join with a cluster.
TEST_F(ServerDirectoryTest, CreatingServerDirectoryJoinsToCluster) {
    auto datastore = make_shared<MockDatastore>();
    EXPECT_CALL(*datastore, findClusterByName("test_cluster"))
        .WillOnce(Return(*test_cluster_));
        
    ServerDirectory server_directory(datastore, "test_cluster");
    
    Cluster cluster = server_directory.active_cluster();

    EXPECT_EQ("test_cluster", cluster.name());
}
