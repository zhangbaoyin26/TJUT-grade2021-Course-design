/********************START***********************
 * ��Ŀ���������ѧ�����ѧ����Ŀ�����������߼��
 * ģ�飺stm32f103c8t6, 
 *        0.96oled, 
 *        JDY-31-SPP����ģ��,
 *        HC-SR04���������
 * 
 * ���ߣ�
 *       stm32f103c8t6 -> 0.96oled:
 *          GND         ->  GND
 *          VCC(3.3v)   ->  VCC
 *          PA5         ->  SCL
 *          PA6         ->  SDA
 *       stm32f103c8t6 -> HC-SR04���������:
 *          VCC(5v)     ->  VCC
 *          PB8         ->  Trig
 *          PB9         ->  Echo
 *          GND         ->  GND
 *       stm32f103c8t6 -> JDY-31-SPP����ģ�飺
 *          PA7         ->  STATE
 *          PA9         ->  RXD
 *          PA10        ->  TXD
 *          GND         ->  GND
 *          VCC(5V)     ->  VCC
 *          <float>     ->  EN
 * ���뻷����
 * 
 ********************END************************/


#include "stm32f10x.h"
#include "usart\usart.h"
#include "SysTick\bsp_SysTick.h"
#include "stm32f10x_it.h"
#include "oled\oled.h"
#include "TimBase\bsp_TiMbase.h"
#include "EXIT\bsp_exti.h"
#include "HCSR04\hcsr.h"
#include "delay\delay.h"

#define CLI() __set_PRIMASK(1)				/* �����ж� */  
#define SEI() __set_PRIMASK(0)				/* �����ж� */ 

volatile u32 triger_time = 0; // 10us triger��ʱ����,>10us
volatile u32 echo_time = 0;   // 10us echo��ʱ����, =����*2/����
u32 distance_time = 0;				// 10us ��¼echo�ߵ�ƽʱ��

uint8_t echo_flag = 0;				// echo_flag=1 ->���յ��ز��ź�

uint8_t global_arr[100];
void delay(uint32_t tim){
  uint32_t i = 0;
  for (; i < tim; ++i);
}



int main(){
  // ��ʼ��SysTick
  // �жϳ�ʼ��
  USART_Config();

  // oled��ʼ��
  delay_init();         //��ʱ������ʼ��
  NVIC_Configuration(); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  delay_ms(8000);
  GPIO_Conf();
  OLED_Init(); //��ʼ��OLED
  OLED_Clear();

  // ��ʼִ��main��������
  // ����Ƿ��������������û�����ӣ���һֱ�ȴ���������
  while(GPIO_ReadInputDataBit(STATUS_PORT, STATUS_Pin) == 0){
    OLED_ShowString(16, 2, "Disconnect", 16);
  }
  OLED_Clear();
  OLED_ShowString(16, 2, "Connecting...", 16);
  delay_ms(20000);
  OLED_Clear();
  OLED_ShowString(16, 0, "Connected", 16);

  // ��ʼ��������
  HCSR_GPIO_Config();
  EXTI_Pxy_Config(); 		//����PB9Ϊ�������жϣ�PB9->Echo
  /* ͨ�ö�ʱ�� TIMx,x[2,3,4,5] ��ʱ���� */	
    TIMx_Configuration();
	/* ����ͨ�ö�ʱ�� TIMx,x[2,3,4,5]���ж����ȼ� */
	TIMx_NVIC_Configuration();
	macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);

  // ѭ��
  float dis = 0;
  uint32_t cnt = 0;
  uint8_t  disarr[13];
  disarr[0]='d';
  disarr[1]='i';
  disarr[2]='s';
  disarr[3]=':';
  disarr[4]='0';
  disarr[5]='0';
  disarr[6]='0';
  disarr[7]='.';
  disarr[8]='0';
  disarr[9]='0';
  disarr[10]='c';
  disarr[11]='m';
  OLED_ShowString(16, 3, disarr, 16);
  while (1)
  {
    // ����Ƿ������忿��
    Triger();
		echo_time = 0;
		echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		while(echo_flag == 0)				//�ȴ��յ��ز��ź�
		{
			echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		}
		echo_time = 0;
		while(echo_flag == 1)						//�ȴ��ز��źŽ������
		{
			echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		}
		distance_time = echo_time;
		dis = Calculation();
    if (dis>0.02 && dis<=0.1)   // �������ֵ2cm���ϣ�10cm�ڣ������忿��
    {
      cnt++;
      printf("Somebody nearly: {time:%d, count:%d, distance:%.3fcm}", 000000, cnt, dis*100);
    }
    else    // û�����忿��
    {
      
    }
    // u8 INT_dis = (int)(dis*10000);
      disarr[4] = (uint8_t)(dis*1)%10+'0';
      disarr[5] = (uint8_t)(dis*10)%10+'0';
      disarr[6] = (uint8_t)(dis*100)%10+'0';
      disarr[7] = '.';
      disarr[8] = (uint8_t)(dis*1000)%10+'0';
      disarr[9] = (uint8_t)(dis*10000)%10+'0';
      OLED_ShowString(16, 3, disarr, 16);
    delay_ms(10000);
  }
}

// void SystemInit(){
//   //
// }
