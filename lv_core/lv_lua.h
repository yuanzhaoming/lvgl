/**
 * @file lv_lua.h
 *
 */

#ifndef LV_LUA_H
#define LV_LUA_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if LV_BUILD_LUA

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_lua_set_context(lua_State *L);

lua_State * lv_lua_get_context(void);

void lv_lua_register(void);

void lv_lua_register_function(void * c_func, const char * lua_name);

void lv_lua_register_const(int num, const char * lua_name);

void lv_lua_register_pointer(void * p, const char * lua_name);

/**********************
 *      MACROS
 **********************/
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_LUA_H*/
