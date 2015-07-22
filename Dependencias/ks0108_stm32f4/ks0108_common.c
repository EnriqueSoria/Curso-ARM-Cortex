/**
    @file ks0108_common.c
    @brief ks0108 glcd library. Common parts

    Simple graphical library for the ARM Cortex-M course http://armcortexm.blogs.upv.es
    Based on my previous library for the 8051

    @author Angel Perles
    @author Daniel Fernandez added font support
    @date 2008, 2012
*/


#include "glcd.h"
#include "ks0108.h"

#define UI_8 unsigned char


void ks0108_display_on(void) {
	ks0108_chipselect_1;
	ks0108_write_instruction(0x3F);
	ks0108_chipselect_2;
	ks0108_write_instruction(0x3F);
	ks0108_chipselect_none;
}

void ks0108_display_off(void) {
	ks0108_chipselect_1;
	ks0108_write_instruction(0x3E);
	ks0108_chipselect_2;
	ks0108_write_instruction(0x3E);
	ks0108_chipselect_none;
}

void ks0108_clear_screen(void) {
	UI_8 page,column;

	for (page=0; page<8 ; page++) {
		ks0108_chipselect_1;
		ks0108_write_instruction(X_ADDRESS | page);
		ks0108_write_instruction(Y_ADDRESS);	// column 0

		for (column=0;column<64;column++) {
			ks0108_write_data(0x00);
		}

		ks0108_chipselect_2;
		ks0108_write_instruction(X_ADDRESS | page);
		ks0108_write_instruction(Y_ADDRESS);	// column 0

		for (column=0;column<64;column++) {
			ks0108_write_data(0x00);
		};
	}
	ks0108_chipselect_none;
}

// up-left corner is 0,0 for me, like microsoft windows gui
void ks0108_putpixel(UI_8 x, UI_8 y, UI_8 color) {
    UI_8 dat;

	if ((x>128) || (y>64)) {
		return;
	}

	if (x < 64) {
		ks0108_chipselect_1;
	} else {
		ks0108_chipselect_2;
		x=x-64;
	}

	ks0108_write_instruction(X_ADDRESS | (y/8));	// page
	ks0108_write_instruction(Y_ADDRESS | x);	// column
	dat = ks0108_read_data();
    dat = ks0108_read_data();	//### no se que pasa, punyeta
	if (color == 0)	{// black
		dat &= ~(1 <<(y%8));
	} else {
		dat |= (1 << (y%8));
	}
    ks0108_write_instruction(X_ADDRESS | (y/8));	// page
	ks0108_write_instruction(Y_ADDRESS | x);	// column
	ks0108_write_data(dat);
	ks0108_chipselect_none;
}

// rectangle from x1-y1 to x2-y2, x1-y1 must be up-left corner 
void ks0108_rectangle(UI_8 x1, UI_8 y1, UI_8 x2, UI_8 y2, UI_8 color) {
	UI_8 i;

	for (i=x1;i<=x2;i++) {
		ks0108_putpixel(i,y1,color);
		ks0108_putpixel(i,y2,color);
	}

	for (i=y1+1;i<y2;i++) {
		ks0108_putpixel(x1,i,color);
		ks0108_putpixel(x2,i,color);
	}
}

// image in xbm format
void ks0108_putxbitmap(UI_8 posx, UI_8 posy, UI_8 copy_options, UI_8 width, UI_8 height, const unsigned char *xbm) {

	UI_8 x,y;
	UI_8 bit_data;

	for (x=0;x<width;x++) {
		for (y=0;y<height;y++) {
			bit_data = (*(xbm+(x/8)+((width/8+1)*y))) & (1 << (x%8));
			if (bit_data==0)	// fica sols si es un 1
                ks0108_putpixel(posx+x,posy+y,1);
			else
                ks0108_putpixel(posx+x,posy+y,0);
		}
	}

}

void ks0108_font_putchar(const Tfont *fnt, UI_8 x, UI_8 y, char p) {

 UI_8 mx,my;
 unsigned char *fontdata;
 unsigned char line;

 if ((p <32) || (p>128)) {
	return;
 }

 fontdata = fnt->dat + (p-32)*fnt->fontx;
 for (mx = 0;mx < fnt->fontx; mx++) {
 	line = *fontdata;
 	for (my = 0;my < fnt->fonty; my++) {
    	if (line & (0x01 << my))
            ks0108_putpixel(x+mx,y+my,1);
		else
            ks0108_putpixel(x+mx,y+my,0);
	}
	fontdata++;
 }
}


void ks0108_font_puts(const Tfont *fnt, UI_8 x, UI_8 y, const char *p) {

	UI_8 mx;

	mx = x;
	while (*p != '\0') {
		ks0108_font_putchar(fnt, mx, y, *p);
		p++;
		mx += fnt->fontx + 1;
	}
}


