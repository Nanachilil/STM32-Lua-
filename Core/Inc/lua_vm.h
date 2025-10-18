#ifndef LUA_VM_H
#define LUA_VM_H

#include "lua.h"

lua_State* lua_vm_init(void);
void lua_vm_run(lua_State* L, const char* code);

#endif
