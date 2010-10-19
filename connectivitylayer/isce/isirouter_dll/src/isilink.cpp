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
#include <phonetisi.h>          // // For ISI_HEADER_OFFSET_MEDIA

#include "isilink.h"            // For DISILink
#include "iscedefs.h"           // For EMuxISI...
#include "mlinkmuxif.h"         // For MLinkMuxIf
#include "isiroutertrace.h"     // For C_TRACE, ASSERT_RESET.. and fault codes
#include "misilinkrouterif.h"   // For MISILinkRouterIf

// Faults
enum TISILinkFaults
    {
    EISILinkNullPtr = 0x01,
    EISILinkMemAllocFailed,
    EISILinkInitFailed,
    EISILinkInitFailed2,
    EISILinkRegisterFailed,
    EISILinkWrongMedia,
    EISILinkTrxPresenceSyncFailed,
    };


MISIRouterLinkIf* MISIRouterLinkIf::CreateLinkF(
        MISILinkRouterIf* aRouter,
        const TUint8 aMediaId,
        const TUint8 aTrxId
        )
    {
    C_TRACE( ( _T( "MISIRouterLinkIf::CreateLinkF 0x%x 0x%x 0x%x>" ), aRouter, aMediaId, aTrxId ) );
    ASSERT_RESET_ALWAYS( aRouter, ( EISILinkNullPtr | EDISILinkTraceId << KClassIdentifierShift ) );
    DISILink* link = new DISILink( aRouter, aMediaId, aTrxId );
    ASSERT_RESET_ALWAYS( link, ( EISILinkMemAllocFailed | EDISILinkTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "MISIRouterLinkIf::CreateLinkF 0x%x 0x%x 0x%x created 0x%x<" ), aRouter, aMediaId, aTrxId, link ) );
    return link;
    }

DISILink::DISILink(
        ) : iRouter( NULL ),
            iMux( NULL ),
            iTrxPresent( EFalse )
    {
    C_TRACE( ( _T( "DISILink::DISILink 0x%x>" ), this ) );
    iMediaId = 0; 
    C_TRACE( ( _T( "DISILink::DISILink 0x%x<" ), this ) );
    }

DISILink::DISILink(
        MISILinkRouterIf* aRouter,
        const TUint8 aMediaId,
        const TUint8 aTrxId
        ) : iRouter( aRouter ),
            iMediaId( aMediaId ),
            iTrxId( aTrxId )
    {
    C_TRACE( ( _T( "DISILink::DISILink 0x%x mediaid 0x%x trxid 0x%x>" ), this, aMediaId, aTrxId ) );
    // ISI Media
    iMux = MLinkMuxIf::Register( this, aTrxId, EMuxISI );
    ASSERT_RESET_ALWAYS( aRouter, ( EISILinkRegisterFailed | EDISILinkTraceId << KClassIdentifierShift ) );
    C_TRACE( ( _T( "DISILink::DISILink 0x%x mediaid 0x%x trxid 0x%x<" ), this, iMediaId, iTrxId ) );
    }

DISILink::~DISILink()
    {
    C_TRACE( ( _T( "DISILink::~DISILink 0x%x>" ), this ) );
    iMux = NULL;
    iRouter = NULL;
    C_TRACE( ( _T( "DISILink::~DISILink 0x%x<" ), this ) );
    }

// From MMuxLinkIf start
void DISILink::Receive(
        TDes8& aMsg
        )
    {
    C_TRACE( ( _T( "DISILink::Receive 0x%x 0x%x>" ), this, &aMsg ) );
    ASSERT_RESET_ALWAYS( iMux, ( EISILinkInitFailed | EDISILinkTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( aMsg.Ptr()[ ISI_HEADER_OFFSET_MEDIA ] == iMediaId, ( EISILinkWrongMedia | aMsg.Ptr()[ 0 ] << KMediaIdShift | EDISILinkTraceId << KClassIdentifierShift ) );
    iRouter->ReceiveISIMessage( aMsg, iTrxId );
    C_TRACE( ( _T( "DISILink::Receive 0x%x 0x%x<" ), this, &aMsg ) );
    }

/*
* Called with FM held, no blocking no nesting of FMs and no allocation.
*/
void DISILink::EnqueTrxPresenceChangedDfc(
        TBool aPresent
        )
    {
    // No traces allowed due to not allowed to block.
    ASSERT_RESET_ALWAYS( ( iTrxPresent != aPresent ), ( EISILinkTrxPresenceSyncFailed | EDISILinkTraceId << KClassIdentifierShift ) );
    iTrxPresent = aPresent;
    iRouter->StateChanged();
    }
// From MMuxLinkIf end

// From MISIRouterLinkIf start
void DISILink::Release(
        // None
        )
    {
    C_TRACE( ( _T( "DISILink::Release 0x%x>" ), this ) );
    delete this;
    C_TRACE( ( _T( "DISILink::Release 0x%x<" ), this ) );
    }

TBool DISILink::Send(
        TDes8& aMsg
        )
    {
    C_TRACE( ( _T( "DISILink::Send 0x%x 0x%x iMediaId 0x%x >" ), this, &aMsg, iMediaId  ) );
    ASSERT_RESET_ALWAYS( iMux, ( EISILinkInitFailed2 | EDISILinkTraceId << KClassIdentifierShift ) );
    if( iTrxPresent )
        {
        TUint8* msgPtr = const_cast<TUint8*>( aMsg.Ptr() );
        msgPtr[ ISI_HEADER_OFFSET_MEDIA ] = iMediaId;
        // Send with default prio.
        iMux->Send( aMsg, EMuxISI );
        C_TRACE( ( _T( "DISILink::Send ok 0x%x 0x%x 0x%x<" ), this, iTrxPresent, &aMsg) );
        return ETrue;
        }
    // No need for else, by default false
    C_TRACE( ( _T( "DISILink::Send failed 0x%x 0x%x 0x%x<" ), this, iTrxPresent, &aMsg) );
    return EFalse;
    }
    
TUint8 DISILink::GetTrxId()
    {
    //C_TRACE( ( _T( "DISILink::GetTrxId return %d <>" ), this, iTrxId ) );
    return iTrxId;
    
    }
// From MISIRouterLinkIf end
