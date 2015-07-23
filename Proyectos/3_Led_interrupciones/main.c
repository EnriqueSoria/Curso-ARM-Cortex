/**
   @file main.c

   @brief Demo for generating EXTIO interrupts
   Uses the "user" pushbutton of the Discovery
   
   @author Angel Perles http://armpower.blogs.upv.es
   @date 2015/04/21
*/

#include "stm32f4xx.h"


void led_init(void), led_on(uint16_t led), led_off(uint16_t led), wait(uint32_t);
uint16_t arr[4] = {GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};

volatile uint8_t on = 0;

int main(void)
{
	
	led_init();
	
	
   EXTI_InitTypeDef   EXTI_InitStructure;
   GPIO_InitTypeDef   GPIO_InitStructure;
   NVIC_InitTypeDef   NVIC_InitStructure;
   
   /* PASO 1: Disable interrupt */
   NVIC_DisableIRQ(EXTI0_IRQn);

   /* PASO 2: Configure target device */
   /* Enable GPIOA clock */
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
   /* Configure PA0 pin as input floating */
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* Configure EXTI Line0 (connected to PA0 pin) in interrupt mode */
   /* Enable SYSCFG clock */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
   /* Connect EXTI Line0 to PA0 pin */
   SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

   /* Configure EXTI Line0 */
   EXTI_InitStructure.EXTI_Line = EXTI_Line0;
   EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
   EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
   EXTI_InitStructure.EXTI_LineCmd = ENABLE;
   EXTI_Init(&EXTI_InitStructure);

   /* PASO 3: Configure NVIC related interrupt */
   /* Enable and set EXTI Line0 Interrupt to the lowest priority */
   NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
  
   /* PASO 4: Enable interrupt */
   NVIC_EnableIRQ(EXTI0_IRQn);
  
	
	 uint8_t ratio = 0;
   while (1) {
		if(on) {
			for(int i=0, j=3; i<32; i++, j++){
				if(on) {
					led_on(arr[i%4]);
					wait(10000000-10000*i);
				}
				led_off(arr[j%4]);
			}
			wait(1000000);
		} else {
			// Limpiamos
			for(int i=0; i<4; i++){
				led_off(arr[i]);
			}
		}
	 
	 };
      
}

void led_init(void){
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
