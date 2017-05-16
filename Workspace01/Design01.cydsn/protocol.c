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
/*
*********************************************************************************************************
*                  uint8 preset_data(uint8 pos, uint8 grade, uint8 *value, uint8 preset)
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
uint8 PresetData(uint8 side, char8 grade, uint8 *value, uint8 preset){
    char8 SendComand[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},decimal;
    uint8 state,x,y,z;
    char8 PumpDataReceive =0;    
    x = 0;
    z = 0;
    state = get_state(side);
    if(state == 0x06 || state == 0x07){
        Pump_PutChar(0x20|side);
        CyDelay(50);
        PumpDataReceive = Pump_ReadRxData();
    }
    Pump_ClearRxBuffer();
    Pump_ClearRxBuffer();
    if(PumpDataReceive == (0xd0|side)){
        SendComand[x] = SOM; x++;
        SendComand[x] = 0xE1; x++;
        if(side == 1){
            SendComand[x] = 0xF0 | (bufferDisplay1.presetType[0]&0x0F); x++;
            SendComand[x] = 0xF6; x++;
            SendComand[x] = 0xE0 | ((grade-1)&0x0F); x++;
            SendComand[x] = PRDn; x++;
            if(bufferDisplay1.presetType[1] == 'F'){
                for(y = x; y < digits+x; y ++ ){
                    SendComand[y] = 0xE9;                    
                }                
                x = y;
                for(y = x; y < 14; y ++ ){
                    SendComand[y] = 0xE0;
                    x++;
                }                
            }
            if(bufferDisplay1.presetType[1] == 'D'){
                z = value[0];
                for(y = x; y < value[0]+x; y ++ ){
                    SendComand[y] = 0xE0|(value[z]&0x0F); z--;                    
                }                
                x = y;
                for(y = x; y < 14; y ++ ){
                    SendComand[y] = 0xE0;
                    x++;
                }                
            }
            if(bufferDisplay1.presetType[1] == 'V'){
                z = value[0];
                for(y = x; y < value[0]+x; y ++ ){
                    SendComand[y] = 0xE0|(value[z]&0x0F); z--;                    
                }                
                x = y;
                for(y = x; y < 14; y ++ ){
                    SendComand[y] = 0xE0;
                    x++;
                }                
            }
            SendComand[x] = 0xFB;  x++;
            SendComand[x] = GetLRC(SendComand);  x++;
            SendComand[x] = (EOM);
            for(y=0; y<=x;y++){
                Pump_PutChar(SendComand[y]);
            } 
            CyDelay(50);
            return 1;
        }
    }
    return 0;
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
    UnitType            =  buffer [10] &0x0F;
    ConversionFactor    =  buffer [12] &0x0F;
    MoneyDec            = (buffer [14] &0x0F)-1;
    if( (buffer [16]&0x0F) == 1 || (buffer [16]&0x0F) == 4){
        VolDec              = 3;
    }
    if( (buffer [16]&0x0F) == 3 || (buffer [16]&0x0F) == 0){
        VolDec              = 2;
    }
    if( (buffer [16]&0x0F) == 2){
        VolDec              = 1;
    }
    PPUDec              = (buffer [18]&0x0F)-1;
    DDMode              =  buffer [20] &0x0F;
    
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
        if(UnitType > 0){
            CyDelay(700);
        }else{
            CyDelay(200);
        }
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

/*
*********************************************************************************************************
*                                         uint8 get_sale(uint8 pos)
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
uint8 getSale(uint8 pos){
	uint8 x;
	Pump_ClearRxBuffer();
	Pump_PutChar(0x40|pos);
    CyDelay(300);
    CyWdtClear();
	if((digits!=7)&&(Pump_GetRxBufferSize()==33)){
		if((Pump_rxBuffer[0]==0xFF)&&(Pump_rxBuffer[2]==0xF8)&&(Pump_rxBuffer[32]==0xF0)){
			if(pos==side.a.dir){
				side.a.productSale=((Pump_rxBuffer[9]&0x0F)+1)+0x30;
				for(x=1;x<=4;x++){
					side.a.ppuSale[5-x]=((Pump_rxBuffer[x+11]&0x0F)+0x30);
				}
                side.a.ppuSale[0]=4;
                if(ppux10==1){
                    side.a.ppuSale[0]=5;
                    side.a.ppuSale[5]='0';
                }
				for(x=1;x<=6;x++){
					side.a.volumeSale[7-x]=((Pump_rxBuffer[x+16]&0x0F)+0x30);
				}		
                side.a.volumeSale[0]=6;
				for(x=1;x<=6;x++){
					side.a.moneySale[7-x]=((Pump_rxBuffer[x+23]&0x0F)+0x30);
				}
                if(digits==5){
                    for(x=6;x>1;x--){
                        side.a.moneySale[x]=side.a.moneySale[x-1];
                    }
                    side.a.moneySale[1]='0';
                }
                side.a.moneySale[0]=6;
			}
			else{
				side.b.productSale=((Pump_rxBuffer[9]&0x0F)+1)+0x30;
				for(x=1;x<=4;x++){
					side.b.ppuSale[5-x]=((Pump_rxBuffer[x+11]&0x0F)+0x30);
				}
                side.b.ppuSale[0]=4;
                if(ppux10==1){
                    side.b.ppuSale[0]=5;
                    side.b.ppuSale[5]='0';
                }
				for(x=1;x<=6;x++){
					side.b.volumeSale[7-x]=((Pump_rxBuffer[x+16]&0x0F)+0x30);
				}
                side.b.volumeSale[0]=6;
				for(x=1;x<=6;x++){
					side.b.moneySale[7-x]=((Pump_rxBuffer[x+23]&0x0F)+0x30);
				}	
                if(digits==5){
                    for(x=6;x>1;x--){
                        side.b.moneySale[x]=side.b.moneySale[x-1];
                    }
                    side.b.moneySale[1]='0';
                }
                side.b.moneySale[0]=6;
			}
			return 1;
		}
		else{
			return 0;
		}
	}
	else if((digits==7)&&(Pump_GetRxBufferSize()==39)){
		if((Pump_rxBuffer[0]==0xFF)&&(Pump_rxBuffer[2]==0xF8)&&(Pump_rxBuffer[38]==0xF0)){
			if(pos==side.a.dir){
				side.a.productSale=((Pump_rxBuffer[9]&0x0F)+1)+0x30;
				for(x=1;x<=6;x++){
					side.a.ppuSale[7-x]=((Pump_rxBuffer[x+11]&0x0F)+0x30);
				}
                side.a.ppuSale[0]=6;
				for(x=1;x<=8;x++){
					side.a.volumeSale[9-x]=((Pump_rxBuffer[x+18]&0x0F)+0x30);
				}		
                side.a.volumeSale[0]=8;
				for(x=1;x<=7;x++){
					side.a.moneySale[9-x]=((Pump_rxBuffer[x+28]&0x0F)+0x30);
				}
                side.a.moneySale[1]='0';
                side.a.moneySale[0]=8;
			}
			else{
				side.b.productSale=((Pump_rxBuffer[9]&0x0F)+1)+0x30;
				for(x=1;x<=6;x++){
					side.b.ppuSale[7-x]=((Pump_rxBuffer[x+11]&0x0F)+0x30);
				}
                side.b.ppuSale[0]=6;
				for(x=1;x<=8;x++){
					side.b.volumeSale[9-x]=((Pump_rxBuffer[x+18]&0x0F)+0x30);
				}	
                side.b.volumeSale[0]=8;
				for(x=1;x<=7;x++){
					side.b.moneySale[9-x]=((Pump_rxBuffer[x+28]&0x0F)+0x30);
				}	
                side.b.moneySale[1]='0';
                side.b.moneySale[0]=8;
			}
			return 1;
		}
		else{
			return 0;
		}
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
