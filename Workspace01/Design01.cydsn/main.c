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
#include <project.h>
#include <protocol.h>
#include <variables.h>
#include <keyboard.h>
#include <ibutton.h>
#include <RFPoll.h>
#include <LCD.h>
#include <I2C.h>
#include <Print.h>
#include <device.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************/

uint8 MuxVersion [10] = "MUX V. 1.0";
uint8 passwordPump[5] = "00204";  


      
CY_ISR(animacion2);
CY_ISR(animacion);
//CY_ISR(timerPump);
CY_ISR(timerRFRX);
//CY_ISR(pollingRF_Tx);
void GlobalInitializer()
{
    EEPROM_1_Start();
    Pump_Start(); 
    Display1_Start();
    Display2_Start();    
    Printer_Start();   
    I2C_Bus_Start();
    RF_Connection_Start();
    isr_1_StartEx(timerRFRX); 
    Waitable_3_Start();
    //isr_2_StartEx(timerPump); 
    Waitable_4_Start();
    //isr_6_StartEx(pollingRF_Tx);
}
void StoreConfiguration(){
    EEPROM_1_WriteByte(UnitType,0);
    EEPROM_1_WriteByte(ConversionFactor,1);
    EEPROM_1_WriteByte(MoneyDec,2);
    EEPROM_1_WriteByte(VolDec,3);
    EEPROM_1_WriteByte(PPUDec,4);
    EEPROM_1_WriteByte(DDMode,5);
    EEPROM_1_WriteByte(digits,6); 
    EEPROM_1_WriteByte(side.a.dir,12);
    EEPROM_1_WriteByte(side.b.dir,13);
    EEPROM_1_WriteByte(side.c.dir,14);
    EEPROM_1_WriteByte(side.d.dir,15);
}
void loadConfiguration(){
    //uint8 x;
    MoneyDec   = EEPROM_1_ReadByte(2);
    VolDec     = EEPROM_1_ReadByte(3);
    PPUDec     = EEPROM_1_ReadByte(4);
    DDMode     = EEPROM_1_ReadByte(5);
    digits     = EEPROM_1_ReadByte(6);
    lockTurn   = EEPROM_1_ReadByte(7);  //Fijo turno abierto para pruebas
    printPortA = EEPROM_1_ReadByte(8);  //Puertos de impresion
    printPortB = EEPROM_1_ReadByte(9); //Puertos de impresion
    IDCast[0]  = EEPROM_1_ReadByte(10); //ID Estacion1
    IDCast[1]  = EEPROM_1_ReadByte(11); //ID Estacion2    
    passwordPump[0] = 0x04;
    configAccess[0] = 0x04; 
    side.a.dir = EEPROM_1_ReadByte(12);
    side.b.dir = EEPROM_1_ReadByte(13);
    side.c.dir = EEPROM_1_ReadByte(14);
    side.d.dir = EEPROM_1_ReadByte(15);
//    for(x=0; x<30;x++){
//        Encabezado1[x]=EEPROM_1_ReadByte(16+x);
//    }
//    for(x=30; x<60;x++){
//        Encabezado2[x]=EEPROM_1_ReadByte(16+x);
//    }
//    for(x=60; x<90;x++){
//        Encabezado3[x]=EEPROM_1_ReadByte(16+x);
//    }
//    for(x=90; x<120;x++){
//        Encabezado4[x]=EEPROM_1_ReadByte(16+x);
//    }
}


/*
*********************************************************************************************************
*                                         void InitDisplay1(void)
*
* Description : Pregunta configuraciones al dispensador 
*               
*
*********************************************************************************************************
*/
void InitDisplay1(){      
    uint8_t iniText[10]="Iniciando"; 
    uint8_t Unit[10] = "S/D/Dupla ";
    uint8_t Unit2[10] = "Quadrupla";
    uint8_t Unit3[10] = "Sextupla";
    uint8_t Unit4[10] = "Octupla";
    uint8_t DisplayMode0[10] = "5-5-4     ";
    uint8_t DisplayMode[10] = "6-6-4     ";
    uint8_t DisplayMode2[10] = "7-7-5     ";
    uint8_t DisplayMode3[10] = "8-7-5     ";
    if(NumPositions == 2){
        SetPicture(1,DISPLAY_INICIO0);         
        ShowMessage(1,iniText,0);    
        CyDelay(300);
        if(UnitType == 0){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            ShowMessage(1,Unit,0);  //Mostrar tipo de dispensador      
        }
        if(UnitType == 1){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            ShowMessage(1,Unit2,0);
        }
        if(UnitType == 2){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            ShowMessage(1,Unit3,0);
        }
        if(UnitType == 3){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            ShowMessage(1,Unit4,0);
        } 
        if(DDMode == 0){
            ShowMessage(1,DisplayMode0,22); //Mostrar modo de display
            digits = 5;
        }
        if(DDMode == 1){
            ShowMessage(1,DisplayMode,22); //Mostrar modo de display
            digits = 6;
        }
        if(DDMode == 2){
           ShowMessage(1,DisplayMode2,22);
            digits = 7;
        }
        if(DDMode == 3){
            ShowMessage(1,DisplayMode3,22);
            digits = 8;
        }        
    }
    if(NumPositions == 4){
        SetPicture(1,DISPLAY_SELECCIONE_POSICION);        
        ShowMessage(1,iniText,0);    
        CyDelay(300);
        if(UnitType == 0){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            side.c.hoseNumber = UnitType+1;
            side.d.hoseNumber = UnitType+1;
            ShowMessage(1,Unit,0);  //Mostrar tipo de dispensador      
        }
        if(UnitType == 1){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            side.c.hoseNumber = UnitType+1;
            side.d.hoseNumber = UnitType+1;
            ShowMessage(1,Unit2,0);
        }
        if(UnitType == 2){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            side.c.hoseNumber = UnitType+1;
            side.d.hoseNumber = UnitType+1;
           ShowMessage(1,Unit3,0);
        }
        if(UnitType == 3){
            side.a.hoseNumber = UnitType+1;
            side.b.hoseNumber = UnitType+1;
            side.c.hoseNumber = UnitType+1;
            side.d.hoseNumber = UnitType+1;
            ShowMessage(1,Unit4,0);
        }    
        if(DDMode == 1){
            ShowMessage(1,DisplayMode,22); //Mostrar modo de display
        }
        if(DDMode == 2){
           ShowMessage(1,DisplayMode2,22);
        }
    }        
    if(NumPositions < 2){
        SetPicture(1,DISPLAY_ERROR);        
    }         
}

/*
*********************************************************************************************************
*                                         void InitDisplay1(void)
*
* Description : Pregunta configuraciones al dispensador 
*               
*
*********************************************************************************************************
*/
void InitDisplay2(){      
    if(NumPositions == 2){        
        SetPicture(2,DISPLAY_INICIO0);  
        flowDisplay2 = 0;
    }
    if(NumPositions == 4){
        SetPicture(2,DISPLAY_SELECCIONE_POSICION);         
    }        
    if(NumPositions < 2){        
        SetPicture(2,DISPLAY_ERROR);
    }     
    ShowMessage(2,MuxVersion,0);
}

