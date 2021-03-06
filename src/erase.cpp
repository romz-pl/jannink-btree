#include <cassert>
#include "erase.h"

//
//
//
Erase::Erase( std::uint32_t pool_size )
    : Insert( pool_size )
{

}



//~~~~~~~~~~~~~~~~~~~~~   top level delete call   ~~~~~~~~~~~~~~~~~~~~~
//
//    The recursive call for deletion carries 5 additional parameters
//    which may be needed to rebalance the B+tree when removing the key.
//    These parameters are:
//        1. immediate left neighbor of the current node
//        2. immediate right neighbor of the current node
//        3. the anchor of the current node and left neighbor
//        4. the anchor of the current node and right neighbor
//        5. the parent of the current node
//
//    All of these parameters are simple to calculate going along the
//    recursive path to the leaf nodes and the point of key deletion.
//    At that time, the algorithm determines which node manipulations
//    are most efficient, that is, cause the least rearranging of data,
//    and minimize the need for non-local key manipulation.
//
void Erase::erase( Key key )
{

#ifdef DEBUG
    fprintf(stderr, "DELETE:  key %d.\n", key.get_value() );
#endif

    // set deletion key
    set_fun_key( key );
    set_merge_path( nullptr );

    Node* new_node = descend_balance( m_root, nullptr, nullptr, nullptr, nullptr, nullptr );

    if( new_node && new_node->is_node() )
    {
        // remove root when superfluous
        collapse_root( m_root, new_node );
    }
}


//
// remove old root node
//
void Erase::collapse_root( Node* old_root, Node* new_root )
{

#ifdef DEBUG
    fprintf(stderr, "Erase::collapse_root:  old %d, new %d.\n", get_node_number( old_root ), get_node_number( new_root ) );
    show_node( old_root );
    show_node( new_root );
#endif

    m_root = new_root;
    new_root->set_flag( Node::isROOT );
    m_pool.put_free_node( old_root );

    // the height of the tree decreases
    dec_tree_height( );
}


