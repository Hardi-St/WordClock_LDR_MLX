/*

 Revision History:
 ~~~~~~~~~~~~~~~~~
 04.08.23:  - Started


 ToDo:
*/

#ifndef __WORD_CLOCK_LDR_EXTENTION__
#define __WORD_CLOCK_LDR_EXTENTION__
#if !defined(AVR) && !defined(ESP32)
  #error Platform is not supported
#endif


#include <MLLExtension.h>


//***************************************
class WordClock_LDR : public MLLExtension
//***************************************
{
  private:
      uint8_t     Pin_NumberLDR;
      uint8_t     MinBrightLEDs;
      uint8_t     MaxBrightLEDs;
      uint16_t    MinLDR;
      uint16_t    MaxLDR;
      uint32_t    LastCalc;
      uint32_t    LastUpdate;
      uint32_t    LastDisp;
      uint16_t    CalcPeriod;
      uint16_t    UpdatePeriod;
      uint16_t    DebugDispPeriod;
      float       AvgLDR;
      float       Filter;

    //---------------------------------------------------------------------------------------------------------------------------------------
    public:WordClock_LDR(uint8_t Pin_NumberLDR, uint16_t MinLDR, uint16_t MaxLDR, uint8_t MinBrightLEDs, uint8_t MaxBrightLEDs, float Filter) // Constructor
    //---------------------------------------------------------------------------------------------------------------------------------------
    {
      this->Pin_NumberLDR = Pin_NumberLDR;
      this->MinLDR        = MinLDR;
      this->MaxLDR        = MaxLDR;
      this->MinBrightLEDs = MinBrightLEDs;
      this->MaxBrightLEDs = MaxBrightLEDs;
      this->Filter        = Filter;
      LastCalc            = 0;
      LastUpdate          = 0;
      LastDisp            = 0;
      CalcPeriod          = 100;    // Period for calculating the LDR lowpass [us]
      UpdatePeriod        = 20;     // Update period for the brightness [ms]
      DebugDispPeriod     = 1000;
	}

    //-----------------------------------------
    public:void setup(MobaLedLib_C& mobaLedLib)
    //-----------------------------------------
    {
//      Serial << F("\nWordClock_LDR setup\n"); // Debug
//      Serial << F("Pin_NumberLDR:") << Pin_NumberLDR << F("\n");
//      Serial << F("MinBrightLEDs:") << MinBrightLEDs << F("\n");
//      Serial << F("MaxBrightLEDs:") << MaxBrightLEDs << F("\n");
//      Serial << F("MinLDR:")        << MinLDR        << F("\n");
//      Serial << F("MaxLDR:")        << MaxLDR        << F("\n");
      AvgLDR = -999;
	}


    //----------------------------------------
    public:void loop2(MobaLedLib_C& mobaLedLib)
    //----------------------------------------
    {
       uint32_t us = micros();
       if (us - LastCalc > CalcPeriod) // Overflow save calculation
          {
          LastCalc = us;
          if (AvgLDR == -999)
               AvgLDR = analogRead(Pin_NumberLDR);
          else AvgLDR = (1-Filter)*AvgLDR + Filter*analogRead(Pin_NumberLDR);
          }

       uint32_t t = millis();
       if (t - LastUpdate > UpdatePeriod)
          {
          LastUpdate = t;
          uint8_t Bright;
          float Fact = 0;
          if      (AvgLDR >= MinLDR)  Bright = MinBrightLEDs;
          else if (AvgLDR <= MaxLDR)  Bright = MaxBrightLEDs;
          else {
               // Default values:
               // MinLDR        = 3500
               // MaxLDR        = 1500
               // MinBrightLEDs = 20
               // MaxBrightLEDs = 255
               Fact = (float)(AvgLDR - MaxLDR) / (float)(MinLDR - MaxLDR);
               Bright = MaxBrightLEDs - Fact * (MaxBrightLEDs - MinBrightLEDs);
               }
          FastLED.setBrightness(Bright);

          #ifdef DEBUG_WORD_CLOCK_LDR
            if (t - LastDisp > DebugDispPeriod)
               {
               LastDisp = t;
               Serial << F("LDR:") << (int)AvgLDR;
             //Serial << F(" Fact:") << Fact;
               Serial << F(" Bright:") << Bright<< F("\n");
               }
          #endif
          }
    }
};


#endif // __WORD_CLOCK_LDR_EXTENTION__
