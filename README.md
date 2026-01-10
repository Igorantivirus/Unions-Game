<h1 align="center">Crossplatform SDL3+RmlUi example</h1>

## Crossplatform SDL3+RmlUi project

[![Russian version](https://img.shields.io/badge/Russian%20version-blue)](README_RU.md)

## Description

Lightweight crossplatform С++ project with SDL3 + RmlUi libraries with light link libraries for Android

# Platform Features

1. Windows
   * Static linking of the library is used
   * The SDL3 libraries should already be statically compiled and are on the way `EXTERNAL_DIR` (See `Инструкция по сборке`)
   * All Cmake see in `cmake/AppWindows.cmake`
2. Android
   * Build via Cmake + Ninja из Android Studio
   * All Cmake see in `cmake/AppAndroid.cmake` 
3. Linux
   * There is currently no cmake build file for Linux, you can send us a pull request with the file `AppLinux.cmake` if you are sure that it works.


## Dependencies

Programming language: **С++23**

Libraries:
* [RmlUi](https://github.com/mikke89/RmlUi) - library for Ui
* [FreeType](https://freetype.org/) - supporting fonts for RmlUi
* [SDL3, SDL3_image](https://github.com/libsdl-org) - Graphic library (project use *.aar* files from official SDL3 repos)
* [SDL_gfx](https://github.com/sabdul-khabir/SDL3_gfx) - Graphics primitives for SDL3

## Install and Run

### Preliminary requirements

#### For Windows
* Use **С++23**
* Cmake **3.22**+

#### For Android

* SDK - **35**
* NDK - **25.1.8937393**+
* Cmake **3.22**+

### Assembly instructions

#### Important!
Cmake variable EXTERNAL_DIR it must point to the directory where the libraries are located: RmlUi, SDL3 and others

1. **Clone the repository**
   ```sh
   git clone https://github.com/Igorantivirus/Crossplatform-Example-SDL3-RmlUi-project
   cd Crossplatform-Example-SDL3-RmlUi-project
   ```

2. **Configure CMake**

    #### For Android
    * In `build.gradle` file change argument `"-DEXTERNAL_DIR=\"LIBRARIES_PATH\""` for Cmake
    * Run synch gradle files throw `Ctrl+Shift+O`
       
    #### For Windows
    ```sh
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DEXTERNAL_DIR="LIBRARIES_PATH" -DUSE_CONSOLE=FALSE
    ```

3. **Build & Run project** 
   
    #### For Android
    Run project in emulator or in Your device
    #### For Widnows
    ```sh
    cmake --build build --config Release
    ```

## License 
MIT License
