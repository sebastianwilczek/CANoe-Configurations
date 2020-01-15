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

/*----------------------------------------------------------------------------
|
| Object type IDs
|
-----------------------------------------------------------------------------*/

#if defined (__linux__)
#include "rtkGlobals.h"
#include "rtkErrors.h"
#endif

#define BL_OBJ_SIGNATURE                 0x4A424F4C       /* object signature */

#define BL_OBJ_TYPE_UNKNOWN                       0       /* unknown object */
#define BL_OBJ_TYPE_CAN_MESSAGE                   1       /* CAN message object */
#define BL_OBJ_TYPE_CAN_ERROR                     2       /* CAN error frame object */
#define BL_OBJ_TYPE_CAN_OVERLOAD                  3       /* CAN overload frame object */
#define BL_OBJ_TYPE_CAN_STATISTIC                 4       /* CAN driver statistics object */
#define BL_OBJ_TYPE_APP_TRIGGER                   5       /* application trigger object */
#define BL_OBJ_TYPE_ENV_INTEGER                   6       /* environment integer object */
#define BL_OBJ_TYPE_ENV_DOUBLE                    7       /* environment double object */
#define BL_OBJ_TYPE_ENV_STRING                    8       /* environment string object */
#define BL_OBJ_TYPE_ENV_DATA                      9       /* environment data object */
#define BL_OBJ_TYPE_LOG_CONTAINER                10       /* container object */

#define BL_OBJ_TYPE_LIN_MESSAGE                  11       /* LIN message object */
#define BL_OBJ_TYPE_LIN_CRC_ERROR                12       /* LIN CRC error object */
#define BL_OBJ_TYPE_LIN_DLC_INFO                 13       /* LIN DLC info object */
#define BL_OBJ_TYPE_LIN_RCV_ERROR                14       /* LIN receive error object */
#define BL_OBJ_TYPE_LIN_SND_ERROR                15       /* LIN send error object */
#define BL_OBJ_TYPE_LIN_SLV_TIMEOUT              16       /* LIN slave timeout object */
#define BL_OBJ_TYPE_LIN_SCHED_MODCH              17       /* LIN scheduler mode change object */
#define BL_OBJ_TYPE_LIN_SYN_ERROR                18       /* LIN sync error object */
#define BL_OBJ_TYPE_LIN_BAUDRATE                 19       /* LIN baudrate event object */
#define BL_OBJ_TYPE_LIN_SLEEP                    20       /* LIN sleep mode event object */
#define BL_OBJ_TYPE_LIN_WAKEUP                   21       /* LIN wakeup event object */

#define BL_OBJ_TYPE_MOST_SPY                     22       /* MOST spy message object */
#define BL_OBJ_TYPE_MOST_CTRL                    23       /* MOST control message object */
#define BL_OBJ_TYPE_MOST_LIGHTLOCK               24       /* MOST light lock object */
#define BL_OBJ_TYPE_MOST_STATISTIC               25       /* MOST statistic object */

#define BL_OBJ_TYPE_reserved_1                   26
#define BL_OBJ_TYPE_reserved_2                   27
#define BL_OBJ_TYPE_reserved_3                   28

#define BL_OBJ_TYPE_FLEXRAY_DATA                 29       /* FLEXRAY data object */
#define BL_OBJ_TYPE_FLEXRAY_SYNC                 30       /* FLEXRAY sync object */

#define BL_OBJ_TYPE_CAN_DRIVER_ERROR             31       /* CAN driver error object */

#define BL_OBJ_TYPE_MOST_PKT                     32       /* MOST Packet */
#define BL_OBJ_TYPE_MOST_PKT2                    33       /* MOST Packet including original timestamp */
#define BL_OBJ_TYPE_MOST_HWMODE                  34       /* MOST hardware mode event */
#define BL_OBJ_TYPE_MOST_REG                     35       /* MOST register data (various chips)*/
#define BL_OBJ_TYPE_MOST_GENREG                  36       /* MOST register data (MOST register) */
#define BL_OBJ_TYPE_MOST_NETSTATE                37       /* MOST NetState event */
#define BL_OBJ_TYPE_MOST_DATALOST                38       /* MOST data lost */
#define BL_OBJ_TYPE_MOST_TRIGGER                 39       /* MOST trigger */

#define BL_OBJ_TYPE_FLEXRAY_CYCLE                40       /* FLEXRAY V6 start cycle object */
#define BL_OBJ_TYPE_FLEXRAY_MESSAGE              41       /* FLEXRAY V6 message object */

#define BL_OBJ_TYPE_LIN_CHECKSUM_INFO            42       /* LIN checksum info event object */
#define BL_OBJ_TYPE_LIN_SPIKE_EVENT              43       /* LIN spike event object */

#define BL_OBJ_TYPE_CAN_DRIVER_SYNC              44       /* CAN driver hardware sync */

#define BL_OBJ_TYPE_FLEXRAY_STATUS               45       /* FLEXRAY status event object */

#define BL_OBJ_TYPE_GPS_EVENT                    46       /* GPS event object */

#define BL_OBJ_TYPE_FR_ERROR                     47       /* FLEXRAY error event object */
#define BL_OBJ_TYPE_FR_STATUS                    48       /* FLEXRAY status event object */
#define BL_OBJ_TYPE_FR_STARTCYCLE                49       /* FLEXRAY start cycle event object */
#define BL_OBJ_TYPE_FR_RCVMESSAGE                50       /* FLEXRAY receive message event object */

#define BL_OBJ_TYPE_REALTIMECLOCK                51       /* Realtime clock object */
#define BL_OBJ_TYPE_AVAILABLE2                   52       /* this object ID is available for the future */
#define BL_OBJ_TYPE_AVAILABLE3                   53       /* this object ID is available for the future */

#define BL_OBJ_TYPE_LIN_STATISTIC                54       /* LIN statistic event object */

#define BL_OBJ_TYPE_J1708_MESSAGE                55       /* J1708 message object */
#define BL_OBJ_TYPE_J1708_VIRTUAL_MSG            56       /* J1708 message object with more than 21 data bytes */

#define BL_OBJ_TYPE_LIN_MESSAGE2                 57       /* LIN frame object - extended */
#define BL_OBJ_TYPE_LIN_SND_ERROR2               58       /* LIN transmission error object - extended */
#define BL_OBJ_TYPE_LIN_SYN_ERROR2               59       /* LIN sync error object - extended */
#define BL_OBJ_TYPE_LIN_CRC_ERROR2               60       /* LIN checksum error object - extended */
#define BL_OBJ_TYPE_LIN_RCV_ERROR2               61       /* LIN receive error object */
#define BL_OBJ_TYPE_LIN_WAKEUP2                  62       /* LIN wakeup event object  - extended */
#define BL_OBJ_TYPE_LIN_SPIKE_EVENT2             63       /* LIN spike event object - extended */
#define BL_OBJ_TYPE_LIN_LONG_DOM_SIG             64       /* LIN long dominant signal object */

#define BL_OBJ_TYPE_APP_TEXT                     65       /* text object */

#define BL_OBJ_TYPE_FR_RCVMESSAGE_EX             66       /* FLEXRAY receive message ex event object */

#define BL_OBJ_TYPE_MOST_STATISTICEX             67       /* MOST extended statistic event */
#define BL_OBJ_TYPE_MOST_TXLIGHT                 68       /* MOST TxLight event */
#define BL_OBJ_TYPE_MOST_ALLOCTAB                69       /* MOST Allocation table event */
#define BL_OBJ_TYPE_MOST_STRESS                  70       /* MOST Stress event */

#define BL_OBJ_TYPE_ETHERNET_FRAME               71       /* Ethernet frame object */

#define BL_OBJ_TYPE_SYS_VARIABLE                 72       /* system variable object */

#define BL_OBJ_TYPE_CAN_ERROR_EXT                73       /* CAN error frame object (extended) */
#define BL_OBJ_TYPE_CAN_DRIVER_ERROR_EXT         74       /* CAN driver error object (extended) */

#define BL_OBJ_TYPE_LIN_LONG_DOM_SIG2            75       /* LIN long dominant signal object - extended */

#define BL_OBJ_TYPE_MOST_150_MESSAGE             76   /* MOST150 Control channel message */
#define BL_OBJ_TYPE_MOST_150_PKT                 77   /* MOST150 Asynchronous channel message */
#define BL_OBJ_TYPE_MOST_ETHERNET_PKT            78   /* MOST Ethernet channel message */
#define BL_OBJ_TYPE_MOST_150_MESSAGE_FRAGMENT    79   /* Partial transmitted MOST50/150 Control channel message */
#define BL_OBJ_TYPE_MOST_150_PKT_FRAGMENT        80   /* Partial transmitted MOST50/150 data packet on asynchronous channel */
#define BL_OBJ_TYPE_MOST_ETHERNET_PKT_FRAGMENT   81   /* Partial transmitted MOST Ethernet packet on asynchronous channel */
#define BL_OBJ_TYPE_MOST_SYSTEM_EVENT            82   /* Event for various system states on MOST */
#define BL_OBJ_TYPE_MOST_150_ALLOCTAB            83   /* MOST50/150 Allocation table event */
#define BL_OBJ_TYPE_MOST_50_MESSAGE              84   /* MOST50 Control channel message */
#define BL_OBJ_TYPE_MOST_50_PKT                  85   /* MOST50 Asynchronous channel message */

#define BL_OBJ_TYPE_CAN_MESSAGE2                 86   /* CAN message object - extended */

#define BL_OBJ_TYPE_LIN_UNEXPECTED_WAKEUP        87
#define BL_OBJ_TYPE_LIN_SHORT_OR_SLOW_RESPONSE   88
#define BL_OBJ_TYPE_LIN_DISTURBANCE_EVENT        89

#define BL_OBJ_TYPE_SERIAL_EVENT                 90

#define BL_OBJ_TYPE_OVERRUN_ERROR                91   /* driver overrun event */

#define BL_OBJ_TYPE_EVENT_COMMENT                92

#define BL_OBJ_TYPE_WLAN_FRAME                   93
#define BL_OBJ_TYPE_WLAN_STATISTIC               94

#define BL_OBJ_TYPE_MOST_ECL                     95   /* MOST Electrical Control Line event */

#define BL_OBJ_TYPE_GLOBAL_MARKER                96

#define BL_OBJ_TYPE_AFDX_FRAME                   97
#define BL_OBJ_TYPE_AFDX_STATISTIC               98

#define BL_OBJ_TYPE_KLINE_STATUSEVENT            99   /* E.g. wake-up pattern */

#define BL_OBJ_TYPE_CAN_FD_MESSAGE              100   /*CAN FD message object*/

#define BL_OBJ_TYPE_CAN_FD_MESSAGE_64           101   /*CAN FD message object */

#define BL_OBJ_TYPE_ETHERNET_RX_ERROR           102   /* Ethernet RX error object */
#define BL_OBJ_TYPE_ETHERNET_STATUS             103   /* Ethernet status object */

#define BL_OBJ_TYPE_CAN_FD_ERROR_64             104   /*CAN FD Error Frame object */
#define BL_OBJ_TYPE_LIN_SHORT_OR_SLOW_RESPONSE2 105

#define BL_OBJ_TYPE_AFDX_STATUS                 106   /* AFDX status object */
#define BL_OBJ_TYPE_AFDX_BUS_STATISTIC          107   /* AFDX line-dependent busstatistic object */
#define BL_OBJ_TYPE_reserved_4                  108
#define BL_OBJ_TYPE_AFDX_ERROR_EVENT            109   /* AFDX asynchronous error event*/

#define BL_OBJ_TYPE_A429_ERROR                  110   /* A429 error object */
#define BL_OBJ_TYPE_A429_STATUS                 111   /* A429 status object */
#define BL_OBJ_TYPE_A429_BUS_STATISTIC          112   /* A429 busstatistic object */
#define BL_OBJ_TYPE_A429_MESSAGE                113   /* A429 Message*/

#define BL_OBJ_TYPE_ETHERNET_STATISTIC          114   /* Ethernet statistic object */

#define BL_OBJ_TYPE_reserved_5                  115

#define BL_OBJ_TYPE_reserved_6                  116

#define BL_OBJ_TYPE_reserved_7                  117

#define BL_OBJ_TYPE_TEST_STRUCTURE              118   /* Event for test execution flow */

#define BL_OBJ_TYPE_DIAG_REQUEST_INTERPRETATION 119   /* Event for correct interpretation of diagnostic requests */

#define BL_OBJ_TYPE_ETHERNET_FRAME_EX           120   /* Ethernet packet extended object */
#define BL_OBJ_TYPE_ETHERNET_FRAME_FORWARDED    121   /* Ethernet packet forwarded object */
#define BL_OBJ_TYPE_ETHERNET_ERROR_EX           122   /* Ethernet error extended object */
#define BL_OBJ_TYPE_ETHERNET_ERROR_FORWARDED    123   /* Ethernet error forwarded object */

#define BL_OBJ_TYPE_FUNCTION_BUS                124   /* OBSOLETE */
#define BL_OBJ_TYPE_COMMUNICATION_OBJECT        124   /* Communication object in the communication setup */

#define BL_OBJ_TYPE_DATA_LOST_BEGIN             125   /* Data lost begin*/
#define BL_OBJ_TYPE_DATA_LOST_END               126   /* Data lost end*/
#define BL_OBJ_TYPE_WATER_MARK_EVENT            127   /* Watermark event*/
#define BL_OBJ_TYPE_TRIGGER_CONDITION           128   /* Trigger Condition event*/
#define BL_OBJ_TYPE_CAN_SETTING_CHANGED         129   /* CAN Settings Changed object */


