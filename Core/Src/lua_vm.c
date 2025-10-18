#include "lua_vm.h"

#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "hw_bindings.h"
#include "lauxlib.h"
#include "lualib.h"
#include "string.h"
#include "task.h"

/**
 * @brief Lua 内存分配限制（单位：字节）
 *        建议根据堆大小和任务数量调整。
 *        例如 FreeRTOS 堆为 64 KB，这里给 Lua 分配 32 KB。
 */
#define LUA_MAX_HEAP (32 * 1024)

/* 运行时统计信息 */
static size_t lua_heap_used = 0;
static size_t lua_heap_peak = 0;

// const char main_lua_script[] =
//     "print('Hello from embedded Lua!')\n"
//     "print(\"hello\" .. \"Nana\")\n";
//
/**
 * @brief  FreeRTOS 版 Lua 内存分配器
 * @param  ud     用户数据（Lua 内部传入，一般不用）
 * @param  ptr    原指针（Lua 需要扩容/释放的旧块）
 * @param  osize  旧块大小（可能为0）
 * @param  nsize  新块大小（为0表示free）
 * @retval 新的内存指针，或 NULL（失败）
 */
static void* lua_alloc_freertos(void* ud, void* ptr, size_t osize, size_t nsize) {
    (void)ud;

    /* 释放阶段 */
    if (nsize == 0) {
        if (ptr) {
            vPortFree(ptr);
            if (lua_heap_used >= osize) {
                lua_heap_used -= osize;
            } else {
                lua_heap_used = 0;
            }
        }
        return NULL;
    }

    /* 分配前检查上限 */
    if (lua_heap_used + nsize > LUA_MAX_HEAP) {
        printf("[Lua] ❌ 内存超出限制: used=%u / limit=%u bytes\r\n", (unsigned)lua_heap_used,
               (unsigned)LUA_MAX_HEAP);
        return NULL;
    }

    /* 分配新块 */
    void* new_ptr = pvPortMalloc(nsize);
    if (!new_ptr) {
        printf("[Lua] ❌ 分配失败: %u bytes\r\n", (unsigned)nsize);
        return NULL;
    }

    /* 如果是扩容（旧指针存在） */
    if (ptr) {
        memcpy(new_ptr, ptr, nsize < osize ? nsize : osize);
        vPortFree(ptr);
        if (lua_heap_used >= osize) {
            lua_heap_used -= osize;
        } else {
            lua_heap_used = 0;
        }
    }

    /* 更新统计 */
    lua_heap_used += nsize;
    if (lua_heap_used > lua_heap_peak) {
        lua_heap_peak = lua_heap_used;
    }

    return new_ptr;
}

/**
 *  * @brief 打印 Lua 当前堆使用情况
 *   */
void lua_print_heap_usage(void) {
    printf("[Lua Heap] used=%u bytes, peak=%u bytes, limit=%u bytes\r\n", (unsigned)lua_heap_used,
           (unsigned)lua_heap_peak, (unsigned)LUA_MAX_HEAP);
}

lua_State* lua_vm_init(void) {
    lua_State* L = lua_newstate(lua_alloc_freertos, NULL);
    if (!L) {
        printf("❌ Lua VM 初始化失败：内存不足！\r\n");
        return NULL;
    }
    luaL_openlibs(L);
    register_hw(L);
    return L;
}

void lua_vm_run(lua_State* L, const char* code) {
    if (luaL_dostring(L, code) != LUA_OK) {
        printf("Lua error: %s\r\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}
