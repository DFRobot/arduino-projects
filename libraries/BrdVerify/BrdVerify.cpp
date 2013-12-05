/******************************************************************************

                        MCU Board Testing Software
  
  Copyright (C) <2013>  <Leo Yan>
  
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
  
  Contact: leo.yan.cn@gmail.com

 ******************************************************************************
  File Name     : BrdVerify.cpp
  Version       : Initial Draft
  Author        : Leo Yan
  Created       : 2013/12/3
  Last Modified :
  Description   : Can be used on AVR board.   The library is depend on Arduino environment. 
  Function List :
    0-Use Case management(done)
    1-Liquid LED Test (done)          
    2-Short circuit Test (done)          
    3-Open circuit Test (done)          
    4-USART Test  (done)          
    5-Output information to Serial     (done)          
    6-Output information to I2C LCD  (done,  should modify the macro of library)          
    7-Record Error information to EEPROM  (Planning)          

******************************************************************************/

    
#include "debug.h" 
#include "stdio.h"
#include "BrdVerify.h"


#if (VFC_SERIAL == VFC_OUTPUT_TYPE)

#define VFC_OUTPUT Serial

#elif (VFC_I2CLCD == VFC_OUTPUT_TYPE)
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C Lcd(VFC_LCD_ADDR, VFC_LCD_COL, VFC_LCD_ROW); 
#define VFC_OUTPUT Lcd

#else


#endif



Verification_c Verification;

/*****************************************************************************
 Prototype    : Verification_c.Verification_c
 Description  : initilize
 Input        :   
 Output       : None
 Return Value : 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
Verification_c::Verification_c( )   
{
	rsltIndicatorPin = 0xFF;
	p_testItemTable = NULL;
	testItemNum = 0;
}

void Verification_c::init( TestItem_t *p_ItemTablePara, uint8_t ItemNumPara )
{
	
    DBG_ASSERT( NULL != p_ItemTablePara );    
	
	p_testItemTable = p_ItemTablePara;
	testItemNum = ItemNumPara;	

#if (VFC_OUTPUT_TYPE == VFC_SERIAL)
    VFC_OUTPUT.begin(9600);
    while(!VFC_OUTPUT);
#elif (VFC_OUTPUT_TYPE == VFC_I2CLCD)

    VFC_OUTPUT.init();         
    VFC_OUTPUT.backlight();

    VFC_OUTPUT.home();
  
    VFC_OUTPUT.setCursor(0, 0);
#else

#endif


    
}


/*****************************************************************************
 Prototype    : Verification_c.run
 Description  : execute Test Case
 Input        : VfcRunType_e testType    
                   uint16_t totalTimesPara  
 Output       : None
 Return Value : boolean,  if all test cases success, return 'true'
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
boolean Verification_c::run( VfcRunType_e testType, uint16_t totalTimesPara )
{

    TestItem_t *p_testItem = NULL;
    
    const char * name;
    TestItem_fun p_ItemFunction;
    void *p_para;  
    uint8_t caseNum;
    boolean Result;

    uint8_t continueCount;

    testTotalTimes = ( ( 0 <= totalTimesPara) && (totalTimesPara < VFC_PASSTEST_JUDGETIMES) ) ? \
                          VFC_PASSTEST_JUDGETIMES : totalTimesPara;

    /*execute whole test for specified times*/
    continueCount = 0;
    
    for ( currentTestTimes = 0; currentTestTimes <= testTotalTimes; currentTestTimes++ )
    {

#ifdef VFC_OUTPUT_TYPE
        delay(10);
        VFC_OUTPUT.print( "== No." );
        VFC_OUTPUT.println( currentTestTimes );
#endif        
        p_testItem = p_testItemTable;
        
        for ( currentTestItem = 0; currentTestItem < testItemNum; currentTestItem++, p_testItem++ )
        {

            name = p_testItem->name;
            p_ItemFunction = p_testItem->p_ItemFunction;
            p_para = p_testItem->p_para;
            caseNum = p_testItem->caseNum;

#ifdef VFC_OUTPUT_TYPE
            VFC_OUTPUT.print( " Test:" );
            VFC_OUTPUT.print( name );
            VFC_OUTPUT.print( ". case=" );
            VFC_OUTPUT.println( caseNum );
#endif        


            Result = (this->*p_ItemFunction)( p_para, caseNum );

            if( !Result )
            {
                continueCount = 0;
            }

        }

        if ( VFC_PASSTEST == testType )
        {
            continueCount++;
            
            if ( continueCount >= VFC_PASSTEST_JUDGETIMES )
            {
                break;
            }
        }

        
        /*To show info  every */
        if ( 0xFF == (currentTestTimes & 0xFF) )
        {
            
        }
        
    }

    
    if ( VFC_PASSTEST == testType )
    {
        
#ifdef VFC_OUTPUT_TYPE
        VFC_OUTPUT.print("**Test ");
#endif
        if ( continueCount >= VFC_PASSTEST_JUDGETIMES ) 
        {
#ifdef VFC_OUTPUT_TYPE
            VFC_OUTPUT.println("PASS:) **");
#endif
            return true;
        }
        else
        {
#ifdef VFC_OUTPUT_TYPE
            VFC_OUTPUT.println("Failed! **");
#endif
            return false;
        }

    }
    else
    {
        
#ifdef VFC_OUTPUT_TYPE
        VFC_OUTPUT.println("** Test finished. **");
#endif
        return true;
    }

}


