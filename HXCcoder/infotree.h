/*****************************************************
 * File..: infotree.h
 * Descr.: Header tree based determination of tolerances
 * Author: Tilo  Strutz
 * Data..: 26.11.2015
 ******************************************************/

#ifndef _INFOTREE_H_
#define _INFOTREE_H_

#define invLOG2	1.442695041  /* 1 / ln(2)  */

extern unsigned long lfdNr; /*already in getHXC2tolerances.c	*/
extern double penalty;
extern double information_content;

void merge_tree( C_TREE_NODE_2 *t_node,	AutoConfig *aC, 
								unsigned int numOfElements, 
								unsigned int cur_level);
void free_hist( HIST_LIST_ELEM2 *hist_elem);
void free_tree( C_TREE_NODE_2 *t_root);
void free_tree_recursive( C_TREE_NODE_2 *t_node);
void get_tree_information( C_TREE_NODE_2 *t_node);
C_TREE_NODE_2 *create_next_item_2( C_TREE_NODE_2 *t_node, 
												long  Y[4], unsigned int numOfElements);
C_TREE_NODE_2 *next_level_node_2( C_TREE_NODE_2 *t_node,	
												 long  Y[], unsigned int numOfElements);
void update_histogram( C_TREE_NODE_2 *t_node, 
											long  Y[], unsigned int numOfElements);

#endif

