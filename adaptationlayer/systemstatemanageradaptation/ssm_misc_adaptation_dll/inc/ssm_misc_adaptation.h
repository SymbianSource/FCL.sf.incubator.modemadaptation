/*
* ==============================================================================
*  Name:           ssm_misc_adaptation.h
*  Part of:        SSMA MiscAdaptation
*  Interface:      N/A
*  Description:    Declaration of CMiscAdaptation class
*  %version:       1 %
*  %date_modified: Tue Dec 29 15:31:47 2009 %
*
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
* Miscadaptation class declarations.
*
*/


#ifndef __SSM_MISC_ADAPTATION_H__
#define __SSM_MISC_ADAPTATION_H__

// INCLUDES
#include <ssm/ssmadaptation.h>

#include "../../../../adaptationlayer/devicemodecontroller/dmc_ext/inc/dmc_user_if.h"

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS


/**
 Collection of different adaptations.
 Implements MMiscAdaptation for different adaptations.

 @internalComponent
 @released

 @see MMiscAdaptation
*/
class CMiscAdaptation : public CBase, public MMiscAdaptation
    {
    public:

        static CMiscAdaptation* NewL();

        //from MMiscAdaptation

        /**
        * Deletes and frees memory allocated.
        */
        void Release();

        /**
        * Request the next security state.
        * @param aState next security state
        * @param aResponsePckg response to state change request on completion
        * @param aStatus to complete when the operation has finished
        */
        void SecurityStateChange( TInt aState, 
                                  TDes8& aResponsePckg, 
                                  TRequestStatus& aStatus );

        /**
        * Get the start-up mode from the Cooperating System.
        * @param aModePckg should contain start-up of the Cooperating System on completion
        * @param aStatus to complete when the operation has finished
        */
        void GetGlobalStartupMode( TDes8& aModePckg, TRequestStatus& aRequest );

        /**
        * Prepare language codes stored in the SIM in preferred language lists. 
        * @param aPriority priority for which the language lists need to be prepared
        * @param aSizePckg on return contains the size of the buffer, which a client 
        *   should allocate and pass as the first parameter (TDes8& aListPckg) 
        *     to GetSimLanguages()
        * @param aStatus to complete when the operation has finished
        */
        void PrepareSimLanguages( TSsmLanguageListPriority aPriority, 
                                  TDes8& aSizePckg, 
                                  TRequestStatus& aStatus );

        /**
        Get language codes stored in the SIM in preferred language lists.
        PrepareSimLanguages() call should precede this call
        @param aBuf should contain the language lists on completion
        @param aCount to contain the count of the languages
        @see PrepareSimLanguages
        */
        void GetSimLanguagesL( CBufBase* aBuf, TInt aCount );
         
        /** 
        * Retrieve information about Hidden reset.
        * @param aHiddenResetPckg Result package (ETrue or EFalse)
        * @param aStatus to complete when the operation has finished
        * @see TRequestStatus
        */
        void GetHiddenReset( TDes8& aHiddenResetPckg, TRequestStatus& aStatus );
              
        /**
        * Cancel the outstanding request
        */
        void Cancel();

    private:
        CMiscAdaptation();
        void ConstructL();  
        ~CMiscAdaptation();

    private:
        // Handle to DMC
        RDmc iDmc;
    };

#endif // __SSM_MISC_ADAPTATION_H__

// End of File

