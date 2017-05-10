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



DisplayCharacterMap _g_charmaplut[] =
{
    { 0x01, 0x31 }, { 0x02, 0x32 }, { 0x03, 0x33 }, { 0x04, 0x34 }, { 0x05, 0x35 }, { 0x06, 0x36 }, { 0x07, 0x37 }, { 0x08, 0x38 }, { 0x09, 0x39 }, { 0x0A, 0x30 },
    { 0x1B, (char8)'Q' }, { 0x1C, (char8)'W'}, { 0x1D, (char8)'E'}, { 0x1E, (char8)'R'}, { 0x1F, (char8)'T'}, { 0x20, (char8)'Y'}, { 0x21, (char8)'U'}, { 0x22, (char8)'I'}, { 0x23, (char8)'O'}, { 0x24, (char8)'P'},
    { 0x25, (char8)'A'}, { 0x26, (char8)'S'}, { 0x27, (char8)'D'}, { 0x28, (char8)'F'}, { 0x29, (char8)'G'}, { 0x2A, (char8)'H'}, { 0x2B, (char8)'J'}, { 0x2C, (char8)'K'}, { 0x2D, (char8)'L'},
    { 0x2F, (char8)'Z'}, { 0x30, (char8)'X'}, { 0x31, (char8)'C'}, { 0x32, (char8)'V'}, { 0x42, (char8)'B'}, { 0x34, (char8)'N'}, { 0x35, (char8)'M'},
    { 0x6A, (char8)'-'}, { 0x68, (char8)' '}, { 0x51, (char8)','}, { 0x00, 0x00 }
};

//@Created By: HIJH
//@Septembre de 2016
char8 UnmapCode(uint8 code)
{
    char8 retval = 0x00;
    DisplayCharacterMap *pcharmap = _g_charmaplut;
    while(pcharmap->_refchar != 0x00)
    {
        if(pcharmap->_refchar == code)
        {
            retval = pcharmap->_mappedto;
            break;
        }
        pcharmap++;
    }
    return retval;
}




