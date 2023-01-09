/********************START***********************
 * 项目：天津理工大学大二上学期项目制作——防疫检测
 * 模块：stm32f103c8t6, 
 *        0.96oled, 
 *        JDY-31-SPP蓝牙模块,
 *        HC-SR04超声波测距
 * 
 * 接线：
 *       stm32f103c8t6 -> 0.96oled:
 *          GND         ->  GND
 *          VCC(3.3v)   ->  VCC
 *          PA5         ->  SCL
 *          PA6         ->  SDA
 *       stm32f103c8t6 -> HC-SR04超声波测距:
 *          VCC(5v)     ->  VCC
 *          PB8         ->  Trig
 *          PB9         ->  Echo
 *          GND         ->  GND
 *       stm32f103c8t6 -> JDY-31-SPP蓝牙模块：
 *          PA7         ->  STATE
 *          PA9         ->  RXD
 *          PA10        ->  TXD
 *          GND         ->  GND
 *          VCC(5V)     ->  VCC
 *          <float>     ->  EN
 * 编译环境：VSCode+EIDE
 * 编译器启动文件：course_project_building.code-workspace
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

#define CLI() __set_PRIMASK(1)				/* 关总中断 */  
#define SEI() __set_PRIMASK(0)				/* 开总中断 */ 

volatile u32 triger_time = 0; // 10us triger计时变量,>10us
volatile u32 echo_time = 0;   // 10us echo计时变量, =距离*2/声速
u32 distance_time = 0;				// 10us 记录echo高电平时间

uint8_t echo_flag = 0;				// echo_flag=1 ->接收到回波信号

uint8_t global_arr[100];
void delay(uint32_t tim){
  uint32_t i = 0;
  for (; i < tim; ++i);
}



int main(){
  // 初始化SysTick
  // 中断初始化
  USART_Config();

  // oled初始化
  delay_init();         //延时函数初始化
  NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
  delay_ms(8000);
  GPIO_Conf();
  OLED_Init(); //初始化OLED
  OLED_Clear();

  // 开始执行main函数主体
  // 检查是否连接蓝牙，如果没有连接，就一直等待蓝牙连接
  while(GPIO_ReadInputDataBit(STATUS_PORT, STATUS_Pin) == 0){
    OLED_ShowString(16, 2, "Disconnect", 16);
  }
  OLED_Clear();
  OLED_ShowString(16, 2, "Connecting...", 16);
  delay_ms(20000);
  OLED_Clear();
  OLED_ShowString(16, 0, "Connected", 16);

  // 初始化超声波
  HCSR_GPIO_Config();
  EXTI_Pxy_Config(); 		//配置PB9为上升沿中断，PB9->Echo
  /* 通用定时器 TIMx,x[2,3,4,5] 定时配置 */	
    TIMx_Configuration();
	/* 配置通用定时器 TIMx,x[2,3,4,5]的中断优先级 */
	TIMx_NVIC_Configuration();
	macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);

  // 循环
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
    // 检测是否有物体靠近
    Triger();
		echo_time = 0;
		echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		while(echo_flag == 0)				//等待收到回波信号
		{
			echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		}
		echo_time = 0;
		while(echo_flag == 1)						//等待回波信号接收完成
		{
			echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
		}
		distance_time = echo_time;
		dis = Calculation();
    if (dis>0.02 && dis<=0.1)   // 在最低阈值2cm以上，10cm内，有物体靠近
    {
      cnt++;
      printf("Somebody nearly: {time:%d, count:%d, distance:%.3fcm}", 000000, cnt, dis*100);
    }
    else    // 没有物体靠近
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
