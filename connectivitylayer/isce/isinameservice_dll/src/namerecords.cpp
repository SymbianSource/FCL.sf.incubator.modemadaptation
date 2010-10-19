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


#include "namerecords.h"
#include <nsisi.h>
#include "isinameservicetrace.h"  // For C_TRACE, ASSERT_RESET.. and fault codes
#include "isihelpers.h"           // For PN_DEV_OWN

enum TISINameRecordsFaults
    {
    EISINameRecordsMemAllocFailure = 0x01,
    EISINameRecordsNullPointer,
    EISINameRecordsNullPointer2,
    EISINameRecordsNullPointer3,
    EISINameRecordsNullPointer4,
    EISINameRecordsNotFoundEntry,
    };

const TUint8 KAlwaysAssert( 0x00 );
const TUint16 KOwnDeviceRouter( PN_DEV_OWN << 8 );

DNameRecords::DNameRecords()
    {
    //nothing
    C_TRACE( ( _T( "DNameRecords::DNameRecords <>" ) ) );
    }

DNameRecords::~DNameRecords()
    {
    C_TRACE( ( _T( "DNameRecords::~DNameRecords>" ) ) );
    for( TUint32 i( 0 ); i < iNameTableArray.Count(); i++ )
        {
        delete iNameTableArray[ i ];
        iNameTableArray.Remove( i );
        }
    iNameTableArray.Reset();
    C_TRACE( ( _T( "DNameRecords::~DNameRecords<" ) ) );
    }

TUint8 DNameRecords::AddName( 
        const TUint32 aName,
        const TUint16 aPhonetAddress,
        const TUint8 aFlags
        )
    {
    C_TRACE( ( _T( "DNameRecords::AddName name:0x%x phonetAdd:0x%04x flags: 0x%x>" ), aName, aPhonetAddress, aFlags ) );
    TUint8 error( PN_NAME_OK );
    switch( aFlags )
        {
        case PN_NAME_UNDEF :
        case PN_NAME_NOCHG :
            {
            error = AddNewName( aName, aPhonetAddress, aFlags );
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS( KAlwaysAssert );
            error = PN_NAME_UNKNOWN;
            break;
            }
        }
    C_TRACE( ( _T( "DNameRecords::AddName name:0x%x phonetAdd:0x%04x flags: 0x%x %d<" ), aName, aPhonetAddress, aFlags, error ) );
    return error;
    }

TUint8 DNameRecords::RemoveName(
        const TUint32 aName
        )
    {
    C_TRACE( ( _T( "DNameRecords::RemoveName name:0x%x>" ), aName ) );
    TUint8 error( PN_NAME_UNKNOWN );
    TNameTableEntry* nameEntry = FindMatchingNameTableEntry( aName );
    if( nameEntry )
        {
        if( nameEntry->iFlags == PN_NAME_UNDEF )
            {
            RemoveUndefinedEntry( *nameEntry );
            error = PN_NAME_OK;
            }
        else
            {
            error = PN_NAME_NOT_ALLOWED;
            TRACE_ASSERT_ALWAYS( KAlwaysAssert );
            }
        }
     // No need for else, as name is unknown
    TRACE_ASSERT_ALWAYS( PN_NAME_OK == error );
    C_TRACE( ( _T( "DNameRecords::RemoveName name:0x%x %d<" ), aName, error ) );
    return error;
    }

TInt DNameRecords::LookupPhonetAddress(
        const TUint32 aName,
        TUint16* aPhonetAddress
        )
    {
    C_TRACE( ( _T( "DNameRecords::LookupPhonetAddress name:0x%x>" ), aName ) );
    ASSERT_RESET_ALWAYS( aPhonetAddress, ( EISINameRecordsNullPointer2 | EDISINameRecordsTraceId << KClassIdentifierShift ) );
    TInt error( KErrNotFound );
    TNameTableEntry* nameEntry = FindMatchingNameTableEntry( aName );
    if( nameEntry )
        {
        *aPhonetAddress = nameEntry->iPhonetAddress;
        error = KErrNone;
        }
     // No need for else, as name is not found
    C_TRACE( ( _T( "DNameRecords::LookupPhonetAddress name:0x%x %d<" ), aName, error ) );
    return error;
    }

