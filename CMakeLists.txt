cmake_minimum_required(VERSION 3.20)

project(CppCMakeTemplate)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

enable_testing()

find_package(GTest CONFIG REQUIRED) 

add_executable(main main.cpp)

