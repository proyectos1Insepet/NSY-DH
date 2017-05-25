void pollingRF_Tx(){
    uint8 x;
    /////////////// COPIA DE RECIBO //////////////////
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
    if(side.b.rfState == RF_COPY_RECEIPT){
        buffer_txDisplay[0] = 0xBC;
        buffer_txDisplay[1] = 0xCB;
        buffer_txDisplay[2] = 0xC8;
        buffer_txDisplay[3] = IDCast[0];
        buffer_txDisplay[4] = IDCast[1];
        buffer_txDisplay[5] = side.b.dir;
        buffer_txDisplay[6] = 0xE6;
        buffer_txDisplay[7] = RF_COPY_RECEIPT;
        buffer_txDisplay[8] = verificar_check(buffer_txDisplay,9);
        for (x = 0; x < 9; x++){
            RF_Connection_PutChar(buffer_txDisplay[x]);
        }
    }
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
    if(side.b.rfState == RF_COPY_RECEIPT){
        buffer_txDisplay[0] = 0xBC;
        buffer_txDisplay[1] = 0xCB;
        buffer_txDisplay[2] = 0xC8;
        buffer_txDisplay[3] = IDCast[0];
        buffer_txDisplay[4] = IDCast[1];
        buffer_txDisplay[5] = side.b.dir;
        buffer_txDisplay[6] = 0xE6;
        buffer_txDisplay[7] = RF_COPY_RECEIPT;
        buffer_txDisplay[8] = verificar_check(buffer_txDisplay,9);
        for (x = 0; x < 9; x++){
            RF_Connection_PutChar(buffer_txDisplay[x]);
        }
    }
    ////////////// DISPENSANDO ////////////////////////////////////
    if((side.a.pumpState == PUMP_BUSY) && RFstateReport == 1){
        RFstateReport = 0;
        buffer_txDisplay[0]  = 0xBC;
        buffer_txDisplay[1]  = 0xCB;
        buffer_txDisplay[2]  = 0xC8;
        buffer_txDisplay[3]  = IDCast[0];
        buffer_txDisplay[4]  = IDCast[1];
        buffer_txDisplay[5]  = side.a.dir;
        buffer_txDisplay[6]  = 0xAA;
        buffer_txDisplay[7]  = RF_DELIVERING;
        buffer_txDisplay[8]  = side.a.activeHose;
        buffer_txDisplay[9]  = bufferDisplay1.presetType[0]&0x03;
        for(x = 17; x >= (10+(10-bufferDisplay1.presetValue[0][0])); x--){
            buffer_txDisplay[x] = (bufferDisplay1.presetValue[0][18-x]); 
        }
        for(x = 10; x <= 17; x++){
            if(buffer_txDisplay[x]== 0x00)
                buffer_txDisplay[x] = 0x30;
        }
        
        buffer_txDisplay[18]  = 0x00;
        if(bufferDisplay1.flagPrint == 0){
            for(x = 0; x <8 ; x++){
                buffer_txDisplay[19+x]=0x20;
            }
        }else{
            for(x = 0; x <8 ; x++){
                buffer_txDisplay[19+x]=bufferDisplay1.licenceSale[x+1];
            }
        }
        for(x=27; x<37;x++){
            buffer_txDisplay[x]= 0x30;
        }
        buffer_txDisplay[37] = verificar_check(buffer_txDisplay,38);
        for (x = 0; x < 39; x++){
            RF_Connection_PutChar(buffer_txDisplay[x]);
        }
        side.a.rfState = RF_DELIVERING;
    }
    ////////////// FIN VENTA ////////////////////////////////////
    if((side.a.pumpState == PUMP_PEOT || side.a.pumpState == PUMP_PEOT) && RFstateReport == 1){
        RFstateReport = 0;
        if(getSale(side.a.dir)){  
            RFstateReport = 0;
            if(bufferDisplay1.flagPrint ==1){
                imprimir(printPortA,side.a.dir);
                flowDisplay1 = 0;
                SetPicture(1,DISPLAY_INICIO0); 
            }else{
                flowDisplay1 = 6;
                bufferDisplay1.flagEndSale = true;
                SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO); 
            }
            bufferDisplay1.flagActiveSale = false;
            buffer_txDisplay[0]  = 0xBC;
            buffer_txDisplay[1]  = 0xCB;
            buffer_txDisplay[2]  = 0xC8;
            buffer_txDisplay[3]  = IDCast[0];
            buffer_txDisplay[4]  = IDCast[1];
            buffer_txDisplay[5]  = side.a.dir;
            buffer_txDisplay[6]  = 0xA4;
            buffer_txDisplay[7]  = RF_CASHSALEREPORT;
            buffer_txDisplay[8]  = side.a.activeHose;
            for(x=1;x <= side.a.ppuSale[0];x++){						   							
        		buffer_txDisplay[x+8]= side.a.ppuSale[x];
        	}
            ///////////////////////////////////////////////////////////////
            if(side.a.volumeSale[1] == 0x30)
                side.a.volumeSale[1] = 0x00;
            if(side.a.volumeSale[1] == 0x00 && side.a.volumeSale[2] ==0x30)
                side.a.volumeSale[2] = 0x00;
            ///////////////////////////////////////////////////////////////
            for(x=1;x <= side.a.volumeSale[0];x++){						   							
                buffer_txDisplay[x+16]=side.a.volumeSale[x];                
        	}
            /////////////////////////////////////////////////////////////
            if(side.a.moneySale[1] ==0x30)
                side.a.moneySale[1] = 0x00;
            if(side.a.moneySale[1] ==0x00 && side.a.moneySale[2] ==0x30)
                side.a.moneySale[2] = 0x00;
            /////////////////////////////////////////////////////////////
            for(x=1;x <= side.a.moneySale[0];x++){						   							
        		buffer_txDisplay[x+24]=side.a.moneySale[x];                
        	}
            
        }else{
            flowDisplay1 = 8;
        }                                                                        
        buffer_txDisplay[29] = verificar_check(buffer_txDisplay,30);
        for (x = 0; x < 30; x++){
            RF_Connection_PutChar(buffer_txDisplay[x]);
        }
        side.a.rfState = RF_DELIVERING;
    }
    
}