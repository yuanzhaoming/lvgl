/**
 * @file lv_lua.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_lua.h"
#if LV_BUILD_LUA
#include "../lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_lua_obj_register(void);
static void lv_lua_style_register(void);
static void lv_lua_btn_register(void);
static void lv_lua_ddlist_register(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lua_State *L;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/



void lv_lua_set_context(lua_State *lv_lua_context)
{
    L = lv_lua_context;
}

lua_State * lv_lua_get_context(void)
{
    return L;
}

void lv_lua_register(void)
{
    lv_lua_obj_register();
    lv_lua_style_register();
    lv_lua_btn_register();
    lv_lua_ddlist_register();

}

void lv_lua_register_function(void * c_func, const char * lua_name)
{
    lua_pushcfunction(L, c_func);
    lua_setglobal(L, lua_name);
}

void lv_lua_register_const(int num, const char * lua_name)
{
    lua_pushinteger(L, num);
    lua_setglobal(L, lua_name);
}

void lv_lua_register_pointer(void * p, const char * lua_name)
{
    lua_pushlightuserdata(L, p);
    lua_setglobal(L, lua_name);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool lv_lua_get_table_int(lua_State * L, int table_stack_index, const char * table_field, int *res_p)
{
    lua_getfield(L, table_stack_index, table_field);        /*stack : -1*/
    if(lua_isnumber(L, -1)) {
        (*res_p) = lua_tointeger(L, -1);
        return true;        /*Integer found*/
    } else {
        (*res_p) = 0;
        return false;       /*No integer with this name*/
    }
}

static bool lv_lua_get_table_bool(lua_State * L, int table_stack_index, const char * table_field, bool *res_p)
{
    lua_getfield(L, table_stack_index, table_field);        /*stack : -1*/
    if(lua_isboolean(L, -1)) {
        (*res_p) = lua_toboolean(L, -1);
        return true;        /*Boolean found*/
    } else {
        (*res_p) = false;
        return false;       /*No boolean with this name*/
    }
}

static bool lv_lua_get_table_ptr(lua_State * L, int table_stack_index, const char * table_field, void **res_p)
{
    lua_getfield(L, table_stack_index, table_field);        /*stack : -1*/
    if(lua_islightuserdata(L, -1)) {
        (*res_p) = (void*)lua_topointer(L, -1);
        return true;        /*Pointer found*/
    } else {
        (*res_p) = NULL;
        return false;       /*No pointer with this name*/
    }
}

static bool lv_lua_get_table_str(lua_State * L, int table_stack_index, const char * table_field, const char **res_p)
{
    lua_getfield(L, table_stack_index, table_field);        /*stack : -1*/
    if(lua_isstring(L, -1)) {
        (*res_p) = lua_tostring(L, -1);
        return true;        /*String found*/
    } else {
        (*res_p) = "";
        return false;       /*No string with this name*/
    }
}
/*----------------------------
 * lv_obj functions for Lua
 *---------------------------*/

static int lua_lv_scr_act (lua_State *L) {
    lua_pushlightuserdata(L, lv_scr_act());  /* push result */
    return 1;  /* number of results */
}