/*----------------------------------------------------------------------------
|
| Define pointers
|
-----------------------------------------------------------------------------*/

#ifndef BL_LPBYTE
#define BL_LPBYTE LPBYTE
#endif

#ifndef BL_LPSTR
#define BL_LPSTR LPSTR
#endif

#ifndef BL_LPWSTR
#define BL_LPWSTR  LPWSTR
#endif

#ifndef BL_PLONGLONG
#define BL_PLONGLONG PLONGLONG
#endif

/*----------------------------------------------------------------------------
|
| Base object header type definition
|
-----------------------------------------------------------------------------*/

typedef struct VBLObjectHeaderBase_t
{
    DWORD     mSignature;                        /* signature (BL_OBJ_SIGNATURE) */
     WORD     mHeaderSize;                       /* sizeof object header */
     WORD     mHeaderVersion;                    /* header version (1) */
    DWORD     mObjectSize;                       /* object size */
    DWORD     mObjectType;                       /* object type (BL_OBJ_TYPE_XXX) */
} VBLObjectHeaderBase;

/*----------------------------------------------------------------------------
|
| Extended base object header type definition with dynamic extendible objects 
|
-----------------------------------------------------------------------------*/


typedef struct VBLVarObjectHeader_t
{
  VBLObjectHeaderBase mBase;                   /* base header object */
  DWORD               mObjectFlags;            /* object flags */
  WORD                mObjectStaticSize;       /* size of the static part of the object */
  WORD                mObjectVersion;          /* object specific version */
  ULONGLONG           mObjectTimeStamp;        /* object timestamp */
} VBLVarObjectHeader;


/*----------------------------------------------------------------------------
|
| Object flag IDs
|
-----------------------------------------------------------------------------*/

#define BL_OBJ_FLAG_TIME_TEN_MICS     0x00000001 /* 10 micro second timestamp */
#define BL_OBJ_FLAG_TIME_ONE_NANS     0x00000002 /* 1 nano second timestamp */

/*----------------------------------------------------------------------------
|
| Object header type definitions
|
-----------------------------------------------------------------------------*/

typedef struct VBLObjectHeader_t
{
    VBLObjectHeaderBase mBase;                   /* base header object */
    DWORD               mObjectFlags;            /* object flags */
    WORD                mClientIndex;            /* client index of send node */
    WORD                mObjectVersion;          /* object specific version */
    ULONGLONG           mObjectTimeStamp;        /* object timestamp */
} VBLObjectHeader;

typedef struct VBLObjectHeader2_t
{
    VBLObjectHeaderBase mBase;                   /* base header object */
    DWORD               mObjectFlags;            /* object flags */
    BYTE                mTimeStampStatus;        /* time stamp status */
    BYTE                mReserved1;              /* reserved */
    WORD                mObjectVersion;          /* object specific version */
    ULONGLONG           mObjectTimeStamp;        /* object timestamp */
    ULONGLONG           mOriginalTimeStamp;      /* original object timestamp */
} VBLObjectHeader2;

/*----------------------------------------------------------------------------
|
| Flags for TimeStampStatus
|
-----------------------------------------------------------------------------*/

#define BL_OBJ_TIMESTAMPSTATUS_FLAG_ORIG   0x01  /* 1: valid orig. timestamp  */
#define BL_OBJ_TIMESTAMPSTATUS_FLAG_SWHW   0x02  /* 1: sw generated ts; 0: hw */
#define BL_OBJ_TIMESTAMPSTATUS_FLAG_USER   0x10  /* protocol specific meaning */

/*----------------------------------------------------------------------------
|
| CAN objects
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANMessage_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mFlags;                      /* CAN dir & rtr */
    BYTE            mDLC;                        /* CAN dlc */
    DWORD           mID;                         /* CAN ID */
    BYTE            mData[8];                    /* CAN data */
} VBLCANMessage;

// CAN dir, rtr, wu & nerr encoded into flags
#define CAN_MSG_DIR( f)          ( BYTE)(   f & 0x0F)
#define CAN_MSG_RTR( f)          ( BYTE)( ( f & 0x80) >> 7)
#define CAN_MSG_WU( f)           ( BYTE)( ( f & 0x40) >> 6)
#define CAN_MSG_NERR( f)         ( BYTE)( ( f & 0x20) >> 5)
#define CAN_MSG_FLAGS( dir, rtr) ( BYTE)( ( ( BYTE)( rtr & 0x01) << 7) | \
                                            ( BYTE)( dir & 0x0F))
#define CAN_MSG_FLAGS_EXT( dir, rtr, wu, nerr) \
                                 ( BYTE)( ( ( BYTE)( rtr  & 0x01) << 7) | \
                                          ( ( BYTE)( wu   & 0x01) << 6) | \
                                          ( ( BYTE)( nerr & 0x01) << 5) | \
                                            ( BYTE)( dir  & 0x0F))
#define CAN_FD_MSG_EDL( f)      (BYTE) (f & 0x1)
#define CAN_FD_MSG_BRS( f)      (BYTE) ((f & 0x2) >> 1)
#define CAN_FD_MSG_ESI( f)      (BYTE) ((f & 0x4) >> 2)


#define  CAN_FD_MSG_FLAGS( edl, brs, esi) \
  ( BYTE)( ( ( BYTE)( edl  & 0x01)) | \
  ( ( BYTE)( brs & 0x01) << 1) | \
  ( BYTE)( esi  & 0x01) << 2)

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANErrorFrame_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mLength;                     /* CAN error frame length */
} VBLCANErrorFrame;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANErrorFrameExt_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mLength;                     /* CAN error frame length */
    DWORD           mFlags;                      /* extended CAN error frame flags */
    BYTE            mECC;                        /* error control code */
    BYTE            mPosition;                   /* error position */
    BYTE            mDLC;                        /* lower 4 bits: DLC from CAN-Core. Upper 4 bits: reserved */
    BYTE            mReserved1;
    DWORD           mFrameLengthInNS;            /* frame length in ns */
    DWORD           mID;                         /* frame ID from CAN-Core */
    WORD            mFlagsExt;                   /* extended error flags */
    WORD            mReserved2;
    BYTE            mData[8];                    /* Payload, only for CAN-Core */
} VBLCANErrorFrameExt;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANOverloadFrame_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mDummy;                      /* pad */
} VBLCANOverloadFrame;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANDriverStatistic_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mBusLoad;                    /* CAN bus load */
    DWORD           mStandardDataFrames;         /* standard CAN id data frames */
    DWORD           mExtendedDataFrames;         /* extended CAN id data frames */
    DWORD           mStandardRemoteFrames;       /* standard CAN id remote frames */
    DWORD           mExtendedRemoteFrames;       /* extented CAN id remote frames */
    DWORD           mErrorFrames;                /* CAN error frames */
    DWORD           mOverloadFrames;             /* CAN overload frames */
} VBLCANDriverStatistic;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANDriverError_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mTXErrors;                   /* # of TX errors */
    BYTE            mRXErrors;                   /* # of RX errors */
    DWORD           mErrorCode;                  /* CAN driver error code */
} VBLCANDriverError;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANDriverErrorExt_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mTXErrors;                   /* # of TX errors */
    BYTE            mRXErrors;                   /* # of RX errors */
    DWORD           mErrorCode;                  /* CAN driver error code */
    DWORD           mFlags;                      /* flags */
    BYTE            mState;                      /* state register */
    BYTE            mReserved1;
    WORD            mReserved2;
    DWORD           mReserved3[4];

} VBLCANDriverErrorExt;

#define BL_HWSYNC_FLAGS_TX      1                /* sync was sent from this channel */
#define BL_HWSYNC_FLAGS_RX      2                /* external sync received */
#define BL_HWSYNC_FLAGS_RX_THIS 4                /* sync received but generated from this hardware */

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLCANDriverHwSync_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* channel where sync occured */
    BYTE            mFlags;                      /* BL_HWSYNC_FLAGS_XXX */
    BYTE            mDummy;                      /* pad */
} VBLCANDriverHwSync;

/*----------------------------------------------------------------------------
|
| CAN extended objects
|
-----------------------------------------------------------------------------*/

/* HINT: This structure might be extended in future versions! */
typedef struct VBLCANMessage2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  WORD            mChannel;                    /* application channel */
  BYTE            mFlags;                      /* CAN dir & rtr */
  BYTE            mDLC;                        /* CAN dlc */
  DWORD           mID;                         /* CAN ID */
  BYTE            mData[8];                    /* CAN data */
  DWORD           mFrameLength;                /* message length in ns - without 3 Interframe Space bits and by Rx-message also without 1 End-Of-Frame bit */
  BYTE            mBitCount;                   /* complete message length in bits */
  BYTE            mReserved1;                  /* reserved */
  WORD            mReserved2;                  /* reserved */
} VBLCANMessage2;





/*----------------------------------------------------------------------------
|
| CAN FD objects
|
-----------------------------------------------------------------------------*/
/* HINT: This structure might be extended in future versions! */
typedef struct VBLCANFDMessage_t
{
  VBLObjectHeader mHeader;                     /* object header */
  WORD            mChannel;                    /* application channel */
  BYTE            mFlags;                      /* CAN dir & rtr */
  BYTE            mDLC;                        /* CAN dlc */
  DWORD           mID;                         /* CAN ID */
  DWORD           mFrameLength;                /* message length in ns - without 3 inter frame space bits and by Rx-message also without 1 End-Of-Frame bit */
  BYTE            mArbBitCount;                /* bit count of arbitration phase */
  BYTE            mCANFDFlags;                 /* CAN FD flags */
  BYTE            mValidDataBytes;             /* Valid payload length of mData */
  BYTE            mReserved1;                  /* reserved */
  DWORD           mReserved2;                  /* reserved */
  BYTE            mData[64];                   /* CAN FD data */
} VBLCANFDMessage;

/*----------------------------------------------------------------------------
|
| CAN FD objects
|
-----------------------------------------------------------------------------*/


typedef struct VBLCANFDExtFrameData_t{
  DWORD mBTRExtArb;
  DWORD mBTRExtData;
  //may be extended in future versions
} VBLCANFDExtFrameData;

#define BLHasExtFrameData(b) (((b)->mExtDataOffset != 0) && ((b)->mHeader.mBase.mObjectSize >= ((b)->mExtDataOffset + sizeof(VBLCANFDExtFrameData))))
#define BLExtFrameDataPtr(b) ((VBLCANFDExtFrameData*)((BYTE*)(b) + (b)->mExtDataOffset))

typedef struct VBLCANFDMessage64_t
{
  VBLObjectHeader mHeader;                     /* object header */
  BYTE            mChannel;                    /* application channel */
  BYTE            mDLC;                        /* CAN dlc */
  BYTE            mValidDataBytes;             /* Valid payload length of mData */
  BYTE            mTxCount;                    /* TXRequiredCount (4 bits), TxReqCount (4 Bits) */
  DWORD           mID;                         /* CAN ID */
  DWORD           mFrameLength;                /* message length in ns - without 3 inter frame space bits */
                                               /* and by Rx-message also without 1 End-Of-Frame bit */
  DWORD           mFlags;                      /* flags */
  DWORD           mBtrCfgArb;                  /* bit rate used in arbitration phase */
  DWORD           mBtrCfgData;                 /* bit rate used in data phase */
  DWORD           mTimeOffsetBrsNs;            /* time offset of brs field */
  DWORD           mTimeOffsetCrcDelNs;         /* time offset of crc delimiter field */
  WORD            mBitCount;                   /* complete message length in bits */
  BYTE            mDir;
  BYTE            mExtDataOffset;
  DWORD           mCRC;                        /* CRC for CAN */
  BYTE            mData[64];                   /* CAN FD data */
  VBLCANFDExtFrameData mExtFrameData;
} VBLCANFDMessage64;


typedef struct VBLCANSettingsChanged_t
{
  VBLObjectHeader      mHeader;                     /* object header */
  WORD                 mChannel;                    /* application channel */
  BYTE                 mChangedType;                /* -1 - Invalid Type; 0 - Reseted; 1 - Bit Timing Changed */
  VBLCANFDExtFrameData mBitTimings;
} VBLCANSettingsChanged;


typedef struct VBLCANFDErrorFrame64_t
{
  VBLObjectHeader mHeader;                     /* object header */
  BYTE            mChannel;                    /* application channel */
  BYTE            mDLC;                        /* CAN dlc */
  BYTE            mValidDataBytes;             /* Valid payload length of mData */
  BYTE            mECC;
  WORD            mFlags;
  WORD            mErrorCodeExt;
  WORD            mExtFlags;                   /* FD specific flags */
  BYTE            mExtDataOffset;
  BYTE            reserved1;
  DWORD           mID;                         /* CAN ID */
  DWORD           mFrameLength;                /* message length in ns - without 3 inter frame space bits */
                                               /* and by Rx-message also without 1 End-Of-Frame bit */
  DWORD           mBtrCfgArb;                  /* bit rate used in arbitration phase */
  DWORD           mBtrCfgData;                 /* bit rate used in data phase */
  DWORD           mTimeOffsetBrsNs;            /* time offset of brs field */
  DWORD           mTimeOffsetCrcDelNs;         /* time offset of crc delimiter field */
  DWORD           mCRC;
  WORD            mErrorPosition;              /* error position as bit offset */
  WORD            mReserved2;
  BYTE            mData[64];                   /* CAN FD data */
  VBLCANFDExtFrameData mExtFrameData;
} VBLCANFDErrorFrame64;

