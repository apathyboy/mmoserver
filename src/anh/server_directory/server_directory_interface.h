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

#ifndef ANH_SERVER_DIRECTORY_SERVER_DIRECTORY_INTERFACE_H_
#define ANH_SERVER_DIRECTORY_SERVER_DIRECTORY_INTERFACE_H_

#include <cstdint>

#include <map>
#include <memory>
#include <string>

namespace anh {
namespace server_directory {

typedef std::map<uint32_t, std::shared_ptr<Cluster>> ClusterMap;
typedef std::map<uint32_t, std::shared_ptr<Process>> ProcessMap;

/// Simple interface
class ServerDirectoryInterface {
public:
    virtual ~ServerDirectoryInterface() {}

    virtual bool registerProcess(const std::string& name, const std::string& process_type, const std::string& version, const std::string& address, uint16_t tcp_port, uint16_t udp_port, uint16_t ping)=0;
    virtual bool removeProcess(std::shared_ptr<Process>& process)=0;
    virtual void updateProcessStatus(std::shared_ptr<Process>& process, int32_t new_status)=0;
    
    virtual bool makePrimaryProcess(std::shared_ptr<Process> process)=0;

    virtual void pulse()=0;
};

}  // namespace server_directory
}  // namespace anh

#endif  // ANH_SERVER_DIRECTORY_SERVER_DIRECTORY_INTERFACE_H_