static int lua_lv_obj_set(lua_State *L)
{
    luaL_checktype(L, 2, LUA_TTABLE);

    lv_obj_t * obj = (lv_obj_t *)lua_topointer(L, 1);
    bool now_created = false;
    /*Create an object if 'nil' passed*/
    if(lua_isnil(L, 1)) {
        lv_obj_t *parent;
        lv_obj_t *copy;
        lv_lua_get_table_ptr(L, 2, "parent", (void**)&parent);
        lv_lua_get_table_ptr(L, 2, "copy", (void**)&copy);
        obj = lv_obj_create(parent, copy);
        now_created = true;
    }

    int num_res;
    bool bool_res;
    void * ptr_res;

    if(lv_lua_get_table_int(L, 2, "width", &num_res))        lv_obj_set_width(obj, num_res);
    if(lv_lua_get_table_int(L, 2, "height", &num_res))       lv_obj_set_height(obj, num_res);
    if(lv_lua_get_table_int(L, 2, "x", &num_res))            lv_obj_set_x(obj, num_res);
    if(lv_lua_get_table_int(L, 2, "y", &num_res))            lv_obj_set_y(obj, num_res);
#ifdef LV_OBJ_FREE_NUM_TYPE
    if(lv_lua_get_table_int(L, 2, "free_num", &num_res))     lv_obj_set_free_num(obj, num_res);
#endif
    if(lv_lua_get_table_bool(L, 2, "hidden", &bool_res))     lv_obj_set_hidden(obj, bool_res);
    if(lv_lua_get_table_bool(L, 2, "click", &bool_res))      lv_obj_set_click(obj, bool_res);
    if(lv_lua_get_table_bool(L, 2, "top", &bool_res))        lv_obj_set_top(obj, bool_res);
    if(lv_lua_get_table_bool(L, 2, "drag", &bool_res))       lv_obj_set_drag(obj, bool_res);
    if(lv_lua_get_table_bool(L, 2, "drag_throw", &bool_res)) lv_obj_set_drag_throw(obj, bool_res);
    if(lv_lua_get_table_bool(L, 2, "drag_parent", &bool_res))          lv_obj_set_drag_parent(obj, bool_res);
    if(lv_lua_get_table_ptr(L, 2, "parent", &ptr_res) && now_created)  lv_obj_set_parent(obj, ptr_res); /*Don't set the parent if just created on it*/
    if(lv_lua_get_table_ptr(L, 2, "style", &ptr_res))           lv_obj_set_style(obj, ptr_res);
#if LV_OBJ_FREE_PTR
    if(lv_lua_get_table_ptr(L, 2, "free_ptr", &ptr_res))    lv_obj_set_free_ptr(obj, ptr_res);
#endif

    lua_pushlightuserdata(L, obj);     /*Always return with the object itself (maybe now created)*/

    return 1;
}


static bool lua_lv_obj_get_core(lv_obj_t *obj, const char * par)
{
    if(!strcmp(par, "width"))           lua_pushinteger(L, lv_obj_get_width(obj));
    else if(!strcmp(par, "height"))     lua_pushinteger(L, lv_obj_get_height(obj));
    else if(!strcmp(par, "x"))          lua_pushinteger(L, lv_obj_get_x(obj));
    else if(!strcmp(par, "y"))          lua_pushinteger(L, lv_obj_get_y(obj));
#ifdef LV_OBJ_FREE_NUM_TYPE
    else if(!strcmp(par, "free_num"))   lua_pushinteger(L, lv_obj_get_free_num(obj));
#endif
    else if(!strcmp(par, "hidden"))     lua_pushboolean(L, lv_obj_get_hidden(obj));
    else if(!strcmp(par, "click"))      lua_pushboolean(L, lv_obj_get_click(obj));
    else if(!strcmp(par, "drag"))       lua_pushboolean(L, lv_obj_get_drag(obj));
    else if(!strcmp(par, "drag_throw")) lua_pushboolean(L, lv_obj_get_drag_throw(obj));
    else if(!strcmp(par, "drag_parent"))lua_pushboolean(L, lv_obj_get_drag_parent(obj));
#if LV_OBJ_FREE_PTR
    else if(!strcmp(par, "free_ptr"))   lua_pushlightuserdata(L, lv_obj_get_free_ptr(obj));
#endif
    else if(!strcmp(par, "style"))      lua_pushlightuserdata(L, lv_obj_get_style(obj));
    else return false;      /*Not matching parameter*/

    return true;            /*Match found*/
}


static int lua_lv_obj_get(lua_State *L)
{
    lv_obj_t * obj = (lv_obj_t *)lua_topointer(L, 1);

    luaL_checktype(L, 2, LUA_TTABLE);

    int par_id =  1;

    while(1) {
        lua_rawgeti(L, 2, par_id);   /*-1*/
        if(lua_isnil(L, -1)) {
            lua_pop(L, 1);     /*Pop the pushed 'nil' because it is not a return value*/
            break;
        }

        const char * par = lua_tostring(L, -1);

        /*Check all the basic attribute*/
        if(lua_lv_obj_get_core(obj, par) == false) {
            lua_pushnil(L);    /*No corresponding get -> use 'nil'*/
        }

        /* stack: [-1] ret_val; [-2]: par_name
         * par_name is not required -> replace it with ret_val*/
        lua_replace(L, -2);

        par_id++;
    }

    return par_id - 1;  /*-1 because started from 1 instead of 0*/
}




