cmake_minimum_required(VERSION 3.16)

project(WindowsVersionExt LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(WindowsVersionExt STATIC
  src/windowsversionext.cpp
  include/windowsversionext.h
)

target_link_libraries(WindowsVersionExt PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_include_directories(WindowsVersionExt PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
