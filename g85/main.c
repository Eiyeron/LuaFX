/*****************************************************************/
/*                                                               */
/*   CASIO fx-9860G SDK Library                                  */
/*                                                               */
/*   File name : LUAg85.c                                 */
/*                                                               */
/*   Copyright (c) 2006 CASIO COMPUTER CO., LTD.                 */
/*                                                               */
/*****************************************************************/


// main + interface + printf
// à améliorer: interface, printf


#include "fxlib.h"
#include "MonochromeLib.c"
#include "mygray.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "lua.h"
#include "lstate.h"
#include "lauxlib.h"
#include "lvm.h"

#include "ltablib.c"
#include "lbaselib.c"
#include "lstrlib.c"

char* VRAM[6];       // 6 pointeur sur des buffers dont le premier est la VRAM par défaut
unsigned char NetB = 1;
#include "lglib.c"


//****************************************************************************
//  AddIn_main (Sample program main function)
//
//  param   :   isAppli   : 1 = This application is launched by MAIN MENU.
//                        : 0 = This application is launched by a strip in eACT application.
//
//              OptionNum : Strip number (0~3)
//                         (This parameter is only used when isAppli parameter is 0.)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************

unsigned char ndigit;      // remarque: dans les commentaires je parle de ndig, il s'agit en fait de ndigit
int cursx,cursy;    // curseur de printf
extern int ral;

void exit (int a)  { reset_calc() }




#include "float.h"
#include "mathf.h"
typedef unsigned long       DWORD;



// fonction dont je ne donnerait pas les sources. retourne le nb de chiffres avant la virgule/
short int avant_virgule (const float f) ;     // hypothèse f != 0


const float tabajout1[] = { -0.0005 ,0.005,0.05,0.5, 5, 50, 500, 5000, 5e4, 5e5, 5e6, 5e7, 5e8, 5e9, 5e10, 5e11,5e12};
const float tabajout2[] = { 1, 10,1e2, 1e3,1e4,1e5,1e6,1e7,1e8,1e9,1e10,1e11 };


// conversion utilisée par tonumber du lua
// programmé de façon à ce que sprintf respecte %.g comme sur g100 (en effet le sprintf g85 ne respecte pas correctement la règle)
// règle: (nb = +- x * 10^exposant, 0.1 <= x <1   ou nb = 0)
// la string doit respecter ndig nb significatif (avec 0 inutiles enlevés)
// exposant < -3 ou > ndig   (ndig fixé par g.misc, défaut 6 ) = > format exponentiel 
// sinon format normat XXXXXX.XXXXXX  avec ndig nb significatifs et 0 inutiles enlevés )
int lua_number2str(char *s,float f)
{
   DWORD dw = *((DWORD*) &f);
   char exp;
   char point;
   int n;
   float ajout;
   char *p;
   int entrop=0;
   short int decpt;
   ral += 3000;
   if(!NetB) gray_quit();      // sprintf ne supporte pas le mode gris
   if(dw == 0)   {
          s[0] = '0'; s[1]=0;
          return 1;      }

   if ((abs(dw)) != 0x7F800000)  decpt =  avant_virgule(f);  else decpt = ndigit+1;
   if (decpt <-3 || decpt > ndigit) {    // on est sur que dans ce cas il faut un exposant
avec_exp:  n = sprintf(s,"%.*g",(ndigit-1),f);
         if(!NetB) gray_init();
         if (s[0] != '+') return n;      // sprintf affiche +++ au lieu de inf
         else {
          s[0] = 'i';
          s[1] = 'n';
          s[2] = 'f';
          s[3] = 0;
          return 3;

         }
     }
   else {
     ajout = tabajout1[decpt+3]/ tabajout2[ndigit-1];
     if (avant_virgule( abs(f) +ajout)!= decpt && ( (decpt == -3) || (decpt == ndigit))) goto avec_exp;
      n = sprintf(s,"%.*f",(ndigit-decpt),f);
      if(!NetB) gray_init();
      p = s + n -1;
      while (*p == '0') {p--; n --;}     // enlever 0 en trop
      if(*p == '.') *p = 0;
      else { p++;*p=0;n++; }
      return n;
   }

}



