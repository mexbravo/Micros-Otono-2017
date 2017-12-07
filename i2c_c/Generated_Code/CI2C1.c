/* ###################################################################
**     This component module is generated by Processor Expert. Do not modify it.
**     Filename    : CI2C1.c
**     Project     : i2c_c
**     Processor   : MC9S08SH8CPJ
**     Component   : InternalI2C
**     Version     : Component 01.287, Driver 01.28, CPU db: 3.00.067
**     Compiler    : CodeWarrior HCS08 C Compiler
**     Date/Time   : 2017-12-06, 19:23, # CodeGen: 5
**     Abstract    :
**          This component encapsulates the internal I2C communication 
**          interface. The implementation of the interface is based 
**          on the Philips I2C-bus specification version 2.0. 
**          Interface features:
**          MASTER mode
**            - Multi master communication
**            - The combined format of communication possible 
**              (see "Automatic stop condition" property)
**            - 7-bit slave addressing (10-bit addressing can be made as well)
**            - Acknowledge polling provided
**            - No wait state initiated when a slave device holds the SCL line low
**            - Holding of the SCL line low by slave device recognized as 'not available bus'
**            - Invalid start/stop condition detection provided
**          SLAVE mode
**            - 7-bit slave addressing
**            - General call address detection provided
**     Settings    :
**         Serial channel              : IIC
**
**         Protocol
**             Mode                    : MASTER
**             Auto stop condition     : yes
**             SCL frequency           : 400 kHz
**
**         Initialization
**
**             Target slave address    : 8
**             Component init          : Enabled
**             Events                  : Enabled
**
**         Registers
**             Input buffer            : IICD      [$005C]
**             Output buffer           : IICD      [$005C]
**             Control register        : IICC1     [$005A]
**             Status register         : IICS      [$005B]
**             Baud setting reg.       : IICF      [$0059]
**             Address register        : IICA      [$0058]
**
**         Interrupt
**             Vector name             : Viic
**             Priority                : 
**
**         Used pins                   :
**       ----------------------------------------------------------
**            Function    | On package |    Name
**       ----------------------------------------------------------
**              SDA       |     6      |  PTB6_SDA_XTAL
**              SCL       |     5      |  PTB7_SCL_EXTAL
**       ----------------------------------------------------------
**     Contents    :
**         SendChar        - byte CI2C1_SendChar(byte Chr);
**         RecvChar        - byte CI2C1_RecvChar(byte *Chr);
**         SendBlock       - byte CI2C1_SendBlock(void* Ptr, word Siz, word *Snt);
**         RecvBlock       - byte CI2C1_RecvBlock(void* Ptr, word Siz, word *Rcv);
**         GetCharsInTxBuf - word CI2C1_GetCharsInTxBuf(void);
**         GetCharsInRxBuf - word CI2C1_GetCharsInRxBuf(void);
**         SelectSlave     - byte CI2C1_SelectSlave(byte Slv);
**
**     Copyright : 1997 - 2014 Freescale Semiconductor, Inc. 
**     All Rights Reserved.
**     
**     Redistribution and use in source and binary forms, with or without modification,
**     are permitted provided that the following conditions are met:
**     
**     o Redistributions of source code must retain the above copyright notice, this list
**       of conditions and the following disclaimer.
**     
**     o Redistributions in binary form must reproduce the above copyright notice, this
**       list of conditions and the following disclaimer in the documentation and/or
**       other materials provided with the distribution.
**     
**     o Neither the name of Freescale Semiconductor, Inc. nor the names of its
**       contributors may be used to endorse or promote products derived from this
**       software without specific prior written permission.
**     
**     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
**     ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
**     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
**     ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
**     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
**     ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**     (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
**     SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**     
**     http: www.freescale.com
**     mail: support@freescale.com
** ###################################################################*/
/*!
** @file CI2C1.c
** @version 01.28
** @brief
**          This component encapsulates the internal I2C communication 
**          interface. The implementation of the interface is based 
**          on the Philips I2C-bus specification version 2.0. 
**          Interface features:
**          MASTER mode
**            - Multi master communication
**            - The combined format of communication possible 
**              (see "Automatic stop condition" property)
**            - 7-bit slave addressing (10-bit addressing can be made as well)
**            - Acknowledge polling provided
**            - No wait state initiated when a slave device holds the SCL line low
**            - Holding of the SCL line low by slave device recognized as 'not available bus'
**            - Invalid start/stop condition detection provided
**          SLAVE mode
**            - 7-bit slave addressing
**            - General call address detection provided
*/         
/*!
**  @addtogroup CI2C1_module CI2C1 module documentation
**  @{
*/         


