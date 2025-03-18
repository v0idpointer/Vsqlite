/*
    Vsqlite
    Copyright (c) 2025 V0idPointer
    Licensed under the MIT License
*/

#ifndef _VSQLITE_SQLITE_H_
#define _VSQLITE_SQLITE_H_

#ifdef VSQLITE_USE_WINSQLITE

#if __has_include(<winsqlite/winsqlite3.h>)
#include <winsqlite/winsqlite3.h>
#pragma comment (lib, "winsqlite3.lib")
#else
#error Vsqlite: cannot include 'winsqlite3.h'
#endif

#else

#if __has_include(<sqlite3.h>)
#include <sqlite3.h>
#else
#error Vsqlite: cannot include 'sqlite3.h'
#endif

#endif

namespace Vsqlite { }

#endif // _VSQLITE_SQLITE_H_