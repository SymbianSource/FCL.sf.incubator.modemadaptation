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




#ifndef     __CMMCUSTOMEXTPTRH_H__
#define     __CMMCUSTOMEXTPTRH_H__

//  INCLUDES
#include <etelmm.h>
#include <et_clsvr.h>
#include <etelpckt.h>
#include "rmmcustomapiext.h"
#include <ctsy/rmmcustomapi.h>

// CONSTANTS
	//None

//MACROS
    //None

// DATA TYPES
	//None

// FUNCTION PROTOTYPES
    //None

// FORWARD DECLARATIONS
	//None

// CLASS DECLARATION

/**
*  CMmCustomExtPtrHolder contains Custom Pointer Holder related functionality.
*
*  @lib CustomApiExt.lib
*  @since 2.0
*/
NONSHARABLE_CLASS(CMmCustomExtPtrHolder) : public CBase
    {
    public: // Constructors and destructor
    
        /**
        * Two-phased constructor.
        */
        static CMmCustomExtPtrHolder* NewL( const TInt aSizeOfPtrArray,
                                            const TInt aSizeOfPtrCArray=0 );
        /**
        * Destructor.
        */
        ~CMmCustomExtPtrHolder();

        template <typename T> inline TPtr8& Set( TInt aSlot, T& aObject )
            {
            TPtr8& ptr=Ptr( aSlot );
            ptr.Set( REINTERPRET_CAST( TText8*, ( &aObject ) ), sizeof( T ),
                sizeof( T ) );
            return ptr;
            };

        template <typename T> inline TPtrC8& SetC( TInt aSlot, const T& aObject)
            {
            TPtrC8& ptr=PtrC( aSlot );
            ptr.Set( REINTERPRET_CAST( const TText8*, ( &aObject ) ), 
                sizeof( T ) );
            return ptr;
            };

    protected:

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TInt aSizeOfPtrArray, 
            const TInt aSizeOfPtrCArray );
        
        /**
        * C++ default constructor.
        */
        CMmCustomExtPtrHolder();

    public: // Data


    protected: // Data
        RArray<TPtr8> iPtrArray;
        RArray<TPtrC8> iPtrCArray;

    private: // Data
        TPtr8& Ptr( TInt aIndex );
        TPtrC8& PtrC( TInt aIndex );

    };

#endif // __CMMCUSTOMEXTPTRH_H__


//End of File
