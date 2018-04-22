#ifndef ROMZ_JANNINK_INSERT_H
#define ROMZ_JANNINK_INSERT_H

#include "search.h"

class Insert : public Search
{
public:
    Insert( std::uint32_t pool_size );
    ~Insert() = default;

    void insert( Key key );

private:
    void place_entry( Node* new_node, int slot, Node* down_ptr );
    void insert_entry( Node* new_node, const int slot, Node* sibling, Node* down_ptr );
    Node* split( Node* newNode );
    void make_new_root( Node* old_root, Node* new_node );
    Node* descend_split( Node* curr );
};


#endif