//
// recurse down and balance back up
//
Node* Erase::descend_balance( Node* curr, Node* left, Node* right, Node* l_anc, Node* r_anc, Node* parent )
{
    Node* new_me = nullptr;

    if( !curr->is_few() )
    {
        set_merge_path( nullptr );
    }
    else if ( get_merge_path() == nullptr )
    {
        // mark which nodes may need rebalancing
        set_merge_path( curr );
    }

    const int slot = get_slot( curr );
    Node *new_node = curr->get_node( slot );
    if( curr->is_internal() )
    {
        Node* my_left = nullptr;
        Node* my_right = nullptr;

        Node* left_anchor = nullptr;
        Node* right_anchor = nullptr;

        // set up next recursion call's parameters
        if( slot == 0 )
        {
            my_left = !left ? nullptr : left->get_last_node();
            left_anchor = l_anc;
        }
        else
        {
            my_left = curr->get_node( slot - 1 );
            left_anchor = curr;
        }

        if( slot == curr->num_entries() )
        {
            my_right = !right ? nullptr : right->get_first_node();
            right_anchor = r_anc;
        }
        else
        {
            my_right = curr->get_node( slot + 1 );
            right_anchor = curr;
        }
        new_me = descend_balance( new_node, my_left, my_right, left_anchor, right_anchor, curr );
    }
    else if( ( slot > 0 ) && !Key::compare( get_fun_key(), curr->get_key( slot ) ) )
    {
        // a key to be deleted is found
        new_me = new_node;
    }
    else
    {
        // no deletion possible, key not found
        new_me = nullptr;
        set_merge_path( nullptr );
    }

    //~~~~~~~~~~~~~~~~   rebalancing tree after deletion   ~~~~~~~~~~~~~~~~
    //
    //    The simplest B+tree rebalancing consists of the following rules.
    //
    //    If a node underflows:
    //    CASE 1 check if it is the root, and collapse it if it is,
    //    CASE 2 otherwise, check if both of its neighbors are minimum
    //        sized and merge the underflowing node with one of them,
    //    CASE 3 otherwise shift surplus entries to the underflowing node.
    //
    //    The choice of which neighbor to use is optional.  However, the
    //    rebalancing rules that follow also ensure whenever possible
    //    that the merges and shifts which do occur use a neighbor whose
    //    anchor is the parent of the underflowing node.
    //
    //    Cases 3, 4, 5 below are more an optimization than a requirement,
    //    and can be omitted, with a change of the action value in case 6,
    //    which actually corresponds to the third case described above.
    //
    //

    //
    // begin deletion, working upwards from leaves
    //

    if( new_me != nullptr )
    {
        /* this node removal doesn't consider duplicates */
        /* removes one of two */
        remove_entry( curr, slot + ( new_me != new_node ) );
    }

#ifdef DEBUG
    fprintf(stderr, "DELETE:  slot %d, node %d.\n", slot, get_node_number( new_me ) );
    show_node( curr);
#endif

    if( get_merge_path() == nullptr )
    {
        new_node = nullptr;
    }
    else
    {
        // tree rebalancing rules for node merges and shifts
        const int notleft = !left;
        const int notright = !right;

        // only used when defined
        //assert( left != nullptr );
        //assert( right != nullptr );
        //const int fewleft = left->is_few();
        //const int fewright = right->is_few();

#define fewleft (left->is_few())
#define fewright (right->is_few())


        if( notleft && notright )
        {
            // CASE 1:  prepare root node (curr) for removal
            // check if B+tree has become empty
            const int test = curr->is_leaf();
            new_node = test ? nullptr : curr->get_first_node();
        }
        else if( ( notleft || fewleft ) && ( notright || fewright ) )
        {
            // CASE 2:  the merging of two nodes is a must
            const int test = !( l_anc == parent );
            new_node = test ? merge( curr, right, r_anc ) : merge( left, curr, l_anc );
        }
        else if( !notleft && fewleft && !notright && !fewright )
        {
            // CASE 3: choose the better of a merge or a shift
            const int test = !( r_anc == parent ) && ( curr == get_merge_path() );
            new_node = test ? merge( left, curr, l_anc ) : shift( curr, right, r_anc );
        }
        else if( !notleft && !fewleft && !notright && fewright )
        {
            // CASE 4: also choose between a merge or a shift
            const int test = !( l_anc == parent ) && ( curr == get_merge_path() );
            new_node = test ? merge( curr, right, r_anc ) : shift( left, curr, l_anc );
        }
        /* CASE 5: choose the more effective of two shifts */
        else if( l_anc == r_anc )
        {
            /* => both anchors are the parent */
            const int test = (left->num_entries() <= right->num_entries());
            new_node = test ? shift( curr, right, r_anc) : shift( left, curr, l_anc );
        }
        else
        {
            // CASE 6: choose the shift with more local effect
            // if omitting cases 3,4,5 use below
            const int test = ( l_anc == parent );        /* test = (!notleft && !fewleft); */
            new_node = test ? shift( left, curr, l_anc ) : shift( curr, right, r_anc );
        }
    }

    return new_node;
}


//
// remove key and pointer from node
//
void Erase::remove_entry( Node* curr, int slot )
{
    // return deleted node to free list
    m_pool.put_free_node( curr->get_node( slot ) );

    for( int x = slot; x < curr->num_entries(); x++ )
    {
        // adjust node with removed key
        curr->pull_entry( x, 1);
    }

    curr->dec_entries();

    // keep flag information up to date
    curr->clr_flag( Node::isFULL );
    if( curr->is_root() )
    {
        if( curr->num_entries() == 1 )
        {
            curr->set_flag( Node::FEWEST );
        }
    }
    else if( curr->num_entries() == m_min_fanout )
    {
        curr->set_flag( Node::FEWEST );
    }
}


