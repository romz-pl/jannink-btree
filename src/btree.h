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
    Tree( int poolsz, int fan, KeyCmp keyCmp );
    ~Tree();


    // access key and data values for B+tree methods
    keyT get_fun_key( ) const;
    void set_fun_key( keyT v );

    dataT get_fun_data( ) const;
    void set_fun_data( const char* v );

     // define number of B+tree nodes for free node pool
    int get_pool_size( ) const;
    void set_pool_size( int v );

    // access memory region containing B+tree nodes
    Node* get_node_array( ) const;
    void set_node_array( Node* v );

    // locations from which tree access begins
    Nptr get_root( ) const;
    void set_root( Nptr v );

    Nptr get_leaf( ) const;
    void set_leaf( Nptr v );

    // define max/min number of pointers per node
    int get_fanout( ) const;
    void set_fanout( int v );

    int get_min_fanout( const Node* j ) const;
    void set_min_fanout( int v );

    // manage B+tree height
    void init_tree_height( );
    void inc_tree_height( );
    void dec_tree_height( );
    int get_tree_height( ) const;

    // access pool of free nodes
    Nptr get_first_free_node( ) const;
    void set_first_free_node( Nptr v );

    // handle split/merge points during insert/delete
    Nptr get_split_path( ) const;
    void set_split_path( Nptr v );

    Nptr get_merge_path( ) const;
    void set_merge_path( Nptr v );

    // exploit function to compare two B+tree keys
    KeyCmp compare_keys( ) const;
    void set_compare_keys( KeyCmp v );

    bool is_node( Node* j ) const;
    bool isnt_node( Node* j ) const;


    void put_free_node( Nptr self );

    Nptr get_data_node( keyT key );

    Nptr NONODE() const;
    Nptr node_array_head( ) const;

    void list_all_btree_values( ) const;
    void list_btree_values( Nptr n, int num ) const;

    Nptr get_free_node();
    void init_free_node_pool( int quantity );

    int best_match( Nptr curr, int slot );
    int find_key( Nptr curr, int lo, int hi );
    int get_slot( Nptr curr );
    Nptr descend_to_leaf( Nptr curr );
    Nptr search( keyT key );

    void place_entry( Nptr newNode, int slot, Nptr downPtr );
    void insert_entry( Nptr newNode, int slot, Nptr sibling, Nptr downPtr );
    Nptr split( Nptr newNode );

public:

    /* `private' variables */
    int     pool_size;    /* # of nodes allocated for tree */
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
/* Tree    *remakeBtree(Tree * B, int fillfactor); */

#ifdef DEBUG
void    showNode(Tree *B, Nptr node);
void    showBtree(Tree *B);
#endif



void btree_insert(Tree *B, keyT key);
void btree_delete(Tree *B, keyT key);





#endif

