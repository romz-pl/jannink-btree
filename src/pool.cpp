#include <stdexcept>

#include "pool.h"

//
//
//
Pool::Pool( std::uint32_t size )
    : m_tree( nullptr )
    , m_pool( nullptr )
{
    // node memory block
    m_tree = ( Node* )malloc( size * sizeof( Node ) );

    // start a list of free nodes
    set_first_free_node( m_tree );

    Node* n = get_first_free_node();
    for( std::uint32_t i = 0; i < size; i++ )
    {
        n->clear_flags();
        n->clear_entries();

        // insert node into free node list
        n->set_next_node( n + 1 );
        n++;
    }
    --n;
    // indicates end of free node list
    n->set_next_node( nullptr );
}

//
//
//
Pool::~Pool()
{
    free( m_tree );
}

//
// take a free B+tree node from the pool
//
Node* Pool::get_free_node( )
{
    Node* newNode = get_first_free_node( );

    if( newNode == nullptr )
    {
          // can't recover from this
          throw std::runtime_error( "Out of tree nodes." );
    }

    // adjust free node list
    set_first_free_node( newNode->get_next_node( ) );

    // remove node from list
    newNode->set_next_node( nullptr );

    return newNode;
}

//
// return a deleted B+tree node to the pool
//
void Pool::put_free_node( Node* self )
{
    self->clear_flags();
    self->clear_entries();

    // add node to list
    self->set_next_node(  get_first_free_node() );

    // set it to be list head
    set_first_free_node( self );
}

//
//
//
Node* Pool::get_first_free_node( ) const
{
    return m_pool;
}

//
//
//
void Pool::set_first_free_node( Node* v )
{
    m_pool = v;
}
