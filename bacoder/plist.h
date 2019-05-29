/*******************************************************************
 *
 * File....: plist.h
 * Function: Contains definitions for a double-linked list data structures,
 *           tailored to the needs of the adjacency-mapping algorithm.
 * Author..: Toia-Cret Titus-Ruben
 * Date....: 08.05.2012
 ********************************************************************/
#include "tstypes.h"
#include "stats.h"
#include <assert.h>

typedef struct pnode
{
	uint index; //Original index within map. Acts as unique identifier.
	int occs;  //No. of occurrences of index with respect to the list's
	            //head index.
	pnode* next; //Next node (null if current node is terminal).
	pnode* prev; //Previous node (null if current node is initial).
} pnode;

typedef struct plist
{
	uint index; //Head Index of the list, with respect to which all the
	            //nodes' index occurrences are calculated.
	uint length; //Number of nodes within the list.
	pnode* first; //Pointer to first node.
} plist;

/* Swaps two nodes in a plist. Nodes must be consecutive, and thus the
 * parameter order matters. Stateful. */
void pnode_swap( pnode*, pnode*);
/* Allocates and returns a pointer to a new pnode. The integer parameter
 * is used to initialise it with an occurrence value. The supplied pnode*
 * is set as antecessor of the newly created pnode*.
 * Note: The antecessor node is not modified, i.e. the newly created node
 * must be manually set as the successor of the parameter pnode */
pnode* pnode_new( int, pnode* = NULL);
/* Increases the occurrence count of the node at the respective index by
 * one. If index is not found, a new pnode* is created and appended to the
 * list. */
void plist_add_occ( plist*, uint index);
/* Returns the 0-based position at which the node with the specified index
 * finds itself in the list, or -1 if it does not exist. */
int plist_get_priority( plist, uint index);
/* Returns the pnode* found at the n-th 0-based position, where n is the
 * supplied "priority" int value. If the list is not long enough, returns
 * NULL. */
pnode* plist_get_node( plist, int priority);
/* Iteratively frees all the nodes in the list. Accepts an optional argument
 * which delimits the number of nodes by an occurrence treshold;
 * i.e. the nodes which do not meet the treshold are not deleted, thus
 * resulting in an effective 'trimming' of the list.
 * Note: The plist* itself is not freed, and must be manually deallocated. */
void plist_free( plist* list, int occs = -1);