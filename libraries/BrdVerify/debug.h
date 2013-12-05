





#if _DEBUG
    //ref AVRlib " assert(e)    __assert(__func__, __FILE__, __LINE__, #e))
    #define DBG_ASSERT(A) if( !(A) ) { Serial.print("ERR:F=");Serial.print(__FILE__);Serial.print(" L=");Serial.println(__LINE__); delay(1000); while(1);}
    #define DBG_WARNING(A) if( !(A) ) { Serial.print("WARNING:F=");Serial.print(__FILE__);Serial.print(" L=");Serial.println(__LINE__); delay(500);}

#else
    #define DBG_ASSERT(A)
#endif



