#include "mygray.h"
#include "MonochromeLib.h"
#include <stdlib.h>
#include <string.h>                  // pour strlen()
#include <ctype.h>                   // pour islower(), toupper();

#define INT_OFFSET 0x600        // car vbr + 600 = code interruption utilisateur (stupide hein! ils aurait pas pu faire vbr + 600 = pointeur sur l'interruption? ça aurait évité des bugs)

#define set_vbr(base)		_builtin_set_vbr(base)


struct st_tmu {
			  union {
					unsigned char BYTE;
					struct {
						   unsigned char	 :5;
						   unsigned char STR2:1;
						   unsigned char STR1:1;
						   unsigned char STR0:1;
						   }	  BIT;
					}		   TSTR;
};
struct st_tmu0 {
			   unsigned int	 TCOR;
			   unsigned int	 TCNT;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short	 :7;
							unsigned short UNF :1;
							unsigned short	 :2;
							unsigned short UNIE:1;
							unsigned short CKEG:2;
							unsigned short TPSC:3;
							}	   BIT;
					 }		  TCR;
};
struct st_intc {
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short NMIL:1;
							unsigned short	 :6;
							unsigned short NMIE:1;
							}	   BIT;
					 }		  ICR0;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short _TMU0:4;
							unsigned short _TMU1:4;
							unsigned short _TMU2:4;
							unsigned short _RTC :4;
							}	   BIT;
					 }		  IPRA;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short _WDT:4;
							unsigned short _REF:4;
							}	   BIT;
					 }		  IPRB;
			   char			 wk[234];
			   unsigned int	 TRA;
			   unsigned int	 EXPEVT;
			   unsigned int	 INTEVT;
};
struct st_intx {
			   unsigned int	 INTEVT2;
			   union {
					 unsigned char BYTE;
					 struct {
							unsigned char PINT0R:1;
							unsigned char PINT1R:1;
							unsigned char IRQ5R :1;
							unsigned char IRQ4R :1;
							unsigned char IRQ3R :1;
							unsigned char IRQ2R :1;
							unsigned char IRQ1R :1;
							unsigned char IRQ0R :1;
							}	  BIT;
					 }		  IRR0;
			   char			 wk1;
			   union {
					 unsigned char BYTE;
					 struct {
							unsigned char TXI0R:1;
							unsigned char	  :1;
							unsigned char RXI0R:1;
							unsigned char ERI0R:1;
							unsigned char DEI3R:1;
							unsigned char DEI2R:1;
							unsigned char DEI1R:1;
							unsigned char DEI0R:1;
							}	  BIT;
					 }		  IRR1;
			   char			 wk2;
			   union {
					 unsigned char BYTE;
					 struct {
							unsigned char	  :3;
							unsigned char ADIR :1;
							unsigned char TXI2R:1;
							unsigned char	  :1;
							unsigned char RXI2R:1;
							unsigned char ERI2R:1;
							}	  BIT;
					 }		  IRR2;
			   char			 wk3[7];
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short MAI   :1;
							unsigned short IRQLVL:1;
							unsigned short BLMSK :1;
							unsigned short	   :1;
							unsigned short IRQ5S :2;
							unsigned short IRQ4S :2;
							unsigned short IRQ3S :2;
							unsigned short IRQ2S :2;
							unsigned short IRQ1S :2;
							unsigned short IRQ0S :2;
							}	   BIT;
					 }		  ICR1;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short PINT15S:1;
							unsigned short PINT14S:1;
							unsigned short PINT13S:1;
							unsigned short PINT12S:1;
							unsigned short PINT11S:1;
							unsigned short PINT10S:1;
							unsigned short PINT9S :1;
							unsigned short PINT8S :1;
							unsigned short PINT7S :1;
							unsigned short PINT6S :1;
							unsigned short PINT5S :1;
							unsigned short PINT4S :1;
							unsigned short PINT3S :1;
							unsigned short PINT2S :1;
							unsigned short PINT1S :1;
							unsigned short PINT0S :1;
							}	   BIT;
					 }		  ICR2;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short PINT15E:1;
							unsigned short PINT14E:1;
							unsigned short PINT13E:1;
							unsigned short PINT12E:1;
							unsigned short PINT11E:1;
							unsigned short PINT10E:1;
							unsigned short PINT9E :1;
							unsigned short PINT8E :1;
							unsigned short PINT7E :1;
							unsigned short PINT6E :1;
							unsigned short PINT5E :1;
							unsigned short PINT4E :1;
							unsigned short PINT3E :1;
							unsigned short PINT2E :1;
							unsigned short PINT1E :1;
							unsigned short PINT0E :1;
							}	   BIT;
					 }		  PINTER;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short _IRQ3:4;
							unsigned short _IRQ2:4;
							unsigned short _IRQ1:4;
							unsigned short _IRQ0:4;
							}	   BIT;
					 }		  IPRC;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short _PINT0_7 :4;
							unsigned short _PINT8_15:4;
							unsigned short _IRQ5	:4;
							unsigned short _IRQ4	:4;
							}	   BIT;
					 }		  IPRD;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short _DMAC :4;
							unsigned short _SCIF0:4;
							unsigned short _SCIF2:4;
							unsigned short _ADC  :4;
							}	   BIT;
					 }		  IPRE;
			   char			 wk4[524260];
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short	 :8;
							unsigned short _USB:4;
							}	   BIT;
					 }		  IPRF;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short _TPU0:4;
							unsigned short _TPU1:4;
							}	   BIT;
					 }		  IPRG;
			   union {
					 unsigned short WORD;
					 struct {
							unsigned short _TPU2:4;
							unsigned short _TPU3:4;
							}	   BIT;
					 }		  IPRH;
};

