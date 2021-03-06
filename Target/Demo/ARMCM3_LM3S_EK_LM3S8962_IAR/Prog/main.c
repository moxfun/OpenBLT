/************************************************************************************//**
* \file         Demo\ARMCM3_LM3S_EK_LM3S8962_IAR\Prog\main.c
* \brief        Demo program application source file.
* \ingroup      Prog_ARMCM3_LM3S_EK_LM3S8962_IAR
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2012  by Feaser    http://www.feaser.com    All rights reserved
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
#include "header.h"                                    /* generic header               */


/****************************************************************************************
* Function prototypes
****************************************************************************************/
static void Init(void);


/************************************************************************************//**
** \brief     This is the entry point for the bootloader application and is called 
**            by the reset interrupt vector after the C-startup routines executed.
** \return    none.
**
****************************************************************************************/
void main(void)
{
  /* initialize the microcontroller */
  Init();
  /* initialize the bootloader interface */
  BootComInit();

  /* start the infinite program loop */
  while (1)
  {
    /* toggle LED with a fixed frequency */
    LedToggle();
    /* check for bootloader activation request */
    BootComCheckActivationRequest();
  }
} /*** end of main ***/


/************************************************************************************//**
** \brief     Initializes the microcontroller.
** \return    none.
**
****************************************************************************************/
static void Init(void)
{
  /* set the clocking to run at 50MHz from the PLL */
  SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ);
  /* init the led driver */
  LedInit();
  /* init the timer driver */
  TimeInit();
  /* enable IRQ's, because they were initially disabled by the bootloader */
  IrqInterruptEnable();
} /*** end of Init ***/


#ifdef DEBUG
/************************************************************************************//**
** \brief     Called when a runtime assertion failed. It stores information about
**            where the assertion occurred and halts the software program.
** \param     pcFilename Name of the source file where the assertion occurred.
** \param     ulLine Linenumber in the source file where the assertion occurred. 
** \return    none.
**
****************************************************************************************/
void __error__(char *pcFilename, unsigned long ulLine)
{
  static volatile char *assert_failure_file;
  static volatile unsigned long assert_failure_line;

  /* store the file string and line number so that it can be read on a breakpoint*/
  assert_failure_file = pcFilename;
  assert_failure_line = ulLine;

  /* hang the software so that it requires a hard reset */
  for(;;) 
  { 
  }
} /*** end of __error__ ***/
#endif


/*********************************** end of main.c *************************************/
