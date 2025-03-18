/*
    Vsqlite
    Copyright (c) 2025 V0idPointer
    Licensed under the MIT License
*/

#ifndef _VSQLITE_SQLITEEXCEPTION_H_
#define _VSQLITE_SQLITEEXCEPTION_H_

#include <Vsqlite/SQLite.h>

#include <string>
#include <cstdint>
#include <exception>
#include <stdexcept>

namespace Vsqlite {

    /**
     * Represents an SQLite error.
     */
    class SqliteException : public std::runtime_error {

    private:
        std::int32_t m_errCode;
        std::int32_t m_errCodeEx;

    public:

        /**
         * Constructs a new SqliteException object.
         * 
         * @param msg An error message.
         * @param errCode An SQLite error code.
         * @param errCodeEx An SQLite extended error code.
         */
        SqliteException(const std::string& msg, const std::int32_t errCode, const std::int32_t errCodeEx);

        /**
         * Constructs a new SqliteException object.
         * 
         * @param pDatabase An SQLite database.
         */
        SqliteException(sqlite3* const pDatabase);

        /**
         * Constructs a new SqliteException object.
         * 
         * @param pStatement An SQLite statement.
         */
        SqliteException(sqlite3_stmt* const pStatement);

        SqliteException(const SqliteException& other) noexcept;
        SqliteException(SqliteException&& other) noexcept;
        virtual ~SqliteException(void);

        SqliteException& operator= (const SqliteException& other) noexcept;
        SqliteException& operator= (SqliteException&& other) noexcept;

        /**
         * Returns the SQLite error code.
         * 
         * The full list of error codes can be found at: https://www.sqlite.org/rescode.html#primary_result_code_list
         * 
         * @returns An integer.
         */
        std::int32_t GetErrorCode(void) const;

        /**
         * Returns the SQLite extended error code.
         * 
         * The full list of extended error codes can be found at: https://www.sqlite.org/rescode.html#extended_result_code_list
         * 
         * @returns An integer.
         */
        std::int32_t GetExtendedErrorCode(void) const;

    };

    inline SqliteException::SqliteException(const std::string& msg, const std::int32_t errCode, const std::int32_t errCodeEx)
        : std::runtime_error(msg), m_errCode(errCode), m_errCodeEx(errCodeEx) { }

    inline SqliteException::SqliteException(sqlite3* const pDatabase)
        : SqliteException(sqlite3_errmsg(pDatabase), sqlite3_errcode(pDatabase), sqlite3_extended_errcode(pDatabase)) { }   
        
    inline SqliteException::SqliteException(sqlite3_stmt* const pStatement)
        : SqliteException(sqlite3_db_handle(pStatement)) { }

    inline SqliteException::SqliteException(const SqliteException& other) noexcept
        : std::runtime_error(other), m_errCode(other.m_errCode), m_errCodeEx(other.m_errCodeEx) { }

    inline SqliteException::SqliteException(SqliteException&& other) noexcept
        : std::runtime_error(std::move(other)), m_errCode(other.m_errCode), m_errCodeEx(other.m_errCodeEx) { }

    inline SqliteException::~SqliteException() { }

    inline SqliteException& SqliteException::operator= (const SqliteException& other) noexcept {

        if (this != &other) {
            std::runtime_error::operator= (other);
            this->m_errCode = other.m_errCode;
            this->m_errCodeEx = other.m_errCodeEx;
        }

        return static_cast<SqliteException&>(*this);
    }

    inline SqliteException& SqliteException::operator= (SqliteException&& other) noexcept {

        if (this != &other) {
            std::runtime_error::operator= (std::move(other));
            this->m_errCode = other.m_errCode;
            this->m_errCodeEx = other.m_errCodeEx;
        }

        return static_cast<SqliteException&>(*this);
    }

    inline std::int32_t SqliteException::GetErrorCode() const {
        return this->m_errCode;
    }

    inline std::int32_t SqliteException::GetExtendedErrorCode() const {
        return this->m_errCodeEx;
    }

}

#endif // _VSQLITE_SQLITEEXCEPTION_H_