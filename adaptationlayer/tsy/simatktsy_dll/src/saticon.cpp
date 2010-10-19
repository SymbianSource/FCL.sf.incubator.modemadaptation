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




//  INCLUDE FILES
#include "saticon.h"        // sat icon class
#include "satmessaging.h"   // sat messaging
#include "satmesshandler.h" // sat message handler class
#include "satutil.h"        // sat utility class
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "saticonTraces.h"
#endif


// CONSTANTS
const TUint8 KDescriptionSize        = 9;

// KMaxThumbPhonetMessageSize (16384) - header (8) - subBlock parameters (16)
const TUint16 KIconDataMaxSize       = 0x3FE8;


// ==================== MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CSatIcon::CSatIcon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatIcon::CSatIcon
        (
        CSatMessHandler* aSatMessHandler,
        CTsySatMessaging* aSatMessaging
        )
        :
        iSatMessHandler( aSatMessHandler ),
        iSatMessaging( aSatMessaging ),
        iIconData( NULL ),
        iIconTsyReqHandle( NULL ),
        iGetIconFlag( EFalse ),
        iRecordNumber( KZero ),
        iImageInstanceNoInRecord( KZero )
    {
    OstTrace0( TRACE_NORMAL,  CSATICON_CSATICON_TD, "CSatIcon::CSatIcon" );
    iRecordData.Zero();
    }


// -----------------------------------------------------------------------------
// CSatIcon::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatIcon::ConstructL()
    {
    OstTrace0( TRACE_NORMAL,  CSATICON_CONSTRUCTL_TD, "CSatIcon::ConstructL" );
    // None. Reserved for future use.
    }


// -----------------------------------------------------------------------------
// CSatIcon::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatIcon* CSatIcon::NewL
        (
        CSatMessHandler*    aSatMessHandler,
        CTsySatMessaging*   aSatMessaging
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATICON_NEWL_TD, "CSatIcon::NewL" );
    TFLOGSTRING("TSY: CSatIcon::NewL");

    CSatIcon* self = new( ELeave ) CSatIcon( aSatMessHandler, aSatMessaging );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CSatCC::~CSatCC
// C++ destructor
// -----------------------------------------------------------------------------
//
CSatIcon::~CSatIcon()
    {
    OstTrace0( TRACE_NORMAL,  DUP1_CSATICON_CSATICON_TD, "CSatIcon::~CSatIcon" );
    TFLOGSTRING("TSY: CSatIcon::~CSatIcon");
    }


// -----------------------------------------------------------------------------
// CSatIcon::GetIcon
// This method gets the content of the EF_IMG record specified
// by aRecordnumber
// -----------------------------------------------------------------------------
//
TInt CSatIcon::GetIcon
        (
        TUint8*         aRecordNumber,     // Record number
        RSat::TIcon*    aIconEf,           // Icon elementary file
        TTsyReqHandle   aTsyReqHandle      // Request handle
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATICON_GETICON_TD, "CSatIcon::GetIcon" );
    TFLOGSTRING("TSY: CSatIcon::GetIcon");
    iImageInstanceNoInRecord = 0;

    TInt ret( KErrNone );

    if ( iIconTsyReqHandle )
        {
        TFLOGSTRING("TSY: CSatIcon::GetIcon : KErrServerBusy");
        OstTrace0( TRACE_NORMAL,  DUP1_CSATICON_GETICON_TD, "CSatIcon::GetIcon : KErrServerBusy" );

        iSatMessaging->ReqCompleted( aTsyReqHandle, KErrServerBusy);
        }
    else
        {
        iIconData = aIconEf;
        iIconTsyReqHandle = aTsyReqHandle;
        iGetIconFlag = ETrue;
        iRecordNumber = *aRecordNumber;

        ret = iSatMessHandler->UiccReadSatIconReq(
            *aRecordNumber,
            0, // File offset, read from start of record
            0, // Data amouont, read all the data
            KUiccTrIdReadSatIcon );
        }

    TFLOGSTRING2("TSY: CSatIcon::GetIcon, ret: %d", ret );
    OstTrace1( TRACE_NORMAL,  DUP3_CSATICON_GETICON_TD, "CSatIcon::GetIcon, ret: %d", ret );

    return ret;

    }