/*
*********************************************************************************************************
*                                         void InitPump(void)
*
* Description : Pregunta configuraciones al dispensador 
*               
*
*********************************************************************************************************
*/
void InitPump(){        
    NumPositions = get_position();
    if(NumPositions > 0){
        if(!PumpIsInValidState(get_state(side.a.dir))){
            SetPicture(1,DISPLAY_ERROR);
            SetPicture(2,DISPLAY_ERROR);
            NumPositions = 0;
        }
        if(!PumpIsInValidState(get_state(side.b.dir))){
            SetPicture(1,DISPLAY_ERROR);
            SetPicture(2,DISPLAY_ERROR);
            NumPositions = 0;
        }
        if(PumpIsInValidState(get_state(side.a.dir)) && PumpIsInValidState(get_state(side.b.dir))){    
            NumPositions = get_position();
            if(get_state(side.a.dir) ==0x06 && get_state(side.b.dir)==0x06){
                PumpCompleteConfiguration(side.a.dir);
                StoreConfiguration();            
            }else{
                SetPicture(1,DISPLAY_BAJE_MANIJA);
                SetPicture(2,DISPLAY_BAJE_MANIJA);
                InitPump();                
            }
        }        
    }
    
}
/*
*********************************************************************************************************
*                                         void PrintTest(void)
*
* Description : Pregunta configuraciones al dispensador 
*               
*
*********************************************************************************************************
*/
void PrintTest(){
    uint8 datoA[26]="Test de Impresion Puerto 1";
    uint8 datoB[26]="Test de Impresion Puerto 2";
    uint8 x;
    for(x = 0; x<26;x++){
        write_psoc1(1,datoA[x]);
    }
    write_psoc1(1,10);
    write_psoc1(1,10);
    write_psoc1(1,10);
    write_psoc1(1,10);
    write_psoc1(1,10);
    write_psoc1(1,10);
    for(x = 0; x<26;x++){
        write_psoc1(2,datoB[x]);
    }
    write_psoc1(2,10);
    write_psoc1(2,10);
    write_psoc1(2,10);
    write_psoc1(2,10);
    write_psoc1(2,10);
    write_psoc1(2,10);
}

/*
*********************************************************************************************************
*                                         void polling_Pump(void)
*
* Description : Pregunta estado al surtidor 
*               
*
*********************************************************************************************************
*/

void PollingPump(void){
    uint8 x;
    if(countPump>8){
        if(flowDisplay1 == 0){
            if(PumpIsInValidState(get_state(side.a.dir)) && PumpIsInValidState(get_state(side.b.dir)) ){
                for(x= 0; x < 4; x++){
                    PrevStatePump[x] = statePump[x];
                }
            }
            if (NumPositions == 2){
                statePump[0] = get_state(side.a.dir);                 
                statePump[1] = get_state(side.b.dir);          
            }
            if (NumPositions == 4 ){
                statePump[0] = get_state(side.a.dir);               
                statePump[1] = get_state(side.b.dir);          
                statePump[2] = get_state(side.c.dir);        
                statePump[3] = get_state(side.d.dir);        
            }
            if (NumPositions == 0){
                InitPump();
            }
            if(!PumpIsInValidState(get_state(side.a.dir))){
                SetPicture(1,DISPLAY_ERROR);
                SetPicture(2,DISPLAY_ERROR);
                NumPositions = 0;
                flowDisplay1 = 0;
                flowDisplay2 = 0;
            }
            if(!PumpIsInValidState(get_state(side.b.dir))){
                SetPicture(1,DISPLAY_ERROR);
                SetPicture(2,DISPLAY_ERROR);
                NumPositions = 0;
                flowDisplay1 = 0;
                flowDisplay2 = 0;
            }
        }
        if(ActiveRF == 0 && flowDisplay1 == 0){
            if(get_state(side.a.dir)== PUMP_IDLE)
                getTotals(side.a.dir);
            if(get_state(side.b.dir)== PUMP_IDLE)
                getTotals(side.b.dir);
        }        
        countPump=0;
    }
}
/*
*********************************************************************************************************
*                                         void polling_Pump(void)
*
* Description : Pregunta estado al surtidor 
*               
*
*********************************************************************************************************
*/

