/*
** $Id: lbaselib.c,v 1.191.1.6 2008/02/14 16:46:22 roberto Exp $
** Basic library
** See Copyright Notice in lua.h
*/

#include <ctype.h>

#define lbaselib_c
#define LUA_LIB

#include "lua.h"

#include "lauxlib.h"




static int luaB_tonumber (lua_State *L) {
  int base = luaL_optint(L, 2, 10);
  if (base == 10) {  /* standard conversion */
    luaL_checkany(L, 1);
    if (lua_isnumber(L, 1)) {       /* numeric string, or a number */
      lua_pushnumber(L, lua_tonumber(L, 1));//lua_pushvalue_as_number(L,1);     /* API extension (not to lose accuracy here) */
      return 1;
    }
  }
  else {
    const char *s1 = luaL_checkstring(L, 1);
    char *s2;
    unsigned long n;
    luaL_argcheck(L, 2 <= base && base <= 36, 2, "base out of range");
    n = strtoul(s1, &s2, base);
    if (s1 != s2) {  /* at least one valid digit? */
      while (isspace((unsigned char)(*s2))) s2++;  /* skip trailing spaces */
      if (*s2 == '\0') {  /* no invalid trailing characters? */
        lua_pushnumber(L, __LNGFLT(n));
        return 1;
      }
    }
  }
  lua_pushnil(L);  /* else not a number */
  return 1;
}


static int luaB_error (lua_State *L) {
  int level = luaL_optint(L, 2, 1);
  lua_settop(L, 1);
  if (lua_isstring(L, 1) && level > 0) {  /* add extra information? */
    luaL_where(L, level);
    lua_pushvalue(L, 1);
    lua_concat(L, 2);
  }
  return lua_error(L);
}


static void getfunc (lua_State *L, int opt) {
  if (lua_isfunction(L, 1)) lua_pushvalue(L, 1);
  else {
    lua_Debug ar;
    int level = opt ? luaL_optint(L, 1, 1) : luaL_checkint(L, 1);
    luaL_argcheck(L, level >= 0, 1, "level must be non-negative");
    if (lua_getstack(L, level, &ar) == 0)
      luaL_argerror(L, 1, "invalid level");
    lua_getinfo(L, "f", &ar);
    if (lua_isnil(L, -1))
      luaL_error(L, "no function environment for tail call at level %d",
                    level);
  }
}


static int luaB_getfenv (lua_State *L) {
  getfunc(L, 1);
  if (lua_iscfunction(L, -1))  /* is a C function? */
    lua_pushvalue(L, LUA_GLOBALSINDEX);  /* return the thread's global env. */
  else
    lua_getfenv(L, -1);
  return 1;
}


static int luaB_setfenv (lua_State *L) {
  luaL_checktype(L, 2, LUA_TTABLE);
  getfunc(L, 0);
  lua_pushvalue(L, 2);
  if (lua_isnumber(L, 1) && lua_tointeger(L, 1) == 0) {
    /* change environment of current thread */
    lua_pushthread(L);
    lua_insert(L, -2);
    lua_setfenv(L, -2);
    return 0;
  }
  else if (lua_iscfunction(L, -2) || lua_setfenv(L, -2) == 0)
    luaL_error(L,
          LUA_QL("setfenv") " cannot change environment of given object");
  return 1;
}



static int luaB_rawequal (lua_State *L) {
  luaL_checkany(L, 1);
  luaL_checkany(L, 2);
  lua_pushboolean(L, lua_rawequal(L, 1, 2));
  return 1;
}



