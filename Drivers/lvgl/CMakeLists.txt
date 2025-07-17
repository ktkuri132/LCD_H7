# 添加源文件
file(GLOB_RECURSE lvgl_Source
    src/*.c
    src/core/*.c
    src/display/*.c
    src/draw/*.c
    src/indev/*.c
    src/widgets/*.c
    src/font/*.c
    src/layouts/*.c
    src/misc/*.c
    src/osal/*.c
    src/stdlib/*.c
    src/tick/*.c
    src/others/*.c
    src/themes/*.c
    src/libs/*.c
    src/drivers/*.c
    examples/porting/*.c
)


add_library(lvgl ${lvgl_Source})

target_include_directories(lvgl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/examples/porting
)


