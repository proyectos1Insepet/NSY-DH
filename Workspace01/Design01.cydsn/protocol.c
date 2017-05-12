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
*                                             MUX ADVANCE CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : protocol.c
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
#include <variables.h>
#include <stdbool.h>
uint8_t GetLRC(char8 *_pbuffer);
/*
*********************************************************************************************************
*                                    uint8 get_state(uint8 pos)
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

uint8 get_state(uint8 pos){
    uint8 state;
    Pump_ClearRxBuffer();
    Pump_PutChar(pos);
    CyDelay(60);
    if(Pump_GetRxBufferSize()>=1){
        state=(Pump_ReadRxData()&0xF0)>>4;
        //state = Pump_ReadRxData();
        Pump_ClearRxBuffer();
        return state;
    }
    else{
        return 0;
    }
}

/*
*********************************************************************************************************
*                                         uint8 get_position(void)
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
uint8 get_position(void){
    uint8 x;            
    side.a.dir=0xff;
    side.b.dir=0xff;
    side.c.dir=0xff;
    side.d.dir=0xff;
    for(x=1;x<=16;x++){
        Pump_PutChar(x);
        CyDelay(100);           
        
        if((Pump_GetRxBufferSize()>=1)&&(side.a.dir==0xff)){
           side.a.dir=x;	
           Pump_ClearRxBuffer();
        }
        if((Pump_GetRxBufferSize()>=1)&&(x!=side.a.dir)&&(side.b.dir==0xff)){
           side.b.dir=x;
           Pump_ClearRxBuffer();
        }
        if((Pump_GetRxBufferSize()>=1)&&(x!=side.a.dir)&&(x!=side.b.dir)&&(side.c.dir==0xff)){
           side.c.dir=x;
           Pump_ClearRxBuffer();
        }
        if((Pump_GetRxBufferSize()>=1)&&(x!=side.a.dir)&&(x!=side.b.dir)&&(x!=side.c.dir)&&(side.d.dir==0xff)){
           side.d.dir=x;
           Pump_ClearRxBuffer();
        }
                        
    }
    if((side.a.dir!=0xff)&&(side.b.dir!=0xff)&&(side.c.dir!=0xff)&&(side.d.dir!=0xff)){
        return 4;
    }
    if((side.a.dir!=0xff)&&(side.b.dir!=0xff)){
        return 2;
    }
    else{
        if((side.a.dir!=0xff)||(side.b.dir!=0xff)){
            return 1;
        }
        else{
            return 0;
        }
    }
}



uint8 PumpCompleteConfiguration( uint8 side){   
    uint8 state,x;
    char8 SendComand[9];
    char8 PumpDataReceive =0;    
    
    uint8 BufferSize = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state = get_state(side);
    if(state == 0x06){
        Pump_PutChar(0x20|side);
        CyDelay(50);
        PumpDataReceive = Pump_ReadRxData();
    }
    Pump_ClearRxBuffer();
    if(PumpDataReceive == (0xd0|side)){    
        SendComand[0] = SOM; //Start Of Message    
        SendComand[1] = (0xE9);//Data Length
        SendComand[2] = (0xFE);//Special Functions Next (0xB0 0xB0 0xBE)
        SendComand[3] = (0xEE);//Complete Pump Configuration Request (sf1)
        SendComand[4] = (0xE0);//Complete Pump Configuration Request (sf2)
        SendComand[5] = (0xE0);//Complete Pump Configuration Request (sf3)
        SendComand[6] = (0xFB);
        SendComand[7] = GetLRC(SendComand);
        SendComand[8] = (EOM);
        for(x=0; x<=8;x++){
            Pump_PutChar(SendComand[x]);
        } 
        CyDelay(200);
    }
    BufferSize =Pump_GetRxBufferSize();
    char8 buffer[BufferSize];
    for(x=0;x<=BufferSize;x++){
       buffer[x]=Pump_ReadRxData(); 
    }
    Pump_ClearRxBuffer();
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    UnitType            = buffer [10] &0x0F;
    ConversionFactor    = buffer [12] &0x0F;
    MoneyDec            = buffer [14] &0x0F;
    VolDec              = buffer [16] &0x0F;
    PPUDec              = buffer [18] &0x0F;
    DDMode              = buffer [20] &0x0F;
    
    return 0;
} 

uint8 PumpHoseActiveState(uint8 side){       
    uint8 state,x;
    char8 SendComand[9];
    char8 PumpDataReceive =0;    
    
    uint8 BufferSize = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    state = get_state(side);
    if(state == PUMP_IDLE || state == PUMP_CALLING){
        Pump_PutChar(0x20|side);
        CyDelay(50);
        PumpDataReceive = Pump_ReadRxData();
    }
    Pump_ClearRxBuffer();
    if(PumpDataReceive == (0xd0|side)){
        SendComand[0] = SOM;//Start Of Message
        SendComand[1] = 0xE9;//Data Length
        SendComand[2] = 0xFE;//Special Functions Next (0xB0 0xB0 0xB6)
        SendComand[3] = 0xE0;//Complete Pump Configuration Request (sf1)
        SendComand[4] = 0xE1;//Complete Pump Configuration Request (sf2)
        SendComand[5] = 0xE0;//Complete Pump Configuration Request (sf3)
        SendComand[6] = 0xFB;
        SendComand[7] = GetLRC(SendComand);
        SendComand[8] = EOM;
        for(x=0; x<=8;x++){
            Pump_PutChar(SendComand[x]);
        } 
        CyDelay(200);
    }    
////////////////////////////////////////////////////////////////////////////////////////////////////////////
    BufferSize =Pump_GetRxBufferSize();
    char8 buffer[BufferSize];
    for(x=0;x<=BufferSize;x++){
       buffer[x]=Pump_ReadRxData(); 
    }
    Pump_ClearRxBuffer();
    if((buffer[0x0C] & 0x0F) == 0x01){
        return (buffer[0x0E] & 0x0F);
    }
    
    return 0;
    
}


bool PumpIsInValidState(state)
{
    bool retval = false;    
    switch(state)
    {
        case PUMP_IDLE:
        case PUMP_CALLING:
        case PUMP_AUTHORIZED:
        case PUMP_BUSY:
        case PUMP_PEOT:
        case PUMP_FEOT:
        case PUMP_STOPPED:
        case PUMP_FAIL:
            retval = true;
            break;
    }
    return retval;
}



uint8_t GetLRC(char8 *_pbuffer)
{
    uint8_t lrc = 0;
    while(*_pbuffer != 0xFB)
    {
        lrc += (*_pbuffer & 0x0F);
        _pbuffer++;
    }
    lrc += (*_pbuffer & 0x0F);
    return (((~lrc + 1) & 0x0F) | 0xE0);
}
