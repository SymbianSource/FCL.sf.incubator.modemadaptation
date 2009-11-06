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
* Description:   Icon testing class for SimAtkTsy testtool. Provides 
*                functionality to load icon data and verify it's content.
*
*/



#ifndef SATICON_H
#define SATICON_H

// INCLUDES
#include <e32base.h>
#include <etelsat.h>

// FORWARDS
class CStifLogger;
class CSatFile;

// CONSTANTS
_LIT( KLoadIcon, "LoadIcon" );


// CLASS DECLARATION

/**
* Icon testing class for SimAtkTsy testtool.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatIcon ) : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Constructor.
        * @param aSat Reseference to Etel Sat
        * @param aType Type of SIM. Used when verifying icon data.
        * @param aRecordId Record/IconId that shall be used in tests.
        */     
        CSatIcon( const RSat& aSat, const TPtrC aType, const TUint8 aRecordId );

        /**
        * Destructor.
        */     
        virtual ~CSatIcon();


    public:  // New Functions
    
        /**
        * Calling of this method start the icon loading and verification 
        * procedure.
        * @param aLog Reference to logger
        * @param aImageNumber Parameter decides which icon to load if a record
        * contain more than one icon description
        * @return KErrNone if verification went successfully or other system
        * wided error code if some error was encountered.
        */        
        TInt CheckIcon( CStifLogger* aLog, const TUint8 aImageNumber = 1 );

    private:    // New functions
    
        /**
        * Opens SIM reference data file for verification purposes.
        * @return KErrNone if file was opened succesfully, KErrNotFound
        * if not.
        */
        TInt OpenFile();
        
        /**
        * Reads a record from SIM.
        * @param aRecord Output for received record data
        * @return Error code returned by TSY
        */
        TInt ReadRecord( RSat::TIcon& aRecord );
        
        /**
        * Verifies the validity of received record against data read from 
        * reference SIM file.
        * @param aLog Reference to logger
        * @param aRecord Reference to record data to be verified
        * @return KErrNone if validation was successfull, KErrCorrupt if
        * not and KErrNotFound if reference data was not found.
        */
        TInt CheckRecord( CStifLogger* aLog, const RSat::TIcon& aRecord );
        
        /**
        * Method parses InstanceInfo structure from received record data.
        * @param aRecord Reference to record data
        * @param aImageNumber Decides which image is selected of icons 
        * described by the record.
        * @return KErrNone if Instance info was parsed successfully.
        * KErrNotFound if requested image was not found.
        */        
        TInt ReadInstanceInfo( const RSat::TIcon& aRecord, const TUint8 aImageNumber );
        
        /**
        * Checks if the parsed InstanceInfo contain reasonable data.
        * @param aLog Reference to logger
        * @return KErrNone if validation was successful, KErrCorrupt if not.
        */
        TInt CheckInstanceInfo( CStifLogger* aLog );
        
        /**
        * Reads image data from SIM.
        * @param aImageData Output for received image data
        * @return Error code returned by TSY
        */
        TInt ReadImage( TDes8& aImageData );
        
        /**
        * Verifies the validity of received image data against data read from
        * reference SIM file.
        * @param aLog Reference to logger
        * @param aImageData Reference to image data to be verified
        * @return KErrNone if validation was successfull, KErrCorrupt if
        * not and KErrNotFound if reference data was not found.
        */
        TInt CheckImage( CStifLogger* aLog, const TDes8& aImageData );
        
        /**
        * Reads clut information data from reference SIM data, as TSY does
        * not provide all necessary data. Needed for color icons.
        */
        TInt ReadClutInfo();
        
        /**
        * Reads clut data from SIM.
        * @param aClutData Output for received clut data
        * @return Error code returned by TSY
        */
        TInt ReadClut( TDes8& aClutData );
        
        /**
        * Verifies the validity of received clut data against data read from
        * reference SIM file.
        * @param aLog Reference to logger
        * @param aClutData Reference to clut data to be verified
        * @return KErrNone if validation was successfull, KErrCorrupt if
        * not and KErrNotFound if reference data was not found.
        */
        TInt CheckClut( CStifLogger* aLog, const TDes8& aClutData );
        

    private:    // Data
    
        // Structure contain data needed when reading clut information    
        struct TClutInfo
            {
            TUint iOffset;  // Start offset of clut data whitin image file
            TUint iLength;  // Length of clut data
            };
    
        // Reference to SAT
        const RSat&             iSat;
       
        // Type of SIM used as reference
        const TPtrC             iType;

        // Record identifier of icon to be loaded and verified
        const TUint8            iRecordId;

        // SIM data file
        CSatFile*               iFile;
        
        // Icon instance info 
        RSat::TInstanceInfoV3   iInfo;
        
        // Clut information element
        TClutInfo               iClutInfo;
    };

#endif      // SATICON_H
            
// End of 
