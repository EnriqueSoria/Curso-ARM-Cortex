// File: glcd_fputc.c
// Angel Perles 2012
// standard io implementation of putchar or fputc

#include <stdio.h>
#include "glcd.h"


static unsigned char putchar_x=0,putchar_y=0;

/**
  * @brief  Retargets the C library printf function to the GLCD
  */
  
#ifdef __GNUC__
  int __io_putchar(int ch)
#else
  int fputc(int ch, FILE *f)
#endif /* __GNUC__ */  
{
	glcd_font_putchar(&font5x7,putchar_x,putchar_y,ch);
	putchar_x = putchar_x + font5x7.fontx + 1;
	return (ch);

}

void glcd_putchar_setxy(UI_8 x, UI_8 y) {

	putchar_x = x;
	putchar_y = y;
} 

