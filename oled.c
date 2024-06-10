#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "i2c.h"
#include "oled.h"
#include "gpio.h"

u8 OLED_GRAM[144][8];

/**
 * @brief 屏幕是否反色
 * @param i 0为正常，1为反色(白底黑字)
 * @retval void
*/
void OLED_ColorTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xA6,OLED_CMD);// 正常显示
		}
	if(i==1)
		{
			OLED_WR_Byte(0xA7,OLED_CMD);// 反色显示
		}
}

/**
 * @brief 屏幕是否反转
 * @param i 0为正常，1为翻转
 * @retval void
*/
void OLED_DisplayTurn(u8 i)
{
	if(i==0)
		{
			OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
			OLED_WR_Byte(0xA1,OLED_CMD);
		}
	if(i==1)
		{
			OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
			OLED_WR_Byte(0xA0,OLED_CMD);
		}
}


/**
 * @brief 向OLED发送一字节命令或数据
 * @param dat 发送的数据
 * @param mode 0写命令, 1写数据
 * @retval void
*/
void OLED_WR_Byte(u8 dat,u8 mode)
{

	if(mode){
		 HAL_I2C_Mem_Write(&hi2c1, 0x78,0x40,I2C_MEMADD_SIZE_8BIT,&dat,1,0x100);
	}
 	else{
		HAL_I2C_Mem_Write(&hi2c1, 0x78,0x00,I2C_MEMADD_SIZE_8BIT,&dat,1,0x100);
	}

}

/**
 * @brief 打开OLED屏幕
 * @retval void
*/
void OLED_DisPlay_On(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

/**
 * @brief 关闭OLED屏幕
 * @retval void
*/
void OLED_DisPlay_Off(void)
{
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}
 
/**
 * @brief 将显存更新到OLED屏幕
 * @retval void
*/
void OLED_Refresh(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
		OLED_WR_Byte(0xb0+i,OLED_CMD); //设置行起始地址
		OLED_WR_Byte(0x00,OLED_CMD);   //设置低列起始地址
		OLED_WR_Byte(0x10,OLED_CMD);   //设置高列起始地址
		for(n=0;n<128;n++)
		{
			OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
		}
  }
}

/**
 * @brief 清屏函数
*/
void OLED_Clear(void)
{
	u8 i,n;
	for(i=0;i<8;i++)
	{
	   for(n=0;n<128;n++)
			{
			 OLED_GRAM[n][i]=0;//清除所有数据
			}
  }
	OLED_Refresh();//更新显示
}
 
/**
 * @brief 作一个像素点
 * @param x 横坐标(从左往右)
 * @param y 纵坐标(从上往下)
 * @retval void
*/
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 i,m,n;
	i=y/8;
	m=y%8;
	n=1<<m;
	if(t){OLED_GRAM[x][i]|=n;}
	else
	{
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
		OLED_GRAM[x][i]|=n;
		OLED_GRAM[x][i]=~OLED_GRAM[x][i];
	}
}

/**
 * @brief 作一条直线
 * @param void
 * @param 
 * @retval void
*/
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量  
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_x;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		OLED_DrawPoint(uRow,uCol,mode); //画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

/**
 * @brief 作一个圆
 * @param x 圆心横坐标
 * @param y 圆心纵坐标
 * @param r 圆的半径
 * @retval void
*/
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
	int a, b,num;
    a = 0;
    b = r;
    while(2 * b * b >= r * r)      
    {
        OLED_DrawPoint(x + a, y - b,1);
        OLED_DrawPoint(x - a, y - b,1);
        OLED_DrawPoint(x - a, y + b,1);
        OLED_DrawPoint(x + a, y + b,1);
 
        OLED_DrawPoint(x + b, y + a,1);
        OLED_DrawPoint(x + b, y - a,1);
        OLED_DrawPoint(x - b, y - a,1);
        OLED_DrawPoint(x - b, y + a,1);
        
        a++;
        num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
        if(num > 0)
        {
            b--;
            a--;
        }
    }
}



