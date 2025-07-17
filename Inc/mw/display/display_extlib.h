/**
 * @file display_extlib.h
 * @brief OLED显示扩展图形与文本接口
 */

#ifndef DISPLAY_LIB_H
#define DISPLAY_LIB_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <math.h>
#include "display.h"

// ================== 屏幕与字体定义 ==================

#define DISPLAY_WIDTH  128  ///< 屏幕宽度
#define DISPLAY_HEIGHT 64   ///< 屏幕高度

/**
 * @brief 字体样式枚举
 */
typedef enum {
    FONT_STYLE_SMALL,      ///< 5x7 小字体
    FONT_STYLE_MEDIUM,     ///< 8x12 中等字体
    FONT_STYLE_LARGE,      ///< 12x16 大字体
    FONT_STYLE_COUNT
} FontStyle;

/**
 * @brief 字体结构体
 */
typedef struct {
    const uint8_t *bitmap;  ///< 位图数据指针
    uint8_t width;          ///< 字符宽度
    uint8_t height;         ///< 字符高度
    uint8_t first_char;     ///< 第一个字符的ASCII值
    uint8_t last_char;      ///< 最后一个字符的ASCII值
    uint8_t bytes_per_char; ///< 每个字符占用的字节数
} FontDef;

// ================== 基础显示与文本接口 ==================

/**
 * @brief 初始化显示
 */
void display_init(void);

/**
 * @brief 清空显示缓冲区
 */
void display_clear(void);

/**
 * @brief 刷新缓冲区到实际显示
 */
void display_flush(void);

/**
 * @brief 绘制单个字符
 * @param x X坐标
 * @param y Y坐标
 * @param c 字符
 * @param style 字体样式
 * @param color 颜色
 */
void draw_char(uint8_t x, uint8_t y, char c, FontStyle style, PixelColor color);

/**
 * @brief 绘制字符串
 * @param x X坐标
 * @param y Y坐标
 * @param str 字符串
 * @param style 字体样式
 * @param color 颜色
 */
void draw_string(uint8_t x, uint8_t y, const char *str, FontStyle style, PixelColor color);

/**
 * @brief 绘制UTF-8字符串
 * @param x X坐标
 * @param y Y坐标
 * @param str 字符串
 * @param style 字体样式
 * @param color 颜色
 */
void draw_utf8_string(uint8_t x, uint8_t y, const char *str, FontStyle style, PixelColor color);

// ================== 基础与高级图形接口 ==================

/**
 * @brief 绘制矩形
 * @param x 左上角X
 * @param y 左上角Y
 * @param w 宽度
 * @param h 高度
 * @param filled 是否填充
 * @param color 颜色
 */
void draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool filled, PixelColor color);

/**
 * @brief 绘制三角形
 * @param x0 顶点0 X
 * @param y0 顶点0 Y
 * @param x1 顶点1 X
 * @param y1 顶点1 Y
 * @param x2 顶点2 X
 * @param y2 顶点2 Y
 * @param filled 是否填充
 * @param color 颜色
 */
void draw_triangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, bool filled, PixelColor color);

/**
 * @brief 绘制位图
 * @param x 左上角X
 * @param y 左上角Y
 * @param w 宽度
 * @param h 高度
 * @param bitmap 位图数据
 * @param color 颜色
 */
void draw_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap, PixelColor color);

/**
 * @brief 绘制进度条
 * @param x 左上角X
 * @param y 左上角Y
 * @param w 宽度
 * @param h 高度
 * @param progress 进度(0-100)
 * @param color 颜色
 */
void draw_progress_bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t progress, PixelColor color);

/**
 * @brief 绘制圆角矩形
 * @param x 左上角X
 * @param y 左上角Y
 * @param w 宽度
 * @param h 高度
 * @param r 圆角半径
 * @param filled 是否填充
 * @param color 颜色
 */
void draw_rounded_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, bool filled, PixelColor color);

/**
 * @brief 获取字体信息
 * @param style 字体样式
 * @return 字体定义指针
 */
const FontDef* get_font_def(FontStyle style);

// 本文件专注于文本、字体、UI控件等字符与高级图形接口声明
// 图像对象与像素算法请见 display.h

#endif // DISPLAY_LIB_H