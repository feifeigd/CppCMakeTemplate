
file(GLOB_RECURSE DIR_LIB_SRCS *.cpp *.h*)

add_library(Framework ${DIR_LIB_SRCS})
target_link_libraries(Framework PUBLIC asio::asio)
target_include_directories(Framework PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
