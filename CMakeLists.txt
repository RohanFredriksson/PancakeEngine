cmake_minimum_required(VERSION 3.14)

project(pancake CXX C)

# Only build the tests and examples by default when this is the top level project.
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(PANCAKE_TOP_LEVEL ON)
else()
    set(PANCAKE_TOP_LEVEL OFF)
endif()

option(PANCAKE_BUILD_TESTS "Build the Pancake tests" ${PANCAKE_TOP_LEVEL})
option(PANCAKE_BUILD_EXAMPLES "Build the Pancake examples" ${PANCAKE_TOP_LEVEL})
option(PANCAKE_ASAN "Build Pancake and everything linking it with AddressSanitizer" OFF)

add_subdirectory(dependencies/glfw/)
add_subdirectory(dependencies/glad/)
add_subdirectory(dependencies/glm/)
add_subdirectory(dependencies/json/)
add_subdirectory(dependencies/soloud/)

add_library(imgui STATIC
    dependencies/imgui/imgui.cpp
    dependencies/imgui/imgui_demo.cpp
    dependencies/imgui/imgui_draw.cpp
    dependencies/imgui/imgui_tables.cpp
    dependencies/imgui/imgui_widgets.cpp
    dependencies/imgui/backends/imgui_impl_glfw.cpp
    dependencies/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui PRIVATE glfw)
target_include_directories(imgui PUBLIC dependencies/imgui)
target_include_directories(imgui PUBLIC dependencies/imgui/backends)

if (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -g")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -g")
    if (NOT PANCAKE_ASAN)
        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    endif()
endif()

include_directories(include)
include_directories(dependencies)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS src/*.cpp)
add_library(${PROJECT_NAME} ${source_files})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

if (PANCAKE_ASAN)
    target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address -fno-omit-frame-pointer -g)
    target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_include_directories(${PROJECT_NAME} PUBLIC dependencies/)

target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json)
target_link_libraries(${PROJECT_NAME} PUBLIC imgui)
target_link_libraries(${PROJECT_NAME} PUBLIC soloud)

if (PANCAKE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if (PANCAKE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
