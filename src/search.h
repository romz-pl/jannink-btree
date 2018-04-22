#ifndef ROMZ_JANNINK_SEARCH_H
#define ROMZ_JANNINK_SEARCH_H

#include "core.h"

class Search : public Core
{
public:
    Search( std::uint32_t pool_size );
    ~Search() = default;

    Node* search( Key key );

protected:
    int get_slot( Node* curr );

private:
    int best_match( Node* curr, const int slot );
    int find_key( Node* curr, int lo, int hi );

    Node* descend_to_leaf( Node* curr );

};

#endif