// fonctions de scrolling pierrotll

void vertical_scroll(char* buffer, int shift) {
	int i, j;
	char column[64];
	shift %= 64;
	for(i=0 ; i<16 ; i++) {
		for(j=0 ; j<64 ; j++) column[j] = buffer[(j<<4)+i];
		for(j=0 ; j<64 ; j++) buffer[(j<<4)+i] = column[(j-shift+64)&63];
	}
}

void horizontal_scroll(unsigned char* buffer, int shift) {
	int i, j;
	unsigned char line[16], Bshift, next;
	unsigned short word;
	shift %= 128;
	Bshift = 8-(shift&7);
	for(i=0 ; i<64 ; i++) {
		for(j=0 ; j<16 ; j++) line[j] = buffer[(i<<4)+((j-(shift>>3)+15)&15)];
		next = line[15];
		buffer[(i<<4)+15] = 0;
		for(j=15 ; j>0 ; j--) {
			word = next << Bshift;
			next = line[j-1];
			buffer[(i<<4)+j] |= *((char*)&word+1);
			buffer[(i<<4)+j-1] = *((char*)&word);
		}
		word = next << Bshift;
		buffer[(i<<4)] |= *((char*)&word+1);
		buffer[(i<<4)+15] |= *((char*)&word);
	}
}




 // à améliorer pour que les caractères spéciaux sur 2 octets s'affichent sur g100 et g85 de la même manière
 // actuellement sur g100 les caractères spéciaux sont comme sur le document à la fin de 'manuel du programmeur g100 v2' du site gprog.tk
 // il faut faire des tableaux et un système pour que les codes de caractère spéciaux g100 s'affichent sur g85.
 // ainsi on peut acceder à => par exemple.
 
 
 // format est une liste variable d'arguments de type: string ou/et entiers (%d). Jamais de flottants (car à ce niveau déjà converti en string par l'algo lua_number2str)

int printf(const char *format,...)
{
     unsigned char buffer [1000];
     unsigned char one [2]= { 0 , 0};
     int n=6;
     int i;
     n = vsprintf(buffer,format,(va_list)&(format) + sizeof(format));

     for (i=0;i<n;i++) {
     locate(cursx,cursy);
     if(buffer[i] == '\n')  {cursy++;}
     else if (buffer[i] == '\r') {cursx=1;}
    else {
     one[0] = buffer[i];
     PrintC(one);
     ral+=300;
     cursx++;
    }
     if (cursx >21) {cursx = 1; cursy++;}
     if (cursy >8) {cursy = 8;ral+=3000;vertical_scroll(VRAM[0],-8); ML_rectangle(0,63-8,127,63,0,WHITE,WHITE);}   // ral est un élement du ralentissement g85 de lvm.c. pas vraiment mesuré, 3000 est très imprécis.
     Sleep(2);                                                                                                   // affichage plus rapide sur g85 que sur g100.
     }
     ML_display_vram(VRAM[0]);
    return n;
}


// partie pour executer fichier lua

typedef struct LoadF {
	int pointer;
	int handle;
	char buff[LUAL_BUFFERSIZE];
	
} LoadF;

LoadF loadF;

void closeF() {
	Bfile_CloseFile(loadF.handle);
	loadF.handle = -1;
	loadF.pointer = 0;
}

static const char *readF (lua_State *L, void *ud, size_t *size) {
           LoadF *lf = (LoadF *)ud;
	if(lf->handle < 0) return NULL;
	*size = Bfile_ReadFile(lf->handle, lf->buff, LUAL_BUFFERSIZE, lf->pointer);
	if(*size > 0) lf->pointer += *size;
	else *size = 0;
	return (*size > 0 ? lf->buff : NULL);
}

