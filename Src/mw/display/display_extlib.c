/**
 * @file display_extlib.c
 * @brief OLED显示扩展图形与文本实现
 */

#include "../../../Inc/mw/display/display_extlib.h"
#include "../../../Inc/mw/display/display.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

// ================== 显存缓冲区 ==================
// extern uint8_t FrameBuffer[SCREEN_HEIGHT/8][SCREEN_WIDTH];

// ================== UTF-8解码状态 ==================
static uint32_t utf8_state = 0;
static uint32_t utf8_code = 0;

// ================== 基础显示控制 ==================

/**
 * @brief 初始化显示
 */
void display_init(void) {
    memset(FrameBuffer, 0, sizeof(FrameBuffer));
}

/**
 * @brief 清空显示缓冲区
 */
void display_clear(void) {
    memset(FrameBuffer, 0, sizeof(FrameBuffer));
}

/**
 * @brief 刷新缓冲区到实际显示
 */
void display_flush(void) {
    // 需根据实际硬件实现
}

// ================== 文本绘制 ==================

/**
 * @brief 绘制单个字符
 */
void draw_char(uint8_t x, uint8_t y, char c, FontStyle style, PixelColor color) {
    const FontDef* font = get_font_def(style);
    if (c < font->first_char || c > font->last_char) {
        c = '?';
    }
    uint16_t char_index = (c - font->first_char) * font->bytes_per_char;
    const uint8_t* char_bitmap = &font->bitmap[char_index];
    for (uint8_t col = 0; col < font->width; col++) {
        uint8_t col_data = char_bitmap[col];
        for (uint8_t bit = 0; bit < 8; bit++) {
            if (y + bit >= DISPLAY_HEIGHT) break;
            if (col_data & (1 << bit)) {
                set_pixel(x + col, y + bit, color);
            }
        }
    }
}

/**
 * @brief 绘制字符串
 */
void draw_string(uint8_t x, uint8_t y, const char *str, FontStyle style, PixelColor color) {
    const FontDef* font = get_font_def(style);
    uint8_t cur_x = x;
    while (*str) {
        draw_char(cur_x, y, *str, style, color);
        cur_x += font->width + 1;
        str++;
    }
}

/**
 * @brief UTF-8解码器
 */
static bool decode_utf8(uint8_t byte, uint32_t *state, uint32_t *code) {
    if (*state == 0) {
        if (byte < 0x80) {
            *code = byte;
            return true;
        } else if (byte >= 0xC2 && byte <= 0xDF) {
            *state = 1;
            *code = byte & 0x1F;
        } else if (byte >= 0xE0 && byte <= 0xEF) {
            *state = 2;
            *code = byte & 0x0F;
        } else if (byte >= 0xF0 && byte <= 0xF4) {
            *state = 3;
            *code = byte & 0x07;
        } else {
            return false;
        }
    } else {
        if (byte < 0x80 || byte > 0xBF) {
            *state = 0;
            return false;
        }
        *code = (*code << 6) | (byte & 0x3F);
        (*state)--;
        if (*state == 0) {
            return true;
        }
    }
    return false;
}

/**
 * @brief 绘制UTF-8字符串
 */
void draw_utf8_string(uint8_t x, uint8_t y, const char *str, FontStyle style, PixelColor color) {
    const FontDef* font = get_font_def(style);
    uint8_t cur_x = x;
    while (*str) {
        if (decode_utf8((uint8_t)*str, &utf8_state, &utf8_code)) {
            if (utf8_code < 0x80) {
                draw_char(cur_x, y, (char)utf8_code, style, color);
                cur_x += font->width + 1;
            }
            // 其它语言字符可扩展
        }
        str++;
    }
}

// ================== 基础与高级图形绘制 ==================

/**
 * @brief 绘制矩形
 */
void draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool filled, PixelColor color) {
    if (filled) {
        for (uint8_t i = y; i < y + h; i++) {
            for (uint8_t j = x; j < x + w; j++) {
                set_pixel(j, i, color);
            }
        }
    } else {
        for (uint8_t i = x; i < x + w; i++) {
            set_pixel(i, y, color);
            set_pixel(i, y + h - 1, color);
        }
        for (uint8_t i = y; i < y + h; i++) {
            set_pixel(x, i, color);
            set_pixel(x + w - 1, i, color);
        }
    }
}

/**
 * @brief 绘制三角形
 */
