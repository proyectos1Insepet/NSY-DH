/*
*********************************************************************************************************
*                                         MUX ADVANCE CODE
*
*                             (c) Copyright 2016; Sistemas Insepet LTDA
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                            MUX ADVANCE CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : LCD.c
* Version       : V1.00
* Programmer(s) : 
                  
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include <device.h>
#include "variables.h"
//#include "operations.h"
//#include "I2C.h"
#include "LCD.h"


/*
*********************************************************************************************************
*                                        void SetPicture(uint8 lcd, uint16 picture)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   :  
*
* Note(s)     : none.
*********************************************************************************************************
*/

void SetPicture(uint8 lcd, uint16 picture){
    uint8 buffer[8]={0xAA,0x70,0x00,0x00,0xCC,0x33,0xC3,0x3C},x;
    buffer[2]=(picture>>8)&0xFF;
    buffer[3]=picture&0xFF;
    for(x=0;x<8;x++){
        if(lcd==1){
            Display1_PutChar(buffer[x]);
        }else{
            Display2_PutChar(buffer[x]);
        }
    }
}

/*
***************************************************************************************************************************
* void WriteLCD(uint8_t lcd,uint8_t data, uint16_t posy, uint16_t posx,uint8_t size, uint16_t fgcolor, uint8_t bottomless)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   :  
*
* Note(s)     : none.
***************************************************************************************************************************
*/

void WriteLCD(uint8_t lcd,uint8_t data, uint16_t posy, uint16_t posx,uint8_t size, uint16_t fgcolor, uint8_t bottomless){
uint8_t buffer[18]={0xAA,0x98,0,0,0x01,0x39,0x22,0xC5,0x03,0x00,0x00,0xFF,0xFF,0,0xCC,0x33,0xC3,0x3C},x;
	buffer[2] =(0x0C*(posx*(size+1)))>>8;
	buffer[3] =(0x0C*(posx*(size+1)))&0xFF;
	buffer[4] =(0x0E*posy)>>8;
	buffer[5] =(0x0E*posy)&0xFF;
	buffer[6] =0x20+size;
    if(bottomless=='Y'){
         buffer[7]=0x85;
    }
    buffer[9] =(fgcolor&0xFF00)>>8;
    buffer[10]=(fgcolor&0x00FF);
	buffer[13]=data;
	for(x=0;x<=17;x++){
		if(lcd==1){
			Display1_PutChar(buffer[x]);
		}
		else{
			Display2_PutChar(buffer[x]);
		}	
	}	
}


/*
*********************************************************************************************************
*                                    void ShowRectangle(uint8 lcd)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   :  
*
* Note(s)     : none.
*********************************************************************************************************
*/

void ShowRectangle(uint8 lcd, uint8 coordinate){
    uint8_t buffer[14]={0xAA, 0x5C, 0x00,0x20,0x00,0x20,0x00, 0x30,0x00, 0x30, 0xCC, 0x33, 0xC3, 0x3C},x;  
    buffer[7]=coordinate;
    buffer[9]=coordinate;
    for(x=0;x <=13;x++){
		if(lcd==1){            
			Display1_PutChar(buffer[x]);
		}
		else{
			Display2_PutChar(buffer[x]);
		}	
	}	
    CyDelay(100);
    
}



/*
*********************************************************************************************************
*                             void ShowMessage(uint8 lcd,uint8 *msg)
*
* Description : 
*               
*
* Argument(s) : none
*
* Return(s)   : none
*
* Caller(s)   :  
*
* Note(s)     : none.
*********************************************************************************************************
*/

void ShowMessage(uint8 lcd,uint8 *msg,uint8 posx){
    uint8 x;
    for(x=0;x<10;x++){
        WriteLCD(lcd,msg[x],31,posx + x,1,0x0000,'N');
    }
}




