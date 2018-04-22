#ifndef ROMZ_JANNINK_ERASE_H
#define ROMZ_JANNINK_ERASE_H

#include "insert.h"

class Erase : public Insert
{
public:
    Erase( std::uint32_t pool_size );
    ~Erase() = default;

    void erase( Key key );

private:
    Node* shift( Node* left, Node* right, Node* anchor );
    Node* merge( Node* left, Node* right, Node* anchor );
    void remove_entry( Node* curr, int slot );
    Node* descend_balance( Node* curr, Node* left, Node* right, Node* lAnc, Node* rAnc, Node* parent );
    void collapse_root( Node* old_root, Node* new_root );


};

#endif




