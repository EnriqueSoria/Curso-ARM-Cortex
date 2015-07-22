/**
    @file ks0108_common.c
    @brief ks0108 glcd library. Cheapdaq version for testing the display

    Simple graphical library for the ARM Cortex-M course http://armcortexm.blogs.upv.es
    Based on my previous library for the 8051

    @author Angel Perles
    @author Daniel Fernandez added font support
    @date 2008, 2012
*/


#include "glcd.h"
#include "ks0108.h"

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

// Delays, change for your display and uC, 0 generally, but take care of superfast 8051 i.e. Silabs
#define DELAY_RESET 65535
#define DELAY_TWL 	100		// 450 nS
#define DELAY_TWH	100		// 450 nS
#define DELAY_TDHWR	5		// data hold time for read and write 20 ns
#define DELAY_TASU	50		// address set-up time 140 nS
#define DELAY_TD	50		// data delay time 320 nS max.

#define DELAY_KK	10	// de moment per a proves
#define MAX_BUSY_WAIT 200	// wait cnt. for the no busy condition, avoids software lock if glcd malfunction 


void ks0108_write_instruction(UI_8 cmd);
void ks0108_write_data(UI_8 cmd);
UI_8 ks0108_read_data(void);
void ks0108_no_busy(void);

// -------- delay function
void ks0108_delay(uint32_t cnt) {

		while (cnt--);
}

#define SUBDEVICE_DIGITAL_PORT_A 0x00
#define SUBDEVICE_DIGITAL_PORT_B 0x01
#define SUBDEVICE_DIGITAL_PORT_C 0x02
#define SUBDEVICE_DIGITAL_PORT_D 0x03
#define SUBDEVICE_DIGITAL_PORT_E 0x04

/*************************************************************************************************/
void cds_DAQWriteDigitalLine(uint8_t subdevice, uint8_t line, uint8_t value) {

    switch(subdevice) {
    case SUBDEVICE_DIGITAL_PORT_A:
        if (value) {
            GPIOA->BSRRL = (((uint16_t)0x0001) << line);
        } else {
            GPIOA->BSRRH = (((uint16_t)0x0001) << line);
        }
        break;

    case SUBDEVICE_DIGITAL_PORT_B:
        if (value) {
            GPIOB->BSRRL = (((uint16_t)0x0001) << line);
        } else {
            GPIOB->BSRRH = (((uint16_t)0x0001) << line);
        }
        break;
    case SUBDEVICE_DIGITAL_PORT_C:
        if (value) {
            GPIOC->BSRRL = (((uint16_t)0x0001) << line);
        } else {
            GPIOC->BSRRH = (((uint16_t)0x0001) << line);
        }
        break;
    case SUBDEVICE_DIGITAL_PORT_D:
        if (value) {
            GPIOD->BSRRL = (((uint16_t)0x0001) << line);
        } else {
            GPIOD->BSRRH = (((uint16_t)0x0001) << line);
        }
        break;
    case SUBDEVICE_DIGITAL_PORT_E:
        if (value) {
            GPIOE->BSRRL = (((uint16_t)0x0001) << line);
        } else {
            GPIOE->BSRRH = (((uint16_t)0x0001) << line);
        }
        break;
    default:
        //result = CD_Error_DAQBadSubdevice;  /* bad subdevice */
		break;
    }

	//return(result);
}