// hypothèse: fichier chargé par l'interface
void Launch(char* name) // name est le nom de la chunk
{
int status=0;
unsigned long i;
unsigned int segment;
unsigned int offset;
lua_State *L;
ML_clear_vram(VRAM[0]);
locate(1,1);
cursy=1;cursx=1;
ndigit=6;
L = lua_open();
if (L == NULL) {printf("cannot create state: not enough memory"); Sleep(2000); Reset_Calc();}


luag_init(L);    // librairie g
luaopen_table(L); // les autres librairies
luaopen_base(L);
luaopen_string(L);
printf("debut du... test !!!!\n\r");
Sleep(2000);
status = lua_load(L, readF, &loadF, name);
closeF();
if(!status) status = lua_pcall(L, 0, -1, 0);
if(!NetB) gray_quit();
ML_clear_vram(VRAM[0]);
if (status) {
cursy=1;cursx=1;
printf("%s", lua_tostring(L, -1));
lua_pop(L, 1); // pop error message from the stack
Sleep(3000);
}
Sleep(1000);

lua_close(L);

}

void draw_bmp_or_cl(unsigned char *bmp, int x, int y, int width, int height, char* buffer)
{
        unsigned short line;
        char shift, *screen, *p;
        int i, j, real_width, begin_x, end_x, begin_y, end_y;
        char bool1=1, bool2=1, bool3;
        if(!bmp || x<1-width || x>127 || y<1-height || y>63 || height<1 || width<1) return;
        p = (char*)&line;
        real_width = (width-1>>3<<3)+8;
        if(y < 0) begin_y = -y;
        else begin_y = 0;
        if(y+height > 64) end_y = 64-y;
        else end_y = height;
        shift = 8-(x&7);
        if(x<0)
        {
                begin_x = -x>>3;
                if(shift != 8) bool1 = 0;
        } else begin_x = 0;
        if(x+real_width > 128) end_x = 15-(x>>3), bool2 = 0;
        else end_x = real_width-1>>3;
        bool3 = (end_x == real_width-1>>3);
        screen = buffer+(y+begin_y<<4)+(x>>3);
 
        for(i=begin_y ; i<end_y ; i++)
        {
                if(begin_x < end_x)
                {
                        line = bmp[i*(real_width>>3)+begin_x] << shift;
                        if(bool1) screen[begin_x] |= *p;
                        if(shift!=8) screen[begin_x+1] |= *(p+1);
                        for(j=begin_x+1 ; j<end_x ; j++)
                        {
                                line = bmp[i*(real_width>>3)+j] << shift;
                                screen[j] |= *p;
                                if(shift!=8) screen[j+1] |= *(p+1);
                        }
                }
                line = bmp[i*(real_width>>3)+end_x];
                if(bool3) line &= -1<<real_width-width;
                line <<= shift;
                if(begin_x < end_x || bool1) screen[end_x] |= *p;
                if(bool2) screen[end_x+1] |= *(p+1);
                screen += 16;
        }
}

// intro gris
 
