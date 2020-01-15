/*--------------------------------------------------------------------
       CDLL.H     "CAPL DLL DEFINITION"
                   HEADER for compiling CAPL DLLs

                   THIS HEADER MUST NOT BE MODIFIED BY THE USER
  --------------------------------------------------------------------
       Copyright (C) 1994-2003 Vector Informatik GmbH, Stuttgart

       Function:      Header for Dynamic CAPL Functions
  --------------------------------------------------------------------*/
#ifndef __CDLL_H
#define __CDLL_H

#if !defined ( STRICT )
  #define STRICT
#endif
#include <windows.h>

#include <pshpack1.h> // change structure alignment to 1 byte

#ifndef FAR
 #define FAR far
#endif

#define CDLL_VERSION 2
#define CDLL_VERSION_NAME "CDLL_VERSION"
#define CDLL_EXPORT "CDLL_EXP"
#define CDLL_IMPORT "CDLL_IMP"

// Für CAPL-aufgerufene Funktionen brauchen wir einen verläßlichen Modifizierer
// Dieser scheint mit _pascal gegeben: Teste und verwende ihn dann!
#ifdef _pascal
#error _pascal ist anderweitig gesetzt. CAPL- Bindung damit nicht moeglich
#endif
// wenn _pascal nicht anderweitig gesetzt ist, dann geht der folgende define
// Problem könnte noch sein, wenn _pascal später noch umdefiniert wird
// -> Ein Nachtest in den Implementierungsfiles wÄre sinnvoll
#ifdef _MSC_VER
  #if defined(_WIN32) && !defined(_Windows)
    #define _Windows
  #endif
  #if defined(_WIN32) && !defined(__WIN32__)
    #define __WIN32__
  #endif
  #define CAPLPASCAL  __stdcall
  #define CAPLEXPORT  __declspec(dllexport)
#else
#define CAPLPASCAL _pascal
#define CAPLEXPORT _export
#endif

// Macro for static checks
#if defined ( __cplusplus)
  #if defined ( __BORLANDC__ )
    #define VSTATIC_ERROR(check, cond)\
    template <bool> struct condition_##check; \
    struct condition_##check<false> {}; \
    struct check : public condition_##check<cond>{}
  #else
    #define VSTATIC_ERROR(check, cond)\
    template <bool> struct condition_##check; \
    template <> struct condition_##check<false> {}; \
    struct check : public condition_##check<cond>{}
  #endif
#else
  #define VSTATIC_ERROR(check, cond)
#endif

// Macro to store argument in EAX for function returns
// Note: Modern C/C++ compilers store the return value of functions in the EAX
// register anyway, so the use of the macro is not necessary any more
#ifdef _MSC_VER

#if (_MSC_VER >= 1200)
#define MOVE_EAX(value)
#else
#define MOVE_EAX(value) __asm { mov EAX, (value) } 
#endif

#else
#define MOVE_EAX(value) (_EAX = (value))
#endif

/*---------------------------------------------------
 CAPL_FARCALL : type of a function that is exported
   to CAPL. The return type an parameters are
   ignored. Important are the qualifiers
      CAPLEXPORT far CAPLPASCAL
 ---------------------------------------------------*/
#ifdef _MSC_VER
typedef void CAPLEXPORT far  (CAPLPASCAL *CAPL_FARCALL)();
#else
typedef void CAPLEXPORT far CAPLPASCAL (*CAPL_FARCALL)();
#endif

/*---------------------------------------------------
 CAPL_DLL_CALLCONV : Enum to define calling 
   convention used in the DLL
 ---------------------------------------------------*/
enum CAPL_DLL_CALLCONV {
  CAPL_DLL_PASCAL = 0x0000, // Bit 0 == 0
  CAPL_DLL_CDECL  = 0x0001, // Bit 0 == 1
  //
  CAPL_DLL_CAN_MSG_VER_0 = 0x0000, // Binary 0000 0000 0000 0000, used for old CAN messages, CANwin <= 3.2
  CAPL_DLL_CAN_MSG_VER_1 = 0x0002, // Binary 0000 0000 0000 0010, used for CAN messages with additional 64 bit timestamps, CANwin >= 4.0
  CAPL_DLL_CAN_MSG_VER_2 = 0x0004, // Binary 0000 0000 0000 0100, CANwin >= 6.0
  CAPL_DLL_CAN_MSG_VER_3 = 0x0006, // Binary 0000 0000 0000 0110, CANwin >= 7.1
  CAPL_DLL_CAN_MSG_VER_4 = 0x0008, // Binary 0000 0000 0000 1000, used for CAN message version 4: new members frameLen, startOfFrame, CANwin >= 7.2 SP3
  CAPL_DLL_CAN_MSG_VER_5 = 0x000A, // Binary 0000 0000 0000 1010, for later use (CAN message version 5)
  CAPL_DLL_CAN_MSG_VER_6 = 0x000C, // Binary 0000 0000 0000 1100, for later use (CAN message version 6)
  CAPL_DLL_CAN_MSG_VER_7 = 0x000E, // Binary 0000 0000 0000 1110, for later use (CAN message version 7)
};