static void lv_lua_obj_register(void)
{
    /***Register functions***/

    /*Setter function*/
    lv_lua_register_function(lua_lv_obj_set,        "lv_obj_set");
    lv_lua_register_function(lua_lv_obj_get,        "lv_obj_get");

    lv_lua_register_function(lua_lv_scr_act,        "lv_scr_act");


    /***Register constants***/

    /*Protect*/
    lv_lua_register_const(LV_PROTECT_NONE,      "LV_PROTECT_NONE");
    lv_lua_register_const(LV_PROTECT_CHILD_CHG, "LV_PROTECT_CHILD_CHG");
    lv_lua_register_const(LV_PROTECT_PARENT,    "LV_PROTECT_PARENT");
    lv_lua_register_const(LV_PROTECT_POS,       "LV_PROTECT_POS");
    lv_lua_register_const(LV_PROTECT_FOLLOW,    "LV_PROTECT_FOLLOW");

    /*Align*/
    lv_lua_register_const(LV_ALIGN_CENTER,          "LV_ALIGN_CENTER");
    lv_lua_register_const(LV_ALIGN_IN_TOP_LEFT,     "LV_ALIGN_IN_TOP_LEFT");
    lv_lua_register_const(LV_ALIGN_IN_TOP_MID,      "LV_ALIGN_IN_TOP_MID");
    lv_lua_register_const(LV_ALIGN_IN_TOP_RIGHT,    "LV_ALIGN_IN_TOP_RIGHT");
    lv_lua_register_const(LV_ALIGN_IN_BOTTOM_LEFT,  "LV_ALIGN_IN_BOTTOM_LEFT");
    lv_lua_register_const(LV_ALIGN_IN_BOTTOM_MID,   "LV_ALIGN_IN_BOTTOM_MID");
    lv_lua_register_const(LV_ALIGN_IN_BOTTOM_RIGHT, "LV_ALIGN_IN_BOTTOM_RIGHT");
    lv_lua_register_const(LV_ALIGN_IN_LEFT_MID,     "LV_ALIGN_IN_LEFT_MID");
    lv_lua_register_const(LV_ALIGN_IN_RIGHT_MID,    "LV_ALIGN_IN_RIGHT_MID");
    lv_lua_register_const(LV_ALIGN_OUT_TOP_LEFT,    "LV_ALIGN_OUT_TOP_LEFT");
    lv_lua_register_const(LV_ALIGN_OUT_TOP_MID,     "LV_ALIGN_OUT_TOP_MID");
    lv_lua_register_const(LV_ALIGN_OUT_BOTTOM_LEFT, "LV_ALIGN_OUT_BOTTOM_LEFT");
    lv_lua_register_const(LV_ALIGN_OUT_BOTTOM_MID,  "LV_ALIGN_OUT_BOTTOM_MID");
    lv_lua_register_const(LV_ALIGN_OUT_BOTTOM_RIGHT,"LV_ALIGN_OUT_BOTTOM_RIGHT");
    lv_lua_register_const(LV_ALIGN_OUT_LEFT_TOP,    "LV_ALIGN_OUT_LEFT_TOP");
    lv_lua_register_const(LV_ALIGN_OUT_LEFT_MID,    "LV_ALIGN_OUT_LEFT_MID");
    lv_lua_register_const(LV_ALIGN_OUT_LEFT_BOTTOM, "LV_ALIGN_OUT_LEFT_BOTTOM");
    lv_lua_register_const(LV_ALIGN_OUT_RIGHT_TOP,   "LV_ALIGN_OUT_RIGHT_TOP");
    lv_lua_register_const(LV_ALIGN_OUT_RIGHT_MID,   "LV_ALIGN_OUT_RIGHT_MID");
    lv_lua_register_const(LV_ALIGN_OUT_RIGHT_BOTTOM,"LV_ALIGN_OUT_RIGHT_BOTTOM");

    /*Animations*/
    lv_lua_register_const(LV_ANIM_NONE,          "LV_ANIM_NONE");
    lv_lua_register_const(LV_ANIM_FLOAT_TOP,     "LV_ANIM_FLOAT_TOP");
    lv_lua_register_const(LV_ANIM_FLOAT_LEFT,    "LV_ANIM_FLOAT_LEFT");
    lv_lua_register_const(LV_ANIM_FLOAT_BOTTOM,  "LV_ANIM_FLOAT_BOTTOM");
    lv_lua_register_const(LV_ANIM_FLOAT_RIGHT,   "LV_ANIM_FLOAT_RIGHT");
    lv_lua_register_const(LV_ANIM_GROW_H,        "LV_ANIM_GROW_H");
    lv_lua_register_const(LV_ANIM_GROW_V,        "LV_ANIM_GROW_V");

}

