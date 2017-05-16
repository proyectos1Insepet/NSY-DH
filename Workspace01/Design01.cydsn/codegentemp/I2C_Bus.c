/*******************************************************************************
* File Name: I2C_Bus.c
* Version 3.30
*
* Description:
*  This file provides the source code of APIs for the I2C component.
*  Actual protocol and operation code resides in the interrupt service routine
*  file.
*
* Note:
*
*******************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "I2C_Bus_PVT.h"


/**********************************
*      System variables
**********************************/

uint8 I2C_Bus_initVar = 0u;    /* Defines if component was initialized */

volatile uint8 I2C_Bus_state;  /* Current state of I2C FSM */


/*******************************************************************************
* Function Name: I2C_Bus_Init
********************************************************************************
*
* Summary:
*  Initializes I2C registers with initial values provided from customizer.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  None
*
* Reentrant:
*  No
*
*******************************************************************************/
void I2C_Bus_Init(void) 
{
    #if(I2C_Bus_FF_IMPLEMENTED)
        I2C_Bus_CFG_REG  = I2C_Bus_DEFAULT_CFG;
        I2C_Bus_XCFG_REG = I2C_Bus_DEFAULT_XCFG;

        #if(CY_PSOC5A)
            I2C_Bus_CLKDIV_REG  = LO8(I2C_Bus_DEFAULT_DIVIDE_FACTOR);
        #else
            I2C_Bus_CLKDIV1_REG = LO8(I2C_Bus_DEFAULT_DIVIDE_FACTOR);
            I2C_Bus_CLKDIV2_REG = HI8(I2C_Bus_DEFAULT_DIVIDE_FACTOR);
        #endif /* (CY_PSOC5A) */

    #else
        uint8 enableInterrupts;

        I2C_Bus_CFG_REG      = I2C_Bus_DEFAULT_CFG;      /* control  */
        I2C_Bus_INT_MASK_REG = I2C_Bus_DEFAULT_INT_MASK; /* int_mask */

        /* Enable interrupts from block */
        enableInterrupts = CyEnterCriticalSection();
        I2C_Bus_INT_ENABLE_REG |= I2C_Bus_INTR_ENABLE; /* aux_ctl */
        CyExitCriticalSection(enableInterrupts);

        #if(I2C_Bus_MODE_MASTER_ENABLED)
            I2C_Bus_MCLK_PRD_REG = I2C_Bus_DEFAULT_MCLK_PRD;
            I2C_Bus_MCLK_CMP_REG = I2C_Bus_DEFAULT_MCLK_CMP;
         #endif /* (I2C_Bus_MODE_MASTER_ENABLED) */

        #if(I2C_Bus_MODE_SLAVE_ENABLED)
            I2C_Bus_PERIOD_REG = I2C_Bus_DEFAULT_PERIOD;
        #endif  /* (I2C_Bus_MODE_SLAVE_ENABLED) */

    #endif /* (I2C_Bus_FF_IMPLEMENTED) */

    #if(I2C_Bus_TIMEOUT_ENABLED)
        I2C_Bus_TimeoutInit();
    #endif /* (I2C_Bus_TIMEOUT_ENABLED) */

    /* Disable Interrupt and set vector and priority */
    CyIntDisable    (I2C_Bus_ISR_NUMBER);
    CyIntSetPriority(I2C_Bus_ISR_NUMBER, I2C_Bus_ISR_PRIORITY);
    #if(I2C_Bus_INTERN_I2C_INTR_HANDLER)
        (void) CyIntSetVector(I2C_Bus_ISR_NUMBER, &I2C_Bus_ISR);
    #endif /* (I2C_Bus_INTERN_I2C_INTR_HANDLER) */


    /* Put state machine in idle state */
    I2C_Bus_state = I2C_Bus_SM_IDLE;

    #if(I2C_Bus_MODE_SLAVE_ENABLED)
        /* Reset status and buffers index */
        I2C_Bus_SlaveClearReadBuf();
        I2C_Bus_SlaveClearWriteBuf();
        I2C_Bus_slStatus = 0u; /* Reset slave status */

        /* Set default address */
        I2C_Bus_SlaveSetAddress(I2C_Bus_DEFAULT_ADDR);
    #endif /* (I2C_Bus_MODE_SLAVE_ENABLED) */

    #if(I2C_Bus_MODE_MASTER_ENABLED)
        /* Reset status and buffers index */
        I2C_Bus_MasterClearReadBuf();
        I2C_Bus_MasterClearWriteBuf();
        (void) I2C_Bus_MasterClearStatus();
    #endif /* (I2C_Bus_MODE_MASTER_ENABLED) */
}


