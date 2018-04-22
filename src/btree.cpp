#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <cassert>
#include "btree.h"


// causes printing of node information
#define DEBUG 1
#undef DEBUG


//
//
//
Tree::Tree( int pool_size )
    : m_pool_store( pool_size )
    , m_root( nullptr )
    , m_leaf( nullptr )
    , m_height( 0 )
    , m_the_key( 0 )
    , m_the_data( nullptr )
    // , branch.split( nullptr )
{

    branch.split = nullptr;

    // set up the first leaf node
    m_leaf = get_free_node();

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
Tree::~Tree()
{
#ifdef DEBUG
    fprintf(stderr, "FREE:  B+tree at %10p.\n", (void *) this);
#endif

}



// #define getfunkey B->theKey
Key Tree::get_fun_key( ) const
{
    return m_the_key;
}

// #define getfundata B->theData
data_type Tree::get_fun_data( ) const
{
    return m_the_data;
}

// #define setfunkey(v) (B->theKey = (v))
void Tree::set_fun_key( Key v )
{
    m_the_key = v;
}

// #define setfundata(v) (B->theData = strdup(v))
// void setfundata( Tree* B, dataT v )
void Tree::set_fun_data( const char* v )
{
    m_the_data = strdup( v );
}

// #define inctreeheight B->height++
void Tree::inc_tree_height( )
{
    m_height++;
}

// #define dectreeheight B->height--
void Tree::dec_tree_height( )
{
    m_height--;
}

// #define gettreeheight B->height
int Tree::get_tree_height( ) const
{
    return m_height;
}

// #define getsplitpath B->branch.split
Node* Tree::get_split_path( ) const
{
    return branch.split;
}

// #define setsplitpath(v) (B->branch.split = (v))
void Tree::set_split_path( Node* v )
{
    branch.split = v;
}

// #define getmergepath B->branch.merge
Node* Tree::get_merge_path( ) const
{
    return branch.merge;
}

// #define setmergepath(v) (B->branch.merge = (v))
void Tree::set_merge_path( Node* v )
{
    branch.merge = v;
}

//
// top level search call
//
Node* Tree::search( Key key )
{
#ifdef DEBUG
    fprintf( stderr, "Tree::search: key %d.\n", key.get_value() );
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
        fprintf( stderr, "Tree::search: found on page %d.\n", get_node_number( find_node ) );
    }
    else
    {
        fprintf( stderr, "Tree::search: node not found.\n" );
    }
#endif

    return find_node;
}

//
// `recurse' down B+tree
//
Node* Tree::descend_to_leaf( Node* curr )
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
int Tree::get_slot( Node* curr )
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
int Tree::find_key( Node* curr, int lo, int hi )
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
int Tree::best_match( Node* curr, const int slot )
{
    int find_slot = ERROR;

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
            find_slot = slot - 1;
        }
        else
        {
            // key must be below in node ordering
            find_slot = LOWER;
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
            find_slot = slot;
        }
        else if( comp == 0 )
        {
            find_slot = slot + 1;
        }
        else
        {
            // key must be above in node ordering
            find_slot = UPPER;
        }
    }

    assert( find_slot != ERROR ); // Bad key ordering on node "curr"
    return find_slot;
}


