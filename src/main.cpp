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

    Log test_log(1, 10, 100.0, "Oak", "Rough", "Warehouse");
    Board test_board(1, 10, "Warehouse", "Oak", 10, 5, 1, "Air");

    // Insert the log into the 'logs' table
    logDB.insertLog(test_log);
    // Insert the board into the 'boards' table
    logDB.insertBoard(test_board);

    return 0;
}
