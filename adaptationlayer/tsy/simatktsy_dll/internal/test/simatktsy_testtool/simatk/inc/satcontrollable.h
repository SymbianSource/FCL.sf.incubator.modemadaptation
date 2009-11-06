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
* Description:   Interface provides an access for call control related classes 
*                to Call Control class, which holds information how the original 
*                what modified by CC.
*
*/



#ifndef SATCONTROLLABLE_H
#define SATCONTROLLABLE_H

// INCLUDES
#include "satcallcontrol.h"

// CLASS DECLARATION

/**
*  Interface for Cc affected classes for accessing RequestedAction info held by
*  CSatCallControl.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( MSatControllable )
    {
    public: // Constructor
    
        /**
        * Constructor.
        */     
        MSatControllable() : iCallControl( NULL ) {}
            
            
    public: // New methods
    
        /**
        * Stores a pointer to Call Control class. Pointer's pointer has to be
        * used as pointer in the calling side may still be uninitialized.
        * (when CSatCallControl is created after the proactive command class,
        * for example).
        * @param aCallControl Pointer to a pointer, which points to 
        *   CSatCallControl once created.
        */
        void SetCallControl( CSatBase** aCallControl )
            {
            iCallControl = reinterpret_cast<CSatCallControl**> ( aCallControl );
            }
            

    protected: // New methods
    
        /**
        * Getter for Call Control requested actiona, if any. Default structure 
        * is returned if Sat Call Control class is not created.
        * @return TCallControlV1 structure containing either default values, or
        *   CC requested action when Simtk has Completed Cc notify
        */
        RSat::TCallControlV1 RequestedAction() 
            {
            RSat::TCallControlV1 cc;
            if ( *iCallControl )
                {
                cc = (*iCallControl)->iData;
                }
                
            return cc;
            }

    private:    // Member data
    
        // Ponter's pointer into SatCallControl instance if it exists    
        CSatCallControl**    iCallControl;

    };

#endif      // SATCONTROLLABLE_H
            
// End of 
