#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>

#define INPUT_DEVICE "/dev/input/event2"
#define BRIGHTNESS_FILE "/sys/class/backlight/backlight/brightness" // 背光控制路径
#define FB_BLANK_FILE "/sys/class/graphics/fb0/blank" // 帧缓冲控制路径


// 共享变量
volatile unsigned char key_pressed = 0;
unsigned char g_process_flg = 0;
pthread_mutex_t screen_mutex = PTHREAD_MUTEX_INITIALIZER;

void* key_monitor_thread(void* arg)
{
    struct input_event in_ev = {0};
    int fd = -1;
    int value = -1;
    time_t press_time = 0;


    /* 打开文件 */
    if (0 > (fd = open(INPUT_DEVICE, O_RDONLY))) {
        perror("open error");
        exit(-1);
    }

    while(1) 
    {
        /* 循环读取数据 */
        if (sizeof(struct input_event) !=
            read(fd, &in_ev, sizeof(struct input_event))) {
            perror("read error");
            exit(-1);
        }

        if (EV_KEY == in_ev.type && in_ev.code == 106) 
        { //按键事件
            switch (in_ev.value) {
            case 0:
                printf("code<%d>: 松开\n", in_ev.code);
                //system("echo 0 > /sys/class/graphics/fb0/blank");
                if(key_pressed)
                {
                    time_t release_time = time(NULL);
                    double press_duration = difftime(release_time, press_time);
                    if(press_duration < 2)
                    {
                        //system("echo 0 > /sys/class/graphics/fb0/blank");
                        g_process_flg = 1;
                    }
                    else if(press_duration < 5)
                    {
                        //system("echo 1 > /sys/class/graphics/fb0/blank");
                        g_process_flg = 0;
                    }
                    else
                    {
                        // 可以在这里添加长按反馈，比如闪烁LED或发出提示音
                        printf("长按中(已持续%.1f秒)...\n", press_duration);
                        g_process_flg = 0;
                    }
                    key_pressed = 0;
                }
                break;
            case 1:
                printf("code<%d>: 按下\n", in_ev.code);
                //system("echo 1 > /sys/class/graphics/fb0/blank");
                press_time = time(NULL);
                key_pressed = 1;
                break;
            }
        }

    }
}
