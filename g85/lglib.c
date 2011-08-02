
#include "mathf.h"
int lua_toint (lua_State *L, int idx) ;
int lua_opttoint (lua_State *L, int idx,int opt);




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


unsigned int Randommm (int num) ;// code dont je ne donnerais pas les sources
// c'est le même que sur g100


static int randomm(lua_State *L)
{
lua_pushnumber(L,(Randommm((lua_toint(L, 1)))));
return 1;
}



static int waitlua(lua_State *L)
{
 Sleep(20 * lua_toint(L, 1));
 return 0;
}

extern int cursy;
extern int cursx;

static int curseur(lua_State *L)
{
if(NetB) { 
  cursy=lua_toint(L, 1);
cursx =lua_toint(L, 2);
}
return 0;
}


 char *ultoa( unsigned long value, char *buffer, int radix );  // codes dont je ne donnerais pas les sources
char *ltoa( long value, char *buffer, int radix ) ;          // même que sur g100


static int misc(lua_State *L) // fonction qui va changer
{
 unsigned short n = lua_gettop(L);
 static const char *const opts[] ={"space","numcalc","exit","curseur","tostr","cont","prec","scrollx","scrolly","map"};
 int o = luaL_checkoption(L, 1, "space", opts);
 long a;  int b;
 unsigned char c = 100;
 char str[30]={0};
 lua_Number parametre;
 lua_Number result = -1;
 if (o>=4 && o<7) a=(int)(lua_tonumber(L, 2));
 if (o>=7 && o<9) {b = lua_toint(L,2);
 if (n==3) c= lua_toint(L,3);
 }
 switch(o)
 {
  case 0: result=40000 - (1024*lua_gc(L,LUA_GCCOUNT,0)+ lua_gc(L,LUA_GCCOUNTB,0));break;//coreleft()); break;   // 40000 est la mémoire disponible estimée sur g85
  case 1: result=85.f; break;
  case 2: reset_calc();
  case 3: lua_pushnumber(L,cursx);
          lua_pushnumber(L,cursy);
          return 2;
  case 4: ltoa( a, (char*)&str,10 ) ;
       lua_pushstring(L, str);  return 1;
  case 5: if (a==-1)  ML_set_contrast(ML_get_contrast() -1) ;
 else if (a==1) ML_set_contrast(ML_get_contrast() +1); break;
  case 6: ndigit=a;break;
  case 7: //scrollx(b,c) ;break;
  case 8: break;//scrolly(b,c); break;
  case 9: if(!NetB)draw_bmp((char*)lua_tostring(L,2));
 }
 lua_pushnumber(L,result);
 return 1;

}

// programmeurs: trouver lorsqu'il y a erreur dans le calcul (erreur range , ...) et si erreur alors -> getmatherror = true

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
  case 0: result = logf(a); break;
  case 1: result = expf(a); break;
  case 2: case 3: result = sqrtf(a); break;
 }
 /*if (getmatherror) {
   getmatherror = false; //for the next time
   lua_pushnil(L);
 } */
 //else  
 lua_pushnumber(L,result);
 return 1;

}


static int setcolor (lua_State *L)
{
 int parametre;
parametre = lua_toboolean(L, 1);
if(!parametre && !NetB)
{ gray_quit(); NetB = 1; }
else if(parametre &&NetB)
{gray_init(); NetB = 0;   }
 return 0;
}


static int fill (lua_State *L)
{
 unsigned char y; int color;
if(!NetB) {
  color =  lua_toint(L, 1);
  for (y=0;y<64;y++) {
                     Hline(0,127,y,color);
                     }

 }
 return 0;
}


 void gotoxy(char y, char x) {cursx = x; cursy = y; }
long InputBoxNum(long max)    // code de vebveb écrit pour g100. j'espère un jour qu'elle sera remplacée par la fonction que j'ai écrit en lua qui est plus performante/
{
	int key = 0;
	int nbc = 0;
	long num = 0;
	int i,j;
	char ch;
	char x=cursx,y=cursy;
	char press=1;

        while (keytch(5) == 1) Sleep(2);
	while((keytch(5)) == 0)
	{
		if((keytch(36) == 1))
		{
			num = -1;
			break;
		}
		if(nbc > 0 && (keytch(19) == 1))
		{
			nbc--;
			num /= 10;
                        gotoxy(y,x);
                        for(j=0;j<nbc+1;j++) printf(" ");
		gotoxy(y,x);
		printf("%li",num);
		while (keytch(19) == 1) Sleep(2);
		ch=10;
		}
		else if(keytch(1) == 1) ch = 0;
                else if (keytch(6) == 1) ch = 1;
                else if (keytch(7) == 1)        ch = 2;
                else if (keytch(8) == 1)         ch = 3;
                else if (keytch(11) == 1)        ch = 4;
                else if (keytch(12) == 1)             ch = 5;
                else if (keytch(13) == 1)                ch =6;
                else if (keytch(16) == 1)                 ch = 7;
                else if (keytch(17) == 1)                   ch = 8;
                else if (keytch(18) == 1)                    ch = 9;
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
		Sleep(4);
	}
	return num;
}

