
add_subdirectory(Framework)

add_subdirectory(DatetimeServer)

aux_source_directory(. CURRENT_SRC)
add_executable(Application ${CURRENT_SRC})
target_link_libraries(Application PUBLIC
    Framework)
