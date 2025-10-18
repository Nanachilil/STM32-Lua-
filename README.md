# 🧩 STM32 上的 Lua 虚拟机移植

本项目展示了如何在 **STM32 微控制器** 上移植并运行 **Lua 虚拟机（Lua VM）**，实现通过 Lua 脚本控制硬件外设。  
示例工程基于 **CMake + STM32CubeMX + FreeRTOS** 构建，并实现了 LED 灯的脚本化闪烁控制。
开发版：stm32f429i-disco1
（别买这个板子，现在居然还有usb mini-B接口的板子，柱状按键非常容易坏）

---

## ✨ 功能简介

- ✅ 在 STM32 上成功运行 Lua 解释器
- 🔌 提供 **C ↔ Lua 胶水层（hw_bindings.c）**
- 💡 使用 Lua 脚本直接控制 LED 开关、闪烁
- ⚙️ 使用 FreeRTOS 动态内存管理（`pvPortMalloc` / `vPortFree`）构建 Lua 堆
- 🧱 支持扩展更多硬件函数（GPIO、UART、SPI、I2C 等）

---

## 🗂️ 工程结构

```bash
STM32-Lua-VM/
├── Core/
│   ├── Src/
│   │   ├── main.c              # 主程序：初始化 Lua 并执行脚本
│   │   ├── lua_vm.c            # Lua 虚拟机创建与运行逻辑
│   │   ├── hw_bindings.c       # C ↔ Lua 硬件接口绑定（LED 控制）
│   └── Inc/
│       ├── lua_vm.h
│       ├── hw_bindings.h
├── Middlewares/
│   └── Lua/                    # Lua 官方源码
├── CMakeLists.txt              # 项目构建配置
└── STM32F429XX_FLASH.ld        # 链接脚本
```
