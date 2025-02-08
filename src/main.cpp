#include <sqlite3.h>
#include <string>
#include <iostream>
#include "logs.hpp"

int main() {

    // Create or open the database file "logs.db"
    Database logDB("test.db");

    // Create the logs table
    logDB.createLogsTable();
    // Create the boards table
    logDB.createBoardsTable();

    return 0;
}
