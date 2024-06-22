/*******************************************************************************
  Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This file contains the "main" function for a project.

  Description:
    This file contains the "main" function for a project.  The
    "main" function calls the "SYS_Initialize" function to initialize the state
    machines of all modules in the system
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include "definitions.h"                // SYS function prototypes
#include "global_defs.h"
#include "misc.h"
#include <stdio.h>

uint8_t i,j;
uint8_t RD_Buffer[16], WR_BUffer[16];

uint32_t Heartbeat_Timer;

// *****************************************************************************
// *****************************************************************************
// Section: Main Entry Point
// *****************************************************************************
// *****************************************************************************

int main ( void )
{
    /* Initialize all modules */
    SYS_Initialize ( NULL );
            
    SYSTICK_TimerCallbackSet (IncSysTick, (uintptr_t) NULL ); // Register the call back function
    SYSTICK_TimerStart(); // Start the SysTick Timer
    
    SetHearbeatInterval(10);
    
    // Runniong from 32KHz, how switch to 32MHz Xtal
    EN_TXVCO_Set(); // Make sure this is turned on to enable TXVCO
    Heartbeat_Timer = GetSysTick();  //Get systick

    while((uint32_t)(GetSysTick() - Heartbeat_Timer) < 100)  // Wait 1 seconds
    {
        DoHeartBeat();
    } // End LED Beat fast
    
    SetHearbeatInterval(50);  // Slow the heartbeat
    
    
    /* Configure External Oscillator */
    OSCCTRL_REGS->OSCCTRL_XOSCCTRL = OSCCTRL_XOSCCTRL_STARTUP(1U) | OSCCTRL_XOSCCTRL_GAIN(0U) | OSCCTRL_XOSCCTRL_RUNSTDBY_Msk | OSCCTRL_XOSCCTRL_ENABLE_Msk;
//    while((OSCCTRL_REGS->OSCCTRL_STATUS & OSCCTRL_STATUS_XOSCRDY_Msk) != OSCCTRL_STATUS_XOSCRDY_Msk)
//    {
//        /* Waiting for the XOSC Ready state */
//    }
    
    // Waiting did not work, just skip it...
    SYSTICK_DelayUs(30);

    // OSC Should be up and running, switch over
    GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_DIV(1U) | GCLK_GENCTRL_SRC(0U) | GCLK_GENCTRL_GENEN_Msk;
    

    
    
    while ( true )
    {
        /* Maintain state machines of all polled MPLAB Harmony modules. */
        SYS_Tasks ( );
        DoHeartBeat();
        
        // Echo chars on serial port
        while(SERCOM1_USART_ReadCountGet())
        {
            i = SERCOM1_USART_Read(RD_Buffer, 1);  // read a byte
            SERCOM1_USART_Write(RD_Buffer, 1); // Write it out
        }
    }

    /* Execution should not come here during normal operation */

    return ( EXIT_FAILURE );
}


/*******************************************************************************
 End of File
*/

