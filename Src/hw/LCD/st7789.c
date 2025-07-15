//
// Created by 34575 on 25-7-8.
//
#include <main.h>
#include <hw/LCD/st7789.h>
#include <hw/LCD/lcd_fonts.h>
static pFONT *ST7789_AsciiFonts;		// 英文字体，ASCII字符集
static pFONT *ST7789_CHFonts;		   // 中文字体（同时也包含英文字体）

uint16_t FrameBuffer[ST7789_WIDTH][ST7789_HEIGHT] __attribute__((__section__(".ram_d1")));  // 帧缓冲区

struct	//LCD相关参数结构体
{
	uint32_t Color;  				//	LCD当前画笔颜色
	uint32_t BackColor;			//	背景色
	uint8_t  ShowNum_Mode;		// 数字显示模式
	uint8_t  Direction;			//	显示方向
	uint16_t Width;            // 屏幕像素长度
	uint16_t Height;           // 屏幕像素宽度
	uint8_t  X_Offset;         // X坐标偏移，用于设置屏幕控制器的显存写入方式
	uint8_t  Y_Offset;         // Y坐标偏移，用于设置屏幕控制器的显存写入方式
}ST7789;

void ST7789_WriteCommand(uint8_t cmd){
    ST7789_DC_Command;     // 数据指令选择 引脚输出低电平，代表本次传输 指令
    HAL_SPI_Transmit(&ST7789_Dev, &cmd, 1, 10) ;
}

void ST7789_WriteData(uint8_t data){
    ST7789_DC_Data;        // 数据指令选择 引脚输出高电平，代表本次传输 数据
    HAL_SPI_Transmit(&ST7789_Dev, &data, 1, 10) ;   // 启动SPI传输
}

void ST7789_WriteData_8bit(uint8_t lcd_data)
{
	ST7789_DC_Data;        // 数据指令选择 引脚输出高电平，代表本次传输 数据
	HAL_SPI_Transmit(&ST7789_Dev, &lcd_data, 1, 10) ;   // 启动SPI传输
}

void  ST7789_WriteData_16bit(uint16_t lcd_data)
{
    uint8_t lcd_data_buff[2];    // 数据发送区
    ST7789_DC_Data;      // 数据指令选择 引脚输出高电平，代表本次传输 数据

    lcd_data_buff[0] = lcd_data>>8;  // 将数据拆分
    lcd_data_buff[1] = lcd_data;

    HAL_SPI_Transmit(&ST7789_Dev, lcd_data_buff, 2, 100) ;   // 启动SPI传输
}

/****************************************************************************************************************************************
*	函 数 名:	ST7789_SetDirection
*
*	入口参数:	direction - 要显示的方向
*
*	函数功能:	设置要显示的方向
*
*	说    明:   1. 可输入参数 Direction_H 、Direction_V 、Direction_H_Flip 、Direction_V_Flip
*              2. 使用示例 ST7789_DisplayDirection(Direction_H) ，即设置屏幕横屏显示
*
*****************************************************************************************************************************************/

void ST7789_SetDirection(uint8_t direction)
{
	ST7789.Direction = direction;    // 写入全局ST7789参数

	if( direction == Direction_H )   // 横屏显示
	{
		ST7789_WriteCommand(0x36);    		// 显存访问控制 指令，用于设置访问显存的方式
		ST7789_WriteData_8bit(0x70);        // 横屏显示
		ST7789.X_Offset   = 0;             // 设置控制器坐标偏移量
		ST7789.Y_Offset   = 0;
		ST7789.Width      = ST7789_HEIGHT;		// 重新赋值长、宽
		ST7789.Height     = ST7789_WIDTH;
	}
	else if( direction == Direction_V )
	{
		ST7789_WriteCommand(0x36);    		// 显存访问控制 指令，用于设置访问显存的方式
		ST7789_WriteData_8bit(0x00);        // 垂直显示
		ST7789.X_Offset   = 0;              // 设置控制器坐标偏移量
		ST7789.Y_Offset   = 0;
		ST7789.Width      = ST7789_WIDTH;		// 重新赋值长、宽
		ST7789.Height     = ST7789_HEIGHT;
	}
	else if( direction == Direction_H_Flip )
	{
		ST7789_WriteCommand(0x36);   			 // 显存访问控制 指令，用于设置访问显存的方式
		ST7789_WriteData_8bit(0xA0);         // 横屏显示，并上下翻转，RGB像素格式
		ST7789.X_Offset   = 80;              // 设置控制器坐标偏移量
		ST7789.Y_Offset   = 0;
		ST7789.Width      = ST7789_HEIGHT;		 // 重新赋值长、宽
		ST7789.Height     = ST7789_WIDTH;
	}
	else if( direction == Direction_V_Flip )
	{
		ST7789_WriteCommand(0x36);    		// 显存访问控制 指令，用于设置访问显存的方式
		ST7789_WriteData_8bit(0xC0);        // 垂直显示 ，并上下翻转，RGB像素格式
		ST7789.X_Offset   = 0;              // 设置控制器坐标偏移量
		ST7789.Y_Offset   = 80;
		ST7789.Width      = ST7789_WIDTH;		// 重新赋值长、宽
		ST7789.Height     = ST7789_HEIGHT;
	}
}

