#include <project.h>
#include <protocol.h>
#include <variables.h>
#include <RFPoll.h>
#include <LCD.h>
#include <I2C.h>
#include <Print.h>
#include <device.h>
#include <stdio.h>
#include <stdlib.h>



/*
*********************************************************************************************************
*                             uint8 verificar_check(uint8 *datos, uint16 size)
*
* Description : calcula el checksum
*               

*********************************************************************************************************
*/
uint8 verificar_check(uint8 *datos, uint16 size){
	uint8 checksum,index; 
	uint16 i;
    uint8 table[256] = { 
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
    190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
    219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
    87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};	
	checksum=0;
	for(i=0;i<(size-1);i++){
		index = (uint8)(checksum ^ datos[i]);
		checksum = table[index];				
	}
	return checksum;
}




void pollingRF_Rx(){
    uint16 i,x,y;       
    CyDelay(50);
    if(RF_Connection_GetRxBufferSize() >= 1){
        ActiveRF=1;
        counterRF = 0;
        i=0;
        while(RF_Connection_GetRxBufferSize()>0){//&& (RF_Connection_RX_STS_FIFO_NOTEMPTY)
           buffer_rf[i]=RF_Connection_ReadRxData(); 	
    	   i++;	
        } 
        y=0;
        buffer_tx[y] = 0xBC; y++;
        buffer_tx[y] = 0xCB; y++;
        buffer_tx[y] = 0xC8; y++;
        buffer_tx[y] = IDCast[0]; y++;
        buffer_tx[y] = IDCast[1]; y++;
        if(((buffer_rf[0]==0xBC)&&(buffer_rf[1]==0xBB)&&(buffer_rf[2]==0xB8)&&(buffer_rf[3]==IDCast[0])&&(buffer_rf[4]==IDCast[1]))){                  
            switch(buffer_rf[6]){
               case 0xA1:               //Peticion de estado
                if(buffer_rf[5] == side.a.dir){
                    if(bufferAready == 0){                        
                        buffer_tx[5] = side.a.dir;y++;
                        buffer_tx[y] = 0xA1;y++;
                        buffer_tx[y] = side.a.rfState; y++;
                        buffer_tx[y] = verificar_check(buffer_tx,y+1);y++;
                        for (x = 0; x < y; x++){
                            RF_Connection_PutChar(buffer_tx[x]);
                        }
                    }else{
                        for (x = 1; x < buffer_A[0]; x++){
                            RF_Connection_PutChar(buffer_A[x]);
                        }
                        bufferAready = 0;
                    } 
                }
                if(buffer_rf[5] == side.b.dir){
                    if(side.b.RFstateReport == 0){
                        buffer_tx[5] = side.b.dir;y++;
                        buffer_tx[y] = 0xA1;y++;
                        buffer_tx[y] = side.b.rfState;y++;
                        buffer_tx[y] = verificar_check(buffer_tx,y+1);y++;
                        for (x = 0; x < y; x++){
                            RF_Connection_PutChar(buffer_tx[x]);
                        }
                    }else{
                        pollingRFB_Tx();
                    } 
                }
                if(buffer_rf[5] == side.c.dir){
                    buffer_tx[5] = side.c.dir; y++;
                    buffer_tx[y] = 0xA1; y++;
                    buffer_tx[y] = side.c.rfState; y++;
                    buffer_tx[y] = verificar_check(buffer_tx,y+1);y++;
                    for (x = 0; x < y; x++){
                        RF_Connection_PutChar(buffer_tx[x]);
                    }
                }
                if(buffer_rf[5] == side.d.dir){
                    buffer_tx[5] = side.d.dir; y++;
                    buffer_tx[y] = 0xA1; y++;
                    buffer_tx[y] = side.d.rfState; y++;
                    buffer_tx[y] = verificar_check(buffer_tx,y+1);y++;
                    for (x = 0; x < y; x++){
                        RF_Connection_PutChar(buffer_tx[x]);
                    }
                }                    
               break;
               
               
               case 0xA5:
                if(buffer_rf[5] == side.a.dir){                      
                    pollTotals = 1;
                    buffer_tx[5] = side.a.dir;y++;
                    buffer_tx[y] = 0xA1;y++;
                    buffer_tx[y] = side.a.rfState; y++;
                    buffer_tx[y] = verificar_check(buffer_tx,y+1);y++;
                    for (x = 0; x < y; x++){
                        RF_Connection_PutChar(buffer_tx[x]);
                    }                    
                }                
                if(buffer_rf[5] == side.b.dir){                      
                    pollTotals = 2;
                    buffer_tx[5] = side.b.dir;y++;
                    buffer_tx[y] = 0xA1;y++;
                    buffer_tx[y] = side.b.rfState; y++;
                    buffer_tx[y] = verificar_check(buffer_tx,y+1);y++;
                    for (x = 0; x < y; x++){
                        RF_Connection_PutChar(buffer_tx[x]);
                    }                    
                }                 
                
               break;
                
               case 0xA6:               //Envio de precios
                if(buffer_rf[5] == side.a.dir){                        
                    for(x=9;x < 14 ; x++ ){
                        side.a.ppuAuthorized[buffer_rf[8]][x-9] = buffer_rf[x];
                    }
                    side.a.changePPU = true;
                    side.a.grade = buffer_rf[8];
                    buffer_tx[5] = side.a.dir;y++;
                    buffer_tx[y] = 0xA6;y++;
                    buffer_tx[y] = 0x03;y++;
                } 
                if(buffer_rf[5] == side.b.dir){                        
                    for(x=9;x < 14 ; x++ ){
                        side.b.ppuAuthorized[buffer_rf[8]][x-9] = buffer_rf[x];
                    }
                    side.b.changePPU = true;
                    side.b.grade = buffer_rf[8];
                    buffer_tx[5] = side.b.dir; y++;
                    buffer_tx[y] = 0xA6;y++;
                    buffer_tx[y] = 0x03;y++;
                } 
                if(buffer_rf[5] == side.c.dir){                        
                    for(x=9;x < 14 ; x++ ){
                        side.c.ppuAuthorized[buffer_rf[8]][x-9] = buffer_rf[x];
                    }
                    side.c.changePPU = true;
                    side.c.grade = buffer_rf[8];
                    buffer_tx[5] = side.c.dir; y++;
                    buffer_tx[y] = 0xA6;y++;
                    buffer_tx[y] = 0x03;y++;
                } 
                if(buffer_rf[5] == side.d.dir){                        
                    for(x=9;x < 14 ; x++ ){
                        side.d.ppuAuthorized[buffer_rf[8]][x-9] = buffer_rf[x];
                    }
                    side.d.changePPU = true;
                    side.d.grade = buffer_rf[8];
                    buffer_tx[5] = side.d.dir; y++;
                    buffer_tx[y] = 0xA6;y++;
                    buffer_tx[y] = 0x03;y++;
                }
                buffer_tx[y] = verificar_check(buffer_tx,y+1);y++;
                for (x = 0; x < y; x++){
                    RF_Connection_PutChar(buffer_tx[x]);
                }
               break;
                
               case 0xA7:               //Impresion general
                for(x = 9; x <= buffer_rf[8];x++){
                    //buffer_print[x-9] = buffer_rf[x];
                    write_psoc1(printPortA,buffer_rf[x]);
                }
                if(buffer_rf[5] == side.a.dir){
                    buffer_tx[5] = side.a.dir;
                    buffer_tx[6] = 0xA7;
                    buffer_tx[7] = side.a.rfState;
                    buffer_tx[8] = 0x03;
                }  
                if(buffer_rf[5] == side.b.dir){
                    buffer_tx[5] = side.b.dir;
                    buffer_tx[6] = 0xA7;
                    buffer_tx[7] = side.b.rfState;
                    buffer_tx[8] = 0x03;
                }
                if(buffer_rf[5] == side.c.dir){
                    buffer_tx[5] = side.c.dir;
                    buffer_tx[6] = 0xA7;
                    buffer_tx[7] = side.c.rfState;
                    buffer_tx[8] = 0x03;
                }
                if(buffer_rf[5] == side.d.dir){
                    buffer_tx[5] = side.d.dir;
                    buffer_tx[6] = 0xA7;
                    buffer_tx[7] = side.d.rfState;
                    buffer_tx[8] = 0x03;
                }
                buffer_tx[8] = verificar_check(buffer_tx,9);
                for (x = 0; x < 9; x++){
                    RF_Connection_PutChar(buffer_tx[x]);
                }
                
                
               break;
                
               case 0xA9:
                if(buffer_rf[5] == side.a.dir){
                    for(x = 0; x < 10; x++){
                        side.a.saleNumber[x] = buffer_rf[x+8];
                    }
                    if(bufferDisplay1.flagPrint)
                        imprimir(printPortA,side.a.dir); 
                }
                if(buffer_rf[5] == side.b.dir){
                    for(x = 0; x < 10; x++){
                        side.b.saleNumber[x] = buffer_rf[x+8];
                    }
                    if(bufferDisplay2.flagPrint)
                        imprimir(printPortB,side.b.dir); 
                }
                if(buffer_rf[5] == side.c.dir){
                    for(x = 0; x < 10; x++){
                        side.c.saleNumber[x] = buffer_rf[x+8];
                    }
                    if(bufferDisplay3.flagPrint)
                        imprimir(printPortA,side.c.dir); 
                }
                if(buffer_rf[5] == side.d.dir){
                    for(x = 0; x < 10; x++){
                        side.d.saleNumber[x] = buffer_rf[x+8];
                    }
                    if(bufferDisplay4.flagPrint)
                        imprimir(printPortB,side.d.dir); 
                }
               break;
               case 0xAB:
                buffer_tx[0] = 0xBC;
               break;
                
               case 0xE1:               //Configuracion de la estacion
                symbols[1] = buffer_rf[8];
                date[0]    = buffer_rf[9];
                date[1]    = buffer_rf[10];
                date[2]    = buffer_rf[11];
                time[1]    = buffer_rf[12];
                time[0]    = buffer_rf[13];
                for(x = 17; x < 47; x++){
                    Encabezado1[x-17] = buffer_rf[x];
                }
                for(x = 47; x < 77; x++){
                    Encabezado2[x-47] = buffer_rf[x];
                }
                for(x = 77; x < 107; x++){
                    Encabezado3[x-77] = buffer_rf[x];
                }
                for(x = 107; x < 137; x++){
                    Encabezado4[x-77] = buffer_rf[x];
                }
                CopiasCredito = buffer_rf[16];
                write_hora();
                write_fecha(); 
                for(x =0 ; x <30; x++){
                    EEPROM_1_WriteByte(Encabezado1[x],16+x);
                }
                for(x =30 ; x <60; x++){
                    EEPROM_1_WriteByte(Encabezado2[x-30],16+x);
                }
                for(x =60 ; x <90; x++){
                    EEPROM_1_WriteByte(Encabezado3[x-30],16+x);
                }
                for(x =90 ; x <120; x++){
                    EEPROM_1_WriteByte(Encabezado4[x-30],16+x);
                }
                side.a.dir = EEPROM_1_ReadByte(12);
                side.b.dir = EEPROM_1_ReadByte(13);
                side.c.dir = EEPROM_1_ReadByte(14);
                side.d.dir = EEPROM_1_ReadByte(15);
                imprimir(printPortA,side.a.dir);
               break;
               
               case 0xE2:               //Configuracion de la posicion                                                 
                buffer_tx[5] = buffer_rf[5];
                buffer_tx[6] = 0xE2;
                buffer_tx[7] = 0x08;
                buffer_tx[8] = 0x03;
                buffer_tx[9] = verificar_check(buffer_tx,10);
                for (x = 0; x < 10; x++){
                    RF_Connection_PutChar(buffer_tx[x]);
                }
                for(x = 8; x < 12; x++){
                    GradesHose[x-8] = buffer_rf[x];
                }
                lockTurn = buffer_rf[12];
                EEPROM_1_WriteByte(lockTurn,7);
               break;
                
               case 0xE4:               //Turno                                                 
                if(buffer_rf[7]== 0x02){
                    flowDisplay1 = 0;
                    ShiftState = 0;
                    side.a.rfState = RF_IDLE;
                    side.a.RFstateReport = 0;
                    SetPicture(1,DISPLAY_ERROR);
                }else{
                    ShiftState = 0;
                    lockTurn = buffer_rf[7];
                    flowDisplay1 = 0;
                    side.a.RFstateReport = 0;
                    CyDelay(200);
                    SetPicture(1,DISPLAY_ID_RECONOCIDO);
                }
                    
               break;
           }                       
        } 
        RF_Connection_ClearRxBuffer(); 
    }                            
    //countRX = 0;            
}


