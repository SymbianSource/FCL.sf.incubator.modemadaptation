/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Abstract base class for all SIM ATK TSY proactive command
*                tests classes.
*
*/



#ifndef SATCOMMANDBASE_H
#define SATCOMMANDBASE_H

//  INCLUDES
#include "satbase.h"


// DATA TYPES

// Structure that contain both string and number presentation of a value
struct TValuePair
    {
    const TText* iString;
    const TInt iValue;
    };
    
// CONSTANTS

// Array of GenreralResults
TValuePair const KGeneralResultList[] =
    {
        { _S( "PCmdResultNotSet" ),                         -1 },

        { _S( "AccessTechUnableProcessCmd" ),               0x3B },
        { _S( "BackwardModeRequestedByUser" ),              0x11 },
        { _S( "BearerIndepProtocolError" ),                 0x3A },
        { _S( "CallClearedBeforeConnectionOrReleased" ),    0x23 },
        { _S( "CmdBeyondMeCapabilities" ),                  0x30 },
        { _S( "CmdDataNotUnderstood" ),                     0x32 },
        { _S( "CmdNumberNotKnown" ),                        0x33 },
        { _S( "CmdTypeNotUnderstood" ),                     0x31 },
        { _S( "ContradictionWithTimerState" ),              0x24 },
        { _S( "ErrorRequiredValuesMissing" ),               0x36 },
        { _S( "HelpRequestedByUser" ),                      0x13 },
        { _S( "InteractionWithCCPermanentError" ),          0x39 },
        { _S( "InteractionWithCCTemporaryError" ),          0x25 },
        { _S( "LaunchBrowserError" ),                       0x26 },
        { _S( "MeUnableToProcessCmd" ),                     0x20 },
        { _S( "MissingInformation" ),                       0x02 },
        { _S( "ModifiedByCallControl" ),                    0x05 },
        { _S( "MultipleCardCmdsError" ),                    0x38 },
        { _S( "NetworkUnableToProcessCmd" ),                0x21 },
        { _S( "NoResponseFromUser" ),                       0x12 },
        { _S( "PartialComprehension" ),                     0x01 },
        { _S( "PCmdNotAcceptedByUser" ),                    0x22 },
        { _S( "PerformedWithModifications" ),               0x07 },
        { _S( "RefreshAdditionEFRead" ),                    0x03 },
        { _S( "RefreshUSIMNotActive" ),                     0x08 },
        { _S( "SessionTerminatedByUser" ),                  0x10 },
        { _S( "SmsRpError" ),                               0x35 },
        { _S( "SsReturnError" ),                            0x34 },
        { _S( "Success" ),                                  0x00 },
        { _S( "SuccessLimitedService" ),                    0x06 },
        { _S( "SuccessRequestedIconNotDisplayed" ),         0x04 },
        { _S( "UssdReturnError" ),                          0x37 },
        { _S( "UssdTransactionTerminatedByUser" ),          0x14 }

    };

const TUint KGeneralResultListSize = static_cast<TUint>
    ( sizeof( KGeneralResultList ) / sizeof( TValuePair ) ) ;

// Array of AdditionalInfo types
TValuePair const KInfoTypeList[] =
    {
        { _S( "ATResponse" ),                               0x00400 },
        { _S( "CardATR" ),                                  0x00200 },
        { _S( "CardReaderInfo" ),                           0x04000 },
        { _S( "CallControlRequestedAction" ),               0x00080 },
        { _S( "ChannelData" ),                              0x00800 },
        { _S( "ChannelStatusInfo" ),                        0x01000 },
        { _S( "ControlInteraction" ),                       0x00008 },
        { _S( "ItemIdentifier" ),                           0x00040 },
        { _S( "LocalInformation" ),                         0x08000 },
        { _S( "MeProblem" ),                                0x00002 },
        { _S( "NoAdditionalInfo" ),                         0x00001 },
        { _S( "PollingIntv" ),                              0x20000 },
        { _S( "RApdu" ),                                    0x00100 },
        { _S( "SatNetworkErrorInfo" ),                      0x00004 },
        { _S( "SendSsInfo" ),                               0x00010 },
        { _S( "ServiceAvailability" ),                      0x02000 },
        { _S( "TextString" ),                               0x00020 },
        { _S( "TimerMgmt" ),                                0x10000 }
    };

const TUint KInfoTypeListSize = static_cast<TUint>
    ( sizeof( KInfoTypeList ) / sizeof( TValuePair ) ) ;

