#ifndef __PINE_H__
#define __PINE_H__

#include "config.h"
#include "typedefs.h"

#include "list.h"
#include "btree.h"

#if defined(__cplusplus)
extern "C"
{
#endif

//pine define.
typedef struct CPine_t
{
	
	CBTreeNode BTNode;
}CPine;

#define EXTENDS_PINE \
	CPine PineSuper;

#define CHILD_ADDR_OF_PINE(pine) \
	((int8u_t *)( ((int8u_t *)pine) + sizeof(CPine) ))

//pine method list.
typedef struct CPineMethod_t
{
	void (*init)( CPine *pPine );

	void (*release)( CPine *pPine );

	CListNode LNode;//ring list.
}CPineMethod;

//init pine system.
int32_t init_pine_system( void );

//release pine system.
void release_pine_system( void );

//create pine memory.
int8u_t *create_pine( int32_t iSizeOfPine );


//add pine method fron base to child.
int32_t add_pine_method( CPineMethod **ppHeadMethod, CPineMethod *pNewMethod );

//init pine.
int32_t pine_init( CPine *pPine, CPineMethod *pMethod );

//copy pine
CPine *pine_den( CPine *pPineSrc );

//release.
int32_t pine_release( CPine *pPine );

#if defined(__cplusplus)
}
#endif

#endif/* __PINE_H__ */


