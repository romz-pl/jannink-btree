#ifndef ROMZ_JANNINEK_BTREE_H
#define ROMZ_JANNINEK_BTREE_H

/*~~~~~~~~~~~~~~~~    sample B+tree parametrization    ~~~~~~~~~~~~~~~*/
                /* causes printing of node information */
#define DEBUG 1
#undef DEBUG

/* 24 bytes is minimal size for 2-3 trees */
const int NODE_SIZE = 72;

/*  16 bytes to store a data point */
const int DATA_SIZE = 16;

/* maximum number of available nodes */
const int ARRAY_SIZE = 24576;


/*~~~~~~~    flag bits (5 of 16 used, 11 for magic value)    ~~~~~~~*/
/* bits set at node creation/split/merge */
const int isLEAF = 0x1;
const int isROOT = 0x2;

/* bits set at key insertion/deletion */
const int isFULL = 0x4;
const int FEWEST = 0x8;
const int FLAGS = 0xF;

/* identifies data as being a B+tree node */
const int MAGIC = 0xDEC0;
const int MASK = 0xFFF0;





/*~~~~~~~~~~~~~~~~~~~~~~~~    constants    ~~~~~~~~~~~~~~~~~~~~~~~*/
/* ARRAY is a place holder value for:  fanout */
const int ARRAY = 1;

/* special node slot values used in key search */
const int ERROR = -1;
const int UPPER = -2;
const int LOWER = -3;


/*~~~~~~~~~~~~~~~~~~~~~~~~    node pointer and key type    ~~~~~~~*/
typedef struct node    *Nptr;    /* streamlined pointer representation */

typedef int        keyT;    /* adapt key type to requirements */
typedef char        *dataT;    /* adapt data type to requirements */


/*~~~~~~~~~~~~~~~~~~~~~~~~    node status    ~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct state {
  short    flags;
  short    pairs;
} State;            /* sizeof(State) must be <= sizeof(keyT) */


/*~~~~~~~~~~~~~~    single node entry with key value    ~~~~~~~*/
typedef struct entrY {
  keyT    key;            /* can be a hashed value */
  Nptr    downNode;
} Entry;            /* WARNING: entry was a RESERVED word in C */


/*~~~~~~~~~~~~~~~~~~~~    special header entry for internal node    ~~~~~~~*/
typedef struct inner {
  State info;
  Nptr    firstNode;        /* node of smallest values */
} Inner;


/*~~~~~~~~~~~~~~~~~~~~    special header entry for leaf node    ~~~~~~~*/
typedef struct leaf {
  State info;
  Nptr    nextNode;        /* next leaf in sequential scan */
} Leaf;


/*~~~~~~~~~~~~~~~~~~~~    unstructured data node    ~~~~~~~~~~~~~~~~~~~~~~~*/
typedef struct data {
  char    value[NODE_SIZE];
} Data;


/*~~~~~~~~~~~~    data node header to handle duplicates    ~~~~~~~~~~~~~~~*/
typedef struct dupdata {
  int    copy;            /* tallies the duplicate keys */
  Nptr    next;            /* next node with same key value */
  char  value[DATA_SIZE];
} DupData;


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
typedef struct node {
  union {
    Entry    e[ARRAY];    /* allows access to entry array */
    Inner    i;
    Leaf    l;
    Data    d;
    DupData    dd;
  } X;
} Node;


/*~~~~~~~~~~~~~~~~~~~~~~~~    key comparison function type    ~~~~~~~*/
typedef int (*KeyCmp)(keyT, keyT);


/*~~~~~~~~~~~~~~~~~~~~~~~~    tree definitions    ~~~~~~~~~~~~~~~*/
class Tree {
public:
            /* `private' variables */
  int         poolsize;    /* # of nodes allocated for tree */
  Node        *tree;        /* pointer to array of nodes (NOT Nptr !) */
  Nptr        root;        /* pointer to root node */
  Nptr        leaf;        /* pointer to first leaf node in B+tree */
  int         fanout;        /* # of pointers to other nodes */
  int         minfanout;    /* usually minfanout == ceil(fanout/2) */
  int         height;        /* nodes traversed from root to leaves */
  Nptr        pool;        /* list of empty nodes */
  keyT        theKey;        /*  the key value used in tree operations */
  dataT        theData;    /*  data used for insertions/deletions */
  union {            /* nodes to change in insert and delete */
    Nptr    split;
    Nptr    merge;
  } branch;
  KeyCmp    keycmp;        /* pointer to function comparing two keys */
};


/*~~~~~~~~~~~~~~~~~~~~~~~~    B+tree methods        ~~~~~~~~~~~~~~~*/
Tree    *btree_init(int poolsz, int fan, KeyCmp keyCmp);
/* Tree    *remakeBtree(Tree * B, int fillfactor); */
void    btree_free(Tree *B);

#ifdef DEBUG
void    showNode(Tree *B, Nptr node);
void    showBtree(Tree *B);
#endif

void    list_btree_values(Tree *B, Nptr start, int count);
void    list_all_btree_values(Tree *B);

int    compare_keys(keyT key1, keyT key2);

Nptr    btree_search(Tree *B, keyT key);
void    btree_insert(Tree *B, keyT key);
void    btree_delete(Tree *B, keyT key);



/* low level definition of Nptr value usage */
// #define nodearrayhead B->tree
inline Node* node_array_head( Tree *B )
{
    return B->tree;
}

/* corresponds to a NULL node pointer value */
#define NONODE    (node_array_head( B ) - 1)


#endif