//
// top level insert call
//
void Tree::insert( Key key )
{
#ifdef DEBUG
    fprintf( stderr, "Tree::insert:  key %d.\n", key.get_value() );
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
Node* Tree::descend_split( Node* curr )
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
void Tree::insert_entry( Node* new_node, const int slot, Node* sibling, Node* down_ptr )
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
void Tree::place_entry( Node* new_node, int slot, Node* down_ptr )
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
Node* Tree::split( Node* new_node )
{
    Node* sibling = get_free_node();

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
void Tree::make_new_root( Node* old_root, Node* new_node )
{
    m_root = get_free_node();

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
void Tree::erase( Key key )
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
void Tree::collapse_root( Node* old_root, Node* new_root )
{

#ifdef DEBUG
    fprintf(stderr, "Tree::collapse_root:  old %d, new %d.\n", get_node_number( old_root ), get_node_number( new_root ) );
    show_node( old_root );
    show_node( new_root );
#endif

    m_root = new_root;
    new_root->set_flag( Node::isROOT );
    put_free_node( old_root );

    // the height of the tree decreases
    dec_tree_height( );
}


//
// recurse down and balance back up
//
Node* Tree::descend_balance( Node* curr, Node* left, Node* right, Node* l_anc, Node* r_anc, Node* parent )
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
void Tree::remove_entry( Node* curr, int slot )
{
    // return deleted node to free list
    put_free_node( curr->get_node( slot ) );

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
Node* Tree::merge( Node* left, Node* right, Node* anchor )
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
Node* Tree::shift( Node* left, Node* right, Node* anchor )
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




//
// take a free B+tree node from the pool
//
Node* Tree::get_free_node( )
{
    return m_pool_store.get_free_node();
}


//
// return a deleted B+tree node to the pool
//
void Tree::put_free_node( Node* self )
{
    m_pool_store.put_free_node( self );

}

//
// fill a free data node with a key and associated data
// can add data parameter
//
Node* Tree::get_data_node( Key key )
{
    Node* newNode = get_free_node( );
    Key* value = ( Key * ) &newNode->data;

    // can add code to fill node
    *value = key;

    return newNode;
}

#ifdef DEBUG

//
// B+tree node printer
//
void Tree::show_node( Node* n ) const
{

    fprintf(stderr, "------------------------------------------------\n");
    fprintf(stderr, "| node %6d                 ", get_node_number(n));
    fprintf(stderr, "  magic    %4x  |\n", n->get_flags() & Node::MASK);
    fprintf(stderr, "|- - - - - - - - - - - - - - - - - - - - - - - |\n");
    fprintf(stderr, "| flags   %1d%1d%1d%1d ", n->is_few(), n->is_full(), n->is_root(), n->is_leaf());
    fprintf(stderr, "| keys = %5d ", n->num_entries());
    fprintf(stderr, "| node = %6d  |\n", get_node_number( n->get_first_node()));
    for( int x = 1; x <= n->num_entries(); x++)
    {
        fprintf(stderr, "| entry %6d ", x);
        fprintf(stderr, "| key = %6d ", n->get_key( x ).get_value() & 0xFFFF);
        fprintf(stderr, "| node = %6d  |\n", get_node_number( n->get_node( x )));
    }
    fprintf(stderr, "------------------------------------------------\n\n");
}

//
// B+tree class variable printer
//
void Tree::show_btree( ) const
{
    fprintf(stderr, "-  --  --  --  --  --  -\n");
    fprintf(stderr, "|  B+tree  %10p  |\n", (void *) this);
    fprintf(stderr, "-  --  --  --  --  --  -\n");
    fprintf(stderr, "|  root        %6d  |\n", get_node_number( m_root ));
    fprintf(stderr, "|  leaf        %6d  |\n", get_node_number( get_leaf() ));
    fprintf(stderr, "|  fanout         %3d  |\n", m_fanout + 1);
    fprintf(stderr, "|  minfanout      %3d  |\n", get_min_fanout( m_root ) + 1);
    fprintf(stderr, "|  height         %3d  |\n", get_tree_height() );
//    fprintf(stderr, "|  freenode    %6d  |\n", get_node_number( get_first_free_node() ));
    fprintf(stderr, "|  theKey      %6d  |\n", get_fun_key().get_value() );
    fprintf(stderr, "|  theData     %6s  |\n", get_fun_data( ));
    fprintf(stderr, "-  --  --  --  --  --  -\n");

    Node* n = m_root;
    while( n != nullptr )
    {
        show_node( n );
        n = n->get_next_node();
    }
}


//
// B+tree data printer
//
void Tree::list_btree_values( Node* n ) const
{
    int slot, prev = -1;
    int num = 0;

    for( slot = 1; ( n != nullptr ) && n->num_entries(); num++ )
    {
        if( n->get_key( slot ).get_value() <= prev)
        {
            fprintf(stderr, "BOMB");
        }
        prev = n->get_key( slot ).get_value();


        fprintf( stderr, "%8d%c", prev, ( num & 7 ? ' ' : '\n' ) );
        if( ++slot > n->num_entries() )
        {
            n = n->get_next_node();
            slot = 1;
        }
    }
    fprintf( stderr, "\n\n" );
}

//
// entire B+tree data printer
//
void Tree::list_all_btree_values( ) const
{
    list_btree_values( get_leaf( ) );
}

#endif