/* MODULE CI2C1. */


#pragma MESSAGE DISABLE C4002          /* Disable warning C4002 "Result not used" */

#include "Events.h"
#include "CI2C1.h"

/* SerFlag bits */
#define OVERRUN_ERR      0x01U         /* Overrun error flag bit   */
#define WAIT_RX_CHAR     0x02U         /* Wait for received char. flag bit (Master)  */
#define CHAR_IN_TX       0x04U         /* Char is in TX buffer (Master)    */
#define CHAR_IN_RX       0x08U         /* Char is in RX buffer     */
#define FULL_TX          0x10U         /* Full transmit buffer     */
#define IN_PROGRES       0x20U         /* Communication is in progress (Master) */
#define FULL_RX          0x40U         /* Full receive buffer      */
#define MSxSL            0x80U         /* Master x Slave flag bit  */

/* Mask definitions used in 'MainComm' internal method */

static byte CI2C1_SlaveAddr;           /* Variable for Slave address */
static word InpLenM;                   /* Length of input bufer's content */
static byte *InpPtrM;                  /* Pointer to input buffer for Master mode */
static word OutLenM;                   /* Length of output bufer's content */
static byte * OutPtrM;                 /* Pointer to output buffer for Master mode */
word CI2C1_SndRcvTemp;                 /* Temporary variable for SendChar (RecvChar) when they call SendBlock (RecvBlock) */
static byte ChrTemp;                   /* Temporary variable for SendChar method */
static byte CI2C1_SerFlag;             /* Flags for serial communication */
                                       /* Bits: 0 - OverRun error */
                                       /*       1 - Wait for received char. flag bit (Master) */
                                       /*       2 - Char is in TX buffer (Master) */
                                       /*       3 - Char in RX buffer */
                                       /*       4 - Full TX buffer */
                                       /*       5 - Running int from TX */
                                       /*       6 - Full RX buffer */
                                       /*       7 - Master x Slave */
