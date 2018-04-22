#include <cassert>
#include <cstring>
#include "core.h"

//
//
//
Core::Core( std::uint32_t pool_size )
    : m_pool( pool_size )
    , m_root( nullptr )
    , m_leaf( nullptr )
    , m_height( 0 )
    , m_the_key( 0 )
    , m_the_data( nullptr )
    // , branch.split( nullptr )
{

    branch.split = nullptr;

    // set up the first leaf node
    m_leaf = m_pool.get_free_node();

    // the root is initially the leaf
    m_root = m_leaf;

    m_root->set_flag( Node::isLEAF );
    m_root->set_flag( Node::isROOT );
    m_root->set_flag( Node::FEWEST );

    set_fun_data( "0" );


#ifdef DEBUG
    fprintf(stderr, "INIT:  B+tree of fanout %d.\n", m_fanout );
    show_btree();
    show_node( m_root );
#endif

}

//
//
//
Core::~Core()
{
#ifdef DEBUG
    fprintf(stderr, "FREE:  B+tree at %10p.\n", (void *) this);
#endif

}



// #define getfunkey B->theKey
Key Core::get_fun_key( ) const
{
    return m_the_key;
}

// #define getfundata B->theData
data_type Core::get_fun_data( ) const
{
    return m_the_data;
}

// #define setfunkey(v) (B->theKey = (v))
void Core::set_fun_key( Key v )
{
    m_the_key = v;
}

// #define setfundata(v) (B->theData = strdup(v))
// void setfundata( Tree* B, dataT v )
void Core::set_fun_data( const char* v )
{
    m_the_data = strdup( v );
}

// #define inctreeheight B->height++
void Core::inc_tree_height( )
{
    m_height++;
}

// #define dectreeheight B->height--
void Core::dec_tree_height( )
{
    assert( m_height > 0 );
    m_height--;
}


// #define getsplitpath B->branch.split
Node* Core::get_split_path( ) const
{
    return branch.split;
}

// #define setsplitpath(v) (B->branch.split = (v))
void Core::set_split_path( Node* v )
{
    branch.split = v;
}

// #define getmergepath B->branch.merge
Node* Core::get_merge_path( ) const
{
    return branch.merge;
}

// #define setmergepath(v) (B->branch.merge = (v))
void Core::set_merge_path( Node* v )
{
    branch.merge = v;
}

//
// fill a free data node with a key and associated data
// can add data parameter
//
Node* Core::get_data_node( Key key )
{
    Node* newNode = m_pool.get_free_node( );
    Key* value = ( Key * ) &newNode->data;

    // can add code to fill node
    *value = key;

    return newNode;
}

