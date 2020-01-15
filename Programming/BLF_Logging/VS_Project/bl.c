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
| 08/21/02     1.0  Sn      BLF sample
|-----------------------------------------------------------------------------
|               C O P Y L E F T
|-----------------------------------------------------------------------------
|               2002 by Vector Informatik GmbH.  All rights reserved.
 ----------------------------------------------------------------------------*/

#define _CRT_SECURE_NO_WARNINGS

#include <tchar.h>                     /* RTL   */
#include <stdio.h>

#define STRICT                         /* WIN32 */
#include <windows.h>

#include "binlog.h"                    /* BL    */

/******************************************************************************
*                                                                             *
* write BL file                                                               *
*                                                                             *
******************************************************************************/

#define ENV_NAME1   _T( "EnvString")
#define ENV_NAME2   _T( "EnvInt")
#define ENV_DATA1   _T( "01234567")
#define ENV_DATA2   _T( "76543210")

int write_test( LPCTSTR pFileName, LPDWORD pWritten)
{
    HANDLE hFile;
    SYSTEMTIME systemTime;
    BOOL bSuccess;
    ULONGLONG i;
    VBLAppTrigger appTrigger;
    VBLCANMessage message;
    VBLEnvironmentVariable variable_s;
    VBLEnvironmentVariable variable_i;
    VBLEthernetFrame ethframe;

    BYTE src[6] = { 0, 1, 2, 3, 4, 5};
    BYTE dst[6] = { 6, 7, 8, 9, 10, 11};
    BYTE ethbuffer[1500];
    VBLAppText appText;
    ULONGLONG time;

    if ( NULL == pWritten)
    {
        return -1;
    }

    *pWritten = 0;

    /* open file */
    hFile = BLCreateFile( pFileName, GENERIC_WRITE);

    if ( INVALID_HANDLE_VALUE == hFile)
    {
        return -1;
    }

    /* set applicaton information */
    /* bSuccess = BLSetApplication( hFile, BL_APPID_UNKNOWN, 1, 0, 0); */
    bSuccess = BLSetApplication( hFile, BL_APPID_CANCASEXLLOG, 1, 0, 1);

    GetSystemTime( &systemTime);

    bSuccess = bSuccess && BLSetMeasurementStartTime( hFile, &systemTime);

    /* set write options */
    bSuccess = bSuccess && BLSetWriteOptions( hFile, 6, 0);
    
    if ( bSuccess)
    {
		// initialize all members to zero (including reserved space)
		memset(&appTrigger, 0, sizeof(VBLAppTrigger));
		memset(&message,    0, sizeof(VBLCANMessage));
		memset(&variable_s, 0, sizeof(VBLEnvironmentVariable));
		memset(&variable_i, 0, sizeof(VBLEnvironmentVariable));
		memset(&ethframe,   0, sizeof(VBLEthernetFrame));

        /* setup object headers */
        appTrigger.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
        appTrigger.mHeader.mBase.mHeaderSize = sizeof( appTrigger.mHeader);
        appTrigger.mHeader.mBase.mHeaderVersion = 1;
        appTrigger.mHeader.mBase.mObjectSize = sizeof( VBLAppTrigger);
        appTrigger.mHeader.mBase.mObjectType = BL_OBJ_TYPE_APP_TRIGGER;
        appTrigger.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

        message.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
        message.mHeader.mBase.mHeaderSize = sizeof( message.mHeader);
        message.mHeader.mBase.mHeaderVersion = 1;
        message.mHeader.mBase.mObjectSize = sizeof( VBLCANMessage);
        message.mHeader.mBase.mObjectType = BL_OBJ_TYPE_CAN_MESSAGE;
        message.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

        variable_s.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
        variable_s.mHeader.mBase.mHeaderSize = sizeof( variable_s.mHeader);
        variable_s.mHeader.mBase.mHeaderVersion = 1;
        variable_s.mHeader.mBase.mObjectType = BL_OBJ_TYPE_ENV_STRING;
        variable_s.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

        variable_i.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
        variable_i.mHeader.mBase.mHeaderSize = sizeof( variable_i.mHeader);
        variable_i.mHeader.mBase.mHeaderVersion = 1;
        variable_i.mHeader.mBase.mObjectType = BL_OBJ_TYPE_ENV_INTEGER;
        variable_i.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

        ethframe.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
        ethframe.mHeader.mBase.mHeaderSize = sizeof( ethframe.mHeader);
        ethframe.mHeader.mBase.mHeaderVersion = 1;
        ethframe.mHeader.mBase.mObjectType = BL_OBJ_TYPE_ETHERNET_FRAME;
        ethframe.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

        appText.mHeader.mBase.mSignature = BL_OBJ_SIGNATURE;
        appText.mHeader.mBase.mHeaderSize = sizeof( appText.mHeader);
        appText.mHeader.mBase.mHeaderVersion = 1;
        appText.mHeader.mBase.mObjectType = BL_OBJ_TYPE_APP_TEXT;
        appText.mHeader.mObjectFlags = BL_OBJ_FLAG_TIME_ONE_NANS;

        for ( i = 0; i < 1000; ++i)
        {
            ethbuffer[i] = ( BYTE)i;
        }

        for ( i = 0; i < 1000 && bSuccess; ++i)
        {
            /* increment in milliseconds */
            time = i * 10000000;

            /* setup app trigger object header */
            appTrigger.mHeader.mObjectTimeStamp = time;

            /* write app trigger object */
            bSuccess = BLWriteObject( hFile, &appTrigger.mHeader.mBase);
            *pWritten += bSuccess ? 1 : 0;
            
            /* setup CAN object header */
            message.mHeader.mObjectTimeStamp = time;
    
            /* setup CAN message */
            message.mChannel = 1;
            message.mFlags = CAN_MSG_FLAGS( 0, 0);
            message.mDLC = 8;
            message.mID = 0x100;
            memcpy( message.mData, ( i % 2) ? _T( "01234567") : _T( "76543210"), message.mDLC);

            /* write CAN message */
            bSuccess = BLWriteObject( hFile, &message.mHeader.mBase);
            *pWritten += bSuccess ? 1 : 0;

            if ( 0 == ( i % 3) && bSuccess)
            {
                /* setup environment variable object headers */
                variable_s.mHeader.mObjectTimeStamp = time;
                variable_i.mHeader.mObjectTimeStamp = time;

                /* setup environment variables */
                variable_s.mNameLength = (DWORD) (strlen( ENV_NAME1));
                variable_s.mDataLength = (DWORD) (strlen( ( i % 2) ? ENV_DATA1 : ENV_DATA2));
                variable_s.mName = ENV_NAME1;
                variable_s.mData = ( i % 2) ? ((LPBYTE)(ENV_DATA1)) : ((LPBYTE)(ENV_DATA2));
                variable_s.mHeader.mBase.mObjectSize = sizeof( VBLEnvironmentVariable) + variable_s.mNameLength + variable_s.mDataLength;

                variable_i.mNameLength = (DWORD) (strlen( ENV_NAME2));
                variable_i.mDataLength = sizeof( int);
                variable_i.mName = ENV_NAME2;
                variable_i.mData = ( LPBYTE)&i;
                variable_i.mHeader.mBase.mObjectSize = sizeof( VBLEnvironmentVariable) + variable_i.mNameLength + variable_i.mDataLength;
        
                /* write environment variables */
                bSuccess = BLWriteObject( hFile, &variable_s.mHeader.mBase);
                *pWritten += bSuccess ? 1 : 0;

                bSuccess = bSuccess && BLWriteObject( hFile, &variable_i.mHeader.mBase);
                *pWritten += bSuccess ? 1 : 0;

                /* write ethernet frame */
                ethframe.mHeader.mObjectTimeStamp = time;
                memcpy( ethframe.mSourceAddress, src, sizeof( ethframe.mSourceAddress));
                ethframe.mChannel = 0;
                memcpy( ethframe.mDestinationAddress, dst, sizeof( ethframe.mDestinationAddress));
                ethframe.mDir = 0;
                ethframe.mType = 0x0800;
                ethframe.mTPID = 0;
                ethframe.mTCI = 0;
                ethframe.mPayLoadLength = ( WORD)i;
                ethframe.mPayLoad = ethbuffer;
                ethframe.mHeader.mBase.mObjectSize = sizeof( VBLEthernetFrame) + ethframe.mPayLoadLength;

                bSuccess = bSuccess && BLWriteObject( hFile, &ethframe.mHeader.mBase);
                *pWritten += bSuccess ? 1 : 0;

                /* write text */
                if ( ( i % 100) == 0)
                {
                    char text[128];

                    sprintf( text, "%d objects written...", *pWritten);

                    appText.mText = text;
                    appText.mTextLength = (DWORD) (strlen( appText.mText));
                    appText.mHeader.mBase.mObjectSize = sizeof( VBLAppText) + appText.mTextLength;
                    appText.mHeader.mObjectTimeStamp = time;

                    bSuccess = bSuccess && BLWriteObject( hFile, &appText.mHeader.mBase);
                    *pWritten += bSuccess ? 1 : 0;
                }
            }
        }

        appText.mText = "All objects written...";
        appText.mTextLength = (DWORD) (strlen( appText.mText));
        appText.mHeader.mBase.mObjectSize = sizeof( VBLAppText) + appText.mTextLength;

        bSuccess = bSuccess && BLWriteObject( hFile, &appText.mHeader.mBase);
        *pWritten += bSuccess ? 1 : 0;
    }

    /* close file */
    if ( !BLCloseHandle( hFile))
    {
        return -1;
    }

    return bSuccess ? 0 : -1;
}

