/*
* vs_dlp2607.c
*
*  Created on: 2018-5-17
*      Author: Nick
*/

#include "vs_dlpc3430.h"
#include "stdio.h"

#define LED_CURRENT_MAX  (420) 	/** LED电流值最大1A，对应十进制为420 */

unsigned int dlpc3430_init(void)
{
    unsigned int ret;

    // IIC 初始化
    dpd_i2c_device_init();

    ret = dlpc3430_reg_init();

    dlpc3430_set_brightness(200);

    return ret;
}

unsigned char dlpc3430_reg_init(void)
{
    unsigned char data[10];

    data[0] = 0x16;
    data[1] = 0x01;
    dpd_i2c_device_write(data, 2);

    data[0] = 0x10;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0xd0;
    data[6] = 0x02;
    data[7] = 0xe0;
    data[8] = 0x01;
    dpd_i2c_device_write(data, 9);

    data[0] = 0x12;
    data[1] = 67;   // 显示720，总长854，偏移67
    data[2] = 0x00;
    data[3] = 0x00;
    data[4] = 0x00;
    data[5] = 0xd0;
    data[6] = 0x02;
    data[7] = 0xe0;
    data[8] = 0x01;
    dpd_i2c_device_write(data, 9);

    data[0] = 0x2e;
    data[1] = 0xd0;
    data[2] = 0x02;
    data[3] = 0xE0;
    data[4] = 0x01;
    dpd_i2c_device_write(data, 5);

    data[0] = 0x14;
    data[1] = 0x04;
    dpd_i2c_device_write(data, 2);

    data[0] = 0x07;
    data[1] = 0x40;
    dpd_i2c_device_write(data, 2);

    data[0] = 0x05;
    data[1] = 0x00;
    dpd_i2c_device_write(data, 2);

    // 再发一遍
    data[0] = 0x14;
    data[1] = 0x04;
    dpd_i2c_device_write(data, 2);

    data[0] = 0x07;
    data[1] = 0x40;
    dpd_i2c_device_write(data, 2);

    data[0] = 0x05;
    data[1] = 0x00;
    dpd_i2c_device_write(data, 2);

    data[0] = 0x16;
    data[1] = 0x00;
    dpd_i2c_device_write(data, 2);

    return 1;
}

void dlpc3430_set_brightness(int brightness)
{
    if(brightness > LED_CURRENT_MAX)
        brightness = LED_CURRENT_MAX;
    if(brightness < 0)
        brightness = 0;

    unsigned char i2c_array[7];

    i2c_array[0] = 0x54;
    i2c_array[1] = brightness;
    i2c_array[2] = brightness >> 8;
    i2c_array[3] = brightness;
    i2c_array[4] = brightness >> 8;
    i2c_array[5] = brightness;
    i2c_array[6] = brightness >> 8;

    dpd_i2c_device_write(i2c_array, 7);
}

void dlpc3430_set_current(unsigned short r, unsigned short g, unsigned short b)
{
    if (r < LED_CURRENT_MAX && g < LED_CURRENT_MAX & b < LED_CURRENT_MAX)
    {
        unsigned char i2c_array[7];

        i2c_array[0] = 0x54;
        i2c_array[1] = r;
        i2c_array[2] = r >> 8;
        i2c_array[3] = g;
        i2c_array[4] = g >> 8;
        i2c_array[5] = b;
        i2c_array[6] = b >> 8;

        dpd_i2c_device_write(i2c_array, 7);
    }
}

void dlpc3430_on(void)
{
    unsigned char data[2];
    data[0] = 0x52;
    data[1] = 0x07;
    dpd_i2c_device_write(data, 2);
}

void dlpc3430_off(void)
{
    unsigned char data[2];
    data[0] = 0x52;
    data[1] = 0x00;
    dpd_i2c_device_write(data, 2);
}