static int luaB_collectgarbage (lua_State *L) {
  static const char *const opts[] = {"stop", "restart", "collect",
    "count", "step", "setpause", "setstepmul","setmemlimit","getmemlimit", NULL};
  static const int optsnum[] = {LUA_GCSTOP, LUA_GCRESTART, LUA_GCCOLLECT,
    LUA_GCCOUNT, LUA_GCSTEP, LUA_GCSETPAUSE, LUA_GCSETSTEPMUL,
		LUA_GCSETMEMLIMIT,LUA_GCGETMEMLIMIT};;
  int o = luaL_checkoption(L, 1, "collect", opts);
  int ex = luaL_optint(L, 2, 0);
  int res = lua_gc(L, optsnum[o], ex);
  switch (optsnum[o]) {
    case LUA_GCCOUNT: {
      int b = lua_gc(L, LUA_GCCOUNTB, 0);
      lua_pushnumber(L, __UINFLT(res*1024 + b));
      return 1;
    }
    case LUA_GCSTEP: {
      lua_pushboolean(L, res);
      return 1;
    }
    default: {
      lua_pushinteger(L, res);
      return 1;
    }
  }
}

  
static int luaB_type (lua_State *L) {
  luaL_checkany(L, 1);
  lua_pushstring(L, luaL_typename(L, 1));
  return 1;
}


static int luaB_next (lua_State *L) {
  luaL_checktype(L, 1, LUA_TTABLE);
  lua_settop(L, 2);  /* create a 2nd argument if there isn't one */
  if (lua_next(L, 1))
    return 2;
  else {
    lua_pushnil(L);
    return 1;
  }
}




static int load_aux (lua_State *L, int status) {
  if (status == 0)  /* OK? */
    return 1;
  else {
    lua_pushnil(L);
    lua_insert(L, -2);  /* put before error message */
    return 2;  /* return nil plus error message */
  }
}



static int luaB_assert (lua_State *L) {
  luaL_checkany(L, 1);
  if (!lua_toboolean(L, 1))
    return luaL_error(L, "%s", luaL_optstring(L, 2, "assertion failed!"));
  return lua_gettop(L);
}


static int luaB_unpackold (lua_State *L) {
  int i, e, n;
  luaL_checktype(L, 1, LUA_TTABLE);
  i = luaL_optint(L, 2, 1);
  e = luaL_opt(L, luaL_checkint, 3, luaL_getn(L, 1));
  if (i > e) return 0;  /* empty range */
  n = e - i + 1;  /* number of elements */
  if (n <= 0 || !lua_checkstack(L, n))  /* n <= 0 means arith. overflow */
    return luaL_error(L, "too many results to unpack");
  lua_rawgeti(L, 1, i);  /* push arg[i] (avoiding overflow problems) */
  while (i++ < e)  /* push arg[i + 1...e] */
    lua_rawgeti(L, 1, i);
  return n;
}

static int luaB_unpack(lua_State *L) {
  int i, e, n;
  luaL_checktype(L, 1, LUA_TTABLE);
  i = luaL_optint(L, 2, 1);
  e = luaL_opt(L, luaL_checkint, 3, luaL_getn(L, 1));
  if (i > e) return 0;  /* empty range */
  n = e - i + 1;  /* number of elements */
  if (n <= 0 || !lua_checkstack(L, n))  /* n <= 0 means arith. overflow */
    return luaL_error(L, "too many results to unpack");
  StkId o = index2adr(L,1);
  setobj2s(L, L->top, luaH_getint(hvalue(o), i));
  L->top++;
  while (i++ < e)  /* push arg[i + 1...e] */  
  {
    setobj2s(L, L->top, luaH_getint(hvalue(o), i));
    L->top++;                                  }
  return n;
}



static int luaB_pcall (lua_State *L) {
  int status;
  luaL_checkany(L, 1);
  status = lua_pcall(L, lua_gettop(L) - 1, LUA_MULTRET, 0);
  lua_pushboolean(L, (status == 0));
  lua_insert(L, 1);
  return lua_gettop(L);  /* return status + all results */
}


