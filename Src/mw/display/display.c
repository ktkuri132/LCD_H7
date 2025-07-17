/**
 * @file mw_display.c
 * @brief OLED图形显示核心实现与对象管理
 */

#include "../../../Inc/mw/display/display.h"
#include <stdlib.h>

// ================== 显存缓冲区 ==================
#if DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_MONO
uint8_t FrameBuffer[SCREEN_HEIGHT/8][SCREEN_WIDTH];
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB565
uint16_t  FrameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH] __attribute__((__section__(".ram_d1")));
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB888
uint32_t FrameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
#endif

// ================== 基础像素操作 ==================

/**
 * @brief 设置像素点状态（支持多色深）
 */
void set_pixel(uint16_t x, uint16_t y, PixelColor color) {
#if DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_MONO
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        uint8_t page = y / 8;
        uint8_t bit_mask = 1 << (y % 8);
        if (color == PIXEL_ON) {
            FrameBuffer[page][x] |= bit_mask;
        } else {
            FrameBuffer[page][x] &= ~bit_mask;
        }
    }
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB565
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        FrameBuffer[y][x] = color;
    }
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB888
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        FrameBuffer[y][x] = color;
    }
#endif
}

/**
 * @brief 获取像素点状态（支持多色深）
 */
PixelColor get_pixel(uint16_t x, uint16_t y) {
#if DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_MONO
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        uint8_t page = y / 8;
        uint8_t bit_mask = 1 << (y % 8);
        return (FrameBuffer[page][x] & bit_mask) ? PIXEL_ON : PIXEL_OFF;
    }
    return PIXEL_OFF;
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB565
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        return FrameBuffer[y][x];
    }
    return 0;
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB888
    if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
        return FrameBuffer[y][x];
    }
    return 0;
#endif
}

/**
 * @brief 翻转像素点状态（单色有效）
 */
void toggle_pixel(uint16_t x, uint16_t y) {
#if DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_MONO
    set_pixel(x, y, !get_pixel(x, y));
#else
    // 彩色模式下不支持toggle
#endif
}

// ================== 基础图形绘制 ==================

/**
 * @brief 绘制直线（Bresenham算法）
 */
void draw_line(Point p1, Point p2, PixelColor state) {
    int16_t dx = abs(p2.x - p1.x);
    int16_t dy = -abs(p2.y - p1.y);
    int16_t sx = p1.x < p2.x ? 1 : -1;
    int16_t sy = p1.y < p2.y ? 1 : -1;
    int16_t err = dx + dy, e2;
    while (1) {
        set_pixel(p1.x, p1.y, state);
        if (p1.x == p2.x && p1.y == p2.y) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; p1.x += sx; }
        if (e2 <= dx) { err += dx; p1.y += sy; }
    }
}

/**
 * @brief 绘制水平线
 */
void draw_horizontal_line(int16_t x0, int16_t x1, int16_t y, PixelColor state) {
    if (x0 > x1) { int16_t t = x0; x0 = x1; x1 = t; }
    for (int16_t x = x0; x <= x1; x++) {
        set_pixel(x, y, state);
    }
}

/**
 * @brief 绘制圆（中点圆算法）
 */
void draw_circle(Point center, int16_t radius, PixelColor state, uint8_t filled) {
    int16_t x = radius;
    int16_t y = 0;
    int16_t err = 0;
    while (x >= y) {
        if (filled) {
            draw_horizontal_line(center.x - x, center.x + x, center.y + y, state);
            draw_horizontal_line(center.x - x, center.x + x, center.y - y, state);
            draw_horizontal_line(center.x - y, center.x + y, center.y + x, state);
            draw_horizontal_line(center.x - y, center.x + y, center.y - x, state);
        } else {
            set_pixel(center.x + x, center.y + y, state);
            set_pixel(center.x + y, center.y + x, state);
            set_pixel(center.x - y, center.y + x, state);
            set_pixel(center.x - x, center.y + y, state);
            set_pixel(center.x - x, center.y - y, state);
            set_pixel(center.x - y, center.y - x, state);
            set_pixel(center.x + y, center.y - x, state);
            set_pixel(center.x + x, center.y - y, state);
        }
        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

// ================== 图形对象管理 ==================

#define MAX_OBJECTS 20 ///< 最大对象数
static GraphicObject* objectPool[MAX_OBJECTS] = {0};

/**
 * @brief 初始化图形对象池
 */
void init_graphics() {
    for (int i = 0; i < MAX_OBJECTS; i++) {
        objectPool[i] = NULL;
    }
}

/**
 * @brief 创建一个图形对象
 */
GraphicObject* create_shape(ShapeType type, Point position, void* shapeData) {
    int slot = -1;
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (objectPool[i] == NULL) {
            slot = i;
            break;
        }
    }
    if (slot == -1) return NULL;
    GraphicObject* obj = malloc(sizeof(GraphicObject));
    obj->type = type;
    obj->position = position;
    obj->rotation = 0;
    obj->visible = 1;
    obj->shapeData = shapeData;
    switch (type) {
        case SHAPE_CIRCLE:
            obj->drawFunc = draw_circle_object;
            break;
        case SHAPE_TRIANGLE:
            obj->drawFunc = draw_triangle_object;
            break;
        // 其他图形类型...
        default:
            obj->drawFunc = NULL;
    }
    objectPool[slot] = obj;
    return obj;
}

/**
 * @brief 删除一个图形对象
 */
void delete_shape(GraphicObject* obj) {
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (objectPool[i] == obj) {
            free(obj->shapeData);
            free(obj);
            objectPool[i] = NULL;
            break;
        }
    }
}

/**
 * @brief 检查对象是否越界
 */
static bool check_boundary(const GraphicObject* obj, const MoveOption* opt, int16_t new_x, int16_t new_y) {
    // 这里只以圆和三角形为例，其他类型可扩展
    if (obj->type == SHAPE_CIRCLE) {
        CircleData* data = (CircleData*)obj->shapeData;
        int16_t r = data->radius;
        if ((opt->boundary & BOUNDARY_LEFT)   && (new_x - r < 0)) return true;
        if ((opt->boundary & BOUNDARY_RIGHT)  && (new_x + r >= SCREEN_WIDTH)) return true;
        if ((opt->boundary & BOUNDARY_TOP)    && (new_y - r < 0)) return true;
        if ((opt->boundary & BOUNDARY_BOTTOM) && (new_y + r >= SCREEN_HEIGHT)) return true;
    }
    // ...三角形、矩形等可扩展...
    return false;
}

/**
 * @brief 简单像素级碰撞检测（仅圆形为例）
 */