void pollingRFA_Tx(){   
    uint16 x,y,z;     
    if(countTX >15){
    /////////////// COPIA DE RECIBO //////////////////
//    if(side.a.rfState == RF_COPY_RECEIPT && side.a.RFstateReport == 1){        
//        buffer_txDisplay[5] = side.a.dir;
//        buffer_txDisplay[6] = 0xE6;
//        buffer_txDisplay[7] = RF_COPY_RECEIPT;
//        buffer_txDisplay[8] = verificar_check(buffer_txDisplay,9);
//        for (x = 0; x < 9; x++){
//            RF_Connection_PutChar(buffer_txDisplay[x]);
//        }
//		side.a.RFstateReport = 0;
//    }    
    ////////////// PRESET - DISPENSANDO ////////////////////////////////////
    if(side.a.pumpState == PUMP_BUSY && side.a.RFstateReport == 1){
        buffer_A[0] = 40;
		buffer_A[1] = 0xBC;
        buffer_A[2] = 0xCB;
        buffer_A[3] = 0xC8;
        buffer_A[4] = IDCast[0];
        buffer_A[5] = IDCast[1];
        buffer_A[6]  = side.a.dir;
        buffer_A[7]  = 0xAA;
        buffer_A[8]  = RF_DELIVERING;
        buffer_A[9]  = side.a.activeHose;
        buffer_A[10]  = bufferDisplay1.presetType[0]&0x03;
        for(x = bufferDisplay1.presetValue[0][0] ; x > 0; x--){
            tempPreset[bufferDisplay1.presetValue[0][0]-x] = (bufferDisplay1.presetValue[0][x]); 
        }
        for(x = 11; x<=18; x++){
            buffer_A[x] = tempPreset[x-11]; 
            if(buffer_A[x]== 0x00)
                buffer_A[x] = 0x30;
        }        
        buffer_A[19]  = 0x00;
        if(bufferDisplay1.flagPrint == 0){
            for(x = 0; x <8 ; x++){
                buffer_A[20+x]=0x20;
                tempPreset[x] = 0x00;
            }
        }else{
            for(x = 0; x <8 ; x++){
                buffer_A[20+x]=bufferDisplay1.licenceSale[x+1];
                tempPreset[x] = 0x00;
            }
        }
        for(x=28; x<=38;x++){
            buffer_A[x]= 0x30;
        }
        buffer_A[39] = verificar_check(buffer_A,40);                                
        side.a.rfState = RF_DELIVERING;
        side.a.RFstateReport = 0;
        bufferAready =1;
    }          
    ////////////// FIN VENTA ////////////////////////////////////
    if((side.a.pumpState == PUMP_PEOT || side.a.pumpState == PUMP_FEOT) && side.a.RFstateReport == 1){  //bufferDisplay1.flagEndSale == true        
        buffer_A[0] = 32;
		buffer_A[1] = 0xBC;
        buffer_A[2] = 0xCB;
        buffer_A[3] = 0xC8;
        buffer_A[4] = IDCast[0];
        buffer_A[5] = IDCast[1];
        buffer_A[6]  = side.a.dir;
        buffer_A[7]  = 0xA4;
        buffer_A[8]  = RF_CASHSALEREPORT;
        buffer_A[9]  = side.a.activeHose-1;
        for(x=side.a.ppuSale[0];x > 0 ;x--){						   							
    		tempPPU[side.a.ppuSale[0]-x]= side.a.ppuSale[x];
    	}
        for(x = 10; x<=14; x++){
            buffer_A[x] = tempPPU[x-10]; 
            if(buffer_A[x]== 0x00)
                buffer_A[x] = 0x30;
        } 
        if(digits ==5 || digits ==6){
            for(x=1;x <= side.a.volumeSale[0];x++){						   							
                buffer_A[x+14]=side.a.volumeSale[7-x];                
        	}
        }else{
            for(x=1;x <= side.a.volumeSale[0];x++){						   							
                buffer_A[x+14]=side.a.volumeSale[9-x];                
        	}
        }        
        if(buffer_A[21]==0x00){
            buffer_A[21] = 0x30;
            buffer_A[22] = 0x30;
        }
        if(digits ==5 || digits ==6){
            for(x=1;x <= side.a.moneySale[0];x++){						   							
        		buffer_A[x+22]=side.a.moneySale[7-x];                
        	} 
        }else{
            for(x=1;x <= side.a.moneySale[0];x++){						   							
        		buffer_A[x+22]=side.a.moneySale[9-x];                
        	}            
        }
        if(buffer_A[29]==0x00){
            buffer_A[29] = 0x30;
            buffer_A[30] = 0x30;
        }
        buffer_A[31] = verificar_check(buffer_A,32);                          
        side.a.pumpState = PUMP_IDLE;
        side.a.rfState = RF_IDLE;
        bufferDisplay1.flagActiveSale = false;
        side.a.RFstateReport = 0;
        pollTotals = 1;
        bufferAready = 1;        
    }                                               
    ////////////// TURNOS ////////////////////////////////////
    if(ShiftState ==1  && side.a.RFstateReport == 1){   
        buffer_txDisplay[0] = 0xBC;
        buffer_txDisplay[1] = 0xCB;
        buffer_txDisplay[2] = 0xC8;
        buffer_txDisplay[3] = IDCast[0];
        buffer_txDisplay[4] = IDCast[1];
        buffer_txDisplay[5]  = side.a.dir;
        buffer_txDisplay[6]  = 0xE3;
        buffer_txDisplay[7]  = RF_WORKSHIFTREQ;
        for(x=bufferDisplay1.shiftId[0];x >=1  ;x--){						   							
    		buffer_txDisplay[x+7]= bufferDisplay1.shiftId[x];            
    	}
//        for(x=8; x<19; x++){
//            if(buffer_txDisplay[x] ==0x00)
//                buffer_txDisplay[x]=0x30;
//        }
        for(x=1;x <= bufferDisplay1.shiftPassword[0] ;x++){						   							
    		buffer_txDisplay[x+18]= bufferDisplay1.shiftPassword[(bufferDisplay1.shiftPassword[0]+1)-x];            
    	}
//        for(x=19; x<31; x++){
//            if(buffer_txDisplay[x] ==0x00)
//                buffer_txDisplay[x]=0x30;
//        }
        buffer_txDisplay[32] = verificar_check(buffer_txDisplay,33);
        for (x = 0; x < 33; x++){
            RF_Connection_PutChar(buffer_txDisplay[x]);
        }
        side.a.rfState = RF_IDLE;
        side.a.RFstateReport = 0;
    }
    ////////////// CAMBIO PPU ////////////////////////////////////
    if(side.a.changePPU){
        if(priceChange(side.a.dir,side.a.grade,side.a.ppuAuthorized[side.a.grade]))
            side.a.changePPU = false;          
    }    
    ////////////// TOTALES ////////////////////////////////////
    
    if(side.a.RFstateReport == 1 && pollTotals ==1){
        pollTotals = 0;
        y=0;
        buffer_txTotals[y] = 0xBC; y++;
        buffer_txTotals[y] = 0xCB; y++;
        buffer_txTotals[y] = 0xC8; y++;
        buffer_txTotals[y] = IDCast[0]; y++;
        buffer_txTotals[y] = IDCast[1]; y++;
        buffer_txTotals[5] = side.a.dir; y++;
        buffer_txTotals[y] = 0xE8; y++;
        buffer_txTotals[y] = side.a.rfState; y++;
        buffer_txTotals[y] = 0x00; y++;
        for(z=1;z <= side.a.hoseNumber; z++){
            buffer_txTotals[y] = z; y++;                    
            if (digits == 6 || digits == 5){                            
                for(x = 8; x > 0; x--){ //Volumen
                    buffer_txTotals[y] = (side.a.totalsHandle[z-1][0][x])-48;y++;
                }
                for(x =0; x < 4; x++){
                    buffer_txTotals[y] = 0x30;y++;
                }
                for(x = 8; x > 0; x--){//Dinero
                    buffer_txTotals[y] = (side.a.totalsHandle[z-1][1][x])-48;y++;
                }
                for(x =0; x < 4; x++){
                    buffer_txTotals[y] = 0x30;y++;
                }
            }else{
                for(x = 12; x > 0; x--){ //Volumen
                    buffer_txTotals[y] = (side.a.totalsHandle[z-1][0][x])-48;y++;
                }                        
                for(x = 12; x > 0; x--){//Dinero
                    buffer_txTotals[y] = (side.a.totalsHandle[z-1][1][x])-48;y++;
                }                        
            } 
        }
        buffer_txTotals[8] = y-9;
        buffer_txTotals[y] = verificar_check(buffer_txTotals,y+1);y++;                 
        for (x = 0; x < y; x++){
            RF_Connection_PutChar(buffer_txTotals[x]);
        }        
        side.a.RFstateReport = 0;        
    }    
    countTX = 0; 
    }
    
}

