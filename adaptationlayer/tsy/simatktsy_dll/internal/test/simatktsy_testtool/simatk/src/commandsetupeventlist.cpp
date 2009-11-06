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
* Description:   SetUpCall specific test class. Inherits CCommandBase.
*
*/



// INCLUDE FILES
#include <stifparser.h>
#include "commandsetupeventlist.h"

// CONSTANTS
_LIT( KEvents,              "Events" );
_LIT( KType,                "Type" );
_LIT( KCause,               "Cause" );
_LIT( KChannelStatus,       "ChannelStatus" );
_LIT( KDataLength,          "DataLength" );
_LIT( KLanguage,            "Language" );


// -----------------------------------------------------------------------------
// CCommandSetUpEventList::CCommandSetUpEventList
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCommandSetUpEventList::CCommandSetUpEventList
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, RSat::ESetUpEventList ),
        iDataPckg( iData ),
        iRspPckg( iRsp )
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpEventList::~CCommandSetUpEventList
// Destructor
// -----------------------------------------------------------------------------
//
CCommandSetUpEventList::~CCommandSetUpEventList()
    {
    }

// -----------------------------------------------------------------------------
// CCommandSetUpEventList::NotifySat
// Send a notify request to Etel and clear internal data
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpEventList::NotifySat
        (
        TRequestStatus& aSimAtkStatus
        )
    {
    TInt ret ( KErrNone );
    
    // Clear the package
    Init( iData );
    Init( iRsp );

    iSat.NotifySetUpEventListPCmd( aSimAtkStatus, iDataPckg );
    
    return ret;
    }

// -----------------------------------------------------------------------------
// CCommandSetUpEventList::CheckData
// Verifies the validity of received command
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpEventList::CheckData()
    {
    iLog.Log( _L( "CCommandSetUpEventList::CheckData" ) );    
    
    TInt ret( KErrNone );
    
    // Type, default EUpdateEventList
    if ( KErrNone != CheckValue( KType, iData.iType, RSat::EUpdateEventList ) )
        {
        ret = KErrCorrupt;
        }
        
    // Events, needed if event list is updated
    if ( RSat::EUpdateEventList == iData.iType )
        {
        if ( KErrNone != CheckValue( KEvents, iData.iEvents ) )
            {
            ret = KErrCorrupt;
            }
        } 
    
    return ret;
    }
 