/*---------------------------------------------------
 CAPL_DLL_INFO (CAPL_DLL_INFO2, CAPL_DLL_INFO3, CAPL_DLL_INFO4): Structure for description of one
   to CAPL exported function
 ---------------------------------------------------*/
#define MAX_CDL_NAME 20
// Maximum number of parameters of a CAPL function, valid for CANoe versions <= 8.0 SPx
#define MAXCAPLFUNCPARS 10

 // force byte alignment for table
struct VCdllAlignmentTest1 {
  char f[1];
  unsigned int i;
};

#ifdef __BCPLUSPLUS__
#pragma option  -a-
#pragma warn -pin
#endif

struct VCdllAlignmentTest2 {
  char f[1];
  unsigned int i;
};

#ifdef __WIN32__
VSTATIC_ERROR(allignmentCheck1, sizeof(VCdllAlignmentTest1) != 5);
VSTATIC_ERROR(allignmentCheck2, sizeof(VCdllAlignmentTest2) != 5);
#else
VSTATIC_ERROR(allignmentCheck1, sizeof(VCdllAlignmentTest1) != 3);
VSTATIC_ERROR(allignmentCheck2, sizeof(VCdllAlignmentTest2) != 3);
#endif

typedef struct CAPL_DLL_INFO{
  char              cdlName[MAX_CDL_NAME];     // name of the function
  CAPL_FARCALL      adr;                       // address of the function
  char              resultType;                // type of func result
  int               parCount;                  // no. of parameters
  char              parTypes[MAXCAPLFUNCPARS]; // types of parameters
  unsigned char     array[MAXCAPLFUNCPARS];    // depth of array. Without: \0
} CAPL_DLL_INFO;

// New struct supporting function names with up to 50 characters
#define MAX_CDL_NAME2 50
typedef struct CAPL_DLL_INFO2{
  char              cdlName[MAX_CDL_NAME2];    // name of the function
  CAPL_FARCALL      adr;                       // address of the function
  char              resultType;                // type of func result
  int               parCount;                  // no. of parameters
  char              parTypes[MAXCAPLFUNCPARS]; // types of parameters
  unsigned char     array[MAXCAPLFUNCPARS];    // depth of array. Without: \0
} CAPL_DLL_INFO2;

// New struct supporting parameter names
typedef struct CAPL_DLL_INFO3{
  char              cdlName[MAX_CDL_NAME2];    // name of the function
  CAPL_FARCALL      adr;                       // address of the function
  const char*       categoryName;              // name of function category
  const char*       hintText;                  // hint text for function selection
  char              resultType;                // type of func result
  int               parCount;                  // no. of parameters
  char              parTypes[MAXCAPLFUNCPARS]; // types of parameters
  unsigned char     array[MAXCAPLFUNCPARS];    // depth of array. Without: \0
  const char*       parNames[MAXCAPLFUNCPARS]; // names of parameters
} CAPL_DLL_INFO3;

// Maximum number of parameters of a CAPL function, valid for CANoe versions >= 8.1 Main release
#define MAXCAPLFUNCPARS_8_1 64
// New struct supporting up to 64 parameters
typedef struct CAPL_DLL_INFO4{
  char              cdlName[MAX_CDL_NAME2];        // name of the function
  CAPL_FARCALL      adr;                           // address of the function
  const char*       categoryName;                  // name of function category
  const char*       hintText;                      // hint text for function selection
  char              resultType;                    // type of func result
  int               parCount;                      // no. of parameters
  char              parTypes[MAXCAPLFUNCPARS_8_1]; // types of parameters
  unsigned char     array[MAXCAPLFUNCPARS_8_1];    // depth of array. Without: \0
  const char*       parNames[MAXCAPLFUNCPARS_8_1]; // names of parameters
} CAPL_DLL_INFO4;

enum VXMLTableEntryType
{
  TET_Pattern = 1,
  TET_Check   = 2,
  TET_PrivCheckControl = 3,
  TET_PrivCheckQuery = 4
};

