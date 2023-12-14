cmake_minimum_required(VERSION 3.15)
project(jcc-language-compiler)

set(CMAKE_CXX_STANDARD 20)

set(BUILD_RELEASE off)

# Find libcrypto path
find_library(CRYPTO_LIB libcrypto.a)
if (NOT CRYPTO_LIB)
    message(FATAL_ERROR "libcrypto not found")
endif ()

find_library(GMP_LIB libgmp.a)
if (NOT GMP_LIB)
    message(FATAL_ERROR "libgmp not found")
endif ()

message(STATUS "libcrypto found at ${CRYPTO_LIB}")
message(STATUS "libgmp found at ${GMP_LIB}")

file(GLOB_RECURSE SOURCES "source/*.cpp")
file(GLOB_RECURSE HEADERS "source/*.h" "source/*.hpp")

add_executable(jcc ${SOURCES} ${HEADERS})
target_include_directories(jcc PRIVATE include)
target_compile_options(jcc PRIVATE -O3 -Wall -Wextra -Wpedantic -Werror)
target_link_libraries(jcc PRIVATE -static-libgcc -static-libstdc++ ${CRYPTO_LIB} ${GMP_LIB} -static )

if (BUILD_RELEASE MATCHES "on")
    add_custom_command(TARGET jcc POST_BUILD
            COMMAND strip $<TARGET_FILE:jcc>
            COMMAND upx $<TARGET_FILE:jcc>
            COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/bin
            COMMAND mv $<TARGET_FILE:jcc> ${CMAKE_SOURCE_DIR}/bin/jcc)
else ()
    add_custom_command(TARGET jcc POST_BUILD
            COMMAND mv $<TARGET_FILE:jcc> ${CMAKE_SOURCE_DIR}/jcc)
endif ()