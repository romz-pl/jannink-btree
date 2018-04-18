#include "gtest/gtest.h"
#include "btree.h"


TEST( btree, init_free )
{
    Tree *tree;
    tree = btree_init( ARRAY_SIZE, NODE_SIZE / sizeof( Entry ), compareKeys );

    EXPECT_NO_THROW( btree_free( tree ) );
}

