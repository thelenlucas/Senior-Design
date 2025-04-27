# LogDBProject

A C++ application developed using the Qt5 framework for managing woodworking logs and related data.

## Prerequisites

Before building the project, ensure you have the following installed:

*   **C++17 Compiler:** A compiler supporting C++17 (e.g., GCC, Clang, MSVC).
*   **CMake:** Version 3.10 or higher.
*   **Qt5:** Version 5.15 or later, including the following modules:
    *   Widgets
    *   Sql
    *   WebEngineWidgets
*   **(Optional) Doxygen & Graphviz:** Required only if you want to build the documentation (`-DBUILD_DOCS=ON`). Documentation can also be found hosted [on github](https://thelenlucas.github.io/Senior-Design/).

### Warning! QT5 is about to be deprecated! You may have to build from source!

## Building the Project

The general build process uses CMake:

1.  **Clone the repository:**
    ```bash
    git clone <repository-url>
    cd Senior-Design # Or your project directory name
    ```
2.  **Create a build directory:**
    ```bash
    mkdir build && cd build
    ```
3.  **Run CMake:**
    *   Adjust the commands based on your operating system and Qt installation path.
    *   See platform-specific dependency installation instructions below.
    ```bash
    # Example for Linux/macOS with Qt in standard path
    cmake ..

    # Example for macOS using Homebrew Qt5
    # cmake .. -DCMAKE_PREFIX_PATH="$(brew --prefix qt@5)"

    # Example for Windows using Visual Studio 2022
    # cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/path/to/Qt/5.15.x/msvc2019_64"
    ```
    *   **(Optional)** Enable documentation generation:
        ```bash
        cmake .. -DBUILD_DOCS=ON
        ```
4.  **Build the application:**
    ```bash
    # For Makefiles (Linux/macOS)
    make -j$(nproc) # or make -j$(sysctl -n hw.logicalcpu) on macOS

    # For Visual Studio (Windows)
    cmake --build . --config Release
    ```

### Platform-Specific Dependencies

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake qt5-qmake qtbase5-dev libqt5webkit5-dev qtwebengine5-dev
# Optional for docs:
sudo apt-get install -y doxygen graphviz
```

**macOS:**
```bash
brew update
brew install qt@5 cmake
# Optional for docs:
brew install doxygen graphviz
# Remember to set CMAKE_PREFIX_PATH when running cmake, see step 3 above.
```

**Windows:**
*   Install CMake from the official website.
*   Install Visual Studio 2022 (Community Edition is sufficient) with C++ development workload.
*   Install Qt 5.15.x (e.g., using the official Qt online installer), ensuring the `msvc2019_64` version is selected and the `WebEngine` module is included.
*   Add CMake and Qt's `bin` directory to your system's PATH environment variable.
*   Run the CMake command (Step 3) from a Developer Command Prompt for VS 2022, specifying the generator and `CMAKE_PREFIX_PATH`.
*   After building (Step 4), bundle required Qt DLLs using `windeployqt`:
    ```bash
    cd Release # Or Debug if you built that configuration
    C:\path\to\Qt\5.15.x\msvc2019_64\bin\windeployqt.exe logdb.exe
    ```

## Running the Application

After a successful build, the executable (`logdb` or `logdb.exe`) will be located in the `build` directory (or `build/Release` on Windows).

```bash
./logdb # Linux/macOS
# or
./Release/logdb.exe # Windows
```

## Generating Documentation

If you enabled the `BUILD_DOCS` option during CMake configuration:

```bash
# Run from the build directory
make doc
```
The documentation will be generated in the `docs/html` directory within your build folder.
