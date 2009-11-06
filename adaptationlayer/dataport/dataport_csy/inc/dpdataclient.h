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



#ifndef DPDATACLIENT_H
#define DPDATACLIENT_H

// INCLUDES
// none

// CONSTANTS
// none

// MACROS
// none

// DATA TYPES
// none

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  CDpDataClient is base class for data clients, which access data buffer.
*  Data clients act either in reader or writer role. CDpDataClient provides
*  also methods for flushing and releasing of data client.
*
*  @lib ?library
*  @since Series ?XX ?SeriesXX_version
*/
class MDpDataClient
    {
    public:
        enum
            {
            EDbDataClientReader,
            EDbDataClientWriter
            };

    public: // New methods

        /**
        * This method notifies that buffer is flushed. Classes which have
        * access to buffers are derived from this class. Derived class could
        * override this method, otherwise this default method will be used.
        */
        virtual void FlushNotify() = 0;

        /**
        * This method notifies data client, when data client can be released.
        * Classes which have access to buffers are derived from this class.
        * Derived class could override this method, otherwise this default
        * method will be used.
        */
        virtual void ReleaseNotify() = 0;

        /**
        * This method gets the role (reader/writer) of data client.
        * @return Role (EDbDataClientReader/EDbDataClientWriter)
        */
        virtual TUint8 Role() = 0;
    
    public: // Methods from base classes

        // none

    protected: // New methods

        // none

    protected: // Methods from base classes

        // none

    private: // Methods

        // none

    public:     // Data
        // none
    
    protected:  // Data
        // none

    private:    // Data
        // none

    };

#endif // DPDATACLIENT_H

// End of File
