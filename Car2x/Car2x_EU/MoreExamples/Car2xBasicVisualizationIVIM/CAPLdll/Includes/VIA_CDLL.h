/*----------------------------------------------------------------------------
|
|  File Name: VIA_CDLL.h 
|
|  Comment:   External interface for CANoe\CANalyzer CAPL DLLs (CDLL), that 
|             makes some of the VIA APIs available also for capl dlls                            
|
|-----------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|-----------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     -------------------------------------    
| As           Stefan  Albus             Vector Informatik GmbH  

|-----------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-----------------------------------------------------------------------------
| Date         Ver  Author  Description
| ----------   ---  ------  --------------------------------------------------
| 2003-08-27   1.0   As     Creation
| ...
| sometime     2.0          By definition version 2.0 will be the first release 
| in the                    of VIA_CDLL interface, that is not longer compatible  
| future                    with version 1.x 
|                           
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
| Copyright (c) 1994 - 2003 by Vector Informatik GmbH.  All rights reserved.
-----------------------------------------------------------------------------*/



#ifndef VIA_CDLL_H
#define VIA_CDLL_H


#ifndef VIA_H
  #include "VIA.h"
#endif


// ============================================================================
// Version of the interfaces, which are declared in this header
// See  REVISION HISTORY for a description of the versions
// ============================================================================


const int VIACDLLMajorVersion = 1;
const int VIACDLLMinorVersion = 0;


// ============================================================================
// An object of class VIACapl represents a CAPL programs. By use of this 
// object, you can acess the functions, that are implemeted in a CAPL programm.
//
// While the measurement is running, every loaded capl program is identified by
// a unique handle (VIACaplHandle). The CAPL programmer gets this handle by a
// call of the capl function 'registerCAPLDll', whereas the DLL programmer gets
// is from the VIACapl when the C-fucntion 'VIARegisterCDLL' is called.
// ============================================================================



typedef uint32 VIACaplHandle;     // unique handle of a capl program


class VIACapl
{
public:
   //  Get the version of this interface object (VIACDLLMajorVersion and 
    // VIACDLLMinorVersion).
   VIASTDDECL GetVersion (int32* major, int32* minor) =0;

   // 
   VIASTDDECL GetCaplHandle (uint32* handle) =0;

   // Get a CAPL function handle. The handle stays valid until end of
   // measurement or a call of ReleaseCaplFunction. 
   VIASTDDECL GetCaplFunction(VIACaplFunction** caplfct,     // the created handle
                              const char* functionName) =0;  // name of the function

   // Release the CAPL function handle received by 'GetCaplFunction' (see above)
   // Call this method at or before the end of the measurement for every CAPL 
   // function handle, that you have asked for.
   VIASTDDECL ReleaseCaplFunction(VIACaplFunction* caplfct) =0;
};



// ==========================================================================
// VIARegisterCDLL, a setup function that must be implemented by every CAPL 
// DLL, that whats to use the services of 'VIA_CDLL.h'
//
// The following C-function is for initializing of the DLL. It is called 
// when a CAPL Program executes the funktion 'registerCAPLDll'.
// ==========================================================================


VIACLIENT(void) VIARegisterCDLL (VIACapl* service);
#endif

