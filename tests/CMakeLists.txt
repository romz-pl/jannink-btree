set( TEST_NAME testbtree )

add_executable( ${TEST_NAME}
    #testbtree.cpp
    node.cpp
)

target_compile_options( ${TEST_NAME} PRIVATE ${ROMZ_CXX_FLAGS} )

target_include_directories( ${TEST_NAME} PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries( ${TEST_NAME} 
    -fprofile-arcs 
    gtest 
    gtest_main 
    pthread
    jannink-btree 
)

add_test( NAME ${TEST_NAME} COMMAND ${TEST_NAME} )

