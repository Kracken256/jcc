cmake_minimum_required(VERSION 3.15)
project(jcc)

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

set(JCC_LIB ${CMAKE_BINARY_DIR}/libjcc/libjcc.a)

message(STATUS "libcrypto found at ${CRYPTO_LIB}")
message(STATUS "libgmp found at ${GMP_LIB}")

file(GLOB_RECURSE SOURCES "source/*.cpp")
file(GLOB_RECURSE HEADERS "source/*.h" "source/*.hpp")

add_executable(jcc ${SOURCES} ${HEADERS})
add_dependencies(jcc libjcc)
target_include_directories(jcc PRIVATE ../libjcc/include)
target_compile_options(jcc PRIVATE -O3 -Wall -Wextra -Wpedantic -Werror -g -fPIC -Wno-error=unused-function)
target_link_libraries(jcc PRIVATE ${JCC_LIB} ${CRYPTO_LIB} ${GMP_LIB} -static-libgcc -static-libstdc++ -pthread -static)

if (BUILD_RELEASE MATCHES "on")
    add_custom_command(TARGET jcc POST_BUILD
            COMMAND strip $<TARGET_FILE:jcc>
            COMMAND upx $<TARGET_FILE:jcc>)
endif ()
