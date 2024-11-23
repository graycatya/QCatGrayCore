cmake_minimum_required(VERSION 3.9)

project(QCatGrayCore LANGUAGES CXX)

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
elseif (UNIX)
add_compile_options(-Wall -fPIC -Wno-stringop-overflow)
endif()

message(STATUS "------------------------------------------------------------" )
message(STATUS "[QCatGrayCore] Configuration summary."                     )
message(STATUS "------------------------------------------------------------ ")
message(STATUS " System configuration:"                                       )
message(STATUS " .. Processor type .............. = ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS " .. CMake executable ............ = ${CMAKE_COMMAND}"         )
message(STATUS " .. CMake version ............... = ${CMAKE_VERSION}"         )
message(STATUS " .. System name ................. = ${CMAKE_SYSTEM}"          )
message(STATUS " .. C++ compiler ................ = ${CMAKE_CXX_COMPILER}"    )
message(STATUS " .. C compiler .................. = ${CMAKE_C_COMPILER}"      )
message(STATUS " .. size(void*) ................. = ${CMAKE_SIZEOF_VOID_P}"   )
message(STATUS " .. cmake current source ................. = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS " .. cmake system name ................. = ${CMAKE_SYSTEM_NAME}")
message(STATUS "------------------------------------------------------------ ")


set(3RDPARTY

)

set(MODULES
    QCatGrayNetWork
    QCatGraySerial
    QCatGrayUniversal
)

set(EXAMPLES
)

foreach(3rdparty_module ${3RDPARTY})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/${3rdparty_module})
endforeach(3rdparty_module ${3RDPARTY})

foreach(module ${MODULES})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/${module})
endforeach(module ${MODULES})

foreach(example ${EXAMPLES})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples/${example})
endforeach(example ${EXAMPLES})
