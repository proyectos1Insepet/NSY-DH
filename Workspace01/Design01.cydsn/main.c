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
void InitLCD(){      
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
        SetPicture(2,DISPLAY_INICIO0); 
        ShowMessage(1,iniText,0);    
        CyDelay(500);
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
        SetPicture(2,DISPLAY_SELECCIONE_POSICION); 
        ShowMessage(1,iniText,0);    
        CyDelay(500);
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
        SetPicture(2,DISPLAY_ERROR);
    }     
    ShowMessage(2,MuxVersion,0);
}
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
            InitLCD(); 
        }        
    }
    
}

/*
*********************************************************************************************************
*                                         void polling_Pump(void)
*
* Description : Pregunta estado al surtidor cada segundo
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
    }
}

/* [] END OF FILE */
