/*
    Vsqlite
    Copyright (c) 2025 V0idPointer
    Licensed under the MIT License
*/

#ifndef _VSQLITE_STATEMENT_H_
#define _VSQLITE_STATEMENT_H_

#include <Vsqlite/SQLite.h>
#include <Vsqlite/SqliteException.h>

#include <string_view>
#include <cstdint>

namespace Vsqlite {

    class Database;

    /**
     * Represents an SQLite statement.
     */
    class Statement {

    private:
        sqlite3_stmt* m_pStatement;

    public:

        /**
         * Constructs a new Statement object.
         * 
         * @param database An SQLite database.
         * @param sql An SQL statement.
         * @param flags Prepare flags. The full list of flags can be found at: https://www.sqlite.org/c3ref/c_prepare_persistent.html
         * @exception std::invalid_argument - The 'sql' parameter is an empty string.
         * @exception SqliteException
         */
        Statement(const Database& database, const std::string_view sql, const std::int32_t flags);

        /**
         * Constructs a new Statement object.
         * 
         * @param pDatabase An SQLite database.
         * @param sql An SQL statement.
         * @param flags Prepare flags. The full list of flags can be found at: https://www.sqlite.org/c3ref/c_prepare_persistent.html
         * @exception std::invalid_argument - The 'sql' parameter is an empty string.
         * @exception SqliteException
         */
        Statement(sqlite3* const pDatabase, const std::string_view sql, const std::int32_t flags);

        Statement(const Statement&) = delete;
        Statement(Statement&& statement) noexcept;
        virtual ~Statement(void);

        Statement& operator= (const Statement&) = delete;
        Statement& operator= (Statement&& statement) noexcept;

        /**
         * Returns the SQLite statement handle.
         * 
         * @returns sqlite3_stmt*
         */
        sqlite3_stmt* GetStatementHandle(void) const;

        /**
         * Resets the prepared statement.
         * 
         * @exception SqliteException
         */
        void Reset(void);

        /**
         * Evaluates the statement.
         * 
         * @exception SqliteException
         */
        void Step(void);

        /**
         * Executes the statement.
         * 
         * @exception SqliteException
         */
        void Execute(void);

    };

    inline Statement::Statement(sqlite3* pDatabase, const std::string_view sql, const std::int32_t flags) {

        if (sql.empty()) 
            throw std::invalid_argument("'sql': Empty string.");

        const std::int32_t res = sqlite3_prepare_v3(
            pDatabase,
            sql.data(),
            static_cast<std::int32_t>(sql.length()),
            flags,
            &this->m_pStatement,
            nullptr
        );

        if (res != SQLITE_OK) {
            const SqliteException ex = { pDatabase };
            sqlite3_finalize(this->m_pStatement);
            this->m_pStatement = nullptr;
            throw ex;
        }

    }

    inline Statement::Statement(Statement&& statement) noexcept {
        this->m_pStatement = nullptr;
        this->operator= (std::move(statement));
    }

    inline Statement::~Statement() { 

        if (this->m_pStatement) {
            sqlite3_finalize(this->m_pStatement);
            this->m_pStatement = nullptr;
        }

    }

    inline Statement& Statement::operator= (Statement&& statement) noexcept {

        if (this != &statement) {
            
            if (this->m_pStatement) {
                sqlite3_finalize(this->m_pStatement);
                this->m_pStatement = nullptr;
            }

            this->m_pStatement = statement.m_pStatement;
            statement.m_pStatement = nullptr;
            
        }

        return static_cast<Statement&>(*this);
    }

    inline sqlite3_stmt* Statement::GetStatementHandle() const {
        return this->m_pStatement;
    }

    inline void Statement::Reset() { 
        const std::int32_t res = sqlite3_reset(this->m_pStatement);
        if (res != SQLITE_OK) throw SqliteException(this->m_pStatement);
    }

    inline void Statement::Step() { 
        const std::int32_t res = sqlite3_step(this->m_pStatement);
        if ((res != SQLITE_ROW) && (res != SQLITE_DONE)) throw SqliteException(this->m_pStatement); 
    }

    inline void Statement::Execute() { 
        this->Reset();
        this->Step();
    }

}

#include <Vsqlite/Database.h>

namespace Vsqlite {

    inline Statement::Statement(const Database& database, const std::string_view sql, const std::int32_t flags)
        : Statement(database.GetDatabaseHandle(), sql, flags) { }

}

#endif // _VSQLITE_STATEMENT_H_