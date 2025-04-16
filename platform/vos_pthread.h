/******************************************************************************
 * vos_pthread.h - 
 * 
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0
 * 
 ******************************************************************************/

#ifndef _VOS_PTHREAD_H_
#define _VOS_PTHREAD_H_


#ifdef __cplusplus
extern "C"
{
#endif

#include "vos.h"
#include "vos_lst.h"
#include "vos_xmemory.h"
#include <pthread.h>

#ifndef THREAD_ID_T
typedef pthread_t THREAD_ID_T;
#endif

#define TASK_NAME_LEN   64

typedef struct vos_tcb
{
    NODE node;
    THREAD_ID_T taskId;
    pid_t uiThreadId;
    pthread_attr_t stThreadAttr;
    pthread_mutex_t stThreadMut;
    pthread_cond_t stThreadCond;
    u_int8 ucSchedPolicy;//调度策略
    u_int32 ulPriority;//优先级
    u_int32 ulSchedPriority;//调度优先级,优先级限制，在用户态0~99
    u_int8 ucTaskState;
    u_int8 ucName[TASK_NAME_LEN];
    VOIDFUNCPTR pFunc;
    void *pArg;
} VOS_TCB_T;

typedef struct 
{
    LIST lstTcb;
    pthread_mutex_t stMuxLock;
} LST_TCB_T;

/*****************************************************************************
函 数 名  : vos_pthread_init
功能描述  :任务初始化
输入参数  : 无
输出参数  : 无
返 回 值  : 

修改历史      :
    1. 2025/04/15  Created by sarainco
*****************************************************************************/
void vos_pthread_init();

/*****************************************************************************
函 数 名  : vos_pthread_create
功能描述  :创建任务
输入参数  : u_int8    *pucName 任务名字 
            u_int8    ucPriority   任务优先级 
            VOIDFUNCPTR  pFuncEntryPt 任务回调函数
            void     *pArg 任务回调函数参数
输出参数  : 无
返 回 值  : 

修改历史      :
    1. 2025/04/15  Created by sarainco
*****************************************************************************/
THREAD_ID_T vos_pthread_create
(
    u_int8    *pucName, 
    u_int8    ucPriority, 
    VOIDFUNCPTR  pFuncEntryPt, 
    void     *pArg
);

#ifdef __cplusplus
}
#endif


#endif