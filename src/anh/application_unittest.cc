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
#include <fstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <anh/application.h>
#include <anh/database/mock_cppconn.h>
#include <anh/database/mock_database_manager.h>
#include <anh/event_dispatcher/mock_event_dispatcher.h>

using namespace std;
using namespace anh;
using namespace testing;
using namespace event_dispatcher;
using namespace database;

namespace {

class MockApplication : public BaseApplication {
public:
    MockApplication(shared_ptr<IEventDispatcher> event_dispatcher, shared_ptr<DatabaseManagerInterface> db_manager) 
        : BaseApplication(event_dispatcher, db_manager){};
     MockApplication(list<string> config_files,shared_ptr<IEventDispatcher> event_dispatcher, shared_ptr<DatabaseManagerInterface> db_manager) 
        : BaseApplication(config_files, event_dispatcher, db_manager){};
    MOCK_CONST_METHOD0(hasStarted, bool());
};

class ApplicationTest : public testing::Test
{
public:
    shared_ptr<MockDatabaseManager> manager;
    shared_ptr<NiceMock<MockEventDispatcher>> mock_dispatcher;    
protected:
    virtual void SetUp();
    virtual void TearDown();
};

/// tests that the app won't do any processing if the startup process hasn't run
TEST_F(ApplicationTest, dieOnProcessIfNotStarted) {
    MockApplication app(mock_dispatcher, manager);

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    ASSERT_DEATH(app.process(), "Must call startup before process");
}
/// verify the Startup event is triggered on startup
TEST_F(ApplicationTest, startupEventTriggered) {
    MockApplication app(mock_dispatcher, manager);

    EXPECT_CALL(*mock_dispatcher, trigger(_));
    app.startup();

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(true));
}
/// verify the Startup event is not triggered if startup isn't called
TEST_F(ApplicationTest, startupEventNotTriggered) {
    MockApplication app(mock_dispatcher, manager);

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(*mock_dispatcher, trigger(_))
        .Times(0);

    ASSERT_DEATH(app.process(), "Must call startup before process");

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));
}

/// verify the Process event triggered 
TEST_F(ApplicationTest, processEventTriggered) {
    MockApplication app(mock_dispatcher, manager);

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    // once for Startup and once for Process
    EXPECT_CALL(*mock_dispatcher, trigger(_))
        .Times(2);

    app.startup();

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(true));

    app.process();
}

/// Verifies that process cannot be called after shutdown.
/// also verifies no events occur after the shutdown
TEST_F(ApplicationTest, DiesWhenProcessCalledAfterShutdown) {
    MockApplication app(mock_dispatcher, manager);

    Expectation expect_on_startup = EXPECT_CALL(*mock_dispatcher, trigger(_));
    EXPECT_CALL(app, hasStarted())
        .After(expect_on_startup)
        .WillRepeatedly(Return(true));
    // start testing the app here
    app.startup();
    
    EXPECT_CALL(*mock_dispatcher, trigger(_));

    ASSERT_TRUE(app.hasStarted());
    app.process();

    Expectation expect_on_shutdown = EXPECT_CALL(*mock_dispatcher, trigger(_));
    EXPECT_CALL(app, hasStarted())
        .After(expect_on_shutdown)
        .WillRepeatedly(Return(false));

    app.shutdown();

    EXPECT_CALL(*mock_dispatcher, trigger(_))
        .Times(0);

    ASSERT_DEATH(app.process(), "Must call startup before process");
}
/// BASE CONFIGURATION TESTS

/// checks based on a test cfg file we are able to load and register two storage types
TEST_F(ApplicationTest, doesLoadConfigurationFile)
{
    list<string> config_files;
	config_files.push_back("general.cfg");
    
    EXPECT_CALL(*manager, registerStorageType(_,"swganh_static","localhost","root", "swganh"));
    EXPECT_CALL(*manager, registerStorageType(_,"swganh","localhost","root", "swganh"));

    EXPECT_NO_THROW(
        MockApplication app(config_files,mock_dispatcher, manager);
    );
}
TEST_F(ApplicationTest, cantLoadConfigFile)
{
    list<string> config_files;
	config_files.push_back("notfound.cfg");

    // expectation is an exception is thrown as file not found
    EXPECT_ANY_THROW(
        MockApplication app(config_files,mock_dispatcher, manager);    
    ); 
}
/// checks based on a test cfg file we are able to load and register two storage types
TEST_F(ApplicationTest, foundConfigNoValidValues)
{
    list<string> config_files;
	config_files.push_back("invalid_data.cfg");
    
    EXPECT_ANY_THROW(
        MockApplication app(config_files,mock_dispatcher, manager);
    );
}

void ApplicationTest::SetUp()
{    
    ofstream of("general.cfg");
    of << "# Cluster Configuration " << endl;
    of << "cluster.name = naritus " << endl;
    of << "cluster.datastore.name = global "<< endl;
    of << "cluster.datastore.host = localhost "<< endl;
    of << "cluster.datastore.username = root " << endl;
    of << "cluster.datastore.password = swganh " << endl;
    of << "cluster.datastore.schema = swganh_static " << endl;
    of << "# Galaxy Configuration "<< endl;
    of << "galaxy.datastore.name = galaxy " << endl;
    of << "galaxy.datastore.host = localhost " << endl;
    of << "galaxy.datastore.username = root " << endl;
    of << "galaxy.datastore.password = swganh " << endl;
    of << "galaxy.datastore.schema = swganh " << endl;
    of.flush();
    of.close();
    of.open("invalid_data.cfg");
    of << "nothing to see here" << endl;
    of.close();

    manager = make_shared<MockDatabaseManager>();
    mock_dispatcher = make_shared<NiceMock<MockEventDispatcher>>();

}
void ApplicationTest::TearDown()
{
    remove("general.cfg");
    remove("invalid_data.cfg");
}
}  // namespace