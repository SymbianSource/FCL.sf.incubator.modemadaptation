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
*
*/


#ifndef CUSBPNSERVERSESSION_H
#define CUSBPNSERVERSESSION_H

//  INCLUDES
#include <e32base.h>    // For CSession2

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
class CUsbPnServer;

// CLASS DECLARATION


class CUsbPnServerSession : public CSession2
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CUsbPnServerSession( CUsbPnServer& );

        /**
        * Destructor.
        */
        ~CUsbPnServerSession();

    public: // New functions


    public: // Functions from base classes

        /**
        * From CServer2 ServiceL().
        * This function is called by Client/Server framework when a new message
        * is received from client
        * @return none
        */
        void ServiceL( const RMessage2& aMessage );

        /**
        * From CServer2 CreateL().
        * This function is called by Client/Server framework when a new session
        * is created
        * @return none
        */
        void CreateL();

    protected:  // New functions


    protected:  // Functions from base classes


    private:

        // Prohibit copy constructor if not deriving from CBase.
        // CUsbPnServerSession( const CUsbPnServerSession& );
        // Prohibit assigment operator if not deriving from CBase.
        // CUsbPnServerSession& operator=( const CUsbPnServerSession& );

    public:     // Data

    protected:  // Data

    private:    // Data

        // Reference to the System Adaptation server
        CUsbPnServer& iUsbPnServer;


    public:     // Friend classes
    protected:  // Friend classes
    private:    // Friend classes

    };

#endif      // CUSBPNSERVERSESSION_H

// End of File
