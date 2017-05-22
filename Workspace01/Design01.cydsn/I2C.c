/*
*********************************************************************************************************
*                                           GRP550/700 CODE
*
*                             (c) Copyright 2013; Sistemas Insepet LTDA
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
*                                             GRP550/700 CODE
*
*                                             CYPRESS PSoC5LP
*                                                with the
*                                            CY8C5969AXI-LP035
*
* Filename      : I2C.c
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
#include <Variables.h>
#include "LCD.h"



/*
*********************************************************************************************************
*                                   uint8 write_psoc1( uint8 puerto, uint8 valor )
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

uint8 write_psoc1( uint8 puerto, uint8 valor ){
	uint8 status, dato[2],i;
	dato[0]=puerto;
	dato[1]=valor;
	I2C_Bus_MasterClearStatus();
    status = I2C_Bus_MasterSendStart(7u, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<2; i++)
        {
            status = I2C_Bus_MasterWriteByte(dato[i]);
            if(status != I2C_Bus_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_Bus_MasterSendStop();	
	return 1;
}
/*
*********************************************************************************************************
*                                         uint8 leer_fecha( void )
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
uint8 leer_fecha(){
	uint8 status,i;
	for(i=4;i<=6;i++){
        I2C_Bus_MasterClearStatus();
        status = I2C_Bus_MasterSendStart(0x68, I2C_Bus_WRITE_XFER_MODE);
        if(I2C_Bus_MSTR_NO_ERROR == status)								 		/* Check if transfer completed without errors */
    	{
            status = I2C_Bus_MasterWriteByte(i);
            if(status != I2C_Bus_MSTR_NO_ERROR)
            {
                return 0;
            }
			else{
		        I2C_Bus_MasterSendStop(); 										/* Send Stop */
		        CyDelay(10);
		        status = I2C_Bus_MasterSendStart(0x68, I2C_Bus_READ_XFER_MODE);
		        if(I2C_Bus_MSTR_NO_ERROR == status){
		            dateDownHandle[i-4] = I2C_Bus_MasterReadByte(I2C_Bus_NAK_DATA);
		        }
				else{
					return 0;
				}
		        I2C_Bus_MasterSendStop();	
			}
        }
		else{
			return 0;
		}		
	}
	return 1;
}
/*
*********************************************************************************************************
*                                         uint8 write_fecha( void )
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

uint8 write_fecha( void ){
	uint8 status, dato[4],i;
	dato[0]=4;
	dato[1]=date[0];
	dato[2]=date[1];
	dato[3]=date[2];
	I2C_Bus_MasterClearStatus();
    status = I2C_Bus_MasterSendStart(0x68, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<4; i++)
        {
            status = I2C_Bus_MasterWriteByte(dato[i]);
            if(status != I2C_Bus_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_Bus_MasterSendStop();	
	return 1;
}
/*
*********************************************************************************************************
*                                         uint8 leer_hora( void )
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
uint8 leer_hora(){
	uint8 status,i;
	for(i=1;i<=2;i++){
        I2C_Bus_MasterClearStatus();
        status = I2C_Bus_MasterSendStart(0x68, I2C_Bus_WRITE_XFER_MODE);
        if(I2C_Bus_MSTR_NO_ERROR == status)								 		/* Check if transfer completed without errors */
    	{
            status = I2C_Bus_MasterWriteByte(i);
            if(status != I2C_Bus_MSTR_NO_ERROR)
            {
                return 0;
            }
			else{
		        I2C_Bus_MasterSendStop(); 										/* Send Stop */
		        CyDelay(10);
		        status = I2C_Bus_MasterSendStart(0x68, I2C_Bus_READ_XFER_MODE);
		        if(I2C_Bus_MSTR_NO_ERROR == status){
		            timeDownHandle[i-1] = I2C_Bus_MasterReadByte(I2C_Bus_NAK_DATA);
		        }
				else{
					return 0;
				}
		        I2C_Bus_MasterSendStop();	
			}
        }
		else{
			return 0;
		}		
	}
	return 1;
}
/*
*********************************************************************************************************
*                                         uint8 write_hora( void )
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

uint8 write_hora( void ){
	uint8 status, dato[3],i;
	dato[0]=1;
	dato[1]=time[0];
	dato[2]=time[1];
	I2C_Bus_MasterClearStatus();
    status = I2C_Bus_MasterSendStart(0x68, I2C_Bus_WRITE_XFER_MODE);
    if(I2C_Bus_MSTR_NO_ERROR == status) 
    {
        for(i=0; i<3; i++)
        {
            status = I2C_Bus_MasterWriteByte(dato[i]);
            if(status != I2C_Bus_MSTR_NO_ERROR)
            {
                return 0;
            }
        }
    }
    else{
		return 0;
    }
    I2C_Bus_MasterSendStop();	
	return 1;
}

/* [] END OF FILE */