void cds_DAQWriteDigitalPortMask(uint8_t subdevice, uint16_t value, uint16_t mask) {

   uint32_t my_value;
   uint32_t *port_BSRR=0; 

   my_value = 0;

   switch(subdevice) {
   case SUBDEVICE_DIGITAL_PORT_A:
       port_BSRR = (uint32_t *)&(GPIOA->BSRRL);
       break;
    case SUBDEVICE_DIGITAL_PORT_B:
       port_BSRR = (uint32_t *)&(GPIOB->BSRRL);
       break;
    case SUBDEVICE_DIGITAL_PORT_C:
       port_BSRR = (uint32_t *)&(GPIOC->BSRRL);
       break;
    case SUBDEVICE_DIGITAL_PORT_D:
       port_BSRR = (uint32_t *)&(GPIOD->BSRRL);
       break;
    case SUBDEVICE_DIGITAL_PORT_E:
       port_BSRR = (uint32_t *)&(GPIOE->BSRRL);
       break;
    default:
        //return(CD_Error_DAQBadSubdevice);  /* bad subdevice */
       break;
    }

    my_value = mask & (~value);
    my_value <<= 16;  
    my_value |= (mask & value);  // obtaining 1's

    *port_BSRR = my_value;
    //return(CD_Error_NoError);   

}


void cds_DAQReadDigitalPort(uint8_t subdevice, uint16_t *value) {

    switch(subdevice) {
    case SUBDEVICE_DIGITAL_PORT_A:
       *value = GPIOA->IDR;
       break;
    case SUBDEVICE_DIGITAL_PORT_B:
       *value = GPIOB->IDR;
       break;
    case SUBDEVICE_DIGITAL_PORT_C:
       *value = GPIOC->IDR;
       break;
    case SUBDEVICE_DIGITAL_PORT_D:
       *value = GPIOD->IDR;
       break;
    case SUBDEVICE_DIGITAL_PORT_E:
       *value = GPIOE->IDR;
       break;
    default:
        //return(CD_Error_DAQBadSubdevice);  /* bad subdevice */
       break;
    }
    //return(CD_Error_NoError);

}




/*************************************************************************************************/
/**
    @brief set values for the signals of the glcd

    Data bus: pull-up, D7-D0 -> PE10-PE3
    Rest of signals in push-pull mode
    PLCD_CS1 -> PD6
    PLCD_CS2 -> PD7
    PLCD_DI  -> PD8
    PLCD_RW  -> PD9
    PLCD_E   -> PD10
    PLCD_RST -> PD11

*/
//enum TGlcdSignal {PLCD_DATA, PLCD_CS1, PLCD_CS2, PLCD_DI, PLCD_RW, PLCD_E, PLCD_RST};
void ks0108_set_signal(enum TGlcdSignal sig, unsigned char value) {

   
    ks0108_delay(100);


    switch(sig) {
    case PLCD_CS1:
        cds_DAQWriteDigitalLine(3,6,value);
        break;
    case PLCD_CS2:
        cds_DAQWriteDigitalLine(3,7,value);
        break;
    case PLCD_DI:
        cds_DAQWriteDigitalLine(3,8,value);
        break;
    case PLCD_RW:
        cds_DAQWriteDigitalLine(3,9,value);
        break;
    case PLCD_E:
        cds_DAQWriteDigitalLine(3,10,value);
        break;
    case PLCD_RST:
        cds_DAQWriteDigitalLine(3,11,value);
        break;
    case PLCD_DATA:
        cds_DAQWriteDigitalPortMask(4,((uint16_t)value) << 3, 0x7F8);
        break;
    }

}

/*************************************************************************************************/
/**
    @brief read data register
*/
unsigned char ks0108_read_databus(void) {

    uint16_t value;
    unsigned char dat;

   cds_DAQReadDigitalPort(4, &value);
   dat = (value >> 3) & 0xFF;

/*      
    for (i=0;i<8;i++) {
        cds_DAQReadDigitalLine(4,3+i,&value);
        if (value) {
            dat |= (1 << i);
        }
    }
*/
      
    return(dat);

}



