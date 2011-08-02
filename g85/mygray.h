#ifndef _GRAY
#define _GRAY

typedef struct {
	char *DD1, *DD2, *VRAM1, *VRAM2, which;
	int time1, time2;
} Screen;

void draw_all(char* buffer);
void reset_calc();

void gray_interrupt();

int gray_init();
void gray_quit();
void gray_clearBuffers();
void gray_refresh();
int draw_read_pix(int x, int y);
void draw_write_pix(int x, int y, int color);  
void sprite(unsigned char *sprite, int x, int y);
Screen* gray_getScreen();
void Vline(int x, int y1, int y2, int color);
void Hline(int x1, int x2, int y, int color);
void scrollx(char x,unsigned char coul);
void scrolly(char y,unsigned char coul);
void gray_state(int mode);
void draw_bmp(unsigned char* bmp);
void writeg(int x, int y, char string[]);
void writevalue(int x,int y,long value);

#endif
