#ifndef TEMPERATUR_H
#define TEMPERATUR_H

#include "smbus.h"

int bmp085_i2c_Begin();
__s32 bmp085_i2c_Read_Int(int fd, __u8 address);
void bmp085_i2c_Write_Byte(int fd, __u8 address, __u8 value);
void bmp085_i2c_Read_Block(int fd, __u8 address, __u8 length, __u8 *values);
void bmp085_Calibration();
unsigned int bmp085_ReadUT();
unsigned int bmp085_ReadUP();
unsigned int bmp085_GetPressure(unsigned int up);
unsigned int bmp085_GetTemperature(unsigned int ut);
unsigned int bmp085_Altitude(float pressure, float temperature);



#endif
