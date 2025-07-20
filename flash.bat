@echo off
echo ================================
echo   STM32H7 快速烧录脚本
echo   使用OpenOCD + CMSIS-DAP
echo ================================

echo.
echo 正在检查构建文件...
if not exist "build\Debug\LCD_H7.elf" (
    echo 错误: 找不到构建文件 LCD_H7.elf
    echo 请先运行构建任务
    pause
    exit /b 1
)

echo 找到ELF文件: build\Debug\LCD_H7.elf
echo.

echo 正在连接目标设备...
openocd -f interface/cmsis-dap.cfg -f target/stm32h7x.cfg -c "program build/Debug/LCD_H7.elf verify reset exit"

if %errorlevel% equ 0 (
    echo.
    echo ================================
    echo     烧录完成！
    echo ================================
) else (
    echo.
    echo ================================
    echo     烧录失败！
    echo ================================
    echo 请检查:
    echo 1. CMSIS-DAP是否正确连接
    echo 2. 目标板是否正确供电
    echo 3. OpenOCD路径是否正确
)

echo.
pause
