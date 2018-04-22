#ifndef ROMZ_JANNINEK_BTREE_H
#define ROMZ_JANNINEK_BTREE_H

#include "erase.h"


//
// tree definitions
//
class Tree : public Erase
{

public:
    Tree( std::uint32_t pool_size );
    ~Tree() = default;





private:

#ifdef DEBUG
    void show_node( Node* n ) const;
    void show_btree( ) const;
    void list_all_btree_values( ) const;
    void list_btree_values( Node* n ) const;
#endif


};


#endif

