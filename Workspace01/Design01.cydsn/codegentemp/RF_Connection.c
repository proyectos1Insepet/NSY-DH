/*******************************************************************************
* File Name: RF_Connection.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
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
#if (RF_Connection_INTERNAL_CLOCK_USED)
    #include "RF_Connection_IntClock.h"
#endif /* End RF_Connection_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 RF_Connection_initVar = 0u;

#if (RF_Connection_TX_INTERRUPT_ENABLED && RF_Connection_TX_ENABLED)
    volatile uint8 RF_Connection_txBuffer[RF_Connection_TX_BUFFER_SIZE];
    volatile uint8 RF_Connection_txBufferRead = 0u;
    uint8 RF_Connection_txBufferWrite = 0u;
#endif /* (RF_Connection_TX_INTERRUPT_ENABLED && RF_Connection_TX_ENABLED) */

#if (RF_Connection_RX_INTERRUPT_ENABLED && (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED))
    uint8 RF_Connection_errorStatus = 0u;
    volatile uint8 RF_Connection_rxBuffer[RF_Connection_RX_BUFFER_SIZE];
    volatile uint16 RF_Connection_rxBufferRead  = 0u;
    volatile uint16 RF_Connection_rxBufferWrite = 0u;
    volatile uint8 RF_Connection_rxBufferLoopDetect = 0u;
    volatile uint8 RF_Connection_rxBufferOverflow   = 0u;
    #if (RF_Connection_RXHW_ADDRESS_ENABLED)
        volatile uint8 RF_Connection_rxAddressMode = RF_Connection_RX_ADDRESS_MODE;
        volatile uint8 RF_Connection_rxAddressDetected = 0u;
    #endif /* (RF_Connection_RXHW_ADDRESS_ENABLED) */
#endif /* (RF_Connection_RX_INTERRUPT_ENABLED && (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)) */


