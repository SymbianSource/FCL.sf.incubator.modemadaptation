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


#ifndef __NAMERECORDS_H__
#define __NAMERECORDS_H__

#include <e32def.h>                 // For TUint16
#include <e32cmn.h>                 // For TDesC8
#include <klib.h>                   // For DBase

NONSHARABLE_CLASS( TNameTableEntry )
    {
    public:

        TNameTableEntry( const TUint32 aName, const TUint16 aPhonetAddress, const TUint8 aFlags )
                    :iName(aName),
                     iPhonetAddress(aPhonetAddress),
                     iFlags(aFlags)
                     {}

        TUint32     iName;
        TUint16     iPhonetAddress;
        TUint8      iFlags;

    };

NONSHARABLE_CLASS( DNameRecords ) : public DBase
    {

    public:

        /*
        * Assumption each function executed from single thread at the time.
        * Constructor
        */
        DNameRecords();

        /*
        * Assumption each function executed from single thread at the time.
        * Destructor
        */
        ~DNameRecords();

        /*
        * Assumption each function executed from single thread at the time.
        * @param aName, name to be added
        * @param aPhonetAddress, PhoNet address of the name to be added
        * @param aFlags, flags of the name to be added
        * @return PN_NAME_UNKNOWN if flags value invalid, PN_NAME_NOT_ALLOWED if not changeable name
        */
        TUint8 AddName( const TUint32 aName,
                        const TUint16 aPhonetAddress,
                        const TUint8  aFlags );

        /*
        * Assumption each function executed from single thread at the time.
        * @param aName
        * @param aPhonetAddress, modified to contain names PhoNet address if name is found
        * @return KErrNone if found KErrNotFound if not
        */
        TInt LookupPhonetAddress( const TUint32 aName, TUint16* aPhonetAddress );

        /*
        * Assumption each function executed from single thread at the time.
        * @param aName
        * @param aPhonetAddress, modified to contain names PhoNet address if name is found
        * @param aFlags, modified to contain names flag if name is found
        * @return KErrNone if found KErrNotFound if not
        */
        TInt LookupNamePhonetAddressAndFlags( const TUint32 aName, TUint16* aPhonetAddress, TUint8* aFlags );

        /*
        * Assumption each function executed from single thread at the time.
        * @param aName
        * @param aMask, mask for matching names
        * @param aResultsArray, array where matches are appended
        */
        void NameQuery( const TUint32 aName, const TUint32 aMask, RArray <TNameTableEntry*> *aResultsArray );

        /*
        * Assumption each function executed from single thread at the time.
        * @param aName
        * @return PN_NAME_UNKNOWN if name not found, PN_NAME_NOT_ALLOWED if not allowed to remove name
        */
        TUint8 RemoveName( const TUint32 aName );

    private:

        /*
        * Adds new name to name table.
        * @param aName
        * @param aPhonetAddress
        * @param aFlags
        * @return PN_NAME_NOT_ALLOWED if name not found or it's address was equal to own device router, otherwise PN_NAME_OK
        */
        TUint8 AddNewName( const TUint32 aName,
                           const TUint16 aPhonetAddress,
                           const TUint8 aFlags );

        /*
        * Ownership of the parameter not given.
        * Adds existing name to the name table.
        * @param aNameTableEntry
        * @param aName
        * @param aPhonetAddress
        * @param aFlags
        * @return PN_NAME_NOT_ALLOWED if not changeable PN_NAME_OK otherwise
        */
        TUint8 AddExistinNameToNameTableArray( TNameTableEntry* aNameTableEntry,
                                               const TUint32 aName,
                                               const TUint16 aPhonetAddress,
                                               const TUint8 aFlags );

        /*
        * Adds and creates new name table entry
        * @param aName
        * @param aPhonetAddress
        * @param aFlags
        * @return PN_NAME_NOT_ALLOWED if router object tried to add PN_NAME_OK otherwise
        */
        TUint8 AddNewNameToNameTableArray( const TUint32 aName,
                                           const TUint16 aPhonetAddress,
                                           const TUint8 aFlags );

        /*
        * Ownership of the return value not given.
        * @param aName
        * @return pointer to table entry, NULL if not found
        */
        TNameTableEntry* FindMatchingNameTableEntry( const TUint32 aName );

        /*
        * Ownership of the array is not given.
        * @param aName
        * @param aMask, mask to find name table entries
        * @aResultsArray, array where found name table entries are added
        */
        void FindMatchingNameTableEntriesWithMask( const TUint32 aName,
                                                   const TUint32 aMask,
                                                   RArray <TNameTableEntry*>* aResultsArray );

        /*
        * Remove entry given as paramater.
        * @param aNameEntryToRemove
        */
        void RemoveUndefinedEntry( TNameTableEntry& aNameEntryToRemove );

        // Name table array, owned.
        RArray<TNameTableEntry*> iNameTableArray;

    };

#endif /* __NAMERECORDS_H__ */
