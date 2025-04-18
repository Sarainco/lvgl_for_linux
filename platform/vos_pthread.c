/******************************************************************************
 * vos_pthread.c - 
 * 
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0
 * 
 ******************************************************************************/

#include "vos_pthread.h"
#include "vos_lock.h"
#include "vos_lst.h"
#include "vos_xmemory.h"
#include <sys/syscall.h>
#include <sys/prctl.h>

LST_TCB_T g_stLstTcb;

void vos_pthread_init()
{
    vos_pthread_lock_init(&g_stLstTcb.stMuxLock);
    lstInit(&g_stLstTcb.lstTcb);
}

/*****************************************************************************
 函 数 名  : gettid
 功能描述  :获取线程id
 输入参数  :
                void
 输出参数  : 线程id
 返 回 值  :

 修改历史      :
       1. 
*****************************************************************************/
pid_t gettid()
{
    return syscall(SYS_gettid);
}

void *vos_pthread_start(void *pArg)
{
    VOS_TCB_T *pstVosTcb = NULL;
    pstVosTcb = (VOS_TCB_T *)pArg;
    pstVosTcb->uiThreadId = gettid();
    prctl(PR_SET_NAME, pstVosTcb->ucName);
    pstVosTcb->pFunc(pstVosTcb->pArg);

    return NULL;
}

/*****************************************************************************
 函 数 名  : vos_linux_pthread_translate
 功能描述  :根据vxworks的任务优先级获取实际运行的任务优先级
 输入参数  : u_int32 ulVxPriority vxworks的任务优先级
                u_int32 ulSchedPolicy 实际调度优先级
 输出参数  : 无
 返 回 值  :

 修改历史      :
       1. 2025/04/16  Created by sarainco
*****************************************************************************/
u_int32 vos_linux_pthread_translate(u_int32 ulVxPriority, u_int32 ulSchedPolicy)
{
    u_int32 ulMaxPriority = 99;
    u_int32 ulPthreadPriority = 0;

    if (ulVxPriority > ulMaxPriority)
    {
        ulPthreadPriority = 99;
    }
    else if (ulVxPriority == 0)
    {
        ulPthreadPriority = 1;
    }
    else
    {
        ulPthreadPriority = ulVxPriority;
    }
    return (ulPthreadPriority);
}

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

*****************************************************************************/
THREAD_ID_T vos_pthread_create(u_int8 *pucName, u_int8 ucPriority, VOIDFUNCPTR pFuncEntryPt, void *pArg)
{
    int iRet = 0;
    VOS_TCB_T *pstVosTcb = NULL;
    struct sched_param stSchedParam;

    pstVosTcb  = (VOS_TCB_T *)XMALLOC(MTYPE_PTHREAD, sizeof(VOS_TCB_T));
    if(pstVosTcb == NULL)
    {
        return -1;
    }

    pstVosTcb->pArg = pArg;
    pstVosTcb->pFunc = pFuncEntryPt;
    pstVosTcb->ucSchedPolicy = SCHED_RR;
    strcpy(pstVosTcb->ucName, pucName);
    pthread_attr_init(&(pstVosTcb->stThreadAttr));
    pthread_attr_getschedparam(&(pstVosTcb->stThreadAttr), &stSchedParam);
    pstVosTcb->ulPriority = ucPriority;
    stSchedParam.sched_priority = vos_linux_pthread_translate(ucPriority, pstVosTcb->ucSchedPolicy);
    pstVosTcb->ulSchedPriority = stSchedParam.sched_priority;
    pthread_attr_setschedpolicy(&(pstVosTcb->stThreadAttr), SCHED_RR);
    pthread_attr_setschedparam(&(pstVosTcb->stThreadAttr), &stSchedParam);
    pthread_mutex_init(&pstVosTcb->stThreadMut, NULL);
    pthread_cond_init(&pstVosTcb->stThreadCond, NULL);

    iRet = pthread_create(&(pstVosTcb->taskId), &(pstVosTcb->stThreadAttr), vos_pthread_start, (void *)pstVosTcb);
    if(iRet)
    {
        return -1;
    }

    vos_pthread_lock(&(g_stLstTcb.stMuxLock));
    lstAdd(&(g_stLstTcb.lstTcb), (NODE *)pstVosTcb);
    vos_pthread_unlock(&(g_stLstTcb.stMuxLock));
    pthread_detach(pstVosTcb->taskId);
    pthread_attr_destroy(&(pstVosTcb->stThreadAttr));

    return pstVosTcb->taskId;
}


void vos_pthread_delete(THREAD_ID_T taskId)
{
    VOS_TCB_T *pstTcb = NULL;

    vos_pthread_lock(&(g_stLstTcb.stMuxLock));
    for(pstTcb = (VOS_TCB_T *)(lstFirst(&(g_stLstTcb.lstTcb))); pstTcb != NULL; pstTcb = (VOS_TCB_T *)(lstNext((NODE *)pstTcb)))
    {
        if(pstTcb->taskId == taskId)
        {
            vos_pthread_lock(&(pstTcb->stThreadMut));
            pthread_cancel(taskId);
            vos_pthread_unlock(&(pstTcb->stThreadMut));
            break;
        }
    }
    if(pstTcb != NULL)
    {
        lstDelete(&(g_stLstTcb.lstTcb), (NODE *)pstTcb);
        XFREE(MTYPE_PTHREAD, pstTcb);
    }
    vos_pthread_unlock(&(g_stLstTcb.stMuxLock));

}
