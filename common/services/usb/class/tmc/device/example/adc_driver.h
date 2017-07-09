//==============================================================================
/*
 * @file
 *
 * @brief Constants and function prototypes used to acquire data from ADC's
 *
 * @author Dave Billin
 */
//==============================================================================
#ifndef _ADC_DRIVER_H_

#include <stdint.h>

/// The rate at which ADC samples are acquired
#define ADC_SAMPLES_PER_SECOND   2000

/// The number of ADC channels
#define ADC_NUM_CHANNELS         6

/// The number of Bytes in each ADC sample
#define ADC_BYTES_PER_SAMPLE     sizeof(uint32_t)



////////////////////////////////////////////////////////////////////////////////
/// \brief Initializes the driver
///
void adc_initialize( void );


////////////////////////////////////////////////////////////////////////////////
/// \brief Reads acquired ADC sample data into a buffer
///
/// \param dest   Buffer to copy sample data to
///
/// \param numBytes  Maximum number of Bytes of sample data to copy to dest
///
/// \return  The number of Bytes of sample data copied to dest
///
uint32_t adc_readSamples( void* dest, uint16_t numBytes );


#endif   // END include guard
