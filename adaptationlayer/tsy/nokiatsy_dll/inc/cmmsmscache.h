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



#ifndef _CMMSMSCACHE_H_
#define _CMMSMSCACHE_H_

//  INCLUDES
#include <e32def.h>
#include <e32base.h>

// CONSTANTS
//none
#define EMPTY_LOCATION 0xFF

// MACROS
//none

// DATA TYPES
//none

// EXTERNAL DATA STRUCTURES
//none

// FUNCTION PROTOTYPES
//none

//  FORWARD DECLARATIONS
class TIsiReceiveC;

//  CLASS DEFINITIONS

// DESCRIPTION
// SIM SMS cache: class declaration
// CMmSmsCache is auxillary class used by CMmSmsMessHandler to implement
// caching of SIM SMS messages. In boot  SMS's are read from SIM and read
// resp's are cached to this cache (except empty slots, where a null ptr is
// put to the array). Basically there are three kinds of operations that can
// modify cache state:
// - read resp, save received isi msg to array (since all writes read SMS
//   this works for them too)
// - delete isi msg from array if delete resp is ok.
// - out of memory/read error (if reading written sms), invalidates cache
// If cache is invalidated there are two ways it can be refilled:
// - CMmSmsMessHandler::GetSmsStoreInfo - initiate refill and wait
// - incoming SMS 2 - initiate refill and fail
//
class CMmSmsCache : public CBase
    {
    // METHODS
    public:
        /**
        * Constructor
        */
        CMmSmsCache();

        /**
        * Destructor
        */
        ~CMmSmsCache();

        /**
        * Reset cache to not ready state
        */
        void Reset();

        /**
        * Get the number of used entries from cache
        * @return TInt number of used entries.
        */
        TInt UsedEntries();

        /**
        * Get the total number of entries in SIM
        * @return TInt total number of entries
        */
        TInt TotalEntries();

        /**
        * Get cache status, this can be KErrNotReady (cache is being read)
        * KErrNone (cache is ok and can be used)
        * KErrNoMemory (cache was delete due to no memory)
        * any other (cache is in error state and is unusable)
        * @return TInt error code
        */
        TInt Status();

        /**
        * Set cache error state
        */
        void SetStatus( TInt aError );

        /**
        * Initialize cache to contain aTotal number of empty entries
        * it is assumed that cache is Reset before call to this..
        * @param aTotal total entries in sim
        */
        void SetTotalEntriesL( TInt aTotal );

        /**
        * Adds new entry to cache, if the cause of isi msg is not
        * ok, then we add just a null pointer (to save space)
        * returns true if element was cache and false
        * if cache location is empty
        * @param aFileData to add to cache
        * @param aRecordId of the data
        * @return TBool ETrue if entry was added empty
        */
        TBool AddEntryL( const RMobileSmsStore::TMobileGsmSmsEntryV1* aEntry,
                         const TUint8  aRecordId );

        /**
        * Get entry from cache, returns null if cache doesn't contain
        * valid entry in the given location location range is in range
        * [1... max sim sms slots]
        * @param aLocation location to get entry from
        * @return TIsiReceiveC* isi msg
        */
        RMobileSmsStore::TMobileGsmSmsEntryV1* GetEntry( TInt aLocation );

        /**
        * Return first free location in range [1... max sim sms slots]
        * or zero if no free slots are found
        * @return TUint location
        */
        TUint FirstFreeLocation();

        /**
        * Set the location to delete by Delete()
        * @param aLocation location
        */
        void SetDeleteLocation( TInt aLocation );

        /**
        * If delete location is set, then delete
        */
        void Delete();

        /**
        * Delete an element from cache
        */
        void Delete( TInt aLocation );

        /**
        * Empty all of cache
        */
        void DeleteAll();

    // ATTRIBUTES
    private:
        // cache error code
        TInt iError;

        // location to delete
        TUint iDeleteLocation;

        // SIM SMS contents, as a SIM_SMS_RESP.SIM_ST_SMS_READ
        // ISI message.
        // null ptr == empty, other == slot contains sms.
        RArray<RMobileSmsStore::TMobileGsmSmsEntryV1*> iElements;
    };

#endif // _CMMSMSCACHE_H_

//End of file
