<h1 align="center">Crossplatform SDL3+RmlUi example</h1>

## Кроссплатформенный SDL3+RmlUi проект

[![English version](https://img.shields.io/badge/English%20version-blue)](README.md)

## Описание

Легковесный кроссплатформенный С++ проект с библиотеками SDL3 + RmlUi с легким подключением библиотек для Android

# Особенности Платформ

1. Windows
   * Используется статическая линковка библиотеки
   * Библиотеки SDL3 должны быть уже статически собраны и находится по пути `EXTERNAL_DIR` (смотри `Инструкция по сборке`)
   * Весь Cmake смотри в файле `cmake/AppWindows.cmake`
2. Android
   * Сборка через Cmake + Ninja из Android Studio
   * Весь Cmake смотри в файле `cmake/AppAndroid.cmake` 
3. Linux
   * В настоящий момент нет cmake файла для сборки под Linux, вы можете отправить нам pull request с файлом `AppLinux.cmake`, если вы будете уверены, что она работает

## Зависимости

Язык программирования: **С++23**

Библиотеки:
* [RmlUi](https://github.com/mikke89/RmlUi) - библиотека для Ui
* [FreeType](https://freetype.org/) - Поддержка шрифтов для RmlUi
* [SDL3, SDL3_image](https://github.com/libsdl-org) - Графическая библиотека (используются *.aar* файлы из официальных релизов)
* [SDL_gfx](https://github.com/sabdul-khabir/SDL3_gfx) - Графические примитивы для SDL3

## Установка и запуск

### Предварительные требования

#### Для Windows
* Используйте **С++23**
* Cmake **3.22**+

#### Для Android

* SDK - **35**
* NDK - **25.1.8937393**+
* Cmake **3.22**+

### Инструкция по сборке

#### Важно!
Cmake переменная EXTERNAL_DIR должна указывать на директорию, в которой находятся библиотеки RmlUi, SDL3 и другие

1. **Клонируйте репозиторий**
   ```sh
   git clone https://github.com/Igorantivirus/Crossplatform-Example-SDL3-RmlUi-project
   cd Crossplatform-Example-SDL3-RmlUi-project
   ```

2. **Соберите CMake**

    #### Для Android
    * В файле `build.gradle` измените аргумент для Cmake `"-DEXTERNAL_DIR=\"ПУТЬ_ДО_БИБЛИОТЕК\""`
    * Запустите синхронизацию gradle файлов через `Ctrl+Shift+O`
       
    #### Для Windows
    ```sh
    cmake -B build -DCMAKE_BUILD_TYPE=Release -DEXTERNAL_DIR="ПУТЬ_ДО_БИБЛИОТЕК" -DUSE_CONSOLE=FALSE
    ```

3. **Соберите проект** 
   
    #### Для Android
    Запустите проект на эмуляторе или на Вашем устройстве
    #### Для Widnows
    ```sh
    cmake --build build --config Release
    ```

## Лицензия 
MIT Лицензия
