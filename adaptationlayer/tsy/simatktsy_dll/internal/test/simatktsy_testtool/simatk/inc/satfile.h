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
* Description:   Provides functionality to open SIM data file and
*                to make searches and comperations to its content.
*                Each line in SIM data file represent a separate elementary file 
*                or file's record (referenced as RefEfs).
*
*/



#ifndef SATFILE_H
#define SATFILE_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARDS
// None

// DATA TYPES

// CLASS DECLARATION

/**
*  Provides functionality to interact with SIM data file 
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatFile ) : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Constructor.
        */     
        CSatFile();

        /**
        * Destructor.
        */     
        virtual ~CSatFile();
        
        
    public:     // New Methods
    
        /*
        * Opens SIM data file
        * @return KErrNone if OK, KErrNotFound if file was not found.
        */ 
        TInt OpenFile();
        
        /*
        * Locate reference elementary file (RefEf) in SIM data file
        * @param aSimType Type of RefEf (SATTS/GCF)
        * @param aFileId File to be searched (eg. 4F21)
        * @param aRecord Record to be searched. Default is none.
        * @return KErrNone if RefEf was found from the SIM data
        * file. KErrNotFound if not.
        */        
        TInt FindFile( const TPtrC aSimType, const TInt aFileId, 
            const TInt aRecord = 0 );
        
        /**
        * Match the content to provided data to the RefEf. 
        * Correct SIM file has to be searched first using the FindFile
        * method.
        * @param aData Data to be validated
        * @param aOffset Offset to data start within the RefEf.
        * default is zero (=start validation from the beginging of RefEf.
        * @return KErrNone if data matches, KErrCorrupt if not. KErrOverFlow
        * if provided data contain more bytes that could be parsed from 
        * RefEf
        */
        TInt MatchData( const TDesC8& aData, const TUint aOffset = 0 );
        
        /**
        * Reads data from RefEf to output. Reading continues until the max 
        * capacity of output is reached or more values can't be read from
        * RefEf.
        * Correct SIM file has to be searched first using the FindFile
        * method.
        * @param aData Read data is stored here.
        * @param Offset from where the reading of data shall start.
        * @return KErrNone or KErrNotFound if output buffer could not be 
        * filled completely.
        */        
        TInt FetchData( TDes8& aData, const TUint aOffset );
            
    private:    // New Methods
    
        /**
        * Return next character from current line. Error is returned if end
        * Line has been reached.
        * @param aChar Next character read from line
        * @return KErrNone if successful, KErrNotFound if end of line 
        * encountered.
        */    
        TInt NextChar( TChar& aChar );
        
        /**
        * Skips the current line. Error is return if end of file has been 
        * reached.
        * @return KErrNone or KErrNout found if end of file encountered
        */        
        TInt NextLine();
        
        /**
        * Reads next token (word, number) from the file.
        * @param aToken Token read from the file.
        * @return KErrNone KErrNotFound if end of file reached.
        */
        TInt NextToken( TDes8& aToken );

        /**
        * Reads next integer value from the RefEf.
        * @param aValue Value read from the file.
        * @return KErrNone KErrNotFound if end of line reached.
        */
        TInt NextValue( TUint& aValue );
        
        /**
        * Goes to a specified position in the RefEf file.
        */
        TInt Goto( const TUint aOffset );
        
            
    private:    // Data
    
    // Resource file server
    RFs         iFileServer;
    
    // Resource file
    RFile       iFile;
    
    // Start position of requested RefEf within the SIM data file
    TInt        iSimFilePos;
    
    };

#endif      // SATFILE_H
            
// End of 
