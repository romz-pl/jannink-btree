#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "btree.h"



/* check that a node is in fact a node */
// #define isnode(j) (((j) != NONODE) && ((nAdr(j).i.info.flags & MASK) == MAGIC))
bool is_node( Tree* B, Node* j )
{
    return ( j != NONODE && ( ( j->X.i.info.flags & MASK ) == MAGIC ) );
}

// #define isntnode(j) ((j) == NONODE)
bool isnt_node( Tree* B, Node* j )
{
    return ( j == NONODE );
}


// #define getfunkey B->theKey
keyT Tree::get_fun_key( ) const
{
    return theKey;
}

// #define getfundata B->theData
dataT Tree::get_fun_data( ) const
{
    return theData;
}

// #define setfunkey(v) (B->theKey = (v))
void Tree::set_fun_key( keyT v )
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
Nptr Tree::get_root( ) const
{
    return root;
}

// #define setroot(v) (B->root = (v))
void set_root( Tree* B, Nptr v )
{
    B->root = v;
}

// #define getleaf B->leaf
Nptr get_leaf( Tree* B )
{
    return B->leaf;
}

// #define setleaf(v) (B->leaf = (v))
void set_leaf( Tree* B, Nptr v )
{
    B->leaf = v;
}

/* define max/min number of pointers per node */
// #define getfanout B->fanout
int get_fanout( Tree* B )
{
    return B->fanout;
}

// #define setfanout(v) (B->fanout = (v) - 1)
void set_fanout( Tree* B, int v )
{
    B->fanout = v - 1;
}

// #define getminfanout(j) ((nAdr(j).i.info.flags & isLEAF) ? B->fanout - B->minfanout: B->minfanout)
int get_min_fanout( Tree* B, Node* j )
{
    return ( j->X.i.info.flags & isLEAF ) ? B->fanout - B->minfanout : B->minfanout;
}


// #define setminfanout(v) (B->minfanout = (v) - 1)
void set_min_fanout( Tree* B, int v )
{
    B->minfanout = v - 1;
}

/* manage B+tree height */
// #define inittreeheight (B->height = 0)
void init_tree_height( Tree* B )
{
    B->height = 0;
}

// #define inctreeheight B->height++
void inc_tree_height( Tree* B )
{
    B->height++;
}

// #define dectreeheight B->height--
void dec_tree_height( Tree* B )
{
    B->height--;
}

// #define gettreeheight B->height
int get_tree_height( Tree* B )
{
    return B->height;
}


/* access pool of free nodes */
// #define getfirstfreenode B->pool
Nptr get_first_free_node( Tree* B )
{
    return B->pool;
}

// #define setfirstfreenode(v) (B->pool = (v))
void set_first_free_node( Tree* B, Nptr v )
{
    B->pool = v;
}

/* handle split/merge points during insert/delete */
// #define getsplitpath B->branch.split
Nptr get_split_path( Tree* B )
{
    return B->branch.split;
}

// #define setsplitpath(v) (B->branch.split = (v))
void set_split_path( Tree* B, Nptr v )
{
    B->branch.split = v;
}

// #define getmergepath B->branch.merge
Nptr get_merge_path( Tree* B )
{
    return B->branch.merge;
}

// #define setmergepath(v) (B->branch.merge = (v))
void set_merge_path( Tree* B, Nptr v )
{
    B->branch.merge = v;
}

/* exploit function to compare two B+tree keys */
// #define comparekeys (*B->keycmp)
KeyCmp compare_keys( Tree* B )
{
    return *B->keycmp;
}

// #define setcomparekeys(v) (B->keycmp = (v))
void set_compare_keys( Tree* B, KeyCmp v )
{
    B->keycmp = v;
}


/* representation independent node numbering */
// #define getnodenumber(v) ((v) - nodearrayhead)



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    Sample Key Comparison Function                    |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

