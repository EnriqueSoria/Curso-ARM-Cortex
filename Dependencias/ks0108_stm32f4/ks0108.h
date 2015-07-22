/**
    @file ks0108.h
    @brief ks0108.c library

    Simple graphical library for the ARM Cortex-M course http://armcortexm.blogs.upv.es
    Based on my previous library for the 8051

    @author Angel Perles
    @date 2008, 2012
*/

#ifndef ks0108_H
#define ks0108_H

#include <stdint.h>

#define UI_8 unsigned char

void ks0108_reset(void);
void ks0108_display_on(void);
void ks0108_clear_screen(void);
void ks0108_putpixel(UI_8 x, UI_8 y, UI_8 color);
void ks0108_rectangle(UI_8 x1, UI_8 y1, UI_8 x2, UI_8 y2, UI_8 color);
void ks0108_putxbitmap(UI_8 posx, UI_8 posy, UI_8 copy_options, UI_8 width, UI_8 height, const UI_8 *xbm);

enum TGlcdSignal {PLCD_DATA, PLCD_CS1, PLCD_CS2, PLCD_DI, PLCD_RW, PLCD_E, PLCD_RST};
void ks0108_set_signal(enum TGlcdSignal sig, unsigned char value);
void ks0108_write_instruction(UI_8 cmd);
void ks0108_write_data(UI_8 cmd);
UI_8 ks0108_read_data(void);
void ks0108_delay(uint32_t cnt);

#define X_ADDRESS	0xB8	/* Adress base for Page 0 */
#define Y_ADDRESS	0x40	/* Adress base for Y0	  */

// definitions for helping codification
#define ks0108_chipselect_1 ks0108_set_signal(PLCD_CS2,1); ks0108_set_signal(PLCD_CS1,0)
#define ks0108_chipselect_2 ks0108_set_signal(PLCD_CS1,1); ks0108_set_signal(PLCD_CS2,0)
#define ks0108_chipselect_none ks0108_set_signal(PLCD_CS1,1); ks0108_set_signal(PLCD_CS2,1)



#endif