/*****************************************************************************
 Prototype    : Verification_c.showTestResultOnLED
 Description  : show tested result by a LED
 Input        : boolean result:  if 'result' == true,  the LED will keep light up.   Otherwise, quickly flashed
                uint8_t pin     
 Output       : None
 Return Value : void
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
void Verification_c::showTestResultOnLED( boolean result, uint8_t pin )
{
    
    boolean ledState = LOW;

    if ( result )
    {
        digitalWrite( pin, HIGH );
    }
    else
    {
        while (1)
        {
            digitalWrite( pin, ledState );
            ledState = !ledState;
            delay(100);
        }
    }
    
}



/*****************************************************************************
 Prototype    : Verification_c.printCaseErrInfo
 Description  : Output error info when testing
 Input        : uint8_t index     
                   const char *info  
 Output       : None
 Return Value : void
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
void Verification_c::printCaseErrInfo( uint8_t index, const char *info )
{

#ifdef VFC_OUTPUT_TYPE

    TestItem_t *p_testItem = NULL;
    const char *name;

    p_testItem = p_testItemTable + currentTestItem;
    
    name = p_testItem->name;
    //uint8 &failedTimes = p_testItem->failedTimes;
   
    VFC_OUTPUT.print("  Failed= ");
    VFC_OUTPUT.print(index);
    VFC_OUTPUT.print(" - ");
    VFC_OUTPUT.println( info ); 
    
#endif

}


uint8_t Verification_c::loopbackD2DTest( uint8_t pin1, uint8_t pin2 )
{
    uint8_t result;

    result = 0;
    
    pinMode( pin1, OUTPUT );
    pinMode( pin2, INPUT );

    /*testing*/
    digitalWrite( pin1, HIGH );
    result += ( HIGH == digitalRead( pin2 ) ) ? 0 : 1;

    digitalWrite( pin1, LOW ); 
    result += ( LOW == digitalRead( pin2 ) ) ? 0 : 1;
                         
    /*recovery*/
    pinMode( pin1, INPUT );

    return result;

}


/*****************************************************************************
 Prototype    : Verification_c.loopbackOpenTest
 Description  : Open circuit test.    You should connect every tow IOs together.  
 Input        : void * const p_config,  pionted to 'TstLoopBackConfig_t'
                   uint8_t configNum,  the number of Test Case      
 Output       : None
 Return Value : true = success
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
boolean Verification_c::loopbackOpenTest( void * const p_config, uint8_t configNum )
{
    uint8_t i;
    TestType_enum type;
    uint8_t pinA;
    uint8_t pinB;
    uint8_t TotalErrNum;
    uint8_t Result;

    char a_outInfo[VFC_TESTITEM_OUTPUTINFO_LENGTH + 1];

    const TstLoopBackConfig_t *p_record = NULL;

    TotalErrNum = 0;     
    
    /**Check open circuit **/   
    for( i = 0, p_record = (TstLoopBackConfig_t *)p_config; i < configNum; i++, p_record++ )
    {
        
        type = p_record->type;
        pinA = p_record->pinA;
        pinB = p_record->pinB;
        
        Result = 0;

       
        switch ( type )
        {
            case VFC_LOOPBACK_D2D:

              Result += loopbackD2DTest( pinA, pinB );
              Result += loopbackD2DTest( pinB, pinA );
              
              break;
            
            case VFC_LOOPBACK_D2A:
              pinMode( pinA, OUTPUT );
              
              /*testing, it is also confirm whether the AVCC is right */
              digitalWrite( pinA, HIGH );
              //todo: delay?
              Result += ( TST_ANALOG_MAX_VALUE < (analogRead( pinB ) + TST_ANALOG_TOLERANCE) ) ? 0 : 1;
             
              digitalWrite( pinA, LOW ); 
              Result += ( ( TST_ANALOG_MIN_VALUE + TST_ANALOG_TOLERANCE ) > analogRead( pinB ) ) ? 0 : 1;
              
                         
              /*recovery default config*/
              pinMode( pinA, INPUT );          
              break; 
 
            default:
              DBG_ASSERT( 0 );
                
        }
        
        if ( Result > 0 )
        {
            snprintf( a_outInfo, VFC_TESTITEM_OUTPUTINFO_LENGTH, "%u or %u", pinA, pinB );
            printCaseErrInfo( i, a_outInfo );
            TotalErrNum++;
        } 

    }

    
    return ( 0 == TotalErrNum );

}