void pollingRFB_Tx(){   
    uint16 x,y,z;      
    /////////////// COPIA DE RECIBO //////////////////
//    if(side.b.rfState == RF_COPY_RECEIPT && side.b.RFstateReport == 1){        
//        buffer_txDisplay[5] = side.b.dir;
//        buffer_txDisplay[6] = 0xE6;
//        buffer_txDisplay[7] = RF_COPY_RECEIPT;
//        buffer_txDisplay[8] = verificar_check(buffer_txDisplay,9);
//        for (x = 0; x < 9; x++){
//            RF_Connection_PutChar(buffer_txDisplay[x]);
//        }
//		side.b.RFstateReport = 0;
//    }    
    ////////////// PRESET - DISPENSANDO ////////////////////////////////////
    if(side.b.pumpState == PUMP_BUSY && side.b.RFstateReport == 1){
            buffer_txPreset[0] = 0xBC;
            buffer_txPreset[1] = 0xCB;
            buffer_txPreset[2] = 0xC8;
            buffer_txPreset[3] = IDCast[0];
            buffer_txPreset[4] = IDCast[1];
            buffer_txPreset[5]  = side.b.dir;
            buffer_txPreset[6]  = 0xAA;
            buffer_txPreset[7]  = RF_DELIVERING;
            buffer_txPreset[8]  = side.b.activeHose;
            buffer_txPreset[9]  = bufferDisplay2.presetType[0]&0x03;
            for(x = bufferDisplay2.presetValue[0][0] ; x > 0; x--){
                tempPreset[bufferDisplay2.presetValue[0][0]-x] = (bufferDisplay2.presetValue[0][x]); 
            }
            for(x = 10; x<=17; x++){
                buffer_txPreset[x] = tempPreset[x-10]; 
                if(buffer_txPreset[x]== 0x00)
                    buffer_txPreset[x] = 0x30;
            }        
            buffer_txPreset[18]  = 0x00;
            if(bufferDisplay2.flagPrint == 0){
                for(x = 0; x <8 ; x++){
                    buffer_txPreset[19+x]=0x20;
                    tempPreset[x] = 0x00;
                }
            }else{
                for(x = 0; x <8 ; x++){
                    buffer_txPreset[19+x]=bufferDisplay2.licenceSale[x+1];
                    tempPreset[x] = 0x00;
                }
            }
            for(x=27; x<=37;x++){
                buffer_txPreset[x]= 0x30;
            }
            buffer_txPreset[38] = verificar_check(buffer_txPreset,39);            
            for (x = 0; x < 40; x++){
                RF_Connection_PutChar(buffer_txPreset[x]);
            }            
            side.b.rfState = RF_DELIVERING;
            side.b.RFstateReport = 0;
    }      
    ////////////// FIN VENTA ////////////////////////////////////
    if(bufferDisplay2.flagEndSale == true && side.b.RFstateReport == 1){
        buffer_txEOT[0] = 0xBC;
        buffer_txEOT[1] = 0xCB;
        buffer_txEOT[2] = 0xC8;
        buffer_txEOT[3] = IDCast[0];
        buffer_txEOT[4] = IDCast[1];
        buffer_txEOT[5]  = side.b.dir;
        buffer_txEOT[6]  = 0xA4;
        buffer_txEOT[7]  = RF_CASHSALEREPORT;
        buffer_txEOT[8]  = side.b.activeHose;
        for(x=side.b.ppuSale[0];x > 0 ;x--){						   							
    		tempPPU[side.b.ppuSale[0]-x]= side.b.ppuSale[x];
    	}
        for(x = 9; x<=13; x++){
            buffer_txEOT[x] = tempPPU[x-9]; 
            if(buffer_txEOT[x]== 0x00)
                buffer_txEOT[x] = 0x30;
        } 
        if(digits ==5 || digits ==6){
            for(x=1;x <= side.b.volumeSale[0];x++){						   							
                buffer_txEOT[x+13]=side.b.volumeSale[7-x];                
        	}
        }else{
            for(x=1;x <= side.b.volumeSale[0];x++){						   							
                buffer_txEOT[x+13]=side.b.volumeSale[9-x];                
        	}
        }        
        if(buffer_txEOT[20]==0x00){
            buffer_txEOT[20] = 0x30;
            buffer_txEOT[21] = 0x30;
        }
        if(digits ==5 || digits ==6){
            for(x=1;x <= side.b.moneySale[0];x++){						   							
        		buffer_txEOT[x+21]=side.b.moneySale[7-x];                
        	} 
        }else{
            for(x=1;x <= side.b.moneySale[0];x++){						   							
        		buffer_txEOT[x+21]=side.b.moneySale[9-x];                
        	}            
        }
        if(buffer_txEOT[28]==0x00){
            buffer_txEOT[28] = 0x30;
            buffer_txEOT[29] = 0x30;
        }
        buffer_txEOT[30] = verificar_check(buffer_txEOT,31);        
        for (x = 0; x < 31; x++){
            RF_Connection_PutChar(buffer_txEOT[x]);
        }  
        side.a.pumpState = PUMP_IDLE;
        side.b.rfState = RF_IDLE;
        bufferDisplay2.flagActiveSale = false;
        side.b.RFstateReport = 0;
        pollTotals = 2;
    }                                               
    ////////////// TURNOS ////////////////////////////////////
//    if(ShiftState ==1  && side.b.RFstateReport == 1){                                    
//        buffer_txDisplay[5]  = side.b.dir;
//        buffer_txDisplay[6]  = 0xE3;
//        buffer_txDisplay[7]  = RF_WORKSHIFTREQ;
//        for(x=1;x <= bufferDisplay2.shiftId[0] ;x++){						   							
//    		buffer_txDisplay[x+7]= bufferDisplay2.shiftId[(bufferDisplay2.shiftId[0]+1)-x];            
//    	}
//        for(x=8; x<19; x++){
//            if(buffer_txDisplay[x] ==0x00)
//                buffer_txDisplay[x]=0x30;
//        }
//        for(x=1;x <= bufferDisplay2.shiftPassword[0] ;x++){						   							
//    		buffer_txDisplay[x+18]= bufferDisplay2.shiftPassword[(bufferDisplay2.shiftPassword[0]+1)-x];            
//    	}
//        for(x=19; x<31; x++){
//            if(buffer_txDisplay[x] ==0x00)
//                buffer_txDisplay[x]=0x30;
//        }
//        buffer_txDisplay[32] = verificar_check(buffer_txDisplay,33);
//        for (x = 0; x < 33; x++){
//            RF_Connection_PutChar(buffer_txDisplay[x]);
//        }
//        side.b.rfState = RF_WORKSHIFTREQ;
//        side.b.RFstateReport = 0;
//    }
    ////////////// CAMBIO PPU ////////////////////////////////////
    if(side.b.changePPU){
        if(priceChange(side.b.dir,side.b.grade,side.b.ppuAuthorized[side.b.grade]))
            side.b.changePPU = false;          
    }
    ////////////// TOTALES ////////////////////////////////////
    
    if(side.b.RFstateReport == 1 && pollTotals == 2){
        pollTotals = 0;
        y=0;
        buffer_txTotals[y] = 0xBC; y++;
        buffer_txTotals[y] = 0xCB; y++;
        buffer_txTotals[y] = 0xC8; y++;
        buffer_txTotals[y] = IDCast[0]; y++;
        buffer_txTotals[y] = IDCast[1]; y++;
        buffer_txTotals[5] = side.b.dir; y++;
        buffer_txTotals[y] = 0xE8; y++;
        buffer_txTotals[y] = side.b.rfState; y++;
        buffer_txTotals[y] = 0x00; y++;
        for(z=1;z <= side.b.hoseNumber; z++){
            buffer_txTotals[y] = z; y++;                    
            if (digits == 6 || digits == 5){                            
                for(x = 8; x > 0; x--){ //Volumen
                    buffer_txTotals[y] = (side.b.totalsHandle[z-1][0][x])-48;y++;
                }
                for(x =0; x < 4; x++){
                    buffer_txTotals[y] = 0x30;y++;
                }
                for(x = 8; x > 0; x--){//Dinero
                    buffer_txTotals[y] = (side.b.totalsHandle[z-1][1][x])-48;y++;
                }
                for(x =0; x < 4; x++){
                    buffer_txTotals[y] = 0x30;y++;
                }
            }else{
                for(x = 12; x > 0; x--){ //Volumen
                    buffer_txTotals[y] = (side.b.totalsHandle[z-1][0][x])-48;y++;
                }                        
                for(x = 12; x > 0; x--){//Dinero
                    buffer_txTotals[y] = (side.b.totalsHandle[z-1][1][x])-48;y++;
                }                        
            } 
        }
        buffer_txTotals[8] = y-9;
        buffer_txTotals[y] = verificar_check(buffer_txTotals,y+1);y++;                 
        for (x = 0; x < y; x++){
            RF_Connection_PutChar(buffer_txTotals[x]);
        }        
        side.b.RFstateReport = 0;        
    }    
 }
