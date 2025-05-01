cmake_minimum_required(VERSION 3.10)

project(QCatGrayLibuvEventDispatcher LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

file(GLOB SRC_LIST
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(${PROJECT_NAME} SHARED ${SRC_LIST})

add_library(${PROJECT_NAME}_a STATIC ${SRC_LIST})

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        Qt${QT_VERSION_MAJOR}::Core
        uv)

target_link_libraries(${PROJECT_NAME}_a 
    PRIVATE 
        Qt${QT_VERSION_MAJOR}::Core
        uv)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME}_a PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})