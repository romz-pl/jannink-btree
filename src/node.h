#ifndef ROMZ_JANNINK_NODE_H
#define ROMZ_JANNINK_NODE_H





/*~~~~~~~~~~~~~~~~~~~~~~~~    node pointer and key type    ~~~~~~~*/
class Node;
typedef Node    *Nptr;    /* streamlined pointer representation */

typedef int        keyT;    /* adapt key type to requirements */
typedef char        *dataT;    /* adapt data type to requirements */

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
|    Sample Key Comparison Function                    |
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

// int compare_keys(keyT key1, keyT key2);
inline int compare_keys(keyT key1, keyT key2)
{
  return key1 - key2;        /* this is the case when keys are integer */
}

/*~~~~~~~~~~~~~~~~~~~~~~~~    key comparison function type    ~~~~~~~*/
typedef int (*KeyCmp)(keyT, keyT);


/*~~~~~~~~~~~~~~~~~~~~~~~~    node status    ~~~~~~~~~~~~~~~~~~~~~~~*/
class State
{
public:
    short flags;
    short pairs;
};


static_assert( sizeof( State ) <= sizeof( keyT ), "sizeof(State) must be <= sizeof(keyT)" );


/*~~~~~~~~~~~~~~    single node entry with key value    ~~~~~~~*/
class Entry
{
public:
    keyT key;            /* can be a hashed value */
    Nptr downNode;
};            /* WARNING: entry was a RESERVED word in C */


/*~~~~~~~~~~~~~~~~~~~~    special header entry for internal node    ~~~~~~~*/
class Inner
{
public:
    State info;
    Nptr firstNode;        /* node of smallest values */
};


/*~~~~~~~~~~~~~~~~~~~~    special header entry for leaf node    ~~~~~~~*/
class Leaf
{
public:
    State info;
    Nptr nextNode;        /* next leaf in sequential scan */
};


/*~~~~~~~~~~~~~~~~~~~~    unstructured data node    ~~~~~~~~~~~~~~~~~~~~~~~*/
class Data
{
public:
    /* 24 bytes is minimal size for 2-3 trees */
    static constexpr int NODE_SIZE = 72;
    char value[ NODE_SIZE ];
};


/*~~~~~~~~~~~~    data node header to handle duplicates    ~~~~~~~~~~~~~~~*/
class DupData
{
public:
    /*  16 bytes to store a data point */
    static constexpr int DATA_SIZE = 16;

    int   copy;            /* tallies the duplicate keys */
    Nptr  next;            /* next node with same key value */
    char  value[ DATA_SIZE ];
};


/*~~~~~~~~~~~~~~~~~~~~~~~~    structured tree node    ~~~~~~~~~~~~~~~*\
|
|    The structures Entry, Inner and Leaf are all identical in size.
|    Each node is of size:  fanout * sizeof(Entry).  Through the
|    union X, it is possible to access the first space in any node
|    as: X.e[0], X.i, X.l, depending on the algorithms' needs.  The
|    value of the status flag isLEAF should always determine how the
|    first node space is used.  The node structure is defined below.
|
|    Internal node:            Leaf node:
|
|    +---------------+        +---------------+
|    |Inner: X.i     |        |Leaf: X.l      |
|    |  info         |        |  info         |
|    |  firstNode    |        |  nextNode     |
|    |---------------|        |---------------|
|    |Entry: X.e[1]  |        |Entry: X.e[1]  |
|    |  key          |        |  key          |
|    |  downNode     |        |  downNode     |
|    |---------------|        |---------------|
|    |    .          |        |    .          |
|    |    .          |        |    .          |
|    |    .          |        |    .          |
|    |---------------|        |---------------|
|    |Entry: X.e[n]  |        |Entry: X.e[n]  |
|    |  key          |        |  key          |
|    |  downNode     |        |  downNode     |
|    +---------------+        +---------------+
|
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
class Node
{
public:
    //
    // flag bits (5 of 16 used, 11 for magic value)
    //

    /* bits set at node creation/split/merge */
    static constexpr int isLEAF = 0x1;
    static constexpr int isROOT = 0x2;

    /* bits set at key insertion/deletion */
    static constexpr int isFULL = 0x4;
    static constexpr int FEWEST = 0x8;
    static constexpr int FLAGS = 0xF;

    /* identifies data as being a B+tree node */
    static constexpr int MAGIC = 0xDEC0;
    static constexpr int MASK = 0xFFF0;

public:
    keyT get_key( int q ) const;
    void set_key( int q, keyT v );

    Nptr get_node( int q ) const;
    void set_node( int q, Nptr v );

    void set_flag( short v );
    void clr_flag( short v );
    void clear_flags( );
    short get_flags( ) const;

    // test individual flag values
    bool is_internal( ) const;
    bool is_leaf( ) const;
    bool is_root( ) const;
    bool is_full( ) const;
    bool is_few( ) const;

    short num_entries( ) const;
    void clear_entries( );
    void inc_entries( );
    void dec_entries( );

    // manage first/last node pointers in internal nodes
    void set_first_node( Nptr v );
    Nptr get_first_node( ) const;
    Nptr get_last_node( ) const;

    // manage pointers to next nodes in leaf nodes
    void set_next_node( Nptr v );
    Nptr get_next_node( ) const;

    // shift/transfer entries for insertion/deletion
    void push_entry( int q, int v );
    void pull_entry( int q, int v );
    void xfer_entry( int q, Node* v, int z ) const;
    void set_entry( int q, keyT v, Nptr z );

public:
    /* ARRAY is a place holder value for:  fanout */
    static constexpr int ARRAY = 1;
    union
    {
        Entry   e[ ARRAY ];    /* allows access to entry array */
        Inner   i;
        Leaf    l;
        Data    d;
        DupData dd;
    } X;
} ;



#endif




