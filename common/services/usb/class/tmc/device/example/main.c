/**
 * \file
 *
 * \brief Main functions for USB Device TMC example
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>
#include "compiler.h"
#include "conf_usb.h"
#include "usb_protocol_tmc.h"
#include "ui.h"
#include "adc_driver.h"

#include <stdint.h>
#include <stdbool.h>



/// Maximum number of data Bytes to send to the host at a time
#define DEVICE_DATA_BUFFER_SIZE   255

static volatile bool g_bulkIN_xfer_active = false;

//==============================================================================
/// Structures used to send responses for Bulk-IN/OUT abort operations
typedef union
{
   /// Response to an INITIATE_CLEAR request
   uint8_t initiate_clear;

   /// Response to an INITIATE_BULK_IN/OUT_ABORT request
   TMC_initiate_abort_bulk_xfer_response_t initiate_abort;

   /// Response to a CHECK_BULK_OUT_ABORT request
   TMC_check_abort_bulkOUT_status_response_t check_abortOUT;

   /// Response to a CHECK_BULK_IN_ABORT request
   TMC_check_abort_bulkIN_status_response_t check_abortIN;

   /// Response to a CHECK_CLEAR_STATUS request
   TMC_check_clear_status_response_t check_clear;

} Bulk_abort_response_u;


//==============================================================================
/// \brief Data structure used to track the state of a Bulk IN transfer
typedef struct
{
   uint8_t bTag;     ///< bTag ID of the transfer
   uint32_t numBytesRemaining;   ///< Number of Bytes left to report
   uint32_t numBytesTransferred; ///< Number of Bytes transferred so far
} DeviceDataRequest_t;



COMPILER_PACK_SET(1)
/// Structure used to send data the host in a DEV_DEP_MSG_IN message
typedef struct
{
   /// Message header
   TMC_bulkIN_dev_dep_msg_in_header_t header;

   uint8_t data[DEVICE_DATA_BUFFER_SIZE];
} DeviceDataResponse_t;
COMPILER_PACK_RESET()


COMPILER_WORD_ALIGNED static Bulk_abort_response_u g_bulk_abort_response = {0};

/// Values used to track the active data request
static DeviceDataRequest_t activeDataRequest = {0};

/// Buffer used for TMCC data
COMPILER_WORD_ALIGNED static DeviceDataResponse_t deviceDataResponse;
//@}


// Function Prototypes
static void abort_tmc_bulkIN_transfer(void);
static void main_req_dev_dep_msg_in_sent( udd_ep_status_t status,
                                          iram_size_t nb_transfered,
                                          udd_ep_id_t ep );

////////////////////////////////////////////////////////////////////////////////
/// \brief Helper function used to write a uint32 value to a 32-bit field least-
///        significant Byte first
///
/// \param source  Value to write
/// \param dest    Field to receive source with least-significant Byte first
inline static void write_uint32_lsb_first( uint32_t source, uint32_t* dest )
{
   uint8_t* buffer = (uint8_t*)dest;
   buffer[0] = (uint8_t)(source);
   buffer[1] = (uint8_t)(source >>  8);
   buffer[2] = (uint8_t)(source >> 16);
   buffer[3] = (uint8_t)(source >> 24);
}


////////////////////////////////////////////////////////////////////////////////
bool main_tmc_enable(void)
{
   g_bulkIN_xfer_active = true;

   // Start data reception on OUT endpoints
   UDI_TMC_RECEIVE_BULKOUT_COMMAND();

   return true;
}

////////////////////////////////////////////////////////////////////////////////
void main_tmc_disable(void)
{
   g_bulkIN_xfer_active = false;
}

////////////////////////////////////////////////////////////////////////////////
void main_sof_action( void )
{
   // Only process frames if enabled
   if ( g_bulkIN_xfer_active )
   {
      uint16_t frame_number = udd_get_frame_number();
      ui_process(frame_number);  // Update the LED on the board
   }
}

////////////////////////////////////////////////////////////////////////////////
void main_suspend_action(void)
{
   ui_powerdown();
}

////////////////////////////////////////////////////////////////////////////////
void main_resume_action(void)
{
   ui_wakeup();
}

////////////////////////////////////////////////////////////////////////////////
void main_initiate_abort_bulkOUT(void)
{
   // Bulk-OUT transfers (e.g. data transfers from host computer to USB device)
   // are not supported by this design, so there's nothing to do when a request
   // to abort Bulk-OUT transfers is received

   // Populate fields of the response
   g_bulk_abort_response.initiate_abort.usbtmc_status =
                                          TMC_STATUS_TRANSFER_NOT_IN_PROGRESS;
   g_bulk_abort_response.initiate_abort.bTag = 0;

   udd_g_ctrlreq.payload = (uint8_t*)&g_bulk_abort_response.initiate_abort;
   udd_g_ctrlreq.payload_size = sizeof(TMC_initiate_abort_bulk_xfer_response_t);
}

////////////////////////////////////////////////////////////////////////////////
void main_check_abort_bulkOUT_status(void)
{
   // Bulk-OUT transfers (e.g. data transfers from host computer to USB device)
   // are not supported by this design, so there should never be any Bulk-OUT
   // abort requests pending

   g_bulk_abort_response.check_abortOUT.nbytes_rxd = 0;
   g_bulk_abort_response.check_abortOUT.reserved[0] = 0;
   g_bulk_abort_response.check_abortOUT.reserved[1] = 0;
   g_bulk_abort_response.check_abortOUT.reserved[2] = 0;
   g_bulk_abort_response.check_abortOUT.usbtmc_status =
                                          TMC_STATUS_TRANSFER_NOT_IN_PROGRESS;

   udd_g_ctrlreq.payload = (uint8_t*)&g_bulk_abort_response.check_abortOUT;
   udd_g_ctrlreq.payload_size = sizeof(TMC_check_abort_bulkOUT_status_response_t);
}

////////////////////////////////////////////////////////////////////////////////
void main_initiate_abort_bulkIN(void)
{
   // Populate fields of the response
   g_bulk_abort_response.initiate_abort.usbtmc_status =
                  g_bulkIN_xfer_active ? TMC_STATUS_SUCCESS :
                                           TMC_STATUS_TRANSFER_NOT_IN_PROGRESS;
   g_bulk_abort_response.initiate_abort.bTag = activeDataRequest.bTag;

   abort_tmc_bulkIN_transfer();

   udd_g_ctrlreq.payload = (uint8_t*)&g_bulk_abort_response.initiate_abort;
   udd_g_ctrlreq.payload_size = sizeof(TMC_initiate_abort_bulk_xfer_response_t);
}

////////////////////////////////////////////////////////////////////////////////
void main_check_abort_bulkIN_status(void)
{
   g_bulk_abort_response.check_abortIN.nbytes_txd =
                                       activeDataRequest.numBytesTransferred;
   g_bulk_abort_response.check_abortIN.bmAbortBulkIn = 0;
   g_bulk_abort_response.check_abortIN.reserved[0] = 0;
   g_bulk_abort_response.check_abortIN.reserved[1] = 0;
   g_bulk_abort_response.check_abortIN.usbtmc_status =
                  g_bulkIN_xfer_active ? TMC_STATUS_SUCCESS :
                                           TMC_STATUS_TRANSFER_NOT_IN_PROGRESS;

   udd_g_ctrlreq.payload = (uint8_t*)&g_bulk_abort_response.check_abortIN;
   udd_g_ctrlreq.payload_size = sizeof(TMC_check_abort_bulkIN_status_response_t);
}

////////////////////////////////////////////////////////////////////////////////
void main_initiate_clear(void)
{
   g_bulk_abort_response.initiate_clear = TMC_STATUS_SUCCESS;
   udd_g_ctrlreq.payload = &g_bulk_abort_response.initiate_clear;
   udd_g_ctrlreq.payload_size = sizeof(uint8_t);

   // TODO: implement clearing input/output buffers
}

////////////////////////////////////////////////////////////////////////////////
void main_check_clear_status(void)
{
   // NOTE: This function doesn't presently do anything special since buffers
   //       are cleared more or less instantaneously.

   g_bulk_abort_response.check_clear.usbtmc_status = TMC_STATUS_SUCCESS;
   g_bulk_abort_response.check_clear.bmClear = 0;
   udd_g_ctrlreq.payload = (uint8_t*)&g_bulk_abort_response.check_clear;
   udd_g_ctrlreq.payload_size = sizeof(TMC_check_clear_status_response_t);
}


////////////////////////////////////////////////////////////////////////////////
/** \brief Execution entry point
 *
 *  \remarks
 *    This function invokes initialization routines to configure and start the
 *    USB interface, then enters a permanent idle loop.
 */
