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



//  EXTERNAL RESOURCES

//  Include Files
#include <ctsy/rmmcustomapi.h>
#include "cmmsmscache.h"
#include "tsylogger.h"
#include "tisi.h"
#include <smsisi.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmsmscacheTraces.h"
#endif

//  External Data Structures
//  none

//  External Function Prototypes
//  none

//  LOCAL CONSTANTS AND MACROS
//  none

//  MODULE DATA STRUCTURES

//  Local Data Structures
//  none

//  Local Function Prototypes
//  none

//  LOCAL FUNCTIONS
//  none

//  MEMBER FUNCTIONS

//=============================================================================

// -----------------------------------------------------------------------------
// CMmSmsCache::CMmSmsCache
// Creates new CMmSmsCache object
// -----------------------------------------------------------------------------
//
CMmSmsCache::CMmSmsCache()
    {
TFLOGSTRING("TSY: CMmSmsCache::CMmSmsCache");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_CMMSMSCACHE_TD, "CMmSmsCache::CMmSmsCache" );
    Reset();
    // If SIM is offline,then won't get
    // sim ready completion and cache error state stays as
    // KErrNotFound. If user accesses SIM SMS --> immediate complete
    // with KErrNotFound
    SetStatus( KErrNotFound );
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::~CMmSmsCache
// delete CMmSmsCache
// -----------------------------------------------------------------------------
//
CMmSmsCache::~CMmSmsCache()
    {
TFLOGSTRING("TSY: CMmSmsCache::~CMmSmsCache");
OstTrace0( TRACE_NORMAL,  DUP1_CMMSMSCACHE_CMMSMSCACHE_TD, "CMmSmsCache::~CMmSmsCache" );
    Reset();
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::Reset
// reset cache to not ready state
// -----------------------------------------------------------------------------
//
void CMmSmsCache::Reset()
    {
TFLOGSTRING("TSY: CMmSmsCache::Reset");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_RESET_TD, "CMmSmsCache::Reset" );

    iError = KErrNotReady;
    iDeleteLocation = 0;

    for ( TInt i = 0; i < iElements.Count(); i++ )
        {
        delete iElements[i];
        iElements[i] = NULL;
        }
    iElements.Reset();
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::UsedEntries
// get the number of used entries from cache
// -----------------------------------------------------------------------------
//
TInt CMmSmsCache::UsedEntries()
    {
TFLOGSTRING("TSY: CMmSmsCache::UsedEntries");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_USEDENTRIES_TD, "CMmSmsCache::UsedEntries" );
    TInt count( 0 );
    for ( TInt i = 0; i < iElements.Count(); i++ )
       {
       if ( iElements[i] )
           {
           count++;
           }
       }
    return count;
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::TotalEntries
// get the total number of entries in SIM
// -----------------------------------------------------------------------------
//
TInt CMmSmsCache::TotalEntries()
    {
TFLOGSTRING2("TSY: CMmSmsCache::TotalEntries: %d",iElements.Count());
OstTrace1( TRACE_NORMAL,  CMMSMSCACHE_TOTALENTRIES_TD, "CMmSmsCache::TotalEntries;iElements.Count=%d", iElements.Count() );
    return iElements.Count();
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::Status
// Returns cache status, this can be KErrNotReady (cache is being read)
// KErrNone (cache is ok and can be used)
// any other (cache is in error state and is unusable)
// -----------------------------------------------------------------------------
//
TInt CMmSmsCache::Status()
    {
TFLOGSTRING("TSY: CMmSmsCache::Status");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_STATUS_TD, "CMmSmsCache::Status" );
    return iError;
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::SetStatus
// set cache error state
// -----------------------------------------------------------------------------
//
void CMmSmsCache::SetStatus( TInt aError )
    {
TFLOGSTRING2("TSY: CMmSmsCache::SetStatus %d", aError);
OstTrace1( TRACE_NORMAL,  CMMSMSCACHE_SETSTATUS_TD, "CMmSmsCache::SetStatus;aError=%d", aError );
    iError = aError;
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::SetTotalEntriesL
// initialize cache to contain aTotal number of empty entries
// it is assumed that cache is Reset before call to this..
// -----------------------------------------------------------------------------
//
void CMmSmsCache::SetTotalEntriesL( TInt aTotal )
    {
TFLOGSTRING2("TSY: CMmSmsCache::SetTotalEntriesL %d", aTotal);
OstTrace1( TRACE_NORMAL,  CMMSMSCACHE_SETTOTALENTRIESL_TD, "CMmSmsCache::SetTotalEntriesL;aTotal=%d", aTotal );

    RMobileSmsStore::TMobileGsmSmsEntryV1* element = NULL;
    for ( TInt i = 0; i < aTotal; i++ )
        {
        iElements.AppendL( element );
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::AddEntryL
// adds new entry to cache, if the cause of isi msg is not
// ok, then we add just a null pointer (to save space)
// returns true if element was cache and false
// if cache location is empty
// -----------------------------------------------------------------------------
//
TBool CMmSmsCache::AddEntryL(
    const RMobileSmsStore::TMobileGsmSmsEntryV1* aEntry,
    const TUint8  aRecordId )
    {
TFLOGSTRING("TSY: CMmSmsCache::AddEntryL");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_ADDENTRYL_TD, "CMmSmsCache::AddEntryL" );

    if ( 0 < iElements.Count() )
        {
        delete iElements[aRecordId - 1];
        iElements[aRecordId - 1] = NULL;
        }

    // If the location is non-empty (filled with 0xFF (3GPP TS 31.102
    // 4.2.25 EFSMS (Short messages)))

    if ( EMPTY_LOCATION != aEntry->iMsgStatus )
        {
        // Make a copy of the received SIM SMS.
        // Reserve heap memory; can leave if out of mem.
        RMobileSmsStore::TMobileGsmSmsEntryV1* tmpBuf =
            new( ELeave ) RMobileSmsStore::TMobileGsmSmsEntryV1;
        *tmpBuf = *aEntry;
        // copy data
        iElements[aRecordId - 1] = tmpBuf;

        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::GetEntry
// get entry from cache, returns null if cache doesn't c;ontain
// valid entry in the given location location range is in range
// [1... max sim sms slots]
// -----------------------------------------------------------------------------
//
RMobileSmsStore::TMobileGsmSmsEntryV1* CMmSmsCache::GetEntry( TInt aLocation )
    {
TFLOGSTRING2("TSY: CMmSmsCache::GetEntry - location: %d", aLocation);
OstTrace1( TRACE_NORMAL,  CMMSMSCACHE_GETENTRY_TD, "CMmSmsCache::GetEntry;aLocation=%d", aLocation );
    RMobileSmsStore::TMobileGsmSmsEntryV1* smsData = NULL;
    if ( aLocation <= iElements.Count() && aLocation >= 1 )
        {
        if ( iElements[aLocation-1] )
            {
            RMobileSmsStore::TMobileGsmSmsEntryV1* tempHBuf(
                iElements[aLocation-1] );
            smsData = tempHBuf;
            }
        }
    return smsData;
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::FirstFreeLocation
// return first free location in range [1... max sim sms slots]
// or zero if no free slots are found
// -----------------------------------------------------------------------------
//
TUint CMmSmsCache::FirstFreeLocation()
    {
TFLOGSTRING("TSY: CMmSmsCache::FirstFreeLocation");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_FIRSTFREELOCATION_TD, "CMmSmsCache::FirstFreeLocation" );
    TInt location( 0 );
    for ( TInt i = 0; i < iElements.Count(); i++ )
        {
        if ( !iElements[i] )
            {
            location = i + 1;
            i = iElements.Count(); // exit loop
            }
        }
TFLOGSTRING2("TSY: CMmSmsCache::FirstFreeLocation - found location: %d", location);
OstTrace0( TRACE_NORMAL,  DUP1_CMMSMSCACHE_FIRSTFREELOCATION_TD, "CMmSmsCache::FirstFreeLocation" );
    return location;
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::SetDeleteLocation
// set the location to delete by Delete()
// -----------------------------------------------------------------------------
//
void CMmSmsCache::SetDeleteLocation( TInt aLocation )
    {
TFLOGSTRING2("TSY: CMmSmsCache::SetDeleteLocation %d", aLocation);
OstTrace1( TRACE_NORMAL,  CMMSMSCACHE_SETDELETELOCATION_TD, "CMmSmsCache::SetDeleteLocation;aLocation=%d", aLocation );
    iDeleteLocation = aLocation;
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::Delete
// if flush location is set, then flush it
// -----------------------------------------------------------------------------
//
void CMmSmsCache::Delete()
    {
TFLOGSTRING("TSY: CMmSmsCache::Delete");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_DELETE_TD, "CMmSmsCache::Delete" );
    if ( iDeleteLocation!=0 )
        {
        Delete( iDeleteLocation );
        iDeleteLocation = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::Delete
//  Delete an element from cache
// -----------------------------------------------------------------------------
//
void CMmSmsCache::Delete( TInt aLocation )
    {
TFLOGSTRING2("TSY: CMmSmsCache::Delete %d", aLocation);
OstTrace1( TRACE_NORMAL,  DUP1_CMMSMSCACHE_DELETE_TD, "CMmSmsCache::Delete;aLocation=%d", aLocation );

    if ( aLocation <= iElements.Count() && aLocation >= 1 )
        {
        delete iElements[aLocation-1];
        iElements[aLocation-1] = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::DeleteAll
// empty all of cache
// -----------------------------------------------------------------------------
//
void CMmSmsCache::DeleteAll()
    {
TFLOGSTRING("TSY: CMmSmsCache::DeleteAll");
OstTrace0( TRACE_NORMAL,  CMMSMSCACHE_DELETEALL_TD, "CMmSmsCache::DeleteAll" );
    for ( TInt i = 0; i < iElements.Count(); i++ )
        {
        delete iElements[i];
        iElements[i] = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CMmSmsCache::SetStorageStatus
// Changes the storage status of a cached entry (TS 31.102, clause 4.2.25)
// -----------------------------------------------------------------------------
//
void CMmSmsCache::SetStorageStatus(
    TInt aLocation,
    RMobileSmsStore::TMobileSmsStoreStatus aMsgStatus )
    {
TFLOGSTRING2("TSY: CMmSmsCache::SetStorageStatus(loc=%d)", aLocation);
OstTrace1( TRACE_NORMAL,  CMMSMSCACHE_SETSTORAGESTATUS_TD, "CMmSmsCache::SetStorageStatus;aLocation=%d", aLocation );

    if ( aLocation <= iElements.Count()
        && aLocation >= 1
        && iElements[aLocation-1] )
        {
        // Entry exists, update status.
        iElements[aLocation-1]->iMsgStatus = aMsgStatus;
        }
    }
//  End of File
