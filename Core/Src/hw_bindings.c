#include "hw_bindings.h"

#include "main.h"

static int l_led_on(lua_State* L) {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
    return 0;
}

static int l_led_off(lua_State* L) {
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
    return 0;
}

static const luaL_Reg hwlib[] = {{"led_on", l_led_on}, {"led_off", l_led_off}, {NULL, NULL}};

void register_hw(lua_State* L) {
    luaL_newlib(L, hwlib);
    lua_setglobal(L, "hw");
}
