@echo off
echo ================================
echo   构建并烧录STM32H7项目
echo ================================

echo.
echo 步骤1: 配置CMake...
cmake --preset=Debug
if %errorlevel% neq 0 (
    echo CMake配置失败！
    pause
    exit /b 1
)

echo.
echo 步骤2: 构建项目...
cmake --build build/Debug --config Debug -j 4
if %errorlevel% neq 0 (
    echo 构建失败！
    pause
    exit /b 1
)

echo.
echo 步骤3: 烧录固件...
openocd -f interface/cmsis-dap.cfg -f target/stm32h7x.cfg -c "program build/Debug/LCD_H7.elf verify reset exit"

if %errorlevel% equ 0 (
    echo.
    echo ================================
    echo   构建并烧录完成！
    echo ================================
) else (
    echo.
    echo ================================
    echo     烧录失败！
    echo ================================
)

echo.
pause
