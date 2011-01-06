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

#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>

#include "anh/memory.h"

using namespace anh::server_directory;
using namespace boost::posix_time;
using namespace std;

Datastore::Datastore(shared_ptr<sql::Connection> connection) 
    : connection_(connection)
{}

Datastore::~Datastore() {}

std::shared_ptr<Cluster> Datastore::findClusterByName(const std::string& name) const {
    std::unique_ptr<sql::PreparedStatement> statement(connection_->prepareStatement(
        "SELECT * FROM cluster "
        "WHERE name = ? LIMIT 1"));

    statement->setString(1, name);

    std::unique_ptr<sql::ResultSet> result(statement->executeQuery());

    // if no results are found return a nullptr
    if (!result->next()) {
        return nullptr;
    }

    auto cluster = make_shared<Cluster>(
        result->getUInt("id"),
        result->getUInt("primary_id"),
        result->getString("name"),
        static_cast<Cluster::StatusType>(result->getInt("status")),
        result->getString("created_at"),
        result->getString("updated_at"));

    return cluster;
}

std::shared_ptr<Cluster> Datastore::createCluster(const std::string& name) const {
    std::unique_ptr<sql::PreparedStatement> statement(connection_->prepareStatement(
        "INSERT INTO cluster(name, created_at, updated_at) "
        "VALUES(?, NOW(), NOW())"));

    statement->setString(1, name);

    // if the statement fails to process return a nullptr
    if (statement->executeUpdate() <= 0) {
        return nullptr;
    }

    statement.reset(connection_->prepareStatement(
        "SELECT * FROM cluster WHERE id = LAST_INSERT_ID()"));
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery());

    if (!result->next()) {
        return nullptr;
    }

    auto cluster = make_shared<Cluster>(
        result->getUInt("id"),
        result->getUInt("primary_id"),
        result->getString("name"),        
        static_cast<Cluster::StatusType>(result->getInt("status")),
        result->getString("created_at"),
        result->getString("updated_at"));

    return cluster;
}

std::shared_ptr<Process> Datastore::createProcess(std::shared_ptr<Cluster> cluster, const std::string& name, const std::string& type, const std::string& version, const std::string& address, uint16_t tcp_port, uint16_t udp_port) const {

    std::unique_ptr<sql::PreparedStatement> statement(connection_->prepareStatement(
        "INSERT INTO process (cluster_id, "
                             "name, "
                             "type, "
                             "version, "
                             "address, "
                             "tcp_port, "
                             "udp_port, "
                             "status, "
                             "last_pulse, "
                             "created_at, "
                             "updated_at) "
        "VALUES(?, ?, ?, ?, INET_ATON(?), ?, ?, 0, NOW(), NOW(), NOW())"));

    statement->setInt(1, cluster->id());
    statement->setString(2, name);
    statement->setString(3, type);
    statement->setString(4, version);
    statement->setString(5, address);
    statement->setUInt(6, static_cast<uint32_t>(tcp_port));
    statement->setUInt(7, static_cast<uint32_t>(udp_port));
    
    if (! (statement->executeUpdate() > 0)) {
        return nullptr;
    }
    
    statement.reset(connection_->prepareStatement(
        "SELECT id, cluster_id, name, type, version, INET_NTOA(address) as address_string, tcp_port, udp_port, status, CONVERT(TIMESTAMP(last_pulse), CHAR) as last_pulse_timestamp "
        "FROM process WHERE id = LAST_INSERT_ID()"));
    std::unique_ptr<sql::ResultSet> result(statement->executeQuery());
    
    if (!result->next()) {
        return nullptr;
    }
    
    auto proc = make_shared<Process>(
        result->getUInt("id"),
        result->getUInt("cluster_id"),
        result->getString("name"),
        result->getString("type"),
        result->getString("version"),
        result->getString("address_string"),
        result->getUInt("tcp_port"),
        result->getUInt("udp_port"),
        static_cast<Process::StatusType>(result->getInt("status")),
        result->getString("last_pulse_timestamp")
        );

    return proc;
}

std::string Datastore::getClusterTimestamp(std::shared_ptr<Cluster> cluster) const {
    auto process = findProcessById(cluster->primary_id());

    if (!process) {
        return "";
    }

    return process->last_pulse();
}

void Datastore::saveProcess(std::shared_ptr<Process> process) const {
    std::unique_ptr<sql::PreparedStatement> statement(connection_->prepareStatement(
        "UPDATE process SET address = ?, tcp_port = ?, udp_port = ?, status = ?, last_pulse = ? WHERE id = ?"));
    
    statement->setString(1, process->address());
    statement->setUInt(2, process->tcp_port());
    statement->setUInt(3, process->udp_port());
    statement->setInt(4, process->status());
    statement->setString(5, prepareTimestampForStorage_(process->last_pulse()));
    statement->setUInt(6, process->id());
    statement->executeUpdate();
}

std::shared_ptr<Cluster> Datastore::findClusterById(uint32_t id) const {
     std::unique_ptr<sql::PreparedStatement> statement(connection_->prepareStatement(
        "SELECT * FROM cluster "
        "WHERE id = ? LIMIT 1"));

    statement->setUInt(1, id);

    std::unique_ptr<sql::ResultSet> result(statement->executeQuery());

    // if the statement fails to process return a nullptr
    if (!result->next()) {
        return nullptr;
    }

    auto cluster = make_shared<Cluster>(
        result->getUInt("id"),
        result->getUInt("primary_id"),
        result->getString("name"),
        static_cast<Cluster::StatusType>(result->getInt("status")),
        result->getString("created_at"),
        result->getString("updated_at"));

    return cluster;
}

std::shared_ptr<Process> Datastore::findProcessById(uint32_t id) const {
     std::unique_ptr<sql::PreparedStatement> statement(connection_->prepareStatement(
        "SELECT * FROM process "
        "WHERE id = ? LIMIT 1"));

    statement->setUInt(1, id);

    std::unique_ptr<sql::ResultSet> result(statement->executeQuery());

    // if the statement fails to process return a nullptr
    if (!result->next()) {
        return nullptr;
    }
        
    auto proc = make_shared<Process>(
        result->getUInt("id"),
        result->getUInt("cluster_id"),
        result->getString("name"),
        result->getString("type"),
        result->getString("version"),
        result->getString("address_string"),
        result->getUInt("tcp_port"),
        result->getUInt("udp_port"),
        static_cast<Process::StatusType>(result->getInt("status")),
        result->getString("last_pulse_timestamp")
        );

    return proc;
}

bool Datastore::deleteProcessById(uint32_t id) const {
    std::unique_ptr<sql::PreparedStatement> statement(connection_->prepareStatement(
        "DELETE FROM process WHERE id = ? "
    ));

    statement->setUInt(1, id);

    return statement->executeUpdate() > 0;
}

std::string Datastore::prepareTimestampForStorage_(const std::string& timestamp) const {    
    std::stringstream ss;
    
    boost::posix_time::time_facet facet("%Y%m%d%H%M%S%F");    
    ss.imbue(std::locale(ss.getloc(), &facet));
    
    ss << boost::posix_time::time_from_string(timestamp);
    
    return ss.str();
}
