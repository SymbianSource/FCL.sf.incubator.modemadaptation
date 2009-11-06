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
* Description:   Abstract base class for all SIM ATK TSY proactive command
*                tests classes.
*
*/



// INCLUDE FILES
#include "satmenubase.h"

// CONSTANTS
_LIT( KRemoveMenu,              "RemoveMenu" );
_LIT( KNumberOfItems,           "NumberOfItems" );
_LIT( KMenuContent,             "MenuContent" );
_LIT( KMenuItem,                "MenuItem" );


// -----------------------------------------------------------------------------
// CSatMenuBase::CSatMenuBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatMenuBase::CSatMenuBase
        (
        const TSatIpc       aIpc,
        const RSat&         aSat,
        CStifItemParser&    aParser,
        CStifLogger&        aLog,        
        const RSat::TPCmd   aCommandType
        ) : CSatCommandBase ( aIpc, aSat, aParser, aLog, aCommandType )
    {
    }

// -----------------------------------------------------------------------------
// CSatMenuBase::~CSatMenuBase
// Destructor
// -----------------------------------------------------------------------------
//
CSatMenuBase::~CSatMenuBase()
    {  
    }
    

// -----------------------------------------------------------------------------
// CSatMenuBase::CheckMenuStructure
// Verifies the validity of menu structure
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatMenuBase::CheckMenuStructure
        (
        const RSat::TMenuItemBaseV1&    aData
        )
    {
    TInt ret( KErrNone );
    
    TBool isRemoveMenu ( aData.IsRemoveMenu() );

    // Try to parse RemoveMenu from script.
    // Default is 0 (False)
    TUint number( 0 );
    GetValue( KRemoveMenu, number );

    // Check if scripted value matches with 'aIsRemoveMenu'
    if ( 0 != number )
        {
        // Script: 'RemoveMenu 1'
        if ( isRemoveMenu == EFalse )
            {
            ret = KErrCorrupt;
            }
        }
    else 
        {
        // RemoveMenu should be False
        if ( isRemoveMenu )
            {
            ret = KErrCorrupt;
            }            
        }

    if ( KErrNone != ret )
        {
        iLog.Log( _L( "CSatMenuBase::CheckMenuStructure: ERROR \
            - IsRemoveMenu did not match!" ) );
        }
    
    if( isRemoveMenu == EFalse )           
        {
        // If not remove menu, check also the other values
        
        // Number of Items
        if ( KErrNone != GetValue( KNumberOfItems, number ) )
            {
            ret = KErrCorrupt;
            }
        else
            {
            // number of items parsed OK.
            TUint numberOfItems( aData.NumberOfItems() );

            if ( number != numberOfItems )
                {
                iLog.Log( _L( "CSatMenuBase::CheckMenuStructure: ERROR \
                    - NumberOfItems does not match!" ) );
                ret = KErrCorrupt;
                }
                
            // Menu content
            number =  RSat::KItems;
            GetValue( KMenuContent, number );
            TInt menuContent ( aData.MenuContent() );
            if ( number != menuContent )
                {
                iLog.Log( _L( "CSatMenuBase::CheckMenuStructure: ERROR\
                    - MenuContent did not match" ) ); 
                ret = KErrCorrupt;
                }
                
            // Check each menuItem
            TScriptedItem scriptedItem;
            for ( TUint i = 1 ; KErrNone == ret && i <= numberOfItems ; i ++ )
                {
                ret = ParseItem( menuContent, scriptedItem );
                
                if ( KErrNone == ret )
                    {
                    ret = CheckMenuItem( scriptedItem, aData, i, menuContent );
                    }

                if ( KErrCorrupt == ret )
                    {
                    iLog.Log( _L( "CSatMenuBase::CheckMenuStructure: ERROR\
                        - MenuItem %d did not match" ), i );
                    }
                else if ( KErrArgument == ret )
                    {
                    iLog.Log( _L( "CSatMenuBase::CheckMenuStructure: ERROR\
                        - Parsing of menuItem %d failed!" ), i );
                    }
                } // for
            } // NumberOfItems-argument missing
        } // isRemoveMenu == EFalse
        
    return ret;
    }
    
