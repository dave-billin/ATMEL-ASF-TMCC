/**
 * \file
 *
 * \brief USB configuration file
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

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"


/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            0x1234
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                \
	(USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_BUS_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)
// (USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "ATMEL ASF"
#define  USB_DEVICE_PRODUCT_NAME          "USBTMC Sample"
// #define  USB_DEVICE_SERIAL_NAME           "123123123123"

/**
 * Device speeds support
 * Low speed not supported by this TMC class
 * @{
 */
//! To authorize the High speed
#if (UC3A3||UC3A4)
#  define  USB_DEVICE_HS_SUPPORT
#elif (SAM3XA||SAM3U)
#  define  USB_DEVICE_HS_SUPPORT
#endif
//@}


/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  main_sof_action()
#define  UDC_SUSPEND_EVENT()              main_suspend_action()
#define  UDC_RESUME_EVENT()               main_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of TMC interface
 *
 * @remarks
 *    The USB TMC Class does not utilize isochronous endpoints.  A single
 *    interrupt endpoint is optional, but is not enabled in this implementation
 * @{
 */
//! Callback function invoked when the USBTMC interface is enabled
#define UDI_TMC_ENABLE_EXT()           main_tmc_enable()

//! Callback function invoked when the USBTMC interface is disabled
#define UDI_TMC_DISABLE_EXT()          main_tmc_disable()

/** \brief
 *   Callback function invoked when the host requests that an active or pending
 *   Bulk OUT transfer be aborted.  This callback function must send a
 *   TMC_initiate_abort_bulk_out_response_t structure to the host indicating
 *   the status of the request
 */
#define UDI_TMC_INITIATE_ABORT_BULK_OUT()   main_initiate_abort_bulk_out()

/** \brief
 *   Callback function invoked when the host asks for the status of a preceding
 *   INITIATE_ABORT_BULK_OUT request
 */
#define UDI_TMC_CHECK_ABORT_BULK_OUT_STATUS()   main_check_abort_bulk_out_status()




//! endpoints size for full speed
//! Note: Disable the endpoints of a type, if size equal 0
#define UDI_TMC_EPS_SIZE_INT_FS    0      // This TMC class does not include an interrupt endpoint
#define UDI_TMC_EPS_SIZE_BULK_FS   64     // Must be a multiple of 4 for USB TMC
#if SAMG55
#define UDI_TMC_EPS_SIZE_ISO_FS   0       // The TMC class does not have isochronous endpoints
#else
#define UDI_TMC_EPS_SIZE_ISO_FS   0
#endif

//! endpoints size for high speed
#define UDI_TMC_EPS_SIZE_INT_HS    0
#define UDI_TMC_EPS_SIZE_BULK_HS  512
#define UDI_TMC_EPS_SIZE_ISO_HS    0

//@}

//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//! Limit the isochronous endpoint in single bank mode for USBB driver
//! to avoid exceeding USB DPRAM.
#define UDD_ISOCHRONOUS_NB_BANK(ep) 1
//@}

//! The includes of classes and other headers must be done
//! at the end of this file to avoid compile error
#include "udi_tmc_conf.h"
#include "ui.h"
#include "main.h"

#endif // _CONF_USB_H_
