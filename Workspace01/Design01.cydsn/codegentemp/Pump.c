/*******************************************************************************
* File Name: Pump.c
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

#include "Pump.h"
#if (Pump_INTERNAL_CLOCK_USED)
    #include "Pump_IntClock.h"
#endif /* End Pump_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 Pump_initVar = 0u;

#if (Pump_TX_INTERRUPT_ENABLED && Pump_TX_ENABLED)
    volatile uint8 Pump_txBuffer[Pump_TX_BUFFER_SIZE];
    volatile uint8 Pump_txBufferRead = 0u;
    uint8 Pump_txBufferWrite = 0u;
#endif /* (Pump_TX_INTERRUPT_ENABLED && Pump_TX_ENABLED) */

#if (Pump_RX_INTERRUPT_ENABLED && (Pump_RX_ENABLED || Pump_HD_ENABLED))
    uint8 Pump_errorStatus = 0u;
    volatile uint8 Pump_rxBuffer[Pump_RX_BUFFER_SIZE];
    volatile uint16 Pump_rxBufferRead  = 0u;
    volatile uint16 Pump_rxBufferWrite = 0u;
    volatile uint8 Pump_rxBufferLoopDetect = 0u;
    volatile uint8 Pump_rxBufferOverflow   = 0u;
    #if (Pump_RXHW_ADDRESS_ENABLED)
        volatile uint8 Pump_rxAddressMode = Pump_RX_ADDRESS_MODE;
        volatile uint8 Pump_rxAddressDetected = 0u;
    #endif /* (Pump_RXHW_ADDRESS_ENABLED) */
#endif /* (Pump_RX_INTERRUPT_ENABLED && (Pump_RX_ENABLED || Pump_HD_ENABLED)) */


