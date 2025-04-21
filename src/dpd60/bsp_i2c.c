/******************************************************************************
 * bsp_i2c.c - 
 * 
 * 
 * DESCRIPTION: - 
 * 
 * modification history
 * --------------------
 * v1.0
 * 
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../../bsp/i2c/libsoc_i2c.h"

#define DPD_I2C_BUS  2     // 根据实际设备修改，例如 /dev/i2c-2
#define DPD_I2C_ADDR 0x36  // 目标 I2C 设备地址，根据你的设备改

i2c* dpd_i2c_dev = NULL;

// 初始化 I2C
int dpd_i2c_device_init()
{
    dpd_i2c_dev = libsoc_i2c_init(DPD_I2C_BUS, DPD_I2C_ADDR);
    if (!dpd_i2c_dev) 
    {
        fprintf(stderr, "Failed to init i2c bus\n");
        return -1;
    }

    return 0;
}

// 写数据接口（包括寄存器地址在内的完整数据）
int dpd_i2c_device_write(uint8_t *data, uint8_t data_size)
{
    if (!dpd_i2c_dev || !data || data_size == 0)
        return -1;

    return (libsoc_i2c_write(dpd_i2c_dev, data, data_size) == EXIT_SUCCESS) ? 0 : -1;
}

// 写后读接口：先发送参数，再读取数据
int dpd_i2c_device_read(uint8_t *param, uint8_t param_size, uint8_t *data, uint8_t data_size)
{
    if (!dpd_i2c_dev || !data || data_size == 0)
        return -1;

    if (param && param_size > 0) 
    {
        if (libsoc_i2c_write(dpd_i2c_dev, param, param_size) != EXIT_SUCCESS) 
        {
            return -1;
        }
    }

    return (libsoc_i2c_read(dpd_i2c_dev, data, data_size) == EXIT_SUCCESS) ? 0 : -1;
}

// 清理资源
void dpd_i2c_device_close()
{
    if (dpd_i2c_dev) 
    {
        libsoc_i2c_free(dpd_i2c_dev);
        dpd_i2c_dev = NULL;
    }
}
