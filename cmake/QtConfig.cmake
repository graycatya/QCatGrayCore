# ################################################################################
#
# The Qt SDK path
# You can set the evnironment varibale via
# 1. command line
# Windows: SET QTDIR=PATH/TO/QT
# Non-Windows: export QTDIR=PATH/TO/QT
# 2. modifying the value below directly
#

# Qt build toolchain path not set or doesn't exist
# try to read from environment QTDIR
if(NOT EXISTS ${QT_SDK_DIR})
  message(STATUS "QT_SDK_DIR not found, try to read from environment variable: QTDIR")
  set(QT_SDK_DIR "$ENV{QTDIR}" CACHE STRING "QT_SDK_DIR read from environment variable: QTDIR" FORCE)
endif()


# find required components
message(STATUS " Qt SDK dir: " ${QT_SDK_DIR})
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_SDK_DIR})
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)


#--
# cmake 国际化处理
#--
function(build_linguisttools)
if(ARGN)
  if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
      
  else(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    find_package(Qt5LinguistTools REQUIRED)

    find_program(LUPDATE_EXECUTABLE lupdate)
    find_program(LRELEASE_EXECUTABLE lrelease)

    foreach(_ts_file IN LISTS ARGN)

      execute_process(
        COMMAND ${LUPDATE_EXECUTABLE} -recursive ${CMAKE_CURRENT_SOURCE_DIR} -ts ${_ts_file})
      execute_process(
        COMMAND ${LRELEASE_EXECUTABLE} ${_ts_file})

    endforeach()
  endif(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
endif(ARGN)
endfunction(build_linguisttools)

function(add_module_sources)
  if(ARGN)
    set(TEMP_MODULE_SRCS "")
    list(FILTER ARGN EXCLUDE REGEX "${ARGV0}$")
    list(FILTER ARGN EXCLUDE REGEX "${ARGV1}$")
    
    if(ARGV1)
    
      foreach(module ${ARGN})
        string(REGEX REPLACE "/$" "" SOURCES_PATH "${ARGV1}")
        string(APPEND SOURCES_PATH "/${module}")
        file(GLOB CURPATH_DIR_LIB_SRCS
          "${SOURCES_PATH}/*.h"
          "${SOURCES_PATH}/*.cpp"
        )
        list(APPEND TEMP_MODULE_SRCS ${CURPATH_DIR_LIB_SRCS})
        if (EXISTS "${SOURCES_PATH}/resources")
          add_module_resource(MODULE_RESOURCE "${SOURCES_PATH}/resources" ${module})
          list(APPEND TEMP_MODULE_SRCS ${MODULE_RESOURCE})
          message("MODULE_RESOURCE: ${MODULE_RESOURCE}")
        endif(EXISTS "${SOURCES_PATH}/resources")
        foreach(submodule ${${module}})
          #message("${module} submodule: ${submodule}")
          add_module_sources(TEMP_SUBMODULE_SRCS "${ARGV1}/${module}" ${submodule})
          list(APPEND TEMP_MODULE_SRCS ${TEMP_SUBMODULE_SRCS})
        endforeach(submodule ${${module}})
        
      endforeach(module ${ARGN})

      set(${ARGV0} ${TEMP_MODULE_SRCS} PARENT_SCOPE)

    endif(ARGV1)
    
  else(ARGN)
    message("add_module_sources parameter-free error")
  endif(ARGN)
endfunction(add_module_sources)

function(add_module_resource)
  if(ARGN)
    set(TEMP_MODULE_QRC_RESOURCE "")

    if(ARGV1)
      string(REGEX REPLACE "/$" "" RESOURCE_PATH "${ARGV1}")
      if(ARGV2)
        string(APPEND RESOURCE_PATH "/${ARGV2}.qrc")
        if(EXISTS ${RESOURCE_PATH})
          if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
            qt_add_resources(TEMP_MODULE_QRC_RESOURCE ${RESOURCE_PATH})
          else(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
            qt5_add_resources(TEMP_MODULE_QRC_RESOURCE ${RESOURCE_PATH})
          endif(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
        endif(EXISTS ${RESOURCE_PATH})
      endif(ARGV2)
    endif(ARGV1)
    set(${ARGV0} ${TEMP_MODULE_QRC_RESOURCE} PARENT_SCOPE)
  else(ARGN)
    message("add_module_resource parameter-free error")
  endif(ARGN)
endfunction(add_module_resource)
