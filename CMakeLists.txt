cmake_minimum_required(VERSION 3.16)

project(RunningGun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(simdjson REQUIRED CONFIG)

file(GLOB_RECURSE RUNNINGGUN_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_executable(RunningGun ${RUNNINGGUN_SOURCES})

target_include_directories(RunningGun
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RunningGun
    PRIVATE
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        simdjson::simdjson
)