/*******************************************************************************
* Function Name: I2C_Bus_Enable
********************************************************************************
*
* Summary:
*  Enables I2C operations.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  None
*
*******************************************************************************/
void I2C_Bus_Enable(void) 
{
    #if(I2C_Bus_FF_IMPLEMENTED)
        uint8 enableInterrupts;

        /* Enable power to I2C FF block */
        enableInterrupts = CyEnterCriticalSection();
        I2C_Bus_ACT_PWRMGR_REG  |= I2C_Bus_ACT_PWR_EN;
        I2C_Bus_STBY_PWRMGR_REG |= I2C_Bus_STBY_PWR_EN;
        CyExitCriticalSection(enableInterrupts);

    #else

        #if(I2C_Bus_MODE_SLAVE_ENABLED)
            uint8 enableInterrupts;
        #endif /* (I2C_Bus_MODE_SLAVE_ENABLED) */

        #if(I2C_Bus_MODE_SLAVE_ENABLED)
            /* Enable slave bit counter */
            enableInterrupts = CyEnterCriticalSection();
            I2C_Bus_COUNTER_AUX_CTL_REG |= I2C_Bus_CNT7_ENABLE;   /* aux_ctl */
            CyExitCriticalSection(enableInterrupts);
        #endif /* (I2C_Bus_MODE_SLAVE_ENABLED) */

        I2C_Bus_CFG_REG |= I2C_Bus_ENABLE_MS;

    #endif /* (I2C_Bus_FF_IMPLEMENTED) */

    #if(I2C_Bus_TIMEOUT_ENABLED)
        I2C_Bus_TimeoutEnable();
    #endif /* (I2C_Bus_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: I2C_Bus_Start
********************************************************************************
*
* Summary:
*  Starts the I2C hardware. Enables Active mode power template bits or clock
*  gating as appropriate. It is required to be executed before I2C bus
*  operation.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  This component automatically enables it's interrupt.  If I2C is enabled
*  without the interrupt enabled, it could lock up the I2C bus.
*
* Global variables:
*  I2C_Bus_initVar - used to check initial configuration, modified
*  on first function call.
*
* Reentrant:
*  No
*
*******************************************************************************/
void I2C_Bus_Start(void) 
{
    /* Initialize I2C registers, reset I2C buffer index and clears status */
    if(0u == I2C_Bus_initVar)
    {
        I2C_Bus_Init();
        I2C_Bus_initVar = 1u; /* Component initialized */
    }

    I2C_Bus_Enable();
    I2C_Bus_EnableInt();
}


/*******************************************************************************
* Function Name: I2C_Bus_Stop
********************************************************************************
*
* Summary:
*  Disables I2C hardware and disables I2C interrupt. Disables Active mode power
*  template bits or clock gating as appropriate.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void I2C_Bus_Stop(void) 
{
    #if((I2C_Bus_FF_IMPLEMENTED)  || \
        (I2C_Bus_UDB_IMPLEMENTED && I2C_Bus_MODE_SLAVE_ENABLED))
        uint8 enableInterrupts;
    #endif /* ((I2C_Bus_FF_IMPLEMENTED)  || \
               (I2C_Bus_UDB_IMPLEMENTED && I2C_Bus_MODE_SLAVE_ENABLED)) */

    I2C_Bus_DisableInt();

    I2C_Bus_DISABLE_INT_ON_STOP;   /* Interrupt on Stop can be enabled by write */
    (void) I2C_Bus_CSR_REG;        /* Clear CSR reg */
    
    #if(I2C_Bus_TIMEOUT_ENABLED)
        I2C_Bus_TimeoutStop();
    #endif  /* End (I2C_Bus_TIMEOUT_ENABLED) */

    #if(I2C_Bus_FF_IMPLEMENTED)
        #if(CY_PSOC3 || CY_PSOC5LP)
            /* Store registers which are held in reset when Master and Slave bits are cleared */
            #if(I2C_Bus_MODE_SLAVE_ENABLED)
                I2C_Bus_backup.addr = I2C_Bus_ADDR_REG;
            #endif /* (I2C_Bus_MODE_SLAVE_ENABLED) */

            I2C_Bus_backup.clkDiv1  = I2C_Bus_CLKDIV1_REG;
            I2C_Bus_backup.clkDiv2  = I2C_Bus_CLKDIV2_REG;


            /* Reset FF block */
            I2C_Bus_CFG_REG &= ((uint8) ~I2C_Bus_ENABLE_MS);
            CyDelayUs(I2C_Bus_FF_RESET_DELAY);
            I2C_Bus_CFG_REG |= ((uint8)  I2C_Bus_ENABLE_MS);


            /* Restore registers */
            #if(I2C_Bus_MODE_SLAVE_ENABLED)
                I2C_Bus_ADDR_REG = I2C_Bus_backup.addr;
            #endif /* (I2C_Bus_MODE_SLAVE_ENABLED) */

            I2C_Bus_CLKDIV1_REG = I2C_Bus_backup.clkDiv1;
            I2C_Bus_CLKDIV2_REG = I2C_Bus_backup.clkDiv2;

        #endif /* (CY_PSOC3 || CY_PSOC5LP) */

        /* Disable power to I2C block */
        enableInterrupts = CyEnterCriticalSection();
        I2C_Bus_ACT_PWRMGR_REG  &= ((uint8) ~I2C_Bus_ACT_PWR_EN);
        I2C_Bus_STBY_PWRMGR_REG &= ((uint8) ~I2C_Bus_STBY_PWR_EN);
        CyExitCriticalSection(enableInterrupts);

    #else

        #if(I2C_Bus_MODE_SLAVE_ENABLED)
            /* Disable slave bit counter */
            enableInterrupts = CyEnterCriticalSection();
            I2C_Bus_COUNTER_AUX_CTL_REG &= ((uint8) ~I2C_Bus_CNT7_ENABLE);
            CyExitCriticalSection(enableInterrupts);
        #endif /* (I2C_Bus_MODE_SLAVE_ENABLED) */

        I2C_Bus_CFG_REG &= ((uint8) ~I2C_Bus_ENABLE_MS);

    #endif /* (I2C_Bus_FF_IMPLEMENTED) */

    I2C_Bus_ClearPendingInt();  /* Clear interrupt triggers on reset */

    I2C_Bus_state = I2C_Bus_SM_IDLE;  /* Reset software FSM */
}


/* [] END OF FILE */
