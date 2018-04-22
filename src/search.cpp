#include <cassert>
#include "search.h"

//
//
//
Search::Search( std::uint32_t pool_size )
    : Core( pool_size )
{

}

//
// top level search call
//
Node* Search::search( Key key )
{
#ifdef DEBUG
    fprintf( stderr, "Search::search: key %d.\n", key.get_value() );
#endif

    if( m_root->num_entries() == 0 )
    {
        return nullptr;
    }

    set_fun_key( key );

    // start search from root node
    Node* find_node = descend_to_leaf( m_root );

#ifdef DEBUG
    if( find_node  )
    {
        fprintf( stderr, "Search::search: found on page %d.\n", get_node_number( find_node ) );
    }
    else
    {
        fprintf( stderr, "Search::search: node not found.\n" );
    }
#endif

    return find_node;
}

//
// `recurse' down B+tree
//
Node* Search::descend_to_leaf( Node* curr )
{
    int slot = 0;

    while( curr->is_internal() )
    {
        slot = get_slot( curr );
        curr = curr->get_node( slot );
    }

    assert( curr->is_leaf() );
    slot = get_slot( curr );

    if( ( slot > 0 ) && !Key::compare( get_fun_key( ), curr->get_key( slot ) ) )
    {
        // correct key value found
        return curr;
    }


    return nullptr;
}

//
// find slot for search key
//
int Search::get_slot( Node* curr )
{
    int slot = 0;
    const int entries = curr->num_entries();

    if( entries >= 1 )
    {
        slot = find_key( curr, 1, entries );
    }


#ifdef DEBUG
    fprintf(stderr, "get_slot:  slot %d.\n", slot);
#endif

    return slot;
}

//
// recursive binary search
//
int Search::find_key( Node* curr, int lo, int hi )
{

#ifdef DEBUG
    fprintf(stderr, "find_key:  lo %d, hi %d.\n", lo, hi);
    show_node( curr );
#endif

    if( hi == lo )
    {
        // recursion base case
        const int find_slot = best_match( curr, lo );
        return find_slot;
    }

    assert( hi > lo );

    const int mid = ( lo + hi ) >> 1;
    int find_slot = best_match( curr, mid );
    switch ( find_slot )
    {
        case LOWER:
            // check lower half of range
            // never in 2-3+trees
            find_slot = find_key( curr, lo, mid - 1 );
        break;

        case UPPER:
            // check upper half of range
            find_slot = find_key( curr, mid + 1, hi );
        break;

        default:
            // Do nothing
        break;
    }
    return find_slot;
}


//
// comparison of key with a target key slot
//
int Search::best_match( Node* curr, const int slot )
{
    const int diff = Key::compare( get_fun_key( ), curr->get_key( slot ) );
    if( diff < 0 )
    {
        if( slot == 1 )
        {
            return ( slot - 1 );
        }

        //
        // also check the previous slot
        //
        const int comp = Key::compare( get_fun_key( ), curr->get_key( slot - 1 ) );

        if( comp >= 0 )
        {
            return ( slot - 1 );
        }
        else
        {
            // key must be below in node ordering
            return ( LOWER );
        }
    }
    else
    {
        if( slot == curr->num_entries() )
        {
            return slot;
        }

        //
        // or check following slot
        //
        const int comp = Key::compare( get_fun_key( ), curr->get_key( slot + 1 ) );

        if( comp < 0 )
        {
            return ( slot );
        }
        else if( comp == 0 )
        {
            return ( slot + 1 );
        }
        else
        {
            // key must be above in node ordering
            return ( UPPER );
        }
    }

    assert( 0 ); // Bad key ordering on node "curr"
    return -1;
}
