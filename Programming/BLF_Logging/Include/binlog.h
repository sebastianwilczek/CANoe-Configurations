/*----------------------------------------------------------------------------
|
|-----------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|-----------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     ------------------------------------
| Sn           Michael Stephan           Vector Informatik GmbH
|-----------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-----------------------------------------------------------------------------
| Date         Ver  Author  Description
| ---------    ---  ------  --------------------------------------------------
| 01/04/02     1.0  Sn      Creation
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
| Copyright (c) 2002 by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/
#if !defined ( BINARY_LOGGING_H )
       #define BINARY_LOGGING_H

#if defined ( _MSC_VER )
  #pragma warning( disable: 4103)
  #pragma pack( push, 8)
#elif defined ( __BORLANDC__ )
  #pragma option push -a8
  #pragma nopushoptwarn
  #pragma nopackwarning
#elif defined (__linux__)
  #pragma pack( push, 8)
#endif

#if !defined( RC_INVOKED )
  #if defined (_WIN32)
    #include <wtypes.h>
  #endif // _WIN32
#endif

/*----------------------------------------------------------------------------
|
| BL API
|
-----------------------------------------------------------------------------*/

#define BL_MAJOR_NUMBER 4
#define BL_MINOR_NUMBER 9
#define BL_BUILD_NUMBER 1
#define BL_PATCH_NUMBER 3

#if defined ( BINLOG_EXPORTS )
  #if defined (__linux__)
    #define BLAPI(ret) ret
  #else
    #define BLAPI( ret) ret __stdcall
  #endif
#else
  #if defined (__linux__)
    #define BLAPI(ret) ret
  #else
    #define BLAPI(ret) __declspec(dllimport) ret __stdcall
  #endif
#endif

/*----------------------------------------------------------------------------
|
| BL OBJECTS
|
-----------------------------------------------------------------------------*/

#include "binlog_objects.h"

/*----------------------------------------------------------------------------
|
| API
|
-----------------------------------------------------------------------------*/

#define BL_FLUSH_STREAM                         0x00000001
#define BL_FLUSH_FILE                           0x00000002

#define BL_WRITE_OPTION_IMMEDIATE_STREAM_COMMIT 0x00000001

#if defined ( __cplusplus )
extern "C" {
#endif


#if defined ( __cplusplus )
struct IBLCallback
{
    enum BLMessageLevel
    {
        eInfo    = 0, // Error
        eWarning = 1, // Warning
        eError   = 2  // Information only
    };

    enum BLMessageType
    {
        eDefault        = 0, // typically in write window
        eSimple         = 1, // typically in message box
        eAskStop        = 2, // typically in message box with question to stop
        eAskStopFurther = 3  // typically in message box with questions to stop or show further errors
    };

    virtual void OutputMessage(BLMessageType messageType, BLMessageLevel messageLevel, LPCSTR lpMessage) = 0;
};

struct IBLCallback2 : IBLCallback
{
    enum BLEvent
    {
        eUnknownEvent    = 0, // unknown event
        eStreamConnected = 1, // stream is connected
        eStreamBroken    = 2, // stream is broken
        eStreamInit      = 3  // stream is initialized
    };
    virtual void SignalEvent(HANDLE hFile, BLEvent event) = 0;
};
#endif


BLAPI( HANDLE) BLCreateFile( LPCSTR lpFileName, DWORD dwDesiredAccess);
BLAPI( HANDLE) BLCreateFileW( LPCWSTR lpFileName, DWORD dwDesiredAccess);
BLAPI( HANDLE) BLCreateFileEx( LPCSTR lpFileName, DWORD dwDesiredAccess, LPCSTR lpServer, LPCSTR lpHost);
BLAPI( HANDLE) BLCreateFileExW( LPCWSTR lpFileName, DWORD dwDesiredAccess, LPCWSTR lpServer, LPCWSTR lpHost);
#if defined ( __cplusplus )
BLAPI( HANDLE) BLCreateFileEx2( LPCSTR lpFileName, DWORD dwDesiredAccess, LPCSTR lpServer, LPCSTR lpHost, IBLCallback* pCallback);
BLAPI( HANDLE) BLCreateFileEx2W( LPCWSTR lpFileName, DWORD dwDesiredAccess, LPCWSTR lpServer, LPCWSTR lpHost, IBLCallback* pCallback);
#endif
BLAPI( BOOL)   BLCloseHandle( HANDLE hFile);

BLAPI( BOOL)   BLWriteObject( HANDLE hFile, VBLObjectHeaderBase* pBase);
BLAPI( BOOL)   BLPeekObject( HANDLE hFile, VBLObjectHeaderBase* pBase);
BLAPI( BOOL)   BLSkipObject( HANDLE hFile, VBLObjectHeaderBase* pBase);
BLAPI( BOOL)   BLReadObject( HANDLE hFile, VBLObjectHeaderBase* pBase); /* obsolete - replaced by BLReadObjectSecure */
BLAPI( BOOL)   BLReadObjectSecure( HANDLE hFile, VBLObjectHeaderBase* pBase, size_t expectedSize);
BLAPI( BOOL)   BLFreeObject( HANDLE hFile, VBLObjectHeaderBase* pBase);

BLAPI( BOOL)   BLSeekTime( HANDLE hFile, ULONGLONG timeStamp, void* arg, BOOL (*pProgressCallback)(void*, FLOAT), WORD callbackRate);

BLAPI( BOOL)   BLSetApplication( HANDLE hFile, BYTE appID, BYTE appMajor, BYTE appMinor, BYTE appBuild);
BLAPI( BOOL)   BLSetWriteOptions( HANDLE hFile, DWORD dwCompression, DWORD dwFlags);
BLAPI( BOOL)   BLSetMeasurementStartTime( HANDLE hFile, const LPSYSTEMTIME lpStartTime);
BLAPI( BOOL)   BLGetFileStatistics( HANDLE hFile, VBLFileStatistics* pStatistics);
BLAPI( BOOL)   BLGetFileStatisticsEx( HANDLE hFile, VBLFileStatisticsEx* pStatistics);
BLAPI( BOOL)   BLFlushFileBuffers( HANDLE hFile, DWORD dwFlags);

BLAPI( BOOL)   BLSetNotificationEvent( HANDLE hFile, HANDLE handle);

#if defined ( __cplusplus )
}
#endif

/*----------------------------------------------------------------------------
|
| 
|
-----------------------------------------------------------------------------*/

#if defined ( _MSC_VER )
  #pragma pack( pop)
#elif defined ( __BORLANDC__ )
  #pragma option pop
  #pragma nopushoptwarn
  #pragma nopackwarning
#elif defined (__linux__)
  #pragma pack(pop)
#endif

#endif // BINARY_LOGGING_H
