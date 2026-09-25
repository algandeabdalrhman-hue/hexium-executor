#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CONSOLE_BUFFER_SIZE 8192

static lua_State *L = NULL;
static char console_buffer[CONSOLE_BUFFER_SIZE] = {0};
static size_t console_pos = 0;

static int lua_hexium_print(lua_State *L) {
    const char *msg = luaL_checkstring(L, 1);
    if (console_pos + strlen(msg) + 1 < CONSOLE_BUFFER_SIZE) {
        console_pos += snprintf(console_buffer + console_pos, CONSOLE_BUFFER_SIZE - console_pos, "%s\n", msg);
    }
    printf("[Hexium] %s\n", msg);
    return 0;
}

int hexium_execute(const char *script) {
    if (!L) return -1;
    int result = luaL_dostring(L, script);
    if (result != LUA_OK) {
        const char *err = lua_tostring(L, -1);
        if (console_pos + strlen(err) + 1 < CONSOLE_BUFFER_SIZE) {
            console_pos += snprintf(console_buffer + console_pos, CONSOLE_BUFFER_SIZE - console_pos, "Error: %s\n", err);
        }
        lua_pop(L, 1);
    }
    return result;
}

const char *hexium_get_console(void) {
    return console_buffer;
}

void hexium_clear_console(void) {
    memset(console_buffer, 0, CONSOLE_BUFFER_SIZE);
    console_pos = 0;
}

static void init_executor(void) __attribute__((constructor));
static void cleanup_executor(void) __attribute__((destructor));

static void init_executor(void) {
    L = luaL_newstate();
    if (L) {
        luaL_openlibs(L);
        lua_register(L, "hexium_print", lua_hexium_print);
    }
}

static void cleanup_executor(void) {
    if (L) {
        lua_close(L);
        L = NULL;
    }
}
