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



#ifndef MMODEMATOBSERVER_H
#define MMODEMATOBSERVER_H

#include <e32base.h>
// CLASS DECLARATION

enum TATPluginInterface
    {
    EATExtPlugin = 0,
    ECommonPlugin
    };

enum TCommandMode
    {
    EIgnore = 0,
    EDataMode,
    ECommandMode
    };

/**
*  MAtControllerObserver
*  This class implements the MAtControllerObserver
*/

class MAtControllerObserver 
    {
public:
    virtual void HandleATCommandCompleted( TInt aErr ) = 0;
    virtual void HandleUnsolicitedResultReceived( TInt aErr ) = 0;
    virtual void HandleSignalIndication( TInt aErr ) = 0;
    virtual void HandleCommandModeChanged( TInt aErr, TCommandMode aMode = EIgnore ) = 0;
    };



#endif  // MMODEMATOBSERVER_H

