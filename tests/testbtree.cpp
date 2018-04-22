#include "gtest/gtest.h"
#include "btree.h"
#include <random>

TEST( btree, constuction )
{
    const std::uint32_t pool_size = 100;

    ASSERT_NO_THROW( Tree{ pool_size } );
}

TEST( btree, search_empty )
{
    const std::uint32_t pool_size = 100;
    Tree tree( pool_size );
    Node* nb = tree.search( Key( 1 ) );
    ASSERT_TRUE( !nb );
}

TEST( btree, one_item )
{
    const std::uint32_t pool_size = 100;
    const Key k( 17 );

    Tree tree( pool_size );

    ASSERT_NO_THROW( tree.insert( k ) );
    Node* na = tree.search( k );
    ASSERT_TRUE( na );

    tree.erase( k );
    Node* nb = tree.search( k );
    ASSERT_TRUE( !nb );
}

TEST( btree, more_items )
{
    const std::uint32_t pool_size = 1000;
    const int item_no = 200;
    Tree tree( pool_size );

    for( int i = 0; i < item_no; i++ )
    {
        ASSERT_NO_THROW( tree.insert( Key( i ) ) );
    }


    for( int i = 0; i < item_no; i++ )
    {
        Node* na = tree.search( Key( i ) );
        ASSERT_TRUE( na );
    }

    for( int i = 0; i < item_no; i++ )
    {
        tree.erase( Key( i ) );
    }

}

TEST( btree, reverse_erase )
{
    const std::uint32_t pool_size = 1000;
    const int item_no = 200;
    Tree tree( pool_size );

    for( int i = 0; i < item_no; i++ )
    {
        ASSERT_NO_THROW( tree.insert( Key( i ) ) );
    }

    for( int i = 0; i < item_no; i++ )
    {
        Node* na = tree.search( Key( i ) );
        ASSERT_TRUE( na );
    }

    for( int i = item_no - 1; i >= 0 ; i-- )
    {
        tree.erase( Key( i ) );
    }

}

TEST( btree, insert_search_delete )
{
    const int item_no = 9000;
    const unsigned seed = 12345;
    std::set< Key > sset;
    std::srand( seed );
    const std::uint32_t pool_size = 50000;

    Tree tree( pool_size );

    for( int i = 0; i < item_no; i++ )
    {
        const Key key = Key( std::rand() );
        // std::cout << key.get_value() << " " << std::flush;
        sset.insert( key );
        ASSERT_NO_THROW( tree.insert( key ) );
    }

    for( auto v : sset )
    {
        Node* na = tree.search( v );
        ASSERT_TRUE( na );
    }


    for( auto v : sset )
    {
        tree.erase( v );
    }

    for( auto v : sset )
    {
        Node* na = tree.search( v );
        ASSERT_TRUE( !na );
    }
}

TEST( btree, insert_random )
{
    std::set< Key > sset;
    const std::uint32_t pool_size = 20000;
    Tree tree( pool_size );

    const double threshold = 0.6;
    const int iter_no = 2000;

    //std::random_device rd; // only used once to initialise (seed) engine
    //std::mt19937 rng( rd() ); // random-number engine used (Mersenne-Twister in this case)
    std::mt19937 rng;
    std::uniform_real_distribution<> dist_real( 0, 1 );

    std::uniform_int_distribution< int > dist_int( 0, iter_no/2 );

    for( int i = 0; i < iter_no; i++ )
    {
        const Key key( dist_int( rng ) );
        const double x = dist_real( rng );

        if( x > threshold )
        {
            sset.insert( key );
            ASSERT_NO_THROW( tree.insert( key ) );
        }
        else
        {
            sset.erase( key );
            tree.erase( key );
        }

        // std::cout<< sset.size() << "\n" << std::flush;

        for( auto v : sset )
        {
            Node* na = tree.search( v );
            ASSERT_TRUE( na );
        }

    }
}

TEST( btree, erase_random )
{
    std::set< Key > sset;
    const std::uint32_t pool_size = 30000;
    Tree tree( pool_size );

    const int iter_no = 5000;

    //std::random_device rd; // only used once to initialise (seed) engine
    // std::mt19937 rng( rd() ); // random-number engine used (Mersenne-Twister in this case)
    std::mt19937 rng;

    std::uniform_int_distribution< int > dist_int( 0, iter_no/2 );

    for( int i = 0; i < iter_no; i++ )
    {
        const Key key( dist_int( rng ) );
        sset.insert( key );
        ASSERT_NO_THROW( tree.insert( key ) );
    }

    for( auto v : sset )
    {
        Node* na = tree.search( v );
        ASSERT_TRUE( na );
    }

    while( !sset.empty() )
    {
        const Key key( dist_int( rng ) );
        sset.erase( key );
        tree.erase( key );
    }
}