/*----------------------------------------------------------------------------
|
| LIN objects
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINMessage_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mID;                         /* LIN ID */
    BYTE            mDLC;                        /* LIN DLC */
    BYTE            mData[8];                    /* t.b.d. */
    BYTE            mFSMId;                      /* t.b.d. */
    BYTE            mFSMState;                   /* t.b.d. */
    BYTE            mHeaderTime;                 /* t.b.d. */
    BYTE            mFullTime;                   /* t.b.d. */
    WORD            mCRC;                        /* t.b.d. */
    BYTE            mDir;                        /* t.b.d. */
    BYTE            mReserved;                   /* t.b.d. */
} VBLLINMessage;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINCRCError_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mID;                         /* LIN ID */
    BYTE            mDLC;                        /* LIN DLC */
    BYTE            mData[8];                    /* t.b.d. */
    BYTE            mFSMId;                      /* t.b.d. */
    BYTE            mFSMState;                   /* t.b.d. */
    BYTE            mHeaderTime;                 /* t.b.d. */
    BYTE            mFullTime;                   /* t.b.d. */
    WORD            mCRC;                        /* t.b.d. */
    BYTE            mDir;                        /* t.b.d. */
    BYTE            mReserved;                   /* t.b.d. */
} VBLLINCRCError;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINDLCInfo_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mID;                         /* LIN ID */
    BYTE            mDLC;                        /* LIN DLC */
} VBLLINDLCInfo;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINChecksumInfo_t
{
  VBLObjectHeader mHeader;                     /* object header */
  WORD            mChannel;                    /* application channel */
  BYTE            mID;                         /* LIN ID */
  BYTE            mChecksumModel;              /* LIN checksum model */
} VBLLINChecksumInfo;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINReceiveError_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mID;                         /* LIN ID */
    BYTE            mDLC;                        /* LIN DLC */
    BYTE            mFSMId;                      /* t.b.d. */
    BYTE            mFSMState;                   /* t.b.d. */
    BYTE            mHeaderTime;                 /* t.b.d. */
    BYTE            mFullTime;                   /* t.b.d. */
    BYTE            mStateReason;                /* t.b.d. */
    BYTE            mOffendingByte;              /* t.b.d. */
    BYTE            mShortError;                 /* t.b.d. */
    BYTE            mTimeoutDuringDlcDetection;  /* t.b.d. */
} VBLLINReceiveError;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINSendError_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mID;                         /* LIN ID */
    BYTE            mDLC;                        /* LIN DLC */
    BYTE            mFSMId;                      /* t.b.d. */
    BYTE            mFSMState;                   /* t.b.d. */
    BYTE            mHeaderTime;                 /* t.b.d. */
    BYTE            mFullTime;                   /* t.b.d. */
} VBLLINSendError;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINSlaveTimeout_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mSlaveID;                    /* t.b.d. */
    BYTE            mStateID;                    /* t.b.d. */
    DWORD           mFollowStateID;              /* t.b.d. */
} VBLLINSlaveTimeout;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINSchedulerModeChange_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mOldMode;                    /* t.b.d. */
    BYTE            mNewMode;                    /* t.b.d. */
    BYTE            mOldSlot;                    /* t.b.d. */
    BYTE            mNewSlot;                    /* t.b.d. */
    BYTE            mFirstEventAfterWakeUp;      /* t.b.d. */
} VBLLINSchedulerModeChange;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINSyncError_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mDummy;                      /* t.b.d */
    WORD            mTimeDiff[4];                /* t.b.d */
} VBLLINSyncError;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINBaudrateEvent_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mDummy;                      /* t.b.d */
    LONG            mBaudrate;                   /* t.b.d */
} VBLLINBaudrateEvent;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINSleepModeEvent_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mReason;                     /* t.b.d */
    BYTE            mFlags;                      /* LIN "was awake", "is awake" & "external" */
} VBLLINSleepModeEvent;

#define LIN_SLEEP_WAS_AWAKE 0x01
#define LIN_SLEEP_IS_AWAKE  0x02
#define LIN_SLEEP_EXTERNAL  0x04

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINWakeupEvent_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mSignal;                     /* t.b.d */
    BYTE            mExternal;                   /* t.b.d */
} VBLLINWakeupEvent;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINSpikeEvent_t
{
  VBLObjectHeader mHeader;                       /* object header */
  WORD            mChannel;                      /* application channel */
  ULONG           mWidth;                        /* the spike's width */

} VBLLINSpikeEvent;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINStatisticEvent_t
{
  VBLObjectHeader mHeader;                       /* object header */
  WORD            mChannel;                      /* application channel */
  double          mBusLoad;                      /* bus load */
  ULONG           mBurstsTotal;                  /* bursts total */
  ULONG           mBurstsOverrun;                /* bursts overrun */
  ULONG           mFramesSent;                   /* frames sent */
  ULONG           mFramesReceived;               /* frames received */
  ULONG           mFramesUnanswered;             /* frames unanswered */
} VBLLINStatisticEvent;


/*----------------------------------------------------------------------------
|
| MOST objects
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTSpy_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mDir;                        /* t.b.d */
    BYTE            mDummy1;                     /* t.b.d */
    DWORD           mSourceAdr;                  /* t.b.d */
    DWORD           mDestAdr;                    /* t.b.d */
    BYTE            mMsg[17];                    /* t.b.d */
    BYTE            mDummy2;                     /* t.b.d */
    WORD            mRTyp;                       /* t.b.d */
    BYTE            mRTypAdr;                    /* t.b.d */
    BYTE            mState;                      /* t.b.d */
    BYTE            mDummy3;                     /* t.b.d */
    BYTE            mAckNack;                    /* t.b.d */
    DWORD           mCRC;                        /* t.b.d */
} VBLMOSTSpy;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTCtrl_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mDir;                        /* t.b.d */
    BYTE            mDummy1;                     /* t.b.d */
    DWORD           mSourceAdr;                  /* t.b.d */
    DWORD           mDestAdr;                    /* t.b.d */
    BYTE            mMsg[17];                    /* t.b.d */
    BYTE            mDummy2;                     /* t.b.d */
    WORD            mRTyp;                       /* t.b.d */
    BYTE            mRTypAdr;                    /* t.b.d */
    BYTE            mState;                      /* t.b.d */
    BYTE            mDummy3H;                    /* t.b.d */
    BYTE            mAckNack;                    /* acknowledge bits */
} VBLMOSTCtrl;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTLightLock_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    SHORT           mState;                      /* t.b.d */
} VBLMOSTLightLock;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTStatistic_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mPktCnt;                     /* t.b.d */
    LONG            mFrmCnt;                     /* t.b.d */
    LONG            mLightCnt;                   /* t.b.d */
    LONG            mBufferLevel;                /* t.b.d */
} VBLMOSTStatistic;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTPkt_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mDir;                        /* t.b.d */
    BYTE            mDummy1;                     /* t.b.d */
    DWORD           mSourceAdr;                  /* t.b.d */
    DWORD           mDestAdr;                    /* t.b.d */
    BYTE            mArbitration;
    BYTE            mTimeRes;
    BYTE            mQuadsToFollow;
    WORD            mCRC;
    BYTE            mPriority;
    BYTE            mTransferType;
    BYTE            mState;
    DWORD           mPktDataLength;              /* length of variable data in bytes */
    BL_LPBYTE       mPktData;                    /* variable data */
} VBLMOSTPkt;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTPkt2_t
{
    VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to*/
                                                 /* */
                                                 /* mHeader.mObjectSize = sizeof( VBLMOSTPkt2) + mPktDataLength; */
                                                 /* */
    WORD             mChannel;                   /* application channel */
    BYTE             mDir;                       /* t.b.d */
    BYTE             mDummy1;                    /* t.b.d */
    DWORD            mSourceAdr;                 /* t.b.d */
    DWORD            mDestAdr;                   /* t.b.d */
    BYTE             mArbitration;
    BYTE             mTimeRes;
    BYTE             mQuadsToFollow;
    WORD             mCRC;
    BYTE             mPriority;
    BYTE             mTransferType;
    BYTE             mState;
    DWORD            mPktDataLength;             /* length of variable data in bytes */
    BL_LPBYTE        mPktData;                   /* variable data */
} VBLMOSTPkt2;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTHWMode_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mDummy1;
    WORD             mHWMode;                    /* bypass/master/slave/spy */
    WORD             mHWModeMask;                /* marks the altered bits */
} VBLMOSTHWMode;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTReg_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    BYTE             mSubType;                   /* read/write request/result */
    BYTE             mDummy1;
    DWORD            mHandle;                    /* operation handle */
    DWORD            mOffset;                    /* start address */
    WORD             mChip;                      /* chip id */
    WORD             mRegDataLen;                /* number of bytes */
    BYTE             mRegData[16];               /* data bytes */
} VBLMOSTReg;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTGenReg_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    BYTE             mSubType;                   /* read/write request/result */
    BYTE             mDummy1;
    DWORD            mHandle;                    /* operation handle */
    WORD             mRegId;                     /* register ID */
    WORD             mDummy2;
    DWORD            mDummy3;
    ULONGLONG        mRegValue;                  /* register value */
} VBLMOSTGenReg;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTNetState_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mStateNew;                  /* MOST NetState */
    WORD             mStateOld;
    WORD             mDummy1;
} VBLMOSTNetState;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTDataLost_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mDummy1;
    DWORD            mInfo;                      /* info about data loss */
    DWORD            mLostMsgsCtrl;
    DWORD            mLostMsgsAsync;
    ULONGLONG        mLastGoodTimeStampNS;
    ULONGLONG        mNextGoodTimeStampNS;
} VBLMOSTDataLost;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTTrigger_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mDummy1;
    WORD             mMode;                      /* trigger mode */
    WORD             mHW;                        /* HW info */
    DWORD            mPreviousTriggerValue;
    DWORD            mCurrentTriggerValue;
} VBLMOSTTrigger;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTStatisticEx_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mDummy1;
    DWORD            mCodingErrors;
    DWORD            mFrameCounter;
} VBLMOSTStatisticEx;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTTxLight_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mState;
} VBLMOSTTxLight;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTAllocTab_t
{
    VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to*/
                                                 /* */
                                                 /* mHeader.mObjectSize = sizeof( VBLMOSTAllocTab) + mLength; */
                                                 /* */
    WORD             mChannel;                   /* application channel */
    WORD             mLength;
    BL_LPBYTE        mTableData;
} VBLMOSTAllocTab;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTStress_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mState;
    WORD             mMode;
    WORD             mDummy1;
} VBLMOSTStress;

/* HINT: This structure might be extended in future versions! */
typedef struct VBLMOSTEcl_t
{
    VBLObjectHeader2 mHeader;                    /* object header */
    WORD             mChannel;                   /* application channel */
    WORD             mMode;
    WORD             mEclState;                  /* Electrical Control Line level */
    WORD             mDummy2;
} VBLMOSTEcl;


