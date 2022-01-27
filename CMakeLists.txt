cmake_minimum_required(VERSION 3.0)
project(scratch)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(scratch/vendor/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(scratch/vendor/assimp)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(scratch/vendor/bullet3)

add_subdirectory(scratch/vendor/xxHash/cmake_unofficial scratch/vendor/xxHash/build EXCLUDE_FROM_ALL)

add_subdirectory(scratch/vendor/entt)


include(CheckIncludeFile)
set(NFD_SOURCES scratch/vendor/nativefiledialog/src/nfd_common.c)

macro(REQUIRE_INCLUDE_FILE path name)
    CHECK_INCLUDE_FILE(${path} ${name})
    if (NOT ${name})
        message(FATAL_ERROR "${path} not found")
    endif ()
endmacro()

# add specific implementations
if (WIN32)
    REQUIRE_INCLUDE_FILE(windows.h HAS_WINDOWS)
    list(APPEND NFD_SOURCES scratch/vendor/nativefiledialog/src/nfd_win.cpp)
elseif (APPLE)
    REQUIRE_INCLUDE_FILE(AppKit/AppKit.h HAS_APPKIT)
    list(APPEND NFD_SOURCES scratch/vendor/nativefiledialog/src/nfd_cocoa.m)
elseif (UNIX)
    list(APPEND NFD_SOURCES scratch/vendor/nativefiledialog/src/nfd_gtk.c)
elseif (UNIX)
    message(FATAL_ERROR "Cannot detect your system")
endif ()

add_library(nativefiledialog ${NFD_SOURCES})

if (UNIX AND NOT APPLE)
    # GTK3 configuration adapted from https://gist.github.com/fracek/3323924

    # Use the package PkgConfig to detect GTK+ headers/library files
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    # Setup CMake to use GTK+, tell the compiler where to look for headers
    # and to the linker where to look for libraries
    target_include_directories(nativefiledialog PRIVATE ${GTK3_INCLUDE_DIRS})
    target_link_directories(nativefiledialog PRIVATE ${GTK3_LIBRARY_DIRS})

    # Add other flags to the compiler
    target_compile_options(nativefiledialog PRIVATE ${GTK3_CFLAGS_OTHER})

    # Link the target to the GTK+ libraries
    target_link_libraries(nativefiledialog PRIVATE ${GTK3_LIBRARIES})
endif()


if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG /W4 /std:c++17 /MP /incremental")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wno-unused-parameter -std=c++17")
    if (NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif ()
endif ()

include_directories(
        scratch/src/
        scratch/vendor/assimp/include/
        scratch/vendor/bullet3/src/
        scratch/vendor/glad/include/
        scratch/vendor/glfw/include/
        scratch/vendor/glm/
        scratch/vendor/stb/
        scratch/vendor/imgui
        scratch/vendor/ImGuizmo
        scratch/vendor/rapidjson
        scratch/vendor/nativefiledialog/src/include
        scratch/vendor/entt
)

file(GLOB VENDORS_SOURCES scratch/vendor/glad/src/glad.c
        scratch/vendor/imgui/*.h
        scratch/vendor/imgui/*.cpp
        scratch/vendor/imgui/misc/cpp/*.h
        scratch/vendor/imgui/misc/cpp/*.cpp
        scratch/vendor/imgui/backends/imgui_impl_glfw.*
        scratch/vendor/imgui/backends/imgui_impl_opengl3.*
        scratch/vendor/ImGuizmo/*.h
        scratch/vendor/ImGuizmo/*.cpp
        scratch/vendor/rapidjson/include/rapidjson/*.h
        scratch/vendor/rapidjson/include/rapidjson/error/*.h
        scratch/vendor/rapidjson/include/rapidjson/msinttypes/*.h
        scratch/vendor/rapidjson/include/rapidjson/internal/*.h
        scratch/vendor/nativefiledialog/src/include/*)
file(GLOB PROJECT_HEADERS scratch/src/**/*.h* scratch/src/*.h*)
file(GLOB PROJECT_SOURCES scratch/src/**/*.cpp scratch/src/*.cpp)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
        Readme.md
        .gitattributes
        .gitignore
        .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
        -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES}
        ${PROJECT_HEADERS} ${PROJECT_CONFIGS}
        ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} assimp glfw
        ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
        BulletDynamics BulletCollision LinearMath nativefiledialog xxHash::xxhash EnTT::EnTT)
set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scratch/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

