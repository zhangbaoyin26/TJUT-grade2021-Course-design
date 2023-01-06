#include "IIC_Hardware.h"


void IIC_GPIO_Config(void){
    GPIO_InitTypeDef GPIO_IIC1;
    RCC_APB1PeriphClockCmd(RCC_IIC_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_GPIO_CLOCK, ENABLE);
    GPIO_IIC1.GPIO_Mode = GPIO_Mode_AF_OD;  // 要求高阻态，使用复用功能的开漏输出
    GPIO_IIC1.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_IIC1.GPIO_Pin = SCL;
    GPIO_Init(IIC_PORT, &GPIO_IIC1);
    GPIO_IIC1.GPIO_Pin = SDA;
    GPIO_Init(IIC_PORT, &GPIO_IIC1);
}


void IIC1_Config(void){
    I2C_InitTypeDef IIC1_Init;
    IIC1_Init.I2C_ClockSpeed = 100000;
    IIC1_Init.I2C_Mode = I2C_Mode_I2C;
    IIC1_Init.I2C_DutyCycle = I2C_DutyCycle_2;
    IIC1_Init.I2C_OwnAddress1 = 0x07;
    IIC1_Init.I2C_Ack = I2C_Ack_Enable;
    IIC1_Init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &IIC1_Init);
}


void IIC1_INIT(void){
    IIC_GPIO_Config();
    IIC1_Config();
    I2C_Cmd(I2C1, ENABLE);
}


void IIC1_Write_Byte(uint8_t dest_addr, uint8_t data){
    uint8_t Err_Time = 0;
    I2C_GenerateSTART(I2C1, ENABLE);
    // 检测EV5事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
    // 写地址
    I2C_Send7bitAddress(I2C1, dest_addr, I2C_Direction_Transmitter);
    // 检测EV6事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;

    {
        // 开始写数据
        I2C_SendData(I2C1, data);
        while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR && Err_Time++ < 0xFF )
        Err_Time = 0;
        // I2C_SendData(I2C1, data);
        // while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR );
        // I2C_SendData(I2C1, data);
        // while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR );
    }

    // 检测EV8_2,发送完成标志置位,结束iic通信
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR && Err_Time++ < 0xFF  )
    I2C_GenerateSTOP(I2C1, ENABLE);
}



void IIC1_Read_Byte(uint8_t dest_addr, uint8_t *data){
    uint8_t Err_Time = 0;
    I2C_GenerateSTART(I2C1, ENABLE);
    // 检测EV5事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR && Err_Time++ < 0xFF  )
    // 写地址
    I2C_Send7bitAddress(I2C1, dest_addr, I2C_Direction_Receiver);
    // 检测EV6事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) == ERROR && Err_Time++ < 0xFF  )
    Err_Time = 0;

    {
        // 开始接收数据
        *(data) = I2C_ReceiveData(I2C1);
        while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR && Err_Time++ < 0xFF )
        Err_Time = 0;
        // *(data) = I2C_ReceiveData(I2C1);
        // while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR );
        // *(data) = I2C_ReceiveData(I2C1);
        // while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR );
        // *(data) = I2C_ReceiveData(I2C1);
        // while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == ERROR );
        I2C_AcknowledgeConfig(I2C1, DISABLE);
    }

    // 结束iic通信
    I2C_GenerateSTOP(I2C1, ENABLE);

    // 重新使能ACK，方便下次传输数据
    I2C_AcknowledgeConfig(I2C1, ENABLE);
}