/*----------------------------------------------------------------------------
|
| MOST 50/150 objects
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOST150Message_t
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
                                               /* mHeader.mObjectSize = sizeof(VBLMOST150Message) + mMsgLen; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDir;                       /* direction: 0: Rx; 1: Tx; 2: TxRequest */
  BYTE             mDummy1;
  DWORD            mSourceAdr;                 /* source address */
  DWORD            mDestAdr;                   /* target address */
  BYTE             mTransferType;              /* 1: node message; 2: spy message*/
  BYTE             mState;                     /* transmission status */
  BYTE             mAckNack;                   /* acknowledge code */
  BYTE             mDummy2;
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  BYTE             mPAck;                      /* a preemptive acknowledge code */
  BYTE             mCAck;                      /* CRC acknowledge from the packet receiver(s) to the packet transmitter */
  BYTE             mPriority;                  /* priority of the message */
  BYTE             mPIndex;                    /* packet index, increments per message on MOST */
  DWORD            mMsgLen;                    /* length of variable data in bytes (51 max) */
  BL_LPBYTE        mMsg;                       /* variable data */
} VBLMOST150Message;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOST150Pkt_t
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
                                               /* mHeader.mObjectSize = sizeof(VBLMOST150Pkt) + mPktDataLength; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDir;                       /* direction: 0: Rx; 1: Tx; 2: TxRequest */
  BYTE             mDummy1;
  DWORD            mSourceAdr;                 /* source address */
  DWORD            mDestAdr;                   /* target address */
  BYTE             mTransferType;              /* 1: node message; 2: spy message*/
  BYTE             mState;                     /* transmission status */
  BYTE             mAckNack;                   /* acknowledge code */
  BYTE             mDummy2;
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  BYTE             mPAck;                      /* a preemptive acknowledge code */
  BYTE             mCAck;                      /* CRC acknowledge from the packet receiver(s) to the packet transmitter */
  BYTE             mPriority;                  /* priority of the message */
  BYTE             mPIndex;                    /* packet index, increments per message on MOST */
  DWORD            mPktDataLength;             /* length of variable data in bytes (1014 max) */
  BL_LPBYTE        mPktData;                   /* variable data */
} VBLMOST150Pkt;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTEthernetPkt_t
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
                                               /* mHeader.mObjectSize = sizeof(VBLMOSTEthernetPkt) + mPktDataLength; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDir;                       /* direction: 0: Rx; 1: Tx; 2: TxRequest */
  BYTE             mDummy1;
  ULONGLONG        mSourceMacAdr;              /* 48 bit source address */
  ULONGLONG        mDestMacAdr;                /* 48 bit target address */
  BYTE             mTransferType;              /* 1: node message; 2: spy message*/
  BYTE             mState;                     /* transmission status */
  BYTE             mAckNack;                   /* acknowledge code */
  BYTE             mDummy2;
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  BYTE             mPAck;                      /* a preemptive acknowledge code */
  BYTE             mCAck;                      /* CRC acknowledge from the packet receiver(s) to the packet transmitter */
  WORD             mDummy3;
  DWORD            mPktDataLength;             /* length of variable data in bytes (1506 max) */
  BL_LPBYTE        mPktData;                   /* variable data */
} VBLMOSTEthernetPkt;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOST150MessageFragment_t     /* applied for MOST50 and MOST150 */
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
                                               /* mHeader.mObjectSize = sizeof(VBLMOST150MessageFragment) + mFirstDataLen; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDummy1;
  BYTE             mAckNack;                   /* acknowledge code */
  DWORD            mValidMask;                 /* bitfield indicating which members have valid data */
  DWORD            mSourceAdr;                 /* source address */
  DWORD            mDestAdr;                   /* target address */
  BYTE             mPAck;                      /* a preemptive acknowledge code */
  BYTE             mCAck;                      /* CRC acknowledge from the packet receiver(s) to the packet transmitter */
  BYTE             mPriority;                  /* priority of the message */
  BYTE             mPIndex;                    /* packet index, increments per message on MOST */
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  DWORD            mDataLen;                   /* number of transmitted user data bytes */
  DWORD            mDataLenAnnounced;          /* announced user data length at the start of the transmission */
  DWORD            mFirstDataLen;              /* number of bytes in mFirstData */
  BL_LPBYTE        mFirstData;                 /* variable data */
} VBLMOST150MessageFragment;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOST150PktFragment_t         /* applied for MOST50 and MOST150 */
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
                                               /* mHeader.mObjectSize = sizeof(VBLMOST150PktFragment) + mFirstDataLen; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDummy1;
  BYTE             mAckNack;                   /* acknowledge code */
  DWORD            mValidMask;                 /* bitfield indicating which members have valid data */
  DWORD            mSourceAdr;                 /* source address */
  DWORD            mDestAdr;                   /* target address */
  BYTE             mPAck;                      /* a preemptive acknowledge code */
  BYTE             mCAck;                      /* CRC acknowledge from the packet receiver(s) to the packet transmitter */
  BYTE             mPriority;                  /* priority of the message */
  BYTE             mPIndex;                    /* packet index, increments per message on MOST */
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  DWORD            mDataLen;                   /* number of transmitted user data bytes */
  DWORD            mDataLenAnnounced;          /* announced user data length at the start of the transmission */
  DWORD            mFirstDataLen;              /* number of bytes in mFirstData */
  BL_LPBYTE        mFirstData;                 /* variable data */
} VBLMOST150PktFragment;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTEthernetPktFragment_t
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
                                               /* mHeader.mObjectSize = sizeof(VBLMOSTEthernetPktFragment) + mFirstDataLen; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDummy1;
  BYTE             mAckNack;                   /* acknowledge code */
  DWORD            mValidMask;                 /* bitfield indicating which members have valid data */
  ULONGLONG        mSourceMacAdr;              /* 48 bit source address */
  ULONGLONG        mDestMacAdr;                /* 48 bit target address */
  BYTE             mPAck;                      /* a preemptive acknowledge code */
  BYTE             mCAck;                      /* CRC acknowledge from the packet receiver(s) to the packet transmitter */
  WORD             mDummy2;
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  DWORD            mDataLen;                   /* number of transmitted user data bytes */
  DWORD            mDataLenAnnounced;          /* announced user data length at the start of the transmission */
  DWORD            mFirstDataLen;              /* number of bytes in mFirstData */
  BL_LPBYTE        mFirstData;                 /* variable data */
} VBLMOSTEthernetPktFragment;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOSTSystemEvent_t
{
  VBLObjectHeader2 mHeader;                    /* object header */
  WORD             mChannel;                   /* application channel */
  WORD             mId;                        /* identifier of transported data */
  DWORD            mValue;                     /* current value */
  DWORD            mValueOld;                  /* previous value */
} VBLMOSTSystemEvent;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOST150AllocTab_t            /* applied for MOST50 and MOST150 */
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size too */
  WORD             mChannel;                   /* application channel */
  WORD             mEventModeFlags;            /* determines the data layout */
  WORD             mFreeBytes;                 /* number of free bytes after the operation */
  WORD             mLength;                    /* number of bytes in mTableData*/
  BL_LPBYTE        mTableData;
  /*
    Data layout:
    if((mEventModeFlags & 0x0001) == 0)
      layout A: SLLLWWWWSLLLWWWWSLLLWWWW...
    if((mEventModeFlags & 0x0001) == 0x0001)
      layout B: SLLLWWWW<channels>SLLLWWWW<channels>SLLLWWWW<channels>...
    S:    status flags
          0x4: 1: new label (alloc)
          0x8: 1: this label has been removed (dealloc)
    LLL:  label number
    WWWW: label width
    <channels>: list of 16-bit channel numbers (size = label width)
  */
} VBLMOST150AllocTab;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOST50Message_t
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
  /* mHeader.mObjectSize = sizeof(VBLMOST50Message) + mMsgLen; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDir;                       /* direction: 0: Rx; 1: Tx; 2: TxRequest */
  BYTE             mDummy1;
  DWORD            mSourceAdr;                 /* source address */
  DWORD            mDestAdr;                   /* target address */
  BYTE             mTransferType;              /* 1: node message; 2: spy message*/
  BYTE             mState;                     /* transmission status */
  BYTE             mAckNack;                   /* acknowledge code */
  BYTE             mDummy2;
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  BYTE             mDummy3;                    /* reserved */
  BYTE             mDummy4;                    /* reserved */
  BYTE             mPriority;                  /* priority of the message */
  BYTE             mDummy5;                    /* reserved */
  DWORD            mMsgLen;                    /* length of variable data in bytes (17 max) */
  BL_LPBYTE        mMsg;                       /* variable data */
} VBLMOST50Message;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLMOST50Pkt_t
{
  VBLObjectHeader2 mHeader;                    /* object header - NOTE! set the object size to */
  /* mHeader.mObjectSize = sizeof(VBLMOST50Pkt) + mPktDataLength; */
  WORD             mChannel;                   /* application channel */
  BYTE             mDir;                       /* direction: 0: Rx; 1: Tx; 2: TxRequest */
  BYTE             mDummy1;
  DWORD            mSourceAdr;                 /* source address */
  DWORD            mDestAdr;                   /* target address */
  BYTE             mTransferType;              /* 1: node message; 2: spy message*/
  BYTE             mState;                     /* transmission status */
  BYTE             mAckNack;                   /* acknowledge code */
  BYTE             mDummy2;
  DWORD            mCRC;                       /* Cyclic Redundancy Check */
  BYTE             mDummy3;                    /* reserved */
  BYTE             mDummy4;                    /* reserved */
  BYTE             mPriority;                  /* priority of the message */
  BYTE             mDummy5;                    /* reserved */
  DWORD            mPktDataLength;             /* length of variable data in bytes (1014 max) */
  BL_LPBYTE        mPktData;                   /* variable data */
} VBLMOST50Pkt;


/*----------------------------------------------------------------------------
|
| LIN extended objects
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINBusEvent_t
{
  ULONGLONG       mSOF;                        /* Start Of Frame timestamp */
  DWORD           mEventBaudrate;              /* Baudrate of the event in bit/sec */
  WORD            mChannel;                    /* application channel */
  BYTE            mReserved[2];                /* 4-byte alignment */

} VBLLINBusEvent;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINSynchFieldEvent_t
{
  VBLLINBusEvent  mLinBusEvent;                 /* LIN object header */
  ULONGLONG       mSynchBreakLength;            /* Sync Break Length in ns */
  ULONGLONG       mSynchDelLength;              /* Sync Delimiter Length in ns */
} VBLLINSynchFieldEvent;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINMessageDescriptor_t
{
  VBLLINSynchFieldEvent  mLinSynchFieldEvent;   /* LIN object header */
  WORD            mSupplierID;                  /* LIN Sub-Identifier - Supplier ID */
  WORD            mMessageID;                   /* LIN Sub-Identifier - Message ID (16 bits) */
  BYTE            mNAD;                         /* LIN Sub-Identifier - NAD */
  BYTE            mID;                          /* LIN ID */
  BYTE            mDLC;                         /* LIN DLC */
  BYTE            mChecksumModel;               /* LIN checksum model */
} VBLLINMessageDescriptor;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINDatabyteTimestampEvent_t
{
  VBLLINMessageDescriptor mLinMsgDescrEvent;       /* object header */
  ULONGLONG               mDatabyteTimestamps[9];  /* Databyte timestamps, where d[0] = EndOfHeader, d[1]=EndOfDataByte1, ..., d[8]=EndOfDataByte8 */
} VBLLINDatabyteTimestampEvent;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINMessage2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINDatabyteTimestampEvent  mLinTimestampEvent;  /* LIN object header */
  BYTE            mData[8];                    /* data bytes. */
  WORD            mCRC;                        /* checksum byte */
  BYTE            mDir;                        /* direction */
  BYTE            mSimulated;                  /* simulated frame */
  BYTE            mIsETF;                      /* Event-triggered frame */
  BYTE            mETFAssocIndex;              /* Unconditional frame associated with ETF - serial index */
  BYTE            mETFAssocETFId;              /* Unconditional frame associated with ETF - id of ETF */
  BYTE            mFSMId;                      /* t.b.d. */
  BYTE            mFSMState;                   /* t.b.d. */
  BYTE            mReserved[3];                /* 4-byte alignment */
  DWORD           mRespBaudrate;               /* Response baudrate of the event in bit/sec */
  DOUBLE          mExactHeaderBaudrate;        /* Exact baudrate of the header in bit/sec */
  DWORD           mEarlyStopbitOffset;         /* Early stop bit offset for UART timestamps in ns */
  DWORD           mEarlyStopbitOffsetResponse; /* Early stop bit offset in frame response for UART timestamps in ns */
} VBLLINMessage2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINSendError2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINMessageDescriptor mLinMsgDescrEvent;   /* LIN object header */
  ULONGLONG       mEOH;                        /* EndOfHeader timestamp */
  BYTE            mIsETF;                      /* Event-triggered frame */
  BYTE            mFSMId;                      /* t.b.d. */
  BYTE            mFSMState;                   /* t.b.d. */
  BYTE            mReserved;                   /* 4-byte alignment */
  BYTE            mReserved2[4];               /* 4-byte alignment, reserved since BLF 3.9.3.0, BLF files from older versions may have junk data here */
  DOUBLE          mExactHeaderBaudrate;        /* Exact baudrate of the header in bit/sec */
  DWORD           mEarlyStopbitOffset;         /* Early stop bit offset for UART timestamps in ns */
} VBLLINSendError2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINSyncError2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINSynchFieldEvent  mLinSynchFieldEvent;  /* LIN object header */
  WORD            mTimeDiff[4];                /* t.b.d */
} VBLLINSyncError2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINCRCError2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINDatabyteTimestampEvent  mLinTimestampEvent;  /* LIN object header */
  BYTE            mData[8];                    /* data bytes. */
  WORD            mCRC;                        /* checksum byte */
  BYTE            mDir;                        /* direction */
  BYTE            mFSMId;                      /* t.b.d. */
  BYTE            mFSMState;                   /* t.b.d. */
  BYTE            mSimulated;                  /* simulated frame */
  BYTE            mReserved[2];                /* 4-byte alignment */
  DWORD           mRespBaudrate;               /* Response baudrate of the event in bit/sec */
  BYTE            mReserved2[4];               /* 4-byte alignment, reserved since BLF 3.9.3.0, BLF files from older versions may have junk data here */
  DOUBLE          mExactHeaderBaudrate;        /* Exact baudrate of the header in bit/sec */
  DWORD           mEarlyStopbitOffset;         /* Early stop bit offset for UART timestamps in ns */
  DWORD           mEarlyStopbitOffsetResponse; /* Early stop bit offset in frame response for UART timestamps in ns */
} VBLLINCRCError2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINReceiveError2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINDatabyteTimestampEvent  mLinTimestampEvent;  /* LIN object header */
  BYTE            mData[8];                    /* data bytes. */
  BYTE            mFSMId;                      /* t.b.d. */
  BYTE            mFSMState;                   /* t.b.d. */
  BYTE            mStateReason;                /* t.b.d. */
  BYTE            mOffendingByte;              /* t.b.d. */
  BYTE            mShortError;                 /* t.b.d. */
  BYTE            mTimeoutDuringDlcDetection;  /* t.b.d. */
  BYTE            mIsETF;                      /* ETF collision flag */
  BYTE            mHasDatabytes;               /* t.b.d. */
  DWORD           mRespBaudrate;               /* Response baudrate of the event in bit/sec */
  BYTE            mReserved[4];                /* 4-byte alignment, reserved since BLF 3.9.3.0, BLF files from older versions may have junk data here */
  DOUBLE          mExactHeaderBaudrate;        /* Exact baudrate of the header in bit/sec */
  DWORD           mEarlyStopbitOffset;         /* Early stop bit offset for UART timestamps in ns */
  DWORD           mEarlyStopbitOffsetResponse; /* Early stop bit offset in frame response for UART timestamps in ns */
} VBLLINReceiveError2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINWakeupEvent2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINBusEvent  mLinBusEvent;                /* LIN object header */
  BYTE            mLengthInfo;                 /* t.b.d */
  BYTE            mSignal;                     /* t.b.d */
  BYTE            mExternal;                   /* t.b.d */
  BYTE            mReserved;                   /* 4-byte alignment */
} VBLLINWakeupEvent2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINSpikeEvent2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINBusEvent  mLinBusEvent;                /* LIN object header */
  ULONG           mWidth;                      /* the spike's width in microseconds */
  BYTE            mInternal;                   /* t.b.d */
  BYTE            mReserved[3];                /* 4-byte alignment */
} VBLLINSpikeEvent2;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLLINLongDomSignalEvent_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINBusEvent  mLinBusEvent;                /* LIN object header */
  BYTE            mType;                       /* t.b.d */
  BYTE            mReserved[3];                /* 4-byte alignment */
} VBLLINLongDomSignalEvent;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINLongDomSignalEvent2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINBusEvent  mLinBusEvent;                /* LIN object header */
  BYTE            mType;                       /* t.b.d */
  BYTE            mReserved[7];                /* 4-byte alignment */
  ULONGLONG       mLength;
} VBLLINLongDomSignalEvent2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINUnexpectedWakeup_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINBusEvent  mLinBusEvent;                /* LIN object header */
  ULONGLONG       mWidth;                      /* width of the unexpected wakeup in nanoseconds (valid for LIN 2.x) */
  BYTE            mSignal;                     /* byte signal of the unexpected wakeup (valid for LIN 1.x) */
  BYTE            mReserved[7];                /* 8-byte alignment */
} VBLLINUnexpectedWakeup;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINShortOrSlowResponse_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINDatabyteTimestampEvent  mLinTimestampEvent;  /* LIN object header */
  ULONG           mNumberOfRespBytes;          /* number of valid response bytes */
  BYTE            mRespBytes[9];               /* the response bytes (can include the checksum) */
  BYTE            mSlowResponse;               /* non-zero, if the response was too slow */
  BYTE            mInterruptedByBreak;         /* non-zero, if the response was interrupted by a sync break */
  BYTE            mReserved[1];                /* 8-byte alignment */
} VBLLINShortOrSlowResponse;

