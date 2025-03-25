# Project Setup (Dev)

Run the following commands:
```
cd /mnt/c/Users/[user]/Documents/GitHub/Senior-Design
    OR
cd GitHub/Senior-Design
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt install cmake
sudo apt install -y qtcreator qtbase5-dev qt5-qmake cmake
bash init.sh
mkdir build && cd build && cmake .. && cp ../example.db ./test.db && make && ./logdb
cmake ..
make
```

# Commands (Dev)

```
./logdb
    // Creates and runs the basic application GUI
cp ../example.db ./test.db
    // Creates an Oak log for testing purposes (static values)
cmake .. && cp ../example.db ./test.db && make && ./logdb
    // Remake and run app
```

# Errors


# Script Requirements

QT5.15
cmake 0.0.17 (Version # taken from VSCode)