void intro()
{
 
char buffer1[] = {3,128,0,15,224,0,63,248,0,255,254,0,255,250,0,255,226,0,255,130,0,255,2,0,255,2,0,255,3,128,255,3,224,255,3,248,255,3,254,255,3,250,255,3,226,255,3,130,255,2,2,255,0,2,255,0,6,255,0,24,255,0,96,255,1,128,255,6,0,63,24,0,15,96,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0,0};
char buffer2[] = {3,128,0,12,96,0,48,24,0,192,6,0,240,30,0,252,126,0,255,254,0,255,254,0,255,254,0,255,255,128,255,254,96,255,254,24,255,254,6,255,254,30,255,254,126,255,255,254,255,255,254,255,255,254,255,255,254,255,255,248,255,255,224,255,255,128,255,254,0,63,248,0,15,224,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0,0};
        char image[5][2][90] = {
                {
{3,128,0,15,224,0,63,248,0,255,254,0,255,250,0,255,226,0,255,130,0,255,2,0,255,2,0,255,3,128,255,3,224,255,3,248,255,3,254,255,3,250,255,3,226,255,3,130,255,2,2,255,0,2,255,0,6,255,0,24,255,0,96,255,1,128,255,6,0,63,24,0,15,96,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0,0},
{3,128,0,12,96,0,48,24,0,192,6,0,240,30,0,252,126,0,255,254,0,255,254,0,255,254,0,255,255,128,255,254,96,255,254,24,255,254,6,255,254,30,255,254,126,255,255,254,255,255,254,255,255,254,255,255,254,255,255,248,255,255,224,255,255,128,255,254,0,63,248,0,15,224,0,3,128,0,0,0,0,0,0,0,0,0,0,0,0,0}
                },
                {
{0,3,128,0,15,224,0,63,248,0,63,254,0,63,250,14,63,226,63,191,194,255,255,194,255,255,194,255,239,194,255,143,194,255,15,194,255,15,194,255,15,194,255,15,194,255,15,194,255,15,194,255,15,194,255,15,194,255,15,2,255,12,2,255,0,2,255,0,6,255,0,24,255,0,96,255,1,128,255,6,0,63,24,0,15,96,0,3,128,0},
{0,3,128,0,12,96,0,48,24,0,60,6,0,63,30,14,63,254,49,191,254,192,127,254,240,31,254,252,127,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,254,255,255,248,255,255,224,255,255,128,255,254,0,63,248,0,15,224,0,3,128,0}
                },
                {
{0,3,0,0,15,192,0,63,240,0,255,252,3,255,244,15,255,196,63,255,4,255,252,4,255,240,4,255,192,4,255,0,4,254,1,132,254,7,132,254,31,132,254,31,132,254,31,132,254,30,4,254,24,4,254,0,4,254,1,132,254,7,132,254,31,132,254,31,140,254,23,176,254,17,192,254,16,0,254,16,0,62,48,0,14,192,0,3,0,0}, 
{0,3,0,0,12,192,0,48,48,0,192,12,3,0,60,12,0,252,48,3,252,192,15,252,240,63,252,252,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,255,252,255,247,240,255,241,192,255,240,0,255,240,0,63,240,0,15,192,0,3,0,0}
                },
                {
{0,3,0,0,15,192,0,63,240,0,255,252,3,255,252,15,255,228,63,255,132,255,254,4,255,248,4,255,224,24,255,128,96,255,1,128,255,7,0,255,7,192,255,7,240,255,7,208,255,7,16,255,4,16,255,0,16,255,0,96,255,1,128,255,6,0,255,4,0,255,4,0,255,4,0,255,4,0,255,4,0,63,24,0,15,96,0,3,128,0}, 
{0,3,0,0,12,192,0,48,48,0,192,12,3,0,28,12,0,124,48,1,252,192,7,252,240,31,252,252,127,248,255,255,224,255,255,128,255,255,0,255,252,192,255,252,48,255,252,240,255,255,240,255,255,240,255,255,240,255,255,224,255,255,128,255,254,0,255,252,0,255,252,0,255,252,0,255,252,0,255,252,0,63,248,0,15,224,0,3,128,0}
                },
                {
{0,3,128,0,15,224,0,63,248,0,63,254,0,63,250,14,63,226,63,191,194,255,255,194,255,255,194,255,239,194,255,143,196,255,15,196,255,15,196,127,15,196,127,15,132,127,6,4,127,128,2,127,128,2,127,129,130,127,135,194,127,143,194,255,143,194,255,143,198,255,11,216,255,8,224,255,8,0,255,8,0,63,24,0,15,96,0,3,128,0},
{0,3,128,0,12,96,0,48,24,0,60,6,0,63,30,14,63,254,49,191,254,192,127,254,240,31,254,252,127,254,255,255,252,255,255,252,255,255,252,127,255,252,127,255,252,127,255,252,127,255,254,127,255,254,127,255,254,127,255,254,127,255,254,255,255,254,255,255,254,255,251,248,255,248,224,255,248,0,255,248,0,63,248,0,15,224,0,3,128,0}
                }
        };
        int coord[5][4] = {{5,11,-20,0}, {30,19,-18,0}, {54,31,-16,0}, {77,19,-14,0}, {99,8,-12,0}};
        int i, j, dy, y;char * temp;
        gray_init();
        for(dy=-20 ; dy<20 ; dy++) {
                gray_clearBuffers();
                for(i=0 ; i<5 ; i++) {
                        y = 0;
                        if(coord[i][2] < 0) {
                                for(j=-1 ; j>= coord[i][2] ; j--) y += j;
                        } else if(coord[i][2] > 0) {
                                for(j=1 ; j<= coord[i][2] ; j++) y += j;
                        } else {
                                coord[i][3]++;
                                if(coord[i][3] > 10) coord[i][2] = 1;
                        }
                        if(coord[i][2]) coord[i][2]++;
                        draw_bmp_or_cl(image[i][0], coord[i][0], coord[i][1]+y, 23, 30, gray_getScreen()->VRAM1);
                        draw_bmp_or_cl(image[i][1], coord[i][0], coord[i][1]+y, 23, 30, gray_getScreen()->VRAM2);
                }
                gray_refresh();
                for(i=0 ; i<300000 ; i++);
        }
        gray_quit();
}
// interface de louloux avec fin de veb 
void choixprog_execute()
{
        int reussiteouverture;
        unsigned char i;
        unsigned char j;
        unsigned char modeopen;
        unsigned int attente;
        int action;
        int pourfich;
        unsigned int numerofich = 1;
        char prog[13];
        unsigned char Luainterface[1024] =  { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
        255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,252,0,0,0,0,0,0,63,255,255,255,128,
	   255,255,255,255,252,0,0,0,0,0,0,63,255,255,255,128,
	   255,255,255,255,252,0,0,0,0,0,0,63,255,255,255,128,
	   255,255,255,255,252,0,0,0,0,0,0,63,255,255,255,128,
	   255,255,255,255,252,0,0,0,0,0,0,63,255,255,255,128,
	   255,255,255,255,252,0,0,0,0,0,0,63,255,255,255,128,
	   255,255,255,255,252,0,0,0,0,0,0,63,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,248,15,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,252,31,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,254,63,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,127,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,207,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,135,135,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,254,1,135,255,255,128,
	   255,255,255,251,255,255,255,255,255,255,252,0,207,255,255,128,
	   255,255,255,251,255,255,255,255,255,255,248,6,127,255,255,128,
	   255,255,255,251,255,255,143,255,227,7,240,15,63,255,255,128,
	   255,255,255,251,255,255,127,255,221,127,240,15,63,255,255,128,
	   255,255,255,251,255,255,127,255,221,127,224,6,31,255,255,128,
	   255,255,255,251,238,143,31,127,227,15,224,0,31,255,255,128,
	   255,255,255,251,238,247,114,252,221,247,224,0,31,255,255,128,
	   255,255,255,251,238,199,125,251,93,247,224,0,31,255,255,128,
	   255,255,255,251,236,183,122,251,93,247,240,0,63,255,255,128,
	   255,255,255,248,34,139,115,124,99,15,240,0,63,255,255,128,
	   255,255,255,255,255,255,255,255,127,255,248,0,127,255,255,128,
	   255,255,255,255,255,255,255,252,63,255,252,0,255,255,255,128,
	   255,255,255,255,255,255,255,251,127,255,254,1,255,255,255,128,
	   255,255,255,255,255,255,255,251,127,255,255,135,255,255,255,128,
	   255,255,255,255,255,255,255,252,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128,
	   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,128 };
            FONTCHARACTER FICHIER[13] = {'\\','\\',' ',' ',' ','0','\\','*','.','l','u','a',0};
            FONTCHARACTER progexe[20] = {'\\','\\',' ',' ',' ','0','\\',' ',' ',' ',' ',' ',' ',' ',' ','.','l','u','a',0};
            FONTCHARACTER pourbuff[13];
            FILE_INFO lastruct;
            modeopen = 0;
            do {
            PrintMini(28,24,"F1: memoire stockage",MINI_OR);
            PrintMini(28,36,"F2: carte SD",MINI_OR);
            Bdisp_PutDisp_DD();
            if( IsKeyDown(KEY_CTRL_F1) == 1 )
            {
                  modeopen = 1;
                  FICHIER[2] = 'f';
                  FICHIER[3] = 'l';
                  FICHIER[4] = 's'; 
                  progexe[2] = 'f';
                  progexe[3] = 'l';
                  progexe[4] = 's'; 
            }
            if( IsKeyDown(KEY_CTRL_F2) == 1 )
            {
                  modeopen = 2;
                  FICHIER[2] = 'c';
                  FICHIER[3] = 'r';
                  FICHIER[4] = 'd'; 
                  progexe[2] = 'c';
                  progexe[3] = 'r';
                  progexe[4] = 'd';
             }
        } while( modeopen == 0 );
        Bdisp_AllClr_DDVRAM();
        action = Bfile_FindFirst(FICHIER, &pourfich, pourbuff, &lastruct);
        if( action == 0 ) {
                do {
                        PrintMini(2,2,"choix du programme a executer",MINI_OR);
                        ML_bmp_or_cl(Luainterface, 1, 1, 127, 63);
                        for( i = 0; i < 13; i++ ) {
                                prog[i] = pourbuff[i];
                        }
                        PrintMini(40,24,prog,MINI_OR);
                        if( IsKeyDown(KEY_CTRL_DOWN) == 1 ) {
                                action = Bfile_FindNext(pourfich, pourbuff, &lastruct);
                                if( action != 0 ) {
                                       Bfile_FindClose(pourfich);
                                       action = Bfile_FindFirst(FICHIER, &pourfich, pourbuff, &lastruct);
                                }
                                Sleep(500);
                                Bdisp_AllClr_DDVRAM();
                       }
                      Bdisp_PutDisp_DD(); 
                   } while( IsKeyDown(KEY_CTRL_EXE) != 1 );
        } else { 
                PopUpWin(5);
                locate(4,4);
                Print("Pas de prog lua");
                Sleep(3000);
                reset_calc();
        }
        Bfile_FindClose(pourfich);
        for( i = 0; i < 13; i++)
        {
              j = i + 7;
              progexe[j] = pourbuff[i];
        }
        reussiteouverture = Bfile_OpenFile(progexe,_OPENMODE_READ);
        if( reussiteouverture <= 0 )
        {
              PopUpWin(5);
              locate(6,3);
              Print("L'ouverture");
              locate(7,4);
              Print("a echoue");
              Bdisp_PutDisp_DD();
              Sleep(3000);
              reset_calc();
         }
         loadF.pointer = 0;     // preparation pour Luanch : ouverture du fichier
	loadF.handle = reussiteouverture;
         Launch(progexe);
}