/****************************************************************************************************************************************
*	函 数 名:	ST7789_SetBackColor
*
*	入口参数:	Color - 要显示的颜色，示例：0x0000FF 表示蓝色
*
*	函数功能:	设置背景色,此函数用于清屏以及显示字符的背景色
*
*	说    明:	1. 为了方便用户使用自定义颜色，入口参数 Color 使用24位 RGB888的颜色格式，用户无需关心颜色格式的转换
*					2. 24位的颜色中，从高位到低位分别对应 R、G、B  3个颜色通道
*
*****************************************************************************************************************************************/

void ST7789_SetBackColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //各个颜色通道的值

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // 转换成 16位 的RGB565颜色
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	ST7789.BackColor = (uint16_t)(Red_Value | Green_Value | Blue_Value);	// 将颜色写入全局ST7789参数
}

/****************************************************************************************************************************************
*	函 数 名:	ST7789_SetColor
*
*	入口参数:	Color - 要显示的颜色，示例：0x0000FF 表示蓝色
*
*	函数功能:	此函数用于设置画笔的颜色，例如显示字符、画点画线、绘图的颜色
*
*	说    明:	1. 为了方便用户使用自定义颜色，入口参数 Color 使用24位 RGB888的颜色格式，用户无需关心颜色格式的转换
*					2. 24位的颜色中，从高位到低位分别对应 R、G、B  3个颜色通道
*
*****************************************************************************************************************************************/

void ST7789_SetColor(uint32_t Color)
{
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0; //各个颜色通道的值

	Red_Value   = (uint16_t)((Color&0x00F80000)>>8);   // 转换成 16位 的RGB565颜色
	Green_Value = (uint16_t)((Color&0x0000FC00)>>5);
	Blue_Value  = (uint16_t)((Color&0x000000F8)>>3);

	ST7789.Color = (uint16_t)(Red_Value | Green_Value | Blue_Value);  // 将颜色写入全局ST7789参数
}

/****************************************************************************************************************************************
*	函 数 名:	ST7789_SetAsciiFont
*
*	入口参数:	*fonts - 要设置的ASCII字体
*
*	函数功能:	设置ASCII字体，可选择使用 3216/2412/2010/1608/1206 五种大小的字体
*
*	说    明:	1. 使用示例 ST7789_SetAsciiFont(&ASCII_Font24) ，即设置 2412的 ASCII字体
*					2. 相关字模存放在 lcd_fonts.c
*
*****************************************************************************************************************************************/

void ST7789_SetAsciiFont(pFONT *Asciifonts)
{
	ST7789_AsciiFonts = Asciifonts;
}

/*****************************************************************************************************************************************
*	函 数 名:	LCD_ShowNumMode
*
*	入口参数:	mode - 设置变量的显示模式
*
*	函数功能:	设置变量显示时多余位补0还是补空格，可输入参数 Fill_Space 填充空格，Fill_Zero 填充零
*
*	说    明:   1. 只有 LCD_DisplayNumber() 显示整数 和 LCD_DisplayDecimals()显示小数 这两个函数用到
*					2. 使用示例 LCD_ShowNumMode(Fill_Zero) 设置多余位填充0，例如 123 可以显示为 000123
*
*****************************************************************************************************************************************/
void ST7789_ShowNumMode(uint8_t mode)
{
	ST7789.ShowNum_Mode = mode;
}

