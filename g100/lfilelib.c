void change_password(MEMZ *bf, unsigned char *password)
{
    int i;
    unsigned char ctow=1;

    if (bf->ztype != 1)
	return;

    for (i=0; i<8; i++) {
	if (ctow)
	    ctow = password[i];
	pokeb(bf->segment, bf->offset + 8 + 3 + i, ctow);
    }
}

LUALIB_API int luaL_newmetatable (lua_State *L, const char *tname) {
  lua_getfield(L, LUA_REGISTRYINDEX, tname);  /* get registry.name */
  if (!lua_isnil(L, -1))  /* name already in use? */
    return 0;  /* leave previous value on top, but return 0 */
  lua_pop(L, 1);
  lua_newtable(L);  /* create metatable */
  lua_pushvalue(L, -1);
  lua_setfield(L, LUA_REGISTRYINDEX, tname);  /* registry.name = metatable */
  return 1;
}


LUALIB_API void *luaL_checkuserdata (lua_State *L, int ud) {
  void *p = lua_touserdata(L, ud);
  if (p != NULL) {  /* value is a userdata? */

        return p;
      }
  luaL_typerror(L, ud, "file");  /* else error */
  return NULL;  /* to avoid warnings */
}

int globalrefresh =0;

typedef struct filedata
{
  struct memory_zone bf;
  unsigned int taillezone;
  char far* debut;
  int refreshnum ;
  char close;
  char option;
} filedata;




static Fgetsize (lua_State *L) {
filedata *f = (filedata *)lua_touserdata(L, 1);
luaL_argcheck(L, f != NULL, 1, "file expected");
if(f->close) luaL_error(L,"attempt to use a deleted file");
lua_pushnumber(L, f->taillezone);
return 1;
}

static Fnew (lua_State *L) {
struct memory_zone bf;
char* name = luaL_checkstring(L, 1);
luaL_argcheck(L, (strlen(name) >=2)&&(strlen(name <=8)),1, "bad name size");
luaL_argcheck(L, name[0] == 'L' && name[1] == 'F',1,"name must begin with LF");
luaL_argcheck(L, search_mem_zone(1,name,&bf)==-1,1,"file do already exist");
int size = luaL_checkint(L, 2);
int size2 = size;
static const char *const opts[] ={"char","uchar","int","float"};
int option = luaL_checkoption(L, 3, "float", opts);
switch (option) { case 0: case 1: break;
                  case 2: size2*=2; break;
                  case 3: size2*=4;
                }
luaL_argcheck(L, (size2>0)&&(size>0),2,"bad size");
filedata *f = (filedata *)lua_newuserdata(L,sizeof(filedata));
luaL_getmetatable(L, "ramfile");
lua_setmetatable(L, -2);
globalrefresh++;
if (create_mem_zone(1,name,(unsigned int)size2 + 24,CREATE_ZONE)<0 ) luaL_error(L,"not enough memory for file");
search_mem_zone(1,name,&(f->bf));
f->taillezone = size;
f->debut = MK_FP(f->bf.segment+1, f->bf.offset+5);
f->refreshnum = globalrefresh;
f->close =0;
f->option = option;
change_password(&(f->bf),"LUAF");
return 1;
}

