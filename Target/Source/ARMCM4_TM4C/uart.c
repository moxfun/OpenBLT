/************************************************************************************//**
* \file         Source\ARMCM4_TM4C\uart.c
* \brief        Bootloader UART communication interface source file.
* \ingroup      Target_ARMCM4_TM4C
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2014  by Feaser    http://www.feaser.com    All rights reserved
*
*----------------------------------------------------------------------------------------
*                            L I C E N S E
*----------------------------------------------------------------------------------------
* This file is part of OpenBLT. OpenBLT is free software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published by the Free
* Software Foundation, either version 3 of the License, or (at your option) any later
* version.
*
* OpenBLT is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
* PURPOSE. See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with OpenBLT.
* If not, see <http://www.gnu.org/licenses/>.
*
* A special exception to the GPL is included to allow you to distribute a combined work 
* that includes OpenBLT without being obliged to provide the source code for any 
* proprietary components. The exception text is included at the bottom of the license
* file <license.html>.
* 
* \endinternal
****************************************************************************************/

/****************************************************************************************
* Include files
****************************************************************************************/
#include "boot.h"                                /* bootloader generic header          */
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#if (BOOT_COM_UART_ENABLE > 0)
#include "driverlib/sysctl.h"
#include "driverlib/uartlib.h"
#endif


#if (BOOT_COM_UART_ENABLE > 0)
/****************************************************************************************
* Function prototypes
****************************************************************************************/
static blt_bool UartReceiveByte(blt_int8u *data);
static blt_bool UartTransmitByte(blt_int8u data);


/************************************************************************************//**
** \brief     Initializes the UART communication interface.
** \return    none.
**
****************************************************************************************/
void UartInit(void)
{
  /* the current implementation supports UART0. throw an assertion error in case a 
   * different UART channel is configured.  
   */
  ASSERT_CT(BOOT_COM_UART_CHANNEL_INDEX == 0); 
  /* enable the UART0 peripheral */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  /* configure the UART0 baudrate and communication parameters */
  UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), BOOT_COM_UART_BAUDRATE,
                      (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | 
                      UART_CONFIG_PAR_NONE));
} /*** end of UartInit ***/


/************************************************************************************//**
** \brief     Transmits a packet formatted for the communication interface.
** \param     data Pointer to byte array with data that it to be transmitted.
** \param     len  Number of bytes that are to be transmitted.
** \return    none.
**
****************************************************************************************/
void UartTransmitPacket(blt_int8u *data, blt_int8u len)
{
  blt_int16u data_index;
  blt_bool result;

  /* verify validity of the len-paramenter */
  ASSERT_RT(len <= BOOT_COM_UART_TX_MAX_DATA);  

  /* first transmit the length of the packet */  
  result = UartTransmitByte(len);
  ASSERT_RT(result == BLT_TRUE);  
  
  /* transmit all the packet bytes one-by-one */
  for (data_index = 0; data_index < len; data_index++)
  {
    /* keep the watchdog happy */
    CopService();
    /* write byte */
    result = UartTransmitByte(data[data_index]);
    ASSERT_RT(result == BLT_TRUE);  
  }
} /*** end of UartTransmitPacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface packet if one is present.
** \param     data Pointer to byte array where the data is to be stored.
** \return    BLT_TRUE if a packet was received, BLT_FALSE otherwise.
**
****************************************************************************************/
blt_bool UartReceivePacket(blt_int8u *data)
{
  static blt_int8u xcpCtoReqPacket[BOOT_COM_UART_RX_MAX_DATA+1];  /* one extra for length */
  static blt_int8u xcpCtoRxLength;
  static blt_bool  xcpCtoRxInProgress = BLT_FALSE;

  /* start of cto packet received? */
  if (xcpCtoRxInProgress == BLT_FALSE)
  {
    /* store the message length when received */
    if (UartReceiveByte(&xcpCtoReqPacket[0]) == BLT_TRUE)
    {
      if (xcpCtoReqPacket[0] > 0)
      {
        /* indicate that a cto packet is being received */
        xcpCtoRxInProgress = BLT_TRUE;
        /* reset packet data count */
        xcpCtoRxLength = 0;
      }
    }
  }
  else
  {
    /* store the next packet byte */
    if (UartReceiveByte(&xcpCtoReqPacket[xcpCtoRxLength+1]) == BLT_TRUE)
    {
      /* increment the packet data count */
      xcpCtoRxLength++;

      /* check to see if the entire packet was received */
      if (xcpCtoRxLength == xcpCtoReqPacket[0])
      {
        /* copy the packet data */
        CpuMemCopy((blt_int32u)data, (blt_int32u)&xcpCtoReqPacket[1], xcpCtoRxLength);        
        /* done with cto packet reception */
        xcpCtoRxInProgress = BLT_FALSE;

        /* packet reception complete */
        return BLT_TRUE;
      }
    }
  }
  /* packet reception not yet complete */
  return BLT_FALSE;
} /*** end of UartReceivePacket ***/


/************************************************************************************//**
** \brief     Receives a communication interface byte if one is present.
** \param     data Pointer to byte where the data is to be stored.
** \return    BLT_TRUE if a byte was received, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool UartReceiveByte(blt_int8u *data)
{
  blt_int32s result;

  /* try to read a newly received byte */
  result = UARTCharGetNonBlocking(UART0_BASE);
  /* check if a new byte was received */
  if(result != -1)
  {
    /* store the received byte */
    data[0] = (blt_int8u)result;
    /* inform caller of the newly received byte */
    return BLT_TRUE;
  }
  /* inform caller that no new data was received */
  return BLT_FALSE;
} /*** end of UartReceiveByte ***/


/************************************************************************************//**
** \brief     Transmits a communication interface byte.
** \param     data Value of byte that is to be transmitted.
** \return    BLT_TRUE if the byte was transmitted, BLT_FALSE otherwise.
**
****************************************************************************************/
static blt_bool UartTransmitByte(blt_int8u data)
{
  /* write byte to transmit holding register */
  if (UARTCharPutNonBlocking(UART0_BASE, data) == false)
  {
    /* tx holding register can accept new data */
    return BLT_FALSE;
  }
  /* wait for tx holding register to be empty */
  while(UARTSpaceAvail(UART0_BASE) == false) 
  { 
    /* keep the watchdog happy */
    CopService();
  }
  /* byte transmitted */
  return BLT_TRUE;
} /*** end of UartTransmitByte ***/
#endif /* BOOT_COM_UART_ENABLE > 0 */


/*********************************** end of uart.c *************************************/
