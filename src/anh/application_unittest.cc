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
    MockApplication(IEventDispatcher &ev, DatabaseManager &db_manager) 
        : BaseApplication(ev, db_manager){};
    MOCK_CONST_METHOD0(hasStarted, bool());
};
class ApplicationTest : public testing::Test
{
protected:
    virtual void SetUp();
};

/// tests that the app won't do any processing if the startup process hasn't run
TEST_F(ApplicationTest, dieOnProcessIfNotStarted) {
    MockDriver mock_driver;
    DatabaseManager manager(&mock_driver);
    NiceMock<MockEventDispatcher> mock_dispatcher;
    MockApplication app(mock_dispatcher, manager);
        
    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    ASSERT_DEATH(app.process(), "Must call startup before process");
}
/// verify the Startup event is triggered on startup
TEST_F(ApplicationTest, startupEventTriggered) {
    MockDriver mock_driver;
    DatabaseManager manager(&mock_driver);
    NiceMock<MockEventDispatcher> mock_dispatcher;
    MockApplication app(mock_dispatcher, manager);

    EXPECT_CALL(mock_dispatcher, trigger(_));
    app.startup();

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(true));
}
/// verify the Startup event is not triggered if startup isn't called
TEST_F(ApplicationTest, startupEventNotTriggered) {
    MockDriver mock_driver;
    DatabaseManager manager(&mock_driver);
    NiceMock<MockEventDispatcher> mock_dispatcher;
    MockApplication app(mock_dispatcher, manager);

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    EXPECT_CALL(mock_dispatcher, trigger(_))
        .Times(0);

    ASSERT_DEATH(app.process(), "Must call startup before process");

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));
}

/// verify the Process event triggered 
TEST_F(ApplicationTest, processEventTriggered) {
    MockDriver mock_driver;
    DatabaseManager manager(&mock_driver);
    NiceMock<MockEventDispatcher> mock_dispatcher;
    MockApplication app(mock_dispatcher, manager);

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    // once for Startup and once for Process
    EXPECT_CALL(mock_dispatcher, trigger(_))
        .Times(2);

    app.startup();

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(true));

    app.process();
}

/// Verifies that process cannot be called after shutdown.
/// also verifies no events occur after the shutdown
TEST_F(ApplicationTest, DiesWhenProcessCalledAfterShutdown) {
    MockDriver mock_driver;
    DatabaseManager manager(&mock_driver);
    NiceMock<MockEventDispatcher> mock_dispatcher;
    MockApplication app(mock_dispatcher, manager);

    Expectation expect_on_startup = EXPECT_CALL(mock_dispatcher, trigger(_));
    EXPECT_CALL(app, hasStarted())
        .After(expect_on_startup)
        .WillRepeatedly(Return(true));
    // start testing the app here
    app.startup();
    
    EXPECT_CALL(mock_dispatcher, trigger(_));

    ASSERT_TRUE(app.hasStarted());
    app.process();

    Expectation expect_on_shutdown = EXPECT_CALL(mock_dispatcher, trigger(_));
    EXPECT_CALL(app, hasStarted())
        .After(expect_on_shutdown)
        .WillRepeatedly(Return(false));

    app.shutdown();

    EXPECT_CALL(mock_dispatcher, trigger(_))
        .Times(0);

    ASSERT_DEATH(app.process(), "Must call startup before process");
}
/// checks that based on configuration data we configure two data sources
TEST_F(ApplicationTest, doesConfigureDataSource)
{
 /*   MockDriver mock_driver;
    DatabaseManager manager(&mock_driver);
    NiceMock<MockEventDispatcher> mock_dispatcher;
    MockApplication app(mock_dispatcher, manager);

    ASSERT_TRUE(manager.hasConnection(app.getConfigVarMap()["cluster.name"].as<StorageType>()));
    ASSERT_TRUE(manager.hasConnection(app.getConfigVarMap()["galaxy.datastore.schema"].as<StorageType>()));*/
}


void ApplicationTest::SetUp()
{    
       
}

}  // namespace