#define INTC   (*(volatile struct st_intc  *)0xFFFFFEE0)
#define INTX   (*(volatile struct st_intx  *)0xA4000000)
#define TMU	(*(volatile struct st_tmu   *)0xFFFFFE92)
#define TMU0   (*(volatile struct st_tmu0  *)0xFFFFFE94)


Screen* screen = NULL;
extern char* VRAM[6];   // voir main.c pour l'initialisation

#pragma interrupt(gray_interrupt)

void gray_interrupt()
{
	if(screen->which == 1)
	{
		draw_all(screen->DD1);
		screen->which = 2;
		TMU.TSTR.BIT.STR0 = 0;
		TMU0.TCNT = TMU0.TCOR = screen->time1;
	} else {
		draw_all(screen->DD2);
		screen->which = 1;
		TMU.TSTR.BIT.STR0 = 0;
		TMU0.TCNT = TMU0.TCOR = screen->time2;
	}
	TMU0.TCR.BIT.UNF = 0;
	TMU.TSTR.BIT.STR0 = 1;
}
void* oldvbr;   // pour tout remettre en place
int gray_init()
{
	screen = malloc(sizeof(Screen));
	if(!screen) return 0;
	screen->VRAM1 = VRAM[0];
	screen->VRAM2 = VRAM[1];
	screen->DD1 = VRAM[2];
	screen->DD2 = VRAM[3];
	screen->time1 = 3269;
	screen->time2 = 6987;
	screen->which = 1;
	oldvbr = _builtin_get_vbr();
	set_vbr((void *)((unsigned int)gray_interrupt - INT_OFFSET));
	TMU0.TCR.WORD = 0x0022;
	TMU0.TCNT = TMU0.TCOR = screen->time1;
	INTC.IPRA.WORD = 0xf000;
	INTC.IPRB.WORD = 0x0000;
	INTX.IPRC.WORD = 0x0000;
	INTX.IPRD.WORD = 0x0000;
	INTX.IPRE.WORD = 0x0000;
	INTX.IPRF.WORD = 0x0000;
	INTX.IPRG.WORD = 0x0000;
	INTX.IPRH.WORD = 0x0000;
	TMU.TSTR.BIT.STR0 = 1;
	return 1;
}

void gray_quit()
{
	set_vbr(oldvbr);         // on remet en place le vbr (et donc les interrutpions casio utilisées par certaines fonctions comme sprintf)
        TMU0.TCR.WORD = 0x0000;
	TMU0.TCNT = TMU0.TCOR = 0xFFFFFFFF;
	INTC.IPRA.WORD = 0x0000;
	TMU.TSTR.BIT.STR0 = 0;
	TMU.TSTR.BIT.STR1 = 0;
	TMU.TSTR.BIT.STR2 = 0;
	free(screen);
}

