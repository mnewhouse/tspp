cmake_minimum_required(VERSION 2.6)

project(squirrel)

include_directories("${PROJECT_SOURCE_DIR}/include")

add_definitions(-D_LIB)
add_definitions(-D_SQ64)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DGARBAGE_COLLECTOR")

set(SQUIRREL_SRC
    squirrel/sqapi.cpp
    squirrel/sqbaselib.cpp
    squirrel/sqclass.cpp
    squirrel/sqcompiler.cpp
    squirrel/sqdebug.cpp
    squirrel/sqfuncstate.cpp
    squirrel/sqlexer.cpp
    squirrel/sqmem.cpp
    squirrel/sqobject.cpp
    squirrel/sqstate.cpp
    squirrel/sqtable.cpp
    squirrel/sqvm.cpp
)

set(SQSTDLIB_SRC
    sqstdlib/sqstdaux.cpp
    sqstdlib/sqstdblob.cpp
    sqstdlib/sqstdio.cpp
    sqstdlib/sqstdmath.cpp
    sqstdlib/sqstdrex.cpp
    sqstdlib/sqstdstream.cpp
    sqstdlib/sqstdstring.cpp
    sqstdlib/sqstdsystem.cpp
)

add_library(squirrel STATIC ${SQUIRREL_SRC})
add_library(sqstdlib STATIC ${SQSTDLIB_SRC})

