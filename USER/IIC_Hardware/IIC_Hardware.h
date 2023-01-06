#ifndef _IIC_HARDWARE_H
#define _IIC_HARDWARE_H

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_i2c.h"
#define RCC_IIC_CLOCK   RCC_APB1Periph_I2C1
#define RCC_GPIO_CLOCK  RCC_APB2Periph_GPIOB
#define IIC_PORT        GPIOB
#define SCL             GPIO_Pin_6
#define SDA             GPIO_Pin_7

void IIC_GPIO_Config(void);
void IIC1_Config(void);
void IIC1_INIT(void);
void IIC1_Write_Byte(uint8_t dest_addr, uint8_t data);
void IIC1_Read_Byte(uint8_t dest_addr, uint8_t *data);
#endif // !_IIC_HARDWARE_H