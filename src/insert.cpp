#include<cassert>
#include "insert.h"

//
//
//
Insert::Insert( std::uint32_t pool_size )
    : Search( pool_size )
{

}


//
// top level insert call
//
void Insert::insert( Key key )
{
#ifdef DEBUG
    fprintf( stderr, "Insert::insert:  key %d.\n", key.get_value() );
#endif

    // set insertion key
    set_fun_key( key );

    // a node containing data
    set_fun_data( "data" );

    set_split_path( nullptr );

    // insertion point search from root
    Node* new_node = descend_split( m_root );


    // indicates the root node has split
    if( new_node != get_split_path() )
    {
        make_new_root( m_root, new_node );
    }
}


//
// recurse down and split back up
//
Node* Insert::descend_split( Node* curr )
{
    Node* new_me = nullptr;

    if( !curr->is_full() )
    {
        set_split_path( nullptr );
    }
    else if( get_split_path() == nullptr )
    {
        // indicates where nodes must split
        set_split_path( curr );
    }

    // is null only if the root is empty
    const int slot = get_slot( curr );

    // continue recursion to leaves
    if( curr->is_internal() )
    {
        new_me = descend_split( curr->get_node( slot ) );
    }
    else if( ( slot > 0 ) && !Key::compare( get_fun_key( ), curr->get_key( slot ) ) )
    {
        // this code discards duplicates
        new_me = nullptr;
        set_split_path( nullptr );
    }
    else
    {
        // an insertion takes place
        new_me = get_data_node( get_fun_key( ) );
    }

    // assume no node splitting necessary
    Node* new_node = nullptr;

    if( new_me != nullptr )
    {
        // insert only where necessary
        if( get_split_path() != nullptr )
        {
            // a sibling node is prepared
            new_node = split( curr );
        }
        insert_entry( curr, slot, new_node, new_me );
    }

    return new_node;
}

//
// determine location of inserted key
//
void Insert::insert_entry( Node* new_node, const int slot, Node* sibling, Node* down_ptr )
{
    if( sibling == nullptr )
    {
        // no split occurred
        place_entry( new_node, slot + 1, down_ptr );
        new_node->clr_flag( Node::FEWEST );
        return;
    }

    assert( sibling != nullptr );

    // split entries between the two
    const int i = new_node->is_internal();
    // adjustment values
    // const int split = ( i ? m_fanout - get_min_fanout( new_node ) : get_min_fanout( new_node ) );
    const int split = m_fanout / 2;
    const int j = ( slot != split );
    const int k = ( slot >= split );

    for( int x = split + k + j * i, y = 1; x <= m_fanout; x++, y++ )
    {
        // copy entries to sibling
        new_node->xfer_entry( x, sibling, y );
        new_node->dec_entries();
        sibling->inc_entries();
    }

    if( sibling->num_entries() == m_fanout )
    {
        // only ever happens in 2-3+trees
        sibling->set_flag( Node::isFULL );
    }

    if( i )
    {
        // set first pointer of internal node
        if( j )
        {
            sibling->set_first_node( new_node->get_node( split + k ) );
            new_node->dec_entries();
        }
        else
        {
            sibling->set_first_node( down_ptr );
        }
    }

    if( j )
    {
        // insert new entry into correct spot
        const Key xx = new_node->get_key( split + k );
        if( k )
        {
            place_entry( sibling, slot - split + 1 - i, down_ptr );
        }
        else
        {
            place_entry( new_node, slot + 1, down_ptr );
        }
        // set key separating nodes
        set_fun_key( xx );
    }
    else if( !i )
    {
        place_entry( sibling, 1, down_ptr );
    }

    // adjust node flags
    new_node->clr_flag( Node::isFULL );

    if( new_node->num_entries() == m_min_fanout )
    {
        // never happens in even size nodes
        new_node->set_flag( Node::FEWEST );
    }

    if( sibling->num_entries() > m_min_fanout )
    {
        sibling->clr_flag( Node::FEWEST );
    }

#ifdef DEBUG
    fprintf(stderr, "INSERT:  slot %d, node %d.\n", slot, get_node_number(downPtr));
    show_node( newNode);
    show_node( sibling);
#endif

}

//
// place key into appropriate node & slot
//
void Insert::place_entry( Node* new_node, int slot, Node* down_ptr )
{
    for( int x = new_node->num_entries(); x >= slot; x-- )
    {
        // make room for new entry
        new_node->push_entry( x, 1 );
    }

    // place it in correct slot
    new_node->set_entry( slot, get_fun_key(), down_ptr );

    // adjust entry counter
    new_node->inc_entries();

    if( new_node->num_entries() == m_fanout )
    {
        new_node->set_flag( Node::isFULL );
    }
}


//
// split full node and set flags
//
Node* Insert::split( Node* new_node )
{
    Node* sibling = m_pool.get_free_node();

    // set up node flags
    sibling->set_flag( Node::FEWEST );

    if( new_node->is_leaf() )
    {
        sibling->set_flag( Node::isLEAF );

        // adjust leaf pointers
        sibling->set_next_node( new_node->get_next_node() );
        new_node->set_next_node( sibling );
    }

    if( get_split_path() == new_node )
    {
        // no more splitting needed
        set_split_path( nullptr );
    }

    return sibling;
}


//
// build new root node
//
void Insert::make_new_root( Node* old_root, Node* new_node )
{
    m_root = m_pool.get_free_node();

    // old root becomes new root's child
    m_root->set_first_node( old_root );

    // old root's sibling also
    m_root->set_entry( 1, get_fun_key(), new_node );
    m_root->inc_entries();

    old_root->clr_flag( Node::isROOT );
    m_root->set_flag( Node::isROOT );
    m_root->set_flag( Node::FEWEST );
    inc_tree_height();
}

