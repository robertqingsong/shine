/********************************************************
 * file name: btree.h
 * writer: gzl
 * date time:2013-05-28 17:29
 * description: btree for framework kpf
 * *****************************************************/

#ifndef __BTREE_H__
#define __BTREE_H__

#include "config.h"
#include "typedefs.h"
#include "lock.h"

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct CBTree_t
{
	void *pBTreeTbl;
	
	int64u_t iCurrentWaterLevel;
	
	CMutex Locker;
}CBTree;


typedef struct CBTreeNode_t
{
	
}CBTreeNode;

#define CONTAINER_OF_BTNODE(addr, type) \
	CONTAINER_OF( addr, BTNode, type )

//compare type.
typedef int32_t (*btree_comp_t)( const void *pValA, const void *pValB, void *pParam );


//create btree.
CBTree *create_btree( btree_comp_t comp );
//destory btree.
int32_t destory_btree( CBTree *pBTree );

//init btree.
int32_t init_btree( CBTree *pBTree, btree_comp_t comp );
//release btree.
int32_t release_btree( CBTree *pBTree );

//add node to btree.
int32_t add_btree_node( CBTree *pBTree, CBTreeNode *pBTreeNode );
//remove btree node.
CBTreeNode *remove_btree_node( CBTree *pBTree, int64u_t iBTNodeId );
//search btree node.
CBTreeNode *search_btree_node( CBTree *pBTree, int64u_t iBTNodeId );

#if defined(__cplusplus)
}
#endif

#endif/* __BTREE_H__ */
