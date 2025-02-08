#include <sqlite3.h>
#include <string>
#include <iostream>
#include "logs.hpp"

int main() {

    // Create or open the database file "logs.db"
    LogDB logDB("logs.db");

    // Create the logs table
    logDB.createTable();
    std::cout << "Table created successfully." << std::endl;

    return 0;
}