/****************************************************************************************************************************************
*	函 数 名:	 LCD_SetAddress
*
*	入口参数:	 x1 - 起始水平坐标   y1 - 起始垂直坐标
*              x2 - 终点水平坐标   y2 - 终点垂直坐标
*
*	函数功能:   设置需要显示的坐标区域
*****************************************************************************************************************************************/

void ST7789_SetAddress(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	ST7789_WriteCommand(0x2a);			//	列地址设置，即X坐标
	ST7789_WriteData_16bit(x1+ST7789.X_Offset);
	ST7789_WriteData_16bit(x2+ST7789.X_Offset);

	ST7789_WriteCommand(0x2b);			//	行地址设置，即Y坐标
	ST7789_WriteData_16bit(y1+ST7789.Y_Offset);
	ST7789_WriteData_16bit(y2+ST7789.Y_Offset);

	ST7789_WriteCommand(0x2c);			//	开始写入显存，即要显示的颜色数据
}

void ST7789_Init(){
	HAL_Delay(10);               // 屏幕刚完成复位时（包括上电复位），需要等待5ms才能发送指令
 	ST7789_WriteCommand(0x36);       // 显存访问控制 指令，用于设置访问显存的方式
	ST7789_WriteData_8bit(0x00);     // 配置成 从上到下、从左到右，RGB像素格式

	ST7789_WriteCommand(0x3A);			// 接口像素格式 指令，用于设置使用 12位、16位还是18位色
	ST7789_WriteData_8bit(0x05);     // 此处配置成 16位 像素格式

// 接下来很多都是电压设置指令，直接使用厂家给设定值
 	ST7789_WriteCommand(0xB2);
	ST7789_WriteData_8bit(0x0C);
	ST7789_WriteData_8bit(0x0C);
	ST7789_WriteData_8bit(0x00);
	ST7789_WriteData_8bit(0x33);
	ST7789_WriteData_8bit(0x33);

	ST7789_WriteCommand(0xB7);		   // 栅极电压设置指令
	ST7789_WriteData_8bit(0x35);     // VGH = 13.26V，VGL = -10.43V

	ST7789_WriteCommand(0xBB);			// 公共电压设置指令
	ST7789_WriteData_8bit(0x19);     // VCOM = 1.35V

	ST7789_WriteCommand(0xC0);
	ST7789_WriteData_8bit(0x2C);

	ST7789_WriteCommand(0xC2);       // VDV 和 VRH 来源设置
	ST7789_WriteData_8bit(0x01);     // VDV 和 VRH 由用户自由配置

	ST7789_WriteCommand(0xC3);			// VRH电压 设置指令
	ST7789_WriteData_8bit(0x12);     // VRH电压 = 4.6+( vcom+vcom offset+vdv)

	ST7789_WriteCommand(0xC4);		   // VDV电压 设置指令
	ST7789_WriteData_8bit(0x20);     // VDV电压 = 0v

	ST7789_WriteCommand(0xC6); 		// 正常模式的帧率控制指令
	ST7789_WriteData_8bit(0x0F);   	// 设置屏幕控制器的刷新帧率为60帧

	ST7789_WriteCommand(0xD0);			// 电源控制指令
	ST7789_WriteData_8bit(0xA4);     // 无效数据，固定写入0xA4
	ST7789_WriteData_8bit(0xA1);     // AVDD = 6.8V ，AVDD = -4.8V ，VDS = 2.3V

	ST7789_WriteCommand(0xE0);       // 正极电压伽马值设定
	ST7789_WriteData_8bit(0xD0);
	ST7789_WriteData_8bit(0x04);
	ST7789_WriteData_8bit(0x0D);
	ST7789_WriteData_8bit(0x11);
	ST7789_WriteData_8bit(0x13);
	ST7789_WriteData_8bit(0x2B);
	ST7789_WriteData_8bit(0x3F);
	ST7789_WriteData_8bit(0x54);
	ST7789_WriteData_8bit(0x4C);
	ST7789_WriteData_8bit(0x18);
	ST7789_WriteData_8bit(0x0D);
	ST7789_WriteData_8bit(0x0B);
	ST7789_WriteData_8bit(0x1F);
	ST7789_WriteData_8bit(0x23);

	ST7789_WriteCommand(0xE1);      // 负极电压伽马值设定
	ST7789_WriteData_8bit(0xD0);
	ST7789_WriteData_8bit(0x04);
	ST7789_WriteData_8bit(0x0C);
	ST7789_WriteData_8bit(0x11);
	ST7789_WriteData_8bit(0x13);
	ST7789_WriteData_8bit(0x2C);
	ST7789_WriteData_8bit(0x3F);
	ST7789_WriteData_8bit(0x44);
	ST7789_WriteData_8bit(0x51);
	ST7789_WriteData_8bit(0x2F);
	ST7789_WriteData_8bit(0x1F);
	ST7789_WriteData_8bit(0x1F);
	ST7789_WriteData_8bit(0x20);
	ST7789_WriteData_8bit(0x23);

	ST7789_WriteCommand(0x21);       // 打开反显，因为面板是常黑型，操作需要反过来

 // 退出休眠指令，ST7789控制器在刚上电、复位时，会自动进入休眠模式 ，因此操作屏幕之前，需要退出休眠
	ST7789_WriteCommand(0x11);       // 退出休眠 指令
   HAL_Delay(120);               // 需要等待120ms，让电源电压和时钟电路稳定下来

 // 打开显示指令，ST7789控制器在刚上电、复位时，会自动关闭显示
	ST7789_WriteCommand(0x29);       // 打开显示

// 以下进行一些驱动的默认设置
	ST7789_SetDirection(Direction_V);  	      //	设置显示方向
	ST7789_SetBackColor(BLACK);           // 设置背景色
 	ST7789_SetColor(WHITE);               // 设置画笔色
	ST7789_Clear();                           // 清屏

   ST7789_SetAsciiFont(&ASCII_Font24);       // 设置默认字体
   ST7789_ShowNumMode(Fill_Zero);	      	// 设置变量显示模式，多余位填充空格还是填充0

// 全部设置完毕之后，打开背光
   ST7789_Backlight_ON;  // 引脚输出高电平点亮背光
}


