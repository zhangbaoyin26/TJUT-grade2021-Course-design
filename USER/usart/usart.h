// hardware >> 232-usart 
// 中断服务函数在 stm32f10x_it.c 文件中

#ifndef __USART_H
#define	__USART_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include <stdio.h>

/** 
  * 串口宏定义，不同的串口挂载的总线和IO不一样，移植时需要修改这几个宏
  * 1-修改总线时钟的宏，uart1挂载到apb2总线，其他uart挂载到apb1总线
  * 2-修改GPIO的宏
  */

#define __USART1        1
#define __USART2        0
#define __USART3        0
#define __USART4        0
#define __USART5        0
#define redef_printf    1
#define redef_scanf     1

#define STATUS_PORT     GPIOA
#define STATUS_Pin      GPIO_Pin_7
	
#if __USART1
    #define  DEBUG_USARTx                   USART1
    #define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
    #define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
    #define  DEBUG_USART_BAUDRATE           9600    

    // USART GPIO 引脚宏定义
    #define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
    #define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
        
    #define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
    #define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9
    #define  DEBUG_USART_RX_GPIO_PORT       GPIOA
    #define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10

    #define  DEBUG_USART_IRQ                USART1_IRQn
    #define  DEBUG_USART_IRQHandler         USART1_IRQHandler


#elif __USART2
    #define  DEBUG_USARTx                   USART2
    #define  DEBUG_USART_CLK                RCC_APB1Periph_USART2
    #define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
    #define  DEBUG_USART_BAUDRATE           115200

    // USART GPIO 引脚宏定义
    #define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
    #define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
    #define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
    #define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_2
    #define  DEBUG_USART_RX_GPIO_PORT       GPIOA
    #define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_3

    #define  DEBUG_USART_IRQ                USART2_IRQn
    #define  DEBUG_USART_IRQHandler         USART2_IRQHandler


#elif __USART3
    #define  DEBUG_USARTx                   USART3
    #define  DEBUG_USART_CLK                RCC_APB1Periph_USART3
    #define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
    #define  DEBUG_USART_BAUDRATE           115200

    // USART GPIO 引脚宏定义
    #define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOB)
    #define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
    #define  DEBUG_USART_TX_GPIO_PORT       GPIOB   
    #define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_10
    #define  DEBUG_USART_RX_GPIO_PORT       GPIOB
    #define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_11

    #define  DEBUG_USART_IRQ                USART3_IRQn
    #define  DEBUG_USART_IRQHandler         USART3_IRQHandler


#elif __USART4
    #define  DEBUG_USARTx                   UART4
    #define  DEBUG_USART_CLK                RCC_APB1Periph_UART4
    #define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
    #define  DEBUG_USART_BAUDRATE           115200

    // USART GPIO 引脚宏定义
    #define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOC)
    #define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
    #define  DEBUG_USART_TX_GPIO_PORT       GPIOC   
    #define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_10
    #define  DEBUG_USART_RX_GPIO_PORT       GPIOC
    #define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_11

    #define  DEBUG_USART_IRQ                UART4_IRQn
    #define  DEBUG_USART_IRQHandler         UART4_IRQHandler


#elif __USART5
    #define  DEBUG_USARTx                   UART5
    #define  DEBUG_USART_CLK                RCC_APB1Periph_UART5
    #define  DEBUG_USART_APBxClkCmd         RCC_APB1PeriphClockCmd
    #define  DEBUG_USART_BAUDRATE           115200

    // USART GPIO 引脚宏定义
    #define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD)
    #define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
    #define  DEBUG_USART_TX_GPIO_PORT       GPIOC   
    #define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_12
    #define  DEBUG_USART_RX_GPIO_PORT       GPIOD
    #define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_2

    #define  DEBUG_USART_IRQ                UART5_IRQn
    #define  DEBUG_USART_IRQHandler         UART5_IRQHandler
#endif


void USART_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

#endif /* __USART_H */
