/** ###################################################################
**     Filename    : ProcessorExpert.c
**     Project     : ProcessorExpert
**     Processor   : MKL05Z32VLF4
**     Version     : Driver 01.01
**     Compiler    : GNU C Compiler
**     Date/Time   : 2012-07-30, 17:10, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/* MODULE ProcessorExpert */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "I2C.h"
#include "CsIO1.h"
#include "IO1.h"
#include "TIMER.h"
/* Including shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include <stdio.h>
#include <string.h>
#include "Common.h"

/* External 3-axis accelerometer data register addresses */
#define OUT_X_MSB 0x01
#define OUT_X_LSB 0x02
#define OUT_Y_MSB 0x03
#define OUT_Y_LSB 0x04
#define OUT_Z_MSB 0x05
#define OUT_Z_LSB 0x06
/* External 3-axis accelerometer control register addresses */
#define CTRL_REG_1 0x2A
/* External 3-axis accelerometer control register bit masks */
#define ACTIVE_BIT_MASK 0x01
#define F_READ_BIT_MASK 0x02

#define ACC_REG_SIZE 1U
#define READ_COUNT 5

static bool Timeout(LDD_TDeviceData *TimerPtr)
{
  return (TIMER_GetEventStatus(TimerPtr) & LDD_TIMERUNIT_ON_COUNTER_RESTART) != 0;
}

static bool ReadAccRegs(LDD_TDeviceData *I2CPtr, LDD_TDeviceData *TimerPtr,
    TDataState *DataState, uint8_t Address, uint8_t RegCount, uint8_t *Buffer)
{
  DataState->Sent = FALSE;
  I2C_MasterSendBlock(I2CPtr, &Address, sizeof(Address), LDD_I2C_NO_SEND_STOP);
  while (!DataState->Sent && !Timeout(TimerPtr)) {}
  TIMER_ResetCounter(TimerPtr);
  if (!DataState->Sent) {
    printf("ERROR Transmission timed out\n");
    return FALSE;
  }
  DataState->Received = FALSE;
  I2C_MasterReceiveBlock(I2CPtr, Buffer, RegCount, LDD_I2C_SEND_STOP);
  while (!DataState->Received && !Timeout(TimerPtr)) {}
  TIMER_ResetCounter(TimerPtr);
  if (!DataState->Received) {
    printf("ERROR Reception timed out\n");
    return FALSE;
  }
  return TRUE;
}

static bool WriteAccRegs(LDD_TDeviceData *I2CPtr, LDD_TDeviceData *TimerPtr,
    TDataState *DataState, uint8_t Address, uint8_t RegCount, uint8_t *Data)
{
  const uint8_t MAX_REG_COUNT = 16;
  uint8_t SendBuffer[MAX_REG_COUNT];
  
  SendBuffer[0] = Address;
  memcpy(&SendBuffer[1], Data, RegCount);
  DataState->Sent = FALSE;
  I2C_MasterSendBlock(I2CPtr, &SendBuffer, RegCount + 1, LDD_I2C_SEND_STOP);
  while (!DataState->Sent && !Timeout(TimerPtr)) {}
  TIMER_ResetCounter(TimerPtr);
  if (!DataState->Sent) {
    printf("ERROR Transmission timed out\n");
    return FALSE;
  }
  return TRUE;
}

int main(void)
{
  LDD_TDeviceData *I2CPtr = NULL;
  LDD_TDeviceData *TimerPtr = NULL;
  bool Error = FALSE;
  uint8_t Data[10];
  TDataState DataState;
  uint8_t temp;

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  
  printf("*********************************************************\n\r");
  printf("*** DEMO project for the I2C component - begin           \n\r");
  printf("*********************************************************\n\r");
  printf("\n\r");
  printf("Project description:\n\r");
  printf("I2C example of communication with external accelerometer with interrupts.\n\r");
  printf("\n\r");
  
  I2CPtr = I2C_Init(&DataState);
  TimerPtr = TIMER_Init(NULL);

  printf("+++ Step 0 - begin +++\n\r");
  printf("Description: Setup active mode and fast read mode\n\r");
  printf("1) Reading Control register\n\r");
  Error = !ReadAccRegs(I2CPtr, TimerPtr, &DataState, CTRL_REG_1, ACC_REG_SIZE, Data);
  if (!Error) {
    printf("  Control register: 0x%02X\n\r", Data[0]);
  }
  if (!Error) {
    printf("2) Writing Control register\n\r");
    Data[0] = (ACTIVE_BIT_MASK | F_READ_BIT_MASK); /* Set active mode bit and fast read mode bit */
    Error = !WriteAccRegs(I2CPtr, TimerPtr, &DataState, CTRL_REG_1, ACC_REG_SIZE, Data);
  }
  if (!Error) {
    printf("3) Checking Control register\n\r");
    Data[0] = 0;
    Error = !ReadAccRegs(I2CPtr, TimerPtr, &DataState, CTRL_REG_1, ACC_REG_SIZE, Data);
    if (!Error) {
      printf("  Mode register: 0x%02X\n\r", Data[0]);
      if (Data[0] != (ACTIVE_BIT_MASK | F_READ_BIT_MASK)) {
        printf("ERROR Invalid Mode register value\n\r");
        Error = TRUE;
      }
    }
  }
  printf("+++ Step 0 - end +++\n\r");
  if (!Error) {
    printf("+++ Step 1 - begin +++\n\r");
    printf("Description: Read 5 samples of acceleration data\n\r");   
    printf("2) Reading data registers\n\r");
    for(temp = 0 ; temp < READ_COUNT ; temp++){
      Error = !ReadAccRegs(I2CPtr, TimerPtr, &DataState, OUT_X_MSB, 3 * ACC_REG_SIZE, Data);
      if (!Error) {
        printf("  Read data: 0x%02X 0x%02X 0x%02X\n\r", Data[0], Data[1], Data[2]);
        }
    }
  }  
    printf("+++ Step 1 - end +++\n\r");
  
  if (!Error) {
    printf("+++ Step 2 - begin +++\n\r");
    printf("Description: Standby mode setup\n\r");
    printf("1) Setting up standby mode\n\r");
    Data[0] &= (~ACTIVE_BIT_MASK);
    Error = !WriteAccRegs(I2CPtr, TimerPtr, &DataState, CTRL_REG_1, ACC_REG_SIZE, Data);
    if(!Error) {
      printf("  Standby mode activated\n\r");
      }
    printf("2) Clearing Control register\n\r");
    Data[0] = 0x00;
    Error = !WriteAccRegs(I2CPtr, TimerPtr, &DataState, CTRL_REG_1, ACC_REG_SIZE, Data);
    if(!Error) {
      printf("  Control register cleared\n\r");
      }
    printf("+++ Step 2 - end +++\n\r");
  }
  
  I2C_Deinit(I2CPtr);
  TIMER_Deinit(TimerPtr);

  printf("\n\r");
  printf("*********************************************************\n\r");
  printf("*** DEMO project for the I2C component - end             \n\r");
  printf("*********************************************************\n\r");

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/


/* END ProcessorExpert */
/*
** ###################################################################
**
**     This file was created by Processor Expert 0.00.00 [05.02]
**     for the Freescale Kinetis series of microcontrollers.
**
** ###################################################################
*/
