/************************************************************************************//**
* \file         Demo\ARM7_LPC2000_Olimex_LPC_L2294_Crossworks\Prog\vectors.c
* \brief        Demo program interrupt vectors source file.
* \ingroup      Prog_ARM7_LPC2000_Olimex_LPC_L2294_Crossworks
* \internal
*----------------------------------------------------------------------------------------
*                          C O P Y R I G H T
*----------------------------------------------------------------------------------------
*   Copyright (c) 2011  by Feaser    http://www.feaser.com    All rights reserved
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
void __attribute__ ((interrupt("IRQ")))   TIMER0_ISR(void);


/************************************************************************************//**
** \brief     Timer0 exception routine.
** \return    none.
**
****************************************************************************************/
void TIMER0_ISR(void)
{
  /* clear the interrupt flag */
  T0IR = 0x01;
  /* acknowledge interrupt */
  VICVectAddr = 0;
  /* process time tick */
  TimerUpdate();
} /*** end of TIMER0_ISR ***/


/*********************************** end of vectors.c **********************************/