/**
 * @brief 显示字符串
 * @param x 横坐标
 * @param y 纵坐标
 * @param chr 要显示的字符
 * @param size1 字号, {8,12,16,24}依次对应{08*06,12*06,16*08,24*12}
 * @param mode 0为反色显示, 1为正常显示
 * @retval void
*/
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 mode)
{
	u8 i,m,temp,size2,chr1;
	u8 x0=x,y0=y;
	if(size1==8)size2=6;
	else size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++)
	{
		if(size1==8){temp=asc2_0806[chr1][i];} //调用0806字体
		else if(size1==12){temp=asc2_1206[chr1][i];} //调用1206字体
		else if(size1==16){temp=asc2_1608[chr1][i];} //调用1608字体
		else if(size1==24){temp=asc2_2412[chr1][i];} //调用2412字体
		else return;
		for(m=0;m<8;m++)
		{
			if(temp&0x01)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp>>=1;
			y++;
		}
		x++;
		if((size1!=8)&&((x-x0)==size1/2))
		{x=x0;y0=y0+8;}
		y=y0;
  }
}


/**
 * @brief 显示字符串
 * @param x 横坐标
 * @param y 纵坐标
 * @param chr 要显示的字符串
 * @param size1 字号, {8,12,16,24}依次对应{08*06,12*06,16*08,24*12}
 * @param mode 0为反色显示, 1为正常显示
 * @retval void
*/
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 mode)
{
	while((*chr>=' ')&&(*chr<='~'))//判断是否非法字符!
	{
		OLED_ShowChar(x,y,*chr,size1,mode);
		if(size1==8)x+=6;
		else x+=size1/2;
		chr++;
  }
}

/**
 * @brief 幂函数
 * @param m 底数
 * @param n 指数
 * @retval m^n
*/
u32 OLED_Pow(u8 m,u8 n)
{
	u32 result=1;
	while(n--)
	{
	  result*=m;
	}
	return result;
}

/**
 * @brief 以二进制显示数字
 * @param x 横坐标
 * @param y 纵坐标
 * @param num 要显示的数字
 * @param len 显示位数(从右向左计位)
 * @param size 字号, {8,12,16,24}依次对应{08*06,12*06,16*08,24*12}
 * @param mode 反色: 1正常;0反色;
 * @retval void
*/
void OLED_ShowNum_bin(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode){
	uint8_t i, width;
	switch (size1)
	{
	case 8: width = 6; break;
	case 12: width = 6; break;
	case 16: width = 8; break;
	case 24: width = 12; break;
	default:return;
	}
	for (i = 0; i < len; i++)							
	{
		OLED_ShowChar(x+width*i, y , (num/OLED_Pow(2, len - i - 1))%2+'0', size1, mode);
	}
}

/**
 * @brief 以十进制显示数字
 * @param x 横坐标
 * @param y 纵坐标
 * @param num 要显示的数字
 * @param len 显示位数(左侧自动补零)
 * @param size 字号, {8,12,16,24}依次对应{08*06,12*06,16*08,24*12}
 * @param mode 反色: 1正常;0反色;
 * @retval void
*/
void OLED_ShowNum_dec(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
	uint8_t i, width;
	switch (size1)
	{
	case 8: width = 6; break;
	case 12: width = 6; break;
	case 16: width = 8; break;
	case 24: width = 12; break;
	default:return;
	}
	for (i = 0; i < len; i++)							
	{
		OLED_ShowChar(x+width*i, y , (num/OLED_Pow(10,len-i-1))%10+'0', size1, mode);
	}
}


/**
 * @brief 以十六进制显示数字
 * @param x 横坐标
 * @param y 纵坐标
 * @param num 要显示的数字
 * @param len 显示位数(从右向左计位)
 * @param size 字号, {8,12,16,24}依次对应{08*06,12*06,16*08,24*12}
 * @param mode 反色: 1正常;0反色;
 * @retval void
*/
void OLED_ShowNum_hex(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode){
	uint8_t i, width, decnum;
	switch (size1)
	{
	case 8: width = 6; break;
	case 12: width = 6; break;
	case 16: width = 8; break;
	case 24: width = 12; break;
	default:return;
	}
	for (i = 0; i < len; i++)							
	{
		decnum = num / OLED_Pow(16, len - i - 1) % 16;
		if (decnum < 10){OLED_ShowChar(x+width*i, y , decnum + '0', size1, mode);}
		else{OLED_ShowChar(x+width*i, y , decnum - 10 + 'A', size1, mode);}
	}
}

