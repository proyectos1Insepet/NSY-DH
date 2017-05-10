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
#include <LCD.h>
#include <device.h>

/**********************************/

uint8 MuxVersion [10] = "MUX V. 1.0";

void GlobalInitializer()
{
    EEPROM_1_Start();
    Pump_Start(); 
    Display1_Start();
    Display2_Start();
    RF_Physical_Start();    
    Printer_Start();           
}
void StoreConfiguration(){
    EEPROM_1_WriteByte(UnitType,0);
    EEPROM_1_WriteByte(ConversionFactor,1);
    EEPROM_1_WriteByte(MoneyDec,2);
    EEPROM_1_WriteByte(VolDec,3);
    EEPROM_1_WriteByte(PPUDec,4);
    EEPROM_1_WriteByte(DDMode,5);
}
/*
*********************************************************************************************************
*                                         void InitLCD1(void)
*
* Description : Pregunta configuraciones al dispensador 
*               
*
*********************************************************************************************************
*/
void InitLCD1(){      
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
            ShowMessage(1,Unit,0);  //Mostrar tipo de dispensador      
        }
        if(UnitType == 1){
            ShowMessage(1,Unit2,0);
        }
        if(UnitType == 2){
           ShowMessage(1,Unit3,0);
        }
        if(UnitType == 3){
            ShowMessage(1,Unit4,0);
        } 
        if(DDMode == 0){
            ShowMessage(1,DisplayMode0,22); //Mostrar modo de display
        }
        if(DDMode == 1){
            ShowMessage(1,DisplayMode,22); //Mostrar modo de display
        }
        if(DDMode == 2){
           ShowMessage(1,DisplayMode2,22);
        }
        if(DDMode == 3){
           ShowMessage(1,DisplayMode3,22);
        }        
    }
    if(NumPositions == 4){
        SetPicture(1,DISPLAY_SELECCIONE_POSICION);        
        ShowMessage(1,iniText,0);    
        CyDelay(300);
        if(UnitType == 0){
            ShowMessage(1,Unit,0);  //Mostrar tipo de dispensador      
        }
        if(UnitType == 1){
            ShowMessage(1,Unit2,0);
        }
        if(UnitType == 2){
           ShowMessage(1,Unit3,0);
        }
        if(UnitType == 3){
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
*                                         void InitLCD1(void)
*
* Description : Pregunta configuraciones al dispensador 
*               
*
*********************************************************************************************************
*/
void InitLCD2(){      
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
            PumpCompleteConfiguration(side.a.dir);
            StoreConfiguration();            
        }        
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

void PollingPump(void){
    uint8 x;
    for(x= 0; x < 4; x++){
        PrevStatePump[x] = statePump[x];
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
    }
    if(!PumpIsInValidState(get_state(side.b.dir))){
        SetPicture(1,DISPLAY_ERROR);
        SetPicture(2,DISPLAY_ERROR);
        NumPositions = 0;
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
    switch(flowDisplay1){
        case 0:
            InitLCD1();
            flowDisplay1 =1;
        break;
        case 1: //Menu
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x3A:  //Pantalla forma de pago
                            flowDisplay1 = 2;                            
                            SetPicture(1,DISPLAY_FORMA_PAGO_DESEADA);                            
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
        case 2:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x0D:  //Pantalla efectivo              
                            flowDisplay1 = 3;                            
                            SetPicture(1,DISPLAY_FORMA_PROGRAMACION);                            
                        break;
                        case 0x0E:  //Pantalla credito   
                            flowDisplay1 = 4;                                                     
                            SetPicture(1,DISPLAY_ID_DIGITAL);                            
                        break;
                        case 0x45:  //Pantalla otras opciones 
                            flowDisplay1 = 5;                            
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
                            SetPicture(1,DISPLAY_INTRODUZCA_VALOR);                            
                        break; 
                        case 0x10:  //Preset volumen                 
                            flowDisplay1 = 5;
                            SetPicture(1,DISPLAY_INTRODUZCA_VOLUMEN);                            
                        break;
                        case 0x43:  //Preset full                       
                            flowDisplay1 = 6;
                            SetPicture(1,DISPLAY_DESEA_IMPRIMIR_RECIBO);                            
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
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x94:  //Retroceso 
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
            
        case 5: //Preset por volumen
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x94:  //Retroceso 
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
        
        case 6:
            if(Display1_GetRxBufferSize()==8){
                if((Display1_rxBuffer[0]==0xAA) && (Display1_rxBuffer[6]==0xC3) && (Display1_rxBuffer[7]==0x3C)){
                    switch(Display1_rxBuffer[3]){
                        case 0x39:  //Si copia                                                        
                            SetPicture(1,DISPLAY_DIGITE_PLACA);
                            flowDisplay1 = 7;
                        break; 
                        case 0x38:  //No copia                                                       
                            SetPicture(1,DISPLAY_SELECCIONE_PRODUCTO4);
                            flowDisplay1 = 8;//caso para seleccion de producto
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
    switch(flowDisplay2){
        case 0:
            InitLCD2();
            flowDisplay2 =1;
        break;
        case 1: //Menu
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x3A:  //Pantalla forma de pago
                            flowDisplay2 = 2;                            
                            SetPicture(2,DISPLAY_FORMA_PAGO_DESEADA);                            
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
        case 2:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x0D:  //Pantalla efectivo              
                            flowDisplay2 = 3;                            
                            SetPicture(2,DISPLAY_FORMA_PROGRAMACION);                            
                        break;
                        case 0x0E:  //Pantalla credito   
                            flowDisplay2 = 4;                                                     
                            SetPicture(2,DISPLAY_ID_DIGITAL);                            
                        break;
                        case 0x45:  //Pantalla otras opciones 
                            flowDisplay2 = 5;                            
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
                            SetPicture(2,DISPLAY_INTRODUZCA_VALOR);                            
                        break; 
                        case 0x10:  //Preset volumen                 
                            flowDisplay2 = 5;
                            SetPicture(2,DISPLAY_INTRODUZCA_VOLUMEN);                            
                        break;
                        case 0x43:  //Preset full                       
                            flowDisplay2 = 6;
                            SetPicture(2,DISPLAY_DESEA_IMPRIMIR_RECIBO);                            
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
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x94:  //Retroceso 
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
            
        case 5: //Preset por volumen
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x94:  //Retroceso 
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
        
        case 6:
            if(Display2_GetRxBufferSize()==8){
                if((Display2_rxBuffer[0]==0xAA) && (Display2_rxBuffer[6]==0xC3) && (Display2_rxBuffer[7]==0x3C)){
                    switch(Display2_rxBuffer[3]){
                        case 0x39:  //Si copia                                                        
                            SetPicture(2,DISPLAY_DIGITE_PLACA);
                            flowDisplay2 = 7;
                        break; 
                        case 0x38:  //No copia                                                       
                            SetPicture(2,DISPLAY_SELECCIONE_PRODUCTO4);
                            flowDisplay2 = 8;//caso para seleccion de producto
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
    }
}

int main()
{
    uint8 x;
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    for(x= 0; x < 4; x++){
        statePump[x]=0;
    }
    GlobalInitializer();
    InitPump();        
    for(;;)
    {
        CyWdtClear();
        PollingPump();
        CyWdtClear();
        ShowRectangle(2,35);
        CyWdtClear();
        PollingDisplay1();
        CyWdtClear();
        PollingDisplay2();
    }
}












/* [] END OF FILE */
