#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define TESTLED  0


#define LED0 PEout(3)	// PA0


#if TESTLED
    #define LED1 PDout(2)	// PD2
#else
    #define LED1 PEout(2)	// PD2
#endif




void LED_Init(void);//≥ı ºªØ

		 				    
#endif
