# ============= TEST LAYER =============
# Unit tests for RunningGun CORE and GAME layers

enable_testing()

find_package(GTest REQUIRED CONFIG)

# Core layer tests
file(GLOB_RECURSE CORE_TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp
)

# Game layer tests
file(GLOB_RECURSE GAME_TEST_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/game/*.cpp
)

add_executable(RunningGunTests 
    ${CORE_TEST_SOURCES}
    ${GAME_TEST_SOURCES}
)

target_link_libraries(RunningGunTests
    PRIVATE
        RunningGunGame
        RunningGunCore
        GTest::gtest
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(RunningGunTests)
