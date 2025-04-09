

/******************************************************************************
 * vos_lock.h - 
 * 
 * Copyright 1999-2007 tensint Co.,Ltd.
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0
 * 
 ******************************************************************************/

#ifndef _VOS_LOCK_H_
#define _VOS_LOCK_H_

#ifdef	__cplusplus
extern "C" {
#endif	/* __cplusplus */

#include "vos.h"
#include "vos_lst.h"
#include <pthread.h>
#include <semaphore.h>
#include <pthread.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/ipc.h>

/*****************************************************************************
 Prototype    : vos_pthread_lock_init
Description  : 初始化任务锁
Input 	   : st_lock_id *pstMuxLock:任务锁Id指针
Output	   : None
Return Value : 
Calls 	   : 
Called By    : 

History		  :

*****************************************************************************/
void vos_pthread_lock_init(pthread_mutex_t *pstMuxLock);

/*****************************************************************************
 Prototype    : vos_pthread_lock
Description  : 任务锁获取
Input 	   : st_lock_id *pstMuxLock:任务锁Id
Output	   : None
Return Value : 
Calls 	   : 
Called By    : 

History		  :

*****************************************************************************/
int vos_pthread_lock(pthread_mutex_t *pstMuxLock);

/*****************************************************************************
 Prototype    : vos_pthread_timedlock
Description  : 任务锁获取
Input 	   : st_lock_id *pstMuxLock:任务锁Id
            u_int ulTick  超时tick数
Output	   : None
Return Value : VOS_OK VOS_ERR
Calls 	   : 
Called By    : 

History		  :

*****************************************************************************/
int vos_pthread_timedlock(pthread_mutex_t *pstMuxLock,u_int ulTick);


/*****************************************************************************
 Prototype    : vos_pthread_unlock
Description  : 任务锁释放
Input 	   : st_lock_id *pstMuxLock:任务锁Id
Output	   : None
Return Value : 
Calls 	   : 
Called By    : 

History		  :

*****************************************************************************/
void vos_pthread_unlock(pthread_mutex_t *pstMuxLock);

#ifdef	__cplusplus
}
#endif	/* __cplusplus */

#endif  /* _XXXX_H_ */