typedef struct CAPL_DLL_XML_INFO
{
  VXMLTableEntryType  entryType;
  const char*         patternName;
  CAPL_FARCALL        invocationFunction;         // Invocation-function
  CAPL_FARCALL        compilationFunction;        // Compilation-function
  char                resultType;                 //
  int                 parCount;                   // no. of parameters
  char                parTypes[MAXCAPLFUNCPARS];  // types of parameters
  unsigned char       array[MAXCAPLFUNCPARS];     // depth of array. Without: \0
} CAPL_DLL_XML_INFO;

#ifdef __BCPLUSPLUS__
#pragma option  -a.
#endif

/*------------------------------------------------------------------------

Since CANoe / CANalyzer 6.0 SP2, you may export the DLL function tables
as variables instead of using caplDllGetTable4 / caplDllGetCallbackTable4.

You should do that if the compiler shall not attempt to load additional 
modules referenced by your DLL or to execute code in your DLL. Examples are
DLLs for Windows CE or DLLs which need special driver DLLs. Technically, the
compiler will load the DLL using the Windows function LoadLibraryEx and the
parameter DONT_RESOLVE_REFERENCES.

Export your function table with the name caplDllTable4/3/2 and your callback table
with the name caplDllCallbackTable4/3/2.

Example (in the implementation file):

CAPL_DLL_INFO4 table[] = {

{CDLL_VERSION_NAME, (CAPL_FARCALL)CDLL_VERSION, "", "", CAPL_DLL_CDECL, 0xabcd, CDLL_EXPORT },

{"HelloWorld", (CAPL_FARCALL) HelloWorld, "Hello World", "Copies 'Hello, World!' to a buffer", 
'L', 2, "CD", "\001\000", {"buffer", "bufferSize"} },

{0, 0}
};
CAPLEXPORT CAPL_DLL_INFO4 far * caplDllTable4 = table;

Be careful to define all elements of the table statically, i.e., none of them
may be calculated at runtime. In particular, do not use other runtime-initialized
variables (e.g. variables of type char*) in the table definition.

If the variable caplDllTable4 is not found, the compiler will reload the DLL
using LoadLibrary and call the exported functions if the library is for Win32. 
If the library is for Windows CE, the compiler will call the exported functions
without reloading the DLL.

----------------------------------------------------------------------------*/

#ifdef  __cplusplus
extern "C" {
#endif

  extern CAPLEXPORT CAPL_DLL_INFO4 far * caplDllTable4;
  extern CAPLEXPORT CAPL_DLL_INFO4 far * caplDllCallbackTable4;

  extern CAPLEXPORT CAPL_DLL_INFO3 far * caplDllTable3;
  extern CAPLEXPORT CAPL_DLL_INFO3 far * caplDllCallbackTable3;

  extern CAPLEXPORT CAPL_DLL_INFO2 far * caplDllTable2;
  extern CAPLEXPORT CAPL_DLL_INFO2 far * caplDllCallbackTable2;

#ifdef  __cplusplus
}
#endif

// parameter and function result types:
//      V       void
//      L       long
//      D       unsigned long
//      I       int             only in connection with array size != 0
//      W       unsigned int    only in connection with array size != 0
//      B       unsigned char   only in connection with array size != 0
//      C       char            only in connection with array size != 0
//      F       float (means double in 8 Byte 80387 format)
//      0xabcd  sign for valid table

/*---------------------------------------------------
 CAPL_DLL_INFO_LIST : the list of all to CAPL exported
   functions. The list ends with the first occurrence
   of name = NULL.
   This array is only visible inside the DLL!
 ---------------------------------------------------*/
extern CAPL_DLL_INFO CAPL_DLL_INFO_LIST[];
extern CAPL_DLL_INFO2 CAPL_DLL_INFO_LIST2[];
extern CAPL_DLL_INFO3 CAPL_DLL_INFO_LIST3[];

/*---------------------------------------------------
 caplDllGetTable, caplDllGetTable2: Used to export the list of
   dynamic info to the application (loader).
   CAPL_DLL_GETTABLE (CAPL_DLL_GETTABLE2) is the type of that function.
   Of the same type is caplDllGetCallbackTable. But
   this function is optional. Only DLLs which need
   callbacks, have to implement that function. The
   function may return a NULL table.
 ---------------------------------------------------*/
