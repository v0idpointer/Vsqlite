# Vsqlite

Vsqlite is a simple lightweight C++17 wrapper library for SQLite3.

On Windows 10 and Windows 11, you can use winsqlite3 by #define-ing VSQLITE_USE_WINSQLITE

# Example
```cpp
#include <Vsqlite/Database.h>
#include <Vsqlite/Statement.h>

using namespace Vsqlite;

int main(int argc, char* argv[]) {

    Database db("database.db");
    
    db.Execute(R"(
        CREATE TABLE IF NOT EXISTS Customers(
            CustomerId INTEGER PRIMARY KEY NOT NULL,
            FirstName TEXT NOT NULL,
            LastName TEXT NOT NULL,
            Email TEXT NOT NULL
        );
    )");

    Statement s = db.PrepareStatement(R"(
        INSERT INTO Customers (FirstName, LastName, Email)
        VALUES (?, ?, ?);
    )", SQLITE_PREPARE_PERSISTENT);

    s.Execute("John", "Warosa", "jbarosa@example.com");

    return 0;
}
```