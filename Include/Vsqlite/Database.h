/*
    Vsqlite
    Copyright (c) 2025 V0idPointer
    Licensed under the MIT License
*/

#ifndef _VSQLITE_DATABASE_H_
#define _VSQLITE_DATABASE_H_

#include <Vsqlite/SQLite.h>
#include <Vsqlite/SqliteException.h>

#include <string_view>
#include <optional>

namespace Vsqlite {

    class Statement;

    /**
     * Represents an SQLite database.
     */
    class Database {

    private:
        sqlite3* m_pDatabase;

    public:

        /**
         * Constructs a new Database object.
         * 
         * @param filename Database filename. If this parameter is set to std::nullopt,
         * an in-memory database is created.
         * @param flags Flags for file open operations.
         * The full list of flags can be found at: https://www.sqlite.org/c3ref/c_open_autoproxy.html
         * @exception std::invalid_argument - The 'filename' parameter is an empty string.
         * @exception SqliteException
         */
        Database(const std::optional<std::string_view> filename, const std::int32_t flags);

        Database(const Database&) = delete;
        Database(Database&& database) noexcept;
        virtual ~Database(void);

        Database& operator= (const Database&) = delete;
        Database& operator= (Database&& database) noexcept;

        /**
         * Returns the SQLite database handle.
         * 
         * @return sqlite3*
         */
        sqlite3* GetDatabaseHandle(void) const;

        /**
         * Creates a prepared statement.
         * 
         * @param sql An SQL statement.
         * @param flags Prepare flags. The full list of flags can be found at: https://www.sqlite.org/c3ref/c_prepare_persistent.html
         * @returns A Statement.
         * @exception std::invalid_argument - The 'sql' parameter is an empty string.
         * @exception SqliteException
         */
        [[nodiscard]] Statement PrepareStatement(const std::string_view sql, const std::int32_t flags);

        /**
         * Executes an SQL statement.
         * 
         * @param sql An SQL statement.
         * @returns A Statement.
         * @exception std::invalid_argument - The 'sql' parameter is an empty string.
         * @exception SqliteException
         */
        Statement Execute(const std::string_view sql);

    };

    inline Database::Database(const std::optional<std::string_view> filename, const std::int32_t flags) {

        if (filename.has_value() && filename->empty())
            throw std::invalid_argument("'filename': Empty string.");

        const std::int32_t res = sqlite3_open_v2(filename.value_or(":memory:").data(), &this->m_pDatabase, flags, nullptr);
        if (res != SQLITE_OK) {
            const SqliteException ex = { this->m_pDatabase };
            sqlite3_close_v2(this->m_pDatabase);
            this->m_pDatabase = nullptr;
            throw ex;
        }

    }

    inline Database::Database(Database&& database) noexcept {
        this->m_pDatabase = nullptr;
        this->operator= (std::move(database));
    }

    inline Database::~Database() {

        if (this->m_pDatabase) {
            sqlite3_close_v2(this->m_pDatabase);
            this->m_pDatabase = nullptr;
        }

    }

    inline Database& Database::operator= (Database&& database) noexcept {

        if (this != &database) {

            if (this->m_pDatabase) {
                sqlite3_close_v2(this->m_pDatabase);
                this->m_pDatabase = nullptr;
            }

            this->m_pDatabase = database.m_pDatabase;
            database.m_pDatabase = nullptr;

        }

        return static_cast<Database&>(*this);
    }

    sqlite3* Database::GetDatabaseHandle() const {
        return this->m_pDatabase;
    }

}

#include <Vsqlite/Statement.h>

namespace Vsqlite { 

    inline Statement Database::PrepareStatement(const std::string_view sql, const std::int32_t flags) {
        return Statement(static_cast<Database&>(*this), sql, flags);
    }

    inline Statement Database::Execute(const std::string_view sql) {
        Statement s = { static_cast<Database&>(*this), sql, 0 };
        s.Execute();
        return s;
    }

}

#endif // _VSQLITE_DATABASE_H_