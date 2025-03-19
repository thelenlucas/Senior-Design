# Project Setup (Dev)

Run the following commands:
```
cd /mnt/c/Users/[user]/Documents/GitHub/Senior-Design
    // This gets you to the Windows directory 
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt install cmake
sudo apt install -y qtcreator qtbase5-dev qt5-qmake cmake
bash init.sh
mkdir build && cd build && cmake .. && cp ../example.db ./test.db && make && ./log.db
cmake ..
make
```

# Commands (Dev)

```
./logdb
    // Creates and runs the basic application GUI
cp ../example.db ./test.db
    // Creates an Oak log for testing purposes (static values)
```

# Errors

The Enter Log button in the top right currently executes a floating point exception and causes the GUI to crash

Cookies Header:
- Line 51: Database, argument 1 invalid, invalid template id
Cookies C++:
- Line 26: Expected ::, no declaration
- Line 28: Expected ) before ;
- Line 34: Unqualified )
- Line 59: Expected ; before }

# Script Requirements

QT5.15
cmake 0.0.17 (Version # taken from VSCode)
