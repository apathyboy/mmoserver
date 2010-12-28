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

#include "anh/database/transaction.h"

#include <cstdarg>
#include <cstdint>

#include "anh/database/database.h"
#include "anh/database/database_callback.h"
#include "anh/database/database_implementation.h"
#include "anh/database/database_implementation_mysql.h"

Transaction::Transaction(Database* database, DatabaseCallback callback)
    : database_(database)
    , callback_(callback)
{
    queries_.flush();
    queries_ << "CALL "<< database_->galaxy() << ".sp_MultiTransaction(\"";
}


Transaction::~Transaction() {
    queries_.flush();
}


void Transaction::addQuery(const char* query, ...) {
    va_list	args;
    va_start(args,query);
    char localSql[2048], escapedSql[2500];
    int32_t	len = vsnprintf(localSql, sizeof(localSql), query, args);

    // need to escape
    database_->escapeString(escapedSql, localSql, len);

    queries_ << escapedSql << "$$";

    va_end(args);
}


void Transaction::execute() {
    queries_ << "\")";

    database_->executeAsyncProcedure(queries_.str(), callback_);
    database_->destroyTransaction(this);
}
