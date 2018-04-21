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
    : pool_size( 0 )
    , tree( nullptr )
    , root( nullptr )
    , leaf( nullptr )
    , fanout( 0 )
    , minfanout( 0 )
    , height( 0 )
    , pool( 0 )
    , theKey( 0 )
    , theData( nullptr )
    // , branch.split( nullptr )
{

    branch.split = nullptr;

    const int fanout = Data::NODE_SIZE / sizeof( Entry );

    set_fanout( fanout );
    set_min_fanout( ( fanout + 1 ) >> 1 );
    init_free_node_pool( pool_size );

    set_leaf( get_free_node() );        /* set up the first leaf node */
    set_root( get_leaf( ) );            /* the root is initially the leaf */
    get_root( )->set_flag( Node::isLEAF );
    get_root( )->set_flag( Node::isROOT );
    get_root( )->set_flag( Node::Node::FEWEST );
    init_tree_height( );

    set_fun_data( "0" );


#ifdef DEBUG
    fprintf(stderr, "INIT:  B+tree of fanout %d.\n", get_fanout() );
    show_btree();
    show_node( get_root() );
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

    free( (void *)get_node_array( ) );
}

//
// corresponds to a NULL node pointer value
//
Node* Tree::NO_NODE() const
{
    return node_array_head( ) - 1;
}

// #define nodearrayhead B->tree
Node* Tree::node_array_head( ) const
{
    return tree;
}

//
// check that a node is in fact a node
//
// #define isnode(j) (((j) != NONODE) && ((nAdr(j).i.info.flags & Node::MASK) == Node::MAGIC))
bool Tree::is_node( Node* j ) const
{
    return ( j != NO_NODE() && ( ( j->inner.info.flags & Node::MASK ) == Node::MAGIC ) );
}

// #define isntnode(j) ((j) == NONODE)
bool Tree::isnt_node( Node* j ) const
{
    return ( j == NO_NODE() );
}


// #define getfunkey B->theKey
Key Tree::get_fun_key( ) const
{
    return theKey;
}

// #define getfundata B->theData
data_type Tree::get_fun_data( ) const
{
    return theData;
}

// #define setfunkey(v) (B->theKey = (v))
void Tree::set_fun_key( Key v )
{
    theKey = v;
}

// #define setfundata(v) (B->theData = strdup(v))
// void setfundata( Tree* B, dataT v )
void Tree::set_fun_data( const char* v )
{
    theData = strdup( v );
}


// #define getpoolsize B->poolsize
int Tree::get_pool_size( ) const
{
    return pool_size;
}

// #define setpoolsize(v) (B->poolsize = (v))
void Tree::set_pool_size( int v )
{
    pool_size = v;
}

// #define getnodearray B->tree
Node* Tree::get_node_array( ) const
{
    return tree;
}

// #define setnodearray(v) (B->tree = (Node *)(v))
void Tree::set_node_array( Node* v )
{
    tree = v;
}

// #define getroot B->root
Node* Tree::get_root( ) const
{
    return root;
}

// #define setroot(v) (B->root = (v))
void Tree::set_root( Node* v )
{
    root = v;
}

// #define getleaf B->leaf
Node* Tree::get_leaf( ) const
{
    return leaf;
}

// #define setleaf(v) (B->leaf = (v))
void Tree::set_leaf( Node* v )
{
    leaf = v;
}


// #define getfanout B->fanout
int Tree::get_fanout( ) const
{
    return fanout;
}

// #define setfanout(v) (B->fanout = (v) - 1)
void Tree::set_fanout( int v )
{
    fanout = v - 1;
}

// #define getminfanout(j) ((nAdr(j).i.info.flags & isLEAF) ? B->fanout - B->minfanout: B->minfanout)
int Tree::get_min_fanout( const Node* j ) const
{
    // return ( j->X.i.info.flags & isLEAF ) ? fanout - minfanout : minfanout;
    return j->is_leaf() ? fanout - minfanout : minfanout;
}


// #define setminfanout(v) (B->minfanout = (v) - 1)
void Tree::set_min_fanout( int v )
{
    minfanout = v - 1;
}

