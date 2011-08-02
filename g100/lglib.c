
#include "random.h"
#include "gfunc.c"






int lua_toint (lua_State *L, int idx) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  if (tonumber(o, &n)) {
    lua_Integer i;
    lua_Number d;
    if (ttisint(o)) return ivalue(o);
    d= nvalue_fast(o);
    i=ftoi(d);
    return i;
  }
  else
    tag_error(L, idx, LUA_TNUMBER);
}

int lua_opttoint (lua_State *L, int idx,int opt) {
  TValue n;
  const TValue *o = index2adr(L, idx);
  if( o->tt<=0 ) return opt; //nil or none
  if (tonumber(o, &n)) {
    lua_Integer i;
    lua_Number d;
    if (ttisint(o)) return ivalue(o);
    d= nvalue_fast(o);
    i=ftoi(d);
    return i;
  }
  else
    tag_error(L, idx, LUA_TNUMBER);
}


//const static float lgtofl[5]= { 0.0f , 1.0f , 2.0f, 3.0f, 4.0f };

static int testpixel(lua_State *L)
{
 unsigned char xx, yy, page;
 xx = (lua_toint(L, 1) -1)&0x7F;
yy = (lua_toint(L, 2) -1)&0x3F;
// if (testplage(xx,yy)) return 0;
 if(NetB){
page = lua_opttoint(L, 3,0);
//lua_pushnumber(L,lgtofl[tstpixel(xx,yy,page)]);
lua_pushinteger(L,tstpixel(xx,yy,page));
return 1; }
else {
 //lua_pushnumber(L,lgtofl[gxPixTest(xx,yy)]);
lua_pushinteger(L,gxPixTest(xx,yy));
return 1;
}
}
static int pixl(lua_State *L)
{
 unsigned char coul;
 unsigned char xx,yy;
xx = (lua_toint(L, 1) -1)&0x7F;
yy = (lua_toint(L, 2) -1)&0x3F;
coul = lua_opttoint(L, 3,(NetB)? 1 : 4);
if(NetB)setPix((short)xx,(short)yy,lua_opttoint(L, 4,0),coul);
else gxPixel((xx),(yy),coul,0);
return 0;
}


static int print2(lua_State *L)
{

 // récupère le nombre de paramètres dans la pile
 unsigned short n = lua_gettop(L);

 // scanne tous les paramètres
 unsigned short i;
 if(NetB){
 for( i=1; i<=n; i++ )
 {

   // si le paramètre est une chaïne de caractères ou un nombre
   if( lua_isstring(L,i) )
     printf("%s", lua_tostring(L,i) );

   // si le paramètre est nul
   else if( lua_isnil(L,i) )
     printf("nil");

   // si le paramètre est un booléen
   else if( lua_isboolean(L,i) )
     printf(lua_toboolean(L,i) ? "true" : "false" );

 }
 }
 return 0;
}



 static int misc(lua_State *L)
{
 unsigned short n = lua_gettop(L);
 static const char *const opts[] ={"space","numcalc","exit","curseur","tostr","cont","prec","scrollx","scrolly","map"};
 int o = luaL_checkoption(L, 1, "space", opts);
 long a;  int b;
 unsigned char c = 100;
 char str[30]={0};
 lua_Number parametre;
 lua_Number result = -1;
 if (o>=4 && o<7) a=ftol(lua_tonumber(L, 2));
 if (o>=7 && o<9) {b = lua_toint(L,2);
 if (n==3) c= lua_toint(L,3);
 }
 switch(o)
 {
  case 0: result=__UINFLT(coreleft()-_stack);break;//coreleft()); break;
  case 1: result=100.f; break;
  case 2: exit(1);
  case 3: lua_pushnumber(L,__LNGFLT(TXTCURSOR_X+1));
          lua_pushnumber(L,__LNGFLT(TXTCURSOR_Y+1));
          return 2;
  case 4: ltoa( a, (char*)&str,10 ) ;
       lua_pushstring(L, str);  return 1;
  case 5: if (a==-1)  contless() ;
 else if (a==1) contplus(); break;
  case 6: ndig=a;break;
  case 7: //scrollx(b,c) ;break;
  case 8: break;//scrolly(b,c); break;
  case 9: if(!NetB)gxDrawBmp((char*)lua_tostring(L,2));
 }
 lua_pushnumber(L,result);
 return 1;

}



 static int math(lua_State *L)
{
 static const char *const opts[] ={"ln","exp","sqrt","racine"};
 int o = luaL_checkoption(L, 1, "sqrt", opts);
 float a;
 char str[30]={0};
 lua_Number parametre;
 lua_Number result = -1.f;
  a=lua_tonumber(L, 2);
 switch(o)
 {
  case 0: result = logff(a); break;
  case 1: result = expff(a); break;
  case 2: case 3: result = rac2(a); break;
 }
 if (getmatherror) {
   getmatherror = false; //for the next time
   lua_pushnil(L);
 }
 else  lua_pushnumber(L,result);
 return 1;

}