static int luaB_tostring (lua_State *L) {
  luaL_checkany(L, 1);
  if (luaL_callmeta(L, 1, "__tostring"))  /* is there a metafield? */
    return 1;  /* use its value */
  switch (lua_type(L, 1)) {
    case LUA_TNUMBER:
      lua_pushstring(L, lua_tostring(L, 1));
      break;
    case LUA_TSTRING:
      lua_pushvalue(L, 1);
      break;
    case LUA_TBOOLEAN:
      lua_pushstring(L, (lua_toboolean(L, 1) ? "true" : "false"));
      break;
    case LUA_TNIL:
      lua_pushliteral(L, "nil");
      break;
    default:
      lua_pushfstring(L, "%s: %p", luaL_typename(L, 1), lua_topointer(L, 1));
      break;
  }
  return 1;
}

// fichier chargeables : fichiers .lc ( dont une fois envoyés sur g100 on remplace par LCNOM )
// sur g85 le chargement des fichiers est différent (regarder main.c )


// fonction test faite pour tester si sans algo de recherche ça marche
static int test (lua_State *L) {
  struct memory_zone bf;
  search_mem_zone(1,"LCTEST", &bf);
  unsigned int segment=bf.segment +1;
  unsigned int offset=bf.offset + 5;//(unsigned int)offsetl;
  unsigned int taille= bf.real_size;
  return load_aux(L, luaL_loadbuffer (L, MK_FP((segment), (offset)), taille,"LIB"));     // chargement type g100
}

#define _FP_OFF(fp) (*((unsigned __far *)&(fp)))

#define _FP_SEG(fp) (*((unsigned __far *)&(fp)+1))

#define MODLIST 0
#define MODLOAD 1

#define LENGTH_ERROR 0
#define IMPOSSIBLE_ERROR 1
#define  NOTFOUND 2
#define  INVALID_ARG 3
const unsigned char* err[4] = {"bad length","impossible error","module not found","invalid arguments"};

// fonction remplaçable par une utilisation de memcpy
void my_movedata (unsigned int src_seg, unsigned int src_off, unsigned int des_seg, unsigned int des_off, size_t size_to_copy, int direction)
{
    if (size_to_copy < 2) {
	if (size_to_copy==1)
	    *((unsigned char far *)MK_FP(des_seg,des_off))=*((unsigned char far *)MK_FP(src_seg,src_off));
	return;
    }

    if (direction) {
	asm {
	    // pushes {{{
	    push si;
	    push es;
	    push di;
	    push cx;
	    push bx;//saves registers
	    pushf;//save flags
	    // }}}

	    mov bx, ds;//saves ds
	    mov cx, size_to_copy;
	    //set segment and offset source and destination.
	    mov di, des_off;
	    mov es, des_seg;
	    mov si, src_off;
	    mov ds, src_seg;// MUST BE THE LAST SET FROM VARIABLE !!!!

	    //set offsets to end
	    dec cx;
	    dec cx;//makes si and di point 1 word before the string end.
	    add si, cx;
	    add di, cx;

	    //set number of word to copy
	    inc cx;
	    inc cx;
	    shr cx, 1;  //cx/=2

	    std;//backward copy
	    rep movsw;

	    mov ds, bx;//restor ds
	    //copy the first byte (if impair...)
	    mov es, src_seg;
	    mov di, src_off;
	    mov cl, es:[di];
	    mov es, des_seg;
	    mov di, des_off;
	    mov es:[di], cl;

	    // pops {{{
	    popf;//restore flags
	    pop bx;
	    pop cx;
	    pop di;
	    pop es;
	    pop si;
	    // }}}
	}
	return;
    }
    asm {
	// pushes {{{
	push si;
	push es;
	push di;
	push cx;
	push bx;//saves registers
	pushf;//save flags
	// }}}

	mov bx, ds;//saves ds

	mov cx, size_to_copy;
	//set number of word to copy
	shr cx, 1;  //cx/=2

	//set segment and offset source and destination.
	mov di, des_off;
	mov es, des_seg;
	mov si, src_off;
	mov ds, src_seg;// MUST BE THE LAST SET FROM VARIABLE !!!!

	cld;//backward copy
	rep movsw;

	mov ds, bx;//restor ds
	//copy the last byte (if impair...)
	//set offsets to end
	mov cx, size_to_copy;
	dec cx;
	mov es, src_seg;
	mov di, src_off;
	add di, cx;
	mov bl, es:[di];
	mov es, des_seg;
	mov di, des_off;
	add di, cx;
	mov es:[di], bl;

	// pops {{{
	popf;//restore flags
	pop bx;
	pop cx;
	pop di;
	pop es;
	pop si;
	// }}}
    }
    return;
}