/*
** ===================================================================
**     Method      :  CI2C1_Interrupt (component InternalI2C)
**
**     Description :
**         The method services the interrupt of the selected peripheral(s)
**         and eventually invokes event(s) of the component.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
#define RXAK 0x01U
#define SRW  0x04U
#define IBAL 0x10U
#define IAAS 0x40U

#define ON_ARBIT_LOST 0x01U
#define ON_FULL_RX    0x02U
#define ON_RX_CHAR    0x04U
#define ON_FREE_TX    0x08U
#define ON_TX_CHAR    0x10U
#define ON_OVERRUN    0x20U
#define ON_TX_EMPTY   0x40U

ISR(CI2C1_Interrupt)
{
  byte Status;                         /* Safe status register */

  Status = IICS;
  IICS_IICIF = 1U;                     /* Clear interrupt flag and ARBL flag if set (by means of the read modify write effect) */
  if(IICC1_MST) {                      /* Is device in master mode? */
    if(IICC1_TX) {                     /* Is device in Tx mode? */
      if(Status & RXAK) {              /* NACK received? */
        IICC1_MST = 0U;                /* Switch device to slave mode (stop signal sent) */
        IICC1_TX = 0U;                 /* Switch to Rx mode */
        OutLenM = 0U;                  /* No character for sending */
        InpLenM = 0U;                  /* No character for reception */
        CI2C1_SerFlag &= (uint8_t)~(uint8_t)((CHAR_IN_TX|WAIT_RX_CHAR|IN_PROGRES)); /* No character for sending or reception*/
      }
      else {
        if(OutLenM) {                  /* Is any char. for transmitting? */
          OutLenM--;                   /* Decrease number of chars for the transmit */
          IICD = *(OutPtrM)++;         /* Send character */
        }
        else {
          if(InpLenM) {                /* Is any char. for reception? */
            if(InpLenM == 1U) {        /* If only one char to receive */
              IICC1_TXAK = 1U;         /* then transmit ACK disable */
            }
            else {
              IICC1_TXAK = 0U;         /* else transmit ACK enable */
            }
            IICC1_TX = 0U;             /* Switch to Rx mode */
            (void)IICD;                /* Dummy read character */
          }
          else {
            CI2C1_SerFlag &= (uint8_t)~(uint8_t)(IN_PROGRES); /* Clear flag "busy" */
            IICC1_MST = 0U;            /* Switch device to slave mode (stop signal sent) */
            IICC1_TX = 0U;             /* Switch to Rx mode */
            CI2C1_OnTransmitData();    /* Invoke OnTransmitData event */
          }
        }
      }
    }
    else {
      InpLenM--;                       /* Decrease number of chars for the receive */
      if(InpLenM) {                    /* Is any char. for reception? */
        if(InpLenM == 1U) {
          IICC1_TXAK = 1U;             /* Transmit ACK disable */
        }
      }
      else {
        IICC1_MST = 0U;                /* If no, switch device to slave mode (stop signal sent) */
        IICC1_TXAK = 0U;               /* Transmit ACK enable */
      }
      *(InpPtrM)++ = IICD;             /* Receive character */
      if(!InpLenM) {                   /* Is any char. for reception? */
        CI2C1_OnReceiveData();         /* Invoke OnReceiveData event */
      }
    }
  }
  else {
    if(Status & IBAL) {                /* Arbitration lost? */
      OutLenM = 0U;                    /* No character for sending */
      InpLenM = 0U;                    /* No character for reception */
      CI2C1_SerFlag &= (uint8_t)~(uint8_t)((CHAR_IN_TX|WAIT_RX_CHAR|IN_PROGRES)); /* No character for sending or reception*/
      IICC1_TX = 0U;                   /* Switch to Rx mode */
    }
  }
}

/*
** ===================================================================
**     Method      :  CI2C1_SendChar (component InternalI2C)
**     Description :
**         When working as a MASTER, this method writes one (7-bit
**         addressing) or two (10-bit addressing) slave address bytes
**         inclusive of R/W bit = 0 to the I2C bus and then writes one
**         character (byte) to the bus. The slave address must be
**         specified before, by the "SelectSlave" or "SelectSlave10"
**         method or in the component initialization section, "Target
**         slave address init" property. If interrupt service is
**         enabled and the method returns ERR_OK, it doesn't mean that
**         transmission was successful. The state of transmission is
**         obtainable from (OnTransmitData, OnError or OnArbitLost)
**         events. 
**         When working as a SLAVE, this method writes a character to
**         the internal output slave buffer and, after the master
**         starts the communication, to the I2C bus. If no character is
**         ready for a transmission (internal output slave buffer is
**         empty), the Empty character will be sent (see "Empty
**         character" property).
**     Parameters  :
**         NAME            - DESCRIPTION
**         Chr             - Character to send.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  Device is disabled
**                           ERR_BUSY - The slave device is busy, it
**                           does not respond by an acknowledge (only in
**                           master mode and when interrupt service is
**                           disabled)
**                           ERR_BUSOFF - Clock timeout elapsed or
**                           device cannot transmit data
**                           ERR_TXFULL - Transmitter is full (slave
**                           mode only)
**                           ERR_ARBITR - Arbitration lost (only when
**                           interrupt service is disabled and in master
**                           mode)
** ===================================================================
*/
byte CI2C1_SendChar(byte Chr)
{
  if((IICS_BUSY) || (InpLenM) || (CI2C1_SerFlag & (CHAR_IN_TX|WAIT_RX_CHAR|IN_PROGRES))) { /* Is the bus busy */
    return ERR_BUSOFF;                 /* If yes then error */
  }
  ChrTemp = Chr;                       /* Save character */
  return (CI2C1_SendBlock(&ChrTemp, 1U, &CI2C1_SndRcvTemp)); /* Send character and return */
}

