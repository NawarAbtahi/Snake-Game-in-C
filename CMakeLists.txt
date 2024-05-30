cmake_minimum_required(VERSION 3.29.1)

project(Mygame)

add_executable(Mygame src/snake.c)

find_package(SDL2 CONFIG REQUIRED) 
find_package(SDL2_image REQUIRED)

target_link_libraries(Mygame PRIVATE SDL2::SDL2main SDL2::SDL2-static SDL2_image::SDL2_image-static)
