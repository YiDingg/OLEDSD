#ifndef __OLED_H_
#define __OLED_H_
#include "stm32f10x.h"   // Device header
#include "stdlib.h"	
 
//-----------------OLED端口定义(只需调整下面这一部分)----------------// 
#define gpio GPIOA
#define rcc_APB2Periph_gpio RCC_APB2Periph_GPIOA
#define SCL_gpio_pin GPIO_Pin_6
#define SDA_gpio_pin GPIO_Pin_5
//-----------------OLED端口定义(只需调整上面这一部分)----------------// 


#define OLED_SCL_Clr() GPIO_ResetBits(gpio,SCL_gpio_pin)//SCL
#define OLED_SCL_Set() GPIO_SetBits(gpio,SCL_gpio_pin)
 
#define OLED_SDA_Clr() GPIO_ResetBits(gpio,SDA_gpio_pin)//SDA
#define OLED_SDA_Set() GPIO_SetBits(gpio,SDA_gpio_pin)
 
#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
 
void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void I2C_Start(void);
void I2C_Stop(void);
void I2C_WaitAck(void);
void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_Refresh(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode);
u32 OLED_Pow(u8 m,u8 n);
void OLED_ShowNum_bin(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowNum_dec(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowNum_hex(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowNum_signeddec(u8 x,u8 y,int32_t num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,const u8 BMP[],u8 mode);
void OLED_Init(void);
 
#endif
 
