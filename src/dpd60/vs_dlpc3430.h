/*
* vs_dlp2607.h
*
*  Created on: 2018-5-17
*      Author: Nick
*/

#ifndef VS_DLPC3430_H_
#define VS_DLPC3430_H_

#define DLP_IIC_REGS        SOC_I2C_1_REGS
#define DLP_IIC_ADDR        (0x36>>1)

#define READ_REG_SELECT     (0x15)

unsigned int dlpc3430_init(void);

unsigned char dlpc3430_reg_init(void);

void dlpc3430_set_brightness(int brightness);
void dlpc3430_set_current(unsigned short r, unsigned short g, unsigned short b);

void dlpc3430_on(void);
void dlpc3430_off(void);


extern int dpd_i2c_device_init();
extern int dpd_i2c_device_write(unsigned char *, unsigned char );
extern int dpd_i2c_device_read(unsigned char *, unsigned char , unsigned char *, unsigned char );
extern void dpd_i2c_device_close();

#endif /* VS_DLP_H_ */