static int randomm(lua_State *L)
{
lua_pushinteger(L,(Randommm((lua_toint(L, 1)))));
return 1;
}


static int waitlua(lua_State *L)
{
 wait(lua_toint(L, 1));
 return 0;
}

static int setcolor (lua_State *L)
{
 int parametre;
parametre = lua_toboolean(L, 1);
if(!parametre && !NetB)
gxSetMode(false);
else if(parametre &&NetB)
gxSetMode(true);
 return 0;
}
static int letter (lua_State *L)
{
 int x,y;

 if(!NetB){

x = lua_toint(L, 1)-1;
y = lua_toint(L, 2)-1;

switch(lua_type(L,3)){
case LUA_TSTRING:
writeg(x,y,(char*)lua_tostring ( L , 3  ) ) ;
break;
case LUA_TNUMBER:
writevalue(x,y,ftol(lua_tonumber(L, 3)));
break;
default: break;
}

 return 0;
 }
}
static int fill (lua_State *L)
{
 unsigned char x;
if(!NetB) gxFill(lua_toint(L, 1));
 return 0;
}

long InputBoxNum(long max)
{
	int key = 0;
	int nbc = 0;
	long num = 0;
	int i,j;
	char ch;
	char x=TXTCURSOR_X,y=TXTCURSOR_Y;
	char press=1;

        while (thetouche (1,5) == 1) asm hlt;
	while((thetouche (1,5) == 0))
	{
		if((thetouche (7,4) == 1))
		{
			num = -1;
			break;
		}
		if(nbc > 0 && (thetouche (4,4) == 1))
		{
			nbc--;
			num /= 10;
                        gotoxy(y,x);
                        for(j=0;j<nbc+1;j++) printf(" ");
		gotoxy(y,x);
		printf("%li",num);
		while (thetouche (4,4) == 1) asm hlt;
		ch=10;
		}
		else if(thetouche (1,1) == 1) ch = 0;
                else if (thetouche (2,1) == 1) ch = 1;
                else if (thetouche (2,2) == 1)        ch = 2;
                else if (thetouche (2,3) == 1)         ch = 3;
                else if (thetouche (3,1) == 1)        ch = 4;
                else if (thetouche (3,2) == 1)             ch = 5;
                else if (thetouche (3,3) == 1)                ch =6;
                else if (thetouche (4,1) == 1)                 ch = 7;
                else if (thetouche (4,2) == 1)                   ch = 8;
                else if (thetouche (4,3) == 1)                    ch = 9;
                else {ch = 10;press=0;}
                if(press==0 && ch < 10 && num <= (max-ch)/10)
		{
                        num = num * 10 + ch;
                press=1;
		nbc++;
		gotoxy(y,x);
		for(j=0;j<nbc;j++) printf(" ");
		gotoxy(y,x);
		printf("%li",num);
		}
		asm hlt;
		asm hlt;
	}
	return num;
}

static int numm (lua_State *L)
{
 long x;
 unsigned short n = lua_gettop(L);
 if(NetB){ if(n==0) x= InputBoxNum( 999999999);
 else x= InputBoxNum(ftol(lua_tonumber ( L , 1  ))) ;
 lua_pushnumber(L,__LNGFLT(x));
 return 1;
}
return 0;
}


void copypage(int source ,int target){asm{
	push ds
	mov ax,target
	shl ax,6
	add ax,0x1A20
	mov es,ax
	mov di,0
	mov ax,source
	shl ax,6
	add ax,0x1A20
	mov ds,ax
	mov si,0
	cld
	mov cx,512
	rep movsw
	pop ds};
}
static int state (lua_State *L)  // à  modifier
{
unsigned short n = lua_gettop(L);
 unsigned char x,y;
if (n==2 && NetB)
{
  x = lua_toint(L, 1);
  y = lua_toint(L, 2);
if(x>=0 && y>=0 && x<=6 && y<=6) copypage(x,y);
}
return 0;

}


static int curseur(lua_State *L)
{
 unsigned short n = lua_gettop(L);
if(NetB && n==2)gotoxy(lua_toint(L, 1)-1,lua_toint(L, 2)-1);
return 0;
}

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

