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

#include "p2plink.h"            // For DP2PLink
#include "p2proutertrace.h"     // For C_TRACE..
#include "mlinkmuxif.h"         // For MLinkMuxIf
#include "iscedefs.h"           // For EMuxAmountOfProtocols
#include "p2pdefs.h"            // For EP2PAmountOfProtocols
#include "trxdefs.h"            // For EAmountOfTrxs

// Faults
enum TP2PLinkFaults
    {
    EP2PLinkNullPtr = 0x00,
    EP2PMemAllocFailed,
    EP2PInitFailed,
    EP2PInitFailed2,
    EP2PRegisterFailed,
    };

// Macros
// Because P2P ids are different than multiplexing ids they had to be mapped.
// There is other than P2P protocols multiplexed and they shall not be visible 
// in P2P routing nor API.
#define MAP_P2P_TO_MUX( aP2PId )( aP2PId + ( EMuxAmountOfProtocols - EP2PAmountOfProtocols ) )
#define MAP_MUX_TO_P2P( aMuxId )( aMuxId - ( EMuxAmountOfProtocols - EP2PAmountOfProtocols ) )

MP2PRouterLinkIf* MP2PRouterLinkIf::CreateLinkF(
        MP2PLinkRouterIf* aRouter,
        const TUint8 aProtocolId,
        const TUint8 aTrxId
        )
    {

    C_TRACE( ( _T( "MP2PRouterLinkIf::CreateLinkF 0x%x %d %d>" ), aRouter, aProtocolId, aTrxId ) );
    ASSERT_RESET_ALWAYS( aRouter, ( EP2PLinkNullPtr | EDP2PLinkTraceId << KClassIdentifierShift ) );
    DP2PLink* link = new DP2PLink( aRouter, aProtocolId, aTrxId );
    ASSERT_RESET_ALWAYS( aRouter, ( EP2PMemAllocFailed | EDP2PLinkTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "MP2PRouterLinkIf::CreateLinkF 0x%x %d %d created 0x%x<" ), aRouter, aProtocolId, aTrxId, link ) );
    return link;

    }

DP2PLink::DP2PLink(
        ) : iRouter( NULL )
    {

    C_TRACE( ( _T( "DP2PLink::DP2PLink 0x%x>" ), this ) );
    iProtocolId = 0x00;
    iShTrxId = 0x00;
    iMux = NULL;
    C_TRACE( ( _T( "DP2PLink::DP2PLink 0x%x<" ), this ) );

    }

DP2PLink::DP2PLink(
        MP2PLinkRouterIf* aRouter,
        const TUint8 aProtocolId,
        const TUint8 aTrxId
        ) : iRouter( aRouter )
    {

    C_TRACE( ( _T( "DP2PLink::DP2PLink 0x%x protid %d map %d trxid %d>" ), this, aProtocolId, iProtocolId, aTrxId ) );
    // Map from Point-to-point protocol to multiplexing protocol.
    iProtocolId = MAP_P2P_TO_MUX( aProtocolId );
    iShTrxId = ~aTrxId;
    iMux = MLinkMuxIf::Register( this, aTrxId, iProtocolId );
    ASSERT_RESET_ALWAYS( aRouter, ( EP2PRegisterFailed | EDP2PLinkTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DP2PLink::DP2PLink 0x%x protid %d map %d trxid %d<" ), this, aProtocolId, iProtocolId, aTrxId ) );

    }

DP2PLink::~DP2PLink()
    {

    C_TRACE( ( _T( "DP2PLink::~DP2PLink 0x%x>" ), this ) );
    C_TRACE( ( _T( "DP2PLink::~DP2PLink 0x%x<" ), this ) );

    }


// From MMuxLinkIf start
void DP2PLink::Receive(
        TDes8& aMsg
        )
    {

    C_TRACE( ( _T( "DP2PLink::Receive 0x%x 0x%x id %d>" ), this, &aMsg, iProtocolId ) );
    ASSERT_RESET_ALWAYS( iMux, ( EP2PInitFailed | EDP2PLinkTraceId << KClassIdentifierShift ) );
    iRouter->Receive( aMsg, MAP_MUX_TO_P2P( iProtocolId ) );
    C_TRACE( ( _T( "DP2PLink::Receive 0x%x 0x%x id %d<" ), this, &aMsg, iProtocolId ) );

    }

/*
* Called with FM held, no blocking no nesting of FMs and no allocation.
*/
void DP2PLink::EnqueTrxPresenceChangedDfc(
        TBool aPresent
        )
    {

    // No traces allowed due to not allowed to block.
    // Negation sets variable to be over EAmountOfTrxs if it is not present.
    iShTrxId = ~iShTrxId;//  check this atomicity and synch if needed
    iRouter->NotifyTrxPresenceEnqueDfc( aPresent );

    }
// From MMuxLinkIf end

// From MP2PRouterLinkIf start
TBool DP2PLink::TrxPresent()
    {

    C_TRACE( ( _T( "DP2PLink::TrxPresent 0x%x>" ), this ) );
    // Amount of trx shall always be less than the negative
    TBool value( iShTrxId < EAmountOfTrxs ? ETrue : EFalse );
    C_TRACE( ( _T( "DP2PLink::TrxPresent 0x%x %d<" ), this, value ) );
    return value;

    }

void DP2PLink::Release(
        // None
        )
    {

    C_TRACE( ( _T( "DP2PLink::Release 0x%x>" ), this ) );
    delete this;
    C_TRACE( ( _T( "DP2PLink::Release 0x%x<" ), this ) );

    }

void DP2PLink::Send(
        TDes8& aMsg
        )
    {

    C_TRACE( ( _T( "DP2PLink::Send 0x%x 0x%x map %d <>" ), this, &aMsg, iProtocolId ) );
    ASSERT_RESET_ALWAYS( iMux, ( EP2PInitFailed2 | EDP2PLinkTraceId << KClassIdentifierShift ) );
    // Send with default prio.
    iMux->Send( aMsg, iProtocolId ); //  send priorities

    }

// From MP2PRouterLinkIf end