// -----------------------------------------------------------------------------
// CSatMenuBase::ParseItem
// Internal function that parses a menu item from he script
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatMenuBase::ParseItem
        (
        const TInt              aMenuContent,
        TScriptedItem&          aScriptedItem
        )
    {
    // Script should contain one of following sequences
    // MenuItem ItemString ItemId
    // OR
    // MenuItem ItemString ItemId NextAction
    // OR
    // MenuItem ItemString ItemId IconId
   // OR
    // MenuItem ItemString ItemId NextAction IconId
    
    TInt ret( KErrNone );
    TInt number ( 0 );
    
    // Clear data
    aScriptedItem.iItemString.Set( KNullDesC );
    aScriptedItem.iItemId = 0;
    aScriptedItem.iAction = 0;
    aScriptedItem.iIconId = 0;
    
    // ItemString
    if ( KErrNone != GetNextText( KMenuItem, aScriptedItem.iItemString ) )
        {
        ret = KErrArgument;
        }
    
    // ItemId
    if ( KErrNone == ret )
        {
        if ( KErrNone != GetNextValue( number, EHex ) )
            {
            ret = KErrArgument;
            }
        else
            {
            aScriptedItem.iItemId = number;
            }
        }
        
    // If everything OK so far AND menu contains NextActions AND/OR IconId's        
    if ( KErrNone == ret && RSat::KItems < aMenuContent )
        {
        // NextAction or IconId
        if ( KErrNone == GetNextValue( number, EHex ) )
            {
            // Third parameter found. Store it depending on the supposed content
            // of the menu.
            switch ( aMenuContent )
                {
                case RSat::KItems + RSat::KItemNextActions:
                    {
                    aScriptedItem.iAction = number;
                    break;
                    }
                case RSat::KItems + RSat::KItemIconIds:
                    {
                    aScriptedItem.iIconId = number;
                    break;
                    }
                default:
                    {
                    // Menu contain both nextActions AND iconId's. 
                    aScriptedItem.iAction = number;
                    
                    // Script should provide fouth parameter as well
                    if ( KErrNone == GetNextValue( number, EHex ) )
                        {
                        aScriptedItem.iIconId = number;
                        }
                    else
                        {
                        // iconId missing from the script
                        ret = KErrArgument;
                        }         
                    break;
                    }
                } // Switch
            } // GetNextInt for third parameter
        else
            {
            // Script does not provide enough menu content information
            // Third parameter nextAction or iconId is missing
            ret = KErrArgument;
            }
        }
    
    return ret;
    }
    

// -----------------------------------------------------------------------------
// CSatMenuBase::CheckMenuItem
// Internal function that verifies the validity of menu item against 
// the scripted values.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSatMenuBase::CheckMenuItem
        (
        const TScriptedItem&            aScriptedItem,
        const RSat::TMenuItemBaseV1&    aData,
        const TUint                     aIndex,
        const TInt                      aMenuContent
        )
    {
    TInt ret ( KErrNone );
    
    RSat::TItem menuItem;
    TInt nextAction ( 0 );
    TInt iconId ( 0 );
    
    // Get Item and next action & iconId if present
    switch ( aMenuContent )
        {
        case RSat::KItems: 
            {
            // aMenuContent = 1
            ret = aData.GetItem( aIndex, menuItem );
            break;
            }
        case RSat::KItems + RSat::KItemNextActions:
            {
            // aMenuContent = 1 + 2
            ret = aData.GetItem( aIndex, menuItem, nextAction );
            break;
            }
        case RSat::KItems + RSat::KItemIconIds:
            {
            // aMenuContent = 1 + 4
            ret = aData.GetItemIcon( aIndex, menuItem, iconId );
            break;
            }
        case RSat::KItems + RSat::KItemNextActions + RSat::KItemIconIds:
            {
            // aMenuContent = 1 + 2 + 4
            ret = aData.GetItem( aIndex, menuItem, nextAction, iconId );
            break;
            }
        default:
            {
            // Shouldn't end up here
            ret = KErrCorrupt;            
            }
        }
        
    // Compare values
    if ( ret == KErrNone )
        {
        
        // ItemString
        if ( 0 != menuItem.iItemString.Compare( aScriptedItem.iItemString ) )
            {
            // Unexpected itemString
            ret = KErrCorrupt;
            }
        
        // ItemId
        if ( menuItem.iItemId != aScriptedItem.iItemId )
            {
            // Unexpected itemId
            ret = KErrCorrupt;
            }
            
        // NextAction if available
        if ( RSat::KItemNextActions & aMenuContent )
            {
            if ( nextAction != aScriptedItem.iAction )
                {
                // Unexpected itemId
                ret = KErrCorrupt;
                }
            }
        
        // IconID if available
        if ( RSat::KItemIconIds & aMenuContent )
            {
            if ( iconId != aScriptedItem.iIconId )
                {
                // Unexpected IconId
                ret = KErrCorrupt;
                }
            }
        }
    
    return ret;
    }

    
// End of File
