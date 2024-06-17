/*
    Vsqlite
    Copyright (C) V0idPointer
    Licensed under the MIT License
*/

#ifndef _AD_DATABINDING_H_
#define _AD_DATABINDING_H_

#include <Vsqlite/SQLite.h>

#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <optional>

namespace Vsqlite {

    template <typename T>
    class DataBinding {
    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const T& arg) { }
        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, T& arg) { }
    };

}

#define _AD_BINDING_INT(Type) \
template <> \
class DataBinding<Type> { \
public: \
    static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const Type& arg) { \
        return sqlite3_bind_int(pStatement, index, static_cast<Type>(arg)); \
    } \
    static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, Type& arg) { \
        arg = static_cast<Type>(sqlite3_column_int(pStatement, column)); \
    }\
}

#define _AD_BINDING_INT64(Type) \
template <> \
class DataBinding<Type> { \
public: \
    static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const Type& arg) { \
        return sqlite3_bind_int64(pStatement, index, static_cast<sqlite3_int64>(arg)); \
    } \
    static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, Type& arg) { \
        arg = static_cast<Type>(sqlite3_column_int64(pStatement, column)); \
    } \
}

namespace Vsqlite {

    template <>
    class DataBinding<std::nullptr_t> {
    
    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::nullptr_t& arg) { 
            return sqlite3_bind_null(pStatement, index);
        }

    };

    /* string types */

    template <>
    class DataBinding<const char*> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const char* const arg) {
            return sqlite3_bind_text(pStatement, index, arg, -1, SQLITE_TRANSIENT);
        }

    };

    template <std::size_t L>
    class DataBinding<char[L]> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const char (&arg)[L]) {
            return sqlite3_bind_text(pStatement, index, arg, static_cast<int>(L - 1), SQLITE_TRANSIENT);
        }

    };

    template <>
    class DataBinding<std::string_view> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::string_view& arg) {
            return DataBinding<const char*>::Bind(pStatement, index, arg.data());
        }

    };

    template <>
    class DataBinding<std::string> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::string& arg) {
            return DataBinding<const char*>::Bind(pStatement, index, arg.c_str());
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, std::string& arg) {
            const int len = sqlite3_column_bytes(pStatement, column);
            const unsigned char* pText = sqlite3_column_text(pStatement, column);
            arg = { reinterpret_cast<const char*>(pText), static_cast<std::size_t>(len) };
        }

    };

    /* integer types */

    _AD_BINDING_INT(std::int8_t);
    _AD_BINDING_INT(std::uint8_t);
    _AD_BINDING_INT(std::int16_t);
    _AD_BINDING_INT(std::uint16_t);
    _AD_BINDING_INT(std::int32_t);
    _AD_BINDING_INT(std::uint32_t);

    _AD_BINDING_INT64(std::int64_t);
    _AD_BINDING_INT64(std::uint64_t);

    /* floating point types */
    
    template <>
    class DataBinding<double> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const double& arg) {
            return sqlite3_bind_double(pStatement, index, arg);
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, double& arg) {
            arg = sqlite3_column_double(pStatement, column);
        }

    };

    template <>
    class DataBinding<float> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const float& arg) {
            return DataBinding<double>::Bind(pStatement, index, static_cast<double>(arg));
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, float& arg) {
            double x = 0.00;
            DataBinding<double>::Column(pStatement, column, x);
            arg = static_cast<float>(x);
        }

    };

    /* bool type */
    
    template <>
    class DataBinding<bool> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const bool& arg) {
            return sqlite3_bind_int(pStatement, index, static_cast<int>(arg));
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, bool& arg) {
            arg = static_cast<bool>(sqlite3_column_int(pStatement, column));
        }

    };

    /* std::optional */
    
    template <typename T>
    class DataBinding<std::optional<T>> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::optional<T>& arg) {
            if (arg.has_value()) return DataBinding<T>::Bind(pStatement, index, arg.value());
            else return DataBinding<std::nullptr_t>::Bind(pStatement, index, nullptr);
        }

        static inline void Column(sqlite3_stmt* const pStatement, const std::int32_t column, std::optional<T>& arg) {
            if (sqlite3_column_type(pStatement, column) == SQLITE_NULL) arg = std::nullopt;
            else DataBinding<T>::Column(pStatement, column, arg.emplace());
        }

    };

    template <>
    class DataBinding<std::nullopt_t> {

    public:
        static inline int Bind(sqlite3_stmt* const pStatement, const std::int32_t index, const std::nullopt_t& arg) {
            return DataBinding<std::nullptr_t>::Bind(pStatement, index, nullptr);
        }

    };

}

#undef _AD_BINDING_INT
#undef _AD_BINDING_INT64

#endif // _AD_DATABINDING_H_