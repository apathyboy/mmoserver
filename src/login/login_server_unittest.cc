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

using namespace anh::event_dispatcher;
using namespace std;
using namespace testing;

using login::LoginServer;

class LoginServerTest : public Test {
protected:
    shared_ptr<MockEventDispatcher> buildMockEventDispatcher() {
        shared_ptr<MockEventDispatcher> mock_dispatcher(new NiceMock<MockEventDispatcher>());
        
        EXPECT_CALL(*mock_dispatcher, subscribe(_, _))
            .WillRepeatedly(Return(true));

        return mock_dispatcher;
    }
};

/// This test verifies that the login server listens for LoginClientId events.
TEST_F(LoginServerTest, ListensForLoginClientIdEvents) {
    shared_ptr<MockEventDispatcher> mock_dispatcher = buildMockEventDispatcher();
    EXPECT_CALL(*mock_dispatcher, subscribe(EventType("LoginClientId"), _))
        .WillOnce(Return(true));
    
    LoginServer login_server(mock_dispatcher);	
}

/// This test verifies that the login server lists for DeleteCharacterMessage events.
TEST_F(LoginServerTest, ListensForDeleteCharacterMessageEvents) {
    shared_ptr<MockEventDispatcher> mock_dispatcher = buildMockEventDispatcher();
    EXPECT_CALL(*mock_dispatcher, subscribe(EventType("DeleteCharacterMessage"), _))
        .WillOnce(Return(true));

    LoginServer login_server(mock_dispatcher);
}
