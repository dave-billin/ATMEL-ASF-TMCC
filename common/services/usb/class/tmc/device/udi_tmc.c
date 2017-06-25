/**
 * \file
 *
 * \brief USB Vendor class interface.
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

#include "conf_usb.h"
#include "usb_protocol.h"
#include "compiler.h"
#include "udd.h"
#include "udc.h"
#include "udi_tmc.h"
#include "usb_protocol_tmc.h"

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

// Configuration check
#ifndef UDI_TMC_ENABLE_EXT
# error UDI_TMC_ENABLE_EXT must be defined in conf_usb.h file.
#endif
#ifndef UDI_TMC_DISABLE_EXT
# error UDI_TMC_DISABLE_EXT must be defined in conf_usb.h file.
#endif

/**
 * \ingroup udi_vendor_group
 * \defgroup udi_vendor_group_udc Interface with USB Device Core (UDC)
 *
 * Structures and functions required by UDC.
 *
 * @{
 */
static bool udi_tmc_enable(void);
static void udi_tmc_disable(void);
static bool udi_tmc_setup(void);
static uint8_t udi_tmc_getsetting(void);
static void udi_send_usbtmc_capabilities(void);
static void udi_indicator_pulse(void);

//! Global structure which contains standard UDI API for UDC
UDC_DESC_STORAGE udi_api_t udi_api_tmc =
{
   .enable = udi_tmc_enable,           // USB enable callback
   .disable = udi_tmc_disable,         // USB disable callback
   .setup = udi_tmc_setup,             // Callback to handle control transfers
   .getsetting = udi_tmc_getsetting,   // UDI settings getter callback
   .sof_notify = NULL,                 // USB start-of-frame callback
};
//@}


/**
 * \ingroup udi_vendor_group
 * \defgroup udi_vendor_group_internal Implementation of UDI Vendor Class
 *
 * Class internal implementation
 * @{
 */

//! USB descriptor alternate setting used
static uint8_t udi_tmc_alternate_setting = 0;

/**
 * \name Internal routines
 */
//@{

////////////////////////////////////////////////////////////////////////////////
/** \brief Called by UDC (USB stack lower layer) to enable the USB interface
 *
 * @return true on success; else false on error
 */
bool udi_tmc_enable(void)
{
   udi_tmc_alternate_setting = udc_get_interface_desc()->bAlternateSetting;
   if (1 == udi_tmc_alternate_setting)
   {
      // Call application callback
      // to notify that interface is enabled
      if (!UDI_TMC_ENABLE_EXT())
      {
         return false;
      }
   }
   return true;
}


////////////////////////////////////////////////////////////////////////////////
/** \brief Called by UDC (USB stack lower layer) to disable the USB interface
 */
void udi_tmc_disable(void)
{
   if (1 == udi_tmc_alternate_setting)
   {
      UDI_TMC_DISABLE_EXT();
   }
}

////////////////////////////////////////////////////////////////////////////////
/** Handles/dispatches an INDICATOR_PULSE request
 */
inline static void udi_indicator_pulse(void)
{
#if USBTMC_SUPPORT_INDICATOR_PULSE
   static uint8_t result = TMC_STATUS_SUCCESS;
   UDI_TMC_INDICATOR_PULSE_EXT();
#else
   static uint8_t result = TMC_STATUS_FAILED;
#endif
   udd_g_ctrlreq.payload = &result;
   udd_g_ctrlreq.payload_size = sizeof(uint8_t);
}

////////////////////////////////////////////////////////////////////////////////
/** \brief Called when a USBTMC request is received on the Control IN endpoint
 *
 * @return true if the request was accepted; else false on error
 */
static inline bool udi_process_tmc_control_in_request(void)
{
   bool result = true; // Default to indicating the request was handled

   // Process control endpoint requests from the host
   switch ( udd_g_ctrlreq.req.bRequest )
   {
      case TMC_CTRL_REQ_INITIATE_ABORT_BULK_OUT:
      {
         // Call handler function
         UDI_TMC_INITIATE_ABORT_BULK_OUT_EXT();
         break;
      }

      case TMC_CTRL_REQ_CHECK_ABORT_BULK_OUT_STATUS:
      {
         // Call handler function
         UDI_TMC_CHECK_ABORT_BULK_OUT_STATUS_EXT();
         break;
      }

      case TMC_CTRL_REQ_INITIATE_ABORT_BULK_IN:
      {
         UDI_TMC_INITIATE_ABORT_BULK_IN_EXT();
         break;
      }

      case TMC_CTRL_REQ_CHECK_ABORT_BULK_IN_STATUS:
      {
         UDI_TMC_CHECK_ABORT_BULK_IN_STATUS_EXT();
         break;
      }

      case TMC_CTRL_REQ_INITIATE_CLEAR:
      {
         UDI_TMC_INITIATE_CLEAR_EXT();
         break;
      }

      case TMC_CTRL_REQ_CHECK_CLEAR_STATUS:
      {
         UDI_TMC_CHECK_CLEAR_STATUS_EXT();
         break;
      }

      case TMC_CTRL_REQ_GET_CAPABILITIES:
      {
         udi_send_usbtmc_capabilities();
         break;
      }

      case TMC_CTRL_REQ_INDICATOR_PULSE:
      {
         udi_indicator_pulse();
         break;
      }

      default:
      {
         // Hitting this assert means an unrecognized USBTMC control
         // request ID was received.  This should never happen, and ,most
         // likely indicates a programming error somewhere
         result = false;
      }
   }

   return result;
}