static int numm (lua_State *L)
{
 long x;
 unsigned short n = lua_gettop(L);
 if(NetB){
 if(n==0) x= InputBoxNum( 999999999);
 else x= InputBoxNum(lua_tonumber ( L , 1  )) ;
 lua_pushnumber(L,(x));
 return 1;
 }
 return 0;
}

int ML_pixel_test2(int x, int y,char* vram)
{
	char byte;
	byte = 1<<(7-(x&7));
	return (vram[(y<<4)+(x>>3)] & byte ? 1 : 0);
}

static int testpixel(lua_State *L)
{
 unsigned char xx, yy, page;
 xx = (lua_toint(L, 1) -1)&0x7F;
yy = (lua_toint(L, 2) -1)&0x3F;
 if(NetB){
page = lua_opttoint(L, 3,0);
if (page>=6) page = 0;
//lua_pushnumber(L,lgtofl[tstpixel(xx,yy,page)]);
lua_pushinteger(L,ML_pixel_test2(xx,yy,VRAM[page]));
return 1; }
else {
 //lua_pushnumber(L,lgtofl[gxPixTest(xx,yy)]);
lua_pushinteger(L,draw_read_pix(xx,yy));
return 1;
}
}

static int pixl(lua_State *L)
{
 unsigned char coul,page;
 unsigned char xx,yy;
xx = (lua_toint(L, 1) -1)&0x7F;
yy = (lua_toint(L, 2) -1)&0x3F;
coul = lua_opttoint(L, 3,1);
if(NetB) {
page = lua_opttoint(L, 4,0);
if( page >5) page = 0;
ML_pixel((short)xx,(short)yy,coul,VRAM[page]);
if (page == 0)ML_display_vram(VRAM[0]);
}
else {
 draw_write_pix(xx,yy,coul);

}
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
 short int x,y,y1,x1,page,i,xx,dx,dy,err,sx,sy,e2;
 short int coul;
x = (lua_toint(L, 1)-1);
y = (lua_toint(L, 2)-1);
x1 = (lua_toint(L, 3)-1);
y1 = (lua_toint(L, 4)-1);
if(n>=5) {page= lua_toint(L, 5);if(page<0 || page >5) page = 0;}   //peut representer la couleur ou un numero de buffer
else page = 0;
dx = abs(x1-x), sx = x<x1 ? 1 : -1;
dy = abs(y1-y), sy = y<y1 ? 1 : -1;

if ( !NetB ) {
   if (n==6 || dx == 0 || dy == 0) {

      if (x>x1) {xx=x ; x=max(x1 ,0) ;x1= min(xx,127);}
      if (y>y1) {xx=y ; y=max (y1,0) ;y1= min (xx,63);}      // donc y<=y1 et x<=x1

         if (n == 6) {            // rectangle
         coul = lua_toint(L,6);
             for( i=y ; i<=y1 ; i++){
              Hline(x, x1 , i , coul);
             }

               if (page!=coul) {
               Hline(x,x1,y,page);
               Vline(x1,y,y1,page);
               Hline(x,x1,y1,page);
               Vline(x,y,y1,page);
               }
         }
         else if (dy==0) Hline(x,x1,y1,page);
         else Vline(x,y,y1,page);              //dx == 0
      return 0;
      }
}
     err = (dx>dy ? dx : -dy)/2;
     for(;;){
     if ((x == x&0x007F) && (y == y&0x003F)) // si on est dans les plages de l'écran (tester si pas de pb pour les valeurs négatives)
       {
          if(NetB) ML_pixel(x,y,1,VRAM[page]);
          else draw_write_pix(x,y,page);
         }
     if (x==x1 && y==y1) break;
     e2 = err;
     if (e2 >-dx) { err -= dy; x += sx; }
     if (e2 < dy) { err += dx; y += sy; }
     }
 if (NetB && page==0)ML_display_vram(VRAM[0]);
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
writevalue(x,y,(long)(lua_tonumber(L, 3)));
break;
default: break;
}

 return 0;
 }
}

// version 'simulant' une sauvegarde. effacé à chaque lancement ( en effet cette fonction sera supprimée si j'arrive à programmer la gestion des fichiers.
float sauveg[3][256]= {0};
static int sav(lua_State *L)       
{
 unsigned short n = lua_gettop(L);
 char param;
 unsigned char a;
 float b;
 param= lua_toint(L,1) -1;
 param = 2*(param);
 a= ((unsigned int)(lua_toint(L,2)-1));
 if (n==3 && param >=0 && param <=4) { //4 = 2*(3-1)
   b= lua_tonumber(L,3);
   sauveg[param][a]=b;
 }
 else if(n==2 && param >=0 && param <=4)
 {
   b = sauveg[param][a];
 lua_pushnumber(L,b);
 return 1;}
 return 0;
}

static const luaL_Reg glib[] = {
      {"letter", letter},
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
      {"save", sav},
       {"misc", misc},
       {"print", print2},
      {NULL, NULL}  /* sentinel */
   };

void luag_init(lua_State *L)
{
 luaI_openlib(L, "g", glib, 0);
}