/*----------------------------
 * lv_style functions for Lua
 *---------------------------*/

static int lua_lv_style_alloc(lua_State *L)
{
    lv_style_t * copy = (lv_style_t *)lua_topointer(L, 1);
    lv_style_t * new_style = lv_style_alloc(copy);
    lua_pushlightuserdata(L, new_style);
    return 1;  /* number of results */
}


static int lua_lv_style_copy(lua_State *L)
{

    lv_style_t * dest = (lv_style_t *)lua_topointer(L, 1);
    lv_style_t * src = (lv_style_t *)lua_topointer(L, 2);
    lv_style_copy(dest, src);
    return 0;  /* number of results */
}

#if USE_LV_ANIMATION

static int lua_lv_style_anim_create(lua_State *L)
{
    lv_style_anim_t * anim = (lv_style_anim_t *)lua_topointer(L, 1);
    lv_style_anim_create(anim);
    return 0;  /* number of results */
}

#endif

static int lua_lv_style_set_body_border_width(lua_State *L)
{
    lv_style_t * style = (lv_style_t *)lua_topointer(L, 1);
    lv_coord_t w = lua_tointeger(L, 2);
    lv_style_set_body_border_width(style, w);
    return 0;  /* number of results */
}

static void lv_lua_style_register(void)
{
    /***Register functions***/

    lv_lua_register_function(lua_lv_style_alloc,        "lv_style_alloc");
    lv_lua_register_function(lua_lv_style_copy,         "lv_style_copy");
#if USE_LV_ANIMATION
    lv_lua_register_function(lua_lv_style_anim_create,  "lv_style_anim_create");
#endif
    /*Property set*/
    lv_lua_register_function(lua_lv_style_set_body_border_width,    "lv_style_set_body_border_width");

    /***Register pointers***/
    lv_lua_register_pointer(&lv_style_scr,          "lv_style_scr");
    lv_lua_register_pointer(&lv_style_transp,       "lv_style_transp");
    lv_lua_register_pointer(&lv_style_transp_fit,   "lv_style_transp_fit");
    lv_lua_register_pointer(&lv_style_transp_tight, "lv_style_transp_tight");
    lv_lua_register_pointer(&lv_style_plain,        "lv_style_plain");
    lv_lua_register_pointer(&lv_style_plain_color,  "lv_style_plain_color");
    lv_lua_register_pointer(&lv_style_pretty,       "lv_style_pretty");
    lv_lua_register_pointer(&lv_style_pretty_color, "lv_style_pretty_color");
    lv_lua_register_pointer(&lv_style_btn_rel,      "lv_style_btn_rel");
    lv_lua_register_pointer(&lv_style_btn_pr,       "lv_style_btn_pr");
    lv_lua_register_pointer(&lv_style_btn_tgl_rel,  "lv_style_btn_tgl_rel");
    lv_lua_register_pointer(&lv_style_btn_tgl_pr,   "lv_style_btn_tgl_pr");
    lv_lua_register_pointer(&lv_style_btn_ina,      "lv_style_btn_ina");
}


/*----------------------------
 * lv_btn functions for Lua
 *---------------------------*/
#if USE_LV_BTN