/******************************************************************************
*                                                                             *
* read BL file                                                                *
*                                                                             *
******************************************************************************/
int read_test( LPCTSTR pFileName, LPDWORD pRead)
{
    HANDLE hFile;
    VBLObjectHeaderBase base;
    VBLCANMessage message;
    VBLEnvironmentVariable variable;
    VBLEthernetFrame ethframe;
    VBLAppText appText;
    VBLFileStatisticsEx statistics = { sizeof( statistics)};
    BOOL bSuccess;

    if ( NULL == pRead)
    {
        return -1;
    }

    *pRead = 0;

    /* open file */
    hFile = BLCreateFile( pFileName, GENERIC_READ);

    if ( INVALID_HANDLE_VALUE == hFile)
    {
        return -1;
    }

    BLGetFileStatisticsEx( hFile, &statistics);

    bSuccess = TRUE;
    
    /* read base object header from file */
    while ( bSuccess && BLPeekObject( hFile, &base))
    {
        switch ( base.mObjectType)
        {
        case BL_OBJ_TYPE_CAN_MESSAGE:
            /* read CAN message */
            message.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &message.mHeader.mBase, sizeof(message));
            /* free memory for the CAN message */
            if( bSuccess) {
              BLFreeObject( hFile, &message.mHeader.mBase);
            }
            break;
        case BL_OBJ_TYPE_ENV_INTEGER:
        case BL_OBJ_TYPE_ENV_DOUBLE:
        case BL_OBJ_TYPE_ENV_STRING:
        case BL_OBJ_TYPE_ENV_DATA:
            /* read environment variable */
            variable.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &variable.mHeader.mBase, sizeof(variable));
            /* free memory for the environment variable */
            if( bSuccess) {
              BLFreeObject( hFile, &variable.mHeader.mBase);
            }
            break;
        case BL_OBJ_TYPE_ETHERNET_FRAME:
            /* read ethernet frame */
            ethframe.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &ethframe.mHeader.mBase, sizeof(ethframe));
            /* free memory for the frame */
            if( bSuccess) {
              BLFreeObject( hFile, &ethframe.mHeader.mBase);
            }
            break;
        case BL_OBJ_TYPE_APP_TEXT:
            /* read text */
            appText.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &appText.mHeader.mBase, sizeof(appText));
            if ( NULL != appText.mText)
            {
                printf( "%s\n", appText.mText);
            }
            /* free memory for the text */
            if( bSuccess) {
              BLFreeObject( hFile, &appText.mHeader.mBase);
            }
            break;
        default:
            /* skip all other objects */
            bSuccess = BLSkipObject( hFile, &base);
            break;
        }

        if ( bSuccess)
        {
          *pRead += 1;
        }
    }

    /* close file */
    if ( !BLCloseHandle( hFile))
    {
        return -1;
    }

    return bSuccess ? 0 : -1;
}