int compare_keys(keyT key1, keyT key2)
{
  return key1 - key2;        /* this is the case when keys are integer */
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    B+tree Initialization Utilities                    |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
void init_free_node_pool(Tree *B, int quantity);
Nptr get_free_node(Tree *B);
void put_free_node(Tree *B, Nptr self);

/*~~~~~~~~~~~~~~~~~~~   Set up B+tree structure   ~~~~~~~~~~~~~~~~~~~~~*/
Tree *btree_init(int poolsz, int fan, KeyCmp keyCmp)
{
  Tree *B;

  B = (Tree *)( malloc( sizeof( Tree ) ) );
  set_fanout( B, fan );
  set_min_fanout( B, (fan + 1) >> 1 );
  init_free_node_pool( B, poolsz );

  set_leaf( B, get_free_node( B ) );        /* set up the first leaf node */
  set_root( B, get_leaf( B ) );            /* the root is initially the leaf */
  B->get_root( )->set_flag( isLEAF );
  B->get_root( )->set_flag( isROOT );
  B->get_root( )->set_flag( FEWEST );
  init_tree_height( B );

  B->set_fun_key( 0 );
  B->set_fun_data( "0" );
  set_compare_keys( B, keyCmp );

#ifdef DEBUG
  fprintf(stderr, "INIT:  B+tree of fanout %d.\n", fan);
  showBtree(B);
  showNode(B, getroot);
#endif

  return B;
}

/*~~~~~~~~~~~~~~~~~~~   Clean up B+tree structure   ~~~~~~~~~~~~~~~~~~~*/
void btree_free(Tree *B)
{
#ifdef DEBUG
  fprintf(stderr, "FREE:  B+tree at %10p.\n", (void *) B);
#endif

  free((void *) B->get_node_array( ) );
  free((void *) B);
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    Find location for data                        |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
Nptr descend_to_leaf(Tree *B, Nptr curr);
int get_slot(Tree *B, Nptr curr);
int find_key(Tree *B, Nptr curr, int lo, int hi);
int best_match(Tree *B, Nptr curr, int slot);

/*~~~~~~~~~~~~~~~~~~~~~   top level search call   ~~~~~~~~~~~~~~~~~~~~~*/
Nptr btree_search(Tree *B, keyT key)
{
  Nptr    findNode;

#ifdef DEBUG
  fprintf(stderr, "SEARCH:  key %d.\n", key);
#endif

  B->set_fun_key( key );            /* set search key */
  findNode = descend_to_leaf(B, B->get_root( ));    /* start search from root node */

#ifdef DEBUG
  fprintf(stderr, "SEARCH:  found on page %d.\n", getnodenumber(findNode));
#endif

  return findNode;
}

/*~~~~~~~~~~~~~~~~~~~~~   `recurse' down B+tree   ~~~~~~~~~~~~~~~~~~~~~*/
Nptr descend_to_leaf(Tree *B, Nptr curr)
{
  int    slot;
  Nptr    findNode;

  for (slot = get_slot(B, curr); curr->is_internal(); slot = get_slot(B, curr))
    curr = curr->get_node( slot );
  if ((slot > 0) && !compare_keys( B )( B->get_fun_key( ), curr->get_key( slot ) ) )
    findNode = curr;            /* correct key value found */
  else
    findNode = NONODE;            /* key value not in tree */

  return findNode;
}

/*~~~~~~~~~~~~~~~~~~~   find slot for search key   ~~~~~~~~~~~~~~~~~~~~*/
int get_slot(Tree *B, Nptr curr)
{
  int slot, entries;

  entries = curr->num_entries();        /* need this if root is ever empty */
  slot = !entries ? 0 : find_key(B, curr, 1, entries);

#ifdef DEBUG
  fprintf(stderr, "GETSLOT:  slot %d.\n", slot);
#endif

  return slot;
}


/*~~~~~~~~~~~~~~~~~~~   recursive binary search   ~~~~~~~~~~~~~~~~~~~~~*/
int find_key(Tree *B, Nptr curr, int lo, int hi)
{
  int mid, findslot;

#ifdef DEBUG
  fprintf(stderr, "GETSLOT:  lo %d, hi %d.\n", lo, hi);
  showNode(B, curr);
#endif

  if (hi == lo) {
    findslot = best_match(B, curr, lo);        /* recursion base case */

#ifdef DEBUG
    if (findslot == ERROR)
      fprintf(stderr, "Bad key ordering on node %d\n", getnodenumber(curr));
#endif

  }
  else {
    mid = (lo + hi) >> 1;
    switch (findslot = best_match(B, curr, mid)) {
    case LOWER:                /* check lower half of range */
      findslot = find_key(B, curr, lo, mid - 1);        /* never in 2-3+trees */
    break;
    case UPPER:                /* check upper half of range */
      findslot = find_key(B, curr, mid + 1, hi);
    break;

#ifdef DEBUG
    case ERROR:
      fprintf(stderr, "Bad key ordering on node %d\n", getnodenumber(curr));
#endif

    }
  }
  return findslot;
}


/*~~~~~~~~~~~   comparison of key with a target key slot   ~~~~~~~~~~~~*/
int best_match(Tree *B, Nptr curr, int slot)
{
  int diff, comp, findslot;

  diff = compare_keys( B )( B->get_fun_key( ), curr->get_key( slot ));
  if (diff < 0) {        /* also check previous slot */
    if ((slot == 1) ||
        ((comp = compare_keys( B )( B->get_fun_key( ), curr->get_key( slot - 1 ))) >= 0))
      findslot = slot - 1;

#ifdef DEBUG
    else if (comp < diff)
      findslot = ERROR;        /* inconsistent ordering of keys */
#endif

    else
      findslot = LOWER;        /* key must be below in node ordering */
  }
  else {            /* or check following slot */
    if ((slot == curr->num_entries()) ||
        ((comp = compare_keys( B )( B->get_fun_key( ), curr->get_key( slot + 1 ))) < 0))
      findslot = slot;
    else if (comp == 0)
      findslot = slot + 1;

#ifdef DEBUG
    else if (comp > diff)
      findslot = ERROR;        /* inconsistent ordering of keys */
#endif

    else
      findslot = UPPER;        /* key must be above in node ordering */
  }
  return findslot;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    Insert new data into tree                    |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
Nptr get_data_node(Tree *B, keyT key);
Nptr descend_split(Tree *B, Nptr curr);
void insert_entry(Tree *B, Nptr node, int slot, Nptr sibling, Nptr downPtr);
void place_entry(Tree *B, Nptr node, int slot, Nptr downPtr);
Nptr split(Tree *B, Nptr node);
void make_new_root(Tree *B, Nptr oldRoot, Nptr newNode);

/*~~~~~~~~~~~~~~~~~~~~~   top level insert call   ~~~~~~~~~~~~~~~~~~~~~*/
void btree_insert(Tree *B, keyT key)
{
  Nptr newNode;

#ifdef DEBUG
  fprintf(stderr, "INSERT:  key %d.\n", key);
#endif

  B->set_fun_key( key );            /* set insertion key */
  B->set_fun_data( "data" );            /* a node containing data */
  set_split_path( B, NONODE );
  newNode = descend_split(B, B->get_root( ));    /* insertion point search from root */
  if (newNode != get_split_path( B ) )        /* indicates the root node has split */
    make_new_root(B, B->get_root( ), newNode);
}


/*~~~~~~~~~~~~~~~~   recurse down and split back up   ~~~~~~~~~~~~~~~~~*/
Nptr descend_split(Tree *B, Nptr curr)
{
  Nptr    newMe, newNode;
  int    slot;

  if (!curr->is_full())
    set_split_path( B, NONODE );
  else if (get_split_path( B ) == NONODE)
    set_split_path( B, curr );            /* indicates where nodes must split */

  slot = get_slot(B, curr);        /* is null only if the root is empty */
  if (curr->is_internal())            /* continue recursion to leaves */
    newMe = descend_split(B, curr->get_node( slot ));
  else if ((slot > 0) && !compare_keys( B )( B->get_fun_key( ), curr->get_key( slot ))) {
    newMe = NONODE;            /* this code discards duplicates */
    set_split_path( B, NONODE );
  }
  else
    newMe = get_data_node(B, B ->get_fun_key( ));    /* an insertion takes place */

  newNode = NONODE;            /* assume no node splitting necessary */

  if (newMe != NONODE) {        /* insert only where necessary */
    if (get_split_path( B ) != NONODE)
      newNode = split(B, curr);        /* a sibling node is prepared */
    insert_entry(B, curr, slot, newNode, newMe);
  }

  return newNode;
}

/*~~~~~~~~~~~~~~   determine location of inserted key   ~~~~~~~~~~~~~~~*/
void insert_entry(Tree *B, Nptr newNode, int slot, Nptr sibling, Nptr downPtr)
{
  int split, i, j, k, x, y;

  if (sibling == NONODE) {        /* no split occurred */
    place_entry(B, newNode, slot + 1, downPtr);
    newNode->clr_flag( FEWEST );
  }
  else {                /* split entries between the two */
    i = newNode->is_internal();        /* adjustment values */
    split = i ? get_fanout( B ) - get_min_fanout( B, newNode ): get_min_fanout( B, newNode );
    j = (slot != split);
    k = (slot >= split);

    for (x = split + k + j * i, y = 1; x <= get_fanout( B ); x++, y++) {
      newNode->xfer_entry( x, sibling, y);    /* copy entries to sibling */
      newNode->dec_entries();
      sibling->inc_entries();
    }
    if (sibling->num_entries() == get_fanout( B ))
      sibling->set_flag( isFULL );        /* only ever happens in 2-3+trees */

    if (i) {                /* set first pointer of internal node */
      if (j) {
        sibling->set_first_node( newNode->get_node( split + k));
        newNode->dec_entries();
      }
      else
        sibling->set_first_node( downPtr);
    }

    if (j) {                /* insert new entry into correct spot */
      x = newNode->get_key( split + k );
      if (k)
    place_entry(B, sibling, slot - split + 1 - i, downPtr);
      else
    place_entry(B, newNode, slot + 1, downPtr);
      B->set_fun_key( x );            /* set key separating nodes */
    }
    else if (!i)
      place_entry(B, sibling, 1, downPtr);

    newNode->clr_flag( isFULL );        /* adjust node flags */
    if (newNode->num_entries() == get_min_fanout( B, newNode ))
      newNode->set_flag( FEWEST );        /* never happens in even size nodes */
    if (sibling->num_entries() > get_min_fanout( B, sibling ))
      sibling->clr_flag( FEWEST );

#ifdef DEBUG
  fprintf(stderr, "INSERT:  slot %d, node %d.\n", slot, getnodenumber(downPtr));
  showNode(B, newNode);
  showNode(B, sibling);
#endif

  }
}

/*~~~~~~~~~~~   place key into appropriate node & slot   ~~~~~~~~~~~~~~*/
void place_entry(Tree *B, Nptr newNode, int slot, Nptr downPtr)
{
  int x;

  for (x = newNode->num_entries(); x >= slot; x--)    /* make room for new entry */
    newNode->push_entry( x, 1);
  newNode->set_entry( slot, B->get_fun_key( ), downPtr);    /* place it in correct slot */

  newNode->inc_entries();                /* adjust entry counter */
  if (newNode->num_entries() == get_fanout( B ))
    newNode->set_flag( isFULL );
}


/*~~~~~~~~~~~~~~~~   split full node and set flags   ~~~~~~~~~~~~~~~~~~*/
Nptr split(Tree *B, Nptr newNode)
{
  Nptr sibling;

  sibling = get_free_node(B);

  sibling->set_flag( FEWEST );            /* set up node flags */

  if ( newNode->is_leaf()) {
    sibling->set_flag( isLEAF );
    sibling->set_next_node( newNode->get_next_node());    /* adjust leaf pointers */
    newNode->set_next_node( sibling);
  }
  if (get_split_path( B ) == newNode)
    set_split_path( B, NONODE );            /* no more splitting needed */

  return sibling;
}


/*~~~~~~~~~~~~~~~~~~~~~   build new root node   ~~~~~~~~~~~~~~~~~~~~~~~*/
void make_new_root(Tree *B, Nptr oldRoot, Nptr newNode)
{
  set_root( B, get_free_node(B));

  B->get_root( )->set_first_node( oldRoot);    /* old root becomes new root's child */
  B->get_root( )->set_entry( 1, B->get_fun_key( ), newNode);    /* old root's sibling also */
  B->get_root( )->inc_entries();

  oldRoot->clr_flag( isROOT );
  B->get_root( )->set_flag( isROOT );
  B->get_root( )->set_flag( FEWEST );
  inc_tree_height( B );
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    Delete data from tree                        |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~~~   private functions   ~~~~~~~~~~~~~~~~~~~~~~~~*/
Nptr descend_balance(Tree *B, Nptr curr, Nptr left, Nptr right, Nptr lAnc, Nptr rAnc, Nptr parent);
void collapse_root(Tree *B, Nptr oldRoot, Nptr newRoot);
void remove_entry(Tree *B, Nptr curr, int slot);
Nptr merge(Tree *B, Nptr left, Nptr right, Nptr anchor);
Nptr shift(Tree *B, Nptr left, Nptr right, Nptr anchor);

/*~~~~~~~~~~~~~~~~~~~~~   top level delete call   ~~~~~~~~~~~~~~~~~~~~~*\
|
|    The recursive call for deletion carries 5 additional parameters
|    which may be needed to rebalance the B+tree when removing the key.
|    These parameters are:
|        1. immediate left neighbor of the current node
|        2. immediate right neighbor of the current node
|        3. the anchor of the current node and left neighbor
|        4. the anchor of the current node and right neighbor
|        5. the parent of the current node
|
|    All of these parameters are simple to calculate going along the
|    recursive path to the leaf nodes and the point of key deletion.
|    At that time, the algorithm determines which node manipulations
|    are most efficient, that is, cause the least rearranging of data,
|    and minimize the need for non-local key manipulation.
|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void btree_delete(Tree *B, keyT key)
{
  Nptr newNode;

#ifdef DEBUG
  fprintf(stderr, "DELETE:  key %d.\n", key);
#endif

  B->set_fun_key( key );            /* set deletion key */
  set_merge_path( B, NONODE );
  newNode = descend_balance(B, B->get_root( ), NONODE, NONODE, NONODE, NONODE, NONODE);
  if (is_node( B, newNode ))
    collapse_root(B, B->get_root( ), newNode);    /* remove root when superfluous */
}


/*~~~~~~~~~~~~~~~~~~~~~   remove old root node   ~~~~~~~~~~~~~~~~~~~~~~*/
void collapse_root(Tree *B, Nptr oldRoot, Nptr newRoot)
{

#ifdef DEBUG
  fprintf(stderr, "COLLAPSE:  old %d, new %d.\n", getnodenumber(oldRoot), getnodenumber(newRoot));
  showNode(B, oldRoot);
  showNode(B, newRoot);
#endif

  set_root( B, newRoot);
  newRoot->set_flag( isROOT );
  put_free_node(B, oldRoot);
  dec_tree_height( B );            /* the height of the tree decreases */
}


/*~~~~~~~~~~~~~~~   recurse down and balance back up   ~~~~~~~~~~~~~~~~*/
Nptr descend_balance(Tree *B, Nptr curr, Nptr left, Nptr right, Nptr lAnc, Nptr rAnc, Nptr parent)
{
  Nptr    newMe, myLeft, myRight, lAnchor, rAnchor, newNode;
  int    slot, notleft, notright, fewleft, fewright, test;

  if (!curr->is_few())
    set_merge_path( B, NONODE );
  else if (get_merge_path( B ) == NONODE)
    set_merge_path( B, curr );        /* mark which nodes may need rebalancing */

  slot = get_slot(B, curr);
  newNode = curr->get_node( slot );
  if (curr->is_internal()) {    /* set up next recursion call's parameters */
    if (slot == 0) {
      myLeft = isnt_node( B, left ) ? NONODE : left->get_last_node();
      lAnchor = lAnc;
    }
    else {
      myLeft = curr->get_node( slot - 1 );
      lAnchor = curr;
    }
    if (slot == curr->num_entries()) {
      myRight = isnt_node( B, right ) ? NONODE : right->get_first_node();
      rAnchor = rAnc;
    }
    else {
      myRight = curr->get_node( slot + 1 );
      rAnchor = curr;
    }
    newMe = descend_balance(B, newNode, myLeft, myRight, lAnchor, rAnchor, curr);
  }
  else if ((slot > 0) && !compare_keys( B )( B->get_fun_key( ), curr->get_key( slot )))
    newMe = newNode;        /* a key to be deleted is found */
  else {
    newMe = NONODE;        /* no deletion possible, key not found */
    set_merge_path( B, NONODE );
  }

/*~~~~~~~~~~~~~~~~   rebalancing tree after deletion   ~~~~~~~~~~~~~~~~*\
|
|    The simplest B+tree rebalancing consists of the following rules.
|
|    If a node underflows:
|    CASE 1 check if it is the root, and collapse it if it is,
|    CASE 2 otherwise, check if both of its neighbors are minimum
|        sized and merge the underflowing node with one of them,
|    CASE 3 otherwise shift surplus entries to the underflowing node.
|
|    The choice of which neighbor to use is optional.  However, the
|    rebalancing rules that follow also ensure whenever possible
|    that the merges and shifts which do occur use a neighbor whose
|    anchor is the parent of the underflowing node.
|
|    Cases 3, 4, 5 below are more an optimization than a requirement,
|    and can be omitted, with a change of the action value in case 6,
|    which actually corresponds to the third case described above.
|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*             begin deletion, working upwards from leaves */

  if (newMe != NONODE)    /* this node removal doesn't consider duplicates */
    remove_entry(B, curr, slot + (newMe != newNode));    /* removes one of two */

#ifdef DEBUG
  fprintf(stderr, "DELETE:  slot %d, node %d.\n", slot, getnodenumber(newMe));
  showNode(B, curr);
#endif

  if (get_merge_path( B ) == NONODE)
    newNode = NONODE;
  else {        /* tree rebalancing rules for node merges and shifts */
    notleft = isnt_node( B, left );
    notright = isnt_node( B, right );
    fewleft = left->is_few();        /* only used when defined */
    fewright = right->is_few();

            /* CASE 1:  prepare root node (curr) for removal */
    if (notleft && notright) {
      test = curr->is_leaf();        /* check if B+tree has become empty */
      newNode = test ? NONODE : curr->get_first_node();
    }
            /* CASE 2:  the merging of two nodes is a must */
    else if ((notleft || fewleft) && (notright || fewright)) {
      test = !(lAnc == parent);
      newNode = test ? merge(B, curr, right, rAnc) : merge(B, left, curr, lAnc);
    }
            /* CASE 3: choose the better of a merge or a shift */
    else if (!notleft && fewleft && !notright && !fewright) {
      test = !(rAnc == parent) && (curr == get_merge_path( B ) );
      newNode = test ? merge(B, left, curr, lAnc) : shift(B, curr, right, rAnc);
    }
            /* CASE 4: also choose between a merge or a shift */
    else if (!notleft && !fewleft && !notright && fewright) {
      test = !(lAnc == parent) && (curr == get_merge_path( B ) );
      newNode = test ? merge(B, curr, right, rAnc) : shift(B, left, curr, lAnc);
    }
            /* CASE 5: choose the more effective of two shifts */
    else if (lAnc == rAnc) {         /* => both anchors are the parent */
      test = (left->num_entries() <= right->num_entries());
      newNode = test ? shift(B, curr, right, rAnc) : shift(B, left, curr, lAnc);
    }
            /* CASE 6: choose the shift with more local effect */
    else {                /* if omitting cases 3,4,5 use below */
      test = (lAnc == parent);        /* test = (!notleft && !fewleft); */
      newNode = test ? shift(B, left, curr, lAnc) : shift(B, curr, right, rAnc);
    }
  }

  return newNode;
}


/*~~~~~~~~~~~~~~~   remove key and pointer from node   ~~~~~~~~~~~~~~~~*/
void remove_entry(Tree *B, Nptr curr, int slot)
{
  int x;

  put_free_node(B, curr->get_node( slot ));    /* return deleted node to free list */
  for (x = slot; x < curr->num_entries(); x++)
    curr->pull_entry( x, 1);        /* adjust node with removed key */
  curr->dec_entries();
  curr->clr_flag( isFULL );        /* keep flag information up to date */
  if (curr->is_root()) {
    if (curr->num_entries() == 1)
      curr->set_flag( FEWEST );
  }
  else if (curr->num_entries() == get_min_fanout( B, curr ))
    curr->set_flag( FEWEST );
}


/*~~~~~~   merge a node pair & set emptied node up for removal   ~~~~~~*/
Nptr merge(Tree *B, Nptr left, Nptr right, Nptr anchor)
{
  int    x, y, z;

#ifdef DEBUG
  fprintf(stderr, "MERGE:  left %d, right %d.\n", getnodenumber(left), getnodenumber(right));
  showNode(B, left);
  showNode(B, right);
#endif

  if (left->is_internal()) {
    left->inc_entries();            /* copy key separating the nodes */
    B->set_fun_key( right->get_key( 1 ) );    /* defined but maybe just deleted */
    z = get_slot(B, anchor);        /* needs the just calculated key */
    B->set_fun_key( anchor->get_key( z ) );    /* set slot to delete in anchor */
    left->set_entry( left->num_entries(), B->get_fun_key( ), right->get_first_node());
  }
  else
    left->set_next_node( right->get_next_node());
  for (x = left->num_entries() + 1, y = 1; y <= right->num_entries(); x++, y++) {
    left->inc_entries();
    right->xfer_entry( y, left, x);    /* transfer entries to left node */
  }
  if (left->num_entries() > get_min_fanout( B, left ))
    left->clr_flag( FEWEST );
  if (left->num_entries() == get_fanout( B ))
    left->set_flag( isFULL );        /* never happens in even size nodes */

  if (get_merge_path( B ) == left || get_merge_path( B ) == right)
    set_merge_path( B, NONODE );        /* indicate rebalancing is complete */

  return right;
}


/*~~~~~   shift entries in a node pair & adjust anchor key value   ~~~~*/
Nptr shift(Tree *B, Nptr left, Nptr right, Nptr anchor)
{
  int    i, x, y, z;

#ifdef DEBUG
  fprintf(stderr, "SHIFT:  left %d, right %d.\n", getnodenumber(left), getnodenumber(right));
  showNode(B, left);
  showNode(B, right);
#endif

  i = left->is_internal();
  if (left->num_entries() < right->num_entries()) {    /* shift entries to left */
    y = (right->num_entries() - left->num_entries()) >> 1;
    x = left->num_entries() + y;
    B->set_fun_key( right->get_key( y + 1 - i ) );    /* set new anchor key value */
    z = get_slot(B, anchor);            /* find slot in anchor node */
    if (i) {                    /* move out old anchor value */
      right->dec_entries();            /* adjust for shifting anchor */
      left->inc_entries();
      left->set_entry( left->num_entries(), anchor->get_key( z ), right->get_first_node());
      right->set_first_node( right->get_node( y + 1 - i ));
    }
    right->clr_flag( isFULL );
    anchor->set_key( z, B->get_fun_key( ) );        /* set new anchor value */
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

    B->set_fun_key( left->get_key( x ) );            /* set new anchor key value */
    z = get_slot(B, anchor) + 1;
    if (i) {
      left->dec_entries();
      right->inc_entries();
      right->set_entry( y, anchor->get_key( z ), right->get_first_node());
      right->set_first_node( left->get_node( x ));
    }
    left->clr_flag( isFULL );
    anchor->set_key( z, B->get_fun_key( ) );
    for (x = left->num_entries() + i, y -= i; y > 0; y--, x--) {
      left->dec_entries();
      right->inc_entries();
      left->xfer_entry( x, right, y);        /* transfer entries over */
    }
  }
  if (left->num_entries() == get_min_fanout( B, left ))        /* adjust node flags */
    left->set_flag( FEWEST );
  else
    left->clr_flag( FEWEST );            /* never happens in 2-3+trees */
  if (right->num_entries() == get_min_fanout( B, right ))
    right->set_flag( FEWEST );
  else
    right->clr_flag( FEWEST );            /* never happens in 2-3+trees */
  set_merge_path( B, NONODE );

#ifdef DEBUG
  showNode(B, left);
  showNode(B, right);
#endif

  return NONODE;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    Empty Node Utilities                        |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*~~~~~~~~~~~~~~~~~~~~   Set up pool of free nodes   ~~~~~~~~~~~~~~~~~~*/
void init_free_node_pool(Tree *B, int quantity)
{
  int    i;
  Nptr    n;

  B->set_pool_size( quantity );
  B->set_node_array( (Node*)malloc(quantity * sizeof(Node)) );    /* node memory block */
  set_first_free_node( B, node_array_head( B ) );    /* start a list of free nodes */
  for (n = get_first_free_node( B ), i = 0; i < quantity; n++, i++) {
    n->clear_flags();
    n->clear_entries();
    n->set_next_node( n + 1);        /* insert node into free node list */
  }
  (--n)->set_next_node( NONODE);        /* indicates end of free node list */
}


/*~~~~~~~~~~~~~   take a free B+tree node from the pool   ~~~~~~~~~~~~~*/
Nptr get_free_node(Tree *B)
{
  Nptr newNode = get_first_free_node( B );

  if (newNode != NONODE) {
    set_first_free_node( B, newNode->get_next_node( ) );    /* adjust free node list */
    newNode->set_next_node( NONODE);        /* remove node from list */
  }
  else {
    fprintf(stderr, "Out of tree nodes.");    /* can't recover from this */
    exit(0);
  }
  return newNode;
}


/*~~~~~~~~~~~~   return a deleted B+tree node to the pool   ~~~~~~~~~~~*/
void put_free_node(Tree *B, Nptr self)
{
  self->clear_flags();
  self->clear_entries();
  self->set_next_node(  get_first_free_node( B ) );        /* add node to list */
  set_first_free_node( B, self );            /* set it to be list head */
}


/*~~~~~~   fill a free data node with a key and associated data   ~~~~~*/
Nptr get_data_node(Tree *B, keyT key)        /* can add data parameter */
{
  Nptr    newNode = get_free_node(B);
  keyT    *value;

  value = (keyT *) &newNode->X.d;
  *value = key;                    /* can add code to fill node */

  return newNode;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    B+tree Printing Utilities                    |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef DEBUG
/*~~~~~~~~~~~~~~~~~~~~~~   B+tree node printer   ~~~~~~~~~~~~~~~~~~~~~~*/
void showNode(Tree *B, Nptr n)
{
  int x;

  fprintf(stderr, "-  --  --  --  --  --  --  --  --  --  --  --  -\n");
  fprintf(stderr, "| node %6d                 ", getnodenumber(n));
  fprintf(stderr, "  magic    %4x  |\n", getflags(n) & MASK);
  fprintf(stderr, "-  --  --  --  --  --  --  --  --  --  --  --  -\n");
  fprintf(stderr, "| flags   %1d%1d%1d%1d ", isfew(n), isfull(n), isroot(n), isleaf(n));
  fprintf(stderr, "| keys = %5d ", numentries(n));
  fprintf(stderr, "| node = %6d  |\n", getnodenumber(getfirstnode(n)));
  for (x = 1; x <= numentries(n); x++) {
    fprintf(stderr, "| entry %6d ", x);
    fprintf(stderr, "| key = %6d ", getkey(n, x) & 0xFFFF);
    fprintf(stderr, "| node = %6d  |\n", getnodenumber(getnode(n, x)));
  }
  fprintf(stderr, "-  --  --  --  --  --  --  --  --  --  --  --  -\n");
}

/*~~~~~~~~~~~~~~~~~   B+tree class variable printer   ~~~~~~~~~~~~~~~~~*/
void showBtree(Tree *B)
{
  fprintf(stderr, "-  --  --  --  --  --  -\n");
  fprintf(stderr, "|  B+tree  %10p  |\n", (void *) B);
  fprintf(stderr, "-  --  --  --  --  --  -\n");
  fprintf(stderr, "|  root        %6d  |\n", getnodenumber(getroot));
  fprintf(stderr, "|  leaf        %6d  |\n", getnodenumber(getleaf));
  fprintf(stderr, "|  fanout         %3d  |\n", getfanout + 1);
  fprintf(stderr, "|  minfanout      %3d  |\n", getminfanout(getroot) + 1);
  fprintf(stderr, "|  height         %3d  |\n", gettreeheight);
  fprintf(stderr, "|  freenode    %6d  |\n", getnodenumber(getfirstfreenode));
  fprintf(stderr, "|  theKey      %6d  |\n", getfunkey);
  fprintf(stderr, "|  theData     %6s  |\n", getfundata( B ));
  fprintf(stderr, "-  --  --  --  --  --  -\n");
}
#endif

/*~~~~~~~~~~~~~~~~~~~~~~   B+tree data printer   ~~~~~~~~~~~~~~~~~~~~~~*/
void list_btree_values(Tree *B, Nptr n, int num)
{
  int slot, prev = -1;

  for (slot = 1; (n != NONODE) && num && n->num_entries(); num--) {
    if ( n->get_key( slot ) <= prev) fprintf(stderr, "BOMB");
    prev = n->get_key( slot );
    fprintf(stderr, "%8d%c", prev, (num & 7 ? ' ' : '\n'));
    if (++slot > n->num_entries())
      n = n->get_next_node(), slot = 1;
  }
  fprintf(stderr, "\n\n");
}

/*~~~~~~~~~~~~~~~~~~~   entire B+tree data printer   ~~~~~~~~~~~~~~~~~~*/
void list_all_btree_values( Tree *B )
{
  list_btree_values( B, get_leaf( B ), ERROR );
}
