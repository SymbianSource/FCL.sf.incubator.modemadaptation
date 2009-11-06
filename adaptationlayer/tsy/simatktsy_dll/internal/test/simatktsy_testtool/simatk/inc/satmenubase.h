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
* Description:   Abstact class containing menu-specific helper methods.
*
*/



#ifndef SATMENUBASE_H
#define SATMENUBASE_H

// INCLUDES
#include "satcommandbase.h"

// CONSTANTS
_LIT( KIconListQualifier,       "IconListQualifier" );
_LIT( KPreference,              "Preference" );


/**
*  Class provides helper functions for SetUpMenu and SelectItem
*  proactive commands.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatMenuBase ) : public CSatCommandBase
    {
    
    protected:  // Constructors and destructor
        
        /**
        * Constructor.
        */     
        CSatMenuBase( const TSatIpc aIpc, const RSat& aSat,
            CStifItemParser& aParser,  CStifLogger& aLog,
            const RSat::TPCmd aCommandType );
        
       /**
        * Destructor.
        */
        ~CSatMenuBase();
  
    
    private: // Struct
    
        /**
        * Structure is used to store menu related parameters, which
        * are parsed from the script.        
        */
        struct TScriptedItem
            {
            TPtrC16 iItemString;
            TUint8 iItemId;
            TInt iAction;
            TInt iIconId;
            };
            

    protected: // New functions
    
        /**
        * Helper function that verifies the validi of menu structure
        * common to SetUpMenu and SelectItem proactive commands against
        * scripted parameters. Checked parameters include state of RemoveMenu,
        * Number of Items, Menu Content and all Menu Items. Item Id and text
        * of each menu item is checked, as well as next action and/or icon 
        * id's if available.
        * @param aData Reference to proactive command data structure
        * @return KErrNone if data was valid, KErrCorrupt if received data
        *   and script or default did not match.
        */
        TInt CheckMenuStructure( const RSat::TMenuItemBaseV1& aData );
            
                
    private:    // New methods
    
        /**
        * Internal function that parses an menu item from the
        * script. Number and type of cripted parameters should match 
        * with the provided menu content.
        * @param aMenuContent Content of menu, menu items and actions/iconIds.
        * @param aScriptedItem Output where the parsed values are stored.
        * @return KErrNone if menu item was parsed successfully or KErrCorrupt
        *   if mandatory parameters were missing from the script.
        */
        TInt ParseItem ( const TInt aMenuContent,
            TScriptedItem& aScriptedItem );
    
        /**
        * Internal function that verifies the validity of menu item
        * and associted parameters.
        * @param aScriptedItem Menu item parsed from the script. Menu item
        *   received from the proactive command is compared to it.
        * @param aData Reference to proactive command data structure
        * @param aIndex Index of menu item which is under verification
        * @param aMenuContent Content of menu, menu items and actions/iconIds.
        * @return KErrNone if data was valid, KErrCorrupt if received data
        *   and scripted item did not match and KErrArgument is parsing failed.
        */
        TInt CheckMenuItem ( const TScriptedItem& aScriptedItem,
            const RSat::TMenuItemBaseV1& aData, const TUint aIndex,
            const TInt aMenuContent );
                
    };

#endif      // SATMENUBASE_H
            
// End of File