typedef struct VBLLINShortOrSlowResponse2_t
{
  VBLObjectHeader mHeader;                     /* object header */
  VBLLINDatabyteTimestampEvent  mLinTimestampEvent;  /* LIN object header */
  ULONG           mNumberOfRespBytes;          /* number of valid response bytes */
  BYTE            mRespBytes[9];               /* the response bytes (can include the checksum) */
  BYTE            mSlowResponse;               /* non-zero, if the response was too slow */
  BYTE            mInterruptedByBreak;         /* non-zero, if the response was interrupted by a sync break */
  BYTE            mReserved[1];                /* 8-byte alignment */
  DOUBLE          mExactHeaderBaudrate;        /* Exact baudrate of the header in bit/sec */
  DWORD           mEarlyStopbitOffset;         /* Early stop bit offset for UART timestamps in ns */
} VBLLINShortOrSlowResponse2;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLLINDisturbanceEvent_t
{
  VBLObjectHeader mHeader;                     /* object header */
  WORD            mChannel;                    /* application channel */
  BYTE            mID;                         /* LIN ID of disturbed response */
  BYTE            mDisturbingFrameID;          /* LIN ID of disturbing header */
  ULONG           mDisturbanceType;            /* type of disturbance (dominant, recessive, header, bitstream, variable bitstream) */
  ULONG           mByteIndex;                  /* index of the byte that was disturbed */
  ULONG           mBitIndex;                   /* index of the bit that was disturbed */
  ULONG           mBitOffsetInSixteenthBits;   /* offset in 1/16th bits into the disturbed bit */
  ULONG           mDisturbanceLengthInSixteenthBits; /* length of the disturbance in units of 1/16th bit */
} VBLLINDisturbanceEvent;

/*----------------------------------------------------------------------------
|
| FLEXRAY objects
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYData_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mMUX;                        /* t.b.d */
    BYTE            mLen;                        /* t.b.d */
    WORD            mMessageID;                  /* t.b.d */
    WORD            mCRC;                        /* t.b.d */
    BYTE            mDir;                        /* t.b.d */
    BYTE            mDummy1;                     /* t.b.d */
    WORD            mDummy2;                     /* t.b.d */
    BYTE            mDataBytes[12];              /* t.b.d */
} VBLFLEXRAYData;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYSync_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mMUX;                        /* t.b.d */
    BYTE            mLen;                        /* t.b.d */
    WORD            mMessageID;                  /* t.b.d */
    WORD            mCRC;                        /* t.b.d */
    BYTE            mDir;                        /* t.b.d */
    BYTE            mDummy1;                     /* t.b.d */
    WORD            mDummy2;                     /* t.b.d */
    BYTE            mDataBytes[11];              /* t.b.d */
    BYTE            mCycle;                      /* t.b.d */
} VBLFLEXRAYSync;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYV6StartCycleEvent_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mDir;                        /* dir flag (tx, rx) */
    BYTE            mLowTime;                    /* additional time field in simulation */
    DWORD           mFPGATick;                   /* timestamp generated from xModule */
    DWORD           mFPGATickOverflow;           /* overflow counter of the timestamp */
    DWORD           mClientIndex;                /* clientindex of send node */
    DWORD           mClusterTime;                /* relatvie clustertime, from 0 to cyclelength*/
    BYTE            mDataBytes[2];               /* array of databytes*/
    WORD            mReserved;
} VBLFLEXRAYV6StartCycleEvent;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYV6Message_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mDir;                        /* dir flag (tx, rx) */
    BYTE            mLowTime;                    /* additional time field in simulation */
    DWORD           mFPGATick;                   /* timestamp generated from xModule */
    DWORD           mFPGATickOverflow;           /* overflow counter of the timestamp */
    DWORD           mClientIndex;                /* clientindex of send node */
    DWORD           mClusterTime;                /* relatvie clustertime, from 0 to cyclelength*/
    WORD            mFrameId;                    /* slot identifier, word  */
    WORD            mHeaderCRC;                  /*                  word  */
    WORD            mFrameState;                 /* V6 framestate          */
    BYTE            mLength;                     /* dlc of message,  byte  */
    BYTE            mCycle;                      /* current cycle,   byte  */
    BYTE            mHeaderBitMask;              /* Bit0 = NMBit, Bit1 = SyncBit, Bit2 = Reserved */
    BYTE            mReserved1;
    WORD            mReserved2;
    BYTE            mDataBytes[64];              /* array of databytes*/
} VBLFLEXRAYV6Message;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYStatusEvent_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mVersion;                    /* object version */
    WORD            mStatusType;                 /* type of status event */
    WORD            mInfoMask1;                  /* additional info 1 */
    WORD            mInfoMask2;                  /* additional info 2 */
    WORD            mInfoMask3;                  /* additional info 3 */
    WORD            mReserved[16];
} VBLFLEXRAYStatusEvent;

// New FlexRay events
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYVFrReceiveMsgEx_t
{
  VBLObjectHeader mHeader;                     /* object header */
  WORD            mChannel;                    /* application channel */
  WORD            mVersion;                    /* version of data struct */
  WORD            mChannelMask;                /* channel mask */
  WORD            mDir;                        /* dir flag (tx, rx) */
  DWORD           mClientIndex;                /* clientindex of send node */
  DWORD           mClusterNo;                  /* number of cluster */
  WORD            mFrameId;                    /* slot identifier, word  */
  WORD            mHeaderCRC1;                 /* header crc channel 1  */
  WORD            mHeaderCRC2;                 /* header crc channel 2  */
  WORD            mByteCount;                  /* byte count (not payload) of frame from CC receive buffer */
  WORD            mDataCount;                  /* length of the data array (stretchy struct) */
  WORD            mCycle;                      /* current cycle, byte  */
  DWORD           mTag;                        /* type of cc */
  DWORD           mData;                       /* register flags */
  DWORD           mFrameFlags;                 /* frame flags */
  DWORD           mAppParameter;               /* TxRq, TxAck flags */
  DWORD           mFrameCRC;                   /* frame crc */
  DWORD           mFrameLengthNS;              /* length of frame in ns */
  WORD            mFrameId1;                   /* for internal use */
  WORD            mPDUOffset;                  /* payload offset (position in a frame) */
  WORD            mBlfLogMask;                 /* only valid for frames. Every stands for one PDU. If set, the PDU must be extracted out of the frame. The bit order is the PDU order in the frame starting with the PDU with the smallest offset */
  WORD            mReservedW;
  DWORD           mReserved[6];                /* reserved */
  BYTE            mDataBytes[254];             /* array of databytes*/
} VBLFLEXRAYVFrReceiveMsgEx;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYVFrReceiveMsg_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mVersion;                    /* version of data struct */
    WORD            mChannelMask;                /* channel mask */
    BYTE            mDir;                        /* dir flag (tx, rx) */
    DWORD           mClientIndex;                /* clientindex of send node */
    DWORD           mClusterNo;                  /* number of cluster */
    WORD            mFrameId;                    /* slot identifier, word  */
    WORD            mHeaderCRC1;                 /* header crc channel 1  */
    WORD            mHeaderCRC2;                 /* header crc channel 2  */
    WORD            mByteCount;                  /* byte count (not payload) of frame from CC receive buffer */
    WORD            mDataCount;                  /* length of the data array (stretchy struct) */
    BYTE            mCycle;                      /* current cycle, byte  */
    DWORD           mTag;                        /* type of cc */
    DWORD           mData;                       /* register flags */
    DWORD           mFrameFlags;                 /* frame flags */
    DWORD           mAppParameter;               /* TxRq, TxAck flags */
    BYTE            mDataBytes[254];             /* array of databytes*/
} VBLFLEXRAYVFrReceiveMsg;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYVFrStartCycle_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mVersion;                    /* version of data struct */
    WORD            mChannelMask;                /* channel mask */
    BYTE            mDir;                        /* dir flag (tx, rx) */
    BYTE            mCycle;                      /* current cycle,   byte  */
    DWORD           mClientIndex;                /* clientindex of send node */
    DWORD           mClusterNo;                  /* number of cluster */
    WORD            mNmSize;                     /* size of NM Vector */
    BYTE            mDataBytes[12];              /* array of databytes (NM vector max. length)*/
    DWORD           mTag;                        /* type of cc */
    DWORD           mData[5];                    /* register flags */
    WORD            mReserved;
} VBLFLEXRAYVFrStartCycle;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYVFrStatus_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mVersion;                    /* object version */
    WORD            mChannelMask;                /* channel mask */
    BYTE            mCycle;                      /* current cycle,   byte */
    DWORD           mClientIndex;                /* clientindex of send node */
    DWORD           mClusterNo;                  /* number of cluster */
    DWORD           mWus;                        /* wakeup status */
    DWORD           mCcSyncState;                /* sync state of cc */
    DWORD           mTag;                        /* type of cc */
    DWORD           mData[2];                    /* register flags */
    WORD            mReserved[16];
} VBLFLEXRAYVFrStatus;

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLFLEXRAYVFrError_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mVersion;                    /* object version */
    WORD            mChannelMask;                /* channel mask */
    BYTE            mCycle;                      /* current cycle, byte */
    DWORD           mClientIndex;                /* clientindex of send node */
    DWORD           mClusterNo;                  /* number of cluster */
    DWORD           mTag;                        /* type of cc */
    DWORD           mData[4];                    /* register flags */
    WORD            mReserved;
} VBLFLEXRAYVFrError;

/*----------------------------------------------------------------------------
|
| J1708 message object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLJ1708Message_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    BYTE            mDir;                        /* direction */
    WORD            mError;                      /* error code */
    BYTE            mSize;                       /* data size */
    BYTE            mData[255];                  /* data */
} VBLJ1708Message;

/*----------------------------------------------------------------------------
|
| Environment variable object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLEnvironmentVariable_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                                 /* */
                                                 /* mHeader.mObjectSize = sizeof( VBLEnvironmentVariable) + mNameLength + mDataLength; */
                                                 /* */
    DWORD           mNameLength;                 /* length of variable name in bytes */
    DWORD           mDataLength;                 /* length of variable data in bytes */
    BL_LPSTR        mName;                       /* variable name in MBCS */
    BL_LPBYTE       mData;                       /* variable data */
} VBLEnvironmentVariable;

/*----------------------------------------------------------------------------
|
| System variable object
|
-----------------------------------------------------------------------------*/

