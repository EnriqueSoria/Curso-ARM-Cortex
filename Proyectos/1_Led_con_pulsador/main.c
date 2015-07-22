#include <stdint.h>     // definiciones estándar para los números enteros

#include "stm32f4xx_rcc.h"    // cabeceras proporcionadas por St para periféricos
#include "stm32f4xx_gpio.h"	

void led_inicializar(void),
	led_on(uint16_t led),
	led_off(uint16_t led),
	wait(uint32_t wait);

void boton_inicializar(void);
uint8_t boton_presionado(void);


uint16_t arr[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

int main(void) {

	// Inicializamos
	led_inicializar();
	boton_inicializar();
	
	uint8_t encendido = 0;
	
	// Bucle principal
	while(1) {
		
		while(boton_presionado()) {
			for(int i=0; i<4; i++){
					led_on(arr[i]);
					wait(1000000);
			}
			for(int i=0; i<4; i++){
				led_off(arr[i]);
				wait(1000000);
			}
		}
		
	} // fiwhile
	
	return(0);
}


void led_inicializar(void) {
   GPIO_InitTypeDef  GPIO_InitStructure;  // estructura donde se pone la configuración deseada

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // darle reloj al periférico, AHORA VIVE!
    
   GPIO_StructInit(&GPIO_InitStructure);   // establecer valores por defecto

   /* dar los detalles del puerto/pin */
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;        // pin que desamos configurar
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;      // lo vamos a usar como salida
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      // queremos que sea push-pull
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;   // frecuencia máxima para el puerto
   GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;   // desactivar los pull-up, no hacen falta
   
   GPIO_Init(GPIOD, &GPIO_InitStructure);              // hacer efectiva configuración puerto
}

/**
  *	Funciones LED
  */
void led_on(uint16_t led) {
	GPIO_SetBits(GPIOD, led);
}

void led_off(uint16_t led) {
	GPIO_ResetBits(GPIOD, led);
}

void wait(uint32_t wait) {
	while(wait--);
}


/**
  *	Funciones BOTON
  */
void boton_inicializar(void) {
	GPIO_InitTypeDef settings;
	
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE );
	
	GPIO_StructInit( &settings );
	
	settings.GPIO_Pin 	= GPIO_Pin_0;
	settings.GPIO_Mode	= GPIO_Mode_IN;
	settings.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	
	GPIO_Init( GPIOA, &settings );
}

uint8_t boton_presionado(void) {
	return( GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) );
}