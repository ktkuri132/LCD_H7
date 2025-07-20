# STM32H7 + OpenOCD + CMSIS-DAP 烧录配置

此项目配置了使用OpenOCD和CMSIS-DAP调试器来烧录STM32H723xx微控制器的VS Code开发环境。

## 前置要求

### 1. 安装必需的工具

- **ARM GCC工具链**: arm-none-eabi-gcc
- **OpenOCD**: 开源调试器
- **Ninja**: 构建系统（可选，推荐）
- **CMake**: 项目构建工具

### 2. 设置环境变量

在Windows系统中设置以下环境变量：

```
ARM_TOOLCHAIN_PATH=C:\Program Files (x86)\GNU Arm Embedded Toolchain\13.2 Rel1
OPENOCD_PATH=C:\Program Files\OpenOCD
CMAKE_PATH=C:\Program Files\CMake
NINJA_PATH=C:\ninja
```

### 3. 安装VS Code扩展

推荐安装以下扩展：
- C/C++ (Microsoft)
- CMake Tools (Microsoft)  
- Cortex-Debug (marus25)

## 硬件连接

### CMSIS-DAP连接
确保以下引脚正确连接：
- **SWDIO** (调试器) ↔ **PA13** (STM32H7)
- **SWCLK** (调试器) ↔ **PA14** (STM32H7)
- **GND** (调试器) ↔ **GND** (STM32H7)
- **VCC** (调试器) ↔ **3.3V** (STM32H7)

### 电源
- 确保目标板有独立的电源供应
- 检查电源指示灯是否正常

## 使用方法

### 1. 构建项目
按 `Ctrl+Shift+P`，运行任务：
- **CMake: Configure Debug** - 配置CMake
- **CMake: Build Debug** - 构建项目

或者直接运行：
- **Build and Flash** - 构建并烧录

### 2. 烧录固件
运行以下任务之一：
- **OpenOCD: Flash STM32H7** - 烧录编译好的固件
- **Build and Flash** - 构建并烧录（推荐）

### 3. 调试
- 按 `F5` 开始调试
- 选择 "Debug STM32H7 (OpenOCD)" 配置

### 4. 其他操作
- **OpenOCD: Erase Flash** - 擦除Flash
- **OpenOCD: Reset Target** - 复位目标
- **OpenOCD: Start Server** - 启动OpenOCD服务器

## 任务说明

### 构建任务
- `CMake: Configure Debug`: 配置Debug构建
- `CMake: Build Debug`: 执行Debug构建
- `Build and Flash`: 构建并自动烧录

### OpenOCD任务  
- `OpenOCD: Flash STM32H7`: 烧录.elf文件到芯片
- `OpenOCD: Erase Flash`: 完全擦除Flash内存
- `OpenOCD: Reset Target`: 软复位目标芯片
- `OpenOCD: Start Server`: 启动OpenOCD调试服务器

### 调试配置
- `Debug STM32H7 (OpenOCD)`: 启动调试会话
- `Attach to STM32H7 (OpenOCD)`: 附加到运行中的目标

## 故障排除

### 1. 连接问题
```
Error: unable to open CMSIS-DAP device
```
- 检查CMSIS-DAP驱动是否正确安装
- 确保调试器已连接并被系统识别
- 尝试更换USB端口

### 2. 目标检测失败
```
Error: Could not find a device with a CMSIS-DAP interface
```
- 检查硬件连接
- 确保目标板正确供电
- 验证SWDIO/SWCLK连接

### 3. Flash编程失败
```
Error: Failed to program flash
```
- 检查Flash是否被保护
- 尝试先擦除Flash
- 确认.elf文件路径正确

### 4. 环境变量问题
如果工具路径不正确，请检查：
- 环境变量是否正确设置
- 工具是否正确安装
- VS Code是否已重启

## 配置文件说明

- `.vscode/tasks.json`: 定义构建和烧录任务
- `.vscode/launch.json`: 调试配置
- `.vscode/settings.json`: 项目设置和工具路径
- `openocd.cfg`: OpenOCD配置文件

## 支持的操作

✅ 编译项目  
✅ 烧录固件  
✅ 调试代码  
✅ 擦除Flash  
✅ 复位目标  
✅ 实时变量监控  
✅ 断点调试  
✅ 半主机模式(printf重定向)
