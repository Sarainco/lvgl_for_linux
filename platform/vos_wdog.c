/******************************************************************************
 * vos_wdog.c - 
 * 
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0
 * 
 ******************************************************************************/

#include "vos_wdog.h"



u_llong vos_get_system_seconds()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_BOOTTIME, &ts) != 0) 
    {
        return -1;
    }
    return ts.tv_sec;
}

