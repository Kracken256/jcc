cmake_minimum_required(VERSION 3.15)
project(chip-descriptor-lang-compiler)


file(GLOB_RECURSE SOURCES "*.go")

add_custom_target(cdlc ALL
        COMMAND go build -o ${CMAKE_CURRENT_BINARY_DIR}/cdlc ${SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        SOURCES ${SOURCES}
        )

add_custom_command(TARGET cdlc   POST_BUILD
        COMMAND strip ${CMAKE_CURRENT_BINARY_DIR}/cdlc
        # move
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cdlc ${CMAKE_CURRENT_BINARY_DIR}/../../cdlc
        )