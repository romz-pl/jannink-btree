#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "btree.h"


// causes printing of node information
#define DEBUG 1
#undef DEBUG


//
//
//
Tree::Tree( std::uint32_t pool_size )
    : Erase( pool_size )
{

}


#ifdef DEBUG

//
// B+tree node printer
//
void Tree::show_node( Node* n ) const
{

    fprintf(stderr, "------------------------------------------------\n");
    fprintf(stderr, "| node %6d                 ", get_node_number(n));
    fprintf(stderr, "  magic    %4x  |\n", n->get_flags() & Node::MASK);
    fprintf(stderr, "|- - - - - - - - - - - - - - - - - - - - - - - |\n");
    fprintf(stderr, "| flags   %1d%1d%1d%1d ", n->is_few(), n->is_full(), n->is_root(), n->is_leaf());
    fprintf(stderr, "| keys = %5d ", n->num_entries());
    fprintf(stderr, "| node = %6d  |\n", get_node_number( n->get_first_node()));
    for( int x = 1; x <= n->num_entries(); x++)
    {
        fprintf(stderr, "| entry %6d ", x);
        fprintf(stderr, "| key = %6d ", n->get_key( x ).get_value() & 0xFFFF);
        fprintf(stderr, "| node = %6d  |\n", get_node_number( n->get_node( x )));
    }
    fprintf(stderr, "------------------------------------------------\n\n");
}

//
// B+tree class variable printer
//
void Tree::show_btree( ) const
{
    fprintf(stderr, "-  --  --  --  --  --  -\n");
    fprintf(stderr, "|  B+tree  %10p  |\n", (void *) this);
    fprintf(stderr, "-  --  --  --  --  --  -\n");
    fprintf(stderr, "|  root        %6d  |\n", get_node_number( m_root ));
    fprintf(stderr, "|  leaf        %6d  |\n", get_node_number( get_leaf() ));
    fprintf(stderr, "|  fanout         %3d  |\n", m_fanout + 1);
    fprintf(stderr, "|  minfanout      %3d  |\n", get_min_fanout( m_root ) + 1);
    fprintf(stderr, "|  height         %3d  |\n", get_tree_height() );
//    fprintf(stderr, "|  freenode    %6d  |\n", get_node_number( get_first_free_node() ));
    fprintf(stderr, "|  theKey      %6d  |\n", get_fun_key().get_value() );
    fprintf(stderr, "|  theData     %6s  |\n", get_fun_data( ));
    fprintf(stderr, "-  --  --  --  --  --  -\n");

    Node* n = m_root;
    while( n != nullptr )
    {
        show_node( n );
        n = n->get_next_node();
    }
}


//
// B+tree data printer
//
void Tree::list_btree_values( Node* n ) const
{
    int slot, prev = -1;
    int num = 0;

    for( slot = 1; ( n != nullptr ) && n->num_entries(); num++ )
    {
        if( n->get_key( slot ).get_value() <= prev)
        {
            fprintf(stderr, "BOMB");
        }
        prev = n->get_key( slot ).get_value();


        fprintf( stderr, "%8d%c", prev, ( num & 7 ? ' ' : '\n' ) );
        if( ++slot > n->num_entries() )
        {
            n = n->get_next_node();
            slot = 1;
        }
    }
    fprintf( stderr, "\n\n" );
}

//
// entire B+tree data printer
//
void Tree::list_all_btree_values( ) const
{
    list_btree_values( get_leaf( ) );
}

#endif

