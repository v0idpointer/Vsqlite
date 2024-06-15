/*
    Vsqlite
    Copyright (C) V0idPointer
    Licensed under the MIT License
*/

#ifndef _AD_SQLITEEXCEPTION_H_
#define _AD_SQLITEEXCEPTION_H_

#include <Vsqlite/SQLite.h>

#include <string_view>
#include <exception>
#include <stdexcept>

namespace Vsqlite {

    class SQLiteException : public std::runtime_error {
    public:
        SQLiteException(const std::string_view message);
        SQLiteException(sqlite3* const pDatabase);
        SQLiteException(sqlite3_stmt* const pStatement);
        SQLiteException(const SQLiteException& other) noexcept;
        virtual ~SQLiteException(void);
    };

    inline SQLiteException::SQLiteException(const std::string_view message) 
        : std::runtime_error(message.data()) { }

    inline SQLiteException::SQLiteException(sqlite3* const pDatabase)
        : SQLiteException(sqlite3_errmsg(pDatabase)) { }

    inline SQLiteException::SQLiteException(sqlite3_stmt* const pStatement)
        : SQLiteException(sqlite3_db_handle(pStatement)) { }

    inline SQLiteException::SQLiteException(const SQLiteException& other) noexcept
        : SQLiteException(other.what()) { }

    inline SQLiteException::~SQLiteException() { }

}

#endif // _AD_SQLITEEXCEPTION_H_