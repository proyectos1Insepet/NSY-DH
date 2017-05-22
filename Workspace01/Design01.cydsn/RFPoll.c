/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <device.h>
#include <variables.h>
#include <I2C.h>
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
    uint16 i,x;
   
    if(side.a.rfState == RF_COPY_RECEIPT){
        buffer_txDisplay[0] = 0xBC;
        buffer_txDisplay[1] = 0xCB;
        buffer_txDisplay[2] = 0xC8;
        buffer_txDisplay[3] = IDCast[0];
        buffer_txDisplay[4] = IDCast[1];
        buffer_txDisplay[5] = side.a.dir;
        buffer_txDisplay[6] = 0xE6;
        buffer_txDisplay[7] = RF_COPY_RECEIPT;
        buffer_txDisplay[8] = verificar_check(buffer_txDisplay,9);
        for (x = 0; x < 9; x++){
            RF_Connection_PutChar(buffer_txDisplay[x]);
        }
    }
    i=0;
    while((RF_Connection_GetRxBufferSize()>0) && (RF_Connection_RX_STS_FIFO_NOTEMPTY) ){//&& (RF_Connection_RX_STS_FIFO_NOTEMPTY)
       buffer_rf[i]=RF_Connection_ReadRxData(); 	
	   i++;	
    }        
    
    if((buffer_rf[0]==0xBC)&&(buffer_rf[1]==0xBB)&&(buffer_rf[2]==0xB8)&&(buffer_rf[3]==IDCast[0])&&(buffer_rf[4]==IDCast[1])){    
       switch(buffer_rf[6]){
           case 0xA1:               //Peticion de estado
            buffer_tx[0] = 0xBC;
            buffer_tx[1] = 0xCB;
            buffer_tx[2] = 0xC8;
            buffer_tx[3] = IDCast[0];
            buffer_tx[4] = IDCast[1];
            if(buffer_rf[5] == side.a.dir){
                buffer_tx[5] = side.a.dir;
                buffer_tx[6] = 0xA1;
                buffer_tx[7] = side.a.rfState;
            }  
            if(buffer_rf[5] == side.b.dir){
                buffer_tx[5] = side.b.dir;
                buffer_tx[6] = 0xA1;
                buffer_tx[7] = side.b.rfState;
            }
            if(buffer_rf[5] == side.c.dir){
                buffer_tx[5] = side.c.dir;
                buffer_tx[6] = 0xA1;
                buffer_tx[7] = side.c.rfState;
            }
            if(buffer_rf[5] == side.d.dir){
                buffer_tx[5] = side.d.dir;
                buffer_tx[6] = 0xA1;
                buffer_tx[7] = side.d.rfState;
            }
            buffer_tx[8] = verificar_check(buffer_tx,9);
            for (x = 0; x < 9; x++){
                RF_Connection_PutChar(buffer_tx[x]);
            }
           break;
            
           case 0xA6:               //Envio de precios
            buffer_tx[0] = 0xBC;
            buffer_tx[1] = 0xCB;
            buffer_tx[2] = 0xC8;
            buffer_tx[3] = IDCast[0];
            buffer_tx[4] = IDCast[1];
            if(buffer_rf[5] == side.a.dir){
                for(x=13;x > 7; x-- ){
                    side.a.ppuAuthorized[buffer_rf[8]][13-x] = buffer_rf[x];
                }
                buffer_tx[5] = side.a.dir;
                buffer_tx[6] = 0xA6;
                buffer_tx[7] = 0x03;
            } 
            if(buffer_rf[5] == side.b.dir){
                for(x=13;x > 7; x-- ){
                    side.b.ppuAuthorized[buffer_rf[8]][13-x] = buffer_rf[x];
                }
                buffer_tx[5] = side.b.dir;
                buffer_tx[6] = 0xA6;
                buffer_tx[7] = 0x03;
            } 
            if(buffer_rf[5] == side.c.dir){
                for(x=13;x > 7; x-- ){
                    side.c.ppuAuthorized[buffer_rf[8]][13-x] = buffer_rf[x];
                }
                buffer_tx[5] = side.c.dir;
                buffer_tx[6] = 0xA6;
                buffer_tx[7] = 0x03;
            } 
            if(buffer_rf[5] == side.d.dir){
                for(x=13;x > 7; x-- ){
                    side.d.ppuAuthorized[buffer_rf[8]][13-x] = buffer_rf[x];
                }
                buffer_tx[5] = side.d.dir;
                buffer_tx[6] = 0xA6;
                buffer_tx[7] = 0x03;
            }
            buffer_tx[8] = verificar_check(buffer_tx,9);
            for (x = 0; x < 9; x++){
                RF_Connection_PutChar(buffer_tx[x]);
            }
           break;
            
           case 0xA7:               //Impresion general
            buffer_tx[0] = 0xBC;
            buffer_tx[1] = 0xCB;
            buffer_tx[2] = 0xC8;
            buffer_tx[3] = IDCast[0];
            buffer_tx[4] = IDCast[1];
            if(buffer_rf[5] == side.a.dir){
                buffer_tx[5] = side.a.dir;
                buffer_tx[6] = 0xA7;
                buffer_tx[7] = side.a.rfState;
            }  
            if(buffer_rf[5] == side.b.dir){
                buffer_tx[5] = side.b.dir;
                buffer_tx[6] = 0xA7;
                buffer_tx[7] = side.b.rfState;
            }
            if(buffer_rf[5] == side.c.dir){
                buffer_tx[5] = side.c.dir;
                buffer_tx[6] = 0xA7;
                buffer_tx[7] = side.c.rfState;
            }
            if(buffer_rf[5] == side.d.dir){
                buffer_tx[5] = side.d.dir;
                buffer_tx[6] = 0xA7;
                buffer_tx[7] = side.d.rfState;
            }
            buffer_tx[8] = verificar_check(buffer_tx,9);
            for (x = 0; x < 9; x++){
                RF_Connection_PutChar(buffer_tx[x]);
            }
            
           break;
            
           case 0xE1:               //Configuracion de la estacion
            symbols[1] = buffer_rf[8];
            date[0]    = buffer_rf[9];
            date[1]    = buffer_rf[10];
            date[2]    = buffer_rf[11];
            time[1]    = buffer_rf[12];
            time[0]    = buffer_rf[13];
            for(x = 17; x < 45; x++){
                Encabezado1[x-17] = buffer_rf[x];
            }
            CopiasCredito = buffer_rf[16];
            write_hora();
            write_fecha();
           break;
           
           case 0xE2:               //Configuracion de la posicion                                                 
            buffer_tx[0] = 0xBC;
            buffer_tx[1] = 0xCB;
            buffer_tx[2] = 0xC8;
            buffer_tx[3] = IDCast[0];
            buffer_tx[4] = IDCast[1];
            buffer_tx[5] = buffer_rf[5];
            buffer_tx[6] = 0xE2;
            buffer_tx[7] = 0x08;
            buffer_tx[8] = 0x03;
            buffer_tx[9] = verificar_check(buffer_tx,10);
            for (x = 0; x < 10; x++){
                RF_Connection_PutChar(buffer_tx[x]);
            }
            for(x = 8; x < 14; x++){
                GradesHose[8-x] = buffer_rf[x];
            }
           break;
       }                       
    }
   RF_Connection_ClearRxBuffer();            
}



/* [] END OF FILE */
