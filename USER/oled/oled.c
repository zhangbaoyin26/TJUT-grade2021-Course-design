#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay\delay.h"




void IIC1_Wirte_Cmd(uint8_t cmd){
    uint8_t Err_Time = 0;
    I2C_SendData(I2C1, 0x00);
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
    I2C_SendData(I2C1, cmd);
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
}

void IIC1_Wirte_dat(uint8_t dat){
    uint8_t Err_Time = 0;
    I2C_SendData(I2C1, 0x40);
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
    I2C_SendData(I2C1, dat);
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
}



/**********************************************
// IIC Write Cmd
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    uint8_t Err_Time = 0;
    I2C_GenerateSTART(I2C1, ENABLE);
    // 检测EV5事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
    // 写地址
    I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
    // 检测EV6事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;

    IIC1_Wirte_Cmd(IIC_Command);

    // 检测EV8_2,发送完成标志置位,结束iic通信
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR && Err_Time++ < 0xFF  )
    I2C_GenerateSTOP(I2C1, ENABLE);
}


/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    uint8_t Err_Time = 0;
    I2C_GenerateSTART(I2C1, ENABLE);
    // 检测EV5事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
    // 写地址
    I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
    // 检测EV6事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;

    IIC1_Wirte_dat(IIC_Data);

    // 检测EV8_2,发送完成标志置位,结束iic通信
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR && Err_Time++ < 0xFF  )
    I2C_GenerateSTOP(I2C1, ENABLE);
}



void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
    if (cmd) {
        
        Write_IIC_Data(dat);

    } else {
        Write_IIC_Command(dat);
    }
}


/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
    unsigned char m, n;
    for (m = 0; m < 8; m++) {
        OLED_WR_Byte(0xb0 + m, 0); // page0-page1
        OLED_WR_Byte(0x00, 0);     // low column start address
        OLED_WR_Byte(0x10, 0);     // high column start address
        for (n = 0; n < 128; n++) {
            OLED_WR_Byte(fill_Data, 1);
        }
    }
}

/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
    unsigned int m;
    for (; Del_50ms > 0; Del_50ms--)
        for (m = 6245; m > 0; m--)
            ;
}

void Delay_1ms(unsigned int Del_1ms)
{
    unsigned char j;
    while (Del_1ms--) {
        for (j = 0; j < 123; j++)
            ;
    }
}



void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}


void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC����
    OLED_WR_Byte(0X14, OLED_CMD); // DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); // DISPLAY ON
}

void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); // SET DCDC
    OLED_WR_Byte(0X10, OLED_CMD); // DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); // DISPLAY OFF
}

void OLED_Clear(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(0xb0 + i, OLED_CMD); 
        OLED_WR_Byte(0x00, OLED_CMD);     
        OLED_WR_Byte(0x10, OLED_CMD);     
        for (n = 0; n < 128; n++) OLED_WR_Byte(0, OLED_DATA);
    } //������ʾ
}
void OLED_On(void)
{
    u8 i, n;
    for (i = 0; i < 8; i++) {
        OLED_WR_Byte(0xb0 + i, OLED_CMD); 
        OLED_WR_Byte(0x00, OLED_CMD);     
        OLED_WR_Byte(0x10, OLED_CMD);     
        for (n = 0; n < 128; n++) OLED_WR_Byte(1, OLED_DATA);
    } 
}
// x:0~127
// y:0~63
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 Char_Size)
{
    unsigned char c = 0, i = 0;
    c = chr - ' '; 
    if (x > Max_Column - 1) {
        x = 0;
        y = y + 2;
    }
    if (Char_Size == 16) {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
        OLED_Set_Pos(x, y + 1);
        for (i = 0; i < 8; i++)
            OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
    } else {
        OLED_Set_Pos(x, y);
        for (i = 0; i < 6; i++)
            OLED_WR_Byte(F6x8[c][i], OLED_DATA);
    }
}
// m^n
u32 oled_pow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--) result *= m;
    return result;
}

void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size2)
{
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++) {
        temp = (num / oled_pow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1)) {
            if (temp == 0) {
                OLED_ShowChar(x + (size2 / 2) * t, y, ' ', size2);
                continue;
            } else
                enshow = 1;
        }
        OLED_ShowChar(x + (size2 / 2) * t, y, temp + '0', size2);
    }
}

void OLED_ShowString(u8 x, u8 y, u8 *chr, u8 Char_Size)
{
    unsigned char j = 0;
    while (chr[j] != '\0') {
        OLED_ShowChar(x, y, chr[j], Char_Size);
        x += 8;
        if (x > 120) {
            x = 0;
            y += 2;
        }
        j++;
    }
}

void OLED_ShowCHinese(u8 x, u8 y, u8 no)
{
    u8 t, adder = 0;
    OLED_Set_Pos(x, y);
    for (t = 0; t < 16; t++) {
        OLED_WR_Byte(Hzk[2 * no][t], OLED_DATA);
        adder += 1;
    }
    OLED_Set_Pos(x, y + 1);
    for (t = 0; t < 16; t++) {
        OLED_WR_Byte(Hzk[2 * no + 1][t], OLED_DATA);
        adder += 1;
    }
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if (y1 % 8 == 0)
        y = y1 / 8;
    else
        y = y1 / 8 + 1;
    for (y = y0; y < y1; y++) {
        OLED_Set_Pos(x0, y);
        for (x = x0; x < x1; x++) {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

//SSD1306
void OLED_Init(void)    // 0x78 0x00 cmd | 0x78 0x40 dat
{
    uint8_t Err_Time = 0;
    IIC1_INIT();

    delay_ms(800);

    I2C_GenerateSTART(I2C1, ENABLE);
    // 检测EV5事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;
    // 写地址
    I2C_Send7bitAddress(I2C1, 0x78, I2C_Direction_Transmitter);
    // 检测EV6事件
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) == ERROR && Err_Time++ < 0xFF )
    Err_Time = 0;


     IIC1_Wirte_Cmd(0xAE); //--display off
     IIC1_Wirte_Cmd(0x00); //---set low column address
     IIC1_Wirte_Cmd(0x10); //---set high column address
     IIC1_Wirte_Cmd(0x40); //--set start line address
     IIC1_Wirte_Cmd(0xB0); //--set page address
     IIC1_Wirte_Cmd(0x81); // contract control
     IIC1_Wirte_Cmd(0xFF); //--128
     IIC1_Wirte_Cmd(0xA1); // set segment remap
     IIC1_Wirte_Cmd(0xA6); //--normal / reverse
     IIC1_Wirte_Cmd(0xA8); //--set multiplex ratio(1 to 64)
     IIC1_Wirte_Cmd(0x3F); //--1/32 duty
     IIC1_Wirte_Cmd(0xC8); // Com scan direction
     IIC1_Wirte_Cmd(0xD3); //-set display offset
     IIC1_Wirte_Cmd(0x00); //

     IIC1_Wirte_Cmd(0xD5); // set osc division
     IIC1_Wirte_Cmd(0x80); //

     IIC1_Wirte_Cmd(0xD8); // set area color mode off
     IIC1_Wirte_Cmd(0x05); //

     IIC1_Wirte_Cmd(0xD9); // Set Pre-Charge Period
     IIC1_Wirte_Cmd(0xF1); //

     IIC1_Wirte_Cmd(0xDA); // set com pin configuartion
     IIC1_Wirte_Cmd(0x12); //

     IIC1_Wirte_Cmd(0xDB); // set Vcomh
     IIC1_Wirte_Cmd(0x30); //

     IIC1_Wirte_Cmd(0x8D); // set charge pump enable
     IIC1_Wirte_Cmd(0x14); //

     IIC1_Wirte_Cmd(0xAF); //--turn on oled panel


    // 检测EV8_2,发送完成标志置位,结束iic通信
    while( I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED) == ERROR && Err_Time++ < 0xFF  )
    I2C_GenerateSTOP(I2C1, ENABLE);
}
