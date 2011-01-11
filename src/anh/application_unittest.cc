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

#include "anh/application.h"

using namespace anh;
using namespace testing;

namespace {

class MockApplication : public BaseApplication {
public:
    MOCK_METHOD0(onStartup, void());
    MOCK_METHOD0(onProcess, void());
    MOCK_METHOD0(onShutdown, void());
    MOCK_CONST_METHOD0(hasStarted, bool());
};

/// Verifies that process cannot be called before startup.
TEST(ApplicationTest, DiesWhenProcessCalledBeforeStartup) {
    MockApplication app;

    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    Expectation expect_on_startup = EXPECT_CALL(app, onStartup());
    EXPECT_CALL(app, hasStarted())
        .After(expect_on_startup)
        .WillRepeatedly(Return(true));
        
    // start testing the app here
    ASSERT_DEATH(app.process(), "Must call startup before process");

    app.startup();

    ASSERT_TRUE(app.hasStarted());

    // now process can be called without dying
    app.process();
}

/// Verifies that process cannot be called after shutdown.
TEST(ApplicationTest, DiesWhenProcessCalledAfterShutdown) {
    MockApplication app;
    
    EXPECT_CALL(app, hasStarted())
        .WillRepeatedly(Return(false));

    Expectation expect_on_startup = EXPECT_CALL(app, onStartup());
    EXPECT_CALL(app, hasStarted())
        .After(expect_on_startup)
        .WillRepeatedly(Return(true));

    Expectation expect_on_shutdown = EXPECT_CALL(app, onShutdown());
    EXPECT_CALL(app, hasStarted())
        .After(expect_on_shutdown)
        .WillRepeatedly(Return(false));
    
    // start testing the app here
    app.startup();
    ASSERT_TRUE(app.hasStarted());
    app.process();

    app.shutdown();
    ASSERT_DEATH(app.process(), "Must call startup before process");
}

}  // namespace