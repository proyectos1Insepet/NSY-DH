/*******************************************************************************
* File Name: Pump_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Pump.h"


/***************************************
* Local data allocation
***************************************/

static Pump_BACKUP_STRUCT  Pump_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: Pump_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the Pump_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Pump_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Pump_SaveConfig(void)
{
    #if(Pump_CONTROL_REG_REMOVED == 0u)
        Pump_backup.cr = Pump_CONTROL_REG;
    #endif /* End Pump_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Pump_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Pump_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling Pump_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void Pump_RestoreConfig(void)
{
    #if(Pump_CONTROL_REG_REMOVED == 0u)
        Pump_CONTROL_REG = Pump_backup.cr;
    #endif /* End Pump_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: Pump_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The Pump_Sleep() API saves the current component state. Then it
*  calls the Pump_Stop() function and calls 
*  Pump_SaveConfig() to save the hardware configuration.
*  Call the Pump_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Pump_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Pump_Sleep(void)
{
    #if(Pump_RX_ENABLED || Pump_HD_ENABLED)
        if((Pump_RXSTATUS_ACTL_REG  & Pump_INT_ENABLE) != 0u)
        {
            Pump_backup.enableState = 1u;
        }
        else
        {
            Pump_backup.enableState = 0u;
        }
    #else
        if((Pump_TXSTATUS_ACTL_REG  & Pump_INT_ENABLE) !=0u)
        {
            Pump_backup.enableState = 1u;
        }
        else
        {
            Pump_backup.enableState = 0u;
        }
    #endif /* End Pump_RX_ENABLED || Pump_HD_ENABLED*/

    Pump_Stop();
    Pump_SaveConfig();
}


/*******************************************************************************
* Function Name: Pump_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  Pump_Sleep() was called. The Pump_Wakeup() function
*  calls the Pump_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  Pump_Sleep() function was called, the Pump_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Pump_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Pump_Wakeup(void)
{
    Pump_RestoreConfig();
    #if( (Pump_RX_ENABLED) || (Pump_HD_ENABLED) )
        Pump_ClearRxBuffer();
    #endif /* End (Pump_RX_ENABLED) || (Pump_HD_ENABLED) */
    #if(Pump_TX_ENABLED || Pump_HD_ENABLED)
        Pump_ClearTxBuffer();
    #endif /* End Pump_TX_ENABLED || Pump_HD_ENABLED */

    if(Pump_backup.enableState != 0u)
    {
        Pump_Enable();
    }
}


/* [] END OF FILE */
