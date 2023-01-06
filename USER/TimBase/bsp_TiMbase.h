#ifndef TIME_TEST_H
#define TIME_TEST_H


#include "stm32f10x.h"
#include "stm32f10x_tim.h"


/********************ͨ�ö�ʱ�� TIMx,x[2,3,4,5]��������************/

#define             macTIMx                                TIM2
#define             macTIM_APBxClock_FUN                   RCC_APB1PeriphClockCmd
#define             macTIM_CLK                             RCC_APB1Periph_TIM2
#define             macTIM_IRQ                             TIM2_IRQn
#define             macTIM_INT_FUN                         TIM2_IRQHandler

//#define             macTIMx                                TIM3
//#define             macTIM_APBxClock_FUN                   RCC_APB1PeriphClockCmd
//#define             macTIM_CLK                             RCC_APB1Periph_TIM3
//#define             macTIM_IRQ                             TIM3_IRQn
//#define             macTIM_INT_FUN                         TIM3_IRQHandler

/**************************��������********************************/
void               TIMx_NVIC_Configuration                   (void);
void               TIMx_Configuration                        (void);


#endif	/* TIME_TEST_H */


