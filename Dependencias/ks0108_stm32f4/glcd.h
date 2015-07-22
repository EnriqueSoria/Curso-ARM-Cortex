/**
    @file glcd.h
    @brief Wrapper for my ks0108.c library

    Simple graphical library for the ARM Cortex-M course http://armcortexm.blogs.upv.es

    @author Angel Perles
    @date 2008, 2012
*/

#ifndef glcd_H
#define glcd_H

#define UI_8 unsigned char

// library prototypes

void ks0108_reset(void);
void ks0108_display_on(void);
void ks0108_display_off(void);
void ks0108_clear_screen(void);
void ks0108_putpixel(UI_8 x, UI_8 y, UI_8 color);
void ks0108_rectangle(UI_8 x1, UI_8 y1, UI_8 x2, UI_8 y2, UI_8 color);
void ks0108_putxbitmap(UI_8 posx, UI_8 posy, UI_8 copy_options, UI_8 width, UI_8 height, const UI_8 *xbm);

// redefinitions for generalization
#define glcd_inicializar() ks0108_reset()
#define glcd_reset() ks0108_reset()
#define glcd_display_on() ks0108_display_on()
#define glcd_display_off() ks0108_display_off()
#define glcd_clear_screen() ks0108_clear_screen()
#define glcd_putpixel(A,B,C) ks0108_putpixel(A,B,C)
#define glcd_rectangle(A,B,C,D,E) ks0108_rectangle(A,B,C,D,E)
//#define glcd_putxbitmap(A,B,C,D,E,F) ks0108_putxbitmap(A,B,C,D,E,F)
#define glcd_putxbitmap(A,B,C,XBM) ks0108_putxbitmap(A,B,C,XBM##_width,XBM##_height,XBM##_bits)

//#define glcd_(A) ks0108_(A)


// ------------------ font support -------------------------------------------
typedef struct {
	unsigned char fontx;
	unsigned char fonty;
	unsigned char *dat;
} Tfont;

extern Tfont font5x7;

void ks0108_font_putchar(const Tfont *fnt, UI_8 x, UI_8 y, char p);
void ks0108_font_puts(const Tfont *fnt, UI_8 x, UI_8 y, const char *p);

#define glcd_font_putchar(A,B,C,D) ks0108_font_putchar(A,B,C,D)
#define glcd_font_puts(A,B,C,D) ks0108_font_puts(A,B,C,D)

void glcd_putchar_setxy(UI_8 x, UI_8 y);

//----------------------------------------------------------------------------
#endif
