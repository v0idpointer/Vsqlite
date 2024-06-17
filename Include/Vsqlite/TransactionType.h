/*
    Vsqlite
    Copyright (C) V0idPointer
    Licensed under the MIT License
*/

#ifndef _AD_TRANSACTIONTYPE_H_
#define _AD_TRANSACTIONTYPE_H_

#include <cstdint>

namespace Vsqlite {

    enum class TransactionType : std::uint8_t {
        DEFERRED = 0,
        IMMEDIATE = 1,
        EXCLUSIVE = 2,
    };

}

#endif // _AD_TRANSACTIONTYPE_H_