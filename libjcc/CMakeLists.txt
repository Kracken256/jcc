cmake_minimum_required(VERSION 3.15)
project(libjcc)

set(CMAKE_CXX_STANDARD 20)


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

add_library(libjcc STATIC ${SOURCES} ${HEADERS})
target_include_directories(libjcc PRIVATE include)
target_compile_options(libjcc PRIVATE -O3 -Wall -Wextra -Wpedantic )
target_link_libraries(libjcc PRIVATE crypto gmp)
set_target_properties(libjcc PROPERTIES OUTPUT_NAME jcc)

install(TARGETS libjcc DESTINATION /usr/local/lib)
install(DIRECTORY include/ DESTINATION /usr/local/include/jcc)

file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(test-${TEST_NAME} ${TEST_SOURCE} ${SOURCES})
    target_include_directories(test-${TEST_NAME} PRIVATE include)
    target_link_libraries(test-${TEST_NAME} PRIVATE -static-libgcc -static-libstdc++ ${CRYPTO_LIB} ${GMP_LIB} )
    target_compile_options(test-${TEST_NAME} PRIVATE -O3 -Wall -Wextra -Wpedantic )
    add_custom_command(TARGET test-${TEST_NAME} POST_BUILD
        COMMAND mkdir -p ${CMAKE_SOURCE_DIR}/bin/tests
        COMMAND mv $<TARGET_FILE:test-${TEST_NAME}> ${CMAKE_SOURCE_DIR}/bin/tests/${TEST_NAME})
endforeach()
