/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMDATAPORTHANDLER_H
#define CMMDATAPORTHANDLER_H

// INCLUDES
#include "nokia_csd_defs.h"

// CONSTANTS
    // Max len for ATD command string
    // (RMobilePhone::KMaxMobileTelNumberSize + length of literal
    // descriptors KStringD and KReturn).
    const TInt KATDStringMaxLen = RMobilePhone::KMaxMobileTelNumberSize + 3 + 1;

// Match value for handling connect response
const TInt KMatchForConnectResponse = 8;

// Number of characters to be ignored is 7
const TInt KIgnoreSevenCharacters = 7;

// Number of characters to be ignored is 8
const TInt KIgnoreEightCharacters = 8;

// Number of time slots 1
const TInt KTimeSlots1 = 1;

// Number of time slots 2
const TInt KTimeSlots2 = 2;

// Number of time slots 3
const TInt KTimeSlots3 = 3;

// Number of time slots 4
const TInt KTimeSlots4 = 4;

// Number of time slots 5
const TInt KTimeSlots5 = 5;

// Number of time slots 6
const TInt KTimeSlots6 = 6;

// Number of time slots 7
const TInt KTimeSlots7 = 7;

// Number of time slots 8
const TInt KTimeSlots8 = 8;

// No call id
const TInt KCallIdNone = -1;

// Comms DB, table
const TInt KCommsDBTable = 2;

// HSCSD Codings 4.8k Supported
const TInt K4_8kHSCSDCodingsSupported = 1;

// HSCSD Codings 9.6k Supported
const TInt K9_6kHSCSDCodingsSupported = 4;

// HSCSD Codings 14.4k Supported
const TInt K14_4kHSCSDCodingsSupported = 8;

// HSCSD Codings 28.8k Supported
const TInt K28_8kHSCSDCodingsSupported = 16;

// HSCSD Codings 32k Supported
const TInt K32kHSCSDCodingsSupported = 32;

// HSCSD Codings 64k Supported
const TInt K64kHSCSDCodingsSupported = 64;

// All HSCSD Codings Supported
const TInt KAllHSCSDCodingsSupported =
    K4_8kHSCSDCodingsSupported +
    K9_6kHSCSDCodingsSupported +
    K14_4kHSCSDCodingsSupported +
    K28_8kHSCSDCodingsSupported +
    K32kHSCSDCodingsSupported +
    K64kHSCSDCodingsSupported;

// MACROS
    //none

// DATA TYPES
    //none

// FUNCTION PROTOTYPES
    //none

// FORWARD DECLARATIONS
class CCallDataPackage;
class CMmMessageRouter;

// CLASS DECLARATION

/**
* CMmDataPortHandler is used to receive responses to AT commands from
* CMmDataPortReader active object (see utility) and route the responses
* to either Data line or active call object.
*/
class CMmDataPortHandler : public CBase
    {

    public: // Constructors and destructor

        /**
        * The constructor that has to be used for creating new CMmDataPortHandler.
        * @param aMessageRouter: Pointer to the message router
        * @return CMmDataPortHandler*: pointer to the dataport handler
        */
        static CMmDataPortHandler* NewL(
            CMmMessageRouter* aMessageRouter );

        /**
        * C++ destructor.
        */
        ~CMmDataPortHandler();

    public: // New functions

        /**
        * Loans the dataport to the client.
        * @param CDataDataPackage* aCallDataPackage: call data package
        * @return TInt Error code
        */
        TInt LoanDataPort( const CCallDataPackage* aCallDataPackage );

        /**
        * Recovers the dataport from the client.
        * @param CDataDataPackage* aCallDataPackage: call data package
        * @return TInt Error code
        */
        TInt RecoverDataPort( const CCallDataPackage* aCallDataPackage );

    public: // Functions from base classes
        //none

    protected: // New functions
        //none

    protected: // Functions from base classes
        //none

    private: // New functions

        /**
        * C++ constructor.
        */
        CMmDataPortHandler();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        /**
        * Initialises object attributes.
        */
        void ResetInternalAttributes();

        /**
        * Initialise dataport related parameters
        */
        void InitDataParamsL();

        /**
        * Opens dataport for TSY.
        */
        void OpenDataPortL();

        /**
        * Get next free dial-up port.
        * @param RCall::TCommPort* aCommPort: pointer to port informatio
        * @return TInt Error code
        */
        TInt GetNextFreeDialUpPort( RCall::TCommPort* aCommPort ) const;

        /**
        * Loans dial-up port to the client.
        * @param RCall::TCommPort* aCommPort: pointer to port informatio
        * @return TInt Error code
        */
        TInt LoanDialUpPort( RCall::TCommPort* aCommPort );

    private: // Functions from base classes
        // none

    public: // Data
        // none

    protected: // Data
        // none

    private: // Data

        // pointer to the message router
        CMmMessageRouter* iMessageRouter;

        // Communication server instance
        RCommServ iCommServer;

        // Communication port instance
        RCall::TCommPort iCommPort;

        // Comms status
        TCommsStatus iCommsStatus;

        // Dataport instance
        RComm iDataPort;

        // Dataport settings
        TCommConfig iDataPortSettings;

        // Has dataport object reported error,
        // which means that dataport does not work
        TBool iDataPortErrorHappened;

        // CSD parameters
        RMobileCall::TMobileDataCallParamsV1 iCSDParams;

        // HSCSD parameters
        RMobileCall::TMobileHscsdCallParamsV1 iHSCSDParams;

        // Mobile call info
        RMobileCall::TMobileCallInfoV1 iMobileCallInfo;

        // Call direction
        RMobileCall:: TMobileCallDirection iCallDirection;

        // Call type
        TCallType iCallType;

        // Is call multimedia/video call
        TBool iIsMultimedia;

        // Is current call a waiting call
        TBool iIsWaitingCall;

        // Current HSCSD call info
        RMobileCall::TMobileCallHscsdInfoV1 iCurrentHSCSDInfo;

        // Bearer service info
        RCall::TBearerService iBearerService;

        // To inform client about incoming data call
        TBool iAlreadyNotified;

        // For DATAPORT::0
        TBool iDP0LoanedToClient;

        // For DATAPORT::1
        TBool iDP1LoanedToClient;

        // For DATAPORT::4
        TBool iDP4LoanedToClient;

        // For DATAPORT::5
        TBool iDP5LoanedToClient;

        // IPC to be completed when data call is created
        TInt iIPC;

    public: // Friend classes
        // none

    protected: // Friend classes
        // none

    private: // Friend classes
        // none

    };

#endif // CMMDATAPORTHANDLER_H

//    End of file
