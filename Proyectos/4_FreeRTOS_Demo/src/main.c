/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Atollic TrueSTUDIO(R)
 **                STMicroelectronics STM32F4xx Standard Peripherals Library
 **
 **  Distribution: The file is distributed as is, without any warranty
 **                of any kind.
 **
 **  (c)Copyright Atollic AB.
 **  You may use this file as-is or modify it according to the needs of your
 **  project. Distribution of this file (unmodified or modified) is not
 **  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
 **  rights to distribute the assembled, compiled & linked contents of this
 **  file as part of an application binary file, provided that it is built
 **  using the Atollic TrueSTUDIO(R) toolchain.
 **
 **
 *****************************************************************************
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "utils.h"
#include "debug.h"
#include "mems.h"

// Task priorities: Higher numbers are higher priority.
#define mainTIME_TASK_PRIORITY      ( tskIDLE_PRIORITY + 3 )
#define mainMEMS_TASK_PRIORITY      ( tskIDLE_PRIORITY + 2 )
#define mainDEBUG_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1 )
#define mainINTEGER_TASK_PRIORITY   ( tskIDLE_PRIORITY )

xTaskHandle hTimeTask;
xTaskHandle hMemsTask;
xTaskHandle hDebugTask;

portTASK_FUNCTION_PROTO( vTimeTask, pvParameters );
portTASK_FUNCTION_PROTO( vMemsTask, pvParameters );
portTASK_FUNCTION_PROTO( vDebugTask, pvParameters );

uint64_t u64Ticks=0;        // Counts OS ticks (default = 1000Hz).
uint64_t u64IdleTicks=0;    // Value of u64IdleTicksCnt is copied once per sec.
uint64_t u64IdleTicksCnt=0; // Counts when the OS has no task to execute.
uint16_t u16PWM1=0;

// ------ MIS LEDS ----------------------------------------------------------
#define mainLED_TASK_PRIORITY      ( tskIDLE_PRIORITY + 2 )
#define LED1 LED5
#define LED2 LED6

// Definimos el tipo led_holder
typedef struct {
	Led_TypeDef led;
	uint32_t delay;
} led_holder;


// ------ Definimos las tareas para los leds -------
#define NUM_LEDS 4
xTaskHandle hLEDTask[NUM_LEDS];
led_holder parameters[NUM_LEDS];

portTASK_FUNCTION( vled1, pvParameters ) {  // la tarea
	led_holder *par = (led_holder*) pvParameters;
	while(1) {
				STM_EVAL_LEDToggle(par->led);
				vTaskDelay(par->delay / portTICK_RATE_MS); // dormir 200 ms
	}
}
portTASK_FUNCTION( vled2, pvParameters ) {  // la tarea
	led_holder *par = (led_holder*) pvParameters;
	while(1) {
				STM_EVAL_LEDToggle(par->led);
				vTaskDelay(par->delay / portTICK_RATE_MS); // dormir 200 ms
	}
}
portTASK_FUNCTION( vled3, pvParameters ) {  // la tarea
	led_holder *par = (led_holder*) pvParameters;
	while(1) {
				STM_EVAL_LEDToggle(par->led);
				vTaskDelay(par->delay / portTICK_RATE_MS); // dormir 200 ms
	}
}
portTASK_FUNCTION( vled4, pvParameters ) {  // la tarea
	led_holder *par = (led_holder*) pvParameters;
	while(1) {
				STM_EVAL_LEDToggle(par->led);
				vTaskDelay(par->delay / portTICK_RATE_MS); // dormir 200 ms
	}
}



// ============================================================================
int main( void ) {

	HwInit();

	vDebugInitQueue();
	vDebugPrintf( "\r\nEPS Test1 0.0.1 - 1/4/2012\r\n" );
	vDebugPrintResetType();

	// Definimos los leds y los retardos, y creamos las tareas
	parameters[0].led = LED1;
	parameters[0].delay = 100;
	xTaskCreate( vled1, (signed char *) "LED1", configMINIMAL_STACK_SIZE, &parameters[0], mainLED_TASK_PRIORITY, &hLEDTask[0] );
	
	parameters[1].led = LED2;
	parameters[1].delay = 200;
	xTaskCreate( vled2, (signed char *) "LED2", configMINIMAL_STACK_SIZE, &parameters[1], mainLED_TASK_PRIORITY, &hLEDTask[1] );
	
	parameters[2].led = LED3;
	parameters[2].delay = 300;
	xTaskCreate( vled3, (signed char *) "LED3", configMINIMAL_STACK_SIZE, &parameters[2], mainLED_TASK_PRIORITY, &hLEDTask[2] );
	
	parameters[3].led = LED4;
	parameters[3].delay = 400;
	xTaskCreate( vled4, (signed char *) "LED4", configMINIMAL_STACK_SIZE, &parameters[3], mainLED_TASK_PRIORITY, &hLEDTask[3] );
	
	
	vTaskStartScheduler(); // This should never return.

	// Will only get here if there was insufficient memory to create
	// the idle task.
	for( ;; );  
}

// This task should run every 50ms.  The task will average 50ms over time by
// monitoring the actual time between calls and self adjusting accordingly.
// ---------------------------------------------------------------------------- 
portTASK_FUNCTION( vTimeTask, pvParameters ) {
    portTickType xLastWakeTime;
    uint8_t i=0;

    xLastWakeTime = xTaskGetTickCount();

    for(;;) {
        // Once per second, copy the number of idle ticks and then
        // reset the rolling counter.
        if ( ++i == 20 ) {
            i = 0;
            u64IdleTicks = u64IdleTicksCnt;
            u64IdleTicksCnt = 0;
        }

        vTaskDelayUntil( &xLastWakeTime, ( 50 / portTICK_RATE_MS ) );
    }
}

// This FreeRTOS callback function gets called once per tick (default = 1000Hz).
// ---------------------------------------------------------------------------- 
void vApplicationTickHook( void ) {
    ++u64Ticks;
}

// This FreeRTOS call-back function gets when no other task is ready to execute.
// On a completely unloaded system this is getting called at over 2.5MHz!
// ---------------------------------------------------------------------------- 
void vApplicationIdleHook( void ) {
    ++u64IdleTicksCnt;
}

// A required FreeRTOS function.
// ---------------------------------------------------------------------------- 
void vApplicationMallocFailedHook( void ) {
    configASSERT( 0 );  // Latch on any failure / error.
}