int IsEmulator()
{
	return !( *(int*)0x80000300 ); 
}

int AddIn_main(int isAppli,unsigned short OptionNum)
{
 unsigned long i;
VRAM[0]= ML_vram_adress();           // 1 VRAM existe déjà
ML_clear_vram(VRAM[0]);

for (i=1;i<=5;i++) {
  VRAM[i] = malloc( 0x400);        //  création des 5 autres buffers VRAM
  if (VRAM[i] == NULL ) {printf("cannot create state: not enough memory"); Sleep(2000); Reset_Calc();}
}
        intro();

        choixprog_execute();

        Reset_Calc();
        return 1;
}
 
//****************************************************************************
//**************                                              ****************
//**************                 Notice!                      ****************
//**************                                              ****************
//**************  Please do not change the following source.  ****************
//**************                                              ****************
//****************************************************************************
 
 
#pragma section _BR_Size
unsigned long BR_Size;
#pragma section
 
 
#pragma section _TOP
 
//****************************************************************************
//  InitializeSystem
//
//  param   :   isAppli   : 1 = Application / 0 = eActivity
//              OptionNum : Option Number (only eActivity)
//
//  retval  :   1 = No error / 0 = Error
//
//****************************************************************************
int InitializeSystem(int isAppli, unsigned short OptionNum)
{
    return INIT_ADDIN_APPLICATION(isAppli, OptionNum);
}

#pragma section