static int ligne(lua_State *L)
{
 unsigned short n = lua_gettop(L);
 int x,y,y1,x1,page=0,xx;
 int coul;
 void (*funcpix) (uint,uint, uchar, uchar ) ;
x = (lua_toint(L, 1)-1);
y = (lua_toint(L, 2)-1);
x1 = (lua_toint(L, 3)-1);
y1 = (lua_toint(L, 4)-1);
if(n>=5) {page= lua_toint(L, 5);if(0>page || page >5) page = 0;}   //peut representer la couleur

int dx = abs(x1-x), sx = x<x1 ? 1 : -1;
int dy = abs(y1-y), sy = y<y1 ? 1 : -1;

if ( !NetB )
   if (n==6 || dx == 0 || dy == 0) {

      if (x>x1) {xx=x ; x=max(x1 ,0) ;x1= min(xx,127);}
      if (y>y1) {xx=y ; y=max (y1,0) ;y1= min (xx,63);}      // donc y<=y1 et x<=x1

         if (n == 6) {            // rectangle
         coul = lua_toint(L,6);
         int i;
             for( i=y ; i<=y1 ; i++){
              gxHLine(x, x1 , i , coul);
             }

               if (page!=coul) {
               gxHLine(x,x1,y,page);
               gxVLine(x1,y,y1,page);
               gxHLine(x,x1,y1,page);
               gxVLine(x,y,y1,page);
               }
         }
         else if (dy==0) gxHLine(x,x1,y1,page);
         else gxVLine(x,y,y1,page);              //dx == 0
      return 0;
      }
     else funcpix = gxPixel;
 // sinon tracé ligne  (funcpix est bien défini)

else funcpix = setPix;

     int err = (dx>dy ? dx : -dy)/2, e2;
     for(;;){
     if ((x == x&0x007F) && (y == y&0x003F)) // si on est dans les plages de l'écran (tester si pas de pb pour les valeurs négatives)
       funcpix(x,y,page,1);
     //setPixx(x0,y0,couleur);
     if (x==x1 && y==y1) break;
     e2 = err;
     if (e2 >-dx) { err -= dy; x += sx; }
     if (e2 < dy) { err += dx; y += sy; }
     }

return 0;

}



static int picture(lua_State *L)      // à  modifier
{
 unsigned short n = lua_gettop(L);
 char param;
 char a,b;
 int i;
 param= lua_toint(L,1);
 a= lua_toint(L,2);
 b= lua_toint(L,3);
 if (n==3 && pictadr[2*b]!=0 && a<=6) {
 if (param==1)
 {
   copy(MK_FP((0x1A20+40*a),(0x0)),MK_FP((pictadr[2*b]), (pictadr[(2*b)+1])));
   /*for(i = 0x0000;i<0x1000 ;i+=0x0001 )
   {
     pokeb(0x1A20+40*a,i,peekb(pictadr[2*b],(pictadr[(2*b)+1]+i)));
     }*/

 }
 else if (param==2)
 {
   copy(MK_FP((pictadr[2*b]),(pictadr[(2*b)+1])),MK_FP((0x1A20+40*a),0x0));
  /*for(i = 0x0000;i<0x1000 ;i+=0x0001 )
   {
     pokeb(pictadr[2*b],(pictadr[(2*b)+1]+i),peekb(0x1A20+40*a,i));
     } */

 }  lua_pushboolean(L,1);      }
 else lua_pushboolean(L,0);
 return 1;
}
static int sav(lua_State *L)        // à  modifier
{
 unsigned short n = lua_gettop(L);
 char param;
 unsigned char a;
 float b;
 unsigned int* tai;
 param= lua_toint(L,1) -1;
 if (param != 0) {param = 2*(param);}    // evite 2*0
 a= ((unsigned int)(lua_toint(L,2)-1));

 if (n==3 && param >=0 && param <=4) { //4 = 2*(3-1)
   b= lua_tonumber(L,3);
 tai=(unsigned int*)&b;
 poke(saveadr[param], saveadr[(param)+1]+4*a,  tai[0]);
 poke(saveadr[param],(saveadr[(param)+1])+2+4*a,tai[1]);
 }
 else if(n==2 && param >=0 && param <=4)
 {
  tai=(unsigned int*)&b;
 tai[0]=peek(saveadr[param], saveadr[(param)+1]+4*a );
 tai[1]=peek(saveadr[param],(saveadr[(param)+1])+2+4*a);
 lua_pushnumber(L,b);
 return 1;}
 return 0;
}



static const luaL_Reg glib[] = {
      {"letter", letter},
      {"copypage", state},
      {"pixl", pixl},
      {"tstpixel", testpixel},
      {"setcolor", setcolor},
      {"ligne", ligne},
      {"math", math},
      {"random", randomm},
      {"curseur", curseur},
      {"wait", waitlua},
      {"fill", fill},
      {"inputnum", numm},
      {"picture", picture},
      {"save", sav},
       {"misc", misc},
       {"print", print2},
      {NULL, NULL}  /* sentinel */
   };

void luag_init(lua_State *L)
{
 luaI_openlib(L, "g", glib, 0);
}