/*******************************************************************************
* Function Name: Pump_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  Pump_Start() sets the initVar variable, calls the
*  Pump_Init() function, and then calls the
*  Pump_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The Pump_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time Pump_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the Pump_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void Pump_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(Pump_initVar == 0u)
    {
        Pump_Init();
        Pump_initVar = 1u;
    }

    Pump_Enable();
}


/*******************************************************************************
* Function Name: Pump_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call Pump_Init() because
*  the Pump_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void Pump_Init(void) 
{
    #if(Pump_RX_ENABLED || Pump_HD_ENABLED)

        #if (Pump_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(Pump_RX_VECT_NUM, &Pump_RXISR);
            CyIntSetPriority(Pump_RX_VECT_NUM, Pump_RX_PRIOR_NUM);
            Pump_errorStatus = 0u;
        #endif /* (Pump_RX_INTERRUPT_ENABLED) */

        #if (Pump_RXHW_ADDRESS_ENABLED)
            Pump_SetRxAddressMode(Pump_RX_ADDRESS_MODE);
            Pump_SetRxAddress1(Pump_RX_HW_ADDRESS1);
            Pump_SetRxAddress2(Pump_RX_HW_ADDRESS2);
        #endif /* End Pump_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        Pump_RXBITCTR_PERIOD_REG = Pump_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        Pump_RXSTATUS_MASK_REG  = Pump_INIT_RX_INTERRUPTS_MASK;
    #endif /* End Pump_RX_ENABLED || Pump_HD_ENABLED*/

    #if(Pump_TX_ENABLED)
        #if (Pump_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(Pump_TX_VECT_NUM, &Pump_TXISR);
            CyIntSetPriority(Pump_TX_VECT_NUM, Pump_TX_PRIOR_NUM);
        #endif /* (Pump_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (Pump_TXCLKGEN_DP)
            Pump_TXBITCLKGEN_CTR_REG = Pump_BIT_CENTER;
            Pump_TXBITCLKTX_COMPLETE_REG = ((Pump_NUMBER_OF_DATA_BITS +
                        Pump_NUMBER_OF_START_BIT) * Pump_OVER_SAMPLE_COUNT) - 1u;
        #else
            Pump_TXBITCTR_PERIOD_REG = ((Pump_NUMBER_OF_DATA_BITS +
                        Pump_NUMBER_OF_START_BIT) * Pump_OVER_SAMPLE_8) - 1u;
        #endif /* End Pump_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (Pump_TX_INTERRUPT_ENABLED)
            Pump_TXSTATUS_MASK_REG = Pump_TX_STS_FIFO_EMPTY;
        #else
            Pump_TXSTATUS_MASK_REG = Pump_INIT_TX_INTERRUPTS_MASK;
        #endif /*End Pump_TX_INTERRUPT_ENABLED*/

    #endif /* End Pump_TX_ENABLED */

    #if(Pump_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        Pump_WriteControlRegister( \
            (Pump_ReadControlRegister() & (uint8)~Pump_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(Pump_PARITY_TYPE << Pump_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End Pump_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: Pump_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call Pump_Enable() because the Pump_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  Pump_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void Pump_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (Pump_RX_ENABLED || Pump_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        Pump_RXBITCTR_CONTROL_REG |= Pump_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        Pump_RXSTATUS_ACTL_REG  |= Pump_INT_ENABLE;

        #if (Pump_RX_INTERRUPT_ENABLED)
            Pump_EnableRxInt();

            #if (Pump_RXHW_ADDRESS_ENABLED)
                Pump_rxAddressDetected = 0u;
            #endif /* (Pump_RXHW_ADDRESS_ENABLED) */
        #endif /* (Pump_RX_INTERRUPT_ENABLED) */
    #endif /* (Pump_RX_ENABLED || Pump_HD_ENABLED) */

    #if(Pump_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!Pump_TXCLKGEN_DP)
            Pump_TXBITCTR_CONTROL_REG |= Pump_CNTR_ENABLE;
        #endif /* End Pump_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        Pump_TXSTATUS_ACTL_REG |= Pump_INT_ENABLE;
        #if (Pump_TX_INTERRUPT_ENABLED)
            Pump_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            Pump_EnableTxInt();
        #endif /* (Pump_TX_INTERRUPT_ENABLED) */
     #endif /* (Pump_TX_INTERRUPT_ENABLED) */

    #if (Pump_INTERNAL_CLOCK_USED)
        Pump_IntClock_Start();  /* Enable the clock */
    #endif /* (Pump_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Pump_Stop
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
void Pump_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (Pump_RX_ENABLED || Pump_HD_ENABLED)
        Pump_RXBITCTR_CONTROL_REG &= (uint8) ~Pump_CNTR_ENABLE;
    #endif /* (Pump_RX_ENABLED || Pump_HD_ENABLED) */

    #if (Pump_TX_ENABLED)
        #if(!Pump_TXCLKGEN_DP)
            Pump_TXBITCTR_CONTROL_REG &= (uint8) ~Pump_CNTR_ENABLE;
        #endif /* (!Pump_TXCLKGEN_DP) */
    #endif /* (Pump_TX_ENABLED) */

    #if (Pump_INTERNAL_CLOCK_USED)
        Pump_IntClock_Stop();   /* Disable the clock */
    #endif /* (Pump_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (Pump_RX_ENABLED || Pump_HD_ENABLED)
        Pump_RXSTATUS_ACTL_REG  &= (uint8) ~Pump_INT_ENABLE;

        #if (Pump_RX_INTERRUPT_ENABLED)
            Pump_DisableRxInt();
        #endif /* (Pump_RX_INTERRUPT_ENABLED) */
    #endif /* (Pump_RX_ENABLED || Pump_HD_ENABLED) */

    #if (Pump_TX_ENABLED)
        Pump_TXSTATUS_ACTL_REG &= (uint8) ~Pump_INT_ENABLE;

        #if (Pump_TX_INTERRUPT_ENABLED)
            Pump_DisableTxInt();
        #endif /* (Pump_TX_INTERRUPT_ENABLED) */
    #endif /* (Pump_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: Pump_ReadControlRegister
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
uint8 Pump_ReadControlRegister(void) 
{
    #if (Pump_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(Pump_CONTROL_REG);
    #endif /* (Pump_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: Pump_WriteControlRegister
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
void  Pump_WriteControlRegister(uint8 control) 
{
    #if (Pump_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       Pump_CONTROL_REG = control;
    #endif /* (Pump_CONTROL_REG_REMOVED) */
}


#if(Pump_RX_ENABLED || Pump_HD_ENABLED)
    /*******************************************************************************
    * Function Name: Pump_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      Pump_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      Pump_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      Pump_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      Pump_RX_STS_BREAK            Interrupt on break.
    *      Pump_RX_STS_OVERRUN          Interrupt on overrun error.
    *      Pump_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      Pump_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Pump_SetRxInterruptMode(uint8 intSrc) 
    {
        Pump_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: Pump_ReadRxData
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
    *  Pump_rxBuffer - RAM buffer pointer for save received data.
    *  Pump_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Pump_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Pump_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Pump_ReadRxData(void) 
    {
        uint8 rxData;

    #if (Pump_RX_INTERRUPT_ENABLED)

        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Pump_DisableRxInt();

        locRxBufferRead  = Pump_rxBufferRead;
        locRxBufferWrite = Pump_rxBufferWrite;

        if( (Pump_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Pump_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= Pump_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Pump_rxBufferRead = locRxBufferRead;

            if(Pump_rxBufferLoopDetect != 0u)
            {
                Pump_rxBufferLoopDetect = 0u;
                #if ((Pump_RX_INTERRUPT_ENABLED) && (Pump_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Pump_HD_ENABLED )
                        if((Pump_CONTROL_REG & Pump_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Pump_RXSTATUS_MASK_REG  |= Pump_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Pump_RXSTATUS_MASK_REG  |= Pump_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Pump_HD_ENABLED */
                #endif /* ((Pump_RX_INTERRUPT_ENABLED) && (Pump_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = Pump_RXDATA_REG;
        }

        Pump_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = Pump_RXDATA_REG;

    #endif /* (Pump_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Pump_ReadRxStatus
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
    *  Pump_RX_STS_FIFO_NOTEMPTY.
    *  Pump_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  Pump_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   Pump_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   Pump_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 Pump_ReadRxStatus(void) 
    {
        uint8 status;

        status = Pump_RXSTATUS_REG & Pump_RX_HW_MASK;

    #if (Pump_RX_INTERRUPT_ENABLED)
        if(Pump_rxBufferOverflow != 0u)
        {
            status |= Pump_RX_STS_SOFT_BUFF_OVER;
            Pump_rxBufferOverflow = 0u;
        }
    #endif /* (Pump_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: Pump_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. Pump_GetChar() is
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
    *  Pump_rxBuffer - RAM buffer pointer for save received data.
    *  Pump_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  Pump_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  Pump_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 Pump_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (Pump_RX_INTERRUPT_ENABLED)
        uint16 locRxBufferRead;
        uint16 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        Pump_DisableRxInt();

        locRxBufferRead  = Pump_rxBufferRead;
        locRxBufferWrite = Pump_rxBufferWrite;

        if( (Pump_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = Pump_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= Pump_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            Pump_rxBufferRead = locRxBufferRead;

            if(Pump_rxBufferLoopDetect != 0u)
            {
                Pump_rxBufferLoopDetect = 0u;
                #if( (Pump_RX_INTERRUPT_ENABLED) && (Pump_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( Pump_HD_ENABLED )
                        if((Pump_CONTROL_REG & Pump_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            Pump_RXSTATUS_MASK_REG |= Pump_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        Pump_RXSTATUS_MASK_REG |= Pump_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end Pump_HD_ENABLED */
                #endif /* Pump_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = Pump_RXSTATUS_REG;
            if((rxStatus & Pump_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = Pump_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (Pump_RX_STS_BREAK | Pump_RX_STS_PAR_ERROR |
                                Pump_RX_STS_STOP_ERROR | Pump_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        Pump_EnableRxInt();

    #else

        rxStatus =Pump_RXSTATUS_REG;
        if((rxStatus & Pump_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = Pump_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (Pump_RX_STS_BREAK | Pump_RX_STS_PAR_ERROR |
                            Pump_RX_STS_STOP_ERROR | Pump_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (Pump_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: Pump_GetByte
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
    uint16 Pump_GetByte(void) 
    {
        
    #if (Pump_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        Pump_DisableRxInt();
        locErrorStatus = (uint16)Pump_errorStatus;
        Pump_errorStatus = 0u;
        Pump_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | Pump_ReadRxData() );
    #else
        return ( ((uint16)Pump_ReadRxStatus() << 8u) | Pump_ReadRxData() );
    #endif /* Pump_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: Pump_GetRxBufferSize
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
    *  Pump_rxBufferWrite - used to calculate left bytes.
    *  Pump_rxBufferRead - used to calculate left bytes.
    *  Pump_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint16 Pump_GetRxBufferSize(void)
                                                            
    {
        uint16 size;

    #if (Pump_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        Pump_DisableRxInt();

        if(Pump_rxBufferRead == Pump_rxBufferWrite)
        {
            if(Pump_rxBufferLoopDetect != 0u)
            {
                size = Pump_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(Pump_rxBufferRead < Pump_rxBufferWrite)
        {
            size = (Pump_rxBufferWrite - Pump_rxBufferRead);
        }
        else
        {
            size = (Pump_RX_BUFFER_SIZE - Pump_rxBufferRead) + Pump_rxBufferWrite;
        }

        Pump_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((Pump_RXSTATUS_REG & Pump_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (Pump_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: Pump_ClearRxBuffer
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
    *  Pump_rxBufferWrite - cleared to zero.
    *  Pump_rxBufferRead - cleared to zero.
    *  Pump_rxBufferLoopDetect - cleared to zero.
    *  Pump_rxBufferOverflow - cleared to zero.
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
    void Pump_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        Pump_RXDATA_AUX_CTL_REG |= (uint8)  Pump_RX_FIFO_CLR;
        Pump_RXDATA_AUX_CTL_REG &= (uint8) ~Pump_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Pump_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Pump_DisableRxInt();

        Pump_rxBufferRead = 0u;
        Pump_rxBufferWrite = 0u;
        Pump_rxBufferLoopDetect = 0u;
        Pump_rxBufferOverflow = 0u;

        Pump_EnableRxInt();

    #endif /* (Pump_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: Pump_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  Pump__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  Pump__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  Pump__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  Pump__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  Pump__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Pump_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  Pump_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void Pump_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(Pump_RXHW_ADDRESS_ENABLED)
            #if(Pump_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* Pump_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = Pump_CONTROL_REG & (uint8)~Pump_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << Pump_CTRL_RXADDR_MODE0_SHIFT);
                Pump_CONTROL_REG = tmpCtrl;

                #if(Pump_RX_INTERRUPT_ENABLED && \
                   (Pump_RXBUFFERSIZE > Pump_FIFO_LENGTH) )
                    Pump_rxAddressMode = addressMode;
                    Pump_rxAddressDetected = 0u;
                #endif /* End Pump_RXBUFFERSIZE > Pump_FIFO_LENGTH*/
            #endif /* End Pump_CONTROL_REG_REMOVED */
        #else /* Pump_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End Pump_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Pump_SetRxAddress1
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
    void Pump_SetRxAddress1(uint8 address) 
    {
        Pump_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: Pump_SetRxAddress2
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
    void Pump_SetRxAddress2(uint8 address) 
    {
        Pump_RXADDRESS2_REG = address;
    }

#endif  /* Pump_RX_ENABLED || Pump_HD_ENABLED*/


#if( (Pump_TX_ENABLED) || (Pump_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: Pump_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   Pump_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   Pump_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   Pump_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   Pump_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void Pump_SetTxInterruptMode(uint8 intSrc) 
    {
        Pump_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: Pump_WriteTxData
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
    *  Pump_txBuffer - RAM buffer pointer for save data for transmission
    *  Pump_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  Pump_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  Pump_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Pump_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(Pump_initVar != 0u)
        {
        #if (Pump_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            Pump_DisableTxInt();

            if( (Pump_txBufferRead == Pump_txBufferWrite) &&
                ((Pump_TXSTATUS_REG & Pump_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                Pump_TXDATA_REG = txDataByte;
            }
            else
            {
                if(Pump_txBufferWrite >= Pump_TX_BUFFER_SIZE)
                {
                    Pump_txBufferWrite = 0u;
                }

                Pump_txBuffer[Pump_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                Pump_txBufferWrite++;
            }

            Pump_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            Pump_TXDATA_REG = txDataByte;

        #endif /*(Pump_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: Pump_ReadTxStatus
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
    uint8 Pump_ReadTxStatus(void) 
    {
        return(Pump_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: Pump_PutChar
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
    *  Pump_txBuffer - RAM buffer pointer for save data for transmission
    *  Pump_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  Pump_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  Pump_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void Pump_PutChar(uint8 txDataByte) 
    {
    #if (Pump_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            Pump_DisableTxInt();
        #endif /* (Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = Pump_txBufferWrite;
            locTxBufferRead  = Pump_txBufferRead;

        #if ((Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            Pump_EnableTxInt();
        #endif /* (Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(Pump_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((Pump_TXSTATUS_REG & Pump_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            Pump_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= Pump_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            Pump_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3))
            Pump_DisableTxInt();
        #endif /* (Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3) */

            Pump_txBufferWrite = locTxBufferWrite;

        #if ((Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3))
            Pump_EnableTxInt();
        #endif /* (Pump_TX_BUFFER_SIZE > Pump_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (Pump_TXSTATUS_REG & Pump_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                Pump_SetPendingTxInt();
            }
        }

    #else

        while((Pump_TXSTATUS_REG & Pump_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        Pump_TXDATA_REG = txDataByte;

    #endif /* Pump_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: Pump_PutString
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
    *  Pump_initVar - checked to identify that the component has been
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
    void Pump_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Pump_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                Pump_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Pump_PutArray
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
    *  Pump_initVar - checked to identify that the component has been
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
    void Pump_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(Pump_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                Pump_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: Pump_PutCRLF
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
    *  Pump_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void Pump_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(Pump_initVar != 0u)
        {
            Pump_PutChar(txDataByte);
            Pump_PutChar(0x0Du);
            Pump_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: Pump_GetTxBufferSize
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
    *  Pump_txBufferWrite - used to calculate left space.
    *  Pump_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 Pump_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (Pump_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Pump_DisableTxInt();

        if(Pump_txBufferRead == Pump_txBufferWrite)
        {
            size = 0u;
        }
        else if(Pump_txBufferRead < Pump_txBufferWrite)
        {
            size = (Pump_txBufferWrite - Pump_txBufferRead);
        }
        else
        {
            size = (Pump_TX_BUFFER_SIZE - Pump_txBufferRead) +
                    Pump_txBufferWrite;
        }

        Pump_EnableTxInt();

    #else

        size = Pump_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & Pump_TX_STS_FIFO_FULL) != 0u)
        {
            size = Pump_FIFO_LENGTH;
        }
        else if((size & Pump_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (Pump_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: Pump_ClearTxBuffer
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
    *  Pump_txBufferWrite - cleared to zero.
    *  Pump_txBufferRead - cleared to zero.
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
    void Pump_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        Pump_TXDATA_AUX_CTL_REG |= (uint8)  Pump_TX_FIFO_CLR;
        Pump_TXDATA_AUX_CTL_REG &= (uint8) ~Pump_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (Pump_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        Pump_DisableTxInt();

        Pump_txBufferRead = 0u;
        Pump_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        Pump_EnableTxInt();

    #endif /* (Pump_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Pump_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   Pump_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   Pump_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   Pump_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   Pump_SEND_WAIT_REINIT - Performs both options: 
    *      Pump_SEND_BREAK and Pump_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  Pump_initVar - checked to identify that the component has been
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
    *     When interrupt appear with Pump_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The Pump_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void Pump_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(Pump_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(Pump_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == Pump_SEND_BREAK) ||
                (retMode == Pump_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                Pump_WriteControlRegister(Pump_ReadControlRegister() |
                                                      Pump_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                Pump_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Pump_TXSTATUS_REG;
                }
                while((tmpStat & Pump_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Pump_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Pump_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Pump_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Pump_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Pump_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Pump_REINIT) ||
                (retMode == Pump_SEND_WAIT_REINIT) )
            {
                Pump_WriteControlRegister(Pump_ReadControlRegister() &
                                              (uint8)~Pump_CTRL_HD_SEND_BREAK);
            }

        #else /* Pump_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == Pump_SEND_BREAK) ||
                (retMode == Pump_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (Pump_PARITY_TYPE != Pump__B_UART__NONE_REVB) || \
                                    (Pump_PARITY_TYPE_SW != 0u) )
                    Pump_WriteControlRegister(Pump_ReadControlRegister() |
                                                          Pump_CTRL_HD_SEND_BREAK);
                #endif /* End Pump_PARITY_TYPE != Pump__B_UART__NONE_REVB  */

                #if(Pump_TXCLKGEN_DP)
                    txPeriod = Pump_TXBITCLKTX_COMPLETE_REG;
                    Pump_TXBITCLKTX_COMPLETE_REG = Pump_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = Pump_TXBITCTR_PERIOD_REG;
                    Pump_TXBITCTR_PERIOD_REG = Pump_TXBITCTR_BREAKBITS8X;
                #endif /* End Pump_TXCLKGEN_DP */

                /* Send zeros */
                Pump_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = Pump_TXSTATUS_REG;
                }
                while((tmpStat & Pump_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == Pump_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Pump_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = Pump_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & Pump_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == Pump_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == Pump_REINIT) ||
                (retMode == Pump_SEND_WAIT_REINIT) )
            {

            #if(Pump_TXCLKGEN_DP)
                Pump_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                Pump_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End Pump_TXCLKGEN_DP */

            #if( (Pump_PARITY_TYPE != Pump__B_UART__NONE_REVB) || \
                 (Pump_PARITY_TYPE_SW != 0u) )
                Pump_WriteControlRegister(Pump_ReadControlRegister() &
                                                      (uint8) ~Pump_CTRL_HD_SEND_BREAK);
            #endif /* End Pump_PARITY_TYPE != NONE */
            }
        #endif    /* End Pump_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: Pump_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       Pump_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       Pump_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears Pump_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void Pump_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( Pump_CONTROL_REG_REMOVED == 0u )
            Pump_WriteControlRegister(Pump_ReadControlRegister() |
                                                  Pump_CTRL_MARK);
        #endif /* End Pump_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( Pump_CONTROL_REG_REMOVED == 0u )
            Pump_WriteControlRegister(Pump_ReadControlRegister() &
                                                  (uint8) ~Pump_CTRL_MARK);
        #endif /* End Pump_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndPump_TX_ENABLED */

#if(Pump_HD_ENABLED)


    /*******************************************************************************
    * Function Name: Pump_LoadRxConfig
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
    void Pump_LoadRxConfig(void) 
    {
        Pump_WriteControlRegister(Pump_ReadControlRegister() &
                                                (uint8)~Pump_CTRL_HD_SEND);
        Pump_RXBITCTR_PERIOD_REG = Pump_HD_RXBITCTR_INIT;

    #if (Pump_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        Pump_SetRxInterruptMode(Pump_INIT_RX_INTERRUPTS_MASK);
    #endif /* (Pump_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: Pump_LoadTxConfig
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
    void Pump_LoadTxConfig(void) 
    {
    #if (Pump_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        Pump_SetRxInterruptMode(0u);
    #endif /* (Pump_RX_INTERRUPT_ENABLED) */

        Pump_WriteControlRegister(Pump_ReadControlRegister() | Pump_CTRL_HD_SEND);
        Pump_RXBITCTR_PERIOD_REG = Pump_HD_TXBITCTR_INIT;
    }

#endif  /* Pump_HD_ENABLED */


/* [] END OF FILE */
