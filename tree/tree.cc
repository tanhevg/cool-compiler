//
// See copyright.h for copyright notice and limitation of liability
// and disclaimer of warranty provisions.
//
#include "copyright.h"


///////////////////////////////////////////////////////////////////////////
//
// file: tree.cc
//
// This file defines the basic class of tree node
//
///////////////////////////////////////////////////////////////////////////

#include "tree.h"
extern int curr_lineno;


///////////////////////////////////////////////////////////////////////////
//
// tree_node::tree_node
//
// constructor of tree node
//
///////////////////////////////////////////////////////////////////////////
tree_node::tree_node()
{
    line_number = curr_lineno;
}

///////////////////////////////////////////////////////////////////////////
//
// tree_node::get_line_number
//
///////////////////////////////////////////////////////////////////////////
int tree_node::get_line_number()
{	
	return line_number;
}

//
// Set up common area from existing node
//
tree_node *tree_node::set(tree_node *t) {
   line_number = t->line_number;
   return this;
}