// -----------------------------------------------------------------------------
// CSatIcon::CompleteIconRequest
// Completes icon request
// -----------------------------------------------------------------------------
//
void CSatIcon::CompleteIconRequest
        (
        TInt aStatus,   // Message sending status
        TPtrC8 aIcon,    // Icon elementary file data
        const TDesC8& aInfo // Icon info block
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATICON_COMPLETEICONREQUEST_TD, "CSatIcon::CompleteIconRequest" );
    TFLOGSTRING("TSY: CSatIcon::CompleteIconRequest, entered");

    if ( iIconTsyReqHandle )
        {
        if ( KErrNone == aStatus && aIcon.Length() > 0)
            {
            //check the size....
            if ( (*iIconData).MaxLength() >= aIcon.Length())
                {
                //icon body
                if( aInfo.Length() == 6 )
                    {
                    // Color coding scheme is located at index 3 in Image
                    // Instance's Description.
                    TInt index( 3 + ( iImageInstanceNoInRecord - 1)
                        * KDescriptionSize );

                    TInt colorCoding = iRecordData[index];
                    if ( KBlackAndWhite == colorCoding )
                        {
                        // Black and white icon info block contain only
                        // two bytes: width and heigth
                        (*iIconData).Copy( aInfo.Mid( 0, 2) );
                        }
                    else
                        {
                        // Color icons have six bytes info block
                        (*iIconData).Copy( aInfo );
                        }

                    // Append icon data after the info block
                    (*iIconData).Append( aIcon );
                    }
                //clut and record
                else
                    {
                    (*iIconData).Copy( aIcon );
                    }

                // if this is completed for GetIcon request, store records
                // content
                if( iGetIconFlag )
                    {
                    iRecordData.Copy( *iIconData );
                    }
                }
            else //data is too big for client's buffer
                {
                aStatus = KErrOverflow;
                (*iIconData).Zero();
                }
            }
        else
            {
            //reset the length
            (*iIconData).Zero();
            }

        iIconData = NULL;
        iGetIconFlag = EFalse;
        TFLOGSTRING2("TSY: CSatIcon::CompleteIconRequest, aStatus: %d", aStatus );
        OstTrace1( TRACE_NORMAL,  DUP1_CSATICON_COMPLETEICONREQUEST_TD, "CSatIcon::CompleteIconRequest, aStatus: %d", aStatus );

        iSatMessaging->ReqCompleted( iIconTsyReqHandle, aStatus );
        iIconTsyReqHandle = NULL;

        }
    }

