/*******************************************************************************
* File Name: RF_Connection_PM.c
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

#include "RF_Connection.h"


/***************************************
* Local data allocation
***************************************/

static RF_Connection_BACKUP_STRUCT  RF_Connection_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: RF_Connection_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the RF_Connection_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RF_Connection_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RF_Connection_SaveConfig(void)
{
    #if(RF_Connection_CONTROL_REG_REMOVED == 0u)
        RF_Connection_backup.cr = RF_Connection_CONTROL_REG;
    #endif /* End RF_Connection_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: RF_Connection_RestoreConfig
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
*  RF_Connection_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling RF_Connection_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void RF_Connection_RestoreConfig(void)
{
    #if(RF_Connection_CONTROL_REG_REMOVED == 0u)
        RF_Connection_CONTROL_REG = RF_Connection_backup.cr;
    #endif /* End RF_Connection_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: RF_Connection_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The RF_Connection_Sleep() API saves the current component state. Then it
*  calls the RF_Connection_Stop() function and calls 
*  RF_Connection_SaveConfig() to save the hardware configuration.
*  Call the RF_Connection_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RF_Connection_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RF_Connection_Sleep(void)
{
    #if(RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)
        if((RF_Connection_RXSTATUS_ACTL_REG  & RF_Connection_INT_ENABLE) != 0u)
        {
            RF_Connection_backup.enableState = 1u;
        }
        else
        {
            RF_Connection_backup.enableState = 0u;
        }
    #else
        if((RF_Connection_TXSTATUS_ACTL_REG  & RF_Connection_INT_ENABLE) !=0u)
        {
            RF_Connection_backup.enableState = 1u;
        }
        else
        {
            RF_Connection_backup.enableState = 0u;
        }
    #endif /* End RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED*/

    RF_Connection_Stop();
    RF_Connection_SaveConfig();
}


/*******************************************************************************
* Function Name: RF_Connection_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  RF_Connection_Sleep() was called. The RF_Connection_Wakeup() function
*  calls the RF_Connection_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  RF_Connection_Sleep() function was called, the RF_Connection_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RF_Connection_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RF_Connection_Wakeup(void)
{
    RF_Connection_RestoreConfig();
    #if( (RF_Connection_RX_ENABLED) || (RF_Connection_HD_ENABLED) )
        RF_Connection_ClearRxBuffer();
    #endif /* End (RF_Connection_RX_ENABLED) || (RF_Connection_HD_ENABLED) */
    #if(RF_Connection_TX_ENABLED || RF_Connection_HD_ENABLED)
        RF_Connection_ClearTxBuffer();
    #endif /* End RF_Connection_TX_ENABLED || RF_Connection_HD_ENABLED */

    if(RF_Connection_backup.enableState != 0u)
    {
        RF_Connection_Enable();
    }
}


/* [] END OF FILE */
