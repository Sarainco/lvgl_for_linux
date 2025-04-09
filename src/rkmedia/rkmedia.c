/****************************************************************************
*
*    Copyright (c) 2017 - 2019 by Rockchip Corp.  All rights reserved.
*
*    The material in this file is confidential and contains trade secrets
*    of Rockchip Corporation. This is proprietary information owned by
*    Rockchip Corporation. No part of this work may be disclosed,
*    reproduced, copied, transmitted, or used in any way for any purpose,
*    without the express written permission of Rockchip Corporation.
*
*****************************************************************************/

#include "include/rkmedia/rkmedia.h"
#include "include/lv_port/lv_port_indev.h"


extern unsigned char g_process_flg;
extern bool g_bProcessFlag;

void *rkmedia_vi_rockx_thread(void *args)
{
    int disp_width = 720;
    int disp_height = 1280;
    RK_U32 video_width = 720;
    RK_U32 video_height = 1280;

    RK_CHAR *pDeviceName = "rkispp_scale0";
    RK_CHAR *pcDevNode = "/dev/dri/card0";
    char *iq_file_dir = "/etc/iqfiles";
    RK_S32 s32CamId = 0;
    RK_U32 u32BufCnt = 3;
    RK_U32 fps = 20;
    int ret;
    pthread_t rkmedia_vi_rockx_tidp;
    RK_BOOL bMultictx = RK_FALSE;


#if 1
    rockx_ret_t ret1;
    rockx_handle_t face_det_handle;
    rockx_handle_t face_5landmarks_handle;
    rockx_handle_t face_landmark_handle;
    rockx_module_t rockx_module;
#endif
  
    printf("\n###############################################\n");
    printf("VI CameraIdx: %d\npDeviceName: %s\nResolution: %dx%d\n\n",
            s32CamId,pDeviceName,video_width,video_height);
    printf("VO pcDevNode: %s\nResolution: %dx%d\n",
            pcDevNode,disp_height,disp_width);
    printf("###############################################\n\n");

    if (iq_file_dir) 
    {
#ifdef RKAIQ
        printf("#Rkaiq XML DirPath: %s\n", iq_file_dir);
        printf("#bMultictx: %d\n\n", bMultictx);
        rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
        SAMPLE_COMM_ISP_Init(s32CamId, hdr_mode, bMultictx, iq_file_dir);
        SAMPLE_COMM_ISP_Run(s32CamId);
        SAMPLE_COMM_ISP_SetFrameRate(s32CamId, fps);
#endif
    }
  
    RK_MPI_SYS_Init();
    VI_CHN_ATTR_S vi_chn_attr;
    vi_chn_attr.pcVideoNode = pDeviceName;
    vi_chn_attr.u32BufCnt = u32BufCnt;
    vi_chn_attr.u32Width = video_width;
    vi_chn_attr.u32Height = video_height;
    vi_chn_attr.enPixFmt = IMAGE_TYPE_NV12;
    vi_chn_attr.enBufType = VI_CHN_BUF_TYPE_MMAP;
    vi_chn_attr.enWorkMode = VI_WORK_MODE_NORMAL;
    ret = RK_MPI_VI_SetChnAttr(s32CamId, 1, &vi_chn_attr);
    ret |= RK_MPI_VI_EnableChn(s32CamId, 1);
    if (ret) 
    {
        printf("ERROR: create VI[0:1] error! ret=%d\n", ret);
        return -1;
    }

    RGA_ATTR_S stRgaAttr;
    memset(&stRgaAttr, 0, sizeof(stRgaAttr));
    stRgaAttr.bEnBufPool = RK_TRUE;
    stRgaAttr.u16BufPoolCnt = 3;
    stRgaAttr.u16Rotaion = 270;
    stRgaAttr.stImgIn.u32X = 0;
    stRgaAttr.stImgIn.u32Y = 0;
    stRgaAttr.stImgIn.imgType = IMAGE_TYPE_NV12;
    stRgaAttr.stImgIn.u32Width = video_width;
    stRgaAttr.stImgIn.u32Height = video_height;
    stRgaAttr.stImgIn.u32HorStride = video_width;
    stRgaAttr.stImgIn.u32VirStride = video_height;
    stRgaAttr.stImgOut.u32X = 0;
    stRgaAttr.stImgOut.u32Y = 0;
    stRgaAttr.stImgOut.imgType = IMAGE_TYPE_RGB888;
    stRgaAttr.stImgOut.u32Width = video_width;
    stRgaAttr.stImgOut.u32Height = video_height;
    stRgaAttr.stImgOut.u32HorStride = video_width;
    stRgaAttr.stImgOut.u32VirStride = video_height;
    ret = RK_MPI_RGA_CreateChn(1, &stRgaAttr);
    if (ret) 
    {
        printf("ERROR: create RGA[0:1] falied! ret=%d\n", ret);
        return -1;
    }

    VO_CHN_ATTR_S stVoAttr = {0};
    stVoAttr.pcDevNode = pcDevNode;
    stVoAttr.emPlaneType = VO_PLANE_OVERLAY;
    stVoAttr.enImgType = IMAGE_TYPE_RGB888;
    stVoAttr.u16Zpos = 0;
    stVoAttr.stImgRect.s32X = 0;
    stVoAttr.stImgRect.s32Y = 0;
    stVoAttr.stImgRect.u32Width = disp_width;
    stVoAttr.stImgRect.u32Height = disp_height;
    stVoAttr.stDispRect.s32X = 0;
    stVoAttr.stDispRect.s32Y = disp_height / 12;//顶部留白
    stVoAttr.stDispRect.u32Width = disp_width;
    stVoAttr.stDispRect.u32Height = disp_height * 5 / 6;
    ret = RK_MPI_VO_CreateChn(0, &stVoAttr);
    if (ret) 
    {
        printf("ERROR: create VO[0:0] failed! ret=%d\n", ret);
        return -1;
    }

    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    printf("Bind VI[0:1] to RGA[0:1]....\n");
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = s32CamId;
    stSrcChn.s32ChnId = 1;
    stDestChn.enModId = RK_ID_RGA;
    stDestChn.s32DevId = s32CamId;
    stDestChn.s32ChnId = 1;
    ret = RK_MPI_SYS_Bind(&stSrcChn, &stDestChn);
    if (ret) 
    {
        printf("ERROR: Bind VI[0:1] to RGA[0:1] failed! ret=%d\n", ret);
        return -1;
    }


#if 1
    int landmark_count =68;
    //int landmark_count =5;
    if (landmark_count == 5) 
    {
        rockx_module = ROCKX_MODULE_FACE_LANDMARK_5;
    } 
    else if (landmark_count == 68) 
    {
        rockx_module = ROCKX_MODULE_FACE_LANDMARK_68;
    } else 
    {
        printf("error landmark count must 5 or 68\n");
    }

    char *rockx_data = "/demo/src/rockx_data";
    rockx_config_t *config = rockx_create_config();
    rockx_add_config(config, ROCKX_CONFIG_DATA_PATH, rockx_data);

    ret1 = rockx_create(&face_det_handle, ROCKX_MODULE_FACE_DETECTION, config,sizeof(rockx_config_t));
    if (ret1 != ROCKX_RET_SUCCESS) 
    {
        printf("init rockx module ROCKX_MODULE_FACE_DETECTION error %d\n", ret1);
        return NULL;
    }

    ret1 = rockx_create(&face_5landmarks_handle,ROCKX_MODULE_FACE_LANDMARK_5, config, sizeof(rockx_config_t));
    if (ret1 != ROCKX_RET_SUCCESS) 
    {
        printf("ERROR: init rockx module ROCKX_MODULE_FACE_LANDMARK_5 error %d\n",ret1);
    }

    if(landmark_count == 68)
    {
        ret1 = rockx_create(&face_landmark_handle,ROCKX_MODULE_FACE_LANDMARK_68, config, sizeof(rockx_config_t));
        if (ret1 != ROCKX_RET_SUCCESS) 
        {
          printf("init rockx module ROCKX_MODULE_FACE_LANDMARK_68 error %d\n",ret1);
        }
    }

    if(landmark_count == 5)
    {
        ret1 = rockx_create(&face_landmark_handle,ROCKX_MODULE_FACE_LANDMARK_5, config, sizeof(rockx_config_t));
        if (ret1 != ROCKX_RET_SUCCESS) 
        {
          printf("init rockx module ROCKX_MODULE_FACE_LANDMARK_5 error %d\n",ret1);
        }
    }
  #endif

    // pthread_create(&rkmedia_vi_rockx_tidp, NULL, rkmedia_vi_rockx_thread, NULL);
    // printf("%s initial finish\n", __func__);
    while (1) 
    {
        if(!g_bProcessFlag)
        {
            //printf("g_bProcessFlag : %d\n", g_bProcessFlag);
            continue;
        }

        MEDIA_BUFFER src_mb = NULL;
        src_mb = RK_MPI_SYS_GetMediaBuffer(RK_ID_RGA, 1, -1);
        if (!src_mb) 
        {
            printf("RK_MPI_SYS_GetMediaBuffer get null buffer!\n");
            break;
        }

        //todo process
        if(g_process_flg)
        {
            rockx_image_t input_image;
            //input_image.pixel_format = ROCKX_PIXEL_FORMAT_YUV420SP_NV12;
            struct rkMB_IMAGE_INFO ImageInfo={0};
            RK_MPI_MB_GetImageInfo(src_mb,&ImageInfo);
            input_image.width=ImageInfo.u32Width;   
            input_image.height=ImageInfo.u32Height;
            input_image.pixel_format = ROCKX_PIXEL_FORMAT_RGB888;
            input_image.size = RK_MPI_MB_GetSize(src_mb);
            input_image.data = (uint8_t *)RK_MPI_MB_GetPtr(src_mb);

            rockx_object_array_t face_array;
            memset(&face_array, 0, sizeof(face_array));
            ret =rockx_face_detect(face_det_handle, &input_image, &face_array, NULL);
            if (ret != ROCKX_RET_SUCCESS) 
            {
                printf("rockx_face_detect error %d\n", ret);
            }

            if (face_array.count > 0) 
            {
                for (int i = 0; i < face_array.count; i++) 
                {
                  int is_false_face;
                  ret = rockx_face_filter(face_5landmarks_handle, &input_image,&face_array.object[i].box, &is_false_face);
                  if (ret != ROCKX_RET_SUCCESS) 
                  {
                      printf("rockx_face_filter error %d\n", ret);
                  }
                  if (is_false_face)
                  {
                      continue;
                  }

                  rockx_face_landmark_t out_landmark;
                  rockx_face_angle_t out_angle;
                  memset(&out_landmark, 0, sizeof(rockx_face_landmark_t));
                  memset(&out_angle, 0, sizeof(rockx_face_angle_t));

                  ret = rockx_face_landmark(face_landmark_handle, &input_image, &face_array.object[i].box, &out_landmark);

                  if (landmark_count == 5) 
                  {
                      rockx_image_t out_img;
                      memset(&out_img, 0, sizeof(rockx_image_t));
                      ret = rockx_face_align(face_landmark_handle, &input_image, &face_array.object[i].box, &out_landmark, &out_img);
                  }

                  for(int j = 0; j < out_landmark.landmarks_count; j++) 
                  {
                                // 错误修复后的调用
                      rockx_point_t point = {out_landmark.landmarks[j].x, out_landmark.landmarks[j].y};
                      rockx_color_t color = {0, 255, 0};
                      rockx_image_draw_circle(&input_image, point, 3, color, -1);
                      //rockx_image_draw_circle(&input_image, {out_landmark.landmarks[j].x, out_landmark.landmarks[j].y}, 3, {0, 255, 0}, -1);
                  }
              
              }
          }
    }

    //todo
      RK_MPI_SYS_SendMediaBuffer(RK_ID_VO, 0, src_mb);
      
      RK_MPI_MB_ReleaseBuffer(src_mb);
      src_mb = NULL;
      //usleep(500000);
    }

  #if 1
    rockx_destroy(face_det_handle);
    rockx_destroy(face_5landmarks_handle);
    rockx_destroy(face_landmark_handle);
  #endif

    printf("%s exit!\n", __func__);
    printf("Unbind VI[0:1] to RGA[0:1]....\n");
    stSrcChn.enModId = RK_ID_VI;
    stSrcChn.s32DevId = s32CamId;
    stSrcChn.s32ChnId = 1;
    stDestChn.enModId = RK_ID_RGA;
    stSrcChn.s32DevId = s32CamId;
    stDestChn.s32ChnId = 1;
    ret = RK_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
    if (ret) 
    {
        printf("ERROR: unbind VI[0:1] to RGA[0:1] failed! ret=%d\n", ret);
        return -1;
    }

    printf("Destroy VI[0:1] channel\n");
    ret = RK_MPI_VI_DisableChn(s32CamId, 1);
    if (ret) 
    {
        printf("ERROR: destroy VI[0:1] error! ret=%d\n", ret);
        return -1;
    }

    printf("Destroy VO[0:0] channel\n");
    ret = RK_MPI_VO_DestroyChn(0);
    {
        printf("ERROR: destroy VO[0:0] error! ret=%d\n", ret);
        return -1;
    }

    if (iq_file_dir) 
    {
#if RKAIQ
        SAMPLE_COMM_ISP_Stop(s32CamId);
#endif
    }

  return;
}
