cmake_minimum_required(VERSION 3.23)
project(MineSweeperC)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk+-3.0)

include_directories(${GTK_INCLUDE_DIRS})
link_directories(${GTK_LIBRARY_DIRS})
link_libraries(${MineSweeperC} ${GTK_LIBRARIES})

add_definitions(${GTK_CFLAGS_OTHER})

include_directories(.)

add_executable(MineSweeper
        boolean.h
        field.c
        field.h
        main.c
        randomGen.c
        randomGen.h
        square.h)

