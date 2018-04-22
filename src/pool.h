#ifndef ROMZ_JANNINK_POOL_H
#define ROMZ_JANNINK_POOL_H

#include <cstdint>
#include "node.h"

class Pool
{
public:
    Pool( std::uint32_t size );
    ~Pool();

    Node* get_free_node();
    void put_free_node( Node* self );

private:
    Node* get_first_free_node( ) const;
    void set_first_free_node( Node* v );

private:
    Node* m_tree;

    Node* m_pool;

};


#endif