/**
 * @brief 显示有符号数字
 * @param x 横坐标
 * @param y 纵坐标
 * @param num 要显示的数字
 * @param len 显示位数(不计正负号)
 * @param size 字号, {8,12,16,24}依次对应{08*06,12*06,16*08,24*12}
 * @param mode 反色: 1正常;0反色;
 * @retval void
*/
void OLED_ShowNum_signeddec(u8 x,u8 y,int32_t num,u8 len,u8 size1,u8 mode){
	uint8_t width;
	switch (size1)
	{
	case 8: width = 6; break;
	case 12: width = 6; break;
	case 16: width = 8; break;
	case 24: width = 12; break;
	default:return;
	}
	if(num >=0){
		OLED_ShowChar(x,y, '+', size1, mode);
		OLED_ShowNum_dec(x+width, y, num, len, size1, mode);
	}
	else{
		OLED_ShowChar(x,y, '-', size1, mode);
		OLED_ShowNum_dec(x+width, y, -num, len, size1, mode);
	}
}



/**
 * @brief 显示单个汉字
 * @param x 横坐标
 * @param y 纵坐标
 * @param num 汉字对应的序号
 * @param size1 字号, {12,16,24}依次对应{12*12,16*16,24*24}
 * @param mode 1正常, 0反色
 * @retval void
*/
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
	u8 m,temp;
	u8 x0=x,y0=y;
	const u8* TEMP; 
	u16 i,size3=size1*(size1/8 + ((size1%8)?1:0));  //得到字体一个字符对应点阵集所占的字节数
	switch (size1)
	{
	case 12: TEMP = *(Chinese_12+num);break;
	case 16: TEMP = *(Chinese_16+num);break;
	case 24: TEMP = *(Chinese_24+num);break;
	case 32: TEMP = *(Chinese_32+num);break;
	default: return;	// 非法参数直接return
	}
	for(i=0;i<size3;i++)
	{
		temp = *(TEMP+i);
		for(m=0;m<8;m++)
		{
			if(temp&(0x01<<m)){OLED_DrawPoint(x,y,mode);}
			else{OLED_DrawPoint(x,y,!mode);}
			y++;
		}
		x++;
		y=y0;
		if((x-x0)==size1){x=x0;y0=y0+8;y=y0;}
	}
}

/**
 * @brief 滚动显示中文
 * @param num 汉字个数
 * @param space 显示间隔
 * @param mode 1正常, 0反色
 * @retval void
*/
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
	u8 i,n,t=0,m=0,r;
	while(1)
	{
		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //显示间隔
				 {
					for(i=1;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=1;i<144;i++)   //实现左移
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}
 
/**
 * @brief Show one picture
 * @param x (起始)横坐标
 * @param y (起始)纵坐标
 * @note 起始坐标位于字符/图片的左上角
 * @param sizex 图片横宽 
 * @param sizey 图片纵高 
 * @param BMP 图片对应数组名 
 * @param mode 1正常, 0反色显示 
 * @retval void
*/
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,const u8 BMP[],u8 mode)
{
	u16 j=0;
	u8 i,n,temp,m;
	u8 x0=x,y0=y;
	sizey=sizey/8+((sizey%8)?1:0);
	for(n=0;n<sizey;n++)
	{
		 for(i=0;i<sizex;i++)
		 {
				temp=BMP[j];
				j++;
				for(m=0;m<8;m++)
				{
					if(temp&0x01)OLED_DrawPoint(x,y,mode);
					else OLED_DrawPoint(x,y,!mode);
					temp>>=1;
					y++;
				}
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y0=y0+8;
				}
				y=y0;
     }
	 }
}
/**
 * @brief OLED初始化
 * @retval void
*/
void OLED_Init(void)
{
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD);
}

