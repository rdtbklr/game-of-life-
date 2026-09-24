cmake_minimum_required(VERSION 3.16)

project(game_of_life LANGUAGES C)

add_executable(game_of_life main.c)

include(GNUInstallDirs)
install(TARGETS game_of_life
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