int lua_lv_btn_set(lua_State *L)
{
    luaL_checktype(L, 2, LUA_TTABLE);

    lv_obj_t * btn = (lv_obj_t *)lua_topointer(L, 1);
    /*Create an object if 'nil' passed*/
    if(lua_isnil(L, 1)) {
        lv_obj_t *parent;
        lv_obj_t *copy;
        lv_lua_get_table_ptr(L, 2, "parent", (void**)&parent);
        lv_lua_get_table_ptr(L, 2, "copy", (void**)&copy);
        btn = lv_btn_create(parent, copy);

        /*Update the value in the stack too*/
        lua_pushlightuserdata(L, btn);
        lua_replace(L, 1);
    }

    /*Firstly do all base obj. related settings*/
    lua_lv_obj_set(L);

    int num_res;
    bool bool_res;
    void * ptr_res;
    const char * str_res;

    if(lv_lua_get_table_int(L, 2, "toggle", &num_res))          lv_btn_set_toggle(btn, num_res);
    if(lv_lua_get_table_int(L, 2, "state", &num_res))           lv_btn_set_state(btn, num_res);
    if(lv_lua_get_table_str(L, 2, "action_pr", &str_res))       lv_btn_set_lua_action(btn, LV_BTN_ACTION_PR, str_res);
    if(lv_lua_get_table_str(L, 2, "action_click", &str_res))    lv_btn_set_lua_action(btn, LV_BTN_ACTION_CLICK, str_res);
    if(lv_lua_get_table_str(L, 2, "action_long_pr", &str_res))  lv_btn_set_lua_action(btn, LV_BTN_ACTION_LONG_PR, str_res);
    if(lv_lua_get_table_str(L, 2, "action_long_pr_repeate", &str_res))  lv_btn_set_lua_action(btn, LV_BTN_ACTION_LONG_PR_REPEAT, str_res);
    if(lv_lua_get_table_int(L, 2, "layout", &num_res))          lv_btn_set_layout(btn, num_res);
    if(lv_lua_get_table_bool(L, 2, "hor_fit", &bool_res))       lv_btn_set_fit(btn, bool_res, lv_btn_get_ver_fit(btn));
    if(lv_lua_get_table_bool(L, 2, "ver_fit", &bool_res))       lv_btn_set_fit(btn, lv_btn_get_hor_fit(btn), bool_res);
    if(lv_lua_get_table_ptr(L, 2, "style_rel", &ptr_res))       lv_btn_set_style(btn, LV_BTN_STYLE_REL, ptr_res);
    if(lv_lua_get_table_ptr(L, 2, "style_pr", &ptr_res))        lv_btn_set_style(btn, LV_BTN_STYLE_REL, ptr_res);
    if(lv_lua_get_table_ptr(L, 2, "style_tgl_rel", &ptr_res))   lv_btn_set_style(btn, LV_BTN_STYLE_REL, ptr_res);
    if(lv_lua_get_table_ptr(L, 2, "style_tgl_pr", &ptr_res))    lv_btn_set_style(btn, LV_BTN_STYLE_REL, ptr_res);
    if(lv_lua_get_table_ptr(L, 2, "style_ina", &ptr_res))       lv_btn_set_style(btn, LV_BTN_STYLE_REL, ptr_res);


    lua_pushlightuserdata(L, btn);     /*Always return with the object itself (maybe now created)*/

    return 1;
}

