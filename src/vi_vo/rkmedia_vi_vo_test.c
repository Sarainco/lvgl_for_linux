// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "rkmedia_api.h"
#include "sample_common.h"

#include "vi_vo_thread.h"

extern bool g_bool_flag;
static void* vi_vo_thread_func(void *arg) {
    ViVoThreadContext *ctx = (ViVoThreadContext *)arg;
    int ret = 0;

    printf("#CameraIdx: %d\n\n", ctx->cam_id);
    if (ctx->iq_file_dir) {
#ifdef RKAIQ
        printf("#Rkaiq XML DirPath: %s\n", ctx->iq_file_dir);
        rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
        int fps = 30;
        SAMPLE_COMM_ISP_Init(ctx->cam_id, hdr_mode, RK_FALSE, ctx->iq_file_dir);
        SAMPLE_COMM_ISP_Run(ctx->cam_id);
        SAMPLE_COMM_ISP_SetFrameRate(ctx->cam_id, fps);
#endif
    }

    RK_MPI_SYS_Init();

    // VI配置
    VI_CHN_ATTR_S vi_chn_attr = {
        .pcVideoNode = "rkispp_scale0",
        .u32BufCnt = 3,
        .u32Width = ctx->video_width,
        .u32Height = ctx->video_height,
        .enPixFmt = IMAGE_TYPE_NV12,
        .enWorkMode = VI_WORK_MODE_NORMAL
    };
    ret = RK_MPI_VI_SetChnAttr(ctx->cam_id, 0, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(ctx->cam_id, 0);
    if (ret) {
        printf("Create vi[0] failed! ret=%d\n", ret);
        return NULL;
    }

    // RGA配置
    RGA_ATTR_S stRgaAttr = {
        .bEnBufPool = RK_TRUE,
        .u16BufPoolCnt = 3,
        .u16Rotaion = 90,
        .stImgIn = {
            .u32X = 0,
            .u32Y = 0,
            .imgType = IMAGE_TYPE_NV12,
            .u32Width = ctx->video_width,
            .u32Height = ctx->video_height,
            .u32HorStride = ctx->video_width,
            .u32VirStride = ctx->video_height
        },
        .stImgOut = {
            .u32X = 0,
            .u32Y = 0,
            .imgType = IMAGE_TYPE_RGB888,
            .u32Width = ctx->disp_width,
            .u32Height = ctx->disp_height,
            .u32HorStride = ctx->disp_width,
            .u32VirStride = ctx->disp_height
        }
    };
    ret = RK_MPI_RGA_CreateChn(0, &stRgaAttr);
    if (ret) {
        printf("Create rga[0] falied! ret=%d\n", ret);
        goto EXIT_VI;
    }

    // VO配置
    VO_CHN_ATTR_S stVoAttr = {
        .pcDevNode = "/dev/dri/card0",
        .emPlaneType = VO_PLANE_PRIMARY,
        .enImgType = IMAGE_TYPE_RGB888,
        .u16Zpos = 0,
        .stImgRect = {0, 0, ctx->disp_width, ctx->disp_height},
        .stDispRect = {0, 0, ctx->disp_width, ctx->disp_height}
    };
    ret = RK_MPI_VO_CreateChn(0, &stVoAttr);
    if (ret) {
        printf("Create vo[0] failed! ret=%d\n", ret);
        goto EXIT_RGA;
    }

    // 绑定VI->RGA->VO
    MPP_CHN_S stSrcChn = {RK_ID_VI, 0};
    MPP_CHN_S stDestChn = {RK_ID_RGA, 0};
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        printf("Bind vi[0] to rga[0] failed! ret=%d\n", ret);
        goto EXIT_VO;
    }

    stSrcChn.enModId = RK_ID_RGA;
    stDestChn.enModId = RK_ID_VO;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) {
        printf("Bind rga[0] to vo[0] failed! ret=%d\n", ret);
        goto UNBIND_VI_RGA;
    }

    printf("vi_vo_thread running...\n");
    while(ctx->running) {
        usleep(500000); // 保持原有休眠逻辑
    }

    // 资源释放
UNBIND_VI_RGA:
    stSrcChn.enModId = RK_ID_VI;
    stDestChn.enModId = RK_ID_RGA;
    RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);

EXIT_VO:
    RK_MPI_VO_DestroyChn(0);

EXIT_RGA:
    RK_MPI_RGA_DestroyChn(0);

EXIT_VI:
    RK_MPI_VI_DisableChn(ctx->cam_id, 0);

    if (ctx->iq_file_dir) {
#ifdef RKAIQ
        SAMPLE_COMM_ISP_Stop(ctx->cam_id);
#endif
    }

    return NULL;
}

int vi_vo_thread_start(ViVoThreadContext *ctx) {
    if (!ctx) return -1;

    ctx->running = true;
    if (pthread_create(&ctx->tid, NULL, vi_vo_thread_func, ctx)) {
        perror("Create vi_vo thread failed");
        return -1;
    }
    return 0;
}

void vi_vo_thread_stop(ViVoThreadContext *ctx) {
    if (!ctx) return;

    ctx->running = false;
    pthread_join(ctx->tid, NULL);
}