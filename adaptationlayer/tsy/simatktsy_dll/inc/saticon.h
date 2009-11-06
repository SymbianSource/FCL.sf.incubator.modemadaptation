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



#ifndef CSATICON_H
#define CSATICON_H


//  INCLUDES
#include <e32base.h>    // base class cbase
#include <etelsat.h>    // etel sat api
#include <et_tsy.h>     // type declarations for tsy


// FORWARD DECLARATIONS
class CSatMessHandler;
class CTsySatMessaging;

// CLASS DECLARATION
/**
*  CSatIcon 
*  Icon data handling
*  @lib SIMATKTSY.LIB
*  @since Series 60 Series60_2.6
*/
class CSatIcon : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        static CSatIcon* NewL( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );
        
        /**
        * Destructor.
        * @since NCP 5.0
        */
        virtual ~CSatIcon();
        
        
    private:    // Constructors

        /**
        * C++ default constructor.
        * @since NCP 5.0
        * @param aSatMessHandler pointer to satmessagehandler class
        * @param aSatMessaging pointer to satmessaging class
        */
        CSatIcon( CSatMessHandler* aSatMessHandler,
            CTsySatMessaging* aSatMessaging );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @since NCP 5.0
        */
        void ConstructL();        
    

    public: // New functions

        /**
        * Get Icon
        * @since Series 60 Series60_2.6
        * @param TUint8* aRecordNumber: record number
        * @param RSat::TIcon* aIconEf: Icon elementary file
        * @param TTsyReqHandle aTsyReqHandle: Request handle
        * @return TInt: phonet sender statuksen
        */
        TInt GetIcon( TUint8* aRecordNumber, 
            RSat::TIcon* aIconEf, TTsyReqHandle aTsyReqHandle );

        /**
        * Get Image instance
        * @since Series 60 Series60_2.6
        * @param TDesC8* aInstanceInfo: Instance info
        * @param TDes8* aInstance: instance
        * @param TTsyReqHandle aTsyReqHandle: Request handle 
        * @return TInt: phonet sender statuksen
        */
        TInt GetImageInstance( TDesC8* aInstanceInfo, 
            TDes8* aInstance, TTsyReqHandle aTsyReqHandle );

        /**
        * Get Clut
        * @since Series 60 Series60_2.6
        * @param RSat::TInstanceNumberAndOffset* aNumberAndOffset:  
                            Instance number and offset
        * @param TDes8* aClut: Colour look-up table
        * @param TTsyReqHandle aTsyReqHandle 
        * @return TInt: phonet sender statuksen
        */
        TInt GetClut( RSat::TInstanceNumberAndOffset* aNumberAndOffset,
            TDes8* aClut, TTsyReqHandle aTsyReqHandle );  
        
        /**
        * Complete Icon request
        * @since Series 60 Series60_2.6
        * @param TInt aStatus: error status
        * @param TPtrC8 aIcon Data read from ef file
        * @param TDesC8& aInfo Info block of icon
        * @return none
        */
        void CompleteIconRequest( TInt aStatus, TPtrC8 aIcon, 
            const TDesC8& aInfo );
        
    private: // data
        
        // Pointer to message handling class
        CSatMessHandler*    iSatMessHandler;
        
        // Pointer to messaging class
        CTsySatMessaging*   iSatMessaging;
        
        // Icon data
        TDes8*              iIconData;
        
        // Request handle
        TTsyReqHandle       iIconTsyReqHandle;
        
        // flag for GetIcon request 
        TBool               iGetIconFlag;
        
        // Record number from where to read from sim card
        TUint8              iRecordNumber;
        
        // Number of instance in record
        TInt                iImageInstanceNoInRecord;
        
        // Record data from sim card
        TBuf8<255>          iRecordData;

    };

#endif // SATDATADDL_H


// End of file
