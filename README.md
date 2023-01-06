# TJUT_2021级大二上学年项目制作

### 选题 体温监测防疫系统

### 简介

###### 主要功能

    超声波测距每隔0.5s检测与目标物体的**距离**，当距离**小于10cm**时，通过蓝牙串口发送数据到上位机显示，0.96OLED可显示蓝牙是否连接和实时距离。

###### 使用外设

- 主芯片使用stm32f103c8t6

- 片外外设：0.96oled， JDY-31-SPP蓝牙模块，HC-SR04超声波测距

- 片内外设：串口USART1，通用定时器TIM2，PB9中断线EXIT9



----



### 任务分配

| Name    | 3927 | 3929 | 3930 |
|:------- |:----:|:----:|:----:|
| 仿真      | --   | ✔️   | --   |
| 软件设计    | ✔️   | --   | --   |
| 硬件电路PCB | ✔️   | --   | ✔️   |

### 任务目标

- [x] 超声波模块检测到有物体靠近，产生标志信号

- [x] 检测目标信息

- [x] 数据发送到JDY-31蓝牙串口模块，上位机显示

- [x] 0.96寸OLED显示实时信息，设备信息，蓝牙连接状态

- [x] **硬件已完成**
  
  ├─EEPROM存储异常体温数据
  
  └─*含电源*  的体温监测防疫**整机**

