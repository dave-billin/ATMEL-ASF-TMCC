/**
 * \file
 *
 * \brief Declaration of main function used by example
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////
/*! \brief Notify via user interface that enumeration is ok
 * This is called by TMC interface when USB Host enable it.
 *
 * \remarks
 *   After the device enumeration (detecting and identifying USB devices),
 *   the USB host starts the device configuration. When the USB TMC interface
 *   from the device is accepted by the host, the USB host enables this
 *   interface and invokes this callback function (assigned in conf_usb.h via
 *   the UDI_TMC_ENABLE_EXT() macro.  It indicates that transfers to and from
 *   the device are enabled.
 *
 * \retval true if TMC startup is successfully done
 */
bool main_tmc_enable(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief Notify via user interface that enumeration is disabled
 *
 * \remarks
 *   When the USB device is unplugged or is reset by the USB host, the USB
 *   interface is disabled and this callback function (assigned in conf_usb.h
 *   via the UDI_VENDOR_DISABLE_EXT macro) is invoked.  Calling this function
 *   disables data transfers on the TMCC device
 */
void main_tmc_disable(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief Manages the leds behaviors
 * Called when a start of frame is received on USB line each 1 millisecond
 */
void main_sof_action(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief Enters the application in low power mode
 * Callback called when USB host sets USB line in suspend state
 */
void main_suspend_action(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief Turn on a led to notify active mode
 * Called when the USB line is resumed from the suspend state
 */
void main_resume_action(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief
 *   Callback invoked to handle a request to abort a transfer on the Bulk-OUT
 *   endpoint
 *
 * \remarks
 *   This callback function is invoked when an INITIATE_ABORT_BULK_OUT
 *   request is received from the host on the Control OUT endpoint, requesting
 *   that an active or pending Bulk-OUT transfer be aborted.  It sends a
 *   TMC_initiate_abort_bulk_xfer_response_t structure back to the host to
 *   communicate status of the abort request
 */
void main_initiate_abort_bulkOUT(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief
 *   Callback invoked to handle a request to abort a transfer on the Bulk-OUT
 *   endpoint
 *
 * \remarks
 *   This callback function is invoked when a CHECK_ABORT_BULK_OUT_STATUS
 *   request is received from the host on the Control OUT endpoint, requesting
 *   that an active or pending Bulk-OUT transfer be aborted.  It sends a
 *   TMC_check_abort_bulkOUT_status_response_t structure back to the host to
 *   communicate status of the abort request
 */
void main_check_abort_bulkOUT_status(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief
 *   Callback invoked to handle a request to abort a transfer on the Bulk-IN
 *   endpoint
 *
 * \remarks
 *   This callback function is invoked when a INITIATE_ABORT_BULK_IN
 *   request is received from the host on the Control OUT endpoint, requesting
 *   that an active or pending Bulk-IN transfer be aborted.  It sends a
 *   TMC_initiate_abort_bulk_xfer_response_t structure back to the host to
 *   communicate status of the abort request
 */
void main_initiate_abort_bulkIN(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief
 *   Callback invoked to handle a request to abort a transfer on the Bulk-IN
 *   endpoint
 *
 * \remarks
 *   This callback function is invoked when a CHECK_ABORT_BULK_IN_STATUS
 *   request is received from the host on the Control OUT endpoint, requesting
 *   that an active or pending Bulk-IN transfer be aborted.  It sends a
 *   TMC_check_abort_bulkIN_status_response_t structure back to the host to
 *   communicate status of the abort request
 */
void main_check_abort_bulkIN_status(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief
 *   Callback invoked to handle a request to clear all input and output buffers
 *
 * \remarks
 *   This callback function is invoked when an INITIATE_CLEAR request is
 *   received from the host on the Control OUT endpoint, requesting.  It sends a
 *   single Byte containing a value from TMC_status_values back to the host to
 *   communicate status of the clear operation
 */
void main_initiate_clear(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief
 *   Callback invoked to handle a CHECK_CLEAR_STATUS request sent by the host to
 *   determine if all input and output buffers have been cleared
 *
 * \remarks
 *   This callback function returns a TMC_check_clear_status_response_t
 *   indicating the status of a preceding clear operation
 */
void main_check_clear_status(void);

////////////////////////////////////////////////////////////////////////////////
/*! \brief
 *   Callback function invoked when the host sends a REQ_DEV_DEP_MSG_IN message
 *   requesting that device-dependent data be sent to the BulkIN endpoint
 *
 *  \param header  Header of the REQ_DEV_DEP_MSG_IN message preceding the data
 *
 *  \return true if the request was processed; else false in case of an error
 */
bool main_req_dev_dep_msg_in_received(
                   TMC_bulkOUT_request_dev_dep_msg_in_header_t const* header );

#endif // _MAIN_H_
