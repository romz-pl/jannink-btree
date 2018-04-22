#ifndef ROMZ_JANNINK_CORE_H
#define ROMZ_JANNINK_CORE_H

#include "node.h"
#include "pool.h"

class Core
{
public:
    Core( std::uint32_t pool_size );
    ~Core();

protected:
    /* special node slot values used in key search */
    static constexpr int ERROR = -1;
    static constexpr int UPPER = -2;
    static constexpr int LOWER = -3;

public:
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

protected:

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


private:
    union /* nodes to change in insert and delete */
    {
        Node* split;
        Node* merge;
    } branch;


};



#endif




