/******************************************************************************
 * vos_xmemory.h - 
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
#ifndef _VOS_MEM_H_
#define _VOS_MEM_H_

#include "vos.h"
#include "vos_lst.h"


#define FILE_MAX_LEN 64

/* 结构体需要4字节对齐 */
typedef struct 
{
    NODE node;
    u_int32 ulModuleId;
    u_int32 ulMemId;
    u_int32 ulMemSize;
    u_long  ulSec;
#if(VOS_DEBUG)
    u_int32 ulLine;
    char   ucFile[FILE_MAX_LEN];
#endif
}ST_MEM_INFO;

typedef struct
{
    LIST lstMem;
    pthread_mutex_t stMuxLock;
}VOS_MEMORY_ST;

extern VOS_MEMORY_ST g_stVosMemory;

#define VOS_MAGIC_INT 0x12345678

#define VOS_LEN_4BYTE_ALIGN(n) ((((n)-1) & (~0x3)) + 0x4)

/*****************************************************************************
 Prototype    : vos_xmemory_init
 Description  : 初始化内存分配的首地址
 Input        : None
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :

*****************************************************************************/
void vos_xmemory_init();


/*****************************************************************************
 Prototype    : vos_xmalloc
 Description  : 实现上层软件设置调用SDK的统一接口
 Input        : const char *pcFile分配内存的函数名
              u_int32 ulLine 分配内存的行号
              u_int32 ulModuleId 分配内存的模块ID
              u_int32 ulMemSize 分配内存的大小
 Output       : None
 Return Value : 分配的内存首地址
 Calls        : 
 Called By    : 
 
  History        :

*****************************************************************************/
void *vos_xmalloc(const char *pcFile, u_int32 ulLine,u_int32 ulModuleId,u_int32 ulMemSize);


/*****************************************************************************
 Prototype    : vos_xcalloc
 Description  : 实现上层软件设置调用SDK的统一接口
 Input        : const char *pcFile分配内存的函数名
              u_int32 ulLine 分配内存的行号
              u_int32 ulModuleId 分配内存的模块ID
              u_int32 ulMemSize 分配内存的大小
 Output       : None
 Return Value : 分配的内存首地址
 Calls        : 
 Called By    : 
 
  History        :

*****************************************************************************/
void *vos_xcalloc(const char *pcFile, u_int32 ulLine,u_int32 ulModuleId,u_int32 ulMemSize);

/*****************************************************************************
 Prototype    : vos_xrealloc
 Description  : 内存重新分配
 Input        :void *ptr 需要重新分配内存的指针首地址
              const char *pcFile分配内存的函数名
              u_int32 ulLine 分配内存的行号
              u_int32 ulModuleId 分配内存的模块ID
              u_int32 ulMemSize 分配内存的大小
 Output       : None
 Return Value : None
 Calls        : 
 Called By    : 
 
  History        :


*****************************************************************************/
void * vos_xrealloc(void *ptr,const char *pcFile, u_int32 ulLine,u_int32 ulModuleId,u_int32 ulMemSize);

/*****************************************************************************
 Prototype    : vos_get_memory_by_id
 Description  : 根据分配内存的id查找分配的内存信息
 Input        : u_int32 ulMemId:分配的内存id   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :

*****************************************************************************/
ST_MEM_INFO *vos_get_memory_by_id(u_int32 ulMemId);


/*****************************************************************************
 Prototype    : vos_xfree
 Description  : 内存释放
 Input        : void *ptr:需要释放的内存
 Output       : None
 Return Value : None
 Calls        : 
 Called By    : 
 
  History        :


*****************************************************************************/
void *vos_xfree(u_int32 ulModuleId,void *ptr);


/*内存分配相关宏定义*/
#define XMALLOC(mtype, size)  \
    vos_xmalloc (__FILE__, __LINE__, (mtype), (size))
#define XFREE(mtype, ptr) \
    vos_xfree ((mtype), (ptr))

#define XSTRDUP(mtype, str) \
    vos_xstrdup ((str),__FILE__, __LINE__,(mtype))

#define XCALLOC(mtype, size) \
    vos_xcalloc (__FILE__, __LINE__, (mtype), (size))

#define XREALLOC(mtype, ptr, size) \
    vos_xrealloc((ptr),__FILE__, __LINE__, (mtype), (size))


#endif  /* _XXXX_H_ */