// fonction remplaçable par une utilisation de memset
void init_area(unsigned int msegment, unsigned int moffset, unsigned int msize, unsigned int value, int direction)
{
    unsigned long transit;
    if (!msize) return;

    transit = (((unsigned long)msegment)<<4) + (unsigned long)moffset;
    if (direction) {
	transit-=msize;
    }

    if (msize>0xFFF0) msize = 0xFFF0;

    msegment = (unsigned int)(transit >>   4);
    moffset  = (unsigned int)(transit &  0xF);


    if (msize%2) {
	asm {
	    push ax;
	    push es;
	    push di;

	    mov es,msegment;
	    mov di,moffset;
	    add di,msize;
	    dec di;
	    mov ax, value;
	    mov es:[di],al;

	    pop di;
	    pop es;
	    pop ax;
	}
    }


    asm {
	pushf;
       	push es;
       	push di;
       	push cx;
       	push ax;
	
	mov es, msegment;
	mov di, moffset;
	mov cx, msize;
	shr cx,1;
	mov ax, value;
	cld;
	rep stosw;
	
	pop ax;
       	pop cx;
       	pop di;
       	pop es;
	popf;
    }
    return;
}

unsigned int getDS ()
{
    asm mov ax, ds;
    return _AX;
}

#define _DS getDS()

void far* add_transit(void far* fp, unsigned long transit)
{
	return (void far*)((unsigned long)fp + (transit & 0xF) + ((transit >> 4) << 16));
}

// recherch de 'module nomdumodule' avec nomdumodule en moins de 8 lettres

int modfunc (lua_State *L,int id) {
struct memory_zone bf;
unsigned char far* curent_b_ptr;
curent_b_ptr = tell_mem_zone_seg(1);// 1 -> basic files
unsigned char filename[9];
unsigned char far* lieu = filename;
unsigned char luasignature[] = LUA_SIGNATURE;
unsigned char modstring[17] = {'m','o','d','u','l','e',' ','\0','\0','\0','\0','\0','\0','\0','\0','\0'} ;
size_t length; unsigned char* nom;
int error;int i;
int trouve =0;
unsigned short n = lua_gettop(L);
if (id == MODLOAD)
 {
 if ((!lua_isstring(L,1)) || (n!=1)) {error=INVALID_ARG; goto erreur; }
 nom = lua_tolstring(L,1,&length);
 if (length <= 0 || length >8) {error=LENGTH_ERROR; goto erreur; }
  for (i = 0;i<length ; i++) modstring[i+7] = nom[i];
  length += 7; // taille de "module "      -> on recherche juste les fichiers modules et on donnera le nom du module trouvé
  }
 else {length = 7;if (n!=0) {error=INVALID_ARG; goto erreur; }}
while(curent_b_ptr[2] == 1)
		{
                        init_area(_FP_SEG(lieu), _FP_OFF(lieu), 9, 0, 0);
                         my_movedata(_FP_SEG(curent_b_ptr), _FP_OFF(curent_b_ptr)+3,_DS,(unsigned)filename,8,0);
                        filename[8]='\0'; //un peu répétitif...
                                         if (search_mem_zone(1,filename, &bf) <=1)  {error=IMPOSSIBLE_ERROR; goto erreur; }
                                         if(luasignature[0] ==peekb( bf.segment +1,bf.offset + 5)) //probable fichier lua lc ( sur g85 il faudra supprimer le if car le fichier peut ne pas être lc)
                                            {
                                                     int b=0;
	                                             int find=0;
	                                             int s = bf.segment+1;
                                                     int o;                                        // on parcoure tout le fichier à la recherche de 'module nomdumodule'
	                                             for (o=bf.offset+5; o<bf.offset+bf.real_size; o++)
	                                             {
		                                      if(((unsigned char)peekb(s, o))==(unsigned char)modstring[b])
		                                      {
			                               b++;
			                               if (b==length)
			                               {
			                                   find = 1;
			                                       break;
			                                }
                                                       }
		                                       else   b = 0; // on a pas trouvé
	                                            }
	                                            if (find == 1)      {
                                                                         if (id == MODLOAD)
                                                                           return load_aux(L, luaL_loadbuffer (L, MK_FP(s, bf.offset+5), bf.real_size,nom));
                                                                          else {
                                                                            trouve ++;
                                                                            init_area(_FP_SEG(lieu), _FP_OFF(lieu), 9, 0, 0);
                                                                            for (i = 0;i<8; i++) filename[i] = peekb(s, o+1+i);
                                                                            lua_pushstring(L,filename);
                                                                          }

                                                                         }

                                            }
			curent_b_ptr = (char far *)add_transit(curent_b_ptr, *((unsigned int far *)(curent_b_ptr)));
		}
if(id == MODLIST)return trouve;
else error = NOTFOUND;
erreur:
 lua_pushnil(L);
 lua_pushstring(L,err[error]);
return 2;
}
static int modload (lua_State *L) {
  return modfunc(L,MODLOAD);
}