/*
** ===================================================================
**     Method      :  CI2C1_RecvChar (component InternalI2C)
**     Description :
**         When working as a MASTER, this method writes one (7-bit
**         addressing) or two (10-bit addressing) slave address bytes
**         inclusive of R/W bit = 1 to the I2C bus, then reads one
**         character (byte) from the bus and then sends the stop
**         condition. The slave address must be specified before, by
**         the "SelectSlave" or "SelectSlave10" method or in component
**         initialization section, property "Target slave address init".
**         If interrupt service is enabled and the method returns
**         ERR_OK, it doesn't mean that transmission was finished
**         successfully. The state of transmission must be tested by
**         means of events (OnReceiveData, OnError or OnArbitLost). In
**         case of successful transmission, received data is ready
**         after OnReceiveData event is called. 
**         When working as a SLAVE, this method reads a character from
**         the input slave buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Chr             - Received character.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  Device is disabled
**                           ERR_BUSY - The slave device is busy, it
**                           does not respond by the acknowledge (only
**                           in master mode and when interrupt service
**                           is disabled)
**                           ERR_BUSOFF - Clock timeout elapsed or
**                           device cannot receive data
**                           ERR_RXEMPTY - No data in receiver (slave
**                           mode only)
**                           ERR_OVERRUN - Overrun error was detected
**                           from the last character or block received
**                           (slave mode only)
**                           ERR_ARBITR - Arbitration lost (only when
**                           interrupt service is disabled and in master
**                           mode)
**                           ERR_NOTAVAIL - Method is not available in
**                           current mode - see the comment in the
**                           generated code
** ===================================================================
*/
/*
byte CI2C1_RecvChar(byte *Chr)

**  This method is implemented as a macro. See CI2C1.h file.  **
*/

