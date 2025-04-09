
/******************************************************************************
 * vos.h -
 *
 * Copyright 1999-2007 tensint Co.,Ltd.
 *
 * DESCRIPTION: -
 *
 * modification history
 * --------------------
 * v1.0   2025/04/09, sarainco create this file
 *
 ******************************************************************************/

#ifndef _VOS_H_
#define _VOS_H_

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>
#include <stdarg.h>
#include <sys/signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdint.h>


#ifndef u_int8
    typedef unsigned char u_int8;
#endif

#ifndef u_int16
    typedef unsigned short u_int16;
#endif

#ifndef u_int32
    typedef unsigned int u_int32;
#endif

#ifndef u_long
    typedef unsigned long u_long;
#endif

#ifndef u_char
    typedef unsigned char u_char;
#endif

#ifndef u_short
    typedef unsigned short u_short;
#endif

#ifndef u_int
    typedef unsigned int u_int;
#endif

#ifndef llong
    typedef long long llong;
#endif

#ifndef u_llong
    typedef unsigned long long u_llong;
#endif

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#ifndef VOS_OK
#define VOS_OK 0
#endif

#ifndef VOS_ERR
#define VOS_ERR -1
#endif

#ifndef VOS_VALID
#define VOS_VALID 0
#endif

#ifndef VOS_INVALID /*unsigned int 的invalid*/
#define VOS_INVALID 0xffffffff
#endif

#ifndef VOS_CINVALID /*unsigned char 的invalid*/
#define VOS_CINVALID 0xff
#endif

#ifndef VOS_SINVALID /*unsigned short 的invalid*/
#define VOS_SINVALID 0xffff
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif


enum 
{
    MTYPE_PTHREAD = 0,
} MODULE_TYPE_E;


#ifdef __cplusplus
    typedef int (*FUNCPTR)(...);       /* pfunction returning int */
    typedef void (*VOIDFUNCPTR)(...);  /* pfunction returning void */
    typedef double (*DBLFUNCPTR)(...); /* pfunction returning double*/
    typedef float (*FLTFUNCPTR)(...);  /* pfunction returning float */
#else
    typedef int (*FUNCPTR)();       /* pfunction returning int */
    typedef void (*VOIDFUNCPTR)();  /* pfunction returning void */
    typedef double (*DBLFUNCPTR)(); /* pfunction returning double*/
    typedef float (*FLTFUNCPTR)();  /* pfunction returning float */
#endif /* __cplusplus */

/*****************************************************************************
 函 数 名  : vos_module_init
    功能描述  :初始化vos的相关信息
    输入参数  : 无
    输出参数  : 无
    返 回 值  :

    修改历史      :
*****************************************************************************/
//void vos_module_init();

/*****************************************************************************
 函 数 名  : vos_pthread_build
    功能描述  :创建所有任务
    输入参数  : 无
    输出参数  : 无
    返 回 值  :

    修改历史      :
*****************************************************************************/
//void vos_pthread_build();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _XXXX_H_ */
