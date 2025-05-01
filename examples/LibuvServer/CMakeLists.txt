cmake_minimum_required(VERSION 3.5)

project(LibuvServer VERSION 0.1 LANGUAGES CXX)




set(PROJECT_SOURCES
LibuvServer.cpp
)

add_executable(${PROJECT_NAME}
${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    uv
)