// Array of AdditionalInfo types
TValuePair const KAdditionalInfoList[] =
    {
    // MeProblem
        { _S( "NoSpecificMeProblem" ),                      0x00 },
        { _S( "ScreenBusy" ),                               0x01 },
        { _S( "MeBusyOnCall" ),                             0x02 },
        { _S( "MeBusyOnSs" ),                               0x03 },
        { _S( "NoService" ),                                0x04 },
        { _S( "AccBar" ),                                   0x05 },
        { _S( "RadioRessourceNotGranted" ),                 0x06 },
        { _S( "NotInSpeechCall" ),                          0x07 },
        { _S( "MeBusyOnUssd" ),                             0x08 },
        { _S( "MeBusyOnSendDtmf" ),                         0x09 },
        { _S( "NoUSIMActive" ),                             0x0A },
    // CcInteractionError
        { _S( "NoSpecificInteractionError" ),               0x00 },
        { _S( "ActionNotAllowed" ),                         0x01 },
        { _S( "RequestTypeChanged" ),                       0x02 },
    // MultipleCardError
        { _S( "NoSpecificMultiCardError" ),                 0x00 },
        { _S( "NoCardReader" ),                             0x01 },
        { _S( "NoCard" ),                                   0x02 },
        { _S( "CardReadyBusy" ),                            0x03 },
        { _S( "CardPoweredOff" ),                           0x04 },
        { _S( "CApduFormatError" ),                         0x05 },
        { _S( "MuteCard" ),                                 0x06 },
        { _S( "TransmissionError" ),                        0x07 },
        { _S( "ProtocolNotSupported" ),                     0x08 },
        { _S( "SpecifiedReaderNotValid" ),                  0x09 },
    // BrowserError
        { _S( "NoSpecificBrowserError" ),                   0x00 },
        { _S( "BearerUnvailable" ),                         0x01 },
        { _S( "BrowserUnavailable" ),                       0x02 },
        { _S( "MeUnableToReadProvisioningData" ),           0x04 },
    // BearerIndProtoError
        { _S( "NoSpecificBIPError" ),                       0x00 },
        { _S( "NoChannelAvailable" ),                       0x01 },
        { _S( "ChannelClosed" ),                            0x02 },
        { _S( "ChannelIdNotValid" ),                        0x03 },
        { _S( "BufferSizeNotAvailable" ),                   0x04 },
        { _S( "SecurityError" ),                            0x05 },
        { _S( "TranportLevelNotAvailable" ),                0x06 },
        { _S( "RemoteDeviceNotReachable" ),                 0x07 },
        { _S( "ServiceError" ),                             0x08 },
        { _S( "ServiceIdUnknown" ),                         0x09 }    
    };

const TUint KAdditionalInfoListSize = static_cast<TUint>
    ( sizeof( KAdditionalInfoList ) / sizeof( TValuePair ) ) ;
    

// CLASS DECLARATION

/**
*  Abstract base class for all SIM ATK TSY proactive command
*  tests classes. Inherits CSatBase. Adds functionality to
*  generate responses to SIM by providing general helper methods
*  for all subclasses.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatCommandBase ) : public CSatBase
    {
    
    protected:  // Data types
    
    enum TResultNumber
        {
        KFirst,
        KSecond
        };
    
    public:  // Constructors and destructor
    
        /**
        * Constructor.
        */     
        CSatCommandBase( const TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser,  CStifLogger& aLog,
            const RSat::TPCmd aCommandType );
    
        /**
        * Destructor.
        */
        virtual ~CSatCommandBase();
        
    public:  // Pure virtuals
        
        /**
        * Pure virtual method. Must be overridden by subclasses.
        * Constructs and sends response packet to Etel from
        * scripted parameters.
        * @return KErrCorrupt if parsing failed or mandatory
        *   parameters were missing or Request Status of the Etel response
        *   if parsing was successful.
        */
        virtual TInt SendResponse() = 0;
            

    protected: // New functions
    
        /**
        * Sends response packet constructed by subclasses to Etel.
        * @param aRspPckg contain reference to a response packed
        * @return Request Status of the Etel response
        */
        TInt SendResponse( const TDesC8& aRspPckg );
    
        /**
        * Helper function that constructs terminal response's
        * GeneralResult (and GeneralResult2)from the script. If 
        * value is missing from the script, "Command Performed
        *  Succesfully"-value is used.
        * @param aGeneralResult reference to packet which shall be constructed
        * @param aNumber The number of general result. Default value = First.
        *  Set to Second if GeneralResult2 should be parsed.
        * @return ETrue if general result indicates that command was performed
        *  successfully. EFalse if result indicates a failure
        */ 
        TBool SetGeneralResult( RSat::TPCmdResult& aGeneralResult,
            TResultNumber aNumber = KFirst );

        /**
        * Helper function that constructs terminal response's
        * AdditionalInfo and it's Type  (and AdditionalInfo 2 & Type) from 
        * the script. If values are missing from the script, InfoType is set
        * to "No Additional Info" and AdditionalInfo is set to null.
        * @param aInfoType reference to InfoType which shall be set
        * @param aAdditionalInfo reference to AdditionalInfo which shall be set
        * @param aNumber The number of InfoType and AdditionalInfo. Default 
        *  value = First. Set to Second if InfoType2 and AdditionalInfo2 should
        *  be parsed.
        * @return KErrNone or KErrCorrupt if invalid InfoType was detected
        */ 
        TInt SetAdditionalInfo( TUint& aInfoType,
            RSat::TAdditionalInfo& aAdditionalInfo,
            TResultNumber aNumber = KFirst );
            
        /**
        * Helper function that constructs terminal response's
        * Duration-structure from the script. 
        * @param aItem reference to Parser class holding parameter data
        * @param aDuration reference to packet which shall be constructed
        * @return KErrNone or KErrNotFound if duration is not provided by the
        *   script.
        */ 
        TInt SetDuration( RSat::TDuration& aDuration );
                                 
        /**
        * Helper function that identifies whether the script is an
        * envelope or not. To be called from SendResponse when code has to
        * parse either terminal response or envelope from provided script.
        * @return ETrue if script was identified as a envelope. EFalse if not.
        */
        TBool IsEnvelope();
            
        
    private: // New functions
    
        /**
        * Function maps string presentation of an value to a numeretic value
        * from a provided ValuePair array
        * @param aStringValue String presentation of a value.
        * @param aValueArray[] Array of values from which the mapping is done
        * @param aSize Size of Array
        * @return Numeric value mapped to string or KErrNotFound
        */ 
        static TInt MapToValue ( const TDesC& aStringValue, 
            const TValuePair aValueArray[], const TUint aSize );

                                 
    private:    // Data
    
        /**        
        * Type of proactive command is stored here
        */        
        const RSat::TPCmd       iCommandType;

    };

#endif      // SATCOMMANDBASE_H            

// End of File
