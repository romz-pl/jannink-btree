#ifndef ROMZ_JANNINEK_BTREE_H
#define ROMZ_JANNINEK_BTREE_H

#include "insert.h"


//
// tree definitions
//
class Tree : public Insert
{

public:
    Tree( std::uint32_t pool_size );
    ~Tree();



    void erase( Key key );

private:



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

