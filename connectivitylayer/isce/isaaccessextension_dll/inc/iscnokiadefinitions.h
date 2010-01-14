/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description: 
*
*/



#ifndef ISCNOKIADEFINITIONS_H
#define ISCNOKIADEFINITIONS_H

//  INCLUDES
#include <iscdefinitions.h>

// CONSTANTS
enum TIscNokiaUserChannels
    {
    EIscNokiaUpdateApplication = 0x01,  //0x01
    EIscNokiaSelfTest,                  //0x02
    EIscNokiaTest,                      //0x03
    EIscNokiaPowerAdaptation,           //0x04
    EIscNokiaPhoneDoctor,               //0x05
    EIscNokiaFtd,                       //0x06
    EIscNokiaMmTsy,                     //0x07
    EIscNokiaPerm,                      //0x08
    EIscNokiaUMA,                       //0x09
    EIscNokiaJuice,                     //0x0a //10
    EIscNokiaInfo,                      //0x0b
    EIscNokiaHapticsAdaptation,         //0x0c
    EIscNokiaNifPep0,                   //0x0d
    EIscNokiaNifPep1,                   //0x0e
    EIscNokiaNifPep2,                   //0x0f
    EIscNokiaNifPep3,                   //0x10
    EIscNokiaNifPep4,                   //0x11
    EIscNokiaNifPep5,                   //0x12
    EIscNokiaNifPep6,                   //0x13
    EIscNokiaNifPep7,                   //0x14  //20
    EIscNokiaNifPep8,                   //0x15
    EIscNokiaNifPep9,                   //0x16
    EIscNokiaNifPep10,                  //0x17
    EIscNokiaTest2,                     //0x18
    EIscNokiaReserved8,                 //0x19
    EIscNokiaReserved9,                 //0x1a
    EIscNokiaPsy,                       //0x1b
    EIscNokiaNtm,                       //0x1c
    EIscNokiaNtmLicensee,               //0x1d
    EIscATTSIMLockUI,                   //0x1e  //30
    EIscNokiaBtAppl,                    //0x1f
    EIscNokiaStartup,                   //0x20
    EIscNokiaDataProxy,                 //0x21
    EIscNokiaEstart,                    //0x22
    EIscNokiaSystemAdaptation,          //0x23
    EIscNokiaAudioAdaptation0,          //0x24
    EIscNokiaAudioAdaptation1,          //0x25
    EIscNokiaAudioAdaptation2,          //0x26
    EIscNokiaAudioAdaptation3,          //0x27
    EIscNokiaAudioAdaptation4,          //0x28  //40
    EIscNokiaAudioAdaptation5,          //0x29
    EIscNokiaAudioAdaptation6,          //0x2a
    EIscNokiaAudioAdaptation7,          //0x2b
    EIscNokiaUsbPhonetLink,             //0x2c
    EIscNokiaDRM,                       //0x2d
    EIscNokiaDataport1,                 //0x2e
    EIscNokiaDataport2,                 //0x2f
    EIscNokiaDataport3,                 //0x30
    EIscNokiaDataport4,                 //0x31
    EIscNokiaDataport5,                 //0x32  //50
    EIscNokiaSuplPosPlugin,             //0x33
    EIscNokiaLightsAdaptation,          //0x34
    EIscNokiaLocationConfigCtrl,        //0x35
    EIscNokiaBtPhonetLink,              //0x36
    EIscNokiaMobileCrashXFile,          //0x37
    EIscNokiaReserved1,                 //0x38
    EIscNokiaReserved2,                 //0x39
    EIscNokiaUpdateServerApplication,   //0x3a
    EIscNokiaReserved3,                 //0x3b
    EIscNokiaReserved4,                 //0x3c  //60
    EIscNokiaReserved5,                 //0x3d
    EIscNokiaReserved6,                 //0x3e
    EIscNokiaAtController,              //0x3f  //63
    EIscNokiaLastUserChannel            // This is actually first kernel side channel
    };

enum TIscNokiaKernelChannels
    {
    EIscNokiaCustomDriver = EIscNokiaLastUserChannel,
    EIscNokiaSecurityKext,              //0x41    //65
    EIscNokiaUsb,                       //0x42
    EIscNokiaReservedKern1,             //0x43
    EIscNokiaCamera,                    //0x44
    EIscNokia2dnCamera,                 //0x45
    EIscNokiaMediaUsb,                  //0x46    //70
    EIscNokiaLight,                     //0x47
    EIscNokiaSecurityDriver,            //0x48
    EIscNokiaThermalManager,            //0x49
    EIscNokiaTouchDriver,               //0x4a
    EIscNokiaTraceCore,                 //0x4b    //75
    EIscNokiaDVBH,                      //0x4c
    EIscNokiaSSC,                       //0x4d
    EIscNokiaUsbAudioDriver,            //0x4e
    EIscNokiaDMC,                       //0x4f
    EIscNokiaReservedKern2,             //0x50
    EIscNokiaLastKernelChannel          //0x51    //81
    };

// MACROS

// DATA TYPES
enum TIscCustomOperations
    {                    
    EIscNokiaEventSubscribe = 23,
    EIscNokiaEventSend,
    EIscNokiaEventSubscribe32bit
    };

enum TIscAsyncCustomOperations
    {                    
    EIscNokiaAsyncEventSubscribe = 9,
    EIscNokiaAsyncEventSend,
    EIscNokiaAsyncFTDInformation, //FTD DO NOT USE! OBSOLETED TO BE DEPRICATED
    EIscNokiaAsyncEventSubscribe32bit
    };

enum TIscCancelAsyncCustomOperations
    {
    EIscNokiaCancelAsyncEventSubscribe = 38,
    EIscNokiaCancelAsyncEventSend,
    EIscNokiaCancelAsyncEventSubscribe32bit
    };

#ifndef PN_DEV_PC
#define PN_DEV_PC 0x10 // Not in pn_const.h at the moment TODO: Remove when possible
#endif
#ifndef PN_APE_COMMGR
#define PN_APE_COMMGR 0x11 //TODO remove when defined in pn_const.h
#endif
#ifndef THIS_DEVICE
    #if !defined (__WINS__) && !defined(NCP_COMMON_PLATFORM_SIMULATOR)
            // Device IDs
            #define THIS_DEVICE PN_DEV_HOST
            #define OTHER_DEVICE_1 PN_DEV_MODEM
            #define PN_DEV_OWN                                                  0x6c
            #define PN_DEV_DONT_CARE OTHER_DEVICE_1
            // Media IDs
            #define PN_MEDIA_SOS                                                PN_MEDIA_MODEM_HOST_IF
            // Internals CLIENT OF ISC API OR ISA KERNEL API - DO NOT USE! ->
            #define PN_MEDIA_NOT_USED                                           0xfe
            #define PN_NO_ROUTING                                               0xff
            #define PN_OBJ_ROUTER PN_OBJ_ROUTING_REQ
            // To be removed when pipe changes are done.
            #define PNS_PIPE_DATA_OFFSET_DATA                                   3
            // <- Internals CLIENT OF ISC API OR ISA KERNEL API - DO NOT USE!
    #else
        #define THIS_DEVICE PN_DEV_PC
    #endif
#endif

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


#endif      // ISCNOKIADEFINITIONS_H
            
// End of File
