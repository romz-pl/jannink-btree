
set( LIB_NAME jannink-btree )

add_library( ${LIB_NAME} STATIC
    btree.c
)

target_include_directories( ${LIB_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src )

target_compile_options( ${LIB_NAME} PRIVATE ${ROMZ_CXX_FLAGS} )


