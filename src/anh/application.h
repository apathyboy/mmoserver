/*
---------------------------------------------------------------------------------------
This source file is part of SWG:ANH (Star Wars Galaxies - A New Hope - Server Emulator)

For more information, visit http://www.swganh.com

Copyright (c) 2006 - 2010 The SWG:ANH Team
---------------------------------------------------------------------------------------
Use of this source code is governed by the GPL v3 license that can be found
in the COPYING file or at http://www.gnu.org/licenses/gpl-3.0.html

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,a
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
---------------------------------------------------------------------------------------
*/

#ifndef ANH_APPLICATION_H_
#define ANH_APPLICATION_H_

#include <cstdint>
#include <list>
#include <boost/program_options.hpp>
#include <anh/event_dispatcher/event_dispatcher.h>
#include <anh/database/database_manager.h>

/*! \brief Common is a catch-all library containing primarily base classes and
 * classes used for maintaining application lifetimes.
 */
namespace anh {

/**
 * \brief A simple interface for all servers in the cluster.
 */
class ApplicationInterface {
public:
    virtual bool hasStarted() const = 0;
    virtual void startup() = 0;
    virtual void process() = 0;
    virtual void shutdown() = 0;
};

/**
 * \brief Holds common functionality used between all servers in the cluster.
 */
class BaseApplication : public ApplicationInterface {
public:
    /**
     * Initializes general configuration options used between all servers in the cluster.
     */
    BaseApplication(event_dispatcher::IEventDispatcher& event_dispatcher, database::DatabaseManager& db_manager);
    BaseApplication(int argc, const char* argv[], event_dispatcher::IEventDispatcher& event_dispatcher, database::DatabaseManager& db_manager);
    
    /**
     * Default Deconstructor.
     */
    ~BaseApplication();

    void startup();
    void process();
    void shutdown();

    boost::program_options::variables_map getConfigVarMap() { return configuration_variables_map_; }
protected:
    /** 
    *   Registers to the event dispatcher event types Startup, Process and Shutdown
    *   If any specific logic is required the app should attach a listener to these types
    */
    void registerEventTypes_();

    bool addDataSourcesFromOptions_();

    void addDefaultOptions_();

    /**
     * Loads configuration options using the configuration options description from
     * the command-line only.
     *
     * \param argc The number of arugments.
     * \param argv The arguments pulled from the command-line.
     */
    void loadOptions_(uint32_t argc, char* argv[]);

    /**
     * Loads configuration options using the configuration options description from
     * a list of configuration files only.
     *
     * \param config_files The files to load the options from.
     */
    void loadOptions_(std::list<std::string> config_files);

    /**
     * Loads configuration options using the configuration options description from
     * the command-line and configuration files.
     *
     * \param argc The number of arugments.
     * \param argv The arguments pulled from the command-line.
     * \param config_files The files to load the options from.
     */
    void loadOptions_(uint32_t argc, char* argv[], std::list<std::string> config_files);

    database::DatabaseManager &db_manager_;
    event_dispatcher::IEventDispatcher &event_dispatcher_;

    boost::program_options::options_description configuration_options_description_;
    boost::program_options::variables_map configuration_variables_map_;
    int argc_;
    const char** argv_;
    bool started_;
};

}  // namespace anh

#endif  // ANH_SERVER_H_