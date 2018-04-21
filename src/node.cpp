#include "node.h"

/* access keys and pointers in a node */
// #define getkey(j, q) nAdr(j).e[(q)].key
Key Node::get_key( int q ) const
{
    return e[ q ].key;
}

// #define getnode(j, q) nAdr(j).e[(q)].downNode
Node* Node::get_node( int q ) const
{
    return e[ q ].downNode;
}

// #define setkey(j, q, v) (nAdr(j).e[(q)].key = (v))
void Node::set_key( int q, Key v )
{
    e[ q ].key = v;
}

// #define setnode(j, q, v) (nAdr(j).e[(q)].downNode = (v))
void Node::set_node( int q, Node* v )
{
    e[ q ].downNode =  v;
}

/* access node flag values */
// #define setflag(j, v) (nAdr(j).i.info.flags |= (v))
void Node::set_flag( short v )
{
    i.info.flags |= v;
}

// #define clrflag(j, v) (nAdr(j).i.info.flags &= ~(v))
void Node::clr_flag( short v )
{
    i.info.flags &= ~v;
}

// #define getflags(j) nAdr(j).i.info.flags
short Node::get_flags( ) const
{
    return i.info.flags;
}

// #define clearflags(j) (nAdr(j).i.info.flags = (short) MAGIC)
void Node::clear_flags( )
{
    i.info.flags = static_cast< short >( MAGIC );
}

// #define isinternal(j) ((nAdr(j).i.info.flags & isLEAF) == 0)
bool Node::is_internal( ) const
{
    const bool ret = ( ( i.info.flags & isLEAF ) == 0 );
    return ret;
}

// #define isleaf(j) ((nAdr(j).i.info.flags & isLEAF) == isLEAF)
bool Node::is_leaf( ) const
{
    const bool ret = ( ( i.info.flags & isLEAF ) == isLEAF );
    return ret;
}

// #define isroot(j) ((nAdr(j).i.info.flags & isROOT) == isROOT)
bool Node::is_root( ) const
{
    const bool ret = ( ( i.info.flags & isROOT ) == isROOT );
    return ret;
}

// #define isfull(j) ((nAdr(j).i.info.flags & isFULL) == isFULL)
bool Node::is_full( ) const
{
    const bool ret = ( ( i.info.flags & isFULL ) == isFULL );
    return ret;
}

// #define isfew(j) ((nAdr(j).i.info.flags & FEWEST) == FEWEST)
bool Node::is_few( ) const
{
    const bool ret = ( ( i.info.flags & FEWEST ) == FEWEST );
    return ret;
}

/* manage number of keys in a node */
// #define numentries(j) nAdr(j).i.info.pairs
short Node::num_entries( ) const
{
    return i.info.pairs;
}

// #define clearentries(j) (nAdr(j).i.info.pairs = 0)
void Node::clear_entries( )
{
    i.info.pairs = 0;
}

// #define incentries(j) (nAdr(j).i.info.pairs++)
void Node::inc_entries( )
{
    i.info.pairs++;
}

// #define decentries(j) (nAdr(j).i.info.pairs--)
void Node::dec_entries( )
{
    i.info.pairs--;
}


// #define setfirstnode(j, v) (nAdr(j).i.firstNode = (v))
void Node::set_first_node( Node* v )
{
    i.firstNode = v;
}

// #define getfirstnode(j) nAdr(j).i.firstNode
Node* Node::get_first_node( ) const
{
    return i.firstNode;
}

// #define getlastnode(j) nAdr(j).e[nAdr(j).i.info.pairs].downNode
Node* Node::get_last_node( ) const
{
    return e[ i.info.pairs ].downNode;
}

// #define setnextnode(j, v) (nAdr(j).l.nextNode = (v))
void Node::set_next_node( Node* v )
{
    l.nextNode = v;
}

// #define getnextnode(j) nAdr(j).l.nextNode
Node* Node::get_next_node( ) const
{
    return l.nextNode;
}


// #define pushentry(j, q, v) (nAdr(j).e[(q) + (v)] = nAdr(j).e[(q)])
void Node::push_entry( int q, int v )
{
    e[ q + v ] = e[ q ];
}

// #define pullentry(j, q, v) (nAdr(j).e[(q)] = nAdr(j).e[(q) + (v)])
void Node::pull_entry( int q, int v )
{
    e[ q ] = e[ q + v ];
}


// #define xferentry(j, q, v, z) (nAdr(v).e[(z)] = nAdr(j).e[(q)])
void Node::xfer_entry( int q, Node* v, int z ) const
{
    v->e[ z ] = e[ q ];
}


// #define setentry(j, q, v, z) (nAdr(j).e[(q)].key = (v), nAdr(j).e[(q)].downNode = (z))
void Node::set_entry( int q, Key v, Node* z )
{
    e[ q ].key = v;
    e[ q ].downNode = z;
}