/******************************************************************************
*                                                                             *
* skip all elements of an BL file                                             *
*                                                                             *
******************************************************************************/
int skip_test( LPCTSTR pFileName, LPDWORD pRead)
{
    HANDLE hFile;
    VBLObjectHeaderBase base;
    VBLFileStatisticsEx statistics = { sizeof( statistics)};
    BOOL bSuccess;

    if ( NULL == pRead)
    {
        return -1;
    }

    *pRead = 0;

    /* open file */
    hFile = BLCreateFile( pFileName, GENERIC_READ);

    if ( INVALID_HANDLE_VALUE == hFile)
    {
        return -1;
    }

    BLGetFileStatisticsEx( hFile, &statistics);

    bSuccess = TRUE;

    /* read base object header from file */
    while ( bSuccess && BLPeekObject( hFile, &base))
    {
        bSuccess = BLSkipObject( hFile, &base);

        if ( bSuccess)
        {
            *pRead += 1;
        }
    }

    /* close file */
    if ( !BLCloseHandle( hFile))
    {
        return -1;
    }

    printf( "The skip test was: %s\n", bSuccess ? "successful" : "not successful");

    return bSuccess ? 0 : -1;
}

/******************************************************************************
*                                                                             *
*                                                                             *
*                                                                             *
******************************************************************************/
int main( int argc, char** argv)
{
    LPCTSTR pFileName = _T( "test.blf");
    DWORD dwWritten;
    DWORD dwRead;
    DWORD dwSkip;

    int result = 0;
    
    result = write_test( pFileName, &dwWritten);

    if ( 0 != result)
    {
        return result;
    }

    result = read_test( pFileName, &dwRead);

    if ( 0 != result)
    {
        return result;
    }

    result = skip_test( pFileName, &dwSkip);

    if ( 0 != result)
    {
        return result;
    }

    return ( (dwWritten + 1) == dwRead && dwRead == dwSkip ) ? 0 : -1;
}