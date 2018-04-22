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

    int best_match_lower( Node* curr, const int slot );
    int best_match_upper( Node* curr, const int slot );

private:
    // special node slot values used in key search
    static constexpr int UPPER = -2;
    static constexpr int LOWER = -3;

};

#endif




