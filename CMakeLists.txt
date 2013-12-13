# If Lua is installed in a non-standard location, please set the LUA_DIR
# environment variable to point to prefix for the install. Eg:
#       Unix: export LUA_DIR=/home/user/pkg
#       Windows: set LUA_DIR=c:\lua51

project(pb C)
cmake_minimum_required(VERSION 2.6)

option(USE_LUA "Use Lua (also called 'C' Lua) version 5.1 includes (default)" ON)
option(USE_LUAJIT "Use LuaJIT includes instead of 'C' Lua ones (recommended, if you're using LuaJIT, but disabled by default)")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
    "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif()

if(USE_LUAJIT)
  # Find luajit
  find_package(LuaJIT REQUIRED)
  set(USE_LUA OFF)
  # / Find lua
endif()

if(USE_LUA)
  # Find lua
  find_package(Lua51 REQUIRED)
  # / Find lua
endif()

include_directories(${LUA_INCLUDE_DIR})

# Handle platforms missing isinf() macro (Eg, some Solaris systems).
include(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(isinf math.h HAVE_ISINF)
if(NOT HAVE_ISINF)
  add_definitions(-DUSE_INTERNAL_ISINF)
endif()

set(_MODULE_LINK "${CMAKE_THREAD_LIBS_INIT}")
get_filename_component(_lua_lib_dir ${LUA_LIBRARY} PATH)

if(APPLE)
  set(CMAKE_SHARED_MODULE_CREATE_C_FLAGS
    "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS} -undefined dynamic_lookup")
endif()

if(WIN32)
  # Win32 modules need to be linked to the Lua library.
  set(_MODULE_LINK ${LUA_LIBRARY} ${_MODULE_LINK})
  set(_lua_module_dir "${_lua_lib_dir}")
  # Windows sprintf()/strtod() handle NaN/inf differently. Not supported.
  add_definitions(-DDISABLE_INVALID_NUMBERS)
else()
  set(_lua_module_dir "${_lua_lib_dir}/lua/5.1")
endif()

add_library(pb MODULE pb.c)
set_target_properties(pb PROPERTIES PREFIX "")
target_link_libraries(pb ${_MODULE_LINK})
install(TARGETS pb DESTINATION "${_lua_module_dir}")

# vi:ai et sw=4 ts=4:
