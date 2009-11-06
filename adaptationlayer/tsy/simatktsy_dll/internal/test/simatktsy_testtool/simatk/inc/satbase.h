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
* Description:   Abstract base class for all SIM ATK TSY tests classes.
*
*/



#ifndef SATBASE_H
#define SATBASE_H

// INCLUDES
#include <e32base.h>
#include <testscripterinternal.h>
#include <etelsat.h>

// FORWARDS
class CStifItemParser;
class CStifLogger;

// DATA TYPES
typedef TInt        TSatIpc;

// CLASS DECLARATION

/**
*  Abstract base class for all SIM ATK TSY tests classes. Provides
*  functionality to verify the received data against scripted values
*  by providing general helper methods for all subclasses.
*  @lib SimAtk.dll
*/
NONSHARABLE_CLASS( CSatBase ) : public CBase
    {
    public:  // Constructors and destructor
    
        /**
        * Constructor.
        */     
        CSatBase( const TSatIpc aIpc, const RSat& aSat, 
            CStifItemParser& aParser, CStifLogger& aLog );

        /**
        * Destructor.
        */     
        virtual ~CSatBase();
        
        
    public:  // Pure virtuals

        /**
        * Pure virtual method. Must be overridden by subclasses.
        * Method verifies the validity of received data by compaing
        * them against scripted parameters. 
        * @return KErrNone if date was valid or KErrCorrupt if received data
        *   and script did not match.
        */
        virtual TInt CheckData() = 0;

        /**
        * Pure virtual method. Must be overridden by subclasses.
        * Method requests notifications from Etel for specific SAT feature
        * and resets the internal data structure.
        * @param aSimAtkStatus Reference to active object's request status
        * @return KErrNone
        */
        virtual TInt NotifySat( TRequestStatus& aSimAtkStatus ) = 0;


    public:  // New Methods
        
        /**
        * Cancels pending Etel request for current command.
        * @return None
        */
        void CancelRequest();
                
        /**
        * Used to identify the Sat feature by IPC. Return appropriate 
        * boolean value.
        * @return ETrue if provide IPC matches with the IPC of the feature.
        *   Otherwise EFalse is returned
        */
        TBool MatchIpc( TSatIpc aRequestedIpc );

    
    protected:  // New Methods

        /**
        * Helper method that verifies the validity of Address
        * structure against scripted parameters.
        * @param aAddress reference to Address structure which shall be verified
        * @return KErrNone if data was valid, KErrCorrupt if received data
        *   and script did not match or KErrNotFound if script did not
        *   provide valid parameters.
        */
        TInt CheckAddress( const RSat::TAddress& aAddress );

        /**
        * Helper method that verifies the validity of AlphaId
        * against scripted parameters.
        * @param aAlphaId reference to AlphaId which shall be verified
        * @param aAlphaIdTag Script tag which shall be used when parsing
        *   alphaId structure from the script. Default value is "AlphaId".
        * @return KErrNone if data was valid, KErrCorrupt if received data
        *   and script did not match or KErrNotFound if script did not
        *   provide valid parameters.
        */
        TInt CheckAlphaId( const RSat::TAlphaId& aAlphaId,
            const TDesC& aAlphaIdTag = KNullDesC );
            
        /**
        * Helper method that verifies the validity of binary
        * data buffer against scripted parameters.
        * @param aTag Tag used to find reference binary data from script
        * @param aData Contains data to be verified  
        * @return KErrNone if data was valid or KErrCorrupt if received data
        * and script did not match. KErrNotFound if reference data was not
        * found from the script.
        */
        TInt CheckBuffer( const TDesC& aTag, const TDesC& aData );

        /**
        * Helper method that verifies the validity of 8-bit binary
        * data buffer against scripted parameters.
        * @param aTag Tag used to find reference binary data from script
        * @param aData Contains data to be verified  
        * @return KErrNone if data was valid or KErrCorrupt if received data
        * and script did not match. KErrNotFound if reference data was not
        * found from the script.
        */
        TInt CheckBuffer( const TDesC& aTag, const TDesC8& aData );
        
        /**
        * Helper method that verifies the validity of Duration
        * against scripted parameters. If script does not provide duration
        * field, method verfifies that TimeUnit field is NoDurationAvailable
        * and NumOfUnit is 0.
        * @param aDuration reference to Duration which shall be verified
        * @return KErrNone if data was valid, KErrCorrupt if received data
        *   and script did not match or KErrNotFound if script did not
        *   provide valid parameters.
        */
        TInt CheckDuration( const RSat::TDuration& aDuration );
                        
        /**
        * Helper method that verifies the validity of Icon
        * against scripted parameters. If script does not provide icon
        * field, method verfifies that Qualifier field is NoIconId
        * and identifier is 0.
        * @param aIconId reference to Icon which shall be verified
        * @param aIconTag Script tag which shall be used when parsing icon
        *   parameters from the script. Default value is "IconId".
        * @return KErrNone if data was valid, KErrCorrupt if received data
        *   and script did not match or KErrNotFound if script did not
        *   provide valid parameters.
        */
        TInt CheckIcon( const RSat::TIconId& aIconId,
            const TDesC& aIconTag = KNullDesC );
            
#ifndef LEGACY
        /**
        * Helper method that verifies the validity of TTextAttribute
        * against scripted parameters.
        * @param aTa reference to TTextAttribute which shall be verified
        * @return KErrNone if data was valid, KErrCorrupt if received data
        *   and script did not match or KErrArgument if script did not
        *   provide valid parameters.
        */
        TInt CheckTextAttribute( const RSat::TTextAttribute& aTa );
#endif // LEGACY

        /**
        * Helper method that verifies the validity of text data against the
        * scripted parameters.
        * @param aTag Tag used to find text data from script
        * @param aData Contains the text to be verified  
        * @return KErrNone if data was valid or KErrCorrupt if received data
        * and script did not match. KErrNotFound if reference data was not
        * found from the script.
        */
        TInt CheckText( const TDesC& aTag, const TDesC& aData );

        /**
        * Helper method that verifies the validity of 8-bit text data against
        * the scripted parameters.
        * @param aTag Tag used to find text data from script
        * @param aData Contains the text to be verified  
        * @return KErrNone if data was valid or KErrCorrupt if received data
        * and script did not match. KErrNotFound if reference data was not
        * found from the script.
        */
        TInt CheckText( const TDesC& aTag, const TDesC8& aData );

        /**
        * Helper method that verifies the validity of provided value against
        * the scripted parameters.
        * @param aTag Tag used to find text data from script
        * @param aValue Contains the value to be verified  
        * @param aDefault When reference value isn't found from the script,
        * verification is made againts the default value when provided.
        * @return KErrNone if value was valid or KErrCorrupt if not.
        * KErrNotFound if reference data was not found from the script and
        * default value  was not supported.
        */
        TInt CheckValue( const TDesC& aTag, const TInt& aValue,
            const TInt aDefault = KMaxTInt );

        /**
        * Helper method that verifies the validity of provided value against
        * next value parsed from the script. Must not be used alone: use
        * CheckValue first to find correct position in the script.
        * @param aValue Contains the value to be verified  
        * @param aDefault When reference value isn't found from the script,
        * verification is made againts the default value when provided.
        * @return KErrNone if value was valid or KErrCorrupt if not.
        * KErrNotFound if reference data was not found from the script and
        * default value  was not supported.
        */
        TInt CheckNextValue( const TInt& aValue,
            const TInt aDefault = KMaxTInt );

        /**
        * Helper method that parses address structure from the script.
        * Structure consist of three parameters, address, type of number and
        * numbering plan. Address is mandatory. If TON and NPI are missing,
        * defaults are used. Address can provided in one of the following
        * formats:
        * Address "address"
        * Address "address" TON
        * Address "address" TON NPI
        * @param aAddress reference to packet which shall be constructed
        * @return KErrNone or KErrNotFound if address was missing
        */
        TInt GetAddress( RSat::TAddress& aAddress );
            
        /*
        * Method reads data from the script with given tag to a buffer.
        * @param aTag Tag used to find data from script
        * @param aBuffer Output where data is read to
        * @return KErrNone if successful or system wide error code if not.
        */
        TInt GetBuffer( const TDesC& aTag, TDes& aBuffer );
        
        /**
        * Helper method reads requested value from the script. Format of
        * scripted number can be given as argument, decimal is the default.
        * @param aTag Tag used to find value from script
        * @param aValue Value read from the script. 
        * @param aRadix Number format of scripted value.
        * @return KErrNone if valid value was found or KErrCorrupt if not.
        * KErrNotFound if value with provided tag was not found.
        */ 
        TInt GetValue( const TDesC& aTag, TInt& aValue,
            TRadix aRadix = EDecimal );

        /**
        * Helper method reads requested value from the script. Templated
        * parameter enables reading typed parameters from the script.
        * Declaration and implementation of template fuctions has to be 
        * in the same complilation unit ( = file )
        * @param aTag Tag used to find value from script
        * @param aValue Value read from the script. 
        * @param aRadix Number format of scripted value.
        * @return KErrNone if valid value was found or KErrCorrupt if not.
        * KErrNotFound if value with provided tag was not found.
        */ 
        template <typename TGenType>
        TInt GetValue( const TDesC& aTag, TGenType& aType,
            TRadix aRadix = EDecimal )
            {
            return GetValue( aTag, reinterpret_cast<TInt&>( aType ), aRadix );
            }
        
        /**
        * Helper method reads next value from the script. Format of
        * scripted number can be given as argument, decimal is the default.
        * Must not be used alone: use GetValue first to find correct position
        * in the script.        
        * @param aValue Value read from the script. 
        * @return KErrNone if valid value was found or KErrCorrupt if not.
        * KErrNotFound if value with provided tag was not found.
        */            
        TInt GetNextValue( TInt& aValue, TRadix aRadix = EDecimal );

        /**
        * Helper method that parses specific text string from the script as 
        * a value. Text can be presented as a quoted text string or by unicode
        * hex values.
        * @param aTag Tag used to find text from script
        * @param aTextOutput Text output
        * @return KErrNone or KErrNotFound no requested text strings wasn't
        *  found. KErrCorrupt if parsed string did not fit to output buffer.
        */
        TInt GetText( const TDesC& aTag, TDes& aText );

        /**
        * Helper method that parses specific text string from the script as 
        * a reference. Text can be presented as a quoted text string or by
        * unicode hex values.
        * @param aTag Tag used to find text from script
        * @param aText Text output
        * @param aGetNext Specifies the parsing method. Used internally.
        * @return KErrNone or KErrNotFound no requested text strings wasn't
        *  found. KErrCorrupt if parsed string did not fit to output buffer.
        */
        TInt GetText( const TDesC& aTag, TPtrC& aText,
            TBool aGetNext = EFalse );

        /**
        * Helper method reads next text string from the script as a reference.
        * Must not be used alone: use GetText first to find correct position
        * in the script.        
        * @param aTag Tag used to find text from script
        * @param aTextOutput Text output
        * @return KErrNone or KErrNotFound no requested text strings wasn't
        *  found. KErrCorrupt if parsed string did not fit to output buffer.
        */
        TInt GetNextText( const TDesC& aTag, TPtrC& aText );
        
        /**
        * Helper method that parses UssdString structure from the script.
        * UssdString can provided in one of the following formats:
        * UssdString "string"
        * UssdSting  "string" DCS
        * @param aUssdString reference to packet which shall be constructed
        * @return KErrNone or KErrNotFound if string was missing        
        */
        TInt GetUssdString( RSat::TUssdString& aUssdString );
        
        /**
        * Template method that is used to initiate data structures with 
        * default values.
        * Declaration and implementation of template fuctions has to be 
        * in the same complilation unit ( = file )
        * @param aType Type to be initialized.
        */
        template <typename TGenType>
        void Init( TGenType& aType )
            {
            aType = TGenType();
            }

            
    private:    // New methods
            
        /**
        * Method converts Hex-tagged integers with in a text string  to
        * hex numbers. Example in string "\x0F" -> value 0Fh in buffer.
        * @param aString String to be converted
        * @return None
        */ 
        void ConvertHexTags( TDes& aString );

        /**
        * Method converts a string presentation of value to a integer.
        * @param aNumberAsString String conteining the number
        * @param aRadix Defines the number format of string. Hex of decimal
        * @param aValue Number output
        * @return KErrNone if successful or system wide error code if not.
        */ 
        TInt ConvertToNumber( TPtrC& aNumberAsString, TRadix aRadix,
            TInt& aValue );
        
            
    protected:  // Data
            
        /**
        * Reference to SAT
        */        
        const RSat&             iSat;
        
        /**
        * Reference to logger
        */
        CStifLogger&            iLog;

            
    private:    // Data
    
        /**
        * IPC of the sat feature is stored here
        */        
        const TSatIpc           iIpc;
        
        /**
        * Reference to config file parser
        */
        CStifItemParser&        iParser;
    };

#endif      // SATBASE_H
            
// End of 
