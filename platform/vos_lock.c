#include "vos_lock.h"

/*****************************************************************************
 Prototype    : vos_pthread_lock_init
 Description  : 
 Input 	   : st_lock_id *pstMuxLock:Idָ
 Output	   : None
 Return Value :
 Calls 	   :
 Called By    :

  History		  :

*****************************************************************************/
void vos_pthread_lock_init(pthread_mutex_t *pstMuxLock)
{
    pthread_mutex_init((pthread_mutex_t *)pstMuxLock, NULL);
}

/*****************************************************************************
 Prototype    : vos_pthread_lock
 Description  : 
 Input 	   : st_lock_id *pstMuxLock:Id
 Output	   : None
 Return Value :
 Calls 	   :
 Called By    :

  History		  :

*****************************************************************************/
int vos_pthread_lock(pthread_mutex_t *pstMuxLock)
{
    return pthread_mutex_lock(((pthread_mutex_t *)pstMuxLock));
}

/*****************************************************************************
 Prototype    : vos_pthread_timedlock
 Description  :
 Input 	   : st_lock_id *pstMuxLock:Id
          u_int ulTick  ʱtick
 Output	   : None
 Return Value : VOS_OK VOS_ERR
 Calls 	   :
 Called By    :

  History		  :

*****************************************************************************/
#if 0
int vos_pthread_timedlock(pthread_mutex_t *pstMuxLock, u_int ulTick)
{
   struct timespec stTime;
   vos_get_system_tick();
   memset(&stTime, 0, sizeof(struct timespec));
   stTime.tv_sec = ulTick / 100;
   stTime.tv_nsec = ulTick % 100;
   return pthread_mutex_timedlock((pthread_mutex_t *)pstMuxLock, &stTime);
}
#endif

/*****************************************************************************
 Prototype    : vos_pthread_unlock
 Description  : ͷ
 Input 	   : st_lock_id *pstMuxLock:Id
 Output	   : None
 Return Value :
 Calls 	   :
 Called By    :

  History		  :

*****************************************************************************/
void vos_pthread_unlock(pthread_mutex_t *pstMuxLock)
{
    pthread_mutex_unlock((pthread_mutex_t *)pstMuxLock);
}

