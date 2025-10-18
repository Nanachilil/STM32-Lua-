cmake_minimum_required(VERSION 3.22)

# =========================================================
# Project: STM32F429 + LVGL + Lua Integration
# =========================================================

# ---------------------------------------------------------
# 🧩 基础配置阶段
# ---------------------------------------------------------
# 作用：设置项目基础信息与 C 语言标准，告诉 CMake 我们要生成什么项目。

# 声明项目支持的语言（必须放在前面）
project(lvgl_board LANGUAGES C ASM)

# 设置 C 语言标准版本（C11），要求强制启用
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# 如果没有手动指定构建类型（Debug/Release），则默认 Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# 导出 compile_commands.json，供 clangd / VSCode LSP 使用
# 它记录每个源文件的真实编译命令，方便代码跳转、补全。
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 输出当前构建类型信息
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")


# ---------------------------------------------------------
# 🧩 可执行目标定义阶段
# ---------------------------------------------------------
# 作用：告诉 CMake，我们要生成一个可执行文件（而不是静态库/动态库）。
#       所有后续添加的源码、头文件、依赖都将附加到此目标上。

add_executable(${PROJECT_NAME})


# ---------------------------------------------------------
# 🧩 路径定义阶段
# ---------------------------------------------------------
# 作用：集中管理路径变量，保持工程结构清晰，方便引用。

set(CORE_DIR        ${CMAKE_SOURCE_DIR}/Core)                 # 主用户代码
set(MW_DIR          ${CMAKE_SOURCE_DIR}/Middlewares)          # 中间件
set(LUA_DIR         ${MW_DIR}/Lua)                            # Lua 源码目录
set(CUBEMX_DIR      ${CMAKE_SOURCE_DIR}/cmake/stm32cubemx)    # CubeMX 自动生成目录


# ---------------------------------------------------------
# 🧩 Lua 集成阶段
# ---------------------------------------------------------
# 作用：把 Lua 官方 C 源码（解释器部分）编译成静态库 liblua.a，
#       这样主工程链接时只需 target_link_libraries(lvgl_board lua)。

# file(GLOB ...)    — 搜索目录下所有 .c 文件（非递归）
file(GLOB LUA_SOURCES ${LUA_DIR}/*.c)

# add_library(...)  — 定义一个静态库目标（STATIC）
add_library(lua STATIC ${LUA_SOURCES})

# target_include_directories(...) — 为目标添加头文件搜索路径
# PUBLIC  表示此库的头文件也会传递给依赖它的目标（例如主工程）。
target_include_directories(lua PUBLIC ${LUA_DIR})


# ---------------------------------------------------------
# 🧩 用户自定义源文件阶段
# ---------------------------------------------------------
# 作用：添加你自己的胶水层（C 与 Lua 交互）文件，并指定头文件搜索路径。

target_sources(${PROJECT_NAME} PRIVATE
    ${CORE_DIR}/Src/lua_vm.c          # Lua 虚拟机初始化逻辑
    ${CORE_DIR}/Src/hw_bindings.c     # STM32 硬件函数注册
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CORE_DIR}/Inc                   # 头文件所在目录
    # ${LUA_DIR}                        # Lua 的头文件目录
)


# ---------------------------------------------------------
# 🧩 CubeMX 自动生成代码集成阶段
# ---------------------------------------------------------
# 作用：导入 STM32CubeMX 自动生成的驱动/FreeRTOS/USB 子工程。
# add_subdirectory(...) 会进入指定目录并读取其中的 CMakeLists.txt。
add_subdirectory(${CUBEMX_DIR})


# ---------------------------------------------------------
# 🧩 链接配置阶段
# ---------------------------------------------------------
# 作用：告诉 CMake 在链接阶段要合并哪些库，以及库的搜索路径。

# target_link_directories() — 设置链接器搜索路径（可省略）
target_link_directories(${PROJECT_NAME} PRIVATE
    # 示例：${CMAKE_SOURCE_DIR}/lib
)

# target_link_libraries() — 指定要链接的库顺序
# stm32cubemx：CubeMX 生成的底层驱动/FreeRTOS
# lua         ：我们上面编译的 Lua 静态库
# m           ：数学库，提供 floor/fmod/pow 等函数
target_link_libraries(${PROJECT_NAME}
    stm32cubemx
    lua
    m
)


# ---------------------------------------------------------
# 🧩 其他辅助设置阶段
# ---------------------------------------------------------
# 作用：删除无效库、定义宏、添加编译选项等。

# list(REMOVE_ITEM ...) — 从隐式系统库中移除无效项
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# target_compile_definitions() — 添加编译宏定义（可选）
# target_compile_definitions(${PROJECT_NAME} PRIVATE
#     USE_FREERTOS
#     ENABLE_LUA_DEBUG
# )

# target_compile_options() — 自定义编译选项（可选）
# target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)


# ---------------------------------------------------------
# 🧩 输出信息阶段
# ---------------------------------------------------------
# 作用：在 CMake 配置阶段输出项目结构信息，便于调试。
message(STATUS "✅ Project configured successfully.")
message(STATUS "   Core Path  : ${CORE_DIR}")
message(STATUS "   Lua Path   : ${LUA_DIR}")
message(STATUS "   Cube Path  : ${CUBEMX_DIR}")
