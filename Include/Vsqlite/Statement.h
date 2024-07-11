/*
    Vsqlite
    Copyright (C) V0idPointer
    Licensed under the MIT License
*/

#ifndef _AD_STATEMENT_H_
#define _AD_STATEMENT_H_

#include <Vsqlite/DataBinding.h>
#include <Vsqlite/SQLiteException.h>

#include <string_view>
#include <cstdint>

namespace Vsqlite {

    class Database;

    class Statement {

    private:
        sqlite3_stmt* m_pStatement;
        bool m_canFetch;

    public:
        Statement(const Database& database, const std::string_view sql, const std::int32_t flags);
        Statement(sqlite3* const pDatabase, const std::string_view sql, const std::int32_t flags);
        Statement(const Statement&) = delete;
        Statement(Statement&& statement) noexcept;
        virtual ~Statement(void);

        Statement& operator= (const Statement&) = delete;
        Statement& operator= (Statement&& statement) noexcept;
        bool operator== (const Statement& statement) const;

        sqlite3_stmt* GetStatementHandle(void) const;

        void Reset(void);
        void Step(void);
        void Execute(void);

        template <std::int32_t Index, typename T>
        void Bind(const T& arg);

        template <std::int32_t Index, typename T, typename... Args>
        void Bind(const T& arg, const Args&... args);

        template <typename T, typename... Args>
        void Bind(const T& arg, const Args&... args);

        void Column(void);

        template <std::int32_t Column, typename T>
        void Column(T& arg);

        template <std::int32_t Column, typename T, typename... Args>
        void Column(T& arg, Args&... args);

        template <typename T, typename... Args>
        void Column(T& arg, Args&... args);

        template <typename... Args>
        bool Fetch(Args&... args);

        template <typename... Args>
        void Execute(const Args&... args);

    };

    inline Statement::Statement(sqlite3* const pDatabase, const std::string_view sql, const std::int32_t flags) {

        const std::int32_t res = sqlite3_prepare_v3(pDatabase, sql.data(), static_cast<int>(sql.length()), flags, &this->m_pStatement, nullptr);
        if (res != SQLITE_OK) {
            const SQLiteException ex = { pDatabase };
            sqlite3_finalize(this->m_pStatement);
            this->m_pStatement = nullptr;
            this->m_canFetch = false;
            throw ex;
        }

        this->m_canFetch = false;

    }

    inline Statement::Statement(Statement&& statement) noexcept {
        this->m_pStatement = nullptr;
        this->operator= (std::move(statement));
    }

    inline Statement::~Statement() {

        if (this->m_pStatement) {
            sqlite3_finalize(this->m_pStatement);
            this->m_pStatement = nullptr;
            this->m_canFetch = false;
        }

    }

    inline Statement& Statement::operator= (Statement&& statement) noexcept {

        if (this->m_pStatement) {
            sqlite3_finalize(this->m_pStatement);
            this->m_pStatement = nullptr;
            this->m_canFetch = false;
        }

        this->m_pStatement = statement.m_pStatement;
        this->m_canFetch = statement.m_canFetch;
        statement.m_pStatement = nullptr;
        statement.m_canFetch = false;

        return static_cast<Statement&>(*this);
    }

    inline bool Statement::operator== (const Statement& statement) const {
        return (this->m_pStatement == statement.m_pStatement);
    }

    inline sqlite3_stmt* Statement::GetStatementHandle() const {
        return this->m_pStatement;
    }

    inline void Statement::Reset() {
        const std::int32_t res = sqlite3_reset(this->m_pStatement);
        if (res != SQLITE_OK) throw SQLiteException(this->m_pStatement);
    }

    inline void Statement::Step() {
        const std::int32_t res = sqlite3_step(this->m_pStatement);
        if ((res != SQLITE_ROW) && (res != SQLITE_DONE)) throw SQLiteException(this->m_pStatement);
        this->m_canFetch = (res == SQLITE_ROW);
    }

    inline void Statement::Execute() {
        this->Reset();
        this->Step();
    }

    template <std::int32_t Index, typename T>
    inline void Statement::Bind(const T& arg) {
        const std::int32_t res = DataBinding<T>::Bind(this->m_pStatement, Index, arg);
        if (res != SQLITE_OK) throw SQLiteException(this->m_pStatement);
    }

    template <std::int32_t Index, typename T, typename... Args>
    inline void Statement::Bind(const T& arg, const Args&... args) {
        const std::int32_t res = DataBinding<T>::Bind(this->m_pStatement, Index, arg);
        if (res != SQLITE_OK) throw SQLiteException(this->m_pStatement);
        this->Bind<Index + 1>(args...);
    }

    template <typename T, typename... Args>
    inline void Statement::Bind(const T& arg, const Args&... args) {
        this->Bind<1>(arg, args...);
    }

    inline void Statement::Column() { }

    template <std::int32_t Column, typename T>
    inline void Statement::Column(T& arg) {
        DataBinding<T>::Column(this->m_pStatement, Column, arg);
    }

    template <std::int32_t Column, typename T, typename... Args>
    inline void Statement::Column(T& arg, Args&... args) {
        DataBinding<T>::Column(this->m_pStatement, Column, arg);
        this->Column<Column + 1>(args...);
    }

    template <typename T, typename... Args>
    inline void Statement::Column(T& arg, Args&... args) {
        this->Column<0>(arg, args...);
    }

    template <typename... Args>
    inline bool Statement::Fetch(Args&... args) {

        if (!this->m_canFetch) this->Step();

        if (this->m_canFetch) {
            this->Column(args...);
            this->m_canFetch = false;
            return true;
        }

        return false;
    }

    template <typename... Args>
    inline void Statement::Execute(const Args&... args) {
        this->Reset();
        this->Bind(args...);
        this->Step();
    }

}

#include <Vsqlite/Database.h>

namespace Vsqlite {

    inline Statement::Statement(const Database& database, const std::string_view sql, const std::int32_t flags)
        : Statement(database.GetDatabaseHandle(), sql, flags) { }

}

#endif // _AD_STATEMENT_H_