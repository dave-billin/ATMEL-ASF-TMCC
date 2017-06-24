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
#include <compiler.h>
#include "conf_usb.h"
#include "ui.h"


#include <stdint.h>
#include <stdbool.h>

static volatile bool main_b_tmc_enable = false;


/// Size of the buffer used for TMCC data
#define  TMCC_BUFFER_SIZE    1024

/// Buffer used for TMCC data
COMPILER_WORD_ALIGNED static uint8_t adc_data_buffer[TMCC_BUFFER_SIZE];
//@}

// check configuration
#if UDI_TMC_EPS_SIZE_ISO_FS>(TMCC_BUFFER_SIZE/2)
# error UDI_TMC_EPS_SIZE_ISO_FS must be <= TMCC_BUFFER_SIZE/2 in cond_usb.h
#endif
#ifdef USB_DEVICE_HS_SUPPORT
# if UDI_TMC_EPS_SIZE_ISO_HS>(TMCC_BUFFER_SIZE/2)
#   error UDI_TMC_EPS_SIZE_ISO_HS must be <= TMCC_BUFFER_SIZE/2 in cond_usb.h
# endif
#endif

// Function Prototypes
static void main_tmc_bulk_in_received(udd_ep_status_t status,
                                      iram_size_t nb_transfered, udd_ep_id_t ep);

static void main_tmc_bulk_out_received(udd_ep_status_t status,
                                       iram_size_t nb_transfered, udd_ep_id_t ep);


////////////////////////////////////////////////////////////////////////////////
bool main_tmc_enable(void)
{
   main_b_tmc_enable = true;
   // Start data reception on OUT endpoints
#if UDI_TMC_EPS_SIZE_INT_FS
   main_tmc_int_in_received(UDD_EP_TRANSFER_OK, 0, 0);
#endif
#if UDI_TMC_EPS_SIZE_BULK_FS
   main_tmc_bulk_in_received(UDD_EP_TRANSFER_OK, 0, 0);
#endif
#if UDI_TMC_EPS_SIZE_ISO_FS
   main_buf_iso_sel=0;
   main_tmc_iso_out_received(UDD_EP_TRANSFER_OK, 0, 0);
#endif
   return true;
}

////////////////////////////////////////////////////////////////////////////////
void main_tmc_disable(void)
{
   main_b_tmc_enable = false;
}

////////////////////////////////////////////////////////////////////////////////
void main_sof_action( void )
{
   // Only process frames if enabled
   if ( main_b_tmc_enable )
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
void main_initiate_abort_bulk_out(void)
{

   static TMC_initiate_abort_bulk_out_response_t response = {0};

   // TODO: Handle INITIATE_ABORT_BULK_OUT request and populate the fields
   //       of response

   udd_g_ctrlreq.payload = (uint8_t*)&response;
   udd_g_ctrlreq.payload_size = min( udd_g_ctrlreq.req.wLength,
                                     sizeof(TMC_initiate_abort_bulk_out_response_t) );
}

////////////////////////////////////////////////////////////////////////////////
bool main_check_abort_bulk_out_status(void)
{
   return true;
}


#if 0
bool main_setup_out_received(void)
{
   // Tell the dev board API that we are connected (uses the API defined for
   // the MattairTech MT-D11 board
   ui_loop_back_state(true);

   udd_g_ctrlreq.payload = adc_data_buffer;
   udd_g_ctrlreq.payload_size = min( udd_g_ctrlreq.req.wLength,
                                     sizeof(adc_data_buffer) );
   return true;
}

////////////////////////////////////////////////////////////////////////////////
bool main_setup_in_received(void)
{
   // Tell the dev board API that we are disconnected (uses the API defined for
   // the MattairTech MT-D11 board
   ui_loop_back_state(false);

   udd_g_ctrlreq.payload = adc_data_buffer;
   udd_g_ctrlreq.payload_size = min( udd_g_ctrlreq.req.wLength,
                                     sizeof(adc_data_buffer) );
   return true;
}
#endif

////////////////////////////////////////////////////////////////////////////////
/** \brief Helper function used to fill a buffer with fake data
 */
static inline void populate_adc_data_buffer( void )
{
   static char const fakeData[] = "0123456789ab";
   static uint8_t data_offset = 0;

   int index = 0;
   while( index < TMCC_BUFFER_SIZE )
   {
      adc_data_buffer[index] = fakeData[data_offset];
      if ( sizeof(fakeData) == ++data_offset )
      {
         data_offset = 0;
      }
   }
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


//==============================================================================
/** \brief Process events on the bulk IN endpoint
 *
 * \param status
 *   Status of the bulk IN endpoint
 *
 * \param nb_transferred
 *   Number of Bytes transferred on the endpoint
 *
 * \param ep
 *   ID of the bulk IN endpoint
 */
void main_tmc_bulk_in_received( udd_ep_status_t status,
                                iram_size_t nb_transfered,
                                udd_ep_id_t ep)
{
   UNUSED(nb_transfered);
   UNUSED(ep);

   if (UDD_EP_TRANSFER_OK != status)
   {
      return;  // STATUS: transfer was aborted!
   }

   ui_loop_back_state(false); // Stop data transfer

   // Fill the data buffer with fake data
   populate_adc_data_buffer();

   // Wait a full buffer
   udi_tmc_bulk_out_run( adc_data_buffer, sizeof(adc_data_buffer),
                         main_tmc_bulk_out_received );
}


//==============================================================================
/** \brief Process events on the bulk OUT endpoint
 *
 * \param status
 *   Status of the bulk IN endpoint
 *
 * \param nb_transferred
 *   Number of Bytes transferred on the endpoint
 *
 * \param ep
 *   ID of the bulk OUT endpoint
 */
void main_tmc_bulk_out_received( udd_ep_status_t status,
                                 iram_size_t nb_transfered,
                                 udd_ep_id_t ep)
{
   UNUSED(ep);

   if (UDD_EP_TRANSFER_OK != status)
   {
      return; // STATUS: transfer was aborted!
   }

   ui_loop_back_state(true);  // Enable loopback

   // Send on IN endpoint the data received on endpoint OUT
   udi_tmc_bulk_in_run( adc_data_buffer, nb_transfered,
                        main_tmc_bulk_in_received );
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