void PollingDisplay1(void){
    //if(countLCD >10){
    uint8 x,y;     
    switch(flowDisplay1){
        case 0:
            InitDisplay1();
            flowDisplay1 =1;
            isr_3_Stop(); 
            Timer_Animacion_Stop();
            count_protector = 0;
            bufferDisplay1.saleType = 0;
            bufferDisplay1.flagEndSale = false;  
            side.a.RFstateReport = 0;            
        break;
        case 1: //Menu
            if(countRX >700){
                if(bufferDisplay1.flagPrint)
                    imprimir(printPortA,side.a.dir);
                countRX = 0;
            }
            side.a.rfState = RF_IDLE;
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){                                             
                    flowDisplay1 = 2;      //Pantalla forma de pago                      
                    SetPicture(1,DISPLAY_FORMA_PAGO_DESEADA);                                                                                                  
                }  
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }
        break;
        case 2:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x0D:  //Pantalla efectivo              
                            flowDisplay1 = 3; 
                            bufferDisplay1.saleType = 1;
                            SetPicture(1,DISPLAY_FORMA_PROGRAMACION);                            
                        break;
                        case 0x0E:  //Pantalla credito  
                            flowDisplay1 = 10;
                            bufferDisplay1.saleType = 2;
                            count_protector = 0;
                            SetPicture(1,DISPLAY_ID_DIGITAL);                            
                        break;
                        case 0x45:  //Pantalla otras opciones 
                            flowDisplay1 = 12;                            
                            SetPicture(1,DISPLAY_OPERACIONES);                            
                        break;
                            
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                        
                        case 0x7E:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                    }
                }  
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }
        break;
            
        case 3: 
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x0F:  //Preset dinero                
                            flowDisplay1 = 4;                              
                            bufferDisplay1.presetType[0]=2;
                            bufferDisplay1.presetType[1]='D';
                            numberKeys1=0;
                            SetPicture(1,DISPLAY_INTRODUZCA_VALOR);   
                            WriteLCD(1,'$',3,2,1,0x0000,'N');
                        break; 
                        case 0x10:  //Preset volumen                 
                            flowDisplay1 = 4;                            
                            bufferDisplay1.presetType[0]=1;
                            bufferDisplay1.presetType[1]='V';
                            numberKeys1=0;                            
                            SetPicture(1,DISPLAY_INTRODUZCA_VOLUMEN);                            
                            WriteLCD(1,'V',3,2,1,0x0000,'N');                            
                        break;
                        case 0x43:  //Preset full 
                            bufferDisplay1.presetType[0]=2;
                            bufferDisplay1.presetType[1]='F';
                            for(x=1;x<(digits-1);x++){
                                bufferDisplay1.presetValue[0][x]='9';
                            }
                            bufferDisplay1.presetValue[0][x]='0';
                            bufferDisplay1.presetValue[0][x+1]='0';
                            bufferDisplay1.presetValue[0][0]=digits;
                            for(x=0;x<=bufferDisplay1.presetValue[0][0];x++){
                                bufferDisplay1.presetValue[1][x]=bufferDisplay1.presetValue[0][x];
                            }
                            flowDisplay1 = 5;
                            SetPicture(1,DISPLAY_SELECCIONE_PRODUCTO4);                          
                        break;
                        case 0x94:  //Retroceso 
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                                                        
                        break;
                        case 0x3B:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                        case 0x7E:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                    }
                }  
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }
        break;
        
        case 4:            
            switch (alphanumeric_keyboard(1,digits,0)){
                case 0:  //Pantalla Inicial    
                    flowDisplay1 = 0;
                    SetPicture(1,DISPLAY_INICIO0);                            
                break;
                    
                case 1: //Enter
                    for(x=0;x<=bufferDisplay1.valueKeys[0];x++){
                        bufferDisplay1.presetValue[0][x]=bufferDisplay1.valueKeys[x];
                        bufferDisplay1.presetValue[1][x]=bufferDisplay1.valueKeys[x];
                    }  
                    flowDisplay1 = 5;//caso para seleccion de producto
                    SetPicture(1,DISPLAY_SELECCIONE_PRODUCTO4);
                break;
            }
        break;
            
        case 5:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x80:  //Grado 1 
                            flowDisplay1 = 6;
                            side.a.hose = 1;
                            SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;
                            
                        case 0x81:  //Grado 2 
                            flowDisplay1 = 6;
                            side.a.hose = 2;
                            SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;
                            
                        case 0x7F:  //Grado 3
                            flowDisplay1 = 6;
                            side.a.hose = 3;
                            SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;   
                        
                        case 0xB8:  //Grado 4 
                            flowDisplay1 = 6;
                            side.a.hose = 4;
                            SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;
                            
                        case 0x94:  //Retroceso 
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                                                        
                        break;
                            
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }
        break;
                    
        case 6:
            isr_3_StartEx(animacion);
            Timer_Animacion_Start();
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x39:  //Si imprimir  
                            flowDisplay1 = 9;
                            numberKeys1 = 0;
                            bufferDisplay1.flagKeyboard = 1;
                            bufferDisplay1.flagPrint =  1;
                            SetPicture(1,DISPLAY_DIGITE_PLACA);                            
                        break; 
                        case 0x38:  //No imprimir 
                            if(bufferDisplay1.flagEndSale){
                                flowDisplay1 = 0;
                                SetPicture(1,DISPLAY_INICIO0);
                            }else{
                                flowDisplay1 = 7;//Esperando estado del dispensador 
                                bufferDisplay1.flagPrint =  0;
                                count_protector=0;
                                SetPicture(1,DISPLAY_SUBA_MANIJA);
                            }
                        break;
                        case 0x7E:  //Pantalla Inicial                                                                                    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }
            if(count_protector>=30){
                flowDisplay1 = 0;	
    			count_protector=0;
                SetPicture(1,DISPLAY_INICIO0);
    		}
        break;
                                        
            
        case 7:
            //isr_3_StartEx(animacion);
            //Timer_Animacion_Start();
            side.a.activeHose = PumpHoseActiveState(side.a.dir);  
            if (side.a.activeHose == side.a.hose){                
                if(PresetData(side.a.dir,side.a.activeHose,bufferDisplay1.presetValue[0],bufferDisplay1.presetType[0]&0x03)==1){                    
                    isr_3_Stop(); 
                    Timer_Animacion_Stop();
                    get_state(side.a.dir);
                    //CyDelay(20);
                    Pump_PutChar(0x10|side.a.dir);//Autoriza el surtidor
                    //CyDelay(50);
                    side.a.RFstateReport = 1;
                    flowDisplay1 = 8;                    
                    count_protector = 0;                    
                    bufferDisplay1.flagActiveSale = true;
                    SetPicture(1,DISPLAY_DESPACHANDO);   
                    ShowMessage(1,(bufferDisplay1.presetValue[0]),18);
                }else{
                    flowDisplay1 = 0;
                    SetPicture(1,DISPLAY_ERROR);
                    CyDelay(200);
                    SetPicture(1,DISPLAY_INICIO0);
                }
            }else{
                flowDisplay1 = 7;                
//                if(count_protector>=60){
//                    flowDisplay1 = 0;	
//        			count_protector=0;
//                    SetPicture(1,DISPLAY_INICIO0);
//        		}
            }
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){                        
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                    }                    
                }
                CyDelay(10);                         
            }
            Display1_ClearRxBuffer();
        break;
            
        case 8:            
            side.a.pumpState = get_state(side.a.dir);
            switch (side.a.pumpState){
                case PUMP_IDLE://Espera - venta cero
                    flowDisplay1 = 0;
                    side.a.rfState = RF_ZERO_SALE;
                    bufferDisplay1.flagActiveSale = false;
                    SetPicture(1,DISPLAY_INICIO0);
                break;
                case PUMP_BUSY://Surtiendo                    
                    bufferDisplay1.flagActiveSale = true;
                    bufferDisplay1.flagEndSale = false;
                    //side.a.rfState = RF_DELIVERING;
                break;
                case PUMP_PEOT: //reporte fin venta                    
                    if(getSale(side.a.dir)){
                        if(bufferDisplay1.flagPrint ==1){ 
                            countRX = 0;
                            bufferDisplay1.flagEndSale = true;
                            flowDisplay1 = 0;                            
                            SetPicture(1,DISPLAY_INICIO0); 
                        }else{
                            flowDisplay1 = 6;
                            bufferDisplay1.flagEndSale = true;
                            SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO); 
                        }                        
                        bufferDisplay1.flagActiveSale = false;
                        side.a.RFstateReport = 1;
                    }                                         
                break;
                case PUMP_FEOT://Reporte de venta
                    if(getSale(side.a.dir)){
                        if(bufferDisplay1.flagPrint ==1){  
                            countRX = 0;
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0); 
                        }else{
                            flowDisplay1 = 6;
                            bufferDisplay1.flagEndSale = true;
                            SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO); 
                        }
                        bufferDisplay1.flagActiveSale = false;
                        side.a.RFstateReport = 1;
                    }
                break;
                
            }
            
        break;
            
        case 9: //Teclado           
            switch (alphanumeric_keyboard(1,10,0)){
                case 0: //Cancelar
                    switch(bufferDisplay1.flagKeyboard){
                        case 1://Placa
                            for(x=0;x<=10;x++){
                                bufferDisplay1.licenceSale[x]=0;
                            }
                            flowDisplay1 = 0;//Inicio
                            SetPicture(1,DISPLAY_INICIO0);
                        break;
                        
                        case 2://Kilometraje
                            for(x=0;x<=10;x++){
                                bufferDisplay1.mileageSale[x]=0;
                            }
                        break;
                        
                        case 3://CC/NIT
                            for(x=0;x<=10;x++){
                                bufferDisplay1.identySale[x]=0;
                            }
                        break;
                        case 4://ID
                            for(x=0;x<=10;x++){
                                bufferDisplay1.shiftId[x]=0;
                            }
                        break;
                    }                    
                    Display1_ClearRxBuffer();
                break;
                    
                case 1: //Enter
                    switch(bufferDisplay1.flagKeyboard){
                        case 1://Placa
                            for(x=0;x<=bufferDisplay1.valueKeys[0];x++){
                                bufferDisplay1.licenceSale[x]=bufferDisplay1.valueKeys[x];
                            }
                            if(bufferDisplay1.flagEndSale){
                                imprimir(printPortA,side.a.dir);
                                flowDisplay1 = 0;
                                SetPicture(1,DISPLAY_INICIO0); 
                            }else{
                                flowDisplay1 = 7;//Suba manija
                                SetPicture(1,DISPLAY_SUBA_MANIJA);
                            }
                        break;
                        
                        case 2://Kilometraje
                            for(x=0;x<=bufferDisplay1.valueKeys[0];x++){
                                bufferDisplay1.mileageSale[x]=bufferDisplay1.valueKeys[x];
                            }
                            flowDisplay1 = 7;//Suba manija
                            SetPicture(1,DISPLAY_SUBA_MANIJA);
                        break;
                        
                        case 3://CC/NIT
                            for(x=0;x<=bufferDisplay1.valueKeys[0];x++){
                                bufferDisplay1.identySale[x]=bufferDisplay1.valueKeys[x];
                            }
                        case 4://ID
                            for(x=0;x<=10;x++){
                                bufferDisplay1.shiftId[x] =bufferDisplay1.valueKeys[x];
                            }
                            flowDisplay1 = 14;//Pide clave                            
                            numberKeys1 = 0;
                            hiddenKeys = 10;
                            controlChar ='*';
                            bufferDisplay1.flagKeyboard = 3;
                            SetPicture(1,DISPLAY_INGRESE_PASSWORD);
                        break;
                    }                    
                    Display1_ClearRxBuffer();
                break;
            }            
        break;    
    ////////////////// CASOS PARA CRÉDITO  /////////////////////
        case 10:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0xB6:  //Solicitud ibutton  
                            flowDisplay1 = 11;
                            numberKeys1 = 0;                            
                            bufferDisplay1.flagPrint =  1;
                            SetPicture(1,DISPLAY_ESPERANDO_ID);                            
                        break; 
                        case 0xB7:  //ID por número
                            bufferDisplay1.flagKeyboard = 0;
                            flowDisplay1 = 7;//Esperando estado del dispensador  
                            count_protector=0;
                            SetPicture(1,DISPLAY_SUBA_MANIJA);                            
                        break;
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }                        
        break;
        
        case 11:
            isr_3_StartEx(animacion);
            Timer_Animacion_Start();
            if(touch_present(1)==1){
				if(touch_write(1,0x33)){
					for(x=1;x<=8;x++){
						temporal[x]=touch_read_byte(1);
					}
					y=0;
					for(x=1;x<8;x++){
                        y=crc_check(y,temporal[x]);
                    }
					if(y==temporal[8]){
						bufferDisplay1.idSerial[0]=16;
						y=16;
						for(x=1;x<=8;x++){
							if((temporal[x]&0x0F)>=10){
								bufferDisplay1.idSerial[y]=(temporal[x]&0x0F)+55;
							}else{
								bufferDisplay1.idSerial[y]=(temporal[x]&0x0F)+48;				
							}
                            y--;
							if(((temporal[x]>>4)&0x0F)>=10){
								bufferDisplay1.idSerial[y]=((temporal[x]>>4)&0x0F)+55;
							}else{
								bufferDisplay1.idSerial[y]=((temporal[x]>>4)&0x0F)+48;				
							}
                            y--;
						}                        
                        SetPicture(1,DISPLAY_ID_RECONOCIDO);                                           
                        Display1_ClearRxBuffer();
                        CyDelay(700);
                        bufferDisplay1.flagKeyboard = 2;
                        flowDisplay1 = 9;
                        SetPicture(1,DISPLAY_INTRODUZCA_KILOMETRAJE);
                        
					}
				}
			}
            if(count_protector>=60){
                flowDisplay1 = 0;	
    			count_protector=0;
                SetPicture(1,DISPLAY_INICIO0);
    		}
                                    
        break;    
    ///////////////FIN CASOS PARA CRÉDITO  /////////////////////  
    
    //////////// CASOS PARA CONFIGURACIONES  ///////////////////
        case 12:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x46:  //Turnos              
                            flowDisplay1 = 13; 
                            if(lockTurn ==1){
                                SetPicture(1,DISPLAY_CERRAR_TURNO);
                            }else{
                                SetPicture(1,DISPLAY_ABRIR_TURNO);
                            }                            
                        break;
                        case 0x55:  //Configurar módulo 
                            flowDisplay1 = 14;
                            bufferDisplay1.flagKeyboard = 6;
                            numberKeys1 = 0;
                            hiddenKeys = 5;
                            controlChar ='*';
                            SetPicture(1,DISPLAY_INGRESE_PASSWORD);                           
                        break;
                        case 0xB5:  //Copia de recibo 
                            flowDisplay1  = 0;
                            side.a.RFstateReport = 1;
                            side.a.rfState = RF_COPY_RECEIPT;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                        
                        case 0x3B:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                            
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                        
                        case 0x7E:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                    }
                }  
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }
        break;

        case 13:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x47:  //Cambio de turno: Abrir  
                            flowDisplay1 = 9;
                            numberKeys1 = 0;    
                            bufferDisplay1.flagKeyboard = 4;
                            SetPicture(1,DISPLAY_INTRODUZCA_CEDULA);                            
                        break;
                        case 0x48:  //Cambio de turno: Cerrar 
                            flowDisplay1 = 9;
                            numberKeys1 = 0;   
                            bufferDisplay1.flagKeyboard = 4;
                            SetPicture(1,DISPLAY_INTRODUZCA_CEDULA);                           
                        break;                         
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }                        
        break; 
            
        case 14: //Teclado general      
            switch (alphanumeric_keyboard(1,hiddenKeys,controlChar)){
                case 0: //Cancelar
                    switch(bufferDisplay1.flagKeyboard){
                        case 1://ID Estacion
                            for(x=0;x<=4;x++){
                                idStation[x]=0;
                            }
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);
                        break;
                        case 2://Umbral
                            for(x=0;x< hiddenKeys;x++){
                                pumpGap[x]=0;
                            }
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);
                        break;
                        case 3://Pass turno
                            for(x=0;x< hiddenKeys;x++){
                                bufferDisplay1.shiftPassword[x]=0;
                            }
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);
                        break;
                        case 5://Pass
                            for(x=0;x< hiddenKeys;x++){
                                bufferDisplay1.shiftPassword[x]=0;
                            }
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);
                        break;
                            
                        case 6://Pass
                            for(x=1;x<=configAccess[0];x++){
                                configAccess[x]=0;
                            }
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);
                        break;
                    }                    
                    Display1_ClearRxBuffer();
                break;
                    
                case 1: //Enter
                    switch(bufferDisplay1.flagKeyboard){   
                        case 1://ID Estacion
                            for(x=0;x <=4;x++){
                                idStation[x] =bufferDisplay1.valueKeys[x+1];                                
                            }
                            idStation[4] = 0x00;
                            intIDStation = atoi(idStation);
                            IDCast[0] = intIDStation;
                            IDCast[1] = intIDStation>>8;
                            EEPROM_1_WriteByte(IDCast[0],10);
                            EEPROM_1_WriteByte(IDCast[1],11);
                            flowDisplay1 = 15;
                            SetPicture(1,DISPLAY_CONFIGURACIONES);
                        break;
                        case 2://Pass
                            for(x=0;x < hiddenKeys;x++){
                                pumpGap[x] =bufferDisplay1.valueKeys[x];
                            }
                            flowDisplay1 = 15;
                            SetPicture(1,DISPLAY_CONFIGURACIONES);
                        break;
                        case 3://Pass turno
                            for(x=0;x < hiddenKeys;x++){
                                bufferDisplay1.shiftPassword[x] =bufferDisplay1.valueKeys[x];
                            }
                            flowDisplay1 = 19;
                            SetPicture(1,DISPLAY_POR_FAVOR_ESPERE);
                        break;
                        case 5://Pass
                            for(x=0;x < hiddenKeys;x++){
                                bufferDisplay1.shiftPassword[x] =bufferDisplay1.valueKeys[x];
                            }
                            flowDisplay1 = 15;
                            SetPicture(1,DISPLAY_CONFIGURACIONES);
                        break; 
                        case 6://Pass
                            for(x=1;x<=configAccess[0];x++){
                                configAccess[x] =bufferDisplay1.valueKeys[x];
                            }
                            if(configAccess[1] == passwordPump[1] && configAccess[2] == passwordPump[2] && configAccess[3] == passwordPump[3] && configAccess[4] == passwordPump[4]){                                
                                SetPicture(1,DISPLAY_PASSWORD_VALIDO);
                                CyDelay(700);
                                flowDisplay1 = 15;
                                SetPicture(1,DISPLAY_CONFIGURACIONES);
                            }else{
                                SetPicture(1,DISPLAY_PASSWORD_INVALIDO);
                                CyDelay(700);
                                flowDisplay1 = 0;
                                SetPicture(1,DISPLAY_INICIO0);
                            }
                        break;
                    }                    
                    Display1_ClearRxBuffer();
                break;
            }            
        break;
            
        case 15:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x82:  //ID Estacion  
                            flowDisplay1 = 14;
                            numberKeys1 = 0; 
                            controlChar = 0;
                            hiddenKeys  = 5;
                            bufferDisplay1.flagKeyboard = 1;
                            SetPicture(1,DISPLAY_INTRODUZCA_VALOR);                            
                        break;
                        case 0x65:  //Test Impresoras 
                            flowDisplay1 = 16; 
                            PrintTest();
                            SetPicture(1,DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO);                           
                        break;
                        
                        case 0x5A:  //Umbral 
                            flowDisplay1 = 14;
                            numberKeys1 = 0;
                            controlChar = 0;
                            hiddenKeys  = 3;
                            pumpGap[0]  = 2;
                            bufferDisplay1.flagKeyboard = 2;
                            SetPicture(1,DISPLAY_INTRODUZCA_VALOR);                            
                        break;
                        case 0x58:  //Hora y Fecha
                            flowDisplay1 = 18;
                            numberKeys1 = 0;   
                            bufferDisplay1.flagKeyboard = 4;
                            SetPicture(1,DISPLAY_CONFIGURAR_FECHA_HORA);                           
                        break;
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }                        
        break; 
            
        case 16:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x38:  //No funcionan  
                            flowDisplay1 = 17;
                            SetPicture(1,DISPLAY_IMPRESORA_NO_FUNCIONANDO);                            
                        break;
                        case 0x39:  //Si funcionan
                            flowDisplay1 = 15; 
                            printPortA = 1;
                            printPortB = 2;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(1,DISPLAY_CONFIGURACIONES);
                        break;
                                               
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }                        
        break;
            
        case 17:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x8C:  //1 no funciona  
                            flowDisplay1 = 15; 
                            printPortA = 2;
                            printPortB = 2;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(1,DISPLAY_CONFIGURACIONES);                            
                        break;
                        case 0x8D:  //2 no funciona
                            flowDisplay1 = 15; 
                            printPortA = 1;
                            printPortB = 1;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(1,DISPLAY_CONFIGURACIONES); 
                        break;
                            
                        case 0x8E:  //ninguna funciona
                            flowDisplay1 = 15; 
                            printPortA = 0;
                            printPortB = 0;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(1,DISPLAY_CONFIGURACIONES); 
                        break;
                                               
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(1,DISPLAY_INICIO0);
                            flowDisplay1 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }                        
        break;
            
        case 18:
            if(leer_hora()==1){
                WriteLCD(1,(((timeDownHandle[1]&0x10)>>4)+48),13,8,1,0x0000,'N');
                WriteLCD(1,((timeDownHandle[1]&0x0F)+48),13,9,1,0x0000,'N');
                WriteLCD(1,':',13,10,1,0x0000,'N');
                WriteLCD(1,(((timeDownHandle[0]&0xF0)>>4)+48),13,11,1,0x0000,'N');
                WriteLCD(1,((timeDownHandle[0]&0x0F)+48),13,12,1,0x0000,'N');               
            }
            if(leer_fecha()==1){                
                WriteLCD(1,(((dateDownHandle[0]&0x30)>>4)+48),21,8,1,0x0000,'N');
                WriteLCD(1,((dateDownHandle[0]&0x0F)+48),21,9,1,0x0000,'N');
                WriteLCD(1,'/',21,10,1,0x0000,'N');
                WriteLCD(1,(((dateDownHandle[1]&0x10)>>4)+48),21,11,1,0x0000,'N');
                WriteLCD(1,((dateDownHandle[1]&0x0F)+48),21,12,1,0x0000,'N');
                WriteLCD(1,'/',21,13,1,0x0000,'N');
                WriteLCD(1,(((dateDownHandle[2]&0xF0)>>4)+48),21,14,1,0x0000,'N');
                WriteLCD(1,((dateDownHandle[2]&0x0F)+48),21,15,1,0x0000,'N');
            }
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){                                                                       
                        case 0x7E:  //Pantalla Inicial  
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay1 = 0;
                            SetPicture(1,DISPLAY_INICIO0);                            
                        break;
                    }                    
                }
                CyDelay(10);         
                Display1_ClearRxBuffer();
            }                        
        break;
            
        case 19:            
            side.a.RFstateReport = 1;
            ShiftState =1;
        break;
    countLCD = 0;       
    }    
    }