int main(void)
{
   irq_initialize_vectors();
   cpu_irq_enable();

   // Initialize the sleep manager
   sleepmgr_init();
#if !SAM0
   sysclk_init();
   board_init();
#else
   system_init();
#endif

   // Initialize the target board using the board's API
   ui_init();

   // Start USB stack to authorize VBus monitoring
   udc_start();

   // Enter an idle loop.  This loop only manages the power mode
   // because the USB management is done by interrupt
   while (true)
   {
      sleepmgr_enter_sleep();
   }
}

////////////////////////////////////////////////////////////////////////////////
/** \brief Helper function used to abort active/pending Bulk IN transfers
 */
void abort_tmc_bulkIN_transfer(void)
{
   // TODO: this may need to get more complex later
   g_bulkIN_xfer_active = false;

   // Tell the dev board API that we are disconnected (uses the API defined for
   // the MattairTech MT-D11 board
   ui_loop_back_state(false);
}


////////////////////////////////////////////////////////////////////////////////
bool main_req_dev_dep_msg_in_received(
                     TMC_bulkOUT_request_dev_dep_msg_in_header_t const* header )
{
   TMC_bulkIN_dev_dep_msg_in_header_t* responseHeader =
                                                   &deviceDataResponse.header;
   TMC_bulkIN_header_t* bulkInHeader = &responseHeader->header;
   uint32_t numBytesTransferred;

   ui_ledOn();    // Light the LED

   // If a transfer is not currently active, start a new one
   if ( activeDataRequest.bTag != header->header.bTag )
   {
      activeDataRequest.bTag = header->header.bTag;
      activeDataRequest.numBytesRemaining = header->transferSize;
      activeDataRequest.numBytesTransferred = 0;
   }

   // Determine if there is data to transfer
   if ( 0 == activeDataRequest.numBytesRemaining )
   {
      // STATUS:
      //   A request is active, but all requested data Bytes have been
      //   transferred.  This should never happen, and it indicates the host
      //   driver may not be well-behaved.  Return false to signal an error.
      return false;
   }

   // Copy sample data into the message
   // TODO: handle the case where no sample data is available yet
   numBytesTransferred = adc_readSamples( deviceDataResponse.data,
                                     min(activeDataRequest.numBytesRemaining,
                                         DEVICE_DATA_BUFFER_SIZE) );

   //-------------------------------------------------
   // Set up the response BulkIN header
   //-------------------------------------------------
   bulkInHeader->MsgID = TMC_BULKIN_DEV_DEP_MSG_IN;
   bulkInHeader->bTag = activeDataRequest.bTag;
   bulkInHeader->bTagInverse = ~activeDataRequest.bTag;
   bulkInHeader->reserved = 0;

   //-------------------------------------------------
   // Set up device-dependent data response header
   //-------------------------------------------------
   responseHeader->transferSize = numBytesTransferred;

   // Set bit zero (EOM) if no Bytes remain in the transfer
   responseHeader->bmTransferAttributes =
                        (activeDataRequest.numBytesRemaining > 0) ? 0 : 1;
   responseHeader->reserved[0] = 0;
   responseHeader->reserved[2] = 0;
   responseHeader->reserved[3] = 0;

   // Update request state
   activeDataRequest.numBytesRemaining -= numBytesTransferred;
   activeDataRequest.numBytesTransferred += numBytesTransferred;

   // Send the response
   return 1 == udi_tmc_bulk_in_run( (uint8_t*)&deviceDataResponse,
            (sizeof(TMC_bulkIN_dev_dep_msg_in_header_t) + numBytesTransferred),
            main_req_dev_dep_msg_in_sent );
}


////////////////////////////////////////////////////////////////////////////////
void main_req_dev_dep_msg_in_sent( udd_ep_status_t status,
                                   iram_size_t nb_transfered, udd_ep_id_t ep )
{
   ui_ledOff();    // Extinguish the LED
   UDI_TMC_RECEIVE_BULKOUT_COMMAND();  // Receive the next command
}

/**
 * \mainpage ASF USB Device TMC Example
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device implementing TMC Class
 * on Atmel MCU with USB module.
 *
 * \section startup Startup
 * The example uses a vendor class which implements a loopback on
 * all endpoints types: control, interrupt, bulk and isochronous.
 * After loading firmware, connect the board
 * (EVKxx,Xplain,...) to the USB Host. A Host application developed
 * on libusb library is provided with application note AVR4901.

 * \note
 * When the application is connected for the first time to the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation wizard on Windows operating systems.
 * - Choose "No, not this time" to connect to Windows Update for this installation
 * - click "Next"
 * - When requested to search the INF file, browse the avr4901\drivers\ folder
 *   provided package of the AVR4901 application note.
 * - click "Next"
*
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and vendor modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/vendor/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds)
 */