#define BL_SYSVAR_TYPE_DOUBLE       1
#define BL_SYSVAR_TYPE_LONG         2
#define BL_SYSVAR_TYPE_STRING       3
#define BL_SYSVAR_TYPE_DOUBLEARRAY  4
#define BL_SYSVAR_TYPE_LONGARRAY    5
#define BL_SYSVAR_TYPE_LONGLONG     6
#define BL_SYSVAR_TYPE_BYTEARRAY    7

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLSystemVariable_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                                 /* */
                                                 /* mHeader.mObjectSize = sizeof( VBLSystemVariable) + mNameLength + mDataLength; */
                                                 /* */
    DWORD           mType;                       /* type of system variable (see BL_SYSVAR_TYPE_xxx) */
    DWORD           mRepresentation;             /* signed, later perhaps also string codepage */
    DWORD           mReserved[2];
    DWORD           mNameLength;                 /* length of variable name in bytes */
    DWORD           mDataLength;                 /* length of variable data in bytes */
    BL_LPSTR        mName;                       /* variable name in MBCS */
    BL_LPBYTE       mData;                       /* variable data */
} VBLSystemVariable;

/*----------------------------------------------------------------------------
|
| GPS event object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLGPSEvent_t
{
    VBLObjectHeader mHeader;                     /* object header */
    DWORD           mFlags;                      /* t.b.d. */
    WORD            mChannel;                    /* channel of event */
    WORD            mReserved;                   /* t.b.d. */
    DOUBLE          mLatitude;                   /* t.b.d. */
    DOUBLE          mLongitude;                  /* t.b.d. */
    DOUBLE          mAltitude;                   /* t.b.d. */
    DOUBLE          mSpeed;                      /* t.b.d. */
    DOUBLE          mCourse;                     /* t.b.d. */
} VBLGPSEvent;

/*----------------------------------------------------------------------------
|
| Serial event object
|
-----------------------------------------------------------------------------*/

#define BL_SERIAL_TYPE_KLINE_EVENT   0x000000001
#define BL_SERIAL_TYPE_DIAG_REQUEST  0x000000002 /* only valid if BL_SERIAL_TYPE_KLINE_EVENT is set */
#define BL_SERIAL_TYPE_SINGLE_BYTE   0x000000004 /* optimization for logging single bytes */
#define BL_SERIAL_TYPE_COMPACT_BYTES 0x000000008 /* optimization for logging a few bytes without additional timestamps */

/* HINT: this struct might be extended in future versions! */
typedef struct VBLGeneralSerialEvent_t
{
    DWORD           mDataLength;                 /* length of variable data in bytes */
    DWORD           mTimeStampsLength;           /* length of variable timestamps in bytes */
    BL_LPBYTE       mData;                       /* variable data */
    BL_PLONGLONG    mTimeStamps;                 /* variable timestamps (optional) */
} VBLGeneralSerialEvent;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLSingleByteSerialEvent_t
{
    BYTE            mByte;
} VBLSingleByteSerialEvent;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLCompactSerialEvent_t
{
    BYTE            mCompactLength;
    BYTE            mCompactData[15];
} VBLCompactSerialEvent;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLSerialEvent_t
{
    VBLObjectHeader mHeader;                     /* object header */
    DWORD           mFlags;                      /* see above */
    DWORD           mPort;                       /* channel of event */
    DWORD           mBaudrate;                   /* baudrate at which this event was transmitted (optional) */
    DWORD           mReserved;                   /* t.b.d. */
    union
    {
        VBLGeneralSerialEvent    mGeneral;
        VBLSingleByteSerialEvent mSingleByte;
        VBLCompactSerialEvent    mCompact;
    };
} VBLSerialEvent;


#define BL_KLINE_TYPE_TOECU      0x8000       /* If set in mType, direction is tester -> ECU */
#define BL_KLINE_TYPE_MASK       0x7FFF       /* Use this mask to filter out the type from mType */
/* HINT: this struct might be extended in future versions! */
typedef struct VBLKLineStatusEvent_t
{
  VBLObjectHeader mHeader;                    /* object header */
  WORD            mType;                      /* BusSystemFacility::VKLineStatusEventType */
  WORD            mDataLen;                   /* number of *bytes* used in mData */
  DWORD           mPort;                      /* channel of event */
  DWORD           mReserved;                  /* t.b.d. */
  UINT64          mData[3];                   /* the actual data, but only mDataLen BYTES are used! */
} VBLKLineStatusEvent;

/*----------------------------------------------------------------------------
|
| Ethernet frame object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLEthernetFrame_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                                 /* mHeader.mObjectSize = sizeof( VBLEthernetFrame) + mPayLoadLength; */
    BYTE            mSourceAddress[6];
    WORD            mChannel;
    BYTE            mDestinationAddress[6];
    WORD            mDir;                        /* Direction flag: 0=Rx, 1=Tx, 2=TxRq */
    WORD            mType;
    WORD            mTPID;
    WORD            mTCI;
    WORD            mPayLoadLength;              /* Number of valid mPayLoad bytes */
    BL_LPBYTE       mPayLoad;                    /* Max 1582 (1600 packet length - 18 header) data bytes per frame  */
} VBLEthernetFrame;

/*----------------------------------------------------------------------------
|
| Ethernet frame2 object / Ethernet frame forwarded object
|
-----------------------------------------------------------------------------*/
typedef struct VBLEthernetFrameEx_t
{
  VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                               /* mHeader.mObjectSize = offsetof(VBLEthernetFrame, mFrameData) + mPayLoadLength; */
  WORD            mStructLength;               /* Length of this structure, without sizeof(VBLObjectHeader), without mStructLength member and without mFrameData member */
                                               /* mStructLength = sizeof(VBLEthernetFrameEx) - sizeof(VBLObjectHeader) - sizeof(mStructLength) - sizeof(mFrameData) */
  WORD            mFlags;                      /* flags, which indicates the valid fields: */
                                               /*   Bit 0 - reserved */
                                               /*   Bit 1 - mHardwareChannel valid */
                                               /*   Bit 2 - mFrameDuration valid */
                                               /*   Bit 3 - mFrameChecksum valid */
                                               /*   Bit 4 - mFrameHandle valid */
  WORD            mChannel;                    /* application channel, i.e. Eth 1 */
  WORD            mHardwareChannel;            /* HW channel */
  UINT64          mFrameDuration;              /* Transmission duration in [ns] */             
  DWORD           mFrameChecksum;              /* Ethernet checksum */
  WORD            mDir;                        /* Direction flag: 0=Rx, 1=Tx, 2=TxRq */
  WORD            mFrameLength;                /* Number of valid mFrameData bytes */
  DWORD           mFrameHandle;                /* Handle which refer the corresponding VBLEthernetFrameForwarded event */
  DWORD           mReserved;
  BL_LPBYTE       mFrameData;                  /* Max 1612 data bytes per frame. Contains Ethernet header + Ethernet payload  */
} VBLEthernetFrameEx, VBLEthernetFrameForwarded;

/*----------------------------------------------------------------------------
|
| Ethernet RX error
|
-----------------------------------------------------------------------------*/

typedef struct VBLEthernetRxError_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                                 /* mHeader.mObjectSize = offsetof(VBLEthernetRxError, mError) + sizeof(event.mError) + event.mFrameDataLength */
    WORD            mStructLength;               /* Length of this structure, without sizeof(VBLObjectHeader) and without raw data length */
                                                 /* mStructLength = sizeof(VBLEthernetRxError) - sizeof(VBLObjectHeader) - sizeof(DWORD) */
    WORD            mChannel;
    WORD            mDir;                        /* Direction flag: 0=Rx, 1=Tx, 2=TxRq */
    WORD            mHardwareChannel;            /* HW channel. 0 = invalid. */
    DWORD           mFcs;                        /* Frame Check Sum */
    WORD            mFrameDataLength;            /* Number of valid raw ethernet data bytes, starting with Target MAC ID */
    WORD            mReserved2;                  /* Gap */
    DWORD           mError;
    BL_LPBYTE       mFrameData;                  /* Max 1600 data bytes per frame  */
} VBLEthernetRxError;

/*----------------------------------------------------------------------------
|
| Ethernet error / Ethernet error forwarded
|
-----------------------------------------------------------------------------*/

typedef struct VBLEthernetErrorEx_t
{
  VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                               /* mHeader.mObjectSize = offsetof(VBLEthernetRxError, mFrameData) + event.mFrameDataLength */
  WORD            mStructLength;               /* Length of this structure, without sizeof(VBLObjectHeader), without mStructLength member and without mFrameData member */
                                               /* mStructLength = sizeof(VBLEthernetErrorEx) - sizeof(VBLObjectHeader) - sizeof(mStructLength) - sizeof(mFrameData) */
  WORD            mFlags;                      /* flags, which indicates the valid fields: */
                                               /*   Bit 0 - reserved */
                                               /*   Bit 1 - mHardwareChannel valid */
                                               /*   Bit 2 - mFrameDuration valid */
                                               /*   Bit 3 - mFrameChecksum valid */
                                               /*   Bit 4 - mFrameHandle valid */
  WORD            mChannel;                    /* application channel, i.e. Eth 1 */
  WORD            mHardwareChannel;            /* HW channel */
  UINT64          mFrameDuration;              /* Transmission duration in [ns] */
  DWORD           mFrameChecksum;              /* Ethernet checksum */
  WORD            mDir;                        /* Direction flag: 0=Rx, 1=Tx, 2=TxRq */
  WORD            mFrameLength;                /* Number of valid mFrameData bytes */
  DWORD           mFrameHandle;                /* Handle which refer the corresponding VBLEthernetErrorEx event */
  DWORD           mError;
  BL_LPBYTE       mFrameData;                  /* Max 1612 data bytes per frame. Contains Ethernet header + Ethernet payload  */
} VBLEthernetErrorEx, VBLEthernetErrorForwarded;

/*----------------------------------------------------------------------------
|
| Ethernet Status
|
-----------------------------------------------------------------------------*/

typedef struct VBLEthernetStatus_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;
    WORD            mFlags;                      /* Valid fields: */
                                                 /* Bit 0 - Link Status */
                                                 /* Bit 1 - Bitrate */
                                                 /* Bit 2 - Ethernet Phy */
                                                 /* Bit 3 - Duplex */
                                                 /* Bit 4 - MDI Type */
                                                 /* Bit 5 - Connector */
                                                 /* Bit 6 - Clock Mode */
                                                 /* Bit 7 - BR Pair */
                                                 /* Bit 8 - mHardwareChannel */
    BYTE            mLinkStatus;                 /* Link Status:   */
                                                 /* 0 - Unknown    */
                                                 /* 1 - Link down  */
                                                 /* 2 - Link up    */
                                                 /* 3 - Negotiate  */
                                                 /* 4 - Link error */
    BYTE            mEthernetPhy;                /* Ethernet Phy:     */
                                                 /* 0 - Unknown      */
                                                 /* 1 - IEEE 802.3   */
                                                 /* 2 - BroadR-Reach */
    BYTE            mDuplex;                     /* Duplex: */
                                                 /* 0 - Unknown     */
                                                 /* 1 - Half Duplex */
                                                 /* 2 - Full Duplex */
    BYTE            mMdi;                        /* 0 - Unknown */
                                                 /* 1 - Direct*/
                                                 /* 2 - Crossover */
    BYTE            mConnector;                  /* 0 - Unknown */
                                                 /* 1 - RJ45*/
                                                 /* 2 - D-Sub */
    BYTE            mClockMode;                  /* 0 - Unknown */
                                                 /* 1 - Master */
                                                 /* 2 - Slave */
    BYTE            mPairs;                      /* 0 - Unknown */
                                                 /* 1 - BR 1-pair*/
                                                 /* 2 - BR 2-pair */
                                                 /* 3 - BR 4-pair */
    BYTE            mHardwareChannel;
    DWORD           mBitrate;                    /* Bitrate in [kbit/sec] */
} VBLEthernetStatus;

/*----------------------------------------------------------------------------
|
| Ethernet Statistics
|
-----------------------------------------------------------------------------*/

typedef struct VBLEthernetStatistic_t
{
  VBLObjectHeader     mHeader;                     /* object header */
  WORD                mChannel;
  UINT64              mRcvOk_HW;
  UINT64              mXmitOk_HW;
  UINT64              mRcvError_HW;
  UINT64              mXmitError_HW;
  UINT64              mRcvBytes_HW;
  UINT64              mXmitBytes_HW;
  UINT64              mRcvNoBuffer_HW;
  SHORT               mSQI;
  WORD                mHardwareChannel;
} VBLEthernetStatistic;

/*----------------------------------------------------------------------------
|
| WLAN frame object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLWlanFrame_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                                 /* mHeader.mObjectSize = sizeof( VBLWlanFrame_t) + mFrameLength; */
    WORD            mChannel;                    /* application channel 1..n */
    WORD            mFlags;                      /* Bit0=Genuine MAC-Header, Bit1=Correct Frame Control Format */
    BYTE            mDir;                        /* Direction flag: 0=Rx, 1=Tx, 2=TxRq */
    BYTE            mRadioChannel;               /* channel number of the radio frequencey */
    SHORT           mSignalStrength;             /* signal strength in [dbm] */
    WORD            mSignalQuality;              /* signal quality in [dbm] */
    WORD            mFrameLength;                /* Number of bytes (header + payload) */
    BL_LPBYTE       mFrameData;                  /* Max. 2342 data bytes per frame  */
} VBLWlanFrame;

/* HINT: this struct might be extended in future versions! */
typedef struct VBLWlanStatistic_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mFlags;                      /* Bit0=Valid Rx/Tx Counter, Bit1=Valid Error Counter */
    ULONG           mRxPacketCount;
    ULONG           mRxByteCount;
    ULONG           mTxPacketCount;
    ULONG           mTxByteCount;
    ULONG           mCollisionCount;
    ULONG           mErrorCount;
} VBLWlanStatistic;

