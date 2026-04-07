cmake_minimum_required(VERSION 3.20)
project(HalbyFbxEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt MOC/UIC/RCC auto-processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

# Custom find modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Dependencies
find_package(FBXSDK REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)

    # Google Test — prefer installed package, fallback to FetchContent
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

add_subdirectory(src)

# Tests (enable with -DBUILD_TESTING=ON, default ON)
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