////////////////////////////////////////////////////////////////////////////////
/** \brief Called by UDC (USB stack lower layer) when a USB setup interface
 *         request is received
 *
 * @return true on success; else false on error
 */
bool udi_tmc_setup(void)
{
   int setup_type = Udd_setup_type();
   bool result = false; // Default to handling as an unsupported request

   if ( Udd_setup_is_in() )
   {
      if ( setup_type == USB_REQ_TYPE_CLASS )
      {
         result = udi_process_tmc_control_in_request();
      }
   }

#if 0    // This implementation does not support Control-IN requests
   else if ( Udd_setup_is_out() )
   {
      if ((setup_type == USB_REQ_TYPE_VENDOR)
            && (udd_g_ctrlreq.req.bRequest == 0)
            && (0 != udd_g_ctrlreq.req.wLength))
      {
         result = UDI_TMC_SETUP_OUT_RECEIVED();
      }
   }
#endif

   return result;
}


////////////////////////////////////////////////////////////////////////////////
/** \brief Called by UDC (USB stack lower layer) to obtain the current alternate
 *         setting of the USB interface
 *
 * @return The value of the alternate setting
 */
uint8_t udi_tmc_getsetting(void)
{
   return udi_tmc_alternate_setting;
}
//@}


////////////////////////////////////////////////////////////////////////////////
/**
 * \brief Start a transfer on bulk IN
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback
 * is called.  The \a callback returns the transfer status and eventually the
 * number of Bytes transfered.
 *
 * \param buf           Word-aligned buffer in Internal RAM to send or fill
 *                      (use the COMPILER_WORD_ALIGNED macro)
 * \param buf_size      Size of the buffer to send or fill in Bytes
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return 1 on success; else 0
 */
bool udi_tmc_bulk_in_run(uint8_t * buf, iram_size_t buf_size,
                         udd_callback_trans_t callback)
{
   return udd_ep_run(UDI_TMC_EP_BULK_IN,
                     false,
                     buf,
                     buf_size,
                     callback);
}


////////////////////////////////////////////////////////////////////////////////
/**
 * \brief Start a transfer on bulk OUT
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback
 * is called.  The \a callback returns the transfer status and eventually the
 * number of byte transfered
 *
 * \param buf           Word-aligned buffer in Internal RAM to send or fill
 *                      (use the COMPILER_WORD_ALIGNED macro)
 * \param buf_size      Size of the buffer to send or fill in Bytes
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return 1 on success; else 0
 */
bool udi_tmc_bulk_out_run(uint8_t * buf, iram_size_t buf_size,
                          udd_callback_trans_t callback)
{
   return udd_ep_run(UDI_TMC_EP_BULK_OUT,
                     false,
                     buf,
                     buf_size,
                     callback);
}

////////////////////////////////////////////////////////////////////////////////
/** Sends USBTMC capabilities to the host over the Control-IN endpoint
 */
void udi_send_usbtmc_capabilities()
{
   struct USBTMC_capabilities
   {
      uint8_t  usbtmc_status;  ///< Status indication for the request
      uint8_t  reserved1;
      uint16_t bcdUSBTMC;     ///< BCD version number of the USBTMC interface
      uint8_t  interfaceCaps; ///< USBTMC interface capabilities
      uint8_t  deviceCaps;    ///< USBTMC device capabilities
      uint8_t  reserved6[18];

   };

   COMPILER_WORD_ALIGNED static struct USBTMC_capabilities capabilities = {
                      TMC_STATUS_SUCCESS,    // USBTMC_status
                      0,                     // reserved
                      0x0100,                // bcdUSBTMC
                      ( (USBTMC_SUPPORT_INDICATOR_PULSE << 2) ||  // indicator pulse
                        (USBTMC_IS_TALK_ONLY << 1) ||             // talk-only
                        (USBTMC_IS_LISTEN_ONLY << 0)              // listen-only
                      ),
                      (USBTMC_SUPPORT_TERMCHAR << 0),             // Bulk-IN TermChar support
                      {0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0} // Reserved Bytes
                    };

   udd_g_ctrlreq.payload = (uint8_t*)&capabilities;
   udd_g_ctrlreq.payload_size = sizeof(struct USBTMC_capabilities);
}

//@}