/*----------------------------------------------------------------------------
|
| AFDX frame object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLAfdxFrame_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                                 /* mHeader.mObjectSize = sizeof( VBLEthernetFrame) + mPayLoadLength; */
    BYTE            mSourceAddress[6];
    WORD            mChannel;
    BYTE            mDestinationAddress[6];
    WORD            mDir;                        /* Direction flag: 0=Rx, 1=Tx, 2=TxRq */
    WORD            mType;
    WORD            mTPID;
    WORD            mTCI;
    BYTE            mEthChannel;
    WORD            mAfdxFlags;
    ULONG           mBAGusec;
    WORD            mPayLoadLength;              /* Number of valid mPayLoad bytes */
    BL_LPBYTE       mPayLoad;                    /* Max 1582 (1600 packet length - 18 header) data bytes per frame  */
} VBLAfdxFrame;

/*----------------------------------------------------------------------------
|
| AFDX combined bus- and VL- statistic event; used before 8.2
|
-----------------------------------------------------------------------------*/
/* HINT: this struct might be extended in future versions! */
typedef struct VBLAfdxStatistic_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mFlags;                      /* Bit 0 - channel is configured */
                                                 /* Bit 1 - HW related counters valid */
												                         /* Bit 2 - CANwin related counters are valid */
												                         /* Bit 3 - link-related info is valid */
												                         /* Bit 4 - invalid packet counter is valid */
												                         /* Bit 5 - lost packet counter is valid */
												                         /* Bit 6 - dropped packet counter is valid */
												                         /* Bit 7 - byte counters are based on CANwin packets, not HW */
    ULONG           mRxPacketCount;
    ULONG           mRxByteCount;
    ULONG           mTxPacketCount;
    ULONG           mTxByteCount;
    ULONG           mCollisionCount;
    ULONG           mErrorCount;
    ULONG           mStatDroppedRedundantPacketCount;
    ULONG           mStatRedundantErrorPacketCount;
    ULONG           mStatIntegrityErrorPacketCount;
    ULONG           mStatAvrgPeriodMsec;
    ULONG           mStatAvrgJitterMysec;
    ULONG           mVLId;
    ULONG           mStatDuration;
} VBLAfdxStatistic;

/*----------------------------------------------------------------------------
|
| AFDX line-specific bus-statistic event used since 8.2
|
-----------------------------------------------------------------------------*/
/* HINT: this struct might be extended in future versions! */
typedef struct VBLAfdxBusStatistic_t
{
    VBLObjectHeader mHeader;                     /* object header */
    WORD            mChannel;                    /* application channel */
    WORD            mFlags;                      /* Bit0=Valid Rx/Tx Counter, Bit1=Valid Error Counter; Bit2=Valid VLId */
    ULONG           mStatDuration;               /* real time period in mysec of statistic datacollection */
    /* bus-specific info */
    ULONG           mStatRxPacketCountHW;        /* read frames taken from hardware, i.e. on bus */
    ULONG           mStatTxPacketCountHW;        /* send frames as taken from hardware, i.e. on bus */
    ULONG           mStatRxErrorCountHW;         /* number of erronous Rx-frames detected by HW */
    ULONG           mStatTxErrorCountHW;         /* number of erronous Tx-frames detected by HW */
    ULONG           mStatRxBytesHW;              /* bytes received by HW during this time period */
    ULONG           mStatTxBytesHW;              /* bytes sent by HW during this time period */
    /* CANwin specific info */
    ULONG           mStatRxPacketCount;          /* received frames within CANwin */
    ULONG           mStatTxPacketCount;          /* send packets from within CANwin */
    ULONG           mStatDroppedPacketCount;     /* number of packets aktively dropped by CANwin */
    ULONG           mStatInvalidPacketCount;     /* number of packets with incompatible eth-header regarding AFDX-spec */
    ULONG           mStatLostPacketCount;        /* number of packets lost by CABwin due to queue overflow etc */
    /* connection related info */
    BYTE            mLine;                       /* lineA (0) or lineB (1) */
    BYTE            mLinkStatus;                 /* status of adapter as per EthernetStatus */
    WORD            mLinkSpeed;                  /* link speed: 0:=10mbps 1:=100mbps 2:=1000mbps */
    WORD            mLinkLost;                   /* counter of link-losses during this period */
} VBLAfdxBusStatistic;

/*----------------------------------------------------------------------------
|
| AFDX adapter status event, available since 8.2
|
-----------------------------------------------------------------------------*/
/* HINT: this struct might be extended in future versions! */
typedef struct VBLAfdxLineStatus_t               /* note: this should match specific items of VBLAEthernetStatus */
{
    WORD            mFlags;                      /* Valid fields: */
                                                 /* Bit 0 - Link Status */
                                                 /* Bit 1 - Bitrate */
                                                 /* Bit 2 - Ethernet Phy */
                                                 /* Bit 3 - Duplex */
    BYTE            mLinkStatus;                 /* Link Status:   */
                                                 /* 0 - Unknown    */
                                                 /* 1 - Link down  */
                                                 /* 2 - Link up    */
                                                 /* 3 - Negotiate  */
                                                 /* 4 - Link error */
    BYTE            mEthernetPhy;                /* Eternet Phy:     */
                                                 /* 0 - Unknown      */
                                                 /* 1 - IEEE 802.3   */
                                                 /* 2 - BroadR-Reach */
    BYTE            mDuplex;                     /* Duplex: */
                                                 /* 0 - Unknown     */
                                                 /* 1 - Half Duplex */
                                                 /* 2 - Full Duplex */
    BYTE            mMdi;                        /* 0 - Unknown */
                                                 /* 1 - Direct*/
                                                 /* 2 - Crossover */
    BYTE            mConnector;                  /* 0 - Unknown */
                                                 /* 1 - RJ45*/
                                                 /* 2 - D-Sub */
    BYTE            mClockMode;                  /* 0 - Unknown */
                                                 /* 1 - Master */
                                                 /* 2 - Slave */
    BYTE            mPairs;                      /* 0 - Unknown */
                                                 /* 1 - BR 1-pair*/
                                                 /* 2 - BR 2-pair */
                                                 /* 3 - BR 4-pair */
    BYTE            mReserved;
    DWORD           mBitrate;                    /* Bitrate in [kbit/sec] */
} VBLAfdxLineStatus;

typedef struct VBLAfdxStatus_t
{
    VBLObjectHeader   mHeader;                   /* object header */
    WORD              mChannel;                  /* application channel */
    VBLAfdxLineStatus mStatusA;                  /* status of adapter lineA */
    VBLAfdxLineStatus mStatusB;                  /* status of adapter lineB */
} VBLAfdxStatus;

/*----------------------------------------------------------------------------
|
| AFDX general error event, available since 8.5
|
-----------------------------------------------------------------------------*/
/* HINT: this struct might be extended in future versions! */
#define BL_AFDX_ERRORTEXT_LEN      512
typedef struct VBLAfdxErrorEvent_t
{
  VBLObjectHeader   mHeader;                   /* object header */
  WORD              mChannel;                  /* application channel */
  WORD              mErrorLevel;               /* Error Level, 0=error, 1=warning, 2=info*/
  ULONG             mSourceIdentifier;
  char              mErrorText[BL_AFDX_ERRORTEXT_LEN]; /* error events are rare, so no need to optimize storage */
  char              mErrorAttributes[BL_AFDX_ERRORTEXT_LEN];
} VBLAfdxErrorEvent;

/*----------------------------------------------------------------------------
|
| A429 message object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLA429Message_t
{
  VBLObjectHeader   mHeader;                  /* object header */
  BYTE              mA429Data[4];
  WORD              mChannel;
  BYTE              mDir;                     /* direction flag: 0=Rx, 1=Tx */
  BYTE              mReserved;
  ULONG             mBitrate;
  LONG              mErrReason;
  WORD              mErrPosition;
  ULONGLONG         mFrameGap;
  ULONG             mFrameLength;
  WORD              mMsgCtrl;
  ULONG             mCycleTime;
  ULONG             mError;                   /* reserved */
  ULONG             mBitLenOfLastBit;
} VBLA429Message;

/*----------------------------------------------------------------------------
|
| A429 error object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
#define BL_A429_ERRORTEXT_LEN      512
typedef struct VBLA429Error_t
{
  VBLObjectHeader   mHeader;                  /* object header */
  WORD              mChannel;                 /* application channel */
  WORD              mErrorType;               /* error type, 0=error, 1=warning, 2=info*/
  ULONG             mSourceIdentifier;
  ULONG             mErrReason;
  char              mErrorText[BL_A429_ERRORTEXT_LEN]; /* error events are rare, so no need to optimize storage */
  char              mErrorAttributes[BL_A429_ERRORTEXT_LEN];
} VBLA429Error;

/*----------------------------------------------------------------------------
|
| A429 status object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLA429Status_t
{
  VBLObjectHeader   mHeader;                  /* object header */
  WORD              mChannel;
  BYTE              mDir;                     /* direction flag: 0=Rx, 1=Tx */
  WORD              mParity;                  /* parity */
  ULONG             mMinGap;                  /* minimum gap */
  ULONG             mBitrate;                 /* Tx bit rate */
  ULONG             mMinBitrate;              /* Rx min bit rate */
  ULONG             mMaxBitrate;              /* Rx max bit rate */
} VBLA429Status;

/*----------------------------------------------------------------------------
|
| A429 bus statistic object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLA429BusStatistic_t
{
  VBLObjectHeader   mHeader;                  /* object header */
  WORD              mChannel;
  BYTE              mDir;                     /* direction flag: 0=Rx, 1=Tx */
  ULONG             mBusLoad;
  ULONG             mDataTotal;
  ULONG             mErrorTotal;
  ULONG             mBitrate;
  USHORT            mParityErrors;            /* error count (parity) */
  USHORT            mBitrateErrors;           /* error count (bit rate) */
  USHORT            mGapErrors;               /* error count (gaps) */
  USHORT            mLineErrors;              /* error count (lines) */
  USHORT            mFormatErrors;
  USHORT            mDutyFactorErrors;
  USHORT            mWordLenErrors;
  USHORT            mCodingErrors;
  USHORT            mIdleErrors;
  USHORT            mLevelErrors;
  USHORT            mLabelCount[256];
} VBLA429BusStatistic;

/*----------------------------------------------------------------------------
|
| Application trigger object
|
-----------------------------------------------------------------------------*/

#define BL_TRIGGER_FLAG_SINGLE_TRIGGER 0x00000000   /* single trigger type */
#define BL_TRIGGER_FLAG_LOGGING_START  0x00000001   /* start of logging trigger type */
#define BL_TRIGGER_FLAG_LOGGING_STOP   0x00000002   /* stop of logging trigger type */
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLAppTrigger_t
{
    VBLObjectHeader mHeader;                     /* object header */
    ULONGLONG       mPreTriggerTime;             /* pre-trigger time */
    ULONGLONG       mPostTriggerTime;            /* post-trigger time */
    WORD            mChannel;                    /* channel of event which triggered (if any) */
    WORD            mFlags;                      /* trigger type (see above) */
    DWORD           mAppSecific2;                /* app specific member 2 */
} VBLAppTrigger;

/*----------------------------------------------------------------------------
|
| Application text object
|
-----------------------------------------------------------------------------*/
#define BL_APPTEXT_MEASUREMENTCOMMENT 0x00000000
#define BL_APPTEXT_DBCHANNELINFO      0x00000001
#define BL_APPTEXT_METADATA           0x00000002

#define APPTEXT_DBCHANNELINFO_VERSION( f) ( BYTE)(   f & 0x000000FF)
#define APPTEXT_DBCHANNELINFO_CHANNEL( f) ( BYTE)( ( f & 0x0000FF00) >> 8)
#define APPTEXT_DBCHANNELINFO_BUSTYPE( f) ( BYTE)( ( f & 0x00FF0000) >> 16)
#define APPTEXT_DBCHANNELINFO_CANFD( f)   ( BYTE)( ( f & 0x01000000) >> 24)
#define APPTEXT_DBCHANNELINFO_FLAGS( version, bustype, channel, canfd) \
    (DWORD)( ( ( DWORD)(canfd   & 0x01) << 24) | \
             ( ( DWORD)(bustype & 0xFF) << 16) | \
             ( ( DWORD)(channel & 0xFF) << 8) | \
             ( ( DWORD)(version & 0xFF)))
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLAppText_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
                                                 /* */
                                                 /* mHeader.mObjectSize = sizeof( VBLAppText) + mTextLength; */
                                                 /* */
    DWORD           mSource;                     /* source of text */
    DWORD           mReserved;                   /* reserved */
    DWORD           mTextLength;                 /* text length in bytes */
    BL_LPSTR        mText;                       /* text in MBCS */
} VBLAppText;

/*----------------------------------------------------------------------------
|
| Realtime clock object
|
-----------------------------------------------------------------------------*/
/* HINT: Extension of this structure is not allowed! */
typedef struct VBLRealtimeClock_t
{
    VBLObjectHeader mHeader;                     /* object header */
    ULONGLONG       mTime;                       /* logging start time in ns since 00:00 1.1.1970 GMT */
    ULONGLONG       mLoggingOffset;              /* measurement zero offset in ns to 00:00 1.1.1970 GMT */
} VBLRealtimeClock;