/*
*********************************************************************************************************
*                                         void polling_Pump(void)
*
* Description : Pregunta estado al surtidor 
*               
*
*********************************************************************************************************
*/
void PollingDisplay2(void){
    //if(countLCD >10){
    uint8 x,y;     
    switch(flowDisplay2){
        case 0:
			InitDisplay2();
            flowDisplay2 =1;
            side.b.rfState = RF_IDLE;
            isr_4_Stop(); 
            Timer_Animacion2_Stop();
            count_protector2 = 0;
            bufferDisplay2.saleType = 0;
            bufferDisplay2.flagEndSale = false;            
        break;
        case 1: //Menu
            ShowRectangle(2,35);
            side.b.rfState = RF_IDLE;
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){                                             
                    flowDisplay2 = 2;      //Pantalla forma de pago                      
                    SetPicture(2,DISPLAY_FORMA_PAGO_DESEADA);                                                                                                  
                }  
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }
        break;
        case 2:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x0D:  //Pantalla efectivo              
                            flowDisplay2 = 3; 
                            bufferDisplay2.saleType = 1;
                            SetPicture(2,DISPLAY_FORMA_PROGRAMACION);                            
                        break;
                        case 0x0E:  //Pantalla credito  
                            flowDisplay2 = 10;
                            bufferDisplay2.saleType = 2;
                            count_protector = 0;
                            SetPicture(2,DISPLAY_ID_DIGITAL);                            
                        break;
                        case 0x45:  //Pantalla otras opciones 
                            flowDisplay2 = 12;                            
                            SetPicture(2,DISPLAY_OPERACIONES);                            
                        break;
                            
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                        
                        case 0x7E:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                    }
                }  
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }
        break;
            
        case 3: 
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x0F:  //Preset dinero                
                            flowDisplay2 = 4;                              
                            bufferDisplay2.presetType[0]=2;
                            bufferDisplay2.presetType[1]='D';
                            numberKeys1=0;
                            SetPicture(2,DISPLAY_INTRODUZCA_VALOR);   
                            WriteLCD(2,'$',3,2,1,0x0000,'N');
                        break; 
                        case 0x10:  //Preset volumen                 
                            flowDisplay2 = 4;                            
                            bufferDisplay2.presetType[0]=1;
                            bufferDisplay2.presetType[1]='V';
                            numberKeys1=0;                            
                            SetPicture(2,DISPLAY_INTRODUZCA_VOLUMEN);                            
                            WriteLCD(2,'V',3,2,1,0x0000,'N');                            
                        break;
                        case 0x43:  //Preset full 
                            bufferDisplay2.presetType[0]=2;
                            bufferDisplay2.presetType[1]='F';
                            for(x=1;x<(digits-1);x++){
                                bufferDisplay2.presetValue[0][x]='9';
                            }
                            bufferDisplay2.presetValue[0][x]='0';
                            bufferDisplay2.presetValue[0][x+1]='0';
                            bufferDisplay2.presetValue[0][0]=digits;
                            for(x=0;x<=bufferDisplay2.presetValue[0][0];x++){
                                bufferDisplay2.presetValue[1][x]=bufferDisplay2.presetValue[0][x];
                            }
                            flowDisplay2 = 5;
                            SetPicture(2,DISPLAY_SELECCIONE_PRODUCTO4);                          
                        break;
                        case 0x94:  //Retroceso 
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                                                        
                        break;
                        case 0x3B:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                        case 0x7E:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                    }
                }  
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }
        break;
        
        case 4:            
            switch (alphanumeric_keyboard(2,digits,0)){
                case 0:  //Pantalla Inicial    
                    flowDisplay2 = 0;
                    SetPicture(2,DISPLAY_INICIO0);                            
                break;
                    
                case 1: //Enter
                    for(x=0;x<=bufferDisplay2.valueKeys[0];x++){
                        bufferDisplay2.presetValue[0][x]=bufferDisplay2.valueKeys[x];
                        bufferDisplay2.presetValue[1][x]=bufferDisplay2.valueKeys[x];
                    }  
                    flowDisplay2 = 5;//caso para seleccion de producto
                    SetPicture(2,DISPLAY_SELECCIONE_PRODUCTO4);
                break;
            }
        break;
            
        case 5:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x80:  //Grado 1 
                            flowDisplay2 = 6;
                            side.b.hose = 1;
                            SetPicture(2,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;
                            
                        case 0x81:  //Grado 2 
                            flowDisplay2 = 6;
                            side.b.hose = 2;
                            SetPicture(2,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;
                            
                        case 0x7F:  //Grado 3
                            flowDisplay2 = 6;
                            side.b.hose = 3;
                            SetPicture(2,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;   
                        
                        case 0xB8:  //Grado 4 
                            flowDisplay2 = 6;
                            side.b.hose = 4;
                            SetPicture(2,DISPLAY_DESEA_IMPRIMIR_RECIBO);                                                        
                        break;
                            
                        case 0x94:  //Retroceso 
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                                                        
                        break;
                            
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }
        break;
                    
        case 6:
            isr_3_StartEx(animacion);
            Timer_Animacion_Start();
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x39:  //Si imprimir  
                            flowDisplay2 = 9;
                            numberKeys1 = 0;
                            bufferDisplay2.flagKeyboard = 1;
                            bufferDisplay2.flagPrint =  1;
                            SetPicture(2,DISPLAY_DIGITE_PLACA);                            
                        break; 
                        case 0x38:  //No imprimir 
                            if(bufferDisplay2.flagEndSale){
                                flowDisplay2 = 0;
                                SetPicture(2,DISPLAY_INICIO0);
                            }else{
                                flowDisplay2 = 7;//Esperando estado del dispensador 
                                bufferDisplay2.flagPrint =  0;
                                count_protector=0;
                                SetPicture(2,DISPLAY_SUBA_MANIJA);
                            }
                        break;
                        case 0x7E:  //Pantalla Inicial                                                                                    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }
            if(count_protector>=30){
                flowDisplay2 = 0;	
    			count_protector=0;
                SetPicture(2,DISPLAY_INICIO0);
    		}
        break;
                                        
            
        case 7:
            //isr_3_StartEx(animacion);
            //Timer_Animacion_Start();
            side.b.activeHose = PumpHoseActiveState(side.b.dir);  
            if (side.b.activeHose == side.b.hose){                
                if(PresetData(side.b.dir,side.b.activeHose,bufferDisplay2.presetValue[0],bufferDisplay2.presetType[0]&0x03)==1){                    
                    isr_4_Stop(); 
                    Timer_Animacion2_Stop();
                    get_state(side.b.dir);
                    //CyDelay(20);
                    Pump_PutChar(0x10|side.b.dir);//Autoriza el surtidor
                    //CyDelay(50);
                    side.b.RFstateReport = 1;                    
                    flowDisplay2 = 8;                    
                    count_protector2 = 0;                                            
                    SetPicture(2,DISPLAY_DESPACHANDO);   
                    ShowMessage(2,(bufferDisplay2.presetValue[0]),18);
                }else{
                    flowDisplay2 = 0;
                    SetPicture(2,DISPLAY_ERROR);
                    CyDelay(200);
                    SetPicture(2,DISPLAY_INICIO0);
                }
            }else{
                flowDisplay2 = 7;                
//                if(count_protector>=60){
//                    flowDisplay2 = 0;	
//        			count_protector=0;
//                    SetPicture(2,DISPLAY_INICIO0);
//        		}
            }
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){                        
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                    }                    
                }
                CyDelay(10);                         
            }
            Display2_ClearRxBuffer();
        break;
            
        case 8:            
            side.b.pumpState = get_state(side.b.dir);
            switch (side.b.pumpState){
                case PUMP_IDLE: //Espera - venta cero
                    flowDisplay2 = 0;
                    side.b.rfState = RF_ZERO_SALE;
                    bufferDisplay2.flagActiveSale = false;
                    SetPicture(2,DISPLAY_INICIO0);
                break;
                case PUMP_BUSY: //Surtiendo                    
                    bufferDisplay2.flagActiveSale = true;
                    bufferDisplay2.flagEndSale = false;
                    //side.a.rfState = RF_DELIVERING;
                break;
                case PUMP_PEOT: //reporte fin venta                    
                    if(getSale(side.b.dir)){
                        if(bufferDisplay2.flagPrint ==1){
                            imprimir(printPortB,side.b.dir);
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0); 
                        }else{
                            flowDisplay2 = 6;
                            bufferDisplay2.flagEndSale = true;
                            SetPicture(2,DISPLAY_DESEA_IMPRIMIR_RECIBO); 
                        }
                        bufferDisplay2.flagActiveSale = false;
                        side.b.RFstateReport = 1;
                    }                                         
                break;
                case PUMP_FEOT: //Reporte de venta
                    if(getSale(side.b.dir)){
                        if(bufferDisplay2.flagPrint ==1){
                            imprimir(printPortB,side.b.dir);
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0); 
                        }else{
                            flowDisplay2 = 6;
                            bufferDisplay2.flagEndSale = true;
                            SetPicture(2,DISPLAY_DESEA_IMPRIMIR_RECIBO); 
                        }
                        bufferDisplay2.flagActiveSale = false;
                        side.b.RFstateReport = 1;
                    }
                break;
                
            }
            
        break;
            
        case 9: //Teclado           
            switch (alphanumeric_keyboard(2,10,0)){
                case 0: //Cancelar
                    switch(bufferDisplay2.flagKeyboard){
                        case 1://Placa
                            for(x=0;x<=10;x++){
                                bufferDisplay2.licenceSale[x]=0;
                            }
                            flowDisplay2 = 0;//Inicio
                            SetPicture(2,DISPLAY_INICIO0);
                        break;
                        
                        case 2://Kilometraje
                            for(x=0;x<=10;x++){
                                bufferDisplay2.mileageSale[x]=0;
                            }
                        break;
                        
                        case 3://CC/NIT
                            for(x=0;x<=10;x++){
                                bufferDisplay2.identySale[x]=0;
                            }
                        break;
                        case 4://ID
                            for(x=0;x<=10;x++){
                                bufferDisplay2.shiftId[x]=0;
                            }
                        break;
                    }                    
                    Display2_ClearRxBuffer();
                break;
                    
                case 1: //Enter
                    switch(bufferDisplay2.flagKeyboard){
                        case 1://Placa
                            for(x=0;x<=bufferDisplay2.valueKeys[0];x++){
                                bufferDisplay2.licenceSale[x]=bufferDisplay2.valueKeys[x];
                            }
                            if(bufferDisplay2.flagEndSale){
                                imprimir(printPortB,side.b.dir);
                                flowDisplay2 = 0;
                                SetPicture(2,DISPLAY_INICIO0); 
                            }else{
                                flowDisplay2 = 7;//Suba manija
                                SetPicture(2,DISPLAY_SUBA_MANIJA);
                            }
                        break;
                        
                        case 2://Kilometraje
                            for(x=0;x<=bufferDisplay2.valueKeys[0];x++){
                                bufferDisplay2.mileageSale[x]=bufferDisplay2.valueKeys[x];
                            }
                            flowDisplay2 = 7;//Suba manija
                            SetPicture(2,DISPLAY_SUBA_MANIJA);
                        break;
                        
                        case 3://CC/NIT
                            for(x=0;x<=bufferDisplay2.valueKeys[0];x++){
                                bufferDisplay2.identySale[x]=bufferDisplay2.valueKeys[x];
                            }
                        case 4://ID
                            for(x=0;x<=10;x++){
                                bufferDisplay2.shiftId[x] =bufferDisplay2.valueKeys[x];
                            }
                            flowDisplay2 = 14;//Pide clave                            
                            numberKeys1 = 0;
                            hiddenKeys = 10;
                            controlChar ='*';
                            bufferDisplay2.flagKeyboard = 3;
                            SetPicture(2,DISPLAY_INGRESE_PASSWORD);
                        break;
                    }                    
                    Display2_ClearRxBuffer();
                break;
            }            
        break;    
    ////////////////// CASOS PARA CRÉDITO  /////////////////////
        case 10:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0xB6:  //Solicitud ibutton  
                            flowDisplay2 = 11;
                            numberKeys1 = 0;                            
                            bufferDisplay2.flagPrint =  1;
                            SetPicture(2,DISPLAY_ESPERANDO_ID);                            
                        break; 
                        case 0xB7:  //ID por número
                            bufferDisplay2.flagKeyboard = 0;
                            flowDisplay2 = 7;//Esperando estado del dispensador  
                            count_protector=0;
                            SetPicture(2,DISPLAY_SUBA_MANIJA);                            
                        break;
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }                        
        break;
        
        case 11:
            isr_3_StartEx(animacion);
            Timer_Animacion_Start();
            if(touch_present(2)==1){
				if(touch_write(1,0x33)){
					for(x=1;x<=8;x++){
						temporal[x]=touch_read_byte(2);
					}
					y=0;
					for(x=1;x<8;x++){
                        y=crc_check(y,temporal[x]);
                    }
					if(y==temporal[8]){
						bufferDisplay2.idSerial[0]=16;
						y=16;
						for(x=1;x<=8;x++){
							if((temporal[x]&0x0F)>=10){
								bufferDisplay2.idSerial[y]=(temporal[x]&0x0F)+55;
							}else{
								bufferDisplay2.idSerial[y]=(temporal[x]&0x0F)+48;				
							}
                            y--;
							if(((temporal[x]>>4)&0x0F)>=10){
								bufferDisplay2.idSerial[y]=((temporal[x]>>4)&0x0F)+55;
							}else{
								bufferDisplay2.idSerial[y]=((temporal[x]>>4)&0x0F)+48;				
							}
                            y--;
						}                        
                        SetPicture(2,DISPLAY_ID_RECONOCIDO);                                           
                        Display2_ClearRxBuffer();
                        CyDelay(700);
                        bufferDisplay2.flagKeyboard = 2;
                        flowDisplay2 = 9;
                        SetPicture(2,DISPLAY_INTRODUZCA_KILOMETRAJE);
                        
					}
				}
			}
            if(count_protector>=60){
                flowDisplay2 = 0;	
    			count_protector=0;
                SetPicture(2,DISPLAY_INICIO0);
    		}
                                    
        break;    
    ///////////////FIN CASOS PARA CRÉDITO  /////////////////////  
    
    //////////// CASOS PARA CONFIGURACIONES  ///////////////////
        case 12:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x46:  //Turnos              
                            flowDisplay2 = 13; 
                            if(lockTurn ==1){
                                SetPicture(2,DISPLAY_CERRAR_TURNO);
                            }else{
                                SetPicture(2,DISPLAY_ABRIR_TURNO);
                            }                            
                        break;
                        case 0x55:  //Configurar módulo 
                            flowDisplay2 = 14;
                            bufferDisplay2.flagKeyboard = 6;
                            numberKeys1 = 0;
                            hiddenKeys = 5;
                            controlChar ='*';
                            SetPicture(2,DISPLAY_INGRESE_PASSWORD);                           
                        break;
                        case 0xB5:  //Copia de recibo 
                            flowDisplay2  = 0;
                            side.b.RFstateReport = 1;
                            side.b.rfState = RF_COPY_RECEIPT;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                        
                        case 0x3B:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                            
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                        
                        case 0x7E:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                    }
                }  
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }
        break;

        case 13:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x47:  //Cambio de turno: Abrir  
                            flowDisplay2 = 9;
                            numberKeys1 = 0;    
                            bufferDisplay2.flagKeyboard = 4;
                            SetPicture(2,DISPLAY_INTRODUZCA_CEDULA);                            
                        break;
                        case 0x48:  //Cambio de turno: Cerrar 
                            flowDisplay2 = 9;
                            numberKeys1 = 0;   
                            bufferDisplay2.flagKeyboard = 4;
                            SetPicture(2,DISPLAY_INTRODUZCA_CEDULA);                           
                        break;                         
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }                        
        break; 
            
        case 14: //Teclado general      
            switch (alphanumeric_keyboard(2,hiddenKeys,controlChar)){
                case 0: //Cancelar
                    switch(bufferDisplay2.flagKeyboard){
                        case 1://ID Estacion
                            for(x=0;x<=4;x++){
                                idStation[x]=0;
                            }
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);
                        break;
                        case 2://Umbral
                            for(x=0;x< hiddenKeys;x++){
                                pumpGap[x]=0;
                            }
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);
                        break;
                        case 3://Pass turno
                            for(x=0;x< hiddenKeys;x++){
                                bufferDisplay2.shiftPassword[x]=0;
                            }
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);
                        break;
                        case 5://Pass
                            for(x=0;x< hiddenKeys;x++){
                                bufferDisplay2.shiftPassword[x]=0;
                            }
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);
                        break;
                            
                        case 6://Pass
                            for(x=1;x<=configAccess[0];x++){
                                configAccess[x]=0;
                            }
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);
                        break;
                    }                    
                    Display2_ClearRxBuffer();
                break;
                    
                case 1: //Enter
                    switch(bufferDisplay2.flagKeyboard){   
                        case 1://ID Estacion
                            for(x=0;x <=4;x++){
                                idStation[x] =bufferDisplay2.valueKeys[x+1];                                
                            }
                            idStation[4] = 0x00;
                            intIDStation = atoi(idStation);
                            IDCast[0] = intIDStation;
                            IDCast[1] = intIDStation>>8;
                            EEPROM_1_WriteByte(IDCast[0],10);
                            EEPROM_1_WriteByte(IDCast[1],11);
                            flowDisplay2 = 15;
                            SetPicture(2,DISPLAY_CONFIGURACIONES);
                        break;
                        case 2://Pass
                            for(x=0;x < hiddenKeys;x++){
                                pumpGap[x] =bufferDisplay2.valueKeys[x];
                            }
                            flowDisplay2 = 15;
                            SetPicture(2,DISPLAY_CONFIGURACIONES);
                        break;
                        case 3://Pass turno
                            for(x=0;x < hiddenKeys;x++){
                                bufferDisplay2.shiftPassword[x] =bufferDisplay2.valueKeys[x];
                            }
                            flowDisplay2 = 19;
                            SetPicture(2,DISPLAY_POR_FAVOR_ESPERE);
                        break;
                        case 5://Pass
                            for(x=0;x < hiddenKeys;x++){
                                bufferDisplay2.shiftPassword[x] =bufferDisplay2.valueKeys[x];
                            }
                            flowDisplay2 = 15;
                            SetPicture(2,DISPLAY_CONFIGURACIONES);
                        break; 
                        case 6://Pass
                            for(x=1;x<=configAccess[0];x++){
                                configAccess[x] =bufferDisplay2.valueKeys[x];
                            }
                            if(configAccess[1] == passwordPump[1] && configAccess[2] == passwordPump[2] && configAccess[3] == passwordPump[3] && configAccess[4] == passwordPump[4]){                                
                                SetPicture(2,DISPLAY_PASSWORD_VALIDO);
                                CyDelay(700);
                                flowDisplay2 = 15;
                                SetPicture(2,DISPLAY_CONFIGURACIONES);
                            }else{
                                SetPicture(2,DISPLAY_PASSWORD_INVALIDO);
                                CyDelay(700);
                                flowDisplay2 = 0;
                                SetPicture(2,DISPLAY_INICIO0);
                            }
                        break;
                    }                    
                    Display2_ClearRxBuffer();
                break;
            }            
        break;
            
        case 15:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x82:  //ID Estacion  
                            flowDisplay2 = 14;
                            numberKeys1 = 0; 
                            controlChar = 0;
                            hiddenKeys  = 5;
                            bufferDisplay2.flagKeyboard = 1;
                            SetPicture(2,DISPLAY_INTRODUZCA_VALOR);                            
                        break;
                        case 0x65:  //Test Impresoras 
                            flowDisplay2 = 16; 
                            PrintTest();
                            SetPicture(2,DISPLAY_AMBAS_IMPRESORAS_FUNCIONANDO);                           
                        break;
                        
                        case 0x5A:  //Umbral 
                            flowDisplay2 = 14;
                            numberKeys1 = 0;
                            controlChar = 0;
                            hiddenKeys  = 3;
                            pumpGap[0]  = 2;
                            bufferDisplay2.flagKeyboard = 2;
                            SetPicture(2,DISPLAY_INTRODUZCA_VALOR);                            
                        break;
                        case 0x58:  //Hora y Fecha
                            flowDisplay2 = 18;
                            numberKeys1 = 0;   
                            bufferDisplay2.flagKeyboard = 4;
                            SetPicture(2,DISPLAY_CONFIGURAR_FECHA_HORA);                           
                        break;
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }                        
        break; 
            
        case 16:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x38:  //No funcionan  
                            flowDisplay2 = 17;
                            SetPicture(2,DISPLAY_IMPRESORA_NO_FUNCIONANDO);                            
                        break;
                        case 0x39:  //Si funcionan
                            flowDisplay2 = 15; 
                            printPortA = 1;
                            printPortB = 2;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(2,DISPLAY_CONFIGURACIONES);
                        break;
                                               
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }                        
        break;
            
        case 17:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x8C:  //1 no funciona  
                            flowDisplay2 = 15; 
                            printPortA = 2;
                            printPortB = 2;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(2,DISPLAY_CONFIGURACIONES);                            
                        break;
                        case 0x8D:  //2 no funciona
                            flowDisplay2 = 15; 
                            printPortA = 1;
                            printPortB = 1;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(2,DISPLAY_CONFIGURACIONES); 
                        break;
                            
                        case 0x8E:  //ninguna funciona
                            flowDisplay2 = 15; 
                            printPortA = 0;
                            printPortB = 0;
                            EEPROM_1_WriteByte(printPortA,8);
                            EEPROM_1_WriteByte(printPortB,9);
                            SetPicture(2,DISPLAY_CONFIGURACIONES); 
                        break;
                                               
                        case 0x7E:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                        case 0x94:  //Pantalla Inicial                                                        
                            SetPicture(2,DISPLAY_INICIO0);
                            flowDisplay2 = 0;
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }                        
        break;
            
        case 18:
            if(leer_hora()==1){
                WriteLCD(2,(((timeDownHandle[1]&0x10)>>4)+48),13,8,1,0x0000,'N');
                WriteLCD(2,((timeDownHandle[1]&0x0F)+48),13,9,1,0x0000,'N');
                WriteLCD(2,':',13,10,1,0x0000,'N');
                WriteLCD(2,(((timeDownHandle[0]&0xF0)>>4)+48),13,11,1,0x0000,'N');
                WriteLCD(2,((timeDownHandle[0]&0x0F)+48),13,12,1,0x0000,'N');               
            }
            if(leer_fecha()==1){                
                WriteLCD(2,(((dateDownHandle[0]&0x30)>>4)+48),21,8,1,0x0000,'N');
                WriteLCD(2,((dateDownHandle[0]&0x0F)+48),21,9,1,0x0000,'N');
                WriteLCD(2,'/',21,10,1,0x0000,'N');
                WriteLCD(2,(((dateDownHandle[1]&0x10)>>4)+48),21,11,1,0x0000,'N');
                WriteLCD(2,((dateDownHandle[1]&0x0F)+48),21,12,1,0x0000,'N');
                WriteLCD(2,'/',21,13,1,0x0000,'N');
                WriteLCD(2,(((dateDownHandle[2]&0xF0)>>4)+48),21,14,1,0x0000,'N');
                WriteLCD(2,((dateDownHandle[2]&0x0F)+48),21,15,1,0x0000,'N');
            }
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){                                                                       
                        case 0x7E:  //Pantalla Inicial  
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                        case 0x94:  //Pantalla Inicial    
                            flowDisplay2 = 0;
                            SetPicture(2,DISPLAY_INICIO0);                            
                        break;
                    }                    
                }
                CyDelay(10);         
                Display2_ClearRxBuffer();
            }                        
        break;
            
        case 19:            
            side.b.RFstateReport = 1;
            ShiftState =1;
        break;
    countLCD = 0;       
    }    
    }


