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



#ifndef     __RMMCUSTOMAPIEXT_H__
#define     __RMMCUSTOMAPIEXT_H__

//  INCLUDES
#include <etelmm.h>
#include <et_clsvr.h>
#include <etelpckt.h>
#include <ctsy/rmmcustomapi.h>

// CONSTANTS

//TMultimodeETelCustomV1Api identifies Custom API's version number.
const TInt KETelCustomExtMultimodeV1 = 10000;
const TUint8 KSimNumberOfCbMsgIds = 15;
const TUint KUnusedCbMsgId = 0xffff;

//MACROS
    //None

// DATA TYPES

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
class CMmCustomExtPtrHolder;


// CLASS DECLARATION


/**
*  RMmCustomAPI contains multimode API's vendor specific functionality.
*  RMmCustomAPI is a subsession extension i.e. it derives 
*  from RtelSubSessionBase class.

*  Notice that Custom TSY is able to handle only one instance of each Custom API
*  Set-method call at a time. Overlapping Set-method calls occurs if same 
*  Set-method is called from an other instance of  RMmCustomAPI object. Through
*  one RMmCustomAPI object overlapping calls are not allowed - trying this will
*  panic Etel Server. For each overlapping request to the client is returned 
*  error code KErrServerBusy either as a return value in case of synchronous
*  call or in a variable of type TRequestStatus in case of an asynchronous call.
*  This rule applies also in a situation where previous identical request has 
*  been cancelled by client but the triggered operation on DOS side has not been 
*  completed yet.
*
*  On the other hand Custom TSY is able to handle several instances of each
*  Get-method call at a time. Again overlapping Get-method call can be activated
*  only from an other instance of  RMmCustomAPI object.
*
*  Consider carefully consequences and implications of request cancellations.
*  If a request has triggered an operation on DOS side, the subsequent
*  calcellation of the request will not undo the operation on DOS side. It is
*  the responsibility of the Custom API's client to undo the operation if
*  necessary. Cancel request completes only when awaited DOS side response
*  arrives. Then if it is an OK-response, the request is completed normally with
*  error code KErrNone. . Or if the response is a FAILURE, the request is
*  completed with error code KErrCancel. 
*
*
*  Call realted functionality:
*  Custom TSY implements functionality that is not supported by the MM ETEL API.
*
*  @lib CustomApiExt.lib
*  @since 2.0
*/
class RMmCustomApiExt : public RMmCustomAPI
    {

public:
    
    public:

        /**
        *   C++ default constructor.
        */
        IMPORT_C RMmCustomApiExt();

    public: // New functions

        /**
        * Opens a subsession.
        * @param RMobilePhone& aPhone: Mobile Phone object
        * @return TInt: KErrNone or error code
        */
        IMPORT_C TInt Open(RMobilePhone& aPhone);

        /**
        * Closes the subsession.
        * @param None
        * @return None
        */
        IMPORT_C void Close();

    private:

        /**
        * Copy constructor
        * @param const RMmCustomApiExt& aCustomAPI: Custom API handle
        * @return None
        */
        RMmCustomApiExt( const RMmCustomApiExt& aCustomAPI );

    protected:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        IMPORT_C void ConstructL();

        /**
        * Destructor
        */
        IMPORT_C void Destruct();

    private:    // Data
      
        // Pointer to he Custom pointer holder
        CMmCustomExtPtrHolder*  iCustomExtPtrHolder;
    };


#endif // __RMMCUSTOMAPIEXT_H__

//End of File
