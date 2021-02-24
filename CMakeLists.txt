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

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /std:c++17 /MP /incremental")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wno-unknown-pragmas -Wno-unused-parameter -std=c++17")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(
    scratch/src/
    scratch/vendor/assimp/include/
    scratch/vendor/bullet3/src/
    scratch/vendor/glad/include/
    scratch/vendor/glfw/include/
    scratch/vendor/glm/
    scratch/vendor/stb/
    scratch/vendor/tiny-gizmo
    scratch/vendor/imgui
)

file(GLOB VENDORS_SOURCES scratch/vendor/glad/src/glad.c
        scratch/vendor/imgui/*.h
        scratch/vendor/imgui/*.cpp
        scratch/vendor/imgui/backends/imgui_impl_glfw.*
        scratch/vendor/imgui/backends/imgui_impl_opengl3.*)
file(GLOB PROJECT_HEADERS scratch/src/**/*.h* scratch/src/*.h*)
file(GLOB PROJECT_SOURCES scratch/src/**/*.cpp scratch/src/*.cpp)
file(GLOB PROJECT_SHADERS scratch/shader/*.comp
                          scratch/shader/*.frag
                          scratch/shader/*.geom
                          scratch/shader/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} assimp glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scratch/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    DEPENDS ${PROJECT_SHADERS})

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/scratch/models $<TARGET_FILE_DIR:${PROJECT_NAME}>/models)
