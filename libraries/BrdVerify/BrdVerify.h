/*-------------------------------------------------------------------------
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
-------------------------------------------------------------------------*/



/*-------------------------------------------------------------------------
1 Test Type (see 'VfcRunType_e')
        1)VFC_PASSTEST:  Passing testing.  When continuing passing times reaches to 'VFC_PASSTEST_JUDGETIMES', the test will end.
        2)VFC_PRESSURETEST: testing until VFC_TOTAL_TESTTIMES



2 Test cases is managed as follow:

    Test Manager- (see 'TestItem_t g_TestManageTable' in example)

                          |- Test Item A -- (see 'TestItem_t')
                                                --|- Test Case 1      (defined by yourself)
                                                --|- Test Case 2
                                                --|- Test Case ...
                                                --|- Test Case n

                          |- Test Item M --
                                                --|- Test Case 1
                                                --|- Test Case 2
                                                --|- Test Case ...
                                                --|- Test Case n



3 How to add member function for NEW testing item?
  1) prototype is  'typedef boolean (Verification_c::*TestItem_fun)( void * p_caseTable, uint8_t caseNum )'
  2) define struct for Test Case
  3) add function declaration in the Class Verification_c
  4) implement the function
-------------------------------------------------------------------------*/


#ifndef _VERIFICATION_H 
#define _VERIFICATION_H


#include "stdint.h"
#include "arduino.h"


#define VFC_SERIAL 0
#define VFC_I2CLCD 1

/**user define,  the I2CLCD libarary will need more flash space**/
#define VFC_OUTPUT_TYPE VFC_SERIAL   //VFC_I2CLCD  VFC_SERIAL;  If no output, clear it

#define VFC_LCD_ADDR 0x20
#define VFC_LCD_COL  20 
#define VFC_LCD_ROW  4 

/** inner define **/
#define VFC_PASSTEST_JUDGETIMES 5
#define VFC_TESTITEM_OUTPUTINFO_LENGTH 16




typedef enum
{
    VFC_PASSTEST,   
    VFC_PRESSURETEST    
}VfcRunType_e;



class Verification_c;
extern Verification_c Verification;


typedef boolean (Verification_c::*TestItem_fun)( void * p_caseTable, uint8_t caseNum );
typedef struct
{
    const char *name;
    TestItem_fun p_ItemFunction;
    void *p_para;
    uint8_t caseNum;
}TestItem_t;

#define VFC_TABLE_AND_NUM(table) table, sizeof(table)/sizeof(table[0])
#define VFC_TESTITEM( name, function, configTable ) { name, &Verification_c::function, VFC_TABLE_AND_NUM(configTable)}



class Verification_c 
{

        
    public:
      
        Verification_c( );
        
        void init( TestItem_t *p_ItemTablePara, uint8_t ItemNumPara );
              
        boolean run( VfcRunType_e testType , uint16_t totalTimesPara );

        //boolean saveInfo( uint16_t startAddress );

        
        void showTestResultOnLED( boolean result, uint8_t pin );
        void printCaseErrInfo( uint8_t Index, const char *info );

        boolean liquidLedTest( void * const p_config, uint8_t configNum );

        boolean standloneShortTest( void * const p_config, uint8_t configNum );
        boolean loopbackOpenTest( void * const p_config, uint8_t configNum );
        boolean usartTest( void * constp_config, uint8_t configNum );
        /***   ***/
        
        /***   ***/

        
        
    private:
      
        TestItem_t *p_testItemTable;
        uint8_t testItemNum;
        uint8_t currentTestItem;
        uint16_t testTotalTimes;
        uint16_t currentTestTimes;

        uint8_t rsltIndicatorPin;
        
        uint8_t loopbackD2DTest( uint8_t pin1, uint8_t pin2 );

        //void storeTestInfo( uint8_t  );
        
        //void printStoredTestInfo( uint8_t  );

};



/****************************************** 

******************************************/

/**** liquidLedTest  ****/


/**** standloneShortTest  ****/



/**** loopbackOpenTest  ****/
#define TST_ANALOG_MAX_VALUE  0x2FF
#define TST_ANALOG_MIN_VALUE  0x0
#define TST_ANALOG_TOLERANCE  0xA5  // 0.8V/5V * TST_ANALOG_MAX_VALUE

typedef enum
{
    VFC_LOOPBACK_D2D,
    VFC_LOOPBACK_D2A,
    VFC_LOOPBACK_33V2A,   
    VFC_LOOPBACK_50V2A,
}TestType_enum;

typedef struct 
{
    TestType_enum type;
    uint8_t pinA;
    uint8_t pinB;   
}TstLoopBackConfig_t;


/**** usartTest  ****/
typedef struct 
{
    HardwareSerial &tstSerial;
    uint32_t baudRate;
}TstUsartConfig_t;



#endif