void ST7789_DisPlayAll(){
	int i;
	ST7789_SetAddress(0,0, ST7789_WIDTH-1, ST7789_HEIGHT-1);  // 设置显示区域为全屏
	ST7789_DC_Data;      // 数据指令选择 引脚输出高电平，代表本次传输 数据
	// for (i = 0; i < ST7789_WIDTH; i++) {
	// 	int j;
	// 	for (j = 0; j < ST7789_HEIGHT; j++) {
	// 		uint8_t lcd_data_buff[2];    // 数据发送区
	// 		lcd_data_buff[0] = FrameBuffer[i][j]>>8;  // 将数据拆分
	// 		lcd_data_buff[1] = FrameBuffer[i][j];
	//
	// 		HAL_SPI_Transmit(&ST7789_Dev, lcd_data_buff, 2, 1000) ;   // 启动SPI传输
	// 	}
	// }
	HAL_SPI_Transmit(&ST7789_Dev, FrameBuffer, ST7789_WIDTH*ST7789_HEIGHT*2, 1);
}

void ST7789_Clear(){
	// ST7789_SetAddress(0,0, ST7789_WIDTH-1, ST7789_HEIGHT-1);  // 设置显示区域为全屏
	// uint32_t total_pixels = ST7789_WIDTH * ST7789_HEIGHT;  // 计算需要填充的像素数量
	// uint8_t color_bytes[2] = {(uint8_t)(ST7789.BackColor >> 8), (uint8_t)(ST7789.BackColor & 0xFF)}; // 背景色转换为16位RGB565格式
	// ST7789_DC_Data;      // 数据指令选择 引脚输出高电平，
	// for(uint32_t i = 0; i < total_pixels; i++) {
	// 	HAL_SPI_Transmit(&ST7789_Dev, color_bytes, 2, 1); // 使用硬件加速填充
	// }
	// HAL_SPI_Transmit(&ST7789_Dev, FrameBuffer, 2, ;1)
}