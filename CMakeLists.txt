cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0135 NEW)
project(game_of_life LANGUAGES C)
include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
add_executable(game_of_life main.c
    game_of_life_computing.h
    game_of_life_computing.c
    graphics.h
    graphics.c
    memory_management.h
    memory_management.c
    Structs.h
)
target_compile_options(game_of_life PRIVATE -mavx512f)
target_link_libraries(game_of_life PRIVATE raylib)
include(GNUInstallDirs)
install(TARGETS game_of_life
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