/*
*********************************************************************************************************
*                                         CY_ISR(animacion)
*
* Description : Interrupcion que temporiza las imagenes informativas que aparecen en la pantalla 1
*               
*
*********************************************************************************************************
*/
CY_ISR(animacion){
    Timer_Animacion_ReadStatusRegister();    					
    count_protector++; 							//Incrementa el contador 
}

/*
*********************************************************************************************************
*                                         CY_ISR(animacion2)
*
* Description : Interrupcion que temporiza las imagenes informativas que aparecen en la pantalla 2
*               
*
*********************************************************************************************************
*/
CY_ISR(animacion2){
    Timer_Animacion2_ReadStatusRegister();
    count_protector2++;
}
/*
*********************************************************************************************************
*                                         CY_ISR(timerBeagleTX1)
*
* Description : Interrupcion que temporiza tiempos de espera
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
CY_ISR(timerRFRX){
    Waitable_3_ReadStatusRegister();
    countRX++;
    countTX++;
}


/*
*********************************************************************************************************
*                                         CY_ISR(timerPump)
*
* Description : Interrupcion que temporiza tiempos de espera para preguntar al surtidor
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
//CY_ISR(timerPump){
//    Waitable_4_ReadStatusRegister();
//    countPump++;
//    countLCD++;
//}


int main()
{    
    CyGlobalIntEnable; /* Enable global interrupts. */
    GlobalInitializer();
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */       
    InitPump(); 
    loadConfiguration();
    CyWdtStart(CYWDT_1024_TICKS,CYWDT_LPMODE_NOCHANGE);
    for(;;)
    {            
        CyWdtClear();
        pollingRF_Rx();
        PollingDisplay1(); 
        CyWdtClear();
        PollingDisplay2();
        CyWdtClear();
        if(pollTotals ==1){
            if(getTotals(side.a.dir)!=0){
                side.a.RFstateReport = 1;
                side.b.RFstateReport = 0;
            }
        CyWdtClear();
        }
        if(pollTotals ==2){
            if(getTotals(side.b.dir)!=0){
                side.b.RFstateReport = 1;
                side.a.RFstateReport = 0;
            }
        CyWdtClear();
        }
        counterRF++;
        if(counterRF >10)
            ActiveRF =0; 
        
    }
}












/* [] END OF FILE */
