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


#ifndef __NAMERECORDS_H__
#define __NAMERECORDS_H__

#include <e32def.h>                 // For TUint16
#include <e32cmn.h>                 // For TDesC8
#include <klib.h>                   // For DBase

NONSHARABLE_CLASS ( TNameTable )
{
public:
	TNameTable( const TUint32 aName, const TUint16 aPhonetAddress, const TUint8 aFlags )
		:iName(aName),
		iPhonetAddress(aPhonetAddress),
		iFlags(aFlags)
	{}

	TUint32 	iName;
	TUint16 	iPhonetAddress;
	TUint8  	iFlags;
	};

NONSHARABLE_CLASS (DNameRecords) : public DBase
{
public:
	DNameRecords();
	~DNameRecords();

	RArray<TNameTable*> iNameTableArray;


    TUint8 AddName( const TUint32 aName,
                  	const TUint16 aPhonetAddress,
                    const TUint8  aFlags );

    TUint8 RemoveName( const TUint32 aName );

    TInt32 LookupPhonetAddress( const TUint32 aName, TUint16* aPhonetAddress );

    RArray <TNameTable*>* GetNameTable();

    TInt32 NameQuery( const TUint32 aName, const TUint32 aMask, RArray <TNameTable*> *aResultsArray );
};

#endif /* __NAMERECORDS_H__ */
