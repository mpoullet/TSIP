#include "stdafx.h"
#include "plist.h"

/*------------------------------------------------------------------
 * void pnode_swap(pnode*, pnode*)
 *
 * A function which swaps the order of two consecutive nodes in a
 * double-linked list. It is asserted that the nodes are consecutive,
 * and thus the parameter order matters.
 *-----------------------------------------------------------------*/
void pnode_swap( pnode* node1, pnode* node2)
{
	if (node1 == NULL || node2 == NULL)
	{
		return;
	}
	assert(node1->next == node2 && node2->prev == node1);
	
	node1->next = node2->next;
	node2->prev = node1->prev;
	if (node1->prev)
	{
		node1->prev->next = node2;
	}
	
	if (node2->next)
	{
		node2->next->prev = node1;
	}
	node1->prev = node2;
	node2->next = node1;
}

/*------------------------------------------------------------------
 * pnode* pnode_new(int index, pnode* prev)
 *
 * Given an index (a node identifier), and a (final) node from a
 * double linked list, creates and returns a new list node, also
 * linking it to the supplied previous node.
 * Note: The previous node is not altered, i.e. the forward link
 * to the new node is not done, and must be done manually !
 *-----------------------------------------------------------------*/
pnode* pnode_new( int index, pnode* prev)
{
	//pnode* node = (pnode*) malloc(sizeof(pnode));
	pnode* node;
	ALLOC(node, 1, pnode);
	node->index = index;
	node->occs = 1;
	node->prev = prev;
	node->next = NULL;
	return node;
}


/*------------------------------------------------------------------
 * void plist_add_occ(plist*, index)
 *
 * The core function of the adjacency-mapping algorithm for lists.
 * The function takes the given index, and increments the occurrence
 * count of the node having the respective index. If no such node
 * exists, a new node is created at the end of the list.
 *
 * This process also keeps the list sorted descendingly, as for each
 * occurrence incrementation, appropriate swappings are done do keep
 * the list sorted.
 *-----------------------------------------------------------------*/
void plist_add_occ( plist* head, uint index)
{
 //Empty list case.
	if (!head->first)
	{
		head->first = pnode_new( index);
		head->length++;
		return;
	}
	pnode* first = head->first;
	pnode* node = head->first;
	pnode* prev = node;
	//Special case: the first node is the seeked one.
	if (node->index == index)
	{
		node->occs++;
		return;
	}
	node = node->next;
	while (node)
	{
		if (node->index == index)
		{
			node->occs++;
			//Loop repeated swaps to keep the list sorted.
			while (node->prev && node->occs > node->prev->occs)
			{
				pnode_swap( node->prev, node);
				if (node->next == first)
				{
					head->first = node;
				}
			}
			return;
		}
		node = node->next;
		prev = prev->next;
	}
	//No node has been found, so a new one is created and appended.
	prev->next = pnode_new( index, prev);
	head->length++;
}

/*------------------------------------------------------------------
 * int plist_get_priority(plist, int index)
 *
 * Given a list and an index, returns the priority of the respective
 * node. The priority has no special meaning, a node with priority
 * "n" is just the n-th 0-based node in the list.
 *-----------------------------------------------------------------*/
int plist_get_priority( plist list, uint index)
{
	pnode* node = list.first;
	int i = 0;
	while (node != NULL)
	{
		if (node->index == index)
		{
			return i;
		}
		node = node->next;
		i++;
	}
	return -1;
}

/*------------------------------------------------------------------
 * pnode* plist_get_node(plist, int priority)
 *
 * Returns the node in a list which has the given priority. Priority
 * does not have a special meaning, it is just the n-th 0-based node
 * in the list. Returns NULL if no node is found.
 *-----------------------------------------------------------------*/
pnode* plist_get_node( plist list, int priority)
{
	pnode* node = list.first;
	int i = 0;
	while (node != NULL)
	{
		if(i == priority)
		{
			return node;
		}
		node = node->next;
		i++;
	}
	return NULL;
}

/*------------------------------------------------------------------
 * void _pnode_free(pnode* node)
 *
 * Recursive function which frees all nodes from the specified node
 * to the end of the list.
 * If a second parameter is given, it will not free the current node
 * unless it is below the treshold.
 *
 * Note: This function is a helper function for plist_free(plist).
 * It should not be called manually to free any node.
 *
 * Returns 1 in case of deletion, 0 if the next node has not been
 * deleted.
 *-----------------------------------------------------------------*/
int _pnode_free(pnode* node, int occs = 0)
{
	if(!node)
	{
		return 0;
	}
	if (node->next)
	{
		_pnode_free(node->next);
	}
	if (occs > 0 && node->occs < occs) //If below treshold
	{
		free(node);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*------------------------------------------------------------------
 * void plist_free(plist)
 *
 * Frees all the nodes of a list iteratively, appropriately adjusting
 * the size of the list.
 * If a second parameter is given, only the nodes with a number of
 * occurrences below the specified value are erased.
 * If "occs" is set to 1, nothing will be deleted. (All existing
 * nodes in a list have occs >= 1).
 *-----------------------------------------------------------------*/
void plist_free(plist* list, int occs)
{
	pnode* node = list->first;
	pnode* next = list->first;
	if(!(occs > 0))
	{
		occs = -1;
	}
	while (node && node->occs >= occs)
	{
		//Traverse list until a node eligible for deletion is encountered.
		node = node->next;
	}
	if (!node) //End of list reached no eligible node found.
	{
		return;
	}
	
	//Set last node to point to null, since the current node will be deleted.
	if(node != list->first)
	{
		node->prev->next = NULL;
	}
	else
	{
		list->first = NULL;
	}
	while (node) //Dealocate memory iteratively, until end of list.
	{
		next = node->next;
		free(node);
		list->length--;
		node = next;
	}
}