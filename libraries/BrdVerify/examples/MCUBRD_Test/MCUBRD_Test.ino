/******************************************************************************

                        MCU Board Testing Software Example
  
  Copyright (C) <2013>  <www.dfrobot.com>
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 

 ******************************************************************************
  File Name     : MCUBRD_Test.ino
  Version       : Initial Draft
  Author        : Leo Yan
  Created       : 2013/12/3
  Last Modified :
  Description   : k
  Function List :
              loop
              setup
  History       :
  1.Date        : 2013/12/3
    Author      : Leo Yan
    Modification: Created file

******************************************************************************/

#include "BrdVerify.h"

/*!! if there is enough space in flash, remove the follow libraries*/
#if (VFC_I2CLCD == VFC_OUTPUT_TYPE)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#endif



/*** user define ***/
#define VFC_TOTAL_TESTTIMES 10  //Max value is uint16,  if equals 'UINT16_MAX',  it means infinite.


/*use one pin to indicate whether successs or failure.  It is better to use a digital pin.  */

#define VFC_FINAL_INDECATOR_PIN 13


/* IO loopback test para, loopback */
TstLoopBackConfig_t loopbackConfig[] =
{
  { VFC_LOOPBACK_D2A, 9, A0 },
  { VFC_LOOPBACK_D2A, 10, A1 }, 
  { VFC_LOOPBACK_D2A, 11, A2 },
  { VFC_LOOPBACK_D2D, 9, A0 },
  { VFC_LOOPBACK_D2D, 10, A1 }, 
  { VFC_LOOPBACK_D2D, 11, A2 },  
  { VFC_LOOPBACK_D2D, SDA, SCL }, 
  { VFC_LOOPBACK_D2D, 1, 0 }
};



uint8_t pinConfig[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3, A4, A5 };

TstUsartConfig_t serialConfig[] = 
{
#if defined(__AVR_ATmega32U4__)
    {Serial1, 9600},
    {Serial1, 115200},
#endif
};




/* g_TestManageTable control the testitem and sequence  */

TestItem_t g_TestManageTable[] =
{
    
    /*Automatic test, you can use only one LED to indicate the testing result.   Pass = LED lighted on,  Failed = quicky flashing.
          If using TestBed V1, you must not connect GND to TestBed.     */
    VFC_TESTITEM( "Short", standloneShortTest, pinConfig),
    VFC_TESTITEM( "Open", loopbackOpenTest, loopbackConfig),
    VFC_TESTITEM( "Serial", usartTest, serialConfig),
    
    /*liquidLedTest needs to be observed by person. If using TestBed V1, you must connect GND to TestBed*/
    VFC_TESTITEM( "Liquid", usartTest, pinConfig),
};




void setup()
{

    Verification.init( VFC_TABLE_AND_NUM(g_TestManageTable) );
   
    pinMode( 13, OUTPUT );
    digitalWrite ( 13, HIGH );
    delay( 300 );
    digitalWrite ( 13, LOW );
    
}

void loop()
{

    boolean testResult;

    /*There are two types:
            1)VFC_PASSTEST:  Passing testing.  When passing times reaches to 'VFC_PASSTEST_JUDGETIMES', the test will end.
            2)VFC_PRESSURETEST: testing until VFC_TOTAL_TESTTIMES
       */
    testResult = Verification.run( VFC_PASSTEST, VFC_TOTAL_TESTTIMES );

    Verification.showTestResultOnLED( testResult, VFC_FINAL_INDECATOR_PIN );

    while(1);

}