// #define inittreeheight (B->height = 0)
void Tree::init_tree_height( )
{
    height = 0;
}

// #define inctreeheight B->height++
void Tree::inc_tree_height( )
{
    height++;
}

// #define dectreeheight B->height--
void Tree::dec_tree_height( )
{
    height--;
}

// #define gettreeheight B->height
int Tree::get_tree_height( ) const
{
    return height;
}

// #define getfirstfreenode B->pool
Node* Tree::get_first_free_node( ) const
{
    return pool;
}

// #define setfirstfreenode(v) (B->pool = (v))
void Tree::set_first_free_node( Node* v )
{
    pool = v;
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
// representation independent node numbering
//
// #define getnodenumber(v) ((v) - nodearrayhead)
int Tree::get_node_number( Node* v ) const
{
    return ( v - node_array_head() );
}



//
// top level search call
//
Node* Tree::search( Key key )
{
#ifdef DEBUG
    fprintf( stderr, "Tree::search: key %d.\n", key.get_value() );
#endif

    if( get_root()->num_entries() == 0 )
    {
        return nullptr;
    }

    set_fun_key( key );

    // start search from root node
    Node* find_node = descend_to_leaf( get_root() );

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
        //
        // also check previous slot
        //
        const int comp = Key::compare( get_fun_key( ), curr->get_key( slot - 1 ) );

        if( ( slot == 1 ) || ( comp >= 0 ) )
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
        //
        // or check following slot
        //
        const int comp = Key::compare( get_fun_key( ), curr->get_key( slot + 1 ));

        if( ( slot == curr->num_entries() ) || ( comp < 0 ) )
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
  Node* newNode;

#ifdef DEBUG
  fprintf(stderr, "INSERT:  key %d.\n", key.get_value() );
#endif

  set_fun_key( key );            /* set insertion key */
  set_fun_data( "data" );            /* a node containing data */
  set_split_path( NO_NODE() );
  newNode = descend_split( get_root( ));    /* insertion point search from root */
  if (newNode != get_split_path() )        /* indicates the root node has split */
    make_new_root( get_root( ), newNode);
}


//
// recurse down and split back up
//
Node* Tree::descend_split( Node* curr )
{
  Node*    newMe, *newNode;
  int    slot;

  if (!curr->is_full())
    set_split_path( NO_NODE() );
  else if ( get_split_path() == NO_NODE())
    set_split_path( curr );            /* indicates where nodes must split */

  slot = get_slot( curr);        /* is null only if the root is empty */
  if (curr->is_internal())            /* continue recursion to leaves */
    newMe = descend_split( curr->get_node( slot ));
  else if ((slot > 0) && !Key::compare( get_fun_key( ), curr->get_key( slot ))) {
    newMe = NO_NODE();            /* this code discards duplicates */
    set_split_path( NO_NODE() );
  }
  else
    newMe = get_data_node( get_fun_key( ) );    /* an insertion takes place */

  newNode = NO_NODE();            /* assume no node splitting necessary */

  if (newMe != NO_NODE()) {        /* insert only where necessary */
    if ( get_split_path() != NO_NODE())
      newNode = split( curr);        /* a sibling node is prepared */
    insert_entry( curr, slot, newNode, newMe);
  }

  return newNode;
}

//
// determine location of inserted key
//
void Tree::insert_entry( Node* newNode, int slot, Node* sibling, Node* downPtr )
{
    int split, i, j, k, x, y;

    if (sibling == NO_NODE()) {        /* no split occurred */
        place_entry( newNode, slot + 1, downPtr);
        newNode->clr_flag( Node::Node::FEWEST );
    }
    else {                /* split entries between the two */
        i = newNode->is_internal();        /* adjustment values */
        split = i ? get_fanout( ) - get_min_fanout( newNode ): get_min_fanout( newNode );
        j = (slot != split);
        k = (slot >= split);

        for (x = split + k + j * i, y = 1; x <= get_fanout(); x++, y++) {
            newNode->xfer_entry( x, sibling, y);    /* copy entries to sibling */
            newNode->dec_entries();
            sibling->inc_entries();
        }
        if (sibling->num_entries() == get_fanout())
            sibling->set_flag( Node::isFULL );        /* only ever happens in 2-3+trees */

        if (i) {                /* set first pointer of internal node */
            if (j) {
                sibling->set_first_node( newNode->get_node( split + k));
                newNode->dec_entries();
            }
            else
                sibling->set_first_node( downPtr);
        }

        if (j) {                /* insert new entry into correct spot */
            const Key xx = newNode->get_key( split + k );
            if (k)
                place_entry( sibling, slot - split + 1 - i, downPtr);
            else
                place_entry( newNode, slot + 1, downPtr);
            set_fun_key( xx );            /* set key separating nodes */
        }
        else if (!i)
            place_entry( sibling, 1, downPtr);

        newNode->clr_flag( Node::isFULL );        /* adjust node flags */
        if (newNode->num_entries() == get_min_fanout( newNode ))
            newNode->set_flag( Node::FEWEST );        /* never happens in even size nodes */
        if (sibling->num_entries() > get_min_fanout( sibling ))
            sibling->clr_flag( Node::FEWEST );

#ifdef DEBUG
        fprintf(stderr, "INSERT:  slot %d, node %d.\n", slot, get_node_number(downPtr));
        show_node( newNode);
        show_node( sibling);
#endif

    }
}

//
// place key into appropriate node & slot
//
void Tree::place_entry( Node* newNode, int slot, Node* downPtr )
{
  int x;

  for (x = newNode->num_entries(); x >= slot; x--)    /* make room for new entry */
    newNode->push_entry( x, 1);
  newNode->set_entry( slot, get_fun_key( ), downPtr);    /* place it in correct slot */

  newNode->inc_entries();                /* adjust entry counter */
  if (newNode->num_entries() == get_fanout())
    newNode->set_flag( Node::isFULL );
}


//
// split full node and set flags
//
Node* Tree::split( Node* newNode )
{
  Node* sibling;

  sibling = get_free_node();

  sibling->set_flag( Node::FEWEST );            /* set up node flags */

  if ( newNode->is_leaf()) {
    sibling->set_flag( Node::isLEAF );
    sibling->set_next_node( newNode->get_next_node());    /* adjust leaf pointers */
    newNode->set_next_node( sibling);
  }
  if ( get_split_path() == newNode)
    set_split_path( NO_NODE() );            /* no more splitting needed */

  return sibling;
}


//
// build new root node
//
void Tree::make_new_root( Node* oldRoot, Node* newNode )
{
  set_root( get_free_node() );

  get_root( )->set_first_node( oldRoot);    /* old root becomes new root's child */
  get_root( )->set_entry( 1, get_fun_key( ), newNode);    /* old root's sibling also */
  get_root( )->inc_entries();

  oldRoot->clr_flag( Node::isROOT );
  get_root( )->set_flag( Node::isROOT );
  get_root( )->set_flag( Node::FEWEST );
  inc_tree_height( );
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
  Node* newNode;

#ifdef DEBUG
  fprintf(stderr, "DELETE:  key %d.\n", key.get_value() );
#endif

  set_fun_key( key );            /* set deletion key */
  set_merge_path( NO_NODE() );
  newNode = descend_balance( get_root( ), NO_NODE(), NO_NODE(), NO_NODE(), NO_NODE(), NO_NODE());
  if ( is_node( newNode ))
    collapse_root( get_root( ), newNode);    /* remove root when superfluous */
}


//
// remove old root node
//
void Tree::collapse_root( Node* oldRoot, Node* newRoot )
{

#ifdef DEBUG
  fprintf(stderr, "COLLAPSE:  old %d, new %d.\n", get_node_number(oldRoot), get_node_number(newRoot));
  show_node( oldRoot);
  show_node( newRoot);
#endif

  set_root( newRoot);
  newRoot->set_flag( Node::isROOT );
  put_free_node( oldRoot );
  dec_tree_height( );            /* the height of the tree decreases */
}


//
// recurse down and balance back up
//
Node* Tree::descend_balance( Node* curr, Node* left, Node* right, Node* lAnc, Node* rAnc, Node* parent )
{
  Node*    newMe, *myLeft, *myRight, *lAnchor, *rAnchor, *newNode;
  int    slot, notleft, notright, fewleft, fewright, test;

  if (!curr->is_few())
    set_merge_path( NO_NODE() );
  else if ( get_merge_path() == NO_NODE())
    set_merge_path( curr );        /* mark which nodes may need rebalancing */

  slot = get_slot( curr);
  newNode = curr->get_node( slot );
  if (curr->is_internal()) {    /* set up next recursion call's parameters */
    if (slot == 0) {
      myLeft = isnt_node( left ) ? NO_NODE() : left->get_last_node();
      lAnchor = lAnc;
    }
    else {
      myLeft = curr->get_node( slot - 1 );
      lAnchor = curr;
    }
    if (slot == curr->num_entries()) {
      myRight = isnt_node( right ) ? NO_NODE() : right->get_first_node();
      rAnchor = rAnc;
    }
    else {
      myRight = curr->get_node( slot + 1 );
      rAnchor = curr;
    }
    newMe = descend_balance( newNode, myLeft, myRight, lAnchor, rAnchor, curr);
  }
  else if ((slot > 0) && !Key::compare( get_fun_key( ), curr->get_key( slot )))
    newMe = newNode;        /* a key to be deleted is found */
  else {
    newMe = NO_NODE();        /* no deletion possible, key not found */
    set_merge_path( NO_NODE() );
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

  if (newMe != NO_NODE())    /* this node removal doesn't consider duplicates */
    remove_entry( curr, slot + (newMe != newNode));    /* removes one of two */

#ifdef DEBUG
  fprintf(stderr, "DELETE:  slot %d, node %d.\n", slot, get_node_number(newMe));
  show_node( curr);
#endif

  if ( get_merge_path() == NO_NODE())
    newNode = NO_NODE();
  else {        /* tree rebalancing rules for node merges and shifts */
    notleft = isnt_node( left );
    notright = isnt_node( right );
    fewleft = left->is_few();        /* only used when defined */
    fewright = right->is_few();

            /* CASE 1:  prepare root node (curr) for removal */
    if (notleft && notright) {
      test = curr->is_leaf();        /* check if B+tree has become empty */
      newNode = test ? NO_NODE() : curr->get_first_node();
    }
            /* CASE 2:  the merging of two nodes is a must */
    else if ((notleft || fewleft) && (notright || fewright)) {
      test = !(lAnc == parent);
      newNode = test ? merge( curr, right, rAnc) : merge( left, curr, lAnc);
    }
            /* CASE 3: choose the better of a merge or a shift */
    else if (!notleft && fewleft && !notright && !fewright) {
      test = !(rAnc == parent) && (curr == get_merge_path() );
      newNode = test ? merge( left, curr, lAnc) : shift( curr, right, rAnc);
    }
            /* CASE 4: also choose between a merge or a shift */
    else if (!notleft && !fewleft && !notright && fewright) {
      test = !(lAnc == parent) && (curr == get_merge_path() );
      newNode = test ? merge( curr, right, rAnc) : shift( left, curr, lAnc);
    }
            /* CASE 5: choose the more effective of two shifts */
    else if (lAnc == rAnc) {         /* => both anchors are the parent */
      test = (left->num_entries() <= right->num_entries());
      newNode = test ? shift( curr, right, rAnc) : shift( left, curr, lAnc);
    }
            /* CASE 6: choose the shift with more local effect */
    else {                /* if omitting cases 3,4,5 use below */
      test = (lAnc == parent);        /* test = (!notleft && !fewleft); */
      newNode = test ? shift( left, curr, lAnc) : shift( curr, right, rAnc);
    }
  }

  return newNode;
}


//
// remove key and pointer from node
//
void Tree::remove_entry( Node* curr, int slot )
{
  int x;

  put_free_node( curr->get_node( slot ));    /* return deleted node to free list */
  for (x = slot; x < curr->num_entries(); x++)
    curr->pull_entry( x, 1);        /* adjust node with removed key */
  curr->dec_entries();
  curr->clr_flag( Node::isFULL );        /* keep flag information up to date */
  if (curr->is_root()) {
    if (curr->num_entries() == 1)
      curr->set_flag( Node::FEWEST );
  }
  else if (curr->num_entries() == get_min_fanout( curr ))
    curr->set_flag( Node::FEWEST );
}


//
// merge a node pair & set emptied node up for removal
//
Node* Tree::merge( Node* left, Node* right, Node* anchor )
{
  int    x, y, z;

#ifdef DEBUG
  fprintf(stderr, "MERGE:  left %d, right %d.\n", get_node_number(left), get_node_number(right));
  show_node( left);
  show_node( right);
#endif

  if (left->is_internal()) {
    left->inc_entries();            /* copy key separating the nodes */
    set_fun_key( right->get_key( 1 ) );    /* defined but maybe just deleted */
    z = get_slot( anchor);        /* needs the just calculated key */
    set_fun_key( anchor->get_key( z ) );    /* set slot to delete in anchor */
    left->set_entry( left->num_entries(), get_fun_key( ), right->get_first_node());
  }
  else
    left->set_next_node( right->get_next_node());
  for (x = left->num_entries() + 1, y = 1; y <= right->num_entries(); x++, y++) {
    left->inc_entries();
    right->xfer_entry( y, left, x);    /* transfer entries to left node */
  }
  if (left->num_entries() > get_min_fanout( left ))
    left->clr_flag( Node::FEWEST );
  if (left->num_entries() == get_fanout())
    left->set_flag( Node::isFULL );        /* never happens in even size nodes */

  if ( get_merge_path() == left || get_merge_path() == right)
    set_merge_path( NO_NODE() );        /* indicate rebalancing is complete */

  return right;
}


//
// shift entries in a node pair & adjust anchor key value
//
Node* Tree::shift( Node* left, Node* right, Node* anchor )
{
  int    i, x, y, z;

#ifdef DEBUG
  fprintf(stderr, "SHIFT:  left %d, right %d.\n", get_node_number(left), get_node_number(right));
  show_node( left);
  show_node( right);
#endif

  i = left->is_internal();
  if (left->num_entries() < right->num_entries()) {    /* shift entries to left */
    y = (right->num_entries() - left->num_entries()) >> 1;
    x = left->num_entries() + y;
    set_fun_key( right->get_key( y + 1 - i ) );    /* set new anchor key value */
    z = get_slot( anchor);            /* find slot in anchor node */
    if (i) {                    /* move out old anchor value */
      right->dec_entries();            /* adjust for shifting anchor */
      left->inc_entries();
      left->set_entry( left->num_entries(), anchor->get_key( z ), right->get_first_node());
      right->set_first_node( right->get_node( y + 1 - i ));
    }
    right->clr_flag( Node::isFULL );
    anchor->set_key( z, get_fun_key( ) );        /* set new anchor value */
    for (z = y, y -= i; y > 0; y--, x--) {
      right->dec_entries();            /* adjust entry count */
      left->inc_entries();
      right->xfer_entry( y, left, x);        /* transfer entries over */
    }

    for (x = 1; x <= right->num_entries(); x++)    /* adjust reduced node */
      right->pull_entry( x, z);
  }
  else {                    /* shift entries to right */
    y = (left->num_entries() - right->num_entries()) >> 1;
    x = left->num_entries() - y + 1;

    for (z = right->num_entries(); z > 0; z--)    /* adjust increased node */
      right->push_entry( z, y);

    set_fun_key( left->get_key( x ) );            /* set new anchor key value */
    z = get_slot( anchor) + 1;
    if (i) {
      left->dec_entries();
      right->inc_entries();
      right->set_entry( y, anchor->get_key( z ), right->get_first_node());
      right->set_first_node( left->get_node( x ));
    }
    left->clr_flag( Node::isFULL );
    anchor->set_key( z, get_fun_key( ) );
    for (x = left->num_entries() + i, y -= i; y > 0; y--, x--) {
      left->dec_entries();
      right->inc_entries();
      left->xfer_entry( x, right, y);        /* transfer entries over */
    }
  }
  if (left->num_entries() == get_min_fanout( left ))        /* adjust node flags */
    left->set_flag( Node::FEWEST );
  else
    left->clr_flag( Node::FEWEST );            /* never happens in 2-3+trees */
  if (right->num_entries() == get_min_fanout( right ))
    right->set_flag( Node::FEWEST );
  else
    right->clr_flag( Node::FEWEST );            /* never happens in 2-3+trees */
  set_merge_path( NO_NODE() );

#ifdef DEBUG
  show_node( left);
  show_node( right);
#endif

  return NO_NODE();
}


//
// Set up pool of free nodes
//
void Tree::init_free_node_pool( int quantity )
{
    set_pool_size( quantity );
    set_node_array( (Node*)malloc(quantity * sizeof(Node)) );    /* node memory block */
    set_first_free_node( node_array_head() );    /* start a list of free nodes */

    Node* n = get_first_free_node();
    for( int i = 0; i < quantity; i++ )
    {
        n->clear_flags();
        n->clear_entries();
        n->set_next_node( n + 1 );        /* insert node into free node list */
        n++;
    }
    --n;
    n->set_next_node( NO_NODE() );        /* indicates end of free node list */
}


//
// take a free B+tree node from the pool
//
Node* Tree::get_free_node( )
{
    Node* newNode = get_first_free_node( );

    if( newNode == NO_NODE() )
    {
          // can't recover from this
          throw std::runtime_error( "Out of tree nodes." );
    }

    // adjust free node list
    set_first_free_node( newNode->get_next_node( ) );

    // remove node from list
    newNode->set_next_node( NO_NODE() );

    return newNode;
}


//
// return a deleted B+tree node to the pool
//
void Tree::put_free_node( Node* self )
{
  self->clear_flags();
  self->clear_entries();
  self->set_next_node(  get_first_free_node() );        /* add node to list */
  set_first_free_node( self );            /* set it to be list head */
}

//
// fill a free data node with a key and associated data
// can add data parameter
//
Node* Tree::get_data_node( Key key )
{
    Node* newNode = get_free_node( );
    Key* value;

    value = (Key *) &newNode->data;

    // can add code to fill node
    *value = key;

    return newNode;
}


//
// B+tree node printer
//
void Tree::show_node( Node* n ) const
{
  int x;

  fprintf(stderr, "------------------------------------------------\n");
  fprintf(stderr, "| node %6d                 ", get_node_number(n));
  fprintf(stderr, "  magic    %4x  |\n", n->get_flags() & Node::MASK);
  fprintf(stderr, "|- - - - - - - - - - - - - - - - - - - - - - - |\n");
  fprintf(stderr, "| flags   %1d%1d%1d%1d ", n->is_few(), n->is_full(), n->is_root(), n->is_leaf());
  fprintf(stderr, "| keys = %5d ", n->num_entries());
  fprintf(stderr, "| node = %6d  |\n", get_node_number( n->get_first_node()));
  for (x = 1; x <= n->num_entries(); x++) {
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
  fprintf(stderr, "|  root        %6d  |\n", get_node_number( get_root() ));
  fprintf(stderr, "|  leaf        %6d  |\n", get_node_number( get_leaf() ));
  fprintf(stderr, "|  fanout         %3d  |\n", get_fanout() + 1);
  fprintf(stderr, "|  minfanout      %3d  |\n", get_min_fanout( get_root() ) + 1);
  fprintf(stderr, "|  height         %3d  |\n", get_tree_height() );
  fprintf(stderr, "|  freenode    %6d  |\n", get_node_number( get_first_free_node() ));
  fprintf(stderr, "|  theKey      %6d  |\n", get_fun_key().get_value() );
  fprintf(stderr, "|  theData     %6s  |\n", get_fun_data( ));
  fprintf(stderr, "-  --  --  --  --  --  -\n");

   Node* n = get_root();
   while( n != NO_NODE() )
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

    for( slot = 1; ( n != NO_NODE() ) && n->num_entries(); num++ )
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
