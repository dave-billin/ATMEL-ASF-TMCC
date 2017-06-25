/**
 * \file
 *
 * \brief USB Vendor class protocol definitions.
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

#ifndef _USB_PROTOCOL_TMCC_H_
#define _USB_PROTOCOL_TMCC_H_

#include "compiler.h"

/**
 * \ingroup usb_protocol_group
 * \defgroup usb_vendor_protocol USB Vendor Class definitions
 *
 * @{
 */

/**
 * \name Vendor class values
 */
//@{
#define  TMC_CLASS                  0xfe  // Application class code for USBTMC assigned by USB-IF
#define  TMC_SUBCLASS               0x03  // Subclass code assigned by USB-IF
#define  TMC_PROTOCOL               0x00
//@}


/**
 * \name USBTMC Control endpoint request ID's
 *
 * \remarks
 *   USBTMC-specific values that may appear in the bRequest field of control
 *   endpoint requests.
 *
 * \see Table 15 in USBTMC 1.00 specification Section 4.2.1 (USBTMC requests)
 */
//@{
enum TMC_control_request_ids
{
   // Reserved ID that should not be used
   TMC_CTRL_REQ_RESERVED_0 = 0,

   // Aborts a Bulk-OUT transfer
   TMC_CTRL_REQ_INITIATE_ABORT_BULK_OUT = 1,

   // Returns the status of the previously sent TMC_CTRL_REQ_INITIATE_ABORT_BULK_OUT request
   TMC_CTRL_REQ_CHECK_ABORT_BULK_OUT_STATUS = 2,

   // Aborts a Bulk-IN transfer
   TMC_CTRL_REQ_INITIATE_ABORT_BULK_IN = 3,

   // Returns the status of the previously sent TMC_CTRL_REQ_INITIATE_ABORT_BULK_IN request
   TMC_CTRL_REQ_CHECK_ABORT_BULK_IN_STATUS = 4,

   // Clears all previously sent pending and unprocessed Bulk-OUT USBTMC message
   // content and clears all pending Bulk-IN transfers from the USBTMC interface
   TMC_CTRL_REQ_INITIATE_CLEAR = 5,

   // Returns the status of the previously sent TMC_CTRL_REQ_INITIATE_CLEAR request
   TMC_CTRL_REQ_CHECK_CLEAR_STATUS = 6,

   // Returns attributes and capabilities of the USBTMC interface
   TMC_CTRL_REQ_GET_CAPABILITIES = 7,

   // NOTE: ID's 8 through 63 are marked as RESERVED

   // A mechanism to turn on an activity indicator for identification purposes
   // The device indicates whether or not it supports this request in the
   // GET_CAPABILITIES response packet
   TMC_CTRL_REQ_INDICATOR_PULSE = 64
};
//@}


/**
 * \name USBTMC Status Values
 *
 * \remarks
 *   All USBTMC class-specific requests return data to the Host (bmRequestType
 *   direction = Device-to-host) and have a data payload that begins with a 1
 *   byte USBTMC_status field. These USBTMC_status values are defined here.
 *
 * \see Table 16 in USBTMC 1.00 specification Section 4.2.1 (USBTMC requests)
 */
//@{
enum e_TMC_status_values
{
   TMC_STATUS_RESERVED_0 = 0, // Invalid reserved status

   // Success
   TMC_STATUS_SUCCESS = 1,

   // This status is valid if a device has received a USBTMC split transaction
   // CHECK_STATUS request and the request is still being processed
   TMC_STATUS_PENDING = 2,

   // Failure, unspecified reason, and a more specific USBTMC_status is not defined
   TMC_STATUS_FAILED = 0x80,

   // This status is only valid if a device has received an INITIATE_ABORT_BULK_OUT
   // or INITIATE_ABORT_BULK_IN request and the specified transfer to abort is
   // not in progress
   TMC_STATUS_TRANSFER_NOT_IN_PROGRESS = 0x81,

   // This status is valid if the device received a CHECK_STATUS request and the
   // device is not processing an INITIATE request
   TMC_STATUS_SPLIT_NOT_IN_PROGRESS = 0x82,

   // This status is valid if the device received a new class-specific request and
   // the device is still processing an INITIATE request
   TMC_STATUS_SPLIT_IN_PROGRESS = 0x83

};


// Pack all USB structures along a 1-byte boundary
COMPILER_PACK_SET(1)

//==============================================================================
/** \brief
 *   Status message sent from device to host in response to an
 *   INITIATE_ABORT_BULK_OUT or INITIATE_ABORT_BULK_IN request received from the
 *   host on the control OUT endpoint
 */
typedef struct
{
   uint8_t usbtmc_status;  ///< Status code from e_TMC_status_values

   /// The bTag for the the current Bulk IN/OUT transfer. If there is no current
   /// Bulk IN/OUT transfer, bTag must be set to the bTag for the most recent
   /// bulk IN/OUT transfer. If no Bulk IN/OUT transfer has ever been started,
   /// bTag must be 0x00
   uint8_t bTag;

} TMC_initiate_abort_bulk_xfer_response_t;



//==============================================================================
/** \brief
 *   Status message sent from device to host in response to a
 *   CHECK_ABORT_BULK_OUT_STATUS request received from the host on the control
 *   OUT endpoint
 */
typedef struct
{
   uint8_t usbtmc_status;  ///< Status code from e_TMC_status_values
   uint8_t reserved[3];    ///< Reserved field (must be set to 0x000000)

   /** Total number of USBTMC message Bytes (not including Bulk-OUT Header or
    *  alignment bytes) in the transfer received, and not discarded, by the
    *  device.  Value is reported with least significant Byte first.
    */
   uint32_t nbytes_rxd;

} TMC_check_abort_bulkOUT_status_response_t;



//==============================================================================
/** \brief
 *   Status message sent from device to host in response to a
 *   CHECK_ABORT_BULK_IN_STATUS request received from the host on the control
 *   OUT endpoint
 */
typedef struct
{
   uint8_t usbtmc_status;  ///< Status code from e_TMC_status_values
   uint8_t bmAbortBulkIn;  ///< 0 if Bulk-IN FIFO is empty; else 1 if data is still queued
   uint8_t reserved[2];    ///< Reserved field (must be set to 0x000000)

   /** Total number of USBTMC message Bytes (not including Bulk-IN Header or
    *  alignment bytes) sent in the transfer.  Value is reported with least
    *  significant Byte first.
    */
   uint32_t nbytes_txd;

} TMC_check_abort_bulkIN_status_response_t;



//==============================================================================
/** \brief
 *   Status message sent from device to host in response to a
 *   CHECK_CLEAR_STATUS request received from the host on the control OUT
 *   endpoint
 */
typedef struct
{
   uint8_t usbtmc_status;  ///< Status code from e_TMC_status_values
   uint8_t bmClear;        ///< 0 if buffers are cleared; else 1

} TMC_check_clear_status_response_t;


COMPILER_PACK_RESET()

//@}


#endif // _USB_PROTOCOL_TMCC_H_
