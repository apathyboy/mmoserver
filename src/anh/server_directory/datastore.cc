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

#include "anh/server_directory/datastore.h"

#include <cppconn/connection.h>

#include "anh/memory.h"

using namespace anh::server_directory;
using namespace std;

Datastore::Datastore(shared_ptr<sql::Connection> connection) 
    : connection_(connection)
{}

Datastore::~Datastore() {}

std::shared_ptr<Cluster> Datastore::findClusterByName(const std::string& name) const {
    return nullptr;
}

std::shared_ptr<Cluster> Datastore::createCluster(const std::string& name) const {
    return nullptr;
}

std::shared_ptr<Process> Datastore::createProcess(std::shared_ptr<Cluster> cluster, const std::string& name, const std::string& type, const std::string& version, const std::string& address, uint16_t tcp_port, uint16_t udp_port) const {
    return nullptr;
}

std::string Datastore::getClusterTimestamp(std::shared_ptr<Cluster> cluster) const {
    return "";
}

void Datastore::saveProcess(std::shared_ptr<Process> process) const {

}

std::shared_ptr<Cluster> Datastore::findClusterById(uint32_t id) const {
    return nullptr;
}

bool Datastore::deleteProcessById(uint32_t id) const {
    return false;
}
