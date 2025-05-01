cmake_minimum_required(VERSION 3.9)

project(QCatGrayLibuv LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/QCatGrayLibuvEventDispatcher)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/QCatGrayLibuvTcp)