TInt DNameRecords::LookupNamePhonetAddressAndFlags(
        const TUint32 aName,
        TUint16* aPhonetAddress,
        TUint8* aFlags
        )
    {
    C_TRACE( ( _T( "DNameRecords::LookupNamePhonetAddressAndFlags name:0x%x>" ), aName ) );
    ASSERT_RESET_ALWAYS( aPhonetAddress, ( EISINameRecordsNullPointer3 | EDISINameRecordsTraceId << KClassIdentifierShift ) );
    ASSERT_RESET_ALWAYS( aFlags, ( EISINameRecordsNullPointer4 | EDISINameRecordsTraceId << KClassIdentifierShift ) );
    TInt error( KErrNotFound );
    TNameTableEntry* nameEntry = FindMatchingNameTableEntry( aName );
    if( nameEntry )
        {
        *aPhonetAddress = nameEntry->iPhonetAddress;
        *aFlags = nameEntry->iFlags;
        error = KErrNone;
        }
     // No need for else, as name is not found
    C_TRACE( ( _T( "DNameRecords::LookupNamePhonetAddressAndFlags name:0x%x %d<" ), aName, error ) );
    return error;
    }

void DNameRecords::NameQuery(
        const TUint32 aName,
        const TUint32 aMask,
        RArray <TNameTableEntry*>* aResultsArray
        )
    {
    C_TRACE( ( _T( "DNameRecords::NameQuery name:0x%x mask:0x%x &aResultsArray: 0x%x>" ), aName, aMask, &aResultsArray ) );
    FindMatchingNameTableEntriesWithMask( aName, aMask, aResultsArray );
    C_TRACE( ( _T( "DNameRecords::NameQuery name:0x%x mask:0x%x &aResultsArray: 0x%x<" ), aName, aMask, &aResultsArray ) );
    }

// privates start

TUint8 DNameRecords::AddNewName(
        const TUint32 aName,
        const TUint16 aPhonetAddress,
        const TUint8 aFlags
        )
    {
    E_TRACE( ( _T( "DNameRecords::AddNewName 0x%x 0x%04x 0x%x>" ), aName, aPhonetAddress, aFlags ) );
    const TUint32 nameTableCount( iNameTableArray.Count() );
    TUint8 error( PN_NAME_OK );
    TNameTableEntry* nameEntry = FindMatchingNameTableEntry( aName );
    if( nameEntry )
        {
        error = AddExistinNameToNameTableArray( nameEntry, aName, aPhonetAddress, aFlags );
        }
    else
        {
        error = AddNewNameToNameTableArray( aName, aPhonetAddress, aFlags );
        }
    E_TRACE( ( _T( "DNameRecords::AddNewName 0x%x 0x%04x 0x%x error %d<" ), aName, aPhonetAddress, aFlags, error ) );
    return error;
    }

TUint8 DNameRecords::AddExistinNameToNameTableArray(
        TNameTableEntry* aNameTableEntry,
        const TUint32 aName,
        const TUint16 aPhonetAddress,
        const TUint8 aFlags
        )
    {
    E_TRACE( ( _T( "DNameRecords::AddExistinNameToNameTableArray 0x%x 0x%04x 0x%x exists 0x%x>" ), aName, aPhonetAddress, aFlags, aNameTableEntry ) );
    TUint8 error( PN_NAME_NOT_ALLOWED );
    ASSERT_RESET_ALWAYS( aNameTableEntry, ( EISINameRecordsNullPointer | EDISINameRecordsTraceId << KClassIdentifierShift ) );
    if( aNameTableEntry->iFlags != PN_NAME_NOCHG )
        {
        aNameTableEntry->iPhonetAddress = aPhonetAddress;
        aNameTableEntry->iFlags = aFlags;
        error = PN_NAME_OK;
        }
    // No need for else, if not changeable doing nothing.
    TRACE_ASSERT_ALWAYS( PN_NAME_OK == error );
    E_TRACE( ( _T( "DNameRecords::AddExistinNameToNameTableArray 0x%x 0x%04x 0x%x exists 0x%x error %d<" ), aName, aPhonetAddress, aFlags, aNameTableEntry, error ) );
    return error;
    }