/*******************************************************************************
* Function Name: RF_Connection_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  RF_Connection_Start() sets the initVar variable, calls the
*  RF_Connection_Init() function, and then calls the
*  RF_Connection_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The RF_Connection_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time RF_Connection_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the RF_Connection_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void RF_Connection_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(RF_Connection_initVar == 0u)
    {
        RF_Connection_Init();
        RF_Connection_initVar = 1u;
    }

    RF_Connection_Enable();
}


/*******************************************************************************
* Function Name: RF_Connection_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call RF_Connection_Init() because
*  the RF_Connection_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void RF_Connection_Init(void) 
{
    #if(RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)

        #if (RF_Connection_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(RF_Connection_RX_VECT_NUM, &RF_Connection_RXISR);
            CyIntSetPriority(RF_Connection_RX_VECT_NUM, RF_Connection_RX_PRIOR_NUM);
            RF_Connection_errorStatus = 0u;
        #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */

        #if (RF_Connection_RXHW_ADDRESS_ENABLED)
            RF_Connection_SetRxAddressMode(RF_Connection_RX_ADDRESS_MODE);
            RF_Connection_SetRxAddress1(RF_Connection_RX_HW_ADDRESS1);
            RF_Connection_SetRxAddress2(RF_Connection_RX_HW_ADDRESS2);
        #endif /* End RF_Connection_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        RF_Connection_RXBITCTR_PERIOD_REG = RF_Connection_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        RF_Connection_RXSTATUS_MASK_REG  = RF_Connection_INIT_RX_INTERRUPTS_MASK;
    #endif /* End RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED*/

    #if(RF_Connection_TX_ENABLED)
        #if (RF_Connection_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(RF_Connection_TX_VECT_NUM, &RF_Connection_TXISR);
            CyIntSetPriority(RF_Connection_TX_VECT_NUM, RF_Connection_TX_PRIOR_NUM);
        #endif /* (RF_Connection_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (RF_Connection_TXCLKGEN_DP)
            RF_Connection_TXBITCLKGEN_CTR_REG = RF_Connection_BIT_CENTER;
            RF_Connection_TXBITCLKTX_COMPLETE_REG = ((RF_Connection_NUMBER_OF_DATA_BITS +
                        RF_Connection_NUMBER_OF_START_BIT) * RF_Connection_OVER_SAMPLE_COUNT) - 1u;
        #else
            RF_Connection_TXBITCTR_PERIOD_REG = ((RF_Connection_NUMBER_OF_DATA_BITS +
                        RF_Connection_NUMBER_OF_START_BIT) * RF_Connection_OVER_SAMPLE_8) - 1u;
        #endif /* End RF_Connection_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (RF_Connection_TX_INTERRUPT_ENABLED)
            RF_Connection_TXSTATUS_MASK_REG = RF_Connection_TX_STS_FIFO_EMPTY;
        #else
            RF_Connection_TXSTATUS_MASK_REG = RF_Connection_INIT_TX_INTERRUPTS_MASK;
        #endif /*End RF_Connection_TX_INTERRUPT_ENABLED*/

    #endif /* End RF_Connection_TX_ENABLED */

    #if(RF_Connection_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        RF_Connection_WriteControlRegister( \
            (RF_Connection_ReadControlRegister() & (uint8)~RF_Connection_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(RF_Connection_PARITY_TYPE << RF_Connection_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End RF_Connection_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: RF_Connection_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call RF_Connection_Enable() because the RF_Connection_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  RF_Connection_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void RF_Connection_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        RF_Connection_RXBITCTR_CONTROL_REG |= RF_Connection_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        RF_Connection_RXSTATUS_ACTL_REG  |= RF_Connection_INT_ENABLE;

        #if (RF_Connection_RX_INTERRUPT_ENABLED)
            RF_Connection_EnableRxInt();

            #if (RF_Connection_RXHW_ADDRESS_ENABLED)
                RF_Connection_rxAddressDetected = 0u;
            #endif /* (RF_Connection_RXHW_ADDRESS_ENABLED) */
        #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */
    #endif /* (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED) */

    #if(RF_Connection_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!RF_Connection_TXCLKGEN_DP)
            RF_Connection_TXBITCTR_CONTROL_REG |= RF_Connection_CNTR_ENABLE;
        #endif /* End RF_Connection_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        RF_Connection_TXSTATUS_ACTL_REG |= RF_Connection_INT_ENABLE;
        #if (RF_Connection_TX_INTERRUPT_ENABLED)
            RF_Connection_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            RF_Connection_EnableTxInt();
        #endif /* (RF_Connection_TX_INTERRUPT_ENABLED) */
     #endif /* (RF_Connection_TX_INTERRUPT_ENABLED) */

    #if (RF_Connection_INTERNAL_CLOCK_USED)
        RF_Connection_IntClock_Start();  /* Enable the clock */
    #endif /* (RF_Connection_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: RF_Connection_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void RF_Connection_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)
        RF_Connection_RXBITCTR_CONTROL_REG &= (uint8) ~RF_Connection_CNTR_ENABLE;
    #endif /* (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED) */

    #if (RF_Connection_TX_ENABLED)
        #if(!RF_Connection_TXCLKGEN_DP)
            RF_Connection_TXBITCTR_CONTROL_REG &= (uint8) ~RF_Connection_CNTR_ENABLE;
        #endif /* (!RF_Connection_TXCLKGEN_DP) */
    #endif /* (RF_Connection_TX_ENABLED) */

    #if (RF_Connection_INTERNAL_CLOCK_USED)
        RF_Connection_IntClock_Stop();   /* Disable the clock */
    #endif /* (RF_Connection_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)
        RF_Connection_RXSTATUS_ACTL_REG  &= (uint8) ~RF_Connection_INT_ENABLE;

        #if (RF_Connection_RX_INTERRUPT_ENABLED)
            RF_Connection_DisableRxInt();
        #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */
    #endif /* (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED) */

    #if (RF_Connection_TX_ENABLED)
        RF_Connection_TXSTATUS_ACTL_REG &= (uint8) ~RF_Connection_INT_ENABLE;

        #if (RF_Connection_TX_INTERRUPT_ENABLED)
            RF_Connection_DisableTxInt();
        #endif /* (RF_Connection_TX_INTERRUPT_ENABLED) */
    #endif /* (RF_Connection_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: RF_Connection_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 RF_Connection_ReadControlRegister(void) 
{
    #if (RF_Connection_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(RF_Connection_CONTROL_REG);
    #endif /* (RF_Connection_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: RF_Connection_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  RF_Connection_WriteControlRegister(uint8 control) 
{
    #if (RF_Connection_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       RF_Connection_CONTROL_REG = control;
    #endif /* (RF_Connection_CONTROL_REG_REMOVED) */
}


#if(RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)
    /*******************************************************************************
    * Function Name: RF_Connection_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      RF_Connection_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      RF_Connection_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      RF_Connection_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      RF_Connection_RX_STS_BREAK            Interrupt on break.
    *      RF_Connection_RX_STS_OVERRUN          Interrupt on overrun error.
    *      RF_Connection_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      RF_Connection_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void RF_Connection_SetRxInterruptMode(uint8 intSrc) 
    {
        RF_Connection_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: RF_Connection_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  RF_Connection_rxBuffer - RAM buffer pointer for save received data.
    *  RF_Connection_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  RF_Connection_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  RF_Connection_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 RF_Connection_ReadRxData(void) 
    {
        uint8 rxData;

    #if (RF_Connection_RX_INTERRUPT_ENABLED)

        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        RF_Connection_DisableRxInt();

        locRxBufferRead  = RF_Connection_rxBufferRead;
        locRxBufferWrite = RF_Connection_rxBufferWrite;

        if( (RF_Connection_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = RF_Connection_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= RF_Connection_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            RF_Connection_rxBufferRead = locRxBufferRead;

            if(RF_Connection_rxBufferLoopDetect != 0u)
            {
                RF_Connection_rxBufferLoopDetect = 0u;
                #if ((RF_Connection_RX_INTERRUPT_ENABLED) && (RF_Connection_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( RF_Connection_HD_ENABLED )
                        if((RF_Connection_CONTROL_REG & RF_Connection_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            RF_Connection_RXSTATUS_MASK_REG  |= RF_Connection_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        RF_Connection_RXSTATUS_MASK_REG  |= RF_Connection_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end RF_Connection_HD_ENABLED */
                #endif /* ((RF_Connection_RX_INTERRUPT_ENABLED) && (RF_Connection_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = RF_Connection_RXDATA_REG;
        }

        RF_Connection_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = RF_Connection_RXDATA_REG;

    #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: RF_Connection_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  RF_Connection_RX_STS_FIFO_NOTEMPTY.
    *  RF_Connection_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  RF_Connection_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   RF_Connection_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   RF_Connection_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 RF_Connection_ReadRxStatus(void) 
    {
        uint8 status;

        status = RF_Connection_RXSTATUS_REG & RF_Connection_RX_HW_MASK;

    #if (RF_Connection_RX_INTERRUPT_ENABLED)
        if(RF_Connection_rxBufferOverflow != 0u)
        {
            status |= RF_Connection_RX_STS_SOFT_BUFF_OVER;
            RF_Connection_rxBufferOverflow = 0u;
        }
    #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: RF_Connection_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. RF_Connection_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  RF_Connection_rxBuffer - RAM buffer pointer for save received data.
    *  RF_Connection_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  RF_Connection_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  RF_Connection_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 RF_Connection_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (RF_Connection_RX_INTERRUPT_ENABLED)
        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        RF_Connection_DisableRxInt();

        locRxBufferRead  = RF_Connection_rxBufferRead;
        locRxBufferWrite = RF_Connection_rxBufferWrite;

        if( (RF_Connection_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = RF_Connection_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= RF_Connection_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            RF_Connection_rxBufferRead = locRxBufferRead;

            if(RF_Connection_rxBufferLoopDetect != 0u)
            {
                RF_Connection_rxBufferLoopDetect = 0u;
                #if( (RF_Connection_RX_INTERRUPT_ENABLED) && (RF_Connection_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( RF_Connection_HD_ENABLED )
                        if((RF_Connection_CONTROL_REG & RF_Connection_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            RF_Connection_RXSTATUS_MASK_REG |= RF_Connection_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        RF_Connection_RXSTATUS_MASK_REG |= RF_Connection_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end RF_Connection_HD_ENABLED */
                #endif /* RF_Connection_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = RF_Connection_RXSTATUS_REG;
            if((rxStatus & RF_Connection_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = RF_Connection_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (RF_Connection_RX_STS_BREAK | RF_Connection_RX_STS_PAR_ERROR |
                                RF_Connection_RX_STS_STOP_ERROR | RF_Connection_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        RF_Connection_EnableRxInt();

    #else

        rxStatus =RF_Connection_RXSTATUS_REG;
        if((rxStatus & RF_Connection_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = RF_Connection_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (RF_Connection_RX_STS_BREAK | RF_Connection_RX_STS_PAR_ERROR |
                            RF_Connection_RX_STS_STOP_ERROR | RF_Connection_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: RF_Connection_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 RF_Connection_GetByte(void) 
    {
        
    #if (RF_Connection_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        RF_Connection_DisableRxInt();
        locErrorStatus = (uint16)RF_Connection_errorStatus;
        RF_Connection_errorStatus = 0u;
        RF_Connection_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | RF_Connection_ReadRxData() );
    #else
        return ( ((uint16)RF_Connection_ReadRxStatus() << 8u) | RF_Connection_ReadRxData() );
    #endif /* RF_Connection_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: RF_Connection_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint16: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  RF_Connection_rxBufferWrite - used to calculate left bytes.
    *  RF_Connection_rxBufferRead - used to calculate left bytes.
    *  RF_Connection_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint16 RF_Connection_GetRxBufferSize(void)
                                                            
    {
        uint16 size;

    #if (RF_Connection_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        RF_Connection_DisableRxInt();

        if(RF_Connection_rxBufferRead == RF_Connection_rxBufferWrite)
        {
            if(RF_Connection_rxBufferLoopDetect != 0u)
            {
                size = RF_Connection_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(RF_Connection_rxBufferRead < RF_Connection_rxBufferWrite)
        {
            size = (RF_Connection_rxBufferWrite - RF_Connection_rxBufferRead);
        }
        else
        {
            size = (RF_Connection_RX_BUFFER_SIZE - RF_Connection_rxBufferRead) + RF_Connection_rxBufferWrite;
        }

        RF_Connection_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((RF_Connection_RXSTATUS_REG & RF_Connection_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: RF_Connection_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_rxBufferWrite - cleared to zero.
    *  RF_Connection_rxBufferRead - cleared to zero.
    *  RF_Connection_rxBufferLoopDetect - cleared to zero.
    *  RF_Connection_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void RF_Connection_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        RF_Connection_RXDATA_AUX_CTL_REG |= (uint8)  RF_Connection_RX_FIFO_CLR;
        RF_Connection_RXDATA_AUX_CTL_REG &= (uint8) ~RF_Connection_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (RF_Connection_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        RF_Connection_DisableRxInt();

        RF_Connection_rxBufferRead = 0u;
        RF_Connection_rxBufferWrite = 0u;
        RF_Connection_rxBufferLoopDetect = 0u;
        RF_Connection_rxBufferOverflow = 0u;

        RF_Connection_EnableRxInt();

    #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: RF_Connection_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  RF_Connection__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  RF_Connection__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  RF_Connection__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  RF_Connection__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  RF_Connection__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  RF_Connection_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void RF_Connection_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(RF_Connection_RXHW_ADDRESS_ENABLED)
            #if(RF_Connection_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* RF_Connection_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = RF_Connection_CONTROL_REG & (uint8)~RF_Connection_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << RF_Connection_CTRL_RXADDR_MODE0_SHIFT);
                RF_Connection_CONTROL_REG = tmpCtrl;

                #if(RF_Connection_RX_INTERRUPT_ENABLED && \
                   (RF_Connection_RXBUFFERSIZE > RF_Connection_FIFO_LENGTH) )
                    RF_Connection_rxAddressMode = addressMode;
                    RF_Connection_rxAddressDetected = 0u;
                #endif /* End RF_Connection_RXBUFFERSIZE > RF_Connection_FIFO_LENGTH*/
            #endif /* End RF_Connection_CONTROL_REG_REMOVED */
        #else /* RF_Connection_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End RF_Connection_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: RF_Connection_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void RF_Connection_SetRxAddress1(uint8 address) 
    {
        RF_Connection_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: RF_Connection_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void RF_Connection_SetRxAddress2(uint8 address) 
    {
        RF_Connection_RXADDRESS2_REG = address;
    }

#endif  /* RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED*/


#if( (RF_Connection_TX_ENABLED) || (RF_Connection_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: RF_Connection_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   RF_Connection_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   RF_Connection_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   RF_Connection_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   RF_Connection_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void RF_Connection_SetTxInterruptMode(uint8 intSrc) 
    {
        RF_Connection_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: RF_Connection_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  RF_Connection_txBuffer - RAM buffer pointer for save data for transmission
    *  RF_Connection_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  RF_Connection_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  RF_Connection_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void RF_Connection_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(RF_Connection_initVar != 0u)
        {
        #if (RF_Connection_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            RF_Connection_DisableTxInt();

            if( (RF_Connection_txBufferRead == RF_Connection_txBufferWrite) &&
                ((RF_Connection_TXSTATUS_REG & RF_Connection_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                RF_Connection_TXDATA_REG = txDataByte;
            }
            else
            {
                if(RF_Connection_txBufferWrite >= RF_Connection_TX_BUFFER_SIZE)
                {
                    RF_Connection_txBufferWrite = 0u;
                }

                RF_Connection_txBuffer[RF_Connection_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                RF_Connection_txBufferWrite++;
            }

            RF_Connection_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            RF_Connection_TXDATA_REG = txDataByte;

        #endif /*(RF_Connection_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: RF_Connection_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 RF_Connection_ReadTxStatus(void) 
    {
        return(RF_Connection_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: RF_Connection_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_txBuffer - RAM buffer pointer for save data for transmission
    *  RF_Connection_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  RF_Connection_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  RF_Connection_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void RF_Connection_PutChar(uint8 txDataByte) 
    {
    #if (RF_Connection_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            RF_Connection_DisableTxInt();
        #endif /* (RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = RF_Connection_txBufferWrite;
            locTxBufferRead  = RF_Connection_txBufferRead;

        #if ((RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            RF_Connection_EnableTxInt();
        #endif /* (RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(RF_Connection_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((RF_Connection_TXSTATUS_REG & RF_Connection_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            RF_Connection_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= RF_Connection_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            RF_Connection_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3))
            RF_Connection_DisableTxInt();
        #endif /* (RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3) */

            RF_Connection_txBufferWrite = locTxBufferWrite;

        #if ((RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3))
            RF_Connection_EnableTxInt();
        #endif /* (RF_Connection_TX_BUFFER_SIZE > RF_Connection_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (RF_Connection_TXSTATUS_REG & RF_Connection_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                RF_Connection_SetPendingTxInt();
            }
        }

    #else

        while((RF_Connection_TXSTATUS_REG & RF_Connection_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        RF_Connection_TXDATA_REG = txDataByte;

    #endif /* RF_Connection_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: RF_Connection_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void RF_Connection_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(RF_Connection_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                RF_Connection_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: RF_Connection_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void RF_Connection_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(RF_Connection_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                RF_Connection_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: RF_Connection_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void RF_Connection_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(RF_Connection_initVar != 0u)
        {
            RF_Connection_PutChar(txDataByte);
            RF_Connection_PutChar(0x0Du);
            RF_Connection_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: RF_Connection_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  RF_Connection_txBufferWrite - used to calculate left space.
    *  RF_Connection_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 RF_Connection_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (RF_Connection_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        RF_Connection_DisableTxInt();

        if(RF_Connection_txBufferRead == RF_Connection_txBufferWrite)
        {
            size = 0u;
        }
        else if(RF_Connection_txBufferRead < RF_Connection_txBufferWrite)
        {
            size = (RF_Connection_txBufferWrite - RF_Connection_txBufferRead);
        }
        else
        {
            size = (RF_Connection_TX_BUFFER_SIZE - RF_Connection_txBufferRead) +
                    RF_Connection_txBufferWrite;
        }

        RF_Connection_EnableTxInt();

    #else

        size = RF_Connection_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & RF_Connection_TX_STS_FIFO_FULL) != 0u)
        {
            size = RF_Connection_FIFO_LENGTH;
        }
        else if((size & RF_Connection_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (RF_Connection_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: RF_Connection_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_txBufferWrite - cleared to zero.
    *  RF_Connection_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void RF_Connection_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        RF_Connection_TXDATA_AUX_CTL_REG |= (uint8)  RF_Connection_TX_FIFO_CLR;
        RF_Connection_TXDATA_AUX_CTL_REG &= (uint8) ~RF_Connection_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (RF_Connection_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        RF_Connection_DisableTxInt();

        RF_Connection_txBufferRead = 0u;
        RF_Connection_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        RF_Connection_EnableTxInt();

    #endif /* (RF_Connection_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: RF_Connection_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   RF_Connection_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   RF_Connection_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   RF_Connection_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   RF_Connection_SEND_WAIT_REINIT - Performs both options: 
    *      RF_Connection_SEND_BREAK and RF_Connection_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with RF_Connection_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The RF_Connection_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void RF_Connection_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(RF_Connection_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(RF_Connection_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == RF_Connection_SEND_BREAK) ||
                (retMode == RF_Connection_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() |
                                                      RF_Connection_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                RF_Connection_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = RF_Connection_TXSTATUS_REG;
                }
                while((tmpStat & RF_Connection_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == RF_Connection_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == RF_Connection_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = RF_Connection_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & RF_Connection_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == RF_Connection_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == RF_Connection_REINIT) ||
                (retMode == RF_Connection_SEND_WAIT_REINIT) )
            {
                RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() &
                                              (uint8)~RF_Connection_CTRL_HD_SEND_BREAK);
            }

        #else /* RF_Connection_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == RF_Connection_SEND_BREAK) ||
                (retMode == RF_Connection_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (RF_Connection_PARITY_TYPE != RF_Connection__B_UART__NONE_REVB) || \
                                    (RF_Connection_PARITY_TYPE_SW != 0u) )
                    RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() |
                                                          RF_Connection_CTRL_HD_SEND_BREAK);
                #endif /* End RF_Connection_PARITY_TYPE != RF_Connection__B_UART__NONE_REVB  */

                #if(RF_Connection_TXCLKGEN_DP)
                    txPeriod = RF_Connection_TXBITCLKTX_COMPLETE_REG;
                    RF_Connection_TXBITCLKTX_COMPLETE_REG = RF_Connection_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = RF_Connection_TXBITCTR_PERIOD_REG;
                    RF_Connection_TXBITCTR_PERIOD_REG = RF_Connection_TXBITCTR_BREAKBITS8X;
                #endif /* End RF_Connection_TXCLKGEN_DP */

                /* Send zeros */
                RF_Connection_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = RF_Connection_TXSTATUS_REG;
                }
                while((tmpStat & RF_Connection_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == RF_Connection_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == RF_Connection_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = RF_Connection_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & RF_Connection_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == RF_Connection_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == RF_Connection_REINIT) ||
                (retMode == RF_Connection_SEND_WAIT_REINIT) )
            {

            #if(RF_Connection_TXCLKGEN_DP)
                RF_Connection_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                RF_Connection_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End RF_Connection_TXCLKGEN_DP */

            #if( (RF_Connection_PARITY_TYPE != RF_Connection__B_UART__NONE_REVB) || \
                 (RF_Connection_PARITY_TYPE_SW != 0u) )
                RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() &
                                                      (uint8) ~RF_Connection_CTRL_HD_SEND_BREAK);
            #endif /* End RF_Connection_PARITY_TYPE != NONE */
            }
        #endif    /* End RF_Connection_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: RF_Connection_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       RF_Connection_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       RF_Connection_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears RF_Connection_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void RF_Connection_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( RF_Connection_CONTROL_REG_REMOVED == 0u )
            RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() |
                                                  RF_Connection_CTRL_MARK);
        #endif /* End RF_Connection_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( RF_Connection_CONTROL_REG_REMOVED == 0u )
            RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() &
                                                  (uint8) ~RF_Connection_CTRL_MARK);
        #endif /* End RF_Connection_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndRF_Connection_TX_ENABLED */

#if(RF_Connection_HD_ENABLED)


    /*******************************************************************************
    * Function Name: RF_Connection_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void RF_Connection_LoadRxConfig(void) 
    {
        RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() &
                                                (uint8)~RF_Connection_CTRL_HD_SEND);
        RF_Connection_RXBITCTR_PERIOD_REG = RF_Connection_HD_RXBITCTR_INIT;

    #if (RF_Connection_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        RF_Connection_SetRxInterruptMode(RF_Connection_INIT_RX_INTERRUPTS_MASK);
    #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: RF_Connection_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void RF_Connection_LoadTxConfig(void) 
    {
    #if (RF_Connection_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        RF_Connection_SetRxInterruptMode(0u);
    #endif /* (RF_Connection_RX_INTERRUPT_ENABLED) */

        RF_Connection_WriteControlRegister(RF_Connection_ReadControlRegister() | RF_Connection_CTRL_HD_SEND);
        RF_Connection_RXBITCTR_PERIOD_REG = RF_Connection_HD_TXBITCTR_INIT;
    }

#endif  /* RF_Connection_HD_ENABLED */


/* [] END OF FILE */
