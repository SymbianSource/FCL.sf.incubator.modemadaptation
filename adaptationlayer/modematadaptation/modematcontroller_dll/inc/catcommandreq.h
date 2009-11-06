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



#ifndef CATCOMMANDREQ_H
#define CATCOMMANDREQ_H

#include <e32base.h>

class RModemAtController;

class CAtCommandReq : public CActive
	{
public:
	CAtCommandReq( RModemAtController* aClient, const TDesC8& aCommand, TDes8& aResponse );
	~CAtCommandReq();
    
protected:
    // from CActive
    void RunL();
    void DoCancel();
	
private:
    //data
    RModemAtController* iClient;
    };

#endif  //CATCOMMANDREQ_H