#ifdef  __cplusplus
extern "C"
{
#endif
#ifdef _MSC_VER
  unsigned long CAPLEXPORT __cdecl caplDllGetTable(void);
  unsigned long CAPLEXPORT __cdecl caplDllGetTable2(void);
  unsigned long CAPLEXPORT __cdecl caplDllGetTable3(void);
  unsigned long CAPLEXPORT __cdecl caplDllGetTable4(void);
  unsigned long CAPLEXPORT __cdecl XMLDLLGetTable(void);
#else
  CAPL_DLL_INFO far * CAPLEXPORT far pascal caplDllGetTable(void);
  CAPL_DLL_INFO2 far * CAPLEXPORT far pascal caplDllGetTable2(void);
  CAPL_DLL_INFO3 far * CAPLEXPORT far pascal caplDllGetTable3(void);
  CAPL_DLL_INFO4 far * CAPLEXPORT far pascal caplDllGetTable4(void);
  CAPL_DLL_XML_INFO far * CAPLEXPORT far pascal XMLDLLGetTable(void);
#endif
#ifdef  __cplusplus
}
#endif
#ifdef  __cplusplus
extern "C"
{
#endif
#ifdef _MSC_VER
CAPLEXPORT CAPL_DLL_INFO far * far CAPLPASCAL caplDllGetCallbackTable(void);
CAPLEXPORT CAPL_DLL_INFO2 far * far CAPLPASCAL caplDllGetCallbackTable2(void);
CAPLEXPORT CAPL_DLL_INFO3 far * far CAPLPASCAL caplDllGetCallbackTable3(void);
CAPLEXPORT CAPL_DLL_INFO4 far * far CAPLPASCAL caplDllGetCallbackTable4(void);
#else
CAPL_DLL_INFO far * CAPLEXPORT far CAPLPASCAL caplDllGetCallbackTable(void);
CAPL_DLL_INFO2 far * CAPLEXPORT far CAPLPASCAL caplDllGetCallbackTable2(void);
CAPL_DLL_INFO3 far * CAPLEXPORT far CAPLPASCAL caplDllGetCallbackTable3(void);
CAPL_DLL_INFO4 far * CAPLEXPORT far CAPLPASCAL caplDllGetCallbackTable4(void);
#endif
#ifdef  __cplusplus
}
#endif
#ifdef _MSC_VER
typedef unsigned long CAPLEXPORT (__cdecl *CAPL_DLL_GETTABLE)(void);
typedef unsigned long CAPLEXPORT (__cdecl *CAPL_DLL_GETTABLE2)(void);
typedef unsigned long CAPLEXPORT (__cdecl *CAPL_DLL_GETTABLE3)(void);
typedef unsigned long CAPLEXPORT (__cdecl *CAPL_DLL_GETTABLE4)(void);
typedef unsigned long CAPLEXPORT (__cdecl *XML_DLL_GETTABLE)(void);
#else
typedef CAPL_DLL_INFO far * CAPLEXPORT far pascal (*CAPL_DLL_GETTABLE)(void);
typedef CAPL_DLL_INFO2 far * CAPLEXPORT far pascal (*CAPL_DLL_GETTABLE2)(void);
typedef CAPL_DLL_INFO3 far * CAPLEXPORT far pascal (*CAPL_DLL_GETTABLE3)(void);
typedef CAPL_DLL_INFO4 far * CAPLEXPORT far pascal (*CAPL_DLL_GETTABLE4)(void);
typedef CAPL_DLL_XML_INFO far * CAPLEXPORT far pascal (*XML_DLL_GETTABLE)(void);
#endif

typedef struct /*_CAPL_DLL_SERVER*/ {
  HINSTANCE             hInst;          // instance of the DLL
  LPSTR                 name;           // name of that module
  CAPL_DLL_INFO far     *funcList;      // info list
  CAPL_DLL_INFO far     *callBackList;  // CAPL callback functions
  CAPL_DLL_INFO2 far    *funcList2;     // info list, long function names
  CAPL_DLL_INFO2 far    *callBackList2; // CAPL callback functions, long function names
  CAPL_DLL_INFO3 far    *funcList3;     // info list, parameter names
  CAPL_DLL_INFO3 far    *callBackList3; // CAPL callback functions, parameter names
  CAPL_DLL_INFO4 far    *funcList4;     // info list,up to 64 parameters
  CAPL_DLL_INFO4 far    *callBackList4; // CAPL callback functions, up to 64 parameters
  CAPL_DLL_XML_INFO far *xmlFuncList;   // xml info list
  CAPL_DLL_CALLCONV     callConv;       // Calling convention used in DLL
  unsigned int          checkSum;       // can be used to distinguish dlls
} *CAPL_DLL_SERVER;
#define CAPL_DLL_ERROR ((CAPL_DLL_SERVER)HINSTANCE_ERROR)

#include <poppack.h> // restore structure alignment
#endif
