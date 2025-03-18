/*
    Vsqlite
    Copyright (c) 2025 V0idPointer
    Licensed under the MIT License
*/

#ifndef _VSQLITE_DATABINDING_H_
#define _VSQLITE_DATABINDING_H_

#include <Vsqlite/SQLite.h>

#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <concepts>
#include <optional>
#include <stdexcept>

namespace Vsqlite {

    template <typename T>
    struct DataBinding {
        
        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const T& arg) { 
            using namespace std::string_literals;
            throw std::invalid_argument("DataBinding specialization for '"s + typeid(T).name() + "' does not exist.");
        }
        
        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, T& arg) { 
            using namespace std::string_literals;
            throw std::invalid_argument("DataBinding specialization for '"s + typeid(T).name() + "' does not exist.");
        }

    };

}

#ifndef VSQLITE_NO_DEFAULT_DATABINDING_SPECIALIZATIONS
namespace Vsqlite {

    template <>
    struct DataBinding<std::nullptr_t> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::nullptr_t&) {
            return sqlite3_bind_null(pStatement, index);
        }

    };

    /* string types */

    template <>
    struct DataBinding<const char*> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const char* arg) {
            return sqlite3_bind_text(pStatement, index, arg, -1, SQLITE_TRANSIENT);
        }

    };

    template <std::size_t L>
    struct DataBinding<char[L]> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const char (&arg)[L]) {
            return sqlite3_bind_text(pStatement, index, arg, static_cast<std::int32_t>(L - 1), SQLITE_TRANSIENT);
        }

    };

    template <>
    struct DataBinding<std::string_view> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::string_view& arg) {
            return DataBinding<const char*>::Bind(pStatement, index, arg.data());
        }

    };

    template <>
    struct DataBinding<std::string> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::string& arg) {
            return DataBinding<const char*>::Bind(pStatement, index, arg.c_str());
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, std::string& arg) {
            const std::int32_t len = sqlite3_column_bytes(pStatement, column);
            const unsigned char* pText = sqlite3_column_text(pStatement, column);
            arg = { reinterpret_cast<const char*>(pText), static_cast<std::size_t>(len) };
        }

    };

    /* integer types */

    template <typename T>
    concept IsIntegral = std::is_integral<T>::value;

    template <typename T>
    concept IsInt64 = std::is_same<T, std::int64_t>::value;

    template <typename T>
    concept IsUint64 = std::is_same<T, std::uint64_t>::value;

    template <typename T>
    requires (IsInt64<T> || IsUint64<T>)
    struct DataBinding<T> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const T& arg) {
            return sqlite3_bind_int64(pStatement, index, static_cast<sqlite3_int64>(arg));
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, T& arg) {
            arg = static_cast<T>(sqlite3_column_int64(pStatement, column));
        }

    };

    template <typename T>
    requires (IsIntegral<T> && !(IsInt64<T> || IsUint64<T>))
    struct DataBinding<T> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const T& arg) {
            return sqlite3_bind_int(pStatement, index, static_cast<std::int32_t>(arg));
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, T& arg) {
            arg = static_cast<T>(sqlite3_column_int(pStatement, column));
        }

    };

    /* floating point types */

    template <>
    struct DataBinding<double> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const double& arg) {
            return sqlite3_bind_double(pStatement, index, arg);
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, double& arg) {
            arg = sqlite3_column_double(pStatement, column);
        }

    };

    template <>
    struct DataBinding<float> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const float& arg) {
            return DataBinding<double>::Bind(pStatement, index, static_cast<double>(arg));
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, float& arg) {
            double val = 0.00;
            DataBinding<double>::Column(pStatement, column, val);
            arg = static_cast<float>(val);
        }

    };

    /* boolean type */

    template <>
    struct DataBinding<bool> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const bool& arg) {
            return DataBinding<std::int32_t>::Bind(pStatement, index, (arg ? 1 : 0));
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, bool& arg) {
            std::int32_t val = 0;
            DataBinding<std::int32_t>::Column(pStatement, column, val);
            arg = ((val != 0) ? true : false);
        }

    };

    /* std::optional */

    template <typename T>
    struct DataBinding<std::optional<T>> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::optional<T>& arg) {
            if (arg.has_value()) return DataBinding<T>::Bind(pStatement, index, arg.value());
            else return DataBinding<std::nullptr_t>::Bind(pStatement, index, nullptr);
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, std::optional<T>& arg) {
            if (sqlite3_column_type(pStatement, column) == SQLITE_NULL) arg = std::nullopt;
            else DataBinding<T>::Column(pStatement, column, arg.emplace());
        }

    };

    template <>
    struct DataBinding<std::nullopt_t> {

        static inline std::int32_t Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::nullopt_t& arg) {
            return DataBinding<std::nullptr_t>::Bind(pStatement, index, nullptr);
        }

    };

}
#endif // VSQLITE_NO_DEFAULT_DATABINDING_SPECIALIZATIONS

#endif // _VSQLITE_DATABINDING_H_