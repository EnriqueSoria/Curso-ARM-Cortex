// Fichero: test_glcd.c
// Ejemplo de uso del módulo glcd de la placa Mikroe
// Angel Perles 2009/03/25

#include <stdio.h>

#include "glcd.h"

// Incorporar en el codigo una image en formato xbm
#include "./med_sim2.xbm"


void pausa(long i);

//----------------------------------------------------------------------------
int main(void) {

   int i;
   int x,y;

   glcd_inicializar();	// preparar el hardware

   // rejilla
   /*
    for (y=0;y<64;y++) {
        for (x=0;x<128;x++) {
            if (((x+y)%2) == 0) {
                glcd_putpixel(x,y,1);
            } else {
                glcd_putpixel(x,y,0);
            }
        }
    }
      */
   
   // apagar y encender la pantalla
   pausa(20000);
   glcd_display_off();
   pausa(20000);
   glcd_display_on();
 
   // mostrar una imagen en las coordenadas (1,1)
   glcd_putxbitmap(1,1,0,med_sim2);



   // hacer un triangulo a base de pixels
   for (y=0;y<10;y++) {
      for (x=0;x<y;x++) {
         glcd_putpixel(85+x,y,1);
         glcd_putpixel(85-x,y,1);
         glcd_putpixel(85+x,63-y,1);
         glcd_putpixel(85-x,63-y,1);
      }
   }

   // sacar un texto con el font por defecto
   glcd_font_puts(&font5x7,57,20,"Hola GLCD!");

   // dibujar un par de rectángulos
   glcd_rectangle(50,30,120,50,1);
   glcd_rectangle(55,35,115,45,1);

   // probar la redirección de printf()
   glcd_putchar_setxy(57,37);
   printf("Cont=");

   while (1) {
      for(i=0;i<10000;i++) {
         glcd_putchar_setxy(90,37);
         printf("%04d",i);
      }
   }
}

//-----------------------------------------------------------------------------
void pausa(long i) {
   while(i--);
}

// --- Fin del archivo --------------------------------------------------------