static int lua_lv_btn_get(lua_State *L)
{
    lv_obj_t * btn = (lv_obj_t *)lua_topointer(L, 1);

    luaL_checktype(L, 2, LUA_TTABLE);

    int par_id =  1;

    while(1) {
        lua_rawgeti(L, 2, par_id);   /*-1*/
        if(lua_isnil(L, -1)) {
            lua_pop(L, 1);     /*Pop the pushed 'nil' because it is not a return value*/
            break;
        }

        const char * par = lua_tostring(L, -1);

        if(!strcmp(par, "state"))           lua_pushinteger(L, lv_btn_get_state(btn));
        else if(!strcmp(par, "toggle"))     lua_pushinteger(L, lv_btn_get_toggle(btn));
        else if(!strcmp(par, "action_pr"))              lua_pushstring(L, lv_btn_get_lua_action(btn, LV_BTN_ACTION_PR));
        else if(!strcmp(par, "action_click"))           lua_pushstring(L, lv_btn_get_lua_action(btn, LV_BTN_ACTION_CLICK));
        else if(!strcmp(par, "action_long_pr"))         lua_pushstring(L, lv_btn_get_lua_action(btn, LV_BTN_ACTION_LONG_PR));
        else if(!strcmp(par, "action_long_pr_repeate")) lua_pushstring(L, lv_btn_get_lua_action(btn, LV_BTN_ACTION_LONG_PR_REPEAT));
        else if(!strcmp(par, "layout"))     lua_pushinteger(L, lv_btn_get_layout(btn));
        else if(!strcmp(par, "hor_fit"))    lua_pushboolean(L, lv_btn_get_hor_fit(btn));
        else if(!strcmp(par, "ver_fit"))    lua_pushboolean(L, lv_btn_get_hor_fit(btn));
        else if(!strcmp(par, "style_rel"))      lua_pushlightuserdata(L, lv_btn_get_style(btn, LV_BTN_STYLE_REL));
        else if(!strcmp(par, "style_pr"))       lua_pushlightuserdata(L, lv_btn_get_style(btn, LV_BTN_STYLE_PR));
        else if(!strcmp(par, "style_tgl_rel"))  lua_pushlightuserdata(L, lv_btn_get_style(btn, LV_BTN_STYLE_TGL_REL));
        else if(!strcmp(par, "style_tgl_pr"))   lua_pushlightuserdata(L, lv_btn_get_style(btn, LV_BTN_STYLE_TGL_PR));
        else if(!strcmp(par, "style_ina"))      lua_pushlightuserdata(L, lv_btn_get_style(btn, LV_BTN_STYLE_INA));
        else if(lua_lv_obj_get_core(btn, par) == false) {    /*Check the basic obj. attributes too*/
            lua_pushnil(L);    /*No corresponding get -> use 'nil'*/
        }

        /* stack: [-1] ret_val; [-2]: par_name
         * par_name is not required -> replace it with ret_val*/
        lua_replace(L, -2);

        par_id++;
    }

    return par_id - 1;  /*-1 because started from 1 instead of 0*/
}

static void lv_lua_btn_register(void)
{
    /***Register functions***/
    lv_lua_register_function(lua_lv_btn_set,         "lv_btn_set");
    lv_lua_register_function(lua_lv_btn_get,         "lv_btn_get");

    /***Register constants***/

    /*States*/
    lv_lua_register_const(LV_BTN_STATE_REL, "LV_BTN_STATE_REL");
    lv_lua_register_const(LV_BTN_STATE_PR, "LV_BTN_STATE_PR");
    lv_lua_register_const(LV_BTN_STATE_TGL_REL, "LV_BTN_STATE_TGL_REL");
    lv_lua_register_const(LV_BTN_STATE_TGL_PR, "LV_BTN_STATE_TGL_PR");
    lv_lua_register_const(LV_BTN_STATE_INA, "LV_BTN_STATE_INA");
}

#endif




/*----------------------------
 * lv_obj functions for Lua
 *---------------------------*/
#if USE_LV_DDLIST

static int lua_lv_ddlist_create(lua_State *L)
{
    lv_obj_t * par = (lv_obj_t *)lua_topointer(L, 1);
    lv_obj_t * copy = (lv_obj_t *)lua_topointer(L, 2);
    lv_obj_t * new_obj = lv_ddlist_create(par, copy);
    lua_pushlightuserdata(L, new_obj);  /* push result */
    return 1;  /* number of results */
}