static int modlist (lua_State *L) {
  return modfunc(L,MODLIST);
}



static const luaL_Reg base_funcs[] = {
  {"assert", luaB_assert},
  {"collectgarbage", luaB_collectgarbage},
//  {"dofile", luaB_dofile},
  {"error", luaB_error},
//  {"gcinfo", luaB_gcinfo},
  {"getfenv", luaB_getfenv},
//  {"getmetatable", luaB_getmetatable},
 // {"loadfile", luaB_loadfile},
 // {"load", luaB_load},
 // {"loadstring", luaB_loadstring},
  {"next", luaB_next},
  {"pcall", luaB_pcall},
 // {"print", luaB_print},
 // {"rawequal", luaB_rawequal},
//  {"rawget", luaB_rawget},
//  {"rawset", luaB_rawset},
//  {"select", luaB_select},
 {"setfenv", luaB_setfenv},
 // {"setmetatable", luaB_setmetatable},
  {"tonumber", luaB_tonumber},
  {"tostring", luaB_tostring},
  {"type", luaB_type},
  {"unpack", luaB_unpack},
  //{"test",test},
  {"modlist",modlist},
  {"modload",modload},
  {NULL, NULL}
};


static void auxopen (lua_State *L, const char *name,
                     lua_CFunction f, lua_CFunction u) {
  lua_pushcfunction(L, u);
  lua_pushcclosure(L, f, 1);
  lua_setfield(L, -2, name);
}


static void base_open (lua_State *L) {
  /* open lib into global table */
 lua_pushvalue(L, LUA_GLOBALSINDEX);
  lua_setglobal(L, "_G");
  luaL_register(L, "lib", base_funcs);
  /* `ipairs' and `pairs' need auxliliary functions as upvalues */
  //auxopen(L, "ipairs", luaB_ipairs, ipairsaux);
  //auxopen(L, "pairs", luaB_pairs, luaB_next);
}


LUALIB_API int luaopen_base (lua_State *L) {
  base_open(L);
  return 2;
}

/* algo de recherche de string

 
            b=0;
	    find=0;
	    s = segment

	    for (o=offsetdep; o<offsetfin; o++)
	    {
		    if(((unsigned char)peekb(s, o))==(unsigned char)yourstring[b])
		    {
			b++;
			if (b==length)
			{
			    find = 0;
			    break;
			}
                    }
		    else   b = 0; // on a pas trouvé
	    }


*/