TUint8 DNameRecords::AddNewNameToNameTableArray(
        const TUint32 aName,
        const TUint16 aPhonetAddress,
        const TUint8 aFlags
        )
    {
    E_TRACE( ( _T( "DNameRecords::AddNewNameToNameTableArray 0x%x 0x%04x 0x%x>" ), aName, aPhonetAddress, aFlags ) );
    TUint8 error( PN_NAME_NOT_ALLOWED );
    // This check is only needed here as these PhoNet addresses may never exist.
    if( KOwnDeviceRouter != aPhonetAddress )
        {
        TNameTableEntry* table = new TNameTableEntry( aName, aPhonetAddress, aFlags );
        E_TRACE( ( _T( "DNameRecords::AddNewNameToNameTableArray 0x%x 0x%x 0x%x new 0x%x<>" ), aName, aPhonetAddress, aFlags, table ) );
        ASSERT_RESET_ALWAYS( table, ( EISINameRecordsMemAllocFailure | EDISINameRecordsTraceId << KClassIdentifierShift ) );
        iNameTableArray.Append( table );
        error = PN_NAME_OK;
        }
    // No need for else as not allowed.
    TRACE_ASSERT_ALWAYS( PN_NAME_OK == error );
    E_TRACE( ( _T( "DNameRecords::AddNewNameToNameTableArray 0x%x 0x%04x 0x%x 0x%x<" ), aName, aPhonetAddress, aFlags, error ) );
    return error;
    }

TNameTableEntry* DNameRecords::FindMatchingNameTableEntry(
        const TUint32 aName
        )
    {
    E_TRACE( ( _T( "DNameRecords::FindMatchingNameTableEntry 0x%x>" ), aName ) );
    TNameTableEntry* tableEntry = NULL;
    const TUint32 nameTableCount( iNameTableArray.Count() );
    for( TUint32 i( 0 ); i < nameTableCount; i++ )
        {
        if( iNameTableArray[ i ]->iName == aName )
            {
            tableEntry = iNameTableArray[ i ];
            break;
            }
        // No need for else, continue for
        }
    E_TRACE( ( _T( "DNameRecords::FindMatchingNameTableEntry 0x%x entry 0x%x<" ), aName, tableEntry ) );
    return tableEntry;
    }

void DNameRecords::FindMatchingNameTableEntriesWithMask(
        const TUint32 aName,
        const TUint32 aMask,
        RArray <TNameTableEntry*>* aResultsArray
        )
    {
    E_TRACE( ( _T( "DNameRecords::FindMatchingNameTableEntriesWithMask 0x%x 0x%x 0x%x>" ), aName, aMask, aResultsArray ) );
    const TUint32 nameTableCount( iNameTableArray.Count() );
    for( TUint32 i( 0 ); i < nameTableCount; i++ )
        {
        TNameTableEntry* tableEntry = iNameTableArray[ i ];
        if( ( aName == tableEntry->iName ) ||
            ( ( aName & aMask ) == ( tableEntry->iName & aMask ) ) )
            {
            E_TRACE( ( _T( "DNameRecords::FindMatchingNameTableEntriesWithMask 0x%x 0x%x found" ), aName, aMask ) );
            aResultsArray->Append( tableEntry );
            }
        // No need for else, if not found no actions.
        }
    E_TRACE( ( _T( "DNameRecords::FindMatchingNameTableEntriesWithMask 0x%x 0x%x 0x%x<" ), aName, aMask, aResultsArray ) );
    }

void DNameRecords::RemoveUndefinedEntry(
        TNameTableEntry& aNameEntryToRemove
        )
    {
    E_TRACE( ( _T( "DNameRecords::RemoveUndefinedEntry 0x%x>" ), &aNameEntryToRemove ) );
    const TInt index = iNameTableArray.Find( &aNameEntryToRemove );
    ASSERT_RESET_ALWAYS( ( index != KErrNotFound ), ( EISINameRecordsNotFoundEntry | EDISINameRecordsTraceId << KClassIdentifierShift ) );
    delete iNameTableArray[ index ];
    iNameTableArray.Remove( index );
    iNameTableArray.Compress();
    E_TRACE( ( _T( "DNameRecords::RemoveUndefinedEntry 0x%x<" ), &aNameEntryToRemove ) );
    }
