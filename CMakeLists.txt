cmake_minimum_required(VERSION 3.14)

project(pancake)

add_subdirectory(dependencies/glfw/)
add_subdirectory(dependencies/glad/)
add_subdirectory(dependencies/glm/)
add_subdirectory(dependencies/soloud/)
add_subdirectory(dependencies/json/)

if (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -g")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    set(CMAKE_VERBOSE_MAKEFILE ON)
else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -Wall -g")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -fsanitize=address")
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

include_directories(include)
include_directories(dependencies)

file(GLOB_RECURSE source_files src/*.cpp)
add_library(${PROJECT_NAME} ${source_files})

target_include_directories(${PROJECT_NAME} PUBLIC glad)
target_include_directories(${PROJECT_NAME} PUBLIC glfw)
target_include_directories(${PROJECT_NAME} PUBLIC glm)
target_include_directories(${PROJECT_NAME} PUBLIC soloud)
target_include_directories(${PROJECT_NAME} PUBLIC nlohmann_json)
target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_include_directories(${PROJECT_NAME} PUBLIC dependencies/)

target_link_libraries(${PROJECT_NAME} PUBLIC glad)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)
target_link_libraries(${PROJECT_NAME} PUBLIC glm)
target_link_libraries(${PROJECT_NAME} PUBLIC soloud)
target_link_libraries(${PROJECT_NAME} PUBLIC nlohmann_json)