/*******************************************************************************
* File Name: RF_ConnectionINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "RF_Connection.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (RF_Connection_RX_INTERRUPT_ENABLED && (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED))
    /*******************************************************************************
    * Function Name: RF_Connection_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_rxBuffer - RAM buffer pointer for save received data.
    *  RF_Connection_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  RF_Connection_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  RF_Connection_rxBufferOverflow - software overflow flag. Set to one
    *     when RF_Connection_rxBufferWrite index overtakes
    *     RF_Connection_rxBufferRead index.
    *  RF_Connection_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when RF_Connection_rxBufferWrite is equal to
    *    RF_Connection_rxBufferRead
    *  RF_Connection_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  RF_Connection_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(RF_Connection_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef RF_Connection_RXISR_ENTRY_CALLBACK
        RF_Connection_RXISR_EntryCallback();
    #endif /* RF_Connection_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START RF_Connection_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = RF_Connection_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in RF_Connection_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (RF_Connection_RX_STS_BREAK | 
                            RF_Connection_RX_STS_PAR_ERROR |
                            RF_Connection_RX_STS_STOP_ERROR | 
                            RF_Connection_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                RF_Connection_errorStatus |= readStatus & ( RF_Connection_RX_STS_BREAK | 
                                                            RF_Connection_RX_STS_PAR_ERROR | 
                                                            RF_Connection_RX_STS_STOP_ERROR | 
                                                            RF_Connection_RX_STS_OVERRUN);
                /* `#START RF_Connection_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef RF_Connection_RXISR_ERROR_CALLBACK
                RF_Connection_RXISR_ERROR_Callback();
            #endif /* RF_Connection_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & RF_Connection_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = RF_Connection_RXDATA_REG;
            #if (RF_Connection_RXHW_ADDRESS_ENABLED)
                if(RF_Connection_rxAddressMode == (uint8)RF_Connection__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & RF_Connection_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & RF_Connection_RX_STS_ADDR_MATCH) != 0u)
                        {
                            RF_Connection_rxAddressDetected = 1u;
                        }
                        else
                        {
                            RF_Connection_rxAddressDetected = 0u;
                        }
                    }
                    if(RF_Connection_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        RF_Connection_rxBuffer[RF_Connection_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    RF_Connection_rxBuffer[RF_Connection_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                RF_Connection_rxBuffer[RF_Connection_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (RF_Connection_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(RF_Connection_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        RF_Connection_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    RF_Connection_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(RF_Connection_rxBufferWrite >= RF_Connection_RX_BUFFER_SIZE)
                    {
                        RF_Connection_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(RF_Connection_rxBufferWrite == RF_Connection_rxBufferRead)
                    {
                        RF_Connection_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (RF_Connection_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            RF_Connection_RXSTATUS_MASK_REG  &= (uint8)~RF_Connection_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(RF_Connection_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (RF_Connection_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & RF_Connection_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START RF_Connection_RXISR_END` */

        /* `#END` */

    #ifdef RF_Connection_RXISR_EXIT_CALLBACK
        RF_Connection_RXISR_ExitCallback();
    #endif /* RF_Connection_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (RF_Connection_RX_INTERRUPT_ENABLED && (RF_Connection_RX_ENABLED || RF_Connection_HD_ENABLED)) */


#if (RF_Connection_TX_INTERRUPT_ENABLED && RF_Connection_TX_ENABLED)
    /*******************************************************************************
    * Function Name: RF_Connection_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  RF_Connection_txBuffer - RAM buffer pointer for transmit data from.
    *  RF_Connection_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  RF_Connection_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(RF_Connection_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef RF_Connection_TXISR_ENTRY_CALLBACK
        RF_Connection_TXISR_EntryCallback();
    #endif /* RF_Connection_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START RF_Connection_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((RF_Connection_txBufferRead != RF_Connection_txBufferWrite) &&
             ((RF_Connection_TXSTATUS_REG & RF_Connection_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(RF_Connection_txBufferRead >= RF_Connection_TX_BUFFER_SIZE)
            {
                RF_Connection_txBufferRead = 0u;
            }

            RF_Connection_TXDATA_REG = RF_Connection_txBuffer[RF_Connection_txBufferRead];

            /* Set next pointer */
            RF_Connection_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START RF_Connection_TXISR_END` */

        /* `#END` */

    #ifdef RF_Connection_TXISR_EXIT_CALLBACK
        RF_Connection_TXISR_ExitCallback();
    #endif /* RF_Connection_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (RF_Connection_TX_INTERRUPT_ENABLED && RF_Connection_TX_ENABLED) */


/* [] END OF FILE */
