/* lstLib.h - doubly linked list library header */

/* Copyright 1984-1993 Wind River Systems, Inc. */

/*
modification history
--------------------
01m,02apr93,edm  ifdef'd out non-ASMLANGUAGE portions
01l,22sep92,rrr  added support for c++
01k,04jul92,jcf  cleaned up.
01j,26may92,rrr  the tree shuffle
01i,04oct91,rrr  passed through the ansification filter
-changed VOID to void
-changed copyright notice
01h,23oct90,shl  included "vxWorks.h" so include file order isn't crucial.
01g,05oct90,shl  added ANSI function prototypes.
        made #endif ANSI style.
        added copyright notice.
01f,10aug90,dnw  added declaration of lstInsert().
01e,07aug90,shl  added IMPORT type to function declarations.
01d,21may86,llk     added forward declaration of lstNStep.
01c,03jun84,dnw  changed list.{head,tail} to list.node.
added declarations of lst{First,Last,Next,Previous}.
01b,27jan84,ecs  added inclusion test.
01b,15mar83,dnw  changed name from lstlb to lstLib
*/

#ifndef _VOS_LST_H_
#define _VOS_LST_H_

#include "vos.h"

#define FAST    register

typedef struct node        /* Node of a linked list. */
{
    struct node *next;        /* Points at the next node in the list */
    struct node *previous;    /* Points at the previous node in the list */
} NODE;


/* HIDDEN */
typedef struct    /*list*/        /* Header for a linked list. */
{
    NODE node;            /* Header list node */
    int count;            /* Number of nodes in list */
} LIST;


#define LIST_SCAN(pList, pNode, TypeCast) for(pNode=(TypeCast)(lstFirst(pList));NULL!=pNode;pNode=(TypeCast)lstNext((NODE *)(pNode)))

#define LIST_SCAN_REVERSE(pList, pNode, TypeCast) for(pNode=(TypeCast)(lstLast(pList));NULL!=pNode;pNode=(TypeCast)lstPrevious((NODE *)(pNode)))

/* function declarations */
extern NODE *lstFirst (LIST *pList);
extern NODE *lstGet (LIST *pList);
extern NODE *lstLast (LIST *pList);
extern NODE *lstNStep (NODE *pNode, int nStep);
extern NODE *lstNext (NODE *pNode);
extern NODE *lstNth (LIST *pList, int nodenum);
extern NODE *lstPrevious (NODE *pNode);
extern int     lstCount (LIST *pList);
extern int     lstFind (LIST *pList, NODE *pNode);
extern void lstAdd (LIST *pList, NODE *pNode);
extern void lstConcat (LIST *pDstList, LIST *pAddList);
extern void lstDelete (LIST *pList, NODE *pNode);
extern void lstExtract (LIST *pSrcList, NODE *pStartNode, NODE *pEndNode, LIST *pDstList);
extern void lstFree (LIST *pList);
extern void lstInit (LIST *pList);
extern void lstInsert (LIST *pList, NODE *pPrev, NODE *pNode);

extern void lstAddHead (LIST *pList, NODE *pNode);

#endif


