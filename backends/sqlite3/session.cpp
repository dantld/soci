//
// Copyright (C) 2004-2006 Maciej Sobczak, Stephen Hutton, David Courtney
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//


#include "soci-sqlite3.h"

#include <sstream>

#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

using namespace soci;
using namespace soci::details;
using namespace sqlite_api;

namespace // anonymous
{

// helper function for hardcoded queries
void hardExec(sqlite_api::sqlite3 *conn, char const *query, char const *errMsg)
{
    char *zErrMsg = 0;
    int res = sqlite3_exec(conn, query, 0, 0, &zErrMsg);
    if (res != SQLITE_OK)
    {
        std::ostringstream ss;
        ss << errMsg << " "
           << zErrMsg;

        sqlite3_free(zErrMsg);
        
        throw soci_error(ss.str());
    }
}

} // namespace anonymous


sqlite3_session_backend::sqlite3_session_backend(
    std::string const & connectString)
{
    int res;
    res = sqlite3_open(connectString.c_str(), &conn_);
    if (res != SQLITE_OK)
    {
        const char *zErrMsg = sqlite3_errmsg(conn_);

        std::ostringstream ss;
        ss << "Cannot establish connection to the database. "
           << zErrMsg;
        
        throw soci_error(ss.str());
    }
}

sqlite3_session_backend::~sqlite3_session_backend()
{
    cleanUp();
}

void sqlite3_session_backend::begin()
{
    hardExec(conn_, "BEGIN", "Cannot begin transaction.");    
}

void sqlite3_session_backend::commit()
{
    hardExec(conn_, "COMMIT", "Cannot commit transaction.");
}

void sqlite3_session_backend::rollback()
{
    hardExec(conn_, "ROLLBACK", "Cannot rollback transaction.");
}

void sqlite3_session_backend::cleanUp()
{
    sqlite3_close(conn_);
}

sqlite3_statement_backend * sqlite3_session_backend::make_statement_backend()
{
    return new sqlite3_statement_backend(*this);
}

sqlite3_rowid_backend * sqlite3_session_backend::make_rowid_backend()
{
    return new sqlite3_rowid_backend(*this);
}

sqlite3_blob_backend * sqlite3_session_backend::make_blob_backend()
{
    return new sqlite3_blob_backend(*this);
}