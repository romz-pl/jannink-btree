
set( LIB_NAME jannink-btree )

add_library( ${LIB_NAME} STATIC
    btree.cpp
    core.cpp
    key.cpp
    node.cpp
    pool.cpp
    search.cpp
)

target_include_directories( ${LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src )

target_compile_options( ${LIB_NAME} PRIVATE ${ROMZ_CXX_FLAGS} )


