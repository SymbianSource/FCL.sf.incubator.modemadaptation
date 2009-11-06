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
*     DMC Modem MCE ISI-message utility class.
*
*/


#ifndef DMC_MCE_ISI_H
#define DMC_MCE_ISI_H

#ifndef DMC_INTERNAL_INCLUDE
#error Illegal usage of dmc_mce_isi.h, It is only for DMC internal use !
#endif // DMC_INTERNAL_INCLUDE


// INCLUDES
#include <e32def.h>    // Data types

//  CONSTANTS  
//  MACROS  
//  DATA TYPES 
//  EXTERNAL DATA STRUCTURES
//  FUNCTION PROTOTYPES  
//  FORWARD DECLARATIONS
class DDmcExtension;

NONSHARABLE_CLASS(MceIsi)
    {
    public:

        /**
        * Init modem MCE handler utility class.
        *
        * @param   aDmcExt    A pointer to DMC extension
        * @return  None
        */
        static void Init(DDmcExtension* const aDmcExt);
        
        /**
        * A function that sends a modem state query
        * request to modem MCE server.
        *
        * @param   None
        * @return  None 
        */
        static void MceModemStateQueryReq();

        /**
        * Decodes the resp message and returns a current modem state.
        *
        * @param   aMsgPtr             A pointer to the MCE_MODEM_STATE_QUERY_RESP.
        * @param   aModemCurrentState  A reference to the current modem startup mode variable.
        *                              The data in this field is valid only if the function returns
        *                              KErrNone, which indicates that modem has been started up
        *                              properly.
        * @return                      KErrNone or
        *                              KErrNotReady if the modem state transition is not ready.
        */
        static TInt MceModemStateQueryResp(const TUint8* const aMsgPtr, TUint8& aModemCurrentState);

        /**
        * A function that sends a modem reset request to modem MCE server.
        *
        * @param   None
        * @return  None 
        */
        static void MceResetReq();

        /**
        * Decodes a MCE_POWER_OFF_RESP-message.
        *
        * @param   aMsgPtr    A pointer to the response message.
        * @return  None 
        */
        static void MceResetResp(const TUint8* const aMsgPtr);

        /**
        * A function that sends a modem power off request to modem MCE server.
        *
        * @param   None
        * @return  None 
        */
        static void McePowerOffReq();

        /**
        * Decodes a MCE_POWER_OFF_RESP-message.
        *
        * @param   aMsgPtr    A pointer to the response message.
        * @return  None 
        */
        static void McePowerOffResp(const TUint8* const aMsgPtr);

        /**
        * Decodes a MCE_MODEM_STATE_IND-message.
        *
        * @param   *aMsgPtr       A pointer to the response message.
        * @param   &aModemState   A reference to the current modem state.
        * @param   &aModemAction  A reference to the actions that indicates a phase of the modem-state.
        * @return  None 
        */
        static void MceModemStateInd(const TUint8* const aMsgPtr, TUint8& aModemState, TUint8& aModemAction);

    private: // data types

        /**
        * A pointer to DMC extension.
        * DMC owns this pointer.
        * 
        */
        static DDmcExtension* iDmcExtPtr;
    };

#endif // DMC_MCE_ISI_H

// End of File