void gray_clearBuffers()
{

        ML_clear_vram( screen->VRAM1);
	ML_clear_vram( screen->VRAM2);
}

void gray_refresh()
{
	memcpy(screen->DD1,screen->VRAM1,1024);
        memcpy(screen->DD2,screen->VRAM2,1024);
}

void gray_state(int mode)
{
	if (mode) {
         memcpy(screen->VRAM1,VRAM[4],1024);   // charger
         memcpy(screen->VRAM2,VRAM[5],1024);
        }
        else
	{
         memcpy(VRAM[4],screen->VRAM1,1024);     // sauvegarde
         memcpy(VRAM[5],screen->VRAM2,1024);
        }
}

// fonctions graphiques de vebveb (dont certaines tirées de fonctions pierrotll et adaptées)

// 0: blanc
// 1 : gris clair
// 2 : gris moyen  ( gris foncé sur g85)
// 3: gris foncé   ( soit gris foncé soit noir sur g85)
// 4 : noir
// 5 : inversion de couleur

// -> un contraste adapté fait que toutes les couleurs (sauf 3) seront pareil sur g100 et g85.

int draw_read_pix(int x, int y)
{
	char color;
	color = (screen->VRAM1[(y<<4)+(x>>3)]&128>>(x&7) ? 1 : 0);
	color |= (screen->VRAM2[(y<<4)+(x>>3)]&128>>(x&7) ? 2 : 0);
	if (color == 3) color = 4; // noir
	return color;
}

void draw_write_pix(int x, int y, int color)
{
	int endroit = (y<<4)+(x>>3);
	if (color == 5) { color = 4-draw_read_pix(x,y);};// inverser la couleur
        if(color < 0) color = 0;
	else if(color == 3) color = (((x+y)%2)==1 ? 3 : 2);   // en fonction de x et y on choisi la couleur adaptée
        else if(color >= 4) color = 3;

	if(color & 1) screen->VRAM1[endroit] |= 128>>(x&7);
	else screen->VRAM1[endroit] &= ~(unsigned char)(128>>(x&7));
	if(color & 2) screen->VRAM2[endroit] |= 128>>(x&7);
	else screen->VRAM2[endroit] &= ~(unsigned char)(128>>(x&7));
}

 typedef union convert2{
  unsigned short int bloc[4];
    struct  {
    unsigned long gbloc1;
    unsigned long gbloc2;
    } gbloc;
  } convert2;

typedef  union convert3{
  unsigned char bloc[12];
    struct  {
    unsigned long gbloc1;
    unsigned long gbloc2;
    unsigned long gbloc3;
    } gbloc;
  } convert3;

 // décompresser et afficher un sprite codé par spritemaker (sources dispo sur le site de orwell)
