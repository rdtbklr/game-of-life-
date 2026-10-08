cmake_minimum_required(VERSION 3.16)

project(game_of_life LANGUAGES C)

add_executable(game_of_life main.c
    game_of_life_computing.h
    game_of_life_computing.c
)
target_compile_options(game_of_life PRIVATE -mavx512f)
target_link_libraries(game_of_life PRIVATE raylib)
include(GNUInstallDirs)
install(TARGETS game_of_life
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
