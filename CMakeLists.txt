cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Запрещает fallback на старый стандарт
set(CMAKE_CXX_EXTENSIONS OFF)        # Отключает расширения компилятора

project("Android-SDL3-Example" LANGUAGES CXX C)

#УСТАНОВКА ПЕРЕМЕННЫХ

set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(EXTERN_DIR "${PROJECT_SOURCE_DIR}/extern")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(THIRD_PARTY_BUILD_DIR "${PROJECT_SOURCE_DIR}/_3rdparty")

if(ANDROID)
  set(CMAKE_ABI ${CMAKE_ANDROID_ARCH_ABI})
  if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(CMAKE_BUILD_TYPE "Release")
  endif()
endif()

if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(RmlUi_DIR      "${EXTERNAL_DIR}/rmlui/${CMAKE_ABI}/${CMAKE_BUILD_TYPE}/lib/cmake/RmlUi")
set(Freetype_DIR   "${EXTERNAL_DIR}/FreeType/${CMAKE_ABI}/${CMAKE_BUILD_TYPE}/lib/cmake/freetype")
set(SDLWrapper_DIR "${EXTERNAL_DIR}/SDLWrapper/${CMAKE_ABI}/${CMAKE_BUILD_TYPE}/lib/cmake/SDLWrapper")

message("Freetype_DIR=${Freetype_DIR}")

if(NOT DEFINED EXTERNAL_DIR)
    message(ERROR EXTERNAL_DIR is not defined)
endif()

set(SOURCE_CPP
  ${SRC_DIR}/main.cpp 
  ${EXTERN_DIR}/RmlUi/RmlUi_Platform_SDL.cpp
  ${EXTERN_DIR}/RmlUi/RmlUi_Renderer_SDL.cpp
  ${EXTERN_DIR}/pugixml/pugixml.cpp
)
set(SOURCE_HPP
  # ${SRC_DIR}/Objects.hpp
  # ${SRC_DIR}/CollisionMeneger.hpp
)
set(INCLUDE_PATHS
  ${SRC_DIR}
  ${SRC_DIR}/App
  ${EXTERN_DIR}
  D:/dev/libraries/box2d-2.4.2/include
)

# ПОДКЛЮЧЕНИЕ БИБЛИОТЕК

find_package(SDL3_image CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED COMPONENTS SDL3-static)
find_package(Freetype CONFIG REQUIRED)
find_package(RmlUi CONFIG REQUIRED)
find_package(SDLWrapper CONFIG REQUIRED)


#ВРЕМЕННОЕ ПОДКЛЮЧЕНИЕ
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_TESTBED    OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_EXAMPLES   OFF CACHE BOOL "" FORCE) # если есть в твоей версии
add_subdirectory("D:/dev/libraries/box2d-2.4.2" "${THIRD_PARTY_BUILD_DIR}/box2d")

if (ANDROID)
  find_package(ZLIB REQUIRED)
  add_library(${PROJECT_NAME} SHARED ${SOURCE_CPP} ${SOURCE_HPP})
else()
  add_executable(${PROJECT_NAME} ${SOURCE_CPP} ${SOURCE_HPP})
endif()

#TARGET НАСТРОЙКИ

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATHS})

target_compile_definitions(${PROJECT_NAME} PRIVATE
  RMLUI_SDL_VERSION_MAJOR=3
  RMLUI_STATIC_LIB
  SDL_MAIN_USE_CALLBACKS
)

if(ANDROID)

  target_link_libraries(${PROJECT_NAME} PRIVATE
    android
    log
    freetype
    RmlUi::RmlUi
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDLWrapper::SDLWrapper
    box2d
  )

else()

  target_link_libraries(${PROJECT_NAME} PRIVATE
    SDL3::SDL3-static
    SDL3_image::SDL3_image-static
    Freetype::Freetype
    RmlUi::RmlUi
    SDLWrapper::SDLWrapper
    box2d
  )

endif()

#ОПТИМИЗАЦИЯ И ПОСТНАСТРОЙКИ

if(ANDROID AND (CMAKE_BUILD_TYPE MATCHES "Release|MinSizeRel"))
  target_compile_options(${PROJECT_NAME} PRIVATE
          -O3
          -ffunction-sections -fdata-sections
          -fomit-frame-pointer
  )
  target_link_options(${PROJECT_NAME} PRIVATE
          -Wl,--gc-sections
          -Wl,--as-needed
  )
endif()

if(NOT USE_CONSOLE)
  if(MSVC)
    target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
  elseif(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")
  endif()
endif()