/*****************************************************************************
 Prototype    : Verification_c.standloneShortTest
 Description  : Short circuit test.  IOs can kept open.
 Input        : void * const p_config,  pointed to a array stored IOs what you want to test.   
                   uint8_t configNum      
 Output       : None
 Return Value : boolean
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
boolean Verification_c::standloneShortTest( void * const p_config, uint8_t configNum )
{
    uint8_t i;
    uint8_t TotalErrNum;
    uint8_t Result;
    char a_outInfo[VFC_TESTITEM_OUTPUTINFO_LENGTH + 1];
	uint8_t *p_pin = NULL; 

    /**Check short circuit **/

    /*Set all OUT to GND*/
    for( i = 0, p_pin = (uint8_t *)p_config; i < configNum; i++, p_pin++ )
    {
        digitalWrite( *p_pin, LOW );
        pinMode( *p_pin, OUTPUT );
    }

    /*Set OUT to tristate and IN to pullup state.  than read the IN .  if set the OUT to LOW, there will be short between VCC and GND*/
    TotalErrNum = 0;
    
    for( i = 0, p_pin = (uint8_t *)p_config; i < configNum; i++, p_pin++ )
    {       
        pinMode( *p_pin, INPUT );
        pinMode( *p_pin, INPUT_PULLUP );

        Result = ( HIGH == digitalRead( *p_pin ) ) ? 0 : 1;
        
      
        if ( Result > 0 )
        {
            snprintf( a_outInfo, VFC_TESTITEM_OUTPUTINFO_LENGTH ,"%u", *p_pin );
            printCaseErrInfo( i, a_outInfo );
            TotalErrNum++;
        } 

        digitalWrite( *p_pin, LOW );
        pinMode( *p_pin, OUTPUT );
        
    }

    /*recovery default config*/
    for( i = 0, p_pin = (uint8_t *)p_config; i < configNum; i++, p_pin++ )
    {
        pinMode( *p_pin, INPUT );
    }

    return ( 0 == TotalErrNum );

}



/*****************************************************************************
 Prototype    : Verification_c.liquidLedTest
 Description  : Liquid LED test, the method needs to be observed by person.  It cannot be used as automatic testing.  
 Input        : void * const p_config  
                  uint8_t configNum      
 Output       : None
 Return Value :  always 'true' 
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
boolean Verification_c::liquidLedTest( void * const p_config, uint8_t configNum )
{
    uint8_t i;
	uint8_t *p_pin = NULL; 

    /*Set all OUT to GND*/
    for( i = 0, p_pin = (uint8_t *)p_config; i < configNum; i++, p_pin++ )
    {
        digitalWrite( *p_pin, LOW );
        pinMode( *p_pin, OUTPUT );
    }

    
    for( i = 0, p_pin = (uint8_t *)p_config; i < configNum; i++, p_pin++ )
    {       
        pinMode( *p_pin, OUTPUT );

        digitalWrite( *p_pin, HIGH );
		delay(200);
		
        digitalWrite( *p_pin, LOW );
        pinMode( *p_pin, INPUT );
    }


    /*recovery default config*/
    for( i = 0, p_pin = (uint8_t *)p_config; i < configNum; i++, p_pin++ )
    {
        pinMode( *p_pin, INPUT );
    }

    return true;

}





/*****************************************************************************
 Prototype    : Verification_c.usartTest
 Description  : Hardware usart test,  you must connect RXn and TXn together by a wire or a resistor. If using high speed, 
 Input        : void * const p_config, pointed to 'TstUsartConfig_t'  
                   uint8_t configNum      
 Output       : None
 Return Value : boolean
 Calls        : 
 Called By    : 
 
  History        :
  1.Date         : 2013/12/3
    Author       : Leo Yan
    Modification : Created function

*****************************************************************************/
boolean Verification_c::usartTest( void * const p_config, uint8_t configNum )
{
    char dataBuf[] = { 'D','F','R','o','b','o','t','\0' };
    char *p_dataBuf = dataBuf;
    char oneData;
    uint8_t sendNum, receiveNum;
	uint8_t i;
    boolean retVal;

    const TstUsartConfig_t *p_usartConfig = (TstUsartConfig_t *)p_config;

    retVal = true;

    for ( i = 0; i < configNum; i++, p_usartConfig++ )
    {
        HardwareSerial &TestSerial = p_usartConfig->tstSerial;
		uint32_t baudRate = p_usartConfig->baudRate;
       
        TestSerial.end();
        TestSerial.begin(baudRate);
        
        sendNum = TestSerial.print( p_dataBuf );
        TestSerial.setTimeout(100);
        
        for ( uint8_t i = 0; i < sendNum; i++ )
        {
            receiveNum = TestSerial.readBytes( &oneData, 1 );
            
            if ( 1 == receiveNum )
            {
                if ( dataBuf[i] != oneData )
                {
                    printCaseErrInfo( i, "Invalid Data" );
                    retVal = false;
                    break;
                }
            }
            else
            {
                printCaseErrInfo( i, "Can't Receive" );
                retVal = false;
                break;
            }
        }

		/*restore default configuration*/
        TestSerial.setTimeout(1000);
        TestSerial.end();
    }
    
    return retVal;
   
}




