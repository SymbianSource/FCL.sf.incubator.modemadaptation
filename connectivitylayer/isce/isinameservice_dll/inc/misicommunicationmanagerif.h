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



#ifndef __MISICOMMUNICATIONMANAGERIF_H__
#define __MISICOMMUNICATIONMANAGERIF_H__


/*
* Abstract interface for communication manager to use nameservice.
*/
class MISICommunicationManagerIf
    {

    public:
    
        /*
        * Checks if client is registered as a resource.
        * @return name table status
        */
        IMPORT_C static TBool IsValidResource( const TDesC8& aMessage );

    };

#endif /* __MISICOMMUNICATIONMANAGERIF_H__ */