static Fsearch(lua_State *L) {
struct memory_zone bf;
char* name = luaL_checkstring(L, 1);
luaL_argcheck(L, (strlen(name) >=2)&&(strlen(name <=8)),1, "bad name size");
luaL_argcheck(L, name[0] == 'L' && name[1] == 'F',1,"name must begin with LF");
static const char *const opts[] ={"char","uchar","int","float"};
int option = luaL_checkoption(L, 3, "float", opts);
int coeff;
switch (option) { case 0: case 1:coeff = 1; break;
                  case 2: coeff=2; break;
                  case 3: coeff=4;
                }
int res = search_mem_zone(1,name,&bf);
if (res <= 0) { 
  lua_pushnil(L);
  lua_pushstring(L,"not found");
  return 2;
}
if(bf.real_size % coeff !=0)
{
  lua_pushnil(L);
  lua_pushstring(L,"bad size");
  return 2;
}
filedata *f = (filedata *)lua_newuserdata(L,sizeof(filedata));
luaL_getmetatable(L, "ramfile");
lua_setmetatable(L, -2);
memcpy(&(f->bf),&bf,sizeof(MEMZ));
f->taillezone = bf.real_size;
f->debut = MK_FP(f->bf.segment+1, f->bf.offset+5);
f->refreshnum = globalrefresh;
f->close =0;
f->option = option;
return 1;
}
        /*
static Fresizemeth (lua_State *L) {     // fichier + size
filedata *f = (filedata *)luaL_checkuserdata(L, 1);
if(f->close) luaL_error(L,"attempt to use a deleted file");
int size = luaL_checkint(L, 2);
luaL_argcheck(L,size <= -(int)f->taillezone,2,"bad size");
unsigned int newsize = f->taillezone-size;
if (create_mem_zone(1,f->bf.name,newsize + 24,RESIZE_ZONE)<0 ) luaL_error(L,"not enough memory for file");
globalrefresh++;
search_mem_zone(1,f->bf.name,&(f->bf));
f->taillezone = newsize;
f->debut = MK_FP(f->bf.segment+1, f->bf.offset+5);
f->refreshnum = globalrefresh;
f->close =0;
return 0;
}        */

static Fresize (lua_State *L) {
filedata *f = (filedata *)luaL_checkuserdata(L, 1);
if(f->close) luaL_error(L,"attempt to use a deleted file");
int size = luaL_checkint(L, 2);
luaL_argcheck(L,size<=0,2,"bad size");
unsigned int newsize = size;
if (create_mem_zone(1,f->bf.name,newsize + 24,RESIZE_ZONE)<0 ) luaL_error(L,"not enough memory for file");
globalrefresh++;
search_mem_zone(1,f->bf.name,&(f->bf));
f->taillezone = newsize;
f->debut = MK_FP(f->bf.segment+1, f->bf.offset+5);
f->refreshnum = globalrefresh;
//f->close =0;
return 0;
}

static Fdelete (lua_State *L) {
filedata *f = (filedata *)luaL_checkuserdata(L, 1);
if(f->close) luaL_error(L,"attempt to use a deleted file");
if (create_mem_zone(1,f->bf.name,f->taillezone + 24,DELETE_ZONE)<0 ) luaL_error(L,"not enough memory for file");
globalrefresh++;
f->close =1;
StkId o = index2adr(L,1);
setnilvalue(o);
return 0;
}

static Fsizeram (lua_State *L) {
lua_pushnumber(L,__LNGFLT( (long) afxleft()) );
return 0;
}

static Foption (lua_State *L) {
filedata *f = (filedata *)luaL_checkuserdata(L, 1);
if(f->close) luaL_error(L,"attempt to use a deleted file");
lua_pushinteger(L,f->option );
return 0;
}



static const luaL_Reg filelib[] = {
  {"length", Fgetsize},
  {"search", Fsearch},
  {"new", Fnew},
  {"mode", Foption},
  {"resize", Fresize},
  {"delete", Fdelete},
  {"ramsize", Fsizeram},
  {NULL, NULL}
};


static const luaL_Reg metafilelib[] = {
  {"__len", Fgetsize},
  {NULL, NULL}
};




LUALIB_API int luaopen_filelib (lua_State *L) {
  luaI_openlib(L, "file", filelib, 0);
  luaL_newmetatable(L,"ramfile");
  luaL_openlib(L, 0, metafilelib, 0);
  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, -3);               /* dup methods table*/
  lua_rawset(L, -3);                  /* hide metatable:
                                         metatable.__metatable = methods */
  lua_pop(L, 1);                      /* drop metatable */
  lua_pop(L, 1);                       /* drop methods*/
  return 1;                           /* return methods on the stack */
}