// -----------------------------------------------------------------------------
// CSatIcon::GetImageInstance
// This method gets the image instance specified by the
// InstanceNumber, Offset and Length
// -----------------------------------------------------------------------------
//
TInt CSatIcon::GetImageInstance
        (
        TDesC8* aInstanceInfo,         // instance info
        TDes8* aInstance,              // instance
        TTsyReqHandle aTsyReqHandle    // request handle
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATICON_GETIMAGEINSTANCE_TD, "CSatIcon::GetImageInstance" );
    TFLOGSTRING("TSY: CSatIcon::GetImageInstance");

    TInt ret( KErrNotFound );

    if( iIconTsyReqHandle )
        {
        TFLOGSTRING("TSY: CSatIcon::GetImageInstance : KErrServerBusy");
        OstTrace0( TRACE_NORMAL,  DUP1_CSATICON_GETIMAGEINSTANCE_TD, "CSatIcon::GetImageInstance : KErrServerBusy" );

        iSatMessaging->ReqCompleted( aTsyReqHandle, KErrServerBusy);
        }
    else
        {
        iIconData = aInstance;
        iIconTsyReqHandle = aTsyReqHandle;

        RSat::TInstanceInfoV2Pckg* aPckg =
                REINTERPRET_CAST( RSat::TInstanceInfoV2Pckg*, aInstanceInfo );
        RSat::TInstanceInfoV2& instanceInfo = ( *aPckg ) ();

        //Calculate position for icon imageinstance selected from record.
        TInt iconNumber = iRecordData[0];
        TBool iconFound(EFalse);
        TInt pos(4);
        while( iconNumber && !iconFound )
            {
            if( ( (TUint16)instanceInfo.iInstanceNumber ==
                (TUint16)(iRecordData[pos]<<8 | iRecordData[pos+1]))
             && ( (TUint16)instanceInfo.iOffset ==
                (TUint16)(iRecordData[pos+2]<<8 | iRecordData[pos+3]))
             && ( (TUint16)instanceInfo.iLength ==
                (TUint16)(iRecordData[pos+4]<<8 | iRecordData[pos+5])))
                {
                iconFound = ETrue;
                }
            pos+= 9;
            iconNumber--;
            }

        iImageInstanceNoInRecord = iRecordData[0] - iconNumber;

        //Check icon data length
        if( ( KIconDataMaxSize > instanceInfo.iLength ) && iconFound )
            {
            // Calculate the offset of instance
            TUint8 instanceOffset(
                ( iImageInstanceNoInRecord - 1 ) * KDescriptionSize + 1 );


            // At first read image instance data from record of EF IMG
            ret = iSatMessHandler->UiccReadSatIconReq(
                iRecordNumber,
                instanceOffset,
                KDescriptionSize,
                KUiccTrIdReadIconInstancePhase1
                );
            }
        }
    TFLOGSTRING2("TSY: CSatIcon::GetImageInstance, ret: %d", ret );
    OstTrace1( TRACE_NORMAL,  DUP3_CSATICON_GETIMAGEINSTANCE_TD, "CSatIcon::GetImageInstance, ret: %d", ret );

    return ret;
    }


// -----------------------------------------------------------------------------
// CSatIcon::GetClut
// This method gets the CLUT (Color look-up table) specified
// by aInstanceNumber and Offset.
// -----------------------------------------------------------------------------
//
TInt CSatIcon::GetClut
        (
        RSat::TInstanceNumberAndOffset*,
        TDes8*         aClut,              // Colour look-up table
        TTsyReqHandle  aTsyReqHandle       // Request handle
        )
    {
    OstTrace0( TRACE_NORMAL,  CSATICON_GETCLUT_TD, "CSatIcon::GetClut" );
    TFLOGSTRING("TSY: CSatIcon::GetClut");
     TInt ret( KErrNotFound );

    if( iIconTsyReqHandle )
        {
        TFLOGSTRING("TSY: CSatIcon::GetClut : KErrServerBusy");
        OstTrace0( TRACE_NORMAL,  DUP1_CSATICON_GETCLUT_TD, "CSatIcon::GetClut : KErrServerBusy" );

        iSatMessaging->ReqCompleted( aTsyReqHandle, KErrServerBusy);
        }
    else
        {
        iIconData = aClut;
        iIconTsyReqHandle = aTsyReqHandle;

        // Image instance elementary file (includes also clut ) was read in
        // previous phase. Read data from sat message handler and complete
        // request. Check icon data length
        if( KIconDataMaxSize > (*aClut).MaxLength() )
            {
            TBuf8<1> infoNotSet( 0 );
            TPtrC8 clutData( iSatMessHandler->GetClut() );
            CompleteIconRequest( KErrNone, clutData, infoNotSet );
            }
        }
    TFLOGSTRING2("TSY: CSatIcon::GetClut, ret: %d", ret );
    OstTrace1( TRACE_NORMAL,  DUP3_CSATICON_GETCLUT_TD, "CSatIcon::GetClut, ret: %d", ret );

    iImageInstanceNoInRecord = 0;
    return ret;
    }



// ================= OTHER EXPORTED FUNCTIONS =================================

