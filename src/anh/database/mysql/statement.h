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

#ifndef ANH_DATABASE_MYSQL_STATEMENT_H_
#define ANH_DATABASE_MYSQL_STATEMENT_H_

#include <functional>
#include <map>
#include <memory>
#include <string>

#include <boost/any.hpp>

#include <boost/variant.hpp>
#include <boost/lexical_cast.hpp>

#include "anh/database/statement.h"

namespace anh {
namespace database {
namespace mysql {

class Statement : public IStatement {
public:

    template<typename T>
    bool bindParam(std::string name, T callback) {
        return true;
    }

    void execute() {}
};

}  // namespace mysql
}  // namespace database
}  // namespace anh

#endif  // ANH_DATABASE_MYSQL_CONNECTION_H_
