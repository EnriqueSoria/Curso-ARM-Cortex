/**
   @file main.c

   @brief Encendemos los 4 LEDs de la placa de forma intermitente,
	 usando interrupciones para la pulsacion del boton de la placa,
	 y usando interrupciones para realizar esperas de 'x' milisegundos.
   
   @author Enrique Soria
   @date 23/07/2015
*/

#include "stm32f4xx.h"


void led_init(void), led_on(uint16_t led), led_off(uint16_t led), wait(uint32_t);
void wait_ms(uint32_t), wait_init(void), delay_ISR(void); 
uint16_t arr[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

volatile uint8_t on = 0;
volatile uint32_t tick_count = 0;

int main(void)
{
   EXTI_InitTypeDef   EXTI_InitStructure;
   GPIO_InitTypeDef   GPIO_InitStructure;
   NVIC_InitTypeDef   NVIC_InitStructure;
   
   // Deshabilitamos las interrupciones
   NVIC_DisableIRQ(EXTI0_IRQn);

   // Configuramos el dispositivo (tick)
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   // Lo configuramos como interrupcion
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

   // Configuramos el pin
   EXTI_InitStructure.EXTI_Line = EXTI_Line0;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

   // Configuramos la interrupcion
   NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
  
   // Activamos la interrupcion
   NVIC_EnableIRQ(EXTI0_IRQn);

	 // Inicializamos el módulo de espera
	 wait_init();
	 
	 // Inicializamos los LEDs de la placa
	 led_init();
	 
	 // Bucle principal
	 uint32_t k = 1;		// Cada unidad de k reduce 1 ms
   while (1) {
		if(on) {
			for(int i=0, j=3; i<4; i++, j++){
				// Encendemos un led
				led_on(arr[i%4]);
				
				// Hacemos una espera
				if(on) 	{ 
					k = k>250?250:k+1;		// No queremos tiempos de espera negativos
					wait_ms(300-k); 							
				}	else {
					break;
				}			

				// Apagamos OTRO led
				led_off(arr[j%4]);
			}
		} else {
			// Apagamos todos los leds
			for(int i=0; i<4; i++){
				led_off(arr[i]);
			}
			
			k=0;
		}
	 };  
}

void led_init(void){
	 GPIO_InitTypeDef  GPIO_InitStructure;  // estructura donde se pone la configuración deseada

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); // darle reloj al periférico, AHORA VIVE!
    
   GPIO_StructInit(&GPIO_InitStructure);   // establecer valores por defecto

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

void wait_init(void) {
	SysTick_Config( SystemCoreClock / 1000 );
}

void delay_ISR(void) {
	tick_count++;
}

void wait_ms(uint32_t ms) {
	uint32_t max = tick_count + ms;
	while(tick_count <= max);
}
