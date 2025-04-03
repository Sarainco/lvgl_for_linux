// vi_vo_thread.h
#ifndef VI_VO_THREAD_H
#define VI_VO_THREAD_H

#include <pthread.h>
#include "rkmedia_api.h"

typedef struct {
    pthread_t tid;
    RK_S32 cam_id;
    RK_U32 video_width;
    RK_U32 video_height;
    RK_U32 disp_width;
    RK_U32 disp_height;
    volatile bool running;
    char *iq_file_dir;
} ViVoThreadContext;

int vi_vo_thread_start(ViVoThreadContext *ctx);
void vi_vo_thread_stop(ViVoThreadContext *ctx);

#endif