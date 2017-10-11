cmake_minimum_required(VERSION 3.8)
project(HashExtensivel)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/")

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES Trabalho3_ED2.c)

add_executable(HashExtensivel ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME})