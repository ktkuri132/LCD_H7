/**
 * @file mw_display.h
 * @brief OLED图形显示核心接口与对象管理
 */

#ifndef DISPLAY_ENGINE_H
#define DISPLAY_ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define ST7789

/**
 * @defgroup DISPLAY_COLOR_DEPTH 图像色深宏定义
 * @{
 */
#if defined(SSD1306)
    #define DISPLAY_COLOR_DEPTH_MONO   1
    #define DISPLAY_COLOR_DEPTH        DISPLAY_COLOR_DEPTH_MONO
#elif defined(ST7789)
    #define DISPLAY_COLOR_DEPTH_RGB565 2
    #define DISPLAY_COLOR_DEPTH        DISPLAY_COLOR_DEPTH_RGB565
#elif defined(ST7796)
    #define DISPLAY_COLOR_DEPTH_RGB888 3
    #define DISPLAY_COLOR_DEPTH        DISPLAY_COLOR_DEPTH_RGB888
#else
    #define DISPLAY_COLOR_DEPTH_MONO   1
    #define DISPLAY_COLOR_DEPTH        DISPLAY_COLOR_DEPTH_MONO
#endif
/** @} */

/**
 * @brief 显存缓冲区（按页组织/色深）
 */
#if DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_MONO
    #define SCREEN_HEIGHT 64
    #define SCREEN_WIDTH 128
    extern uint8_t FrameBuffer[SCREEN_HEIGHT/8][SCREEN_WIDTH];
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB565
    #define SCREEN_HEIGHT 240
    #define SCREEN_WIDTH 240
    extern uint16_t FrameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB888
    #define SCREEN_HEIGHT 320
    #define SCREEN_WIDTH 480
    extern uint32_t FrameBuffer[SCREEN_HEIGHT][SCREEN_WIDTH];
#endif

/**
 * @brief 单色像素定义
 */
typedef enum {
    PIXEL_OFF = 0,
    PIXEL_ON  = 1
} MonoPixel;

/**
 * @brief RGB565像素定义
 */
typedef uint16_t RGB565Pixel;

/**
 * @brief RGB888像素定义
 */
typedef uint32_t RGB888Pixel;

/**
 * @brief 通用像素类型
 */
#if DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_MONO
    typedef MonoPixel PixelColor;
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB565
    typedef RGB565Pixel PixelColor;
    // 将8位R、G、B值转换为RGB565格式的宏
    // R: 0-31 (5位), G: 0-63 (6位), B: 0-31 (5位)
    #define RGB_TO_565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

    // 或者如果你输入的是0-255范围的RGB值
    #define RGB888_TO_565(r, g, b) ((((r) >> 3) << 11) | (((g) >> 2) << 5) | ((b) >> 3))

    // 常用颜色预定义
    #define RGB565_WHITE    0xFFFF
    #define RGB565_BLACK    0x0000
    #define RGB565_RED      0xF800
    #define RGB565_GREEN    0x07E0
    #define RGB565_BLUE     0x001F
    #define RGB565_CYAN     0x07FF
    #define RGB565_MAGENTA  0xF81F
    #define RGB565_YELLOW   0xFFE0
    #define RGB565_GRAY     0x8410
    #define RGB565_ORANGE   0xFD20
#elif DISPLAY_COLOR_DEPTH == DISPLAY_COLOR_DEPTH_RGB888
    typedef RGB888Pixel PixelColor;
#endif

/**
 * @brief 基础点结构
 */
typedef struct {
    uint16_t x; ///< X坐标
    uint16_t y; ///< Y坐标
} Point;

/**
 * @brief 图形类型枚举
 */
typedef enum {
    SHAPE_CIRCLE,    ///< 圆形
    SHAPE_TRIANGLE,  ///< 三角形SHAPE_POINTER
    SHAPE_RECTANGLE, ///< 矩形
    SHAPE_STAR,      ///< 五角星
    SHAPE_POINTER,   ///< 指针
    SHAPE_CUSTOM     ///< 自定义图形
} ShapeType;

/**
 * @brief 图形对象结构体
 */
typedef struct GraphicObject {
    ShapeType type;   ///< 图形类型
    Point position;   ///< 对象位置（中心点或基准点）
    int16_t rotation; ///< 旋转角度（度数）
    uint8_t visible;  ///< 可见性
    void* shapeData;  ///< 图形特定数据
    void (*drawFunc)(struct GraphicObject*, PixelColor); ///< 绘制函数
} GraphicObject;

/**
 * @brief 圆形数据结构
 */
typedef struct {
    int16_t radius; ///< 半径
} CircleData;

/**
 * @brief 三角形数据结构
 */
typedef struct {
    Point vertices[3]; ///< 相对于中心点的偏移
} TriangleData;

/**
 * @brief 矩形数据结构
 */
typedef struct {
    int16_t width;  ///< 宽度
    int16_t height; ///< 高度
} RectangleData;

/**
 * @brief 帧率计算相关结构体
 */
typedef struct {
    uint32_t lastTick;       ///< 上次计算帧率的时间戳
    uint32_t frameCount;     ///< 累计帧数
    uint32_t updateInterval; ///< 刷新间隔(毫秒)
    float currentFPS;        ///< 当前帧率
} FPSCounter;

// ================== 帧率相关 ==================

/**
 * @brief 初始化帧率计数器
 * @param counter 帧率计数器指针
 * @param updateInterval 刷新间隔（毫秒）
 */
void init_fps_counter(FPSCounter* counter, uint32_t updateInterval);

/**
 * @brief 更新帧率计算
 * @param counter 帧率计数器指针
 * @return 当前帧率
 */
float update_fps(FPSCounter* counter);

/**
 * @brief 在指定位置绘制帧率
 * @param counter 帧率计数器指针
 * @param position 显示位置
 */
void draw_fps(FPSCounter* counter, Point position);

// ================== 图形对象管理 ==================

/**
 * @brief 初始化图形对象池
 */
void init_graphics();

/**
 * @brief 创建一个图形对象
 * @param type 图形类型
 * @param position 位置
 * @param shapeData 图形特定数据指针
 * @return 新建对象指针，失败返回NULL
 */
GraphicObject* create_shape(ShapeType type, Point position, void* shapeData);

/**
 * @brief 删除一个图形对象
 * @param obj 对象指针
 */
void delete_shape(GraphicObject* obj);

/**
 * @brief 旋转图形对象
 * @param obj 对象指针
 * @param degrees 旋转角度（度）
 */
void rotate_shape(GraphicObject* obj, int16_t degrees);

// ================== 基础像素操作 ==================

/**
 * @brief 设置像素点状态（支持多色深）
 * @param x X坐标
 * @param y Y坐标
 * @param color 像素颜色
 */
void set_pixel(uint16_t x, uint16_t y, PixelColor color);

/**
 * @brief 获取像素点状态（支持多色深）
 * @param x X坐标
 * @param y Y坐标
 * @return 当前像素颜色
 */
PixelColor get_pixel(uint16_t x, uint16_t y);

/**
 * @brief 翻转像素点状态（单色有效）
 * @param x X坐标
 * @param y Y坐标
 */
void toggle_pixel(uint16_t x, uint16_t y);

// ================== 基础图形绘制 ==================

/**
 * @brief 绘制直线
 * @param p1 起点
 * @param p2 终点
 * @param state 1:亮, 0:灭
 */
void draw_line(Point p1, Point p2, PixelColor state);

/**
 * @brief 绘制水平线
 * @param x0 起点X
 * @param x1 终点X
 * @param y Y坐标
 * @param state 1:亮, 0:灭
 */
void draw_horizontal_line(int16_t x0, int16_t x1, int16_t y, PixelColor state);

/**
 * @brief 绘制圆
 * @param center 圆心
 * @param radius 半径
 * @param state 1:亮, 0:灭
 * @param filled 是否填充
 */
void draw_circle(Point center, int16_t radius, PixelColor state, uint8_t filled);

// ================== 图形对象绘制函数 ==================

/**
 * @brief 绘制圆形对象
 * @param obj 图形对象指针
 * @param state 1:亮, 0:灭
 */
void draw_circle_object(GraphicObject* obj, PixelColor state);

/**
 * @brief 绘制三角形对象
 * @param obj 图形对象指针
 * @param state 1:亮, 0:灭
 */
void draw_triangle_object(GraphicObject* obj, PixelColor state);

/**
 * @brief 刷新一帧，绘制所有对象
 */
void update_frame();

/**
 * @brief 边界检测类型
 */
typedef enum {
    BOUNDARY_NONE         = 0x00, ///< 不检测
    BOUNDARY_TOP          = 0x01, ///< 检测上边界
    BOUNDARY_BOTTOM       = 0x02, ///< 检测下边界
    BOUNDARY_TOP_BOTTOM    = 0x01 | 0x02, ///< 检测上下边界
    BOUNDARY_LEFT         = 0x04, ///< 检测左边界
    BOUNDARY_RIGHT        = 0x08, ///< 检测右边界
    BOUNDARY_LEFT_RIGHT   = 0x04 | 0x08, ///< 检测左右边界
    BOUNDARY_ALL          = 0x0F  ///< 检测全部边界
} BoundaryCheckType;

/**
 * @brief 物理碰撞检测类型
 */
typedef enum {
    COLLISION_NONE,        ///< 不检测碰撞
    COLLISION_PIXEL,       ///< 像素级碰撞
    COLLISION_BOUNDINGBOX  ///< 边界盒碰撞
} CollisionType;

/**
 * @brief 像素重合处理方式
 */
typedef enum {
    OVERLAP_NONE,      ///< 不处理
    OVERLAP_TOGGLE,    ///< 重合像素翻转
    OVERLAP_KEEP_ON,   ///< 重合像素保持亮
    OVERLAP_KEEP_OFF   ///< 重合像素保持灭
} OverlapHandleType;

/**
 * @brief 移动参数结构体
 */
typedef struct {
    BoundaryCheckType boundary;   ///< 边界检测类型
    CollisionType collision;      ///< 物理碰撞检测类型
    OverlapHandleType overlap;    ///< 像素重合处理方式
    int16_t dx;                   ///< X方向偏移
    int16_t dy;                   ///< Y方向偏移
} MoveOption;

/**
 * @brief 移动图形对象（带高级选项）
 * @param obj 对象指针
 * @param opt 移动选项
 * @return 是否发生碰撞
 */
bool move_shape(GraphicObject* obj, const MoveOption* opt,PixelColor color);

void Display_Test();

#endif // DISPLAY_ENGINE_H

