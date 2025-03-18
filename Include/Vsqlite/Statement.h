/*
    Vsqlite
    Copyright (c) 2025 V0idPointer
    Licensed under the MIT License
*/

#ifndef _VSQLITE_STATEMENT_H_
#define _VSQLITE_STATEMENT_H_

#include <Vsqlite/SQLite.h>
#include <Vsqlite/DataBinding.h>
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
        bool m_canFetch;

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

        /**
         * 
         * 
         * @tparam Index
         * @tparam T
         * @param arg
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <std::int32_t Index, typename T>
        void Bind(const T& arg);

        /**
         * 
         * 
         * @tparam T
         * @param arg
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <typename T>
        void Bind(const T& arg);

        /**
         * 
         * 
         * @tparam Index
         * @tparam T
         * @tparam Args
         * @param arg
         * @param args
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <std::int32_t Index, typename T, typename... Args>
        void Bind(const T& arg, const Args&... args);

        /**
         * 
         * 
         * @tparam T
         * @tparam Args
         * @param arg
         * @param args
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <typename T, typename... Args>
        void Bind(const T& arg, const Args&... args);

        /**
         * Unbinds all data from the SQLite statement.
         * 
         * @exception SqliteException
         */
        void Unbind(void);

        /**
         * This function does nothing.
         */
        void Column(void);

        /**
         * 
         * 
         * @tparam Column
         * @tparam T
         * @param arg
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <std::int32_t Column, typename T>
        void Column(T& arg);

        /**
         * 
         * 
         * @tparam T
         * @param arg
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <typename T>
        void Column(T& arg);

        /**
         * 
         * 
         * @tparam Column
         * @tparam T
         * @tparam Args
         * @param arg
         * @param args
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <std::int32_t Column, typename T, typename... Args>
        void Column(T& arg, Args&... args);

        /**
         * 
         * 
         * @tparam T
         * @tparam Args
         * @param arg
         * @param args
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <typename T, typename... Args>
        void Column(T& arg, Args&... args);

        /**
         * Retrieves values from the current row of an SQLite result set.
         * 
         * @tparam Args...
         * @param args
         * @returns true if Fetch can retrieve more data; otherwise, false.
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <typename... Args>
        bool Fetch(Args&... args);

        /**
         * Executes the statement.
         * 
         * @tparam Args... 
         * @param args 
         * @exception std::invalid_argument
         * @exception SqliteException
         */
        template <typename... Args>
        void Execute(const Args&... args);

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

        if (this != &statement) {
            
            if (this->m_pStatement) {
                sqlite3_finalize(this->m_pStatement);
                this->m_pStatement = nullptr;
                this->m_canFetch = false;
            }

            this->m_pStatement = statement.m_pStatement;
            this->m_canFetch = statement.m_canFetch;
            statement.m_pStatement = nullptr;
            statement.m_canFetch = false;
            
        }

        return static_cast<Statement&>(*this);
    }

    inline sqlite3_stmt* Statement::GetStatementHandle() const {
        return this->m_pStatement;
    }

    inline void Statement::Reset() { 
        const std::int32_t res = sqlite3_reset(this->m_pStatement);
        if (res != SQLITE_OK) throw SqliteException(this->m_pStatement);
        this->m_canFetch = false;
    }

    inline void Statement::Step() { 
        const std::int32_t res = sqlite3_step(this->m_pStatement);
        if ((res != SQLITE_ROW) && (res != SQLITE_DONE)) throw SqliteException(this->m_pStatement); 
        this->m_canFetch = (res == SQLITE_ROW);
    }

    inline void Statement::Execute() { 
        this->Reset();
        this->Unbind();
        this->Step();
    }

    template <std::int32_t Index, typename T>
    inline void Statement::Bind(const T& arg) {
        const std::int32_t res = DataBinding<T>::Bind(this->m_pStatement, Index, arg);
        if (res != SQLITE_OK) throw SqliteException(this->m_pStatement);
    }

    template <typename T>
    inline void Statement::Bind(const T& arg) {
        this->Bind<1>(arg);
    }

    template <std::int32_t Index, typename T, typename... Args>
    inline void Statement::Bind(const T& arg, const Args&... args) {
        const std::int32_t res = DataBinding<T>::Bind(this->m_pStatement, Index, arg);
        if (res != SQLITE_OK) throw SqliteException(this->m_pStatement);
        this->Bind<Index + 1>(args...);
    }

    template <typename T, typename... Args>
    inline void Statement::Bind(const T& arg, const Args&... args) {
        this->Bind<1>(arg, args...);
    }

    inline void Statement::Unbind() {
        const std::int32_t res = sqlite3_clear_bindings(this->m_pStatement);
        if (res != SQLITE_OK) throw SqliteException(this->m_pStatement);
    }

    inline void Statement::Column() { }

    template <std::int32_t Column, typename T>
    inline void Statement::Column(T& arg) {
        DataBinding<T>::Column(this->m_pStatement, Column, arg);
    }

    template <typename T>
    inline void Statement::Column(T& arg) {
        this->Column<0>(arg);
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
        this->Unbind();
        this->Bind(args...);
        this->Step();
    }

}

#include <Vsqlite/Database.h>

namespace Vsqlite {

    inline Statement::Statement(const Database& database, const std::string_view sql, const std::int32_t flags)
        : Statement(database.GetDatabaseHandle(), sql, flags) { }

}

#endif // _VSQLITE_STATEMENT_H_