cmake_minimum_required(VERSION 3.15)
project(jcc-language-compiler)

file(GLOB_RECURSE SOURCES "source/*.cpp")
file(GLOB_RECURSE HEADERS "source/*.h" "source/*.hpp")

add_executable(jcc ${SOURCES} ${HEADERS})
target_include_directories(jcc PRIVATE include)
target_link_libraries(jcc PRIVATE crypto gmp)
target_compile_options(jcc PRIVATE -O3 -Wall -Wextra -Wpedantic )

add_custom_command(TARGET jcc POST_BUILD
        COMMAND mv $<TARGET_FILE:jcc> ${CMAKE_SOURCE_DIR}/jcc)