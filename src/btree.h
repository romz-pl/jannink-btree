#ifndef ROMZ_JANNINEK_BTREE_H
#define ROMZ_JANNINEK_BTREE_H

/*~~~~~~~~~~~~~~~~    sample B+tree parametrization    ~~~~~~~~~~~~~~~*/
                /* causes printing of node information */
#define DEBUG 1
#undef DEBUG

#include "node.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~    tree definitions    ~~~~~~~~~~~~~~~*/
class Tree
{
public:
    // access key and data values for B+tree methods
    keyT get_fun_key( ) const;
    void set_fun_key( keyT v );

    dataT get_fun_data( ) const;
    void set_fun_data( const char* v );

     // define number of B+tree nodes for free node pool
    int get_pool_size( ) const;


public:

    /* `private' variables */
    int     poolsize;    /* # of nodes allocated for tree */
    Node    *tree;        /* pointer to array of nodes (NOT Nptr !) */
    Nptr    root;        /* pointer to root node */
    Nptr    leaf;        /* pointer to first leaf node in B+tree */
    int     fanout;        /* # of pointers to other nodes */
    int     minfanout;    /* usually minfanout == ceil(fanout/2) */
    int     height;        /* nodes traversed from root to leaves */
    Nptr    pool;        /* list of empty nodes */
    keyT    theKey;        /*  the key value used in tree operations */
    dataT   theData;    /*  data used for insertions/deletions */

    union /* nodes to change in insert and delete */
    {
        Nptr    split;
        Nptr    merge;
    } branch;

    KeyCmp    keycmp;        /* pointer to function comparing two keys */
};


/*~~~~~~~~~~~~~~~~~~~~~~~~    B+tree methods        ~~~~~~~~~~~~~~~*/
Tree* btree_init( int poolsz, int fan, KeyCmp keyCmp );
/* Tree    *remakeBtree(Tree * B, int fillfactor); */
void btree_free(Tree *B);

#ifdef DEBUG
void    showNode(Tree *B, Nptr node);
void    showBtree(Tree *B);
#endif

void list_btree_values(Tree *B, Nptr start, int count);
void list_all_btree_values(Tree *B);

int compare_keys(keyT key1, keyT key2);

Nptr btree_search(Tree *B, keyT key);
void btree_insert(Tree *B, keyT key);
void btree_delete(Tree *B, keyT key);



/* low level definition of Nptr value usage */
// #define nodearrayhead B->tree
inline Node* node_array_head( Tree *B )
{
    return B->tree;
}

/* corresponds to a NULL node pointer value */
#define NONODE    (node_array_head( B ) - 1)


#endif

