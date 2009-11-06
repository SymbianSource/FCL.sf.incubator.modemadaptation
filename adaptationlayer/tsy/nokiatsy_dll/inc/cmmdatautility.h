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



#ifndef CMMDATAUTILITY_H
#define CMMDATAUTILITY_H

// INCLUDES
#include <etelmm.h>
#include "nokia_csd_defs.h"

// CONSTANTS  
// Min length of response from modem
const TInt KMinModemRespLength = 2;

// Carriage Return
const TInt KCarriageReturn = 13;

// Line Feed
const TInt KLineFeed = 10;

// MACROS  
    // none

// DATA TYPES  
    // none

// EXTERNAL DATA STRUCTURES  
    // none

// FUNCTION PROTOTYPES  
    // none

// FORWARD DECLARATIONS
class CMmDataPortHandler;

/**
* Dataport writer Active object for writing AT commands to modem
*/
class CDataPortWriter : public CActive
    {
    
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param CMmDataPortHandler* aMmDataPortHandler: pointer to the dataport handler
        * @param RComm& aDataPort: Reference to the dataport
        * @return CDataPortWriter*: pointer to the dataport writer
        */
        static CDataPortWriter* NewL(
            CMmDataPortHandler* aMmDataPortHandler, 
            const RComm& aDataPort );

        /**
        * Destructor.
        */
        ~CDataPortWriter();

    public: // New functions

        /**
        * Wakes DP up with NULL string.
        */
        void Init();

        /**
        * Returns AT command enum buffer for sent AT commands.
        * @return CArrayFixFlat<TUint>* iAtCommandBuffer
        */
        CArrayFixFlat<TUint>* GetAtBuffer();

        /**
        * Writes AT commands into dataport.
        * @param TDesC8& aData: AT command
        * @param TAtCommand aCommand: AT command enum
        */
        void Write( const TDesC8& aData, TLastAtCommandSent aCommand );

        /**
        * Writes next AT command.
        */
        void WriteNextL();
    
        /**
        * Blocks writing to dataport.
        */
        void BlockWrite();

    public: // Functions from base classes
        //none

    protected: // New functions        
        //none

    protected: // Functions from base classes
        //none

    private: // New functions

        /**
        * Constructor.
        */
        CDataPortWriter();

        /**
        * Class attributes are created in ConstructL.
        */
        void ConstructL();

        /**
        * Appends AT command enum into iAtCommandBuffer.
        * @param TAtCommand aCommand: AT command enum
        */
        void AppendCommandL( TLastAtCommandSent aCommand );

    private: // Functions from base classes

        /**
        * Activates active object when request is completed.
        */
        void RunL();

        /**
        * When RunL leaves, RunError is called.
        * @param TInt aError: Error from RunL
        * @return TInt: KErrNone
        */
        TInt RunError( TInt aError );

        /**
        * Cancels active object request.
        */
        void DoCancel();

    public: // Data
        // none
    
    protected: // Data
        // none

    private: // Data

        // A pointer to the dataport handler object
        CMmDataPortHandler* iDataPortHandler;
    
        // Is writing continued
        TBool iContinueWrite;
    
        // Is write completed
        TBool iWriteCompleted;
    
        // Dataport handle
        RComm iDataPort;
    
        // FIFO buffer for AT commands
        CArrayPtrSeg<HBufC8>* iWriteBuffer;
    
        // FIFO buffer for AT command enums
        CArrayFixFlat<TUint>* iAtCommandBuffer;
    
        // Internal buffer to write data to DP
        TPtrC8 iDataToDpPtr;
    };

/**
* Dataport reader Active object for reading AT commands and responses from 
* modem
*/
class CDataPortReader : public CActive
    {
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param CMmDataPortHandler* aDataPortHandler: Pointer to the dataport handler
        * @param RComm& aDataPort: Reference to the dataport
        * @param CDataPortWriter* aDataPortWriter: Pointer to the DP writer
        * @return CDataPortReader*: pointer to the dataport reader
        */
        static CDataPortReader* NewL(
            CMmDataPortHandler* aMmDataPortHandler,
            const RComm& aDataPort,
            CDataPortWriter* aDataPortWriter );

        /**
        * Destructor.
        */
        ~CDataPortReader();

    public: // New functions

        /**
        * Reads responses from the modem through dataport.
        */
        void Read();

        /**
        * Reads all data available from the modem through dataport.
        */
        void ReadAll();

        /**
        * Cancels Read request from the modem through dataport.
        */
        void ReadCancel();

        /**
        * Modem initialisation completed.
        */
        void ModemInitComplete();

    protected: // New functions
        //none

    protected: // Functions from base classes
        //none

    private: // New functions

        /**
        * Constructor.
        */
        CDataPortReader();

    private: // Functions from base classes

        /**
        * Activates active object when request is completed.
        */
        void RunL();

        /**
        * When RunL leaves, RunError is called.
        * @param TInt aError: Error from RunL
        * @return TInt: KErrNone
        */
        TInt RunError( TInt aError );

        /**
        * Cancels active object request.
        */
        void DoCancel();

    public: // Data
        // none
    
    protected: // Data
        // none

    private: // Data

        // Response from the modem
        TBuf8<KDataBufLength> iResp;
    
        // Is modem initialisation complete
        TBool iModemInitComplete;
    
        // Is reading continued
        TBool iContinueRead;

        // A pointer to the dataport handler object
        CMmDataPortHandler* iDataPortHandler;

        // Dataport handle
        RComm iDataPort;
    
        // Dataport writer
        CDataPortWriter* iDataPortWriter;
    
        // Used when DP reader should empty RX buffer before new call is made
        TBool iReaAllComplete;

    public: // Friend classes
        // none
        
    protected: // Friend classes
        // none
        
    private: // Friend classes
        // none
                
    };

#endif // CMMDATAUTILITY_H

// End of File
