
/******************************************************************************
 * vos_xmemory.c - 
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

#include <malloc.h>
#include "vos_xmemory.h"
#include "vos_lock.h"

VOS_MEMORY_ST g_stVosMemory;

/* 下一个内存控制块指针，循环内存控制块时使用，释放当前内存控制块时获取下一个控制块 */
ST_MEM_INFO *g_pstNextMemInfo = NULL;

/*如下的全部printf函数必须用log替换*/

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
void vos_xmemory_init()
{
    vos_pthread_lock_init(&g_stVosMemory.stMuxLock);
    lstInit(&g_stVosMemory.lstMem);
}

/*****************************************************************************
 Prototype    : vos_xzerror
Description  : 打印内存分配错误
Input        : const char *pcFname 内存分配C库函数名
            u_int32 ulModuleId 内存分配的模块ID
            u_int32 ulSize 分配内存的大小
Output       : None
Return Value : None
Calls        : 
Called By    : 

History        :

*****************************************************************************/
static void vos_xzerror(const char *pcFname, u_int32 ulModuleId, u_int32 ulSize)
{
fprintf(stderr, "%s : can't allocate memory for `%d' size %d\n", 
    pcFname, ulModuleId, ulSize);
}

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
void *vos_xmalloc(const char *pcFile, u_int32 ulLine,
                    u_int32 ulModuleId,u_int32 ulMemSize)
{
    void *pMemory =NULL;
    u_int *puiMagic = NULL;
    ST_MEM_INFO *pstInfo = NULL;
    /* 内存长度+控制信息长度+魔术字长度 */
    u_int uiLen = VOS_LEN_4BYTE_ALIGN(ulMemSize) + sizeof(ST_MEM_INFO) + sizeof(u_int32);

    pstInfo = (ST_MEM_INFO *)malloc(uiLen);
    if(pstInfo == NULL)
    {
        vos_xzerror("alloc", ulModuleId, sizeof(ST_MEM_INFO));
        return NULL;
    }
    memset(pstInfo, 0, uiLen);

    pMemory = pstInfo + 1;
    pstInfo->ulModuleId = ulModuleId;
    pstInfo->ulMemSize = uiLen - sizeof(u_int32);
    pstInfo->ulMemId = (u_int32)pstInfo;
    //pstInfo->ulSec = vos_get_system_seconds();
    /* 魔术字填充 */
    puiMagic = (u_int32 *)((u_long)pstInfo + uiLen - sizeof(u_int32));
    *puiMagic = VOS_MAGIC_INT;

#if(VOS_DEBUG)
    pstInfo->ulLine = ulLine;
    snprintf(pstInfo->ucFile,FILE_MAX_LEN,"%s",pcFile);
#endif
    vos_pthread_lock(&g_stVosMemory.stMuxLock);
    lstAddHead(&g_stVosMemory.lstMem, (NODE *)pstInfo);
    vos_pthread_unlock(&g_stVosMemory.stMuxLock);
    return pMemory;
}

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
void *vos_xfree(u_int32 ulModuleId,void *ptr)
{
    ST_MEM_INFO *pstMemInfo;

    pstMemInfo = vos_get_memory_by_id((u_int32)ptr);
    if(ptr&&pstMemInfo)
    {
        vos_pthread_lock(&g_stVosMemory.stMuxLock);
        if (g_pstNextMemInfo == pstMemInfo)
        {
            g_pstNextMemInfo = (ST_MEM_INFO *)lstNext((NODE *)pstMemInfo);
        }

        lstDelete(&g_stVosMemory.lstMem, (NODE *)pstMemInfo);
        //free(ptr);
        free(pstMemInfo);  /* pstMemInfo与ptr内存连续 */
        ptr = NULL;
        pstMemInfo = NULL;
        vos_pthread_unlock(&g_stVosMemory.stMuxLock);
    }
    else
    {
        if(NULL == ptr)
        {
            return NULL;
        } 
        else
        {
            printf("ulModuleId:%d 0x%x address is not first address 0f malloc memory.\r\n", ulModuleId,(u_int32)ptr);
        }
    }

    return NULL;
}

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
ST_MEM_INFO *vos_get_memory_by_id(u_int32 ulMemId)
{
    ST_MEM_INFO *pstMemInfo = NULL;

    vos_pthread_lock(&g_stVosMemory.stMuxLock);
    for(pstMemInfo=(ST_MEM_INFO *)lstFirst((LIST *)&g_stVosMemory.lstMem);
        NULL!=pstMemInfo;pstMemInfo=(ST_MEM_INFO *)lstNext((NODE *)(pstMemInfo)))
    {
        if((u_int32)(pstMemInfo + 1) == ulMemId)
        {
            break;
        }
    }
    vos_pthread_unlock(&g_stVosMemory.stMuxLock);
    return pstMemInfo;
}