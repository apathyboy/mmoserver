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

#include <memory>
#include "anh/event_dispatcher/mock_event_dispatcher.h"

#include "login/login_server.h"

using anh::event_dispatcher::EventDispatcherInterface;
using anh::event_dispatcher::EventType;
using anh::event_dispatcher::MockEventDispatcher;
using login::LoginServer;
using std::shared_ptr;

class LoginServerTest : public testing::Test {};

/// This test verifies that the login server listens for LoginClientId messages.
TEST_F(LoginServerTest, ListensForLoginClientIdMessages) {
    shared_ptr<MockEventDispatcher> mock_dispatcher(new MockEventDispatcher);
    EXPECT_CALL(*mock_dispatcher, subscribe(EventType("LoginClientIdEvent"), testing::_));
    
    LoginServer login_server(mock_dispatcher);	
}