static bool check_pixel_collision(const GraphicObject* obj, int16_t new_x, int16_t new_y) {
    if (obj->type == SHAPE_CIRCLE) {
        CircleData* data = (CircleData*)obj->shapeData;
        int16_t r = data->radius;
        for (int16_t y = -r; y <= r; y++) {
            for (int16_t x = -r; x <= r; x++) {
                if (x*x + y*y <= r*r) {
                    int16_t px = new_x + x;
                    int16_t py = new_y + y;
                    if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                        if (get_pixel(px, py)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    // ...其他类型可扩展...
    return false;
}

/**
 * @brief 处理像素重合（仅圆形为例）
 */
static void handle_overlap(const GraphicObject* obj, int16_t new_x, int16_t new_y, OverlapHandleType overlap) {
    if (obj->type == SHAPE_CIRCLE) {
        CircleData* data = (CircleData*)obj->shapeData;
        int16_t r = data->radius;
        for (int16_t y = -r; y <= r; y++) {
            for (int16_t x = -r; x <= r; x++) {
                if (x*x + y*y <= r*r) {
                    int16_t px = new_x + x;
                    int16_t py = new_y + y;
                    if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                        switch (overlap) {
                            case OVERLAP_TOGGLE:
                                if (get_pixel(px, py)) toggle_pixel(px, py);
                                break;
                            case OVERLAP_KEEP_ON:
                                set_pixel(px, py, 1);
                                break;
                            case OVERLAP_KEEP_OFF:
                                set_pixel(px, py, 0);
                                break;
                            default: break;
                        }
                    }
                }
            }
        }
    }
    // ...其他类型可扩展...
}

/**
 * @brief 移动图形对象
 */
bool move_shape(GraphicObject* obj, const MoveOption* opt,PixelColor color) {
    if (!obj || !opt) return false;
    int16_t new_x = obj->position.x + opt->dx;
    int16_t new_y = obj->position.y + opt->dy;
    bool boundary_hit = check_boundary(obj, opt, new_x, new_y);
    bool collision = false;

    if (boundary_hit) {
        // 可根据需求反弹、停止或其它处理
        return true;
    }

    if (opt->collision == COLLISION_PIXEL) {
        collision = check_pixel_collision(obj, new_x, new_y);
        if (collision) {
            if (opt->overlap != OVERLAP_NONE) {
                handle_overlap(obj, new_x, new_y, opt->overlap);
            }
            return true;
        }
    }
    // 擦除原位置
    obj->drawFunc(obj, 0);
    // 更新位置
    obj->position.x = new_x;
    obj->position.y = new_y;
    // 绘制新位置
    if (obj->visible) {
        obj->drawFunc(obj, color);
    }
    return false;
}

/**
 * @brief 旋转图形对象
 */
void rotate_shape(GraphicObject* obj, int16_t degrees) {
    obj->drawFunc(obj, 0);
    obj->rotation = (obj->rotation + degrees) % 360;
    if (obj->visible) {
        obj->drawFunc(obj, 1);
    }
}

// ================== 图形对象绘制函数 ==================

/**
 * @brief 绘制圆形对象
 */
void draw_circle_object(GraphicObject* obj, PixelColor state) {
    if (!obj->visible) return;
    CircleData* data = (CircleData*)obj->shapeData;
    draw_circle(obj->position, data->radius, state, 1);
}

/**
 * @brief 绘制三角形对象
 */
void draw_triangle_object(GraphicObject* obj, PixelColor state) {
    if (!obj->visible) return;
    TriangleData* data = (TriangleData*)obj->shapeData;
    float rad = obj->rotation * M_PI / 180.0;
    float cosA = cosf(rad);
    float sinA = sinf(rad);
    Point vertices[3];
    for (int i = 0; i < 3; i++) {
        int16_t dx = data->vertices[i].x;
        int16_t dy = data->vertices[i].y;
        vertices[i].x = obj->position.x + (int16_t)(dx * cosA - dy * sinA);
        vertices[i].y = obj->position.y + (int16_t)(dx * sinA + dy * cosA);
    }
    draw_line(vertices[0], vertices[1], state);
    draw_line(vertices[1], vertices[2], state);
    draw_line(vertices[2], vertices[0], state);
}

// ================== 帧更新函数 ==================

/**
 * @brief 刷新一帧，绘制所有可见对象
 */
void update_frame() {
    memset(FrameBuffer, 0, sizeof(FrameBuffer));
    for (int i = 0; i < MAX_OBJECTS; i++) {
        if (objectPool[i] && objectPool[i]->visible) {
            objectPool[i]->drawFunc(objectPool[i], 1);
        }
    }
}

void Display_Test(){
    // 创建多个圆
    CircleData circleData_1 = {.radius = 18};
    CircleData circleData_2 = {.radius = 15};
    CircleData circleData_3 = {.radius = 20};
    CircleData circleData_4 = {.radius = 27};

    GraphicObject* circle_1 = create_shape(SHAPE_CIRCLE, (Point){20, 20}, &circleData_1);
    GraphicObject* circle_2 = create_shape(SHAPE_CIRCLE, (Point){100, 20}, &circleData_2);
    GraphicObject* circle_3 = create_shape(SHAPE_CIRCLE, (Point){20, 40}, &circleData_3);
    GraphicObject* circle_4 = create_shape(SHAPE_CIRCLE, (Point){100, 40}, &circleData_4);


    // 创建不同的移动选项
    MoveOption moveRight = {
        .boundary = BOUNDARY_LEFT_RIGHT,
        .collision = COLLISION_BOUNDINGBOX,
        .overlap = OVERLAP_TOGGLE,
        .dx = 2,
        .dy = 0
    };

    MoveOption moveDown = {
        .boundary = BOUNDARY_TOP_BOTTOM,
        .collision = COLLISION_BOUNDINGBOX,
        .overlap = OVERLAP_TOGGLE,
        .dx = 0,
        .dy = 1
    };

    MoveOption moveDiagonal1 = {
        .boundary = BOUNDARY_ALL,
        .collision = COLLISION_NONE,
        .overlap = OVERLAP_NONE,
        .dx = 1,
        .dy = 1
    };

    MoveOption moveDiagonal2 = {
        .boundary = BOUNDARY_ALL,
        .collision = COLLISION_NONE,
        .overlap = OVERLAP_NONE,
        .dx = -1,
        .dy = 1
    };
    FPSCounter fps;
    // 主循环
    while (1) {
        // 移动所有圆形对象
        if (move_shape(circle_1, &moveRight,RGB565_WHITE)) {
            moveRight.dx = -moveRight.dx;
        }

        if (move_shape(circle_2, &moveDown,RGB565_RED)) {
            moveDown.dy = -moveDown.dy;
        }

        if (move_shape(circle_3, &moveDiagonal1,RGB565_GREEN)) {
            if (circle_3->position.x <= circleData_3.radius ||
                circle_3->position.x >= SCREEN_WIDTH - circleData_3.radius) {
                moveDiagonal1.dx = -moveDiagonal1.dx;
            }
            if (circle_3->position.y <= circleData_3.radius ||
                circle_3->position.y >= SCREEN_HEIGHT - circleData_3.radius) {
                moveDiagonal1.dy = -moveDiagonal1.dy;
            }
        }

        if (move_shape(circle_4, &moveDiagonal2,RGB565_BLUE)) {
            if (circle_4->position.x <= circleData_4.radius ||
                circle_4->position.x >= SCREEN_WIDTH - circleData_4.radius) {
                moveDiagonal2.dx = -moveDiagonal2.dx;
            }
            if (circle_4->position.y <= circleData_4.radius ||
                circle_4->position.y >= SCREEN_HEIGHT - circleData_4.radius) {
                moveDiagonal2.dy = -moveDiagonal2.dy;
            }
        }
        // 渲染帧
        update_frame();
        update_fps(&fps);
        // draw_string(240,0, "FPS: %d", FONT_STYLE_MEDIUM,RGB565_BLUE);
        // HAL_Delay(15); // 约60FPS
    }
}

// ================== 帧率相关实现 ==================

/**
 * @brief 获取系统时钟（需平台实现）
 */
extern uint32_t get_tick_ms(void);

/**
 * @brief 初始化帧率计数器
 */
void init_fps_counter(FPSCounter* counter, uint32_t updateInterval) {
    counter->lastTick = get_tick_ms();
    counter->frameCount = 0;
    counter->updateInterval = updateInterval;
    counter->currentFPS = 0.0f;
}

/**
 * @brief 更新帧率计算
 */
float update_fps(FPSCounter* counter) {
    counter->frameCount++;
    uint32_t currentTick = get_tick_ms();
    uint32_t elapsedTime = currentTick - counter->lastTick;
    if (elapsedTime >= counter->updateInterval) {
        counter->currentFPS = (float)counter->frameCount * 1000.0f / elapsedTime;
        counter->frameCount = 0;
        counter->lastTick = currentTick;
    }

    return counter->currentFPS;
}
