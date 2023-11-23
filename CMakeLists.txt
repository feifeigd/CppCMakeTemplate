cmake_minimum_required(VERSION 3.20)

project(CppCMakeTemplate C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

enable_testing()

find_package(asio CONFIG REQUIRED)
find_package(cppcoro CONFIG REQUIRED)   # target_link_libraries(main PRIVATE cppcoro)
find_package(GTest CONFIG REQUIRED) 

aux_source_directory( . SRC)
add_executable(main ${SRC})

add_subdirectory(asio)
add_subdirectory(concurrency)
add_subdirectory(concept)
