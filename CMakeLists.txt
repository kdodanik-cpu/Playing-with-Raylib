cmake_minimum_required(VERSION 3.20)
project(test_game)

set(CMAKE_CXX_STANDARD 23)

add_executable(test_game main.cpp)
target_include_directories(test_game PRIVATE imported_libraries/raylib/include)
target_link_libraries(test_game PRIVATE
        ${CMAKE_SOURCE_DIR}/imported_libraries/raylib/lib/raylib.lib
        winmm)