// -----------------------------------------------------------------------------
// CCommandSetUpEventList::SendResponse
// Creates terminal response according to scripted parameters
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpEventList::SendResponse()
    {
    iLog.Log( _L( "CCommandSetUpEventList::SendResponse" ) );    
    
    TInt ret( KErrNone );

    if ( IsEnvelope() )
        {
        RSat::TEventList event;
        if ( KErrNone != GetValue( KEvents, event, EHex ) )
            {
            // Mandatory parameter not provided by the script
            ret = KErrArgument;
            }
        else
            {
            switch( event )
                {
                case RSat::KUserActivity:
                    {
                    RSat::TEventDownloadBaseV2 eventDownload;
                    RSat::TEventDownloadBaseV2Pckg eventDownloadPckg( 
                                                    eventDownload );
                    eventDownload.iDataAvailable = EFalse;
                    ret = SendEvent( event, eventDownloadPckg );
                    break;
                    }
                case RSat::KIdleScreenAvailable:
                    {
                    RSat::TEventDownloadBaseV2 eventDownload;
                    RSat::TEventDownloadBaseV2Pckg eventDownloadPckg( 
                                                    eventDownload );
                    eventDownload.iDataAvailable = EFalse;
                    ret = SendEvent( event, eventDownloadPckg );
                    break;
                    }
                case RSat::KBrowserTermination:
                    {
                    RSat::TBrowserTerminationEventV2 browserTermination;
                    RSat::TBrowserTerminationEventV2Pckg 
                    browserTerminationPckg( browserTermination );
                    
                    if ( KErrNone != GetValue( KCause, 
                        browserTermination.iCause ) )
                        {
                        // Mandatory parameter not provided by the script
                        ret = KErrArgument;
                        }
                    else
                        {
                        browserTermination.iDataAvailable = ETrue;
                        ret = SendEvent( event, browserTerminationPckg );
                        }
                    break;
                    }
                case RSat::KLanguageSelection:
                    {
                    RSat::TLanguageSelectionEventV2 languageSelection;
                    RSat::TLanguageSelectionEventV2Pckg languageSelectionPckg(
                                                        languageSelection );

                    if ( KErrNone == GetValue( KLanguage,
                        languageSelection.iLanguage, EHex ) )
                        {
                        languageSelection.iDataAvailable = ETrue;
                        ret = SendEvent( event, languageSelectionPckg );
                        }
                    break;
                    }
                case RSat::KDataAvailable:
                    {
                    RSat::TDataAvailableEventV2 dataAvailable;
                    RSat::TDataAvailableEventV2Pckg dataAvailablePckg( 
                                                    dataAvailable );
                                                    
                    TInt number( 0 );
                    if ( KErrNone != GetValue( KChannelStatus, number, EHex ) )
                        {
                        // Mandatory parameter not provided by the script
                        ret = KErrArgument;
                        }
                    else
                        {
                        // First byte of channel status
                        dataAvailable.iStatus.Append( number );
                        
                        if ( KErrNone != GetNextValue( number , EHex) )
                            {
                            // Mandatory parameter not provided by the script
                            ret = KErrArgument;
                            }
                        else
                            {
                            // Second byte of channel status
                            dataAvailable.iStatus.Append( number );

                            if ( KErrNone != GetValue( KDataLength, number,
                                EHex ) )
                                {
                                // Parameter not provided by the script
                                ret = KErrArgument;
                                }
                            else
                                {
                                dataAvailable.iDataAvailable = ETrue;
                                dataAvailable.iLength = number;
                                ret = SendEvent( event, dataAvailablePckg );
                                }
                            }
                        }
                    break;
                    }
                case RSat::KChannelStatus:
                    {
                    RSat::TChannelStatusEventV2 channelStatus;
                    RSat::TChannelStatusEventV2Pckg channelStatusPckg( 
                                                    channelStatus );
                                                    
                    TInt number( 0 );
                    if ( KErrNone != GetValue( KChannelStatus, number, EHex ) )
                        {
                        // Mandatory parameter not provided by the script
                        ret = KErrArgument;
                        }
                    else
                        {
                        // First byte of channel status
                        channelStatus.iStatus.Append( number );

                        if ( KErrNone != GetNextValue( number, EHex ) )
                            {
                            // Mandatory parameter not provided by the script
                            ret = KErrArgument;
                            }
                        else
                            {
                            // Second byte of channel status
                            channelStatus.iStatus.Append( number );
                            channelStatus.iDataAvailable = ETrue;

                            ret = SendEvent( event, channelStatusPckg );
                            }
                        }
                    break;
                    }
                default:
                    {
                    //Invalid Event
                    ret = KErrArgument;
                    break;
                    }
                }
            }
        }
    else
        {
        // Terminal response
        
        // General result
        SetGeneralResult( iRsp.iGeneralResult );
        
        // Additional info & type
        SetAdditionalInfo( iRsp.iInfoType, iRsp.iAdditionalInfo );
        
        if ( KErrNone == ret )
            {
            ret = CSatCommandBase::SendResponse( iRspPckg );
            }
        }

    return ret;
    }
    

// -----------------------------------------------------------------------------
// CCommandSetUpEventList::SendResponse
// Sends provided event to TSY
// (other items were commented in a header).
// -----------------------------------------------------------------------------       
//
TInt CCommandSetUpEventList::SendEvent
        (
        RSat::TEventList    aSingleEvent, 
        const TDesC8&       aEventInfo 
        )
    {
    // Completion status of a request made to a service provider
    TRequestStatus sendStatus;
    
    // Notify the ME side that one event has occured
    iSat.EventDownload( sendStatus, aSingleEvent, aEventInfo );
    User::WaitForRequest( sendStatus );

    return sendStatus.Int();
    }

// End of File
