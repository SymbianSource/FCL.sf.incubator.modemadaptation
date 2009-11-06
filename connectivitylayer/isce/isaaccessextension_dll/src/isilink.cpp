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



#include <kernel.h>             // For Kern
#include <pn_const.h>           // For PN_MEDIA_NOT_USED
#include "isilink.h"            // For DISILink
#include "iadtrace.h"           // For C_TRACE..
#include "mlinkmuxif.h"         // For MLinkMuxIf
#include "iscedefs.h"           // For EMuxAmountOfProtocols
#include "trxdefs.h"            // For EAmountOfTrxs
#include "iscnokiadefinitions.h"// For PN_MEDIA_NOT_USED
// Faults
enum TISILinkFaults
    {
    EISILinkNullPtr = 0x00,
    EISIMemAllocFailed,
    EISIInitFailed,
    EISIInitFailed2,
    EISIRegisterFailed,
    EISIWrongMedia,
    EISIWrongMedia2,
    };

MISIRouterLinkIf* MISIRouterLinkIf::CreateLinkF(
        MISILinkRouterIf* aRouter,
        const TUint8 aMediaId,
        const TUint8 aTrxId
        )
    {

    C_TRACE( ( _T( "MISIRouterLinkIf::CreateLinkF 0x%x %d %d>" ), aRouter, aMediaId, aTrxId ) );
    ASSERT_RESET_ALWAYS( aRouter, ( EISILinkNullPtr | EDISILinkTraceId << KClassIdentifierShift ) );
    DISILink* link = new DISILink( aRouter, aMediaId, aTrxId );
    ASSERT_RESET_ALWAYS( aRouter, ( EISIMemAllocFailed | EDISILinkTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "MISIRouterLinkIf::CreateLinkF 0x%x %d %d created 0x%x<" ), aRouter, aMediaId, aTrxId, link ) );
    return link;

    }

DISILink::DISILink(
        ) : iRouter( NULL )
    {

    C_TRACE( ( _T( "DISILink::DISILink 0x%x>" ), this ) );
    iMediaId = PN_MEDIA_NOT_USED;
    iShTrxId = EAmountOfTrxs;
    iMux = NULL;
    C_TRACE( ( _T( "DISILink::DISILink 0x%x<" ), this ) );

    }

DISILink::DISILink(
        MISILinkRouterIf* aRouter,
        const TUint8 aMediaId,
        const TUint8 aTrxId
        ) : iRouter( aRouter )
    {

    C_TRACE( ( _T( "DISILink::DISILink 0x%x protid %d map %d trxid %d>" ), this, aMediaId, iMediaId, aTrxId ) );
    // ISI Media.
    iMediaId = aMediaId;
    iShTrxId = ~aTrxId;
    iMux = MLinkMuxIf::Register( this, aTrxId, EMuxISI );
    ASSERT_RESET_ALWAYS( aRouter, ( EISIRegisterFailed | EDISILinkTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISILink::DISILink 0x%x protid %d map %d trxid %d<" ), this, aMediaId, iMediaId, aTrxId ) );

    }

DISILink::~DISILink()
    {

    C_TRACE( ( _T( "DISILink::~DISILink 0x%x>" ), this ) );
    C_TRACE( ( _T( "DISILink::~DISILink 0x%x<" ), this ) );

    }


// From MMuxLinkIf start
void DISILink::Receive(
        TDes8& aMsg
        )
    {

    C_TRACE( ( _T( "DISILink::Receive 0x%x 0x%x id %d>" ), this, &aMsg, iMediaId ) );
    ASSERT_RESET_ALWAYS( iMux, ( EISIInitFailed | EDISILinkTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( aMsg.Ptr()[ 0 ] == iMediaId, ( EISIWrongMedia | EDISILinkTraceId << KClassIdentifierShift ) );
    iRouter->Receive( aMsg );
    C_TRACE( ( _T( "DISILink::Receive 0x%x 0x%x id %d<" ), this, &aMsg, iMediaId ) );

    }

/*
* Called with FM held, no blocking no nesting of FMs and no allocation.
*/
void DISILink::EnqueTrxPresenceChangedDfc(
        TBool aPresent
        )
    {

    // No traces allowed due to not allowed to block.
    // Negation sets variable to be over EAmountOfTrxs if it is not present.
    iShTrxId = ~iShTrxId;// TODO: check this atomicity and synch if needed
    iRouter->NotifyTrxPresenceEnqueDfc( aPresent );

    }
// From MMuxLinkIf end

// From MISIRouterLinkIf start
TBool DISILink::TrxPresent()
    {

    C_TRACE( ( _T( "DISILink::TrxPresent 0x%x>" ), this ) );
    // Amount of trx shall always be less than the negative
    TBool value( iShTrxId < EAmountOfTrxs ? ETrue : EFalse );
    C_TRACE( ( _T( "DISILink::TrxPresent 0x%x %d<" ), this, value ) );
    return value;

    }

void DISILink::Release(
        // None
        )
    {

    C_TRACE( ( _T( "DISILink::Release 0x%x>" ), this ) );
    delete this;
    C_TRACE( ( _T( "DISILink::Release 0x%x<" ), this ) );

    }

void DISILink::Send(
        TDes8& aMsg
        )
    {

    C_TRACE( ( _T( "DISILink::Send 0x%x 0x%x map %d <>" ), this, &aMsg, iMediaId ) );
    ASSERT_RESET_ALWAYS( iMux, ( EISIInitFailed2 | EDISILinkTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( aMsg.Ptr()[ 0 ] == iMediaId, ( EISIWrongMedia2 | EDISILinkTraceId << KClassIdentifierShift ) );
    // Send with default prio.
    iMux->Send( aMsg, EMuxISI ); // TODO: send priorities

    }

// From MISIRouterLinkIf end

