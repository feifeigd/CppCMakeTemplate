
aux_source_directory(. CURRENT_SRC)
add_executable(Application ${CURRENT_SRC})
target_link_libraries(Application PUBLIC
    asio::asio)
