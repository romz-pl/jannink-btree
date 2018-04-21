#ifndef ROMZ_JANNINEK_BTREE_H
#define ROMZ_JANNINEK_BTREE_H


#include "node.h"

//
// tree definitions
//
class Tree
{
private:
    /* special node slot values used in key search */
    static constexpr int ERROR = -1;
    static constexpr int UPPER = -2;
    static constexpr int LOWER = -3;

public:
    Tree( int m_pool_size );
    ~Tree();


    // access key and data values for B+tree methods
    Key get_fun_key( ) const;
    void set_fun_key( Key v );

    data_type get_fun_data( ) const;
    void set_fun_data( const char* v );

    // access memory region containing B+tree nodes
    Node* get_node_array( ) const;
    void set_node_array( Node* v );

    // locations from which tree access begins
    Node* get_root( ) const;
    void set_root( Node* v );

    Node* get_leaf( ) const;
    void set_leaf( Node* v );

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
    Node* get_first_free_node( ) const;
    void set_first_free_node( Node* v );

    // handle split/merge points during insert/delete
    Node* get_split_path( ) const;
    void set_split_path( Node* v );

    Node* get_merge_path( ) const;
    void set_merge_path( Node* v );

    bool is_node( Node* j ) const;
    bool isnt_node( Node* j ) const;


    void put_free_node( Node* self );

    Node* get_data_node( Key key );

    Node* NO_NODE() const;
    Node* node_array_head( ) const;

    Node* get_free_node();
    void init_free_node_pool();

    int best_match( Node* curr, const int slot );
    int find_key( Node* curr, int lo, int hi );
    int get_slot( Node* curr );
    Node* descend_to_leaf( Node* curr );
    Node* search( Key key );

    void place_entry( Node* newNode, int slot, Node* downPtr );
    void insert_entry( Node* newNode, const int slot, Node* sibling, Node* downPtr );
    Node* split( Node* newNode );
    void make_new_root( Node* oldRoot, Node* newNode );
    Node* descend_split( Node* curr );
    void insert( Key key );

    Node* shift( Node* left, Node* right, Node* anchor );
    Node* merge( Node* left, Node* right, Node* anchor );
    void remove_entry( Node* curr, int slot );
    Node* descend_balance( Node* curr, Node* left, Node* right, Node* lAnc, Node* rAnc, Node* parent );
    void collapse_root( Node* oldRoot, Node* newRoot );
    void erase( Key key );

#ifdef DEBUG
    void show_node( Node* n ) const;
    void show_btree( ) const;
    void list_all_btree_values( ) const;
    void list_btree_values( Node* n ) const;
#endif


private:

    // Defines number of B+tree nodes for free node pool
    const int m_pool_size;

    // pointer to array of nodes (NOT Nptr !)
    Node* m_tree;

    // pointer to root node
    Node* m_root;

    // pointer to first leaf node in B+tree
    Node* m_leaf;

    // # of pointers to other nodes
    int m_fanout;

    // usually min_fanout == ceil(fanout / 2)
    int m_min_fanout;

    // nodes traversed from root to leaves
    int m_height;

    // list of empty nodes
    Node* m_pool;

    //  the key value used in tree operations
    Key m_the_key;

    //  data used for insertions/deletions
    data_type m_the_data;

    union /* nodes to change in insert and delete */
    {
        Node* split;
        Node* merge;
    } branch;
};


#endif

