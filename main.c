#include <stdio.h>
#include <stdlib.h>
#include "lua.h"                                /* Always include this when calling Lua */
#include "lauxlib.h"                            /* Always include this when calling Lua */
#include "lualib.h"                            /* Always include this when calling Lua */

#include "lpeg/lptree.h"
#include "plreader.h"


int luaopen_cjson(lua_State *l);

void bail(lua_State *L, char *msg){
    fprintf(stderr, "\nFATAL ERROR:\n  %s: %s\n\n",
        msg, lua_tostring(L, -1));
    exit(1);
	
}

int safe_lua_pcall(lua_State* L,int nargs, int nresults) {

	int error = 0;
    int traceback = 0;
    lua_getglobal(L, "__G__TRACKBACK__");                     /* stack: ... func arg1 arg2 ... G */
    if (!lua_isfunction(L, -1)){
        lua_pop(L, 1);                                        /* stack: ... func arg1 arg2 ... */
    }else{
        traceback = -(nargs + 2);
        lua_insert(L, traceback);                             /* stack: ... G func arg1 arg2 ... */
    }

    
    error = lua_pcall(L, nargs, nresults, traceback);              /* stack: ... ret */
    if (error){
        if (traceback == 0){
            printf("[LUA ERROR] %s", lua_tostring(L, - 1));    /* stack: ... error */
            lua_pop(L, 1); // remove error message from stack
        }
        return 1;
    }
    return 0;
}


char argv1[255];

int main(int argc, char *argv[])
{
    char plfile[255];

    if (argc > 1){
        
        strcpy(plfile, argv[1]);
        if (argc > 2){
            strcpy(argv1, argv[2]);
        } else {
            argv1[0] = '\n';
        }
        
    } else {
        printf("no input pl file\n");
        return -1;
    }

	   
    lua_State *L;

    L = luaL_newstate();                        /* Create Lua state variable */
    luaL_openlibs(L);                           /* Load Lua libraries */

	luaopen_lpeg(L);
    luaopen_cjson(L);
    // lua_pushcfunction(L, l_getarg);
    // lua_setglobal(L, "lua_arg1");
    
    // if (luaL_loadfile(L, script))    /* Load but don't run the Lua script */
    // bail(L, "luaL_loadfile() failed");      /* Error out if file can't be read */

    lua_pushstring(L, plfile);
    lua_setglobal(L, "plfile");

    if (luaL_loadbuffer(L, plreader_lua, plreader_lua_len, plreader_lua)) {
         bail(L, "luaL_dostring() failed"); 
    }


    safe_lua_pcall(L, 0, 0);



    lua_close(L);                               /* Clean up, free the Lua state var */

	
    return 0;
} 