/*----------------------------------------------------------------------------
|
| File statistics
|
-----------------------------------------------------------------------------*/

#define BL_APPID_UNKNOWN          0
#define BL_APPID_CANALYZER        1
#define BL_APPID_CANOE            2
#define BL_APPID_CANSTRESS        3
#define BL_APPID_CANLOG           4
#define BL_APPID_CANAPE           5
#define BL_APPID_CANCASEXLLOG     6
#define BL_APPID_VLCONFIG         7
#define BL_APPID_PORSCHELOGGER    200
#define BL_APPID_CAETECLOGGER     201
#define BL_APPID_VECTORNETWORKSIMULATOR 202
#define BL_APPID_IPETRONIKLOGGER  203
#define BL_APPID_RT_RK            204
#define BL_APPID_PIKETEC          205

#define BL_COMPRESSION_NONE     0
#define BL_COMPRESSION_SPEED    1
#define BL_COMPRESSION_DEFAULT  6
#define BL_COMPRESSION_MAX      9

typedef struct VBLFileStatistics_t
{
    DWORD     mStatisticsSize;                   /* sizeof (VBLFileStatistics) */
    BYTE      mApplicationID;                    /* application ID */
    BYTE      mApplicationMajor;                 /* application major number */
    BYTE      mApplicationMinor;                 /* application minor number */
    BYTE      mApplicationBuild;                 /* application build number */
    ULONGLONG mFileSize;                         /* file size in bytes */
    ULONGLONG mUncompressedFileSize;             /* uncompressed file size in bytes */
    DWORD     mObjectCount;                      /* number of objects */
    DWORD     mObjectsRead;                      /* number of objects read */
} VBLFileStatistics;

typedef struct VBLFileStatisticsEx_t
{
    DWORD      mStatisticsSize;                   /* sizeof (VBLFileStatisticsEx) */
    BYTE       mApplicationID;                    /* application ID */
    BYTE       mApplicationMajor;                 /* application major number */
    BYTE       mApplicationMinor;                 /* application minor number */
    BYTE       mApplicationBuild;                 /* application build number */
    ULONGLONG  mFileSize;                         /* file size in bytes */
    ULONGLONG  mUncompressedFileSize;             /* uncompressed file size in bytes */
    DWORD      mObjectCount;                      /* number of objects */
    DWORD      mObjectsRead;                      /* number of objects read */
    SYSTEMTIME mMeasurementStartTime;             /* measurement start time */
    SYSTEMTIME mLastObjectTime;                   /* last object time */
    DWORD      mReserved[18];                     /* reserved */
} VBLFileStatisticsEx;


/*----------------------------------------------------------------------------
|
| Bus system independent
|
-----------------------------------------------------------------------------*/

#define BL_BUSTYPE_CAN      1
#define BL_BUSTYPE_LIN      5
#define BL_BUSTYPE_MOST     6
#define BL_BUSTYPE_FLEXRAY  7
#define BL_BUSTYPE_J1708    9
#define BL_BUSTYPE_ETHERNET 10
#define BL_BUSTYPE_WLAN     13
#define BL_BUSTYPE_AFDX     14

typedef struct VBLDriverOverrun_t
{
  VBLObjectHeader mHeader;                     /* object header */
  DWORD           mBusType;                    /* bus type (see BL_BUSTYPE_...) */
  WORD            mChannel;                    /* channel where overrun occured */
  WORD            mDummy;
} VBLDriverOverrun;

/*----------------------------------------------------------------------------
|
| Event Comment
|
-----------------------------------------------------------------------------*/
typedef struct VBLEventComment_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
    /* */
    /* mHeader.mObjectSize = sizeof( VBLEventComment) + mTextLength; */
    /* */
    DWORD           mCommentedEventType;         /* commented event type */
    DWORD           mTextLength;                 /* text length in bytes */
    BL_LPSTR        mText;                       /* text in MBCS */
} VBLEventComment;

/*----------------------------------------------------------------------------
|
| Event Global Marker
|
-----------------------------------------------------------------------------*/
typedef struct VBLGlobalMarker_t
{
    VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size to*/
    /* */
    /* mHeader.mObjectSize = sizeof( VBLGlobalMarker) + mGroupNameLength + mMarkerNameLength + mDescriptionLength */
    /* */
    DWORD           mCommentedEventType;         /* commented event type */
    COLORREF        mForegroundColor;
    COLORREF        mBackgroundColor;
    BYTE            mIsRelocatable;
    DWORD           mGroupNameLength;            /* group name length in bytes */
    DWORD           mMarkerNameLength;           /* marker name length in bytes */
    DWORD           mDescriptionLength;          /* description length in bytes */
    BL_LPSTR        mGroupName;                  /* group name in MBCS */
    BL_LPSTR        mMarkerName;                 /* marker name in MBCS */
    BL_LPSTR        mDescription;                /* description in MBCS */
} VBLGlobalMarker;


/*----------------------------------------------------------------------------
|
| Test Structure Event
|
-----------------------------------------------------------------------------*/
#define BL_TESTSTRUCT_TYPE_TM_TESTMODULE      1
#define BL_TESTSTRUCT_TYPE_TM_TESTGROUP       2
#define BL_TESTSTRUCT_TYPE_TM_TESTCASE        3

#define BL_TESTSTRUCT_TYPE_TESTCONFIGURATION  8
#define BL_TESTSTRUCT_TYPE_TESTUNIT           9
#define BL_TESTSTRUCT_TYPE_TESTGROUP         10
#define BL_TESTSTRUCT_TYPE_TESTFIXTURE       11
#define BL_TESTSTRUCT_TYPE_TESTSEQUENCE      12
#define BL_TESTSTRUCT_TYPE_TESTSEQUENCELIST  13
#define BL_TESTSTRUCT_TYPE_TESTCASE          14
#define BL_TESTSTRUCT_TYPE_TESTCASELIST      15

#define BL_TESTSTRUCT_ACTION_BEGIN            1
#define BL_TESTSTRUCT_ACTION_END              2
#define BL_TESTSTRUCT_ACTION_ABORT            3 // early abortion of test execution (due to e.g. verdict impact, user stop or failed assure pattern), 
                                                // always correlated to test module / test configuration and followed by "end" action

#define BL_TESTSTRUCT_VERDICT_UNDEFINED         0
#define BL_TESTSTRUCT_VERDICT_NONE              1
#define BL_TESTSTRUCT_VERDICT_PASSED            2
#define BL_TESTSTRUCT_VERDICT_INCONCLUSIVE      3
#define BL_TESTSTRUCT_VERDICT_FAILED            4
#define BL_TESTSTRUCT_VERDICT_ERRORINTESTSYSTEM 5

/* HINT: Extension of this structure is not allowed! */
typedef struct VBLTestStructure_t
{
  VBLObjectHeader mHeader;                     /* object header - NOTE! set the object size too */
  /* */
  /* mHeader.mObjectSize = sizeof(VBLTestStructure) + (mExecutingObjectNameLength + mNameLength + mTextLength) * sizeof(wchar_t) */
  /* */
  DWORD           mExecutingObjectIdentity;    /* unique ID identifying the executing test module or test configuration */
  WORD            mType;                       /* type of structure element, see BL_TESTSTRUCT_TYPE_xxx */
  DWORD           mUniqueNo;                   /* unique number of structure element (in this test run, transitive, can be used to correlate begin/end events) */
  WORD            mAction;                     /* indicates begin/end of structure element, see BL_TESTSTRUCT_ACTION_xxx */
  WORD            mResult;                     /* overall result (verdict) for end of structure element events */
  DWORD           mExecutingObjectNameLength;  /* string length in wchar_t's for mExecutingObjectName */
  DWORD           mNameLength;                 /* string length in wchar_t's for mName */
  DWORD           mTextLength;                 /* string length in wchar_t's for mText */
  BL_LPWSTR       mExecutingObjectName;        /* name of the executing test module or test configuration as shown in CANoe (wchar_t) */
  BL_LPWSTR       mName;                       /* name of structure element (can change between begin/end when using CAPL function TestCaseTitle or similar (wchar_t) */
  BL_LPWSTR       mText;                       /* full informational text for event as it appears in CANoe trace window */
} VBLTestStructure;

/*----------------------------------------------------------------------------
|
| Communication object in the communication setup
|
-----------------------------------------------------------------------------*/

#define BL_CO_TYPE_UNDEFINED         0
#define BL_CO_TYPE_SIGNAL            1
#define BL_CO_TYPE_SERVICE_FUNCTION  2
#define BL_CO_TYPE_STATE             3

typedef struct VBLCommunicationObject_t
{
  VBLVarObjectHeader mHeader;                    /* object header - NOTE! set the object size to*/
  /* */
  /* mHeader.mObjectSize = sizeof( VBLVarObjectHeader) + sizeof (VBLCommunicationObjectStatic_t) + mNameLength + mDataLength; */
  /* */
  struct VBLCommunicationObjectStatic_t
  {
    DWORD           mObjectType;                 /* type of system variable (see BL_FB_TYPE_xxx) */
    DWORD           mVEType;
    DWORD           mNameLength;                 /* length of variable name in bytes */
    DWORD           mDataLength;                 /* length of variable data in bytes */
  } mStatic;

  struct VBLCommunicationObjectDynamic_t
  {
    BL_LPSTR        mName;                       /* path name in the port server */
    BL_LPBYTE       mData;                       /* variable data */
  } mDynamic;

} VBLCommunicationObject;

/* OBSOLETE */
#define BL_FB_TYPE_UNDEFINED         0
#define BL_FB_TYPE_SIGNAL            1
#define BL_FB_TYPE_SERVICE_FUNCTION  2
#define BL_FB_TYPE_STATE             3
typedef struct VBLCommunicationObject_t VBLFunctionBusObject;


/* HINT: Extension of this structure is not allowed! */
typedef struct VBLDiagRequestInterpretation_t
{
  VBLObjectHeader   mHeader;                  /* object header - NOTE! set the object size too */
  /* */
  /* mHeader.mObjectSize = sizeof(VBLDiagRequestInterpretation) + (mEcuQualifierLength + mVariantQualifierLength + mServiceQualifierLength) */
  /* */
  DWORD             mDiagDescriptionHandle;   /* unique ID identifying the used diagnostic description */
  DWORD             mDiagVariantHandle;       /* unique ID identifying the used diagnostic variant     */
  DWORD             mDiagServiceHandle;       /* unique ID identifying the used diagnostic service     */
  DWORD             mEcuQualifierLength;      /* string length for mEcuQualifier     */
  DWORD             mVariantQualifierLength;  /* string length for mVariantQualifier */
  DWORD             mServiceQualifierLength;  /* string length for mServiceQualifier */
  BL_LPSTR          mEcuQualifier;            /* qualifier of the ECU the request was sent to */
  BL_LPSTR          mVariantQualifier;        /* qualifier of the active diagnostic variant   */
  BL_LPSTR          mServiceQualifier;        /* qualifier of the diagnostic service          */
} VBLDiagRequestInterpretation;

/*----------------------------------------------------------------------------
|
| Queue state
|
-----------------------------------------------------------------------------*/

#define BL_WM_QS_STATUS_NORMAL 0
#define BL_WM_QS_STATUS_EMERGENCY 1
#define BL_WM_QS_STATUS_LOST_DATA 2

typedef struct VBLWaterMarkEvent_t
{
  VBLObjectHeader mHeader;                     /* object header */
  DWORD           mQueueState;                 /* the current state of the queue */
} VBLWaterMarkEvent;

#define BL_DL_QI_RT_QUEUE 0
#define BL_DL_QI_ANLYZ_QUEUE 1
#define BL_DL_QI_RT_AND_ANLYZ_QUEUE 2

typedef struct VBLDataLostBegin_t
{
  VBLObjectHeader mHeader;                     /* object header */
  DWORD           mQueueIdentifier;            /* identifier for the leaking queue */
} VBLDataLostBegin;

typedef struct VBLDataLostEnd_t
{
  VBLObjectHeader mHeader;                     /* object header */
  DWORD           mQueueIdentifier;            /* identifier for the leaking queue */
  ULONGLONG       mFirstObjectLostTimeStamp;   /* timestamp of the first object lost */
  DWORD           mNumberOfLostEvents;          /* number of lost events */
} VBLDataLostEnd;

/*----------------------------------------------------------------------------
|
| Trigger Condition object
|
-----------------------------------------------------------------------------*/

#define BL_TC_STATUS_UNKNOWN   0
#define BL_TC_STATUS_START     1
#define BL_TC_STATUS_STOP      2
#define BL_TC_STATUS_STARTSTOP 3

typedef struct VBLTriggerCondition_t
{
  VBLVarObjectHeader mHeader;                    /* object header - NOTE! set the object size to*/
  /* */
  /* mHeader.mObjectSize = sizeof(VBLVarObjectHeader) + sizeof (VBLTriggerCondition_t) + mNameLength + mDataLength; */
  /* */
  struct VBLTriggerConditionStatic_t
  {
    DWORD           mState;                      /* status */
    DWORD           mTriggerBlockNameLength;     /* length of trigger block name in bytes */
    DWORD           mTriggerConditionLength;     /* length of trigger condition in bytes */
  } mStatic;

  struct VBLTriggerConditionDynamic_t
  {
    BL_LPSTR        mTriggerBlockName;           /* trigger block name */
    BL_LPSTR        mTriggerCondition;           /* trigger condition */
  } mDynamic;

} VBLTriggerCondition;