- [ ] 软件待Debug
  
  └─stm32f103c8t6硬件I2C卡在以下代码段的bug
  
  [./USER/IIC_Hardware.c](https://github.com/zhangbaoyin26/STM32)
  
   └─void OLED_Clear(void)
  
     └─void IIC1_Write_Byte(uint8_t dest_addr, uint8_t data)

```c
    // 检测EV6事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
```

- [x] PCB电路板的绘制



***



### 文件说明

| 根目录       | Funcation                   |
|:--------- |:---------------------------:|
| .vscode   | vscode编译和调试的配置              |
| build     | 编译产出的文件，包括hex, elf, bin, .o |
| Libraries | stm32f10x固件库函数              |
| USER      | 执行函数，main                   |

| USER/        | Funcation     |
|:------------ |:-------------:|
| delay        | TIM1定时器       |
| EXIT         | 中断向量配置寄存器     |
| HCSR04       | 超声波模块         |
| IIC_Hardware | 硬件I2C（待完善）    |
| oled         | 0.96寸OLED驱动函数 |
| usart        | 向蓝牙串口发送数据     |



---



### 硬件设计

| 硬件                              | Pin | Pin | Pin  | Pin  | Pin   | Pin | Pin  |
|:-------------------------------:|:---:|:---:|:----:|:----:|:-----:|:---:|:----:|
| stm32f103c8t6                   | PA5 | PA6 | PB8  | PB9  | PA7   | PA9 | PA10 |
| 0.96OLED(3v3)                   | SCL | SDA |      |      |       |     |      |
| HC-SR04超声波测距 (5v)               |     |     | Trig | Echo |       |     |      |
| JDY-31-SPP蓝牙模块：( 使用USART1, 5v ) |     |     |      |      | STATE | RXD | TXD  |

##### 超声波测距的原理分析

[传感器-HC-SR04超声波传感器 - 知乎 ](https://zhuanlan.zhihu.com/p/339855039)

>     当持续时间至少为10 µS 的脉冲施加到触发引脚Trig时，传感器以40 KHz发射八个脉冲的声音脉冲。这种8脉冲模式使设备的“超声特征”变得独一无二，从而使接收器能够将发射模式与环境超声噪声区分开。

  <img title="" src="https://github.com/zhangbaoyin26/TJUT-grade2021-Course-design/blob/main/image/GIFTemp_C4C.gif" alt="" data-align="inline" width="386">



>     给Trig一个至少10us的高电位信号后，HC-SR04内部会以40 KHz发射八个脉冲的声音脉冲，在空气中传播（Speed=340m/s）
> 
>     发送完脉冲信号后，HC-SR04会立马吧Echo拉高，等待回波信号产生
> 
>     当回波信号产生并被HC-SR04接受到时，会马上把Echo拉低

> 也就是说，Echo高电位的时间就是波在空气中传播的时间

- **distance = echo_time * Speed / 2**



---

 

### 软件设计

#### main.c函数时序

1. 初始化GPIO，Clock， NVIC， OLED， TIM
   
   ```c
      USART_Config();
      delay_init();         //延时函数初始化
      NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
      delay_ms(8000);
      OLED_Init();     //初始化OLED
      OLED_Clear();
      HCSR_GPIO_Config();      // 初始化超声波
      EXTI_Pxy_Config();          //配置PB9为上升沿中断，PB9->Echo
      TIMx_Configuration();    // 通用定时器 TIMx,x[2,3,4,5] 定时配置
      // 配置通用定时器 TIMx,x[2,3,4,5]的中断优先级
      TIMx_NVIC_Configuration();
      macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);
   ```

2. 检查是否连接蓝牙，如果没有连接，就一直等待蓝牙连接
   
   ```c
    while(GPIO_ReadInputDataBit(STATUS_PORT, STATUS_Pin) == 0){
      OLED_ShowString(16, 2, "Disconnect", 16);
    }
   ```

3. 发送起始信号，等待回波信号接收完成
   
   ```c
      echo_time = 0;
      while(echo_flag == 1)                        //等待回波信号接收完成
      {
          echo_flag = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);
      }
   ```

4. 计算distance（dis）
   
   ```c
      distance_time = echo_time;
      dis = Calculation();
   ```

5. 判断dis值， 是否开启USART1
   
   ```c
     // 判断有、没有物体靠近怎么做
      if (dis>0.02 && dis<=0.1) // 在最低阈值2cm以上，10cm内，有物体靠近
      {
        cnt++;
        printf("Somebody nearly: {time:%d, count:%d, distance:%.3fcm}", 000000, cnt, dis*100);
      }
      else // 没有物体靠近
      {
   
      }
   ```

#### 中断服务函数

##### 串口usart1中断服务函数（待完善）

    该函数用于上位机发送相应**指令**，主设备执行相应的语句，目前只有<u>上位机发送指令，主设备返回上位机指令</u>的功能

```c
void DEBUG_USART_IRQHandler(void)
{
  extern uint8_t global_arr[100];
  uint8_t ucTemp;
  u8 i = 0;
    if(USART_GetITStatus(DEBUG_USARTx,USART_IT_RXNE)!=RESET)
    {        
        ucTemp = USART_ReceiveData(DEBUG_USARTx);
    global_arr[i] = ucTemp;
    ++i;
    USART_SendData(DEBUG_USARTx, ucTemp);    
    }    
}
```

##### 超声波中断函数

    该函数用于计数Echo高电平的时间

    当回波信号产生时，Echo连接的GPIO引脚PB9被拉高，马上开启**通用计数器TIM2**，每隔**10us**，triger_time和echo_time的值就++

    读取到的triger_time和echo_time的值，就是声波在空气中传播了多少us

```c
void  macTIM_INT_FUN (void)
{
    if ( TIM_GetITStatus( macTIMx, TIM_IT_Update) != RESET ) 
    {    
        triger_time++;
        echo_time++;
        TIM_ClearITPendingBit( macTIMx , TIM_FLAG_Update);           
    }             
}
```

##### IO线中断，线中断连接PB9

    该函数用于清除echo_time的值，使其在每一次进入TIM2的中断函数时都为0

    由于每一次Echo(PB9)被拉高后，为了恢复echo_time的值，特设此中断服务函数，防止两次回波信号过于接近，**CPU**没有时间给echo_time赋值为0

    因此，NVIC设置的**EXIT9_5**线中断优先级要高于**TIM2**的中断优先级

```c
void macEXTI_INT_FUNCTION (void)
{
    if(EXTI_GetITStatus(macEXTI_LINE) != RESET) 
    {
        macTIM_APBxClock_FUN (macTIM_CLK, ENABLE);
        echo_time = 0;
        EXTI_ClearITPendingBit(macEXTI_LINE);     
    }  
}
```





---



### 项目展示
