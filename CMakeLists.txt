cmake_minimum_required(VERSION 3.16)

project(KGame VERSION 1.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_C_FLAGS "-Ofast")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(KGame main.c Being.c event.c function.c game.c Player.c Projectile.c render.c Scroll.c World.c)

target_link_libraries(KGame PRIVATE SDL3::SDL3)

target_include_directories(KGame PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_BINARY_DIR}/../KGame")