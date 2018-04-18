cmake_minimum_required (VERSION 3.0)
project (jannink-btree)

SET( ROMZ_CXX_FLAGS -Wall -Wpedantic -Wextra -O2 )

add_subdirectory(src)
add_subdirectory(tests)