static int lua_lv_ddlist_set(lua_State *L)
{
    lv_obj_t * ddlist = (lv_obj_t *)lua_topointer(L, 1);

    luaL_checktype(L, 2, LUA_TTABLE);

    int num_res;
    bool bool_res;
    void * ptr_res;
    const char * str_res;

    if(lv_lua_get_table_str(L, 2, "options", &str_res))      lv_ddlist_set_options(ddlist, str_res);
    if(lv_lua_get_table_int(L, 2, "selected", &num_res))     lv_ddlist_set_selected(ddlist, num_res);
   // if(lv_lua_get_table_str(L, 2, "action", &num_res))       lv_ddlist_set_action(ddlist, str_res);       /*TODO*/
    if(lv_lua_get_table_int(L, 2, "fix_height", &num_res))   lv_ddlist_set_fix_height(ddlist, num_res);
    if(lv_lua_get_table_bool(L, 2, "hor_fit", &bool_res))    lv_ddlist_set_hor_fit(ddlist, bool_res);
    if(lv_lua_get_table_int(L, 2, "sb_mode", &num_res))      lv_ddlist_set_sb_mode(ddlist, num_res);
    if(lv_lua_get_table_int(L, 2, "anim_time", &num_res))    lv_ddlist_set_anim_time(ddlist, num_res);
    if(lv_lua_get_table_ptr(L, 2, "style_bg", &ptr_res))     lv_ddlist_set_style(ddlist, LV_DDLIST_STYLE_BG, ptr_res);
    if(lv_lua_get_table_ptr(L, 2, "style_sel", &ptr_res))    lv_ddlist_set_style(ddlist, LV_DDLIST_STYLE_SEL, ptr_res);
    if(lv_lua_get_table_ptr(L, 2, "style_sb", &ptr_res))     lv_ddlist_set_style(ddlist, LV_DDLIST_STYLE_SB, ptr_res);

    return 0;
}

static int lua_lv_ddlist_get(lua_State *L)
{

    return 0;
//    lv_obj_t * ddlist = (lv_obj_t *)lua_topointer(L, 1);
//
//    luaL_checktype(L, 2, LUA_TTABLE);
//
//    int par_id =  1;
//
//    while(1) {
//        lua_rawgeti(L, 2, par_id);   /*-1*/
//        if(lua_isnil(
//                L, -1)) {
//            lua_pop(L, 1);     /*Pop the pushed 'nil' because it is not a return value*/
//            break;
//        }
//
//        const char * par = lua_tostring(L, -1);
//
//        if(strcmp(par, "width") == 0)           lua_pushinteger(L, lv_obj_get_width(ddlist));
//        else if(strcmp(par, "height") == 0)     lua_pushinteger(L, lv_obj_get_height(ddlist));
//        else if(strcmp(par, "x") == 0)          lua_pushinteger(L, lv_obj_get_x(ddlist));
//        else if(strcmp(par, "y") == 0)          lua_pushinteger(L, lv_obj_get_y(ddlist));
//#ifdef LV_OBJ_FREE_NUM_TYPE
//        else if(strcmp(par, "free_num") == 0)   lua_pushinteger(L, lv_obj_get_free_num(ddlist));
//#endif
//        else if(strcmp(par, "hidden") == 0)     lua_pushboolean(L, lv_obj_get_hidden(ddlist));
//        else if(strcmp(par, "click") == 0)      lua_pushboolean(L, lv_obj_get_click(ddlist));
//        else if(strcmp(par, "drag") == 0)       lua_pushboolean(L, lv_obj_get_drag(ddlist));
//        else if(strcmp(par, "drag_throw") == 0) lua_pushboolean(L, lv_obj_get_drag_throw(ddlist));
//        else if(strcmp(par, "drag_parent") == 0)lua_pushboolean(L, lv_obj_get_drag_parent(ddlist));
//#if LV_OBJ_FREE_PTR
//        else if(strcmp(par, "free_ptr") == 0)   lua_pushlightuserdata(L, lv_obj_get_free_ptr(ddlist));
//#endif
//        else if(strcmp(par, "style") == 0)      lua_pushlightuserdata(L, lv_obj_get_style(ddlist));
//        else lua_pushnil(L);    /*No corresponding get -> use 'nil'*/
//
//        /* stack: [-1] ret_val; [-2]: par_name
//         * par_name is not required -> replace it with ret_val*/
//        lua_replace(L, -2);
//
//        par_id++;
//    }
//
//    return par_id - 1;  /*-1 because started from 1 instead of 0*/
}

static void lv_lua_ddlist_register(void)
{
    /***Register functions***/
    lv_lua_register_function(lua_lv_ddlist_create,  "lv_ddlist_create");
    lv_lua_register_function(lua_lv_ddlist_set,     "lv_ddlist_set");
    lv_lua_register_function(lua_lv_ddlist_get,     "lv_ddlist_get");


}

#endif  /*LV_USE_DDLIST*/




/*----------------------------
 * lv_btn functions for Lua
 *---------------------------*/
#if USE_LV_BTNM


#endif  /*LV_USE_BTNM*/





#endif
