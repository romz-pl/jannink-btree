#include "gtest/gtest.h"
#include "btree.h"


TEST( btree, init_free )
{
    Tree B( ARRAY_SIZE, NODE_SIZE / sizeof( Entry ), compare_keys );

    B.insert( 17 );
    Nptr na = B.search( 17 );
    EXPECT_TRUE( na != B.NONODE() );

    btree_delete( &B, 17 );
    Nptr nb = B.search( 17 );
    EXPECT_TRUE( nb == B.NONODE() );
}

TEST( btree, insert_search_delete )
{
    const int item_no = 2000;
    const unsigned seed = 12345;
    std::set< int > sset;
    std::srand( seed );
    Tree B( ARRAY_SIZE, NODE_SIZE / sizeof( Entry ), compare_keys );

    for( int i = 0; i < item_no; i++ )
    {
        const int key = std::rand();
        auto ret = sset.insert( key );
        if( ret.second )
        {
            B.insert( key );
        }
    }

    for( auto v : sset )
    {
        Nptr na = B.search( v );
        EXPECT_TRUE( na != B.NONODE() );
    }

/*    for( auto v : sset )
    {
        btree_delete( &B, v );
    }

    for( auto v : sset )
    {
        Nptr na = B.btree_search( v );
        EXPECT_TRUE( na == B.NONODE() );
    }
    */
}


//int main(void)
//{
//  Tree    *Bplus;
//  int    i, j;

//  Bplus = btree_init(ARRAY_SIZE, NODE_SIZE / sizeof(Entry), compareKeys);
// /*  insert(Bplus,17);
//  insert(Bplus,16);
//  insert(Bplus,15);
//  insert(Bplus,14);
//  insert(Bplus,13);
//  insert(Bplus,12);
//  insert(Bplus,11);
//  insert(Bplus,10);
//  insert(Bplus,9);
//  insert(Bplus,8);
//  insert(Bplus,7);
//  insert(Bplus,6);
//  insert(Bplus,5);
//  insert(Bplus,4);
//  insert(Bplus,3);
//  insert(Bplus,2);
//  insert(Bplus,1);
//  btree_delete(Bplus,1);
//  btree_delete(Bplus,2);
//  btree_delete(Bplus,3);
//  btree_delete(Bplus,4);
//  btree_delete(Bplus,5);
//  btree_delete(Bplus,6);
//  btree_delete(Bplus,7);
//  btree_delete(Bplus,8);
//  btree_delete(Bplus,9);
//  btree_delete(Bplus,10);
//  btree_delete(Bplus,11);
//  btree_delete(Bplus,12);
//  btree_delete(Bplus,13);
//  btree_delete(Bplus,14);
//  btree_delete(Bplus,15);
//  btree_delete(Bplus,16);
//  btree_delete(Bplus,17); */
//  for (i = 0; i < 2048; i++) {
//    j = rand() >> 3 & 255;
//    if (btree_search(Bplus, j) == Bplus->tree - 1) {
//      btree_insert(Bplus, j);
//      fprintf(stderr, "XXX %d, insert %d XXX\n", i, j);
//    }
//    else {
//      btree_delete(Bplus, j);
//      fprintf(stderr, "XXX %d, delete %d XXX\n", i, j);
//    }
//    if (i > 2000)
//      listAllBtreeValues(Bplus);
//  }
//  for (i = 0; i < 256; i++)
//    (void) btree_search(Bplus, i);
//  listAllBtreeValues(Bplus);
//  btree_free(Bplus);

//  return 1;
//}

