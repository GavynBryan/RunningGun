cmake_minimum_required(VERSION 3.16)

project(RunningGun LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to build tests
option(BUILD_TESTS "Build unit tests" OFF)

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(simdjson REQUIRED CONFIG)

# ============= CORE LIBRARY =============
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS
    src/core/*.cpp
)

add_library(RunningGunCore STATIC ${CORE_SOURCES})

target_include_directories(RunningGunCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(RunningGunCore
    PUBLIC
        SDL3::SDL3
        SDL3_image::SDL3_image
        SDL3_ttf::SDL3_ttf
        simdjson::simdjson
)

# ============= GAME LIBRARY =============
file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS
    src/game/*.cpp
)

add_library(RunningGunGame STATIC ${GAME_SOURCES})

target_link_libraries(RunningGunGame
    PUBLIC
        RunningGunCore
)

# ============= APP EXECUTABLE =============
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    src/app/*.cpp
)

add_executable(RunningGun ${APP_SOURCES})

target_link_libraries(RunningGun
    PRIVATE
        RunningGunGame
)

# ============= TESTS =============
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