void sprite(unsigned char *sprite, int x, int y)
{
	char largeur, hauteur, origx, origy; int beginy,endy;
	unsigned char* Code;char* screen1;char* screen2;
	origx=sprite[0] ;
	origy=sprite[1];
	hauteur=sprite[2] ;
	largeur=sprite[3] ;
	Code = sprite+4;
	x -= origx;
        y -= origy;
        if (y<0) beginy = 0; 
           else beginy = y;
        if (y>63) goto fin;
        if ((y+hauteur) >63) endy = 63;
           else endy = y+hauteur;
      while (largeur != 0) {
         if (largeur > 8) {   unsigned long masque,clippingmasque,line1,line2;unsigned char shift; int y1;int j;struct convert2 resultat; struct convert3 lecteur;
                                 Code += (beginy-y)*6;
                                 if (x <= -16) {Code += ((y+hauteur)-beginy)*6;;goto continuer16;}
                                 if (x > 127) goto fin;
                                 shift = x&7;
	                         if(x < 0) {
                                           if (x<=-8) clippingmasque = 0x0000FF00;
                                           else clippingmasque = 0x00FFFF00;
                                           }
                                 else if (x>(127-16)) {
                                                       if(x>(127-8))clippingmasque = 0xFF000000;
                                                       else clippingmasque = 0xFFFF0000;

                                                      }
                                 else clippingmasque = 0xFFFFFF00;
	                         // calcul de screen1, screen2
	                         screen1 = screen->VRAM1 + beginy*16  + x/8 ;
	                         screen2 = screen->VRAM2 + beginy*16  + x/8 ;
                                  for(y1=beginy ; y1<endy ; y1++)
	                          {

                                        //lecteur.gbloc.gbloc1 = ((unsigned long)Code[0])<<8+Code[1];
                                        //lecteur.gbloc.gbloc2 = ((unsigned long)Code[2])<<8+Code[3];
                                        //lecteur.gbloc.gbloc3 = ((unsigned long)Code[4])<<8+Code[5];
                                        lecteur.gbloc.gbloc1 = 0;
                                        lecteur.gbloc.gbloc2 = 0;
                                        lecteur.gbloc.gbloc3 = 0;
                                        lecteur.bloc[2] = Code[1];
                                        lecteur.bloc[3] = Code[0];
                                        lecteur.bloc[6] = Code[3];
                                        lecteur.bloc[7] = Code[2];
                                        lecteur.bloc[10] = Code[5];
                                        lecteur.bloc[11] = Code[4];
                                        Code += 6;
                                        // rq : transparent = (~lecteur.gbloc.gbloc1)&(~lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3);
                                        masque = (~((lecteur.gbloc.gbloc1 | lecteur.gbloc.gbloc2 | lecteur.gbloc.gbloc3) <<16) )>>shift; // masque = ~transparent + decalage
                                        lecteur.gbloc.gbloc1 = ~lecteur.gbloc.gbloc1; // retabli ordre instinctif
                                        resultat.gbloc.gbloc1 = ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) ) |  ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3) )  |  ( (~lecteur.gbloc.gbloc1)&(~lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) ) ;
                                        resultat.gbloc.gbloc2 = ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) ) |  ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3) )  |  ( (~lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) )  |  ( (~lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3) )  |  ( (lecteur.gbloc.gbloc1)&(~lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) );
                                        line1 = (resultat.gbloc.gbloc1)<<(16-shift);
                                        line2 = (resultat.gbloc.gbloc2)<<(16-shift);

                                        masque |= ~clippingmasque;
                                        line1  &= clippingmasque;
                                        line2  &= clippingmasque;

                                        for (j = 0;j<=3;j++)  {
                                         screen1[j] &= ((unsigned char*) &masque)[j];
                                         screen2[j] &= ((unsigned char*) &masque)[j];
                                         screen1[j] |= ((unsigned char*) &line1)[j];
                                         screen2[j] |= ((unsigned char*) &line2)[j];
                                        }

                                        /*((unsigned long *)screen1)[0] &= masque;
                                        ((unsigned long *)screen2)[0] &= masque;
                                        ((unsigned long *)screen1)[0] |= line1;
                                        ((unsigned long *)screen2)[0] |= line2;*/


	                              	screen1 += 16;
	                              	screen2 += 16;
	                            }

                                 Code += ((y+hauteur)-endy)*6;
                                 continuer16:
                                 largeur = *Code;
                                 Code++;
                                 x += 16;
                                }
       else
                              {
                                 unsigned long masque,clippingmasque,line1,line2; unsigned char shift; int y1;int j;  struct convert2 resultat;struct convert3 lecteur;
                                 Code += (beginy-y)*3;
                                 if (x <= -8) {Code += (endy-beginy)*3;goto continuer8;}
                                 if (x > 127) goto fin;
                                 shift = x&7;
                                 clippingmasque = 0xFF000000>>shift;
	                         if(x < 0) {
                                           clippingmasque &= 0x00FF0000;
                                           }
                                 else if (x>(127-8)) clippingmasque &= 0xFF000000 ;
	                         // calcul de screen1, screen2
	                         screen1 = screen->VRAM1 + beginy*16  + x/8 ;
	                         screen2 = screen->VRAM2 + beginy*16  + x/8 ;
                                  for(y1=beginy ; y1<endy ; y1++)
	                          {
                                        lecteur.gbloc.gbloc2 = Code[0] ; // conversion correcte?
                                        lecteur.gbloc.gbloc3 = Code[1];
                                        lecteur.gbloc.gbloc1 = Code[2];
                                        Code += 3;
                                        // rq : transparent = (~lecteur.gbloc.gbloc1)&(~lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3);
                                        masque = (~((lecteur.gbloc.gbloc1 | lecteur.gbloc.gbloc2 | lecteur.gbloc.gbloc3) <<24) )>>shift; // masque = ~transparent + decalage
                                        lecteur.gbloc.gbloc1 = ~lecteur.gbloc.gbloc1; // retabli ordre instinctif
                                        resultat.gbloc.gbloc1 = ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) ) |  ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3) )  |  ( (~lecteur.gbloc.gbloc1)&(~lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) ) ;
                                        resultat.gbloc.gbloc2 = ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) ) |  ( (lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3) )  |  ( (~lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) )  |  ( (~lecteur.gbloc.gbloc1)&(lecteur.gbloc.gbloc2)&(~lecteur.gbloc.gbloc3) )  |  ( (lecteur.gbloc.gbloc1)&(~lecteur.gbloc.gbloc2)&(lecteur.gbloc.gbloc3) );
                                        line1 = (resultat.gbloc.gbloc1)<<(24-shift);
                                        line2 = (resultat.gbloc.gbloc2)<<(24-shift);

                                        masque |= ~clippingmasque;
                                        line1  &= clippingmasque;
                                        line2  &= clippingmasque;

                                        for (j = 0;j<=3;j++)  {
                                         screen1[j] &= ((unsigned char*) &masque)[j];
                                         screen2[j] &= ((unsigned char*) &masque)[j];
                                         screen1[j] |= ((unsigned char*) &line1)[j];
                                         screen2[j] |= ((unsigned char*) &line2)[j];
                                        }

                                        /*((unsigned long *)screen1)[0] &= masque;
                                        ((unsigned long *)screen2)[0] &= masque;
                                        ((unsigned long *)screen1)[0] |= line1;
                                        ((unsigned long *)screen2)[0] |= line2; */


	                              	screen1 += 16;
	                              	screen2 += 16;
	                            }

                                 Code += ((y+hauteur)-endy)*3;
                                 continuer8:
                                 largeur = *Code;
                                 Code++;
                                 x+=8;
                                
                                
                              }

        
        
       }

