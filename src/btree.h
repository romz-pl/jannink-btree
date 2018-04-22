#ifndef ROMZ_JANNINEK_BTREE_H
#define ROMZ_JANNINEK_BTREE_H

#include "core.h"


//
// tree definitions
//
class Tree : public Core
{

public:
    Tree( std::uint32_t pool_size );
    ~Tree();

    Node* search( Key key );
    void insert( Key key );
    void erase( Key key );

private:
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


};


#endif

