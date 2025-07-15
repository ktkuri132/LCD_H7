//
// Created by 34575 on 25-7-8.
//

#ifndef ST7789_H
#define ST7789_H


#include <spi.h>

#define ST7789_Dev hspi6

#define ST7789_WIDTH 240
#define ST7789_HEIGHT 240

#define  ST7789_Backlight_PIN								GPIO_PIN_12				         // 背光  引脚
#define	ST7789_Backlight_PORT							GPIOG									// 背光 GPIO端口
#define 	GPIO_LDC_Backlight_CLK_ENABLE        	__HAL_RCC_GPIOG_CLK_ENABLE()	// 背光 GPIO时钟

#define	ST7789_Backlight_OFF		HAL_GPIO_WritePin(ST7789_Backlight_PORT, ST7789_Backlight_PIN, GPIO_PIN_RESET);	// 低电平，关闭背光

#define 	ST7789_Backlight_ON		HAL_GPIO_WritePin(ST7789_Backlight_PORT, ST7789_Backlight_PIN, GPIO_PIN_SET);		// 高电平，开启背光

#define  ST7789_DC_PIN						GPIO_PIN_15				         // 数据指令选择  引脚
#define	ST7789_DC_PORT						GPIOG									// 数据指令选择  GPIO端口
#define 	GPIO_ST7789_DC_CLK_ENABLE     __HAL_RCC_GPIOG_CLK_ENABLE()	// 数据指令选择  GPIO时钟

#define	ST7789_DC_Command		   HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_RESET);	   // 低电平，指令传输
#define 	ST7789_DC_Data		      HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_SET);		// 高电平，数据传输

// 显示方向参数
// 使用示例：LCD_DisplayDirection(Direction_H) 设置屏幕横屏显示
#define	Direction_H				0					//LCD横屏显示
#define	Direction_H_Flip	   1					//LCD横屏显示,上下翻转
#define	Direction_V				2					//LCD竖屏显示
#define	Direction_V_Flip	   3					//LCD竖屏显示,上下翻转

/*---------------------------------------- 常用颜色 ------------------------------------------------------

 1. 这里为了方便用户使用，定义的是24位 RGB888颜色，然后再通过代码自动转换成 16位 RGB565 的颜色
 2. 24位的颜色中，从高位到低位分别对应 R、G、B  3个颜色通道
 3. 用户可以在电脑用调色板获取24位RGB颜色，再将颜色输入LCD_SetColor()或LCD_SetBackColor()就可以显示出相应的颜色
 */
#define 	WHITE       0xFFFFFF	 // 纯白色
#define 	BLACK       0x000000    // 纯黑色

#define 	BLUE        0x0000FF	 //	纯蓝色
#define 	GREEN       0x00FF00    //	纯绿色
#define 	RED         0xFF0000    //	纯红色
#define 	CYAN        0x00FFFF    //	蓝绿色
#define 	MAGENTA     0xFF00FF    //	紫红色
#define 	YELLOW      0xFFFF00    //	黄色
#define 	GREY        0x2C2C2C    //	灰色

#define 	LIGHT_BLUE      0x8080FF    //	亮蓝色
#define 	LIGHT_GREEN     0x80FF80    //	亮绿色
#define 	LIGHT_RED       0xFF8080    //	亮红色
#define 	LIGHT_CYAN      0x80FFFF    //	亮蓝绿色
#define 	LIGHT_MAGENTA   0xFF80FF    //	亮紫红色
#define 	LIGHT_YELLOW    0xFFFF80    //	亮黄色
#define 	LIGHT_GREY      0xA3A3A3    //	亮灰色

#define 	DARK_BLUE       0x000080    //	暗蓝色
#define 	DARK_GREEN      0x008000    //	暗绿色
#define 	DARK_RED        0x800000    //	暗红色
#define 	DARK_CYAN       0x008080    //	暗蓝绿色
#define 	DARK_MAGENTA    0x800080    //	暗紫红色
#define 	DARK_YELLOW     0x808000    //	暗黄色
#define 	DARK_GREY       0x404040    //	暗灰色

// 设置变量显示时多余位补0还是补空格
// 只有 LCD_DisplayNumber() 显示整数 和 LCD_DisplayDecimals()显示小数 这两个函数用到
// 使用示例： LCD_ShowNumMode(Fill_Zero) 设置多余位填充0，例如 123 可以显示为 000123
#define  Fill_Zero  0		//填充0
#define  Fill_Space 1		//填充空格

extern uint16_t FrameBuffer[ST7789_WIDTH][ST7789_HEIGHT] __attribute__((__section__(".ram_d1")));  // 帧缓冲区

void ST7789_Init();
void ST7789_DisPlayAll();
void ST7789_Clear();
#endif //ST7789_H