fin:
return;
}

Screen* gray_getScreen()
{
	return screen;
}

void horizontal_line(char* buffer,int y, int x1, int x2, ML_Color color)
{
    int i;
    char checker;
    char* vram = buffer;
    if(y<0 || y>63 || (x1<0 && x2<0) || (x1>127 && x2>127)) return;
    if(x1 > x2)
    {
    	i = x1;
    	x1 = x2;
    	x2 = i;
    }
    if(x1 < 0) x1 = 0;
    if(x2 > 127) x2 = 127;
    switch(color)
    {
    	case BLACK:
			if(x1>>3 != x2>>3)
			{
				vram[(y<<4)+(x1>>3)] |= 255 >> (x1&7);
				vram[(y<<4)+(x2>>3)] |= 255 << 7-(x2&7);
				for(i=(x1>>3)+1 ; i<x2>>3 ; i++)
					vram[(y<<4) + i] = 255;
			}
			else vram[(y<<4)+(x1>>3)] |= (255>>(x1%8 + 7-x2%8))<<(7-(x2&7));
			break;
		case WHITE:
			if(x1>>3 != x2>>3)
			{
				vram[(y<<4)+(x1>>3)] &= 255 << 8-(x1&7);
				vram[(y<<4)+(x2>>3)] &= 255 >> 1+(x2&7);
				for(i=(x1>>3)+1 ; i<x2>>3 ; i++)
					vram[(y<<4) + i] = 0;
			}
			else vram[(y<<4)+(x1>>3)] &= (255<<8-(x1&7)) | (255>>1+(x2&7));
			break;

    }
}