void draw_triangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,
                   uint8_t x2, uint8_t y2, bool filled, PixelColor color) {
    if (filled) {
        uint8_t min_y = y0, max_y = y0;
        if (y1 < min_y) min_y = y1;
        if (y2 < min_y) min_y = y2;
        if (y1 > max_y) max_y = y1;
        if (y2 > max_y) max_y = y2;
        for (uint8_t y = min_y; y <= max_y; y++) {
            int xa = -1, xb = -1;
            if (y >= min_y && y <= max_y) {
                if ((y0 <= y && y <= y1) || (y1 <= y && y <= y0)) {
                    float t = (float)(y - y0) / (y1 - y0);
                    int x = x0 + t * (x1 - x0);
                    if (xa == -1) xa = x;
                    else if (xb == -1) xb = x;
                }
                if ((y1 <= y && y <= y2) || (y2 <= y && y <= y1)) {
                    float t = (float)(y - y1) / (y2 - y1);
                    int x = x1 + t * (x2 - x1);
                    if (xa == -1) xa = x;
                    else if (xb == -1) xb = x;
                }
                if ((y2 <= y && y <= y0) || (y0 <= y && y <= y2)) {
                    float t = (float)(y - y2) / (y0 - y2);
                    int x = x2 + t * (x0 - x2);
                    if (xa == -1) xa = x;
                    else if (xb == -1) xb = x;
                }
                if (xa != -1 && xb != -1) {
                    if (xa > xb) {
                        int temp = xa;
                        xa = xb;
                        xb = temp;
                    }
                    for (int x = xa; x <= xb; x++) {
                        set_pixel(x, y, color);
                    }
                }
            }
        }
    } else {
        draw_line((Point){x0, y0}, (Point){x1, y1}, color);
        draw_line((Point){x1, y1}, (Point){x2, y2}, color);
        draw_line((Point){x2, y2}, (Point){x0, y0}, color);
    }
}

/**
 * @brief 绘制位图
 */
void draw_bitmap(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap, PixelColor color) {
    for (uint8_t i = 0; i < h; i++) {
        for (uint8_t j = 0; j < w; j++) {
            uint16_t byte_index = i * ((w + 7) / 8) + (j / 8);
            uint8_t bit_mask = 1 << (j % 8);
            if (bitmap[byte_index] & bit_mask) {
                set_pixel(x + j, y + i, color);
            }
        }
    }
}

/**
 * @brief 绘制进度条
 */
void draw_progress_bar(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t progress, PixelColor color) {
    draw_rect(x, y, w, h, false, color);
    uint8_t fill_width = (progress * (w - 4)) / 100;
    if (fill_width > 0) {
        draw_rect(x + 2, y + 2, fill_width, h - 4, true, color);
    }
}

/**
 * @brief 绘制圆角矩形
 */
void draw_rounded_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r, bool filled, PixelColor color) {
    if (!filled) {
        draw_line((Point){x + r , y}, (Point){x + w - r - 1, y}, color);
        draw_line((Point){x + r, y + h - 1}, (Point){x + w - r - 1, y + h - 1}, color);
        draw_line((Point){x, y + r}, (Point){x, y + h - r - 1}, color);
        draw_line((Point){x + w - 1, y + r}, (Point){x + w - 1, y + h - r - 1}, color);
        draw_circle((Point){x + r, y + r}, r, color, false);
        draw_circle((Point){x + w - r - 1, y + r}, r, color, false);
        draw_circle((Point){x + r, y + h - r - 1}, r, color, false);
        draw_circle((Point){x + w - r - 1, y + h - r - 1}, r, color, false);
    } else {
        draw_rect(x, y + r, w, h - 2 * r, true, color);
        draw_rect(x + r, y, w - 2 * r, r, true, color);
        draw_rect(x + r, y + h - r, w - 2 * r, r, true, color);
        for (uint8_t i = 0; i < r; i++) {
            uint8_t dx = (uint8_t)sqrt(r * r - i * i);
            draw_line((Point){x + r - dx, y + r - i}, (Point){x + r + dx, y + r - i}, color);
            draw_line((Point){x + r - dx, y + r + i}, (Point){x + r + dx, y + r + i}, color);
            draw_line((Point){x + w - r - dx, y + r - i}, (Point){x + w - r + dx, y + r - i}, color);
            draw_line((Point){x + w - r - dx, y + r + i}, (Point){x + w - r + dx, y + r + i}, color);
        }
    }
}

// 本文件专注于文本、字体、UI控件等字符与高级图形实现
// 图像对象与像素算法请见 display.c
