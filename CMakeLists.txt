# Macro to print some message to stdout, useful for debugging purpose.
MACRO(DBG_MSG _MSG)
FOREACH(_MSG_STRING ${_MSG})
  MESSAGE(STATUS "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${_MSG_STRING}")
ENDFOREACH()
ENDMACRO(DBG_MSG)

#
# Build type
#
# If the user specifies -DCMAKE_BUILD_TYPE on the command line, take their definition
# and dump it in the cache along with proper documentation, otherwise set CMAKE_BUILD_TYPE
# to Debug prior to calling PROJECT()
#
set(CMAKE_BUILD_TYPE_DOCUMENTATION "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release")
IF(DEFINED CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING ${CMAKE_BUILD_TYPE_DOCUMENTATION})
ELSE()
   SET(CMAKE_BUILD_TYPE Debug CACHE STRING ${CMAKE_BUILD_TYPE_DOCUMENTATION})
ENDIF()

# The name of our project is "bmextract". CMakeLists files in this project can
# refer to the root source directory of the project as ${bmextract_SOURCE_DIR} and
# to the root binary directory of the project as ${bmextract_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)
project (bmextract)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Compiler flags
#set(LINK_FLAGS "-g")
#set(CMAKE_CXX_FLAGS "-g")
set (CMAKE_CXX_FLAGS "-fopenmp -Wall")

add_executable (bmextract main.cpp)

