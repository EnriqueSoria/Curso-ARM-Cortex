#include <stdint.h>     // definiciones estándar para los números enteros

#include "stm32f4xx_rcc.h"    // cabeceras proporcionadas por St para periféricos
#include "stm32f4xx_gpio.h"	

void led_inicializar(void),
			led_on(uint16_t led),
			led_off(uint16_t led),
			wait(uint32_t wait);

uint16_t arr[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

int main(void) {
	

	led_inicializar();
	
	while(1) {
		for(int i=0; i<4; i++){
			led_on(arr[i]);
			wait(1000000);
		}
		for(int i=0; i<4; i++){
			led_off(arr[i]);
			wait(1000000);
		}
	}
	
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

void led_on(uint16_t led) {
	GPIO_SetBits(GPIOD, led);
}

void led_off(uint16_t led) {
	GPIO_ResetBits(GPIOD, led);
}

void wait(uint32_t wait) {
	while(wait--);
}