// -------- prepare LCD for its use and clear the screen ------------------------
void ks0108_reset(void) {

   GPIO_InitTypeDef  GPIO_InitStructure;
   // pel ks0108 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   
   /* PORT D *****************************************/

   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
   
   /* PD11 - PD6 a */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  // open drain
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOD, &GPIO_InitStructure);
   
   /* PE10 - PE3 a pull-up */
   /* PORT E *****************************************/
   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);  

   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_9 | GPIO_Pin_8 | GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  // open drain
   //GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
   GPIO_Init(GPIOE, &GPIO_InitStructure);

  
    ks0108_set_signal(PLCD_E,1);
    ks0108_set_signal(PLCD_CS1,1);
    ks0108_set_signal(PLCD_CS2,1);
    ks0108_set_signal(PLCD_DI,1);
    ks0108_set_signal(PLCD_RW,1);
    ks0108_set_signal(PLCD_DATA,0xFF);

    ks0108_set_signal(PLCD_RST,0);
    ks0108_delay(1000000);
    //usleep(100);

    ks0108_set_signal(PLCD_RST,1);
    //usleep(100);
    ks0108_delay(1000000);

	ks0108_chipselect_1;
	ks0108_write_instruction(0xC0);/* Adress base for line 0 */

	ks0108_chipselect_2;
	ks0108_write_instruction(0xC0);/* Adress base for line 0 */

   ks0108_clear_screen();
	ks0108_display_on();
	ks0108_chipselect_none;
}


// ------ write an instruccion to the selected CSx of the glcd
void ks0108_write_instruction(UI_8 cmd) {

	// it is assumed that the right chip is selected
	ks0108_no_busy();

    ks0108_set_signal(PLCD_E,0);		// enable
    ks0108_set_signal(PLCD_DI,0);		// instruction mode
    ks0108_set_signal(PLCD_RW,0);	// write mode
    ks0108_set_signal(PLCD_DATA,cmd);

	// strobe
	ks0108_delay(DELAY_TWL);
    ks0108_set_signal(PLCD_E, 1);
	ks0108_delay(DELAY_TWH);
    ks0108_set_signal(PLCD_E,0);
	ks0108_delay(DELAY_TDHWR);
	//PLCD_E = 1;
	
}

// ------ write data to the selected CSx of the glcd
void ks0108_write_data(UI_8 cmd) {

	// it is assumed that the right chip is selected
	ks0108_no_busy();

    ks0108_set_signal(PLCD_E,0);		// enable
    ks0108_set_signal(PLCD_DI,1);	// data mode
    ks0108_set_signal(PLCD_RW,0);	// write mode
    ks0108_set_signal(PLCD_DATA,cmd);

	// strobe
	ks0108_delay(DELAY_TWL);
    ks0108_set_signal(PLCD_E,1);
	ks0108_delay(DELAY_TWH);           
    ks0108_set_signal(PLCD_E,0);
	ks0108_delay(DELAY_TDHWR);
	//PLCD_E = 1;
}


// ------ read data from the selected CSx of the glcd
UI_8 ks0108_read_data(void) {

	//ks0108_no_busy();

    ks0108_set_signal(PLCD_DATA,0xFF);
    ks0108_set_signal(PLCD_E,0);		// enable
    ks0108_set_signal(PLCD_DI,1);	// data mode
    ks0108_set_signal(PLCD_RW,1);	// read mode

	// strobe
	ks0108_delay(DELAY_TASU);
    ks0108_set_signal(PLCD_E,1);
	ks0108_delay(DELAY_TD);

    return(ks0108_read_databus());
}

// ------ waits until no busy
void ks0108_no_busy(void) {

	UI_8 wait_cnt;

    ks0108_set_signal(PLCD_DATA,0xFF);
    ks0108_set_signal(PLCD_E,0);		// enable
    ks0108_set_signal(PLCD_DI,0);	// instruction mode
    ks0108_set_signal(PLCD_RW,1);	// read mode

	// strobe
	ks0108_delay(DELAY_TASU);
    //PLCD_E = 1;
    ks0108_set_signal(PLCD_E,1);
	ks0108_delay(DELAY_TWH);

	// faran falta més strobes?

    //return;  // ELIMINAT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	wait_cnt = MAX_BUSY_WAIT;
    //while (((PLCD_DATA & 0x80) != 0) && (wait_cnt !=0)) {
    while (((ks0108_read_databus() & 0x80) != 0) && (wait_cnt !=0)) {
		wait_cnt--;
	}
	
}
