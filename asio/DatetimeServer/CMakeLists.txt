
aux_source_directory(. DIR_EXE_SRCS)

add_executable(DatetimeServer ${DIR_EXE_SRCS})

target_link_libraries(DatetimeServer Framework)
