//==============================================================================
/*
 * @file
 *
 * @brief Implementation of routines used to acquire data from ADC's
 *
 * @author Dave Billin
 */
//==============================================================================

#include "adc_driver.h"


////////////////////////////////////////////////////////////////////////////////
void adc_initialize( void )
{
   // TODO: initialize ADC data acquisition
}

////////////////////////////////////////////////////////////////////////////////
uint32_t adc_readSamples( void* dest, uint16_t numBytes )
{
   // For now, we just simulate ADC data as incrementing values
   static uint32_t sampleValues[ADC_NUM_CHANNELS] = {0};
   static uint8_t readIndex = 0;

   uint32_t* dest32 = (uint32_t*)dest;
   uint32_t numBytesRead = 0;
   while ( numBytes >= ADC_BYTES_PER_SAMPLE )
   {
      // Read a sample into the buffer, incrementing pointer and read index
      *dest32++ = sampleValues[readIndex++];

      // Decrement remaining Bytes in dest
      numBytes -= ADC_BYTES_PER_SAMPLE;

      // Increment the number of Bytes copied
      numBytesRead += ADC_BYTES_PER_SAMPLE;

      // Wrap read index.  Increment sample value on wrap
      if ( readIndex == ADC_NUM_CHANNELS )
      {
         for (readIndex = 0; readIndex < ADC_NUM_CHANNELS; ++readIndex)
         {
            ++sampleValues[readIndex];
         }
         readIndex = 0;
      }
   }

   return numBytesRead;
}