/*
** ===================================================================
**     Method      :  CI2C1_SendBlock (component InternalI2C)
**     Description :
**         When working as a MASTER, this method writes one (7-bit
**         addressing) or two (10-bit addressing) slave address bytes
**         inclusive of R/W bit = 0 to the I2C bus and then writes the
**         block of characters to the bus. The slave address must be
**         specified before, by the "SelectSlave" or "SlaveSelect10"
**         method or in component initialization section, "Target slave
**         address init" property. If interrupt service is enabled and
**         the method returns ERR_OK, it doesn't mean that transmission
**         was successful. The state of transmission is detectable by
**         means of events (OnTransmitData, OnError or OnArbitLost).
**         Data to be send is not copied to an internal buffer and
**         remains in the original location. Therefore the content of
**         the buffer should not be changed until the transmission is
**         complete. Event OnTransmitData can be used to detect the end
**         of the transmission.
**         When working as a SLAVE, this method writes a block of
**         characters to the internal output slave buffer and then,
**         after the master starts the communication, to the I2C bus.
**         If no character is ready for a transmission (internal output
**         slave buffer is empty), the "Empty character" will be sent
**         (see "Empty character" property). In SLAVE mode the data are
**         copied to an internal buffer, if specified by "Output buffer
**         size" property.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Ptr             - Pointer to the block of data to send.
**         Siz             - Size of the block.
**       * Snt             - Amount of data sent (moved to a buffer).
**                           In master mode, if interrupt support is
**                           enabled, the parameter always returns the
**                           same value as the parameter 'Siz' of this
**                           method.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  Device is disabled
**                           ERR_BUSY - The slave device is busy, it
**                           does not respond by the acknowledge (only
**                           in master mode and when interrupt service
**                           is disabled)
**                           ERR_BUSOFF - Clock timeout elapsed or
**                           device cannot transmit data
**                           ERR_TXFULL - Transmitter is full. Some data
**                           has not been sent. (slave mode only)
**                           ERR_ARBITR - Arbitration lost (only when
**                           interrupt service is disabled and in master
**                           mode)
** ===================================================================
*/
byte CI2C1_SendBlock(const void * Ptr,word Siz,word *Snt)
{
  if (!Siz) {                          /* Test variable Size on zero */
    *Snt = 0U;
    return ERR_OK;                     /* If zero then OK */
  }
  if((IICS_BUSY) || (InpLenM) || (CI2C1_SerFlag & (CHAR_IN_TX|WAIT_RX_CHAR|IN_PROGRES))) { /* Is the bus busy */
    return ERR_BUSOFF;                 /* If yes then error */
  }
  EnterCritical();                     /* Enter the critical section */
  CI2C1_SerFlag |= IN_PROGRES;         /* Set flag "busy" */
  OutLenM = Siz;                       /* Set lenght of data */
  OutPtrM = (byte *)Ptr;               /* Save pointer to data for transmitting */
  IICC1_TX = 1U;                       /* Set TX mode */
  if(IICC1_MST) {                      /* Is device in master mode? */
    IICC1_RSTA = 1U;                   /* If yes then repeat start cycle generated */
  }
  else {
    IICC1_MST = 1U;                    /* If no then start signal generated */
  }
  IICD = CI2C1_SlaveAddr;              /* Send slave address */
  ExitCritical();                      /* Exit the critical section */
  *Snt = Siz;                          /* Dummy number of really sent chars */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CI2C1_RecvBlock (component InternalI2C)
**     Description :
**         When working as a MASTER, this method writes one (7-bit
**         addressing) or two (10-bit addressing) slave address bytes
**         inclusive of R/W bit = 1 to the I2C bus, then reads the
**         block of characters from the bus and then sends the stop
**         condition. The slave address must be specified before, by
**         the "SelectSlave" or "SelectSlave10" method or in component
**         initialization section, "Target slave address init" property.
**         If interrupt service is enabled and the method returns
**         ERR_OK, it doesn't mean that transmission was finished
**         successfully. The state of transmission must be tested by
**         means of events (OnReceiveData, OnError or OnArbitLost). In
**         case of successful transmission, received data is ready
**         after OnReceiveData event is called. 
**         When working as a SLAVE, this method reads a block of
**         characters from the input slave buffer.
**     Parameters  :
**         NAME            - DESCRIPTION
**       * Ptr             - A pointer to the block space for received
**                           data.
**         Siz             - The size of the block.
**       * Rcv             - Amount of received data. In master mode,
**                           if interrupt support is enabled, the
**                           parameter always returns the same value as
**                           the parameter 'Siz' of this method.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  Device is disabled
**                           ERR_BUSY - The slave device is busy, it
**                           does not respond by an acknowledge (only in
**                           master mode and when interrupt service is
**                           disabled)
**                           ERR_BUSOFF - Clock timeout elapsed or
**                           device cannot receive data
**                           ERR_RXEMPTY - The receive buffer didn't
**                           contain the requested number of data. Only
**                           available data (or no data) has been
**                           returned  (slave mode only).
**                           ERR_OVERRUN - Overrun error was detected
**                           from last character or block receiving
**                           (slave mode only)
**                           ERR_ARBITR - Arbitration lost (only when
**                           interrupt service is disabled and in master
**                           mode)
**                           ERR_NOTAVAIL - Method is not available in
**                           current mode - see the comment in the
**                           generated code.
** ===================================================================
*/
byte CI2C1_RecvBlock(void* Ptr,word Siz,word *Rcv)
{
  if (!Siz) {                          /* Test variable Size on zero */
    *Rcv = 0U;
    return ERR_OK;                     /* If zero then OK */
  }
  if((IICS_BUSY) || (InpLenM) || (CI2C1_SerFlag & (CHAR_IN_TX|WAIT_RX_CHAR|IN_PROGRES))) { /* Is the bus busy */
    return ERR_BUSOFF;                 /* If yes then error */
  }
  EnterCritical();                     /* Enter the critical section */
  InpLenM = Siz;                       /* Set lenght of data */
  InpPtrM = (byte *)Ptr;               /* Save pointer to data for reception */
  IICC1_TX = 1U;                       /* Set TX mode */
  if(IICC1_MST) {                      /* Is device in master mode? */
    IICC1_RSTA = 1U;                   /* If yes then repeat start cycle generated */
  }
  else {
    IICC1_MST = 1U;                    /* If no then start signal generated */
  }
  IICD = (byte)(CI2C1_SlaveAddr+1U);   /* Send slave address */
  ExitCritical();                      /* Exit the critical section */
  *Rcv = Siz;                          /* Dummy number of really received chars */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CI2C1_GetCharsInTxBuf (component InternalI2C)
**     Description :
**         Returns number of characters in the output buffer. In SLAVE
**         mode returns the number of characters in the internal slave
**         output buffer. In MASTER mode returns number of characters
**         to be sent from the user buffer (passed by SendBlock method).
**         This method is not supported in polling mode.
**     Parameters  : None
**     Returns     :
**         ---             - Number of characters in the output buffer.
** ===================================================================
*/
word CI2C1_GetCharsInTxBuf(void)
{
  return(OutLenM);                     /* Return number of chars remaining in the Master Tx buffer */
}

/*
** ===================================================================
**     Method      :  CI2C1_GetCharsInRxBuf (component InternalI2C)
**     Description :
**         Returns number of characters in the input buffer. In SLAVE
**         mode returns the number of characters in the internal slave
**         input buffer. In MASTER mode returns number of characters to
**         be received into a user buffer (passed by RecvChar or
**         RecvBlock method).
**         This method is not supported in polling mode.
**     Parameters  : None
**     Returns     :
**         ---             - Number of characters in the input buffer.
** ===================================================================
*/
word CI2C1_GetCharsInRxBuf(void)
{
  return(InpLenM);                     /* Return number of chars remaining in the Master Tx buffer */
}

/*
** ===================================================================
**     Method      :  CI2C1_SelectSlave (component InternalI2C)
**     Description :
**         This method selects a new slave for communication by its
**         7-bit slave address value. Any send or receive method
**         directs to or from selected device, until a new slave device
**         is selected by this method. This method is not available for
**         the SLAVE mode.
**     Parameters  :
**         NAME            - DESCRIPTION
**         Slv             - 7-bit slave address value.
**     Returns     :
**         ---             - Error code, possible codes:
**                           ERR_OK - OK
**                           ERR_BUSY - The device is busy, wait until
**                           the current operation is finished.
**                           ERR_SPEED - This device does not work in
**                           the active speed mode
**                           ERR_DISABLED -  The device is disabled
** ===================================================================
*/
byte CI2C1_SelectSlave(byte Slv)
{
  if (IICC1_MST == 1U) {               /* Is the device in the active state? */
    return ERR_BUSY;                   /* If yes then error */
  }
  CI2C1_SlaveAddr = (byte)(Slv << 1);  /* Set slave address */
  return ERR_OK;                       /* OK */
}

/*
** ===================================================================
**     Method      :  CI2C1_Init (component InternalI2C)
**
**     Description :
**         Initializes the associated peripheral(s) and the component 
**         internal variables. The method is called automatically as a 
**         part of the application initialization code.
**         This method is internal. It is used by Processor Expert only.
** ===================================================================
*/
void CI2C1_Init(void)
{
  CI2C1_SerFlag = 0x80U;               /* Reset all flags */
  CI2C1_SlaveAddr = 0x10U;             /* Set variable for slave address */
  InpLenM = 0U;                        /* No data to be received */
  /* IICF: MULT1=0,MULT0=0,ICR5=0,ICR4=0,ICR3=0,ICR2=0,ICR1=0,ICR0=0 */
  setReg8(IICF, 0x00U);                 
  IICC1_IICEN = 1U;                    /* Enable device */
  /* IICC1: IICEN=1,IICIE=1,MST=0,TX=0,TXAK=0,RSTA=0,??=0,??=0 */
  IICC1 = 0xC0U;                       /* Control register settings */
}


/* END CI2C1. */

/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.09]
**     for the Freescale HCS08 series of microcontrollers.
**
** ###################################################################
*/
