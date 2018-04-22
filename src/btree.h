#ifndef ROMZ_JANNINEK_BTREE_H
#define ROMZ_JANNINEK_BTREE_H


#include "node.h"
#include "pool.h"

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
    Tree( std::uint32_t pool_size );
    ~Tree();

    Node* search( Key key );
    void insert( Key key );
    void erase( Key key );

private:
    // access key and data values for B+tree methods
    Key get_fun_key( ) const;
    void set_fun_key( Key v );

    data_type get_fun_data( ) const;
    void set_fun_data( const char* v );

    // manage B+tree height
    void inc_tree_height( );
    void dec_tree_height( );

    // handle split/merge points during insert/delete
    Node* get_split_path( ) const;
    void set_split_path( Node* v );

    Node* get_merge_path( ) const;
    void set_merge_path( Node* v );

    Node* get_data_node( Key key );

    int best_match( Node* curr, const int slot );
    int find_key( Node* curr, int lo, int hi );
    int get_slot( Node* curr );
    Node* descend_to_leaf( Node* curr );


    void place_entry( Node* new_node, int slot, Node* down_ptr );
    void insert_entry( Node* new_node, const int slot, Node* sibling, Node* down_ptr );
    Node* split( Node* newNode );
    void make_new_root( Node* old_root, Node* new_node );
    Node* descend_split( Node* curr );


    Node* shift( Node* left, Node* right, Node* anchor );
    Node* merge( Node* left, Node* right, Node* anchor );
    void remove_entry( Node* curr, int slot );
    Node* descend_balance( Node* curr, Node* left, Node* right, Node* lAnc, Node* rAnc, Node* parent );
    void collapse_root( Node* old_root, Node* new_root );


#ifdef DEBUG
    void show_node( Node* n ) const;
    void show_btree( ) const;
    void list_all_btree_values( ) const;
    void list_btree_values( Node* n ) const;
#endif


private:

    Pool m_pool;

    // pointer to root node
    Node* m_root;

    // pointer to first leaf node in B+tree
    Node* m_leaf;

    // # of pointers to other nodes
    const int m_fanout = Data::NODE_SIZE / sizeof( Entry ) - 1; // Why (-1) ???

    // usually min_fanout == ceil(fanout / 2)
    const int m_min_fanout = ( ( m_fanout + 1 ) >> 1 ) - 1;

    // nodes traversed from root to leaves
    std::uint32_t m_height;

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

