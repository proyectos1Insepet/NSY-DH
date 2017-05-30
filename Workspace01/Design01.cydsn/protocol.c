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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
*                                    uint8 get_totals(uint8 pos)
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

uint8 getTotals(uint8 pos){
    uint8 x,y,z,w,gradeHandle;
    Pump_ClearRxBuffer();
    Pump_PutChar(0x50|pos);
    CyWdtClear();
    CyDelay(900);
    CyWdtClear();
    x=Pump_GetRxBufferSize();
    if((x==34)||(x==64)||(x==94)||(x==124)){//Version 5 ó 6 digitos
        if((Pump_rxBuffer[0]==0xFF)&&(Pump_rxBuffer[1]==0xF6)&&(Pump_rxBuffer[3]==0xF9)){
            gradeHandle=(x/30)&0x07;//Identificando cantidad de mangueras en el surtidor
            if(pos==side.a.dir){
                for(x=0;x<4;x++){
                    for(y=0;y<3;y++){
                        for(z=0;z<14;z++){
                            side.a.totalsHandle[x][y][z]=0;
                        }
                    }
                }
                w=0;
                for(x=0;x<gradeHandle;x++){
                    for(y=0;y<=2;y++){
                        for(z=1;z<=8;z++){
                            side.a.totalsHandle[x][y][9-z]=(Pump_rxBuffer[w+4]&0x0F)+0x30;
                            w++;
                            if((y==2)&&(z==4)){
                                break;
                            }
                        }
                        w++;
                    }
                    w=w+7;
                    for(z=4;z>=1;z--){
                        side.a.totalsHandle[x][2][z]=side.a.totalsHandle[x][2][z+4];
                    }
                    side.a.totalsHandle[x][0][0]=8;
                    side.a.totalsHandle[x][1][0]=8;
                    side.a.totalsHandle[x][2][0]=4;
                    if(ppux10==1){
                        side.a.totalsHandle[x][2][0]=5;
                        side.a.totalsHandle[x][2][5]='0';
                    }
                }
                Pump_ClearRxBuffer();
                return gradeHandle;
            }else{
                for(x=0;x<4;x++){
                    for(y=0;y<3;y++){
                        for(z=0;z<14;z++){
                            side.b.totalsHandle[x][y][z]=0;
                        }
                    }
                }
                w=0;
                for(x=0;x<gradeHandle;x++){
                    for(y=0;y<=2;y++){
                        for(z=1;z<=8;z++){
                            side.b.totalsHandle[x][y][9-z]=(Pump_rxBuffer[w+4]&0x0F)+0x30;
                            w++;
                            if((y==2)&&(z==4)){
                                break;
                            }
                        }
                        w++;
                    }
                    w=w+7;
                    for(z=4;z>=1;z--){
                        side.b.totalsHandle[x][2][z]=side.b.totalsHandle[x][2][z+4];
                    }
                    side.b.totalsHandle[x][0][0]=8;
                    side.b.totalsHandle[x][1][0]=8;
                    side.b.totalsHandle[x][2][0]=4;
                    if(ppux10==1){
                        side.b.totalsHandle[x][2][0]=5;
                        side.b.totalsHandle[x][2][5]='0';
                    }
                }
                Pump_ClearRxBuffer();
                return gradeHandle;
            }
        }else{
            Pump_ClearRxBuffer();
            return 0;
        }
    }else if((x==46)||(x==88)||(x==130)||(x==172)){//Version 7 digitos
        if((Pump_rxBuffer[0]==0xFF)&&(Pump_rxBuffer[1]==0xF6)&&(Pump_rxBuffer[3]==0xF9)){
            gradeHandle=(x/40)&0x07;//Identificando cantidad de mangueras en el surtidor
            if(pos==side.a.dir){
                for(x=0;x<4;x++){
                    for(y=0;y<3;y++){
                        for(z=0;z<14;z++){
                            side.a.totalsHandle[x][y][z]=0;
                        }
                    }
                }
                w=0;
                for(x=0;x<gradeHandle;x++){
                    for(y=0;y<=2;y++){
                        for(z=1;z<=12;z++){
                            side.a.totalsHandle[x][y][13-z]=(Pump_rxBuffer[w+4]&0x0F)+0x30;
                            w++;
                            if((y==2)&&(z==6)){
                                break;
                            }
                        }
                        w++;
                    }
                    w=w+9;
                    for(z=6;z>=1;z--){
                        side.a.totalsHandle[x][2][z]=side.a.totalsHandle[x][2][z+6];
                    }
                    side.a.totalsHandle[x][0][0]=12;
                    side.a.totalsHandle[x][1][0]=12;
                    side.a.totalsHandle[x][2][0]=6;
                }
                Pump_ClearRxBuffer();
                return gradeHandle;
            }else{
                for(x=0;x<4;x++){
                    for(y=0;y<3;y++){
                        for(z=0;z<14;z++){
                            side.b.totalsHandle[x][y][z]=0;
                        }
                    }
                }
                w=0;
                for(x=0;x<gradeHandle;x++){
                    for(y=0;y<=2;y++){
                        for(z=1;z<=12;z++){
                            side.b.totalsHandle[x][y][13-z]=(Pump_rxBuffer[w+4]&0x0F)+0x30;
                            w++;
                            if((y==2)&&(z==6)){
                                break;
                            }
                        }
                        w++;
                    }
                    w=w+9;
                    for(z=6;z>=1;z--){
                        side.b.totalsHandle[x][2][z]=side.b.totalsHandle[x][2][z+6];
                    }
                    side.b.totalsHandle[x][0][0]=12;
                    side.b.totalsHandle[x][1][0]=12;
                    side.b.totalsHandle[x][2][0]=6;
                }
                Pump_ClearRxBuffer();
                return gradeHandle;
            }
        }else{
            Pump_ClearRxBuffer();
            return 0;
        }
    }else{
        Pump_ClearRxBuffer();
        return 0;
    }
    Pump_ClearRxBuffer();
    return 0;
}
/*
*********************************************************************************************************
*                                         uint8 price_change(uint8 pos)
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
uint8 priceChange(uint8 pos,uint8 handle,uint8 *value){
	uint8 buffer[15]={0xFF,0,0xF4,0xF6,0,0xF7,0,0,0,0,0,0,0,0,0},size,x,y;
    for(x=1;x<=6;x++){
        if(value[x]==','){
            for(y=x;y>=2;y--){
                value[y]=value[y-1];
            }
            value[1]='0';
            break;
        }
    }
	if(digits!=7){
		buffer[1]=0xE5;
		buffer[4]=(0xE0|((handle&0x0f)));
		if(ppux10==0){
			for(x=0;x<4;x++){
				buffer[6+x]=(0xE0|(value[x]&0x0F));
			}
		}else{
            for(x=0;x<3;x++){
				buffer[6+x]=(0xE0|(value[x]&0x0F));
			}	
		}
		buffer[10]=0xFB;
		for(x=0;x<=10;x++){
			buffer[11]+=(buffer[x]&0x0F);	
		}
		buffer[11]=(((~buffer[11])+1)&0x0F)|0xE0;
		buffer[12]=0xF0;
		size=12;
	}
	if(digits==7){
		buffer[1]=0xE3;
		buffer[4]=(0xE0|((handle&0x0f)-1));	
        for(x=0;x<6;x++){
            buffer[6+x]=(0xE0|(value[6-x]&0x0F));
        }	
		buffer[12]=0xFB;
		for(x=0;x<=12;x++){
			buffer[13]+=(buffer[x]&0x0F);	
		}
		buffer[13]=(((~buffer[13])+1)&0x0F)|0xE0;
		buffer[14]=0xF0;
		size=14;
	}	
    Pump_ClearRxBuffer();
	Pump_PutChar(0x20|pos);	
    CyDelay(100);
    if(Pump_GetRxBufferSize()>=1){
		if(Pump_ReadRxData()==(0xD0|pos)){
            Pump_ClearRxBuffer();
            for(x=0;x<=size;x++){
               	Pump_PutChar(buffer[x]); 	
            }
            Pump_ClearRxBuffer();
			CyDelay(50);
			if(get_state(pos)==0){
				return 0;
			}
			else{
				return 1;
			}
		}
		else{
			return 0;
		}
	}
	else{
		return 0;	
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
    char8 SendComand[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    uint8 state,x,y,z;
    char8 PumpDataReceive =0;    
    x = 0;
    z = 0;
    preset = 0;
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
    if(ConversionFactor == 1){
        VolUnit[0] = 0x47;
        VolUnit[1] = 0x61;
        VolUnit[2] = 0x6C;
        VolUnit[3] = 0x20;
        VolUnit[4] = 0x20;
        VolUnit[5] = 0x20;
    }
    if(ConversionFactor == 2){
        VolUnit[0] = 0x55;
        VolUnit[1] = 0x6B;
        VolUnit[2] = 0x20;
        VolUnit[3] = 0x47;
        VolUnit[4] = 0x61;
        VolUnit[5] = 0x6C;        
    }
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
            CyDelay(500);
        }else{
            CyDelay(150);
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
    CyDelay(250);
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
