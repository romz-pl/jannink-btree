#include "node.h"

/* access keys and pointers in a node */
// #define getkey(j, q) nAdr(j).e[(q)].key
keyT get_key( const Node* j, int q )
{
    return j->X.e[ q ].key;
}

// #define getnode(j, q) nAdr(j).e[(q)].downNode
Nptr get_node( const Node* j, int q )
{
    return j->X.e[ q ].downNode;
}

// #define setkey(j, q, v) (nAdr(j).e[(q)].key = (v))
void set_key( Node* j, int q, keyT v )
{
    j->X.e[ q ].key = v;
}

// #define setnode(j, q, v) (nAdr(j).e[(q)].downNode = (v))
void set_node( Node* j, int q, Nptr v )
{
    j->X.e[ q ].downNode =  v;
}

            /* access node flag values */
// #define setflag(j, v) (nAdr(j).i.info.flags |= (v))
void set_flag( Node* j, short v )
{
    j->X.i.info.flags |= v;
}

// #define clrflag(j, v) (nAdr(j).i.info.flags &= ~(v))
void clr_flag( Node* j, short v )
{
    j->X.i.info.flags &= ~v;
}

// #define getflags(j) nAdr(j).i.info.flags
short get_flags( const Node* j )
{
    return j->X.i.info.flags;
}

// #define clearflags(j) (nAdr(j).i.info.flags = (short) MAGIC)
void clear_flags( Node* j )
{
    j->X.i.info.flags = static_cast< short >( MAGIC );
}



/* test individual flag values */
// #define isinternal(j) ((nAdr(j).i.info.flags & isLEAF) == 0)
bool is_internal( Node* j)
{
    return ( ( j->X.i.info.flags & isLEAF ) == 0 );
}

// #define isleaf(j) ((nAdr(j).i.info.flags & isLEAF) == isLEAF)
bool is_leaf( Node* j )
{
    return ( ( j->X.i.info.flags & isLEAF ) == isLEAF );
}

// #define isroot(j) ((nAdr(j).i.info.flags & isROOT) == isROOT)
bool is_root( Node* j )
{
    return ( ( j->X.i.info.flags & isROOT ) == isROOT );
}

// #define isfull(j) ((nAdr(j).i.info.flags & isFULL) == isFULL)
bool is_full( Node* j )
{
    return ( ( j->X.i.info.flags & isFULL ) == isFULL );
}

// #define isfew(j) ((nAdr(j).i.info.flags & FEWEST) == FEWEST)
bool is_few( Node* j )
{
    return ( ( j->X.i.info.flags & FEWEST ) == FEWEST );
}

/* manage number of keys in a node */
// #define numentries(j) nAdr(j).i.info.pairs
short num_entries( Node* j )
{
    return j->X.i.info.pairs;
}

// #define clearentries(j) (nAdr(j).i.info.pairs = 0)
void clear_entries( Node* j )
{
    j->X.i.info.pairs = 0;
}

// #define incentries(j) (nAdr(j).i.info.pairs++)
void inc_entries( Node * j )
{
    j->X.i.info.pairs++;
}

// #define decentries(j) (nAdr(j).i.info.pairs--)
void dec_entries( Node* j )
{
    j->X.i.info.pairs--;
}

/* manage first/last node pointers in internal nodes */
// #define setfirstnode(j, v) (nAdr(j).i.firstNode = (v))
void set_first_node( Node* j, Nptr v )
{
    j->X.i.firstNode = v;
}

// #define getfirstnode(j) nAdr(j).i.firstNode
Nptr getfirstnode( Node* j )
{
    return j->X.i.firstNode;
}

// #define getlastnode(j) nAdr(j).e[nAdr(j).i.info.pairs].downNode
Nptr get_last_node( Node* j )
{
    return j->X.e[ j->X.i.info.pairs ].downNode;
}

/* manage pointers to next nodes in leaf nodes */
// #define setnextnode(j, v) (nAdr(j).l.nextNode = (v))
void set_next_node( Node* j, Nptr v )
{
    j->X.l.nextNode = v;
}

// #define getnextnode(j) nAdr(j).l.nextNode
Nptr get_next_node( Node* j )
{
    return j->X.l.nextNode;
}

/* shift/transfer entries for insertion/deletion */
// #define pushentry(j, q, v) (nAdr(j).e[(q) + (v)] = nAdr(j).e[(q)])
void push_entry( Node* j, int q, int v )
{
    j->X.e[ q + v ] = j->X.e[ q ];
}

// #define pullentry(j, q, v) (nAdr(j).e[(q)] = nAdr(j).e[(q) + (v)])
void pull_entry( Node* j, int q, int v )
{
    j->X.e[ q ] = j->X.e[ q + v ];
}


// #define xferentry(j, q, v, z) (nAdr(v).e[(z)] = nAdr(j).e[(q)])
void xfer_entry( Node* j, int q, Node* v, int z )
{
    v->X.e[ z ] = j->X.e[ q ];
}


// #define setentry(j, q, v, z) (nAdr(j).e[(q)].key = (v), nAdr(j).e[(q)].downNode = (z))
void set_entry( Node* j, int q, keyT v, Nptr z )
{
    j->X.e[ q ].key = v;
    j->X.e[ q ].downNode = z;
}