//
// merge a node pair & set emptied node up for removal
//
Node* Erase::merge( Node* left, Node* right, Node* anchor )
{

#ifdef DEBUG
    fprintf(stderr, "MERGE:  left %d, right %d.\n", get_node_number(left), get_node_number(right));
    show_node( left);
    show_node( right);
#endif

    if( left->is_internal() )
    {
        left->inc_entries();

        // copy key separating the nodes
        set_fun_key( right->get_key( 1 ) );

        // defined but maybe just deleted
        const int z = get_slot( anchor );

        // needs the just calculated key
        // set slot to delete in anchor
        set_fun_key( anchor->get_key( z ) );
        left->set_entry( left->num_entries(), get_fun_key( ), right->get_first_node() );
    }
    else
    {
        left->set_next_node( right->get_next_node() );
    }

    for( int x = left->num_entries() + 1, y = 1; y <= right->num_entries(); x++, y++ )
    {
        left->inc_entries();
        // transfer entries to left node
        right->xfer_entry( y, left, x );
    }

    if( left->num_entries() > m_min_fanout )
    {
        left->clr_flag( Node::FEWEST );
    }

    if( left->num_entries() == m_fanout )
    {
        // never happens in even size nodes
        left->set_flag( Node::isFULL );
    }

    if( get_merge_path() == left || get_merge_path() == right )
    {
        // indicate rebalancing is complete
        set_merge_path( nullptr );
    }

    return right;
}


//
// shift entries in a node pair & adjust anchor key value
//
Node* Erase::shift( Node* left, Node* right, Node* anchor )
{

#ifdef DEBUG
    fprintf(stderr, "SHIFT:  left %d, right %d.\n", get_node_number(left), get_node_number(right));
    show_node( left);
    show_node( right);
#endif

    const int i = left->is_internal();
    if( left->num_entries() < right->num_entries() )
    {
        // shift entries to left
        int y = ( right->num_entries() - left->num_entries() ) >> 1;
        int x = left->num_entries() + y;

        // set new anchor key value
        set_fun_key( right->get_key( y + 1 - i ) );

        // find slot in anchor node
        int z = get_slot( anchor );
        if( i )
        {
            // move out old anchor value
            // adjust for shifting anchor
            right->dec_entries();
            left->inc_entries();
            left->set_entry( left->num_entries(), anchor->get_key( z ), right->get_first_node() );
            right->set_first_node( right->get_node( y + 1 - i ) );
        }

        right->clr_flag( Node::isFULL );

        // set new anchor value
        anchor->set_key( z, get_fun_key() );

        for( z = y, y -= i; y > 0; y--, x-- )
        {
            // adjust entry count
            right->dec_entries();
            left->inc_entries();

            // transfer entries over
            right->xfer_entry( y, left, x );
        }

        for ( x = 1; x <= right->num_entries(); x++ )
        {
            // adjust reduced node
            right->pull_entry( x, z);
        }
    }
    else
    {
        // shift entries to right
        int y = ( left->num_entries() - right->num_entries() ) >> 1;
        int x = left->num_entries() - y + 1;

        for( int z = right->num_entries(); z > 0; z-- )
        {
            // adjust increased node
            right->push_entry( z, y );
        }

        // set new anchor key value
        set_fun_key( left->get_key( x ) );
        int z = get_slot( anchor ) + 1;
        if( i )
        {
            left->dec_entries();
            right->inc_entries();
            right->set_entry( y, anchor->get_key( z ), right->get_first_node() );
            right->set_first_node( left->get_node( x ) );
        }

        left->clr_flag( Node::isFULL );
        anchor->set_key( z, get_fun_key() );
        for( x = left->num_entries() + i, y -= i; y > 0; y--, x-- )
        {
            left->dec_entries();
            right->inc_entries();

            // transfer entries over
            left->xfer_entry( x, right, y );
        }
    }

    if( left->num_entries() == m_min_fanout )
    {
        // adjust node flags
        left->set_flag( Node::FEWEST );
    }
    else
    {
        // never happens in 2-3+trees
        left->clr_flag( Node::FEWEST );
    }

    if( right->num_entries() == m_min_fanout )
    {
        right->set_flag( Node::FEWEST );
    }
    else
    {
        // never happens in 2-3+trees
        right->clr_flag( Node::FEWEST );
    }

    set_merge_path( nullptr );

#ifdef DEBUG
    show_node( left);
    show_node( right);
#endif

    return nullptr;
}