void vertical_line(char * buffer,int x, int y1, int y2, ML_Color color)
{
    int i, j;
    char checker, byte, *vram = buffer;
	if(x<0 || x>127 || (y1<0 && y2<0) || (y1>63 && y2>63)) return;
	if(y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	if(y1 < 0) y1 = 0;
	if(y2 > 63) y2 = 63;

	i = (y1<<4)+(x>>3);
	j = (y2<<4)+(x>>3);
	switch(color)
	{
		case BLACK:
			byte = 128>>(x&7);
			for( ; i<=j ; i+=16)
				vram[i] |= byte;
			break;
		case WHITE:
			byte = ~(128>>(x&7));
			for( ; i<=j ; i+=16)
				vram[i] &= byte;
			break;

	}
}

void Vline(int x, int y1, int y2, int color)
{
    switch (color){
     case 0: vertical_line(screen->VRAM1,x,y1,y2,0);vertical_line(screen->VRAM2,x,y1,y2,0);  break;
     case 1: vertical_line(screen->VRAM1,x,y1,y2,1);vertical_line(screen->VRAM2,x,y1,y2,0);  break;
     case 2: vertical_line(screen->VRAM1,x,y1,y2,0);vertical_line(screen->VRAM2,x,y1,y2,1);  break;
     case 4: vertical_line(screen->VRAM1,x,y1,y2,1);vertical_line(screen->VRAM2,x,y1,y2,1);  break;
     case 3:    // pas de truc adapté pour couleur 3 et 5 -> passer par pix/pix
     case 5:{  int ym = y1; int ymi = y2;int y;
               for (y = ym; y<=ymi; y++) draw_write_pix(x,y,color);



             break;
    }
     default: return;
    }
}

void Hline(int x1, int x2, int y, int color)
{
    switch (color){
     case 0: horizontal_line(screen->VRAM1,y,x1,x2,0);horizontal_line(screen->VRAM2,y,x1,x2,0);  break;
     case 1: horizontal_line(screen->VRAM1,y,x1,x2,1);horizontal_line(screen->VRAM2,y,x1,x2,0);  break;
     case 2: horizontal_line(screen->VRAM1,y,x1,x2,0);horizontal_line(screen->VRAM2,y,x1,x2,1);  break;
     case 4: horizontal_line(screen->VRAM1,y,x1,x2,1);horizontal_line(screen->VRAM2,y,x1,x2,1);  break;
     case 3:
     case 5: {
      int xm = x1; int xmi = x2;int x;
               for (x = xm; x<=xmi; x++) draw_write_pix(x,y,color);



             break;
     }
     default: return;

    }
}

extern void vertical_scroll(char* buffer, int shift) ;

extern void horizontal_scroll(unsigned char* buffer, int shift);


void scrollx(char x,unsigned char coul)
{

  if(abs(x)>8 || x==0) return;
  horizontal_scroll((unsigned char*)screen->VRAM1,(int)x);
  horizontal_scroll((unsigned char*)screen->VRAM2,(int)x);
  if (coul <= 5) {
  if (x<0)
  {
     x=-x;
     x=x-1;
     for(;x>=0;x--)Vline(127-x,0,63,coul);
  }
  else {
    x--;
     for(;x>=0;x--)Vline(x,0,63,coul);
  }
  }

}

void scrolly(char y,unsigned char coul)
{
   if (abs(y)>8 || y==0)  return;
   vertical_scroll(screen->VRAM1,(int)y);
   vertical_scroll(screen->VRAM2,(int)y);
   if (coul <= 5) {
   if (y<0)
   {
      y=-y;
      y=y-1;
      for(;y>=0;y--)Hline (0,127,63-y,coul);
   }
   else{
      y--;
      for(;y>=0;y--)Hline(0,127,y,coul);
   }
   }

}

void draw_bmp(unsigned char* bmp)
{
  int i;
 for (i = 0;i<=1023;i++)
 {
  unsigned char b1,b2,b3,c1,c2;
  b1 = bmp[i];
  b2 = bmp[1024+i];
  b3 = bmp[i+ 2048];
  c1 = ( b1&b2&b3 ) | (b1&b2&~b3) | (~b1&~b2&b3);
  c2 = ( b1&b2&b3 ) | (b1&b2&~b3) | (~b1&b2&b3) | (~b1&b2&~b3) | (b1&~b2&b3);
  (screen->VRAM1)[i] = c1;
  (screen->VRAM2)[i] = c2;
 }

}

// extrait de gxstring.c amélioré pour lua g100   adapté pour g85

static const unsigned char zero[] = {
0,0,5,4,96,96,0,144,144,0,144,144,0,144,144,0,
96,96,0,0 };

static const unsigned char un[] = {
0,0,5,4,32,32,0,96,96,0,32,32,0,32,32,0,
32,32,0,0 };

static const unsigned char deux[] = {
0,0,5,4,96,96,0,144,144,0,32,32,0,64,64,0,
240,240,0,0 };

static const unsigned char trois[] = {
0,0,5,4,96,96,0,144,144,0,32,32,0,144,144,0,
96,96,0,0 };

static const unsigned char quatre[] = {
0,0,5,4,32,32,0,96,96,0,160,160,0,240,240,0,
32,32,0,0 };

static const unsigned char cinq[] = {
0,0,5,4,240,240,0,128,128,0,224,224,0,16,16,0,
224,224,0,0 };

static const unsigned char six[] = {
0,0,5,4,96,96,0,128,128,0,224,224,0,144,144,0,
96,96,0,0 };

static const unsigned char sept[] = {
0,0,5,4,240,240,0,16,16,0,32,32,0,64,64,0,
64,64,0,0 };

static const unsigned char huit[] = {
0,0,5,4,96,96,0,144,144,0,96,96,0,144,144,0,
96,96,0,0 };

static const unsigned char neuf[] = {
0,0,5,4,96,96,0,144,144,0,112,112,0,16,16,0,
96,96,0,0 };

static const unsigned char deuxpt[] = {
0,0,5,4,96,96,0,96,96,0,0,0,0,96,96,0,
96,96,0,0 };

static const unsigned char ptvirgule[] = {
0,0,5,4,96,96,0,0,0,0,96,96,0,32,32,0,
64,64,0,0 };

static const unsigned char inf[] = {
0,0,5,4,16,16,0,32,32,0,64,64,0,32,32,0,
16,16,0,0 };

static const unsigned char egal[] = {
0,0,5,4,0,0,0,240,240,0,0,0,0,240,240,0,
0,0,0,0 };

static const unsigned char sup[] = {
0,0,5,4,128,128,0,64,64,0,32,32,0,64,64,0,
128,128,0,0 };

static const unsigned char ptint[] = {
0,0,5,4,96,96,0,144,144,0,32,32,0,0,0,0,
32,32,0,0 };

static const unsigned char arobase[] = {
0,0,5,4,112,112,0,16,16,0,240,240,0,144,144,0,
240,240,0,0 };

static const unsigned char A[] = {
0,0,5,4,96,96,0,144,144,0,240,240,0,144,144,0,
144,144,0,0 };

static const unsigned char B[] = {
0,0,5,4,224,224,0,144,144,0,224,224,0,144,144,0,
224,224,0,0 };

static const unsigned char C[] = {
0,0,5,4,96,96,0,144,144,0,128,128,0,144,144,0,
96,96,0,0 };

static const unsigned char D[] = {
0,0,5,4,224,224,0,144,144,0,144,144,0,144,144,0,
224,224,0,0 };

static const unsigned char E[] = {
0,0,5,4,240,240,0,128,128,0,224,224,0,128,128,0,
240,240,0,0 };

static const unsigned char F[] = {
0,0,5,4,240,240,0,128,128,0,224,224,0,128,128,0,
128,128,0,0 };

static const unsigned char G[] = {
0,0,5,4,112,112,0,128,128,0,176,176,0,144,144,0,
112,112,0,0 };

static const unsigned char H[] = {
0,0,5,4,144,144,0,144,144,0,240,240,0,144,144,0,
144,144,0,0 };

static const unsigned char I[] = {
0,0,5,4,112,112,0,32,32,0,32,32,0,32,32,0,
112,112,0,0 };

static const unsigned char J[] = {
0,0,5,4,16,16,0,16,16,0,16,16,0,144,144,0,
96,96,0,0 };

static const unsigned char K[] = {
0,0,5,4,144,144,0,160,160,0,192,192,0,160,160,0,
144,144,0,0 };

static const unsigned char L[] = {
0,0,5,4,128,128,0,128,128,0,128,128,0,128,128,0,
240,240,0,0 };

static const unsigned char M[] = {
0,0,5,4,144,144,0,240,240,0,144,144,0,144,144,0,
144,144,0,0 };

static const unsigned char N[] = {
0,0,5,4,144,144,0,208,208,0,176,176,0,144,144,0,
144,144,0,0 };

static const unsigned char O[] = {
0,0,5,4,96,96,0,144,144,0,144,144,0,144,144,0,
96,96,0,0 };

static const unsigned char P[] = {
0,0,5,4,224,224,0,144,144,0,224,224,0,128,128,0,
128,128,0,0 };

static const unsigned char Q[] = {
0,0,5,4,96,96,0,144,144,0,144,144,0,160,160,0,
80,80,0,0 };

static const unsigned char R[] = {
0,0,5,4,224,224,0,144,144,0,224,224,0,144,144,0,
144,144,0,0 };

static const unsigned char S[] = {
0,0,5,4,112,112,0,128,128,0,96,96,0,16,16,0,
224,224,0,0 };

static const unsigned char T[] = {
0,0,5,4,240,240,0,64,64,0,64,64,0,64,64,0,
64,64,0,0 };

static const unsigned char U[] = {
0,0,5,4,144,144,0,144,144,0,144,144,0,144,144,0,
240,240,0,0 };

static const unsigned char V[] = {
0,0,5,4,144,144,0,144,144,0,144,144,0,160,160,0,
64,64,0,0 };

static const unsigned char W[] = {
0,0,5,4,144,144,0,144,144,0,144,144,0,240,240,0,
144,144,0,0 };

static const unsigned char X[] = {
0,0,5,4,144,144,0,144,144,0,96,96,0,144,144,0,
144,144,0,0 };

static const unsigned char Y[] = {
0,0,5,4,144,144,0,144,144,0,96,96,0,64,64,0,
128,128,0,0 };

static const unsigned char Z[] = {
0,0,5,4,240,240,0,16,16,0,32,32,0,64,64,0,
240,240,0,0 };

static const unsigned char indescr[] = {
0,0,5,4,64,64,0,64,64,0,0,0,0,0,0,0,
0,0,0,0 };


static const unsigned char pargauche[] = {
0,0,5,4,32,32,0,64,64,0,64,64,0,64,64,0,
32,32,0,0 };


static const unsigned char pardroite[] = {
0,0,5,4,64,64,0,32,32,0,32,32,0,32,32,0,
64,64,0,0 };


static const unsigned char etoile[] = {
0,0,5,4,0,0,0,144,144,0,96,96,0,96,96,0,
144,144,0,0 };


static const unsigned char plus[] = {
0,0,5,4,0,0,0,64,64,0,224,224,0,64,64,0,
0,0,0,0 };


static const unsigned char moins[] = {
0,0,5,4,0,0,0,0,0,0,112,112,0,0,0,0,
0,0,0,0 };


static const unsigned char point[] = {
0,0,5,4,0,0,0,0,0,0,0,0,0,96,96,0,
96,96,0,0 };


static const unsigned char barre[] = {
0,0,5,4,32,32,0,32,32,0,32,32,0,64,64,0,
64,64,0,0 };

static const unsigned char virgule[] = {
0,0,5,4,0,0,0,0,0,0,32,32,0,32,32,0,
64,64,0,0 };

static const unsigned char* Alphabet[] = {
    indescr,  // '  -39
    pargauche,  // (
    pardroite, // )
    etoile,
    plus,
    virgule,
    moins,  //-
    point,  // .
    barre, //  /
    zero,  //debut -48
    un,
    deux,
    trois,
    quatre,
    cinq,
    six,
    sept,
    huit,
    neuf,
    deuxpt,
    ptvirgule,
    inf,                 //inférieur
    egal,
    sup,
    ptint,              //?
    arobase,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z
};




// fonction qui écrit une chaîne de charactère à l'aide des sprites de Alphabet[]

void writeg(int x, int y, char string[])
{
int size , j;
char spr;
char* str;
size = strlen(string);            // on récupère la longueur de string dans size

   for(j=0 ; j<size ; j++){                // pour chaque caractère de la chaine         
     str=&string[j];
     if(islower(*str))
    *str= toupper(*str);
    spr = *str-39;
    if(*str> 0x26 && *str< 0x5B )sprite(Alphabet[spr],x,y);    // et on affiche le caractère    si != espace
    x=x+5;                     // sans oublier les espacements
   }
}

void writevalue(int x,int y,long value)
{

   char st[30]={0};
    ltoa(value,&st,10);


    writeg(x,y,st);                   // et on affiche la string avec write (ci dessus)
}

