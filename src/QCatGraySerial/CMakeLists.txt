cmake_minimum_required(VERSION 3.10)

project(QCatGraySerial LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core SerialPort)

file(GLOB SRC_LIST
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

if(NOT WIN32)
    list(REMOVE_ITEM SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/QCatGrayWinMonitorSerial.h
    ${CMAKE_CURRENT_SOURCE_DIR}/QCatGrayWinMonitorSerial.cpp)
endif(NOT WIN32)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

add_library(${PROJECT_NAME}_a STATIC ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::SerialPort)

target_link_libraries(${PROJECT_NAME}_a 
    PRIVATE 
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::SerialPort)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME}_a PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
