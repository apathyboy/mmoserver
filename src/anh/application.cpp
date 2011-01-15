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

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
---------------------------------------------------------------------------------------
*/

#include "anh/application.h"

#include <cassert>
#include <iostream>
#include <fstream>

using namespace std;
using namespace anh;
using namespace event_dispatcher;
using namespace database;

BaseApplication::BaseApplication(IEventDispatcher& event_dispatcher, DatabaseManager& db_manager)
    : event_dispatcher_(event_dispatcher)
    , db_manager_(db_manager)
    , configuration_options_description_("Configuration Options")
    , argc_(0)
    , argv_(nullptr)
    , started_(false)
{
    registerEventTypes_();
    addDefaultOptions_();
    addDataSourcesFromOptions_();
}

BaseApplication::BaseApplication(int argc, const char** argv,IEventDispatcher& event_dispatcher, DatabaseManager& db_manager)
    : event_dispatcher_(event_dispatcher)
    , db_manager_(db_manager)
    , configuration_options_description_("Configuration Options")
    , argc_(argc)
    , argv_(argv)
    , started_(false)
{
    registerEventTypes_();
    addDefaultOptions_();
    addDataSourcesFromOptions_();
}

BaseApplication::~BaseApplication() {}

void BaseApplication::startup() {
    // startup logic here
    
    auto startup_event = make_shared<SimpleEvent>("Startup");
    event_dispatcher_.trigger(startup_event);
}

void BaseApplication::process() {
    if (!hasStarted()) {
        assert(false && "Must call startup before process");
        return;
    }
    auto process_event = make_shared<SimpleEvent>("Process");
    event_dispatcher_.trigger(process_event);
}

void BaseApplication::shutdown() {
    // clean up code here before the server shuts down
    auto shutdown_event = make_shared<SimpleEvent>("Shutdown");
    event_dispatcher_.trigger(shutdown_event);
}
void BaseApplication::registerEventTypes_() {
    // register default event types
    event_dispatcher_.registerEventType("Startup");
    event_dispatcher_.registerEventType("Process");
    event_dispatcher_.registerEventType("Shutdown");
}

void BaseApplication::addDefaultOptions_() {
    configuration_options_description_.add_options()
        ("help", "Displays this help dialog.")

        ("cluster.name", boost::program_options::value<string>(), "Name of the cluster this application is participating in")
        ("cluster.datastore.host", boost::program_options::value<string>()->default_value("tcp://localhost:3306"), "Host to connect to the cluster datastore at: e.x. tcp://localhost:3306")
        ("cluster.datastore.username", boost::program_options::value<string>(), "Username to connect to the cluster datastore with")
        ("cluster.datastore.password", boost::program_options::value<string>(), "Password to connect to the cluster datastore with")
        ("cluster.datastore.schema", boost::program_options::value<string>(), "Schema name that contains the cluster data")

        ("galaxy.datastore.host", boost::program_options::value<string>()->default_value("tcp://localhost:3306"), "Host to connect to the galaxy datastore at: e.x. tcp://localhost:3306")
        ("galaxy.datastore.username", boost::program_options::value<string>(), "Username to connect to the galaxy datastore with")
        ("galaxy.datastore.password", boost::program_options::value<string>(), "Password to connect to the galaxy datastore with")
        ("galaxy.datastore.schema", boost::program_options::value<string>(), "Schema name that contains the galaxy data")

        ("optimization.min_threads", boost::program_options::value<uint16_t>()->default_value(0), "Minimum number of threads to use for concurrency operations")
        ("optimization.max_threads", boost::program_options::value<uint16_t>()->default_value(0), "Maximum number of threads to use for concurrency operations")

        ("network.bind_address", boost::program_options::value<string>()->default_value("localhost"), "Address to listen for incoming messages on")
        ("network.bind_port", boost::program_options::value<uint16_t>(), "Port to listen for incoming messages on")
    ;
}

void BaseApplication::loadOptions_(uint32_t argc, char* argv[]) {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, configuration_options_description_), configuration_variables_map_);
    boost::program_options::notify(configuration_variables_map_);

    // The help argument has been flagged, display the
    // server options and throw a runtime_error exception
    // to stop server startup.
    if(configuration_variables_map_.count("help")) {
        cout << configuration_options_description_ << endl;
        throw runtime_error("Help option flagged.");
    }
}

void BaseApplication::loadOptions_(list<string> config_files) {
    // Iterate through the configuration files
    // that are to be loaded. If a configuration file
    // is missing, throw a runtime_error.
    for_each(config_files.begin(), config_files.end(), [=] (const string& filename) {
        ifstream config_file(filename);
        if(!config_file)
            throw runtime_error("Could not open configuration file.");
        else
            boost::program_options::store(boost::program_options::parse_config_file(config_file, configuration_options_description_, true), configuration_variables_map_);
    });

    boost::program_options::notify(configuration_variables_map_);

    // The help argument has been flagged, display the
    // server options and throw a runtime_error exception
    // to stop server startup.
    if(configuration_variables_map_.count("help")) {
        cout << configuration_options_description_ << endl;
        throw runtime_error("Help option flagged.");
    }
}

void BaseApplication::loadOptions_(uint32_t argc, char* argv[], list<string> config_files)
{
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, configuration_options_description_), configuration_variables_map_);

    // Iterate through the configuration files
    // that are to be loaded. If a configuration file
    // is missing, throw a runtime_error.
    for_each(config_files.begin(), config_files.end(), [=] (const string& filename) {
        ifstream config_file(filename);
        if(!config_file)
            throw runtime_error("Could not open configuration file.");
        else
            boost::program_options::store(boost::program_options::parse_config_file(config_file, configuration_options_description_, true), configuration_variables_map_);
    });

    boost::program_options::notify(configuration_variables_map_);

    // The help argument has been flagged, display the
    // server options and throw a runtime_error exception
    // to stop server startup.
    if(configuration_variables_map_.count("help")) {
        cout << configuration_options_description_ << endl;
        throw runtime_error("Help option flagged.");
    }
}
bool BaseApplication::addDataSourcesFromOptions_()
{
    // check to see if options have been loaded properly
    if (configuration_variables_map_.size() < 1)
        return false;

    try {
        // register the cluster first
        db_manager_.registerStorageType(
        configuration_variables_map_["cluster.name"].as<StorageType>(),
        configuration_variables_map_["cluster.datastore.schema"].as<string>(),
        configuration_variables_map_["cluster.datastore.host"].as<string>(),
        configuration_variables_map_["cluster.datastore.username"].as<string>(),
        configuration_variables_map_["cluster.datastore.password"].as<string>());

        // register the galaxy
        db_manager_.registerStorageType(
        configuration_variables_map_["galaxy.datastore.schema"].as<StorageType>(),
        configuration_variables_map_["galaxy.datastore.schema"].as<string>(),
        configuration_variables_map_["galaxy.datastore.host"].as<string>(),
        configuration_variables_map_["galaxy.datastore.username"].as<string>(),
        configuration_variables_map_["galaxy.datastore.password"].as<string>());

    } catch(...) {
        cerr << "No exceptions should be thrown during a registration with valid information";
        return false;
    }
    return true;
}