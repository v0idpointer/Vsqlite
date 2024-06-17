/*
    Vsqlite
    Copyright (C) V0idPointer
    Licensed under the MIT License
*/

#ifndef _AD_DATABASE_H_
#define _AD_DATABASE_H_

#include <Vsqlite/TransactionType.h>
#include <Vsqlite/SQLiteException.h>

#include <string_view>
#include <cstdint>

namespace Vsqlite {

    class Statement;

    class Database {

    private:
        sqlite3* m_pDatabase;

    public:
        Database(const std::string_view filename, const std::int32_t flags = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE));
        Database(const Database&) = delete;
        Database(Database&& database) noexcept;
        virtual ~Database(void);

        Database& operator= (const Database&) = delete;
        Database& operator= (Database&& database) noexcept;
        bool operator== (const Database& database) const;

        sqlite3* GetDatabaseHandle(void) const;

        [[nodiscard]] Statement PrepareStatement(const std::string_view sql, const std::int32_t flags = 0);
        Statement Execute(const std::string_view sql);

        void BeginTransaction(const TransactionType transactionType = TransactionType::DEFERRED);
        void Rollback(void);
        void Commit(void);

    };

    inline Database::Database(const std::string_view filename, const std::int32_t flags) {

        const std::int32_t res = sqlite3_open_v2(filename.data(), &this->m_pDatabase, flags, nullptr);
        if (res != SQLITE_OK) {
            const SQLiteException ex = { this->m_pDatabase };
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

        if (this->m_pDatabase) {
            sqlite3_close_v2(this->m_pDatabase);
            this->m_pDatabase = nullptr;
        }

        this->m_pDatabase = database.m_pDatabase;
        database.m_pDatabase = nullptr;

        return static_cast<Database&>(*this);
    }

    inline bool Database::operator== (const Database& database) const {
        return (this->m_pDatabase == database.m_pDatabase);
    }

    inline sqlite3* Database::GetDatabaseHandle() const {
        return this->m_pDatabase;
    }

}

#include <Vsqlite/Statement.h>

namespace Vsqlite {

    inline Statement Database::PrepareStatement(const std::string_view sql, const std::int32_t flags) {
        return Statement(*this, sql, flags);
    }

    inline Statement Database::Execute(const std::string_view sql) {
        Statement s = { *this, sql, 0 };
        s.Execute();
        return s;
    }

    inline void Database::BeginTransaction(const TransactionType transactionType) {

        switch (transactionType) {

            case TransactionType::DEFERRED:
                this->Execute("BEGIN DEFERRED TRANSACTION;");
                break;

            case TransactionType::IMMEDIATE:
                this->Execute("BEGIN IMMEDIATE TRANSACTION;");
                break;

            case TransactionType::EXCLUSIVE:
                this->Execute("BEGIN EXCLUSIVE TRANSACTION;");
                break;

            default:
                throw std::invalid_argument("Invalid transaction type.");
                break;

        }

    }

    inline void Database::Rollback() {
        this->Execute("ROLLBACK TRANSACTION;");
    }

    inline void Database::Commit() {
        this->Execute("COMMIT TRANSACTION;");
    }

}

#endif // _AD_DATABASE_H_