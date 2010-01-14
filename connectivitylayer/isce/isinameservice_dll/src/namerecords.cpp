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


#include "namerecords.h"
#include <nsisi.h>
#include "isinameservicetrace.h"  // For C_TRACE, ASSERT_RESET.. and fault codes

enum TISINameRecordsFaults
    {
    EISINameRecordsMemAllocFailure = 0x01,
    };

DNameRecords::DNameRecords()
	{
	//nothing
	C_TRACE( ( _T( "DNameRecords::DNameRecords <->" ) ) );
	}

DNameRecords::~DNameRecords()
	{
	C_TRACE( ( _T( "DNameRecords::~DNameRecords ->" ) ) );
	TInt i = 0;
    while( i<iNameTableArray.Count())
		{
    	delete iNameTableArray[i];
    	iNameTableArray.Remove(i);
		}
	iNameTableArray.Reset();
	}

TUint8 DNameRecords::AddName( const TUint32 aName,
    						  const TUint16 aPhonetAddress,
    						  const TUint8  aFlags )
    {
    C_TRACE( ( _T( "DNameRecords::AddName name:0x%x phonetAdd:%d flags: %d ->" ), aName, aPhonetAddress, aFlags ) );

	TUint8 error = PN_NAME_OK ;
    TUint i = 0;
    TBool nameFound = EFalse;

    if( aFlags == PN_NAME_UNDEF ||
    	aFlags == PN_NAME_NOCHG  )
        {
        while( i<iNameTableArray.Count())
        	{
            if( iNameTableArray[i]->iName == aName)
                {
                nameFound = ETrue;
                if(  iNameTableArray[i]->iFlags != PN_NAME_UNDEF )
                    {
                    error = PN_NAME_NOT_ALLOWED;
                    }
                else
                    {
                    iNameTableArray[i]->iPhonetAddress = aPhonetAddress;
                    iNameTableArray[i]->iFlags = aFlags;
                    }
                break;
                }
            i++;
            }//while
        }
    else
    	{
    	error = PN_NAME_UNKNOWN;
    	}
    if( !nameFound )
        {
    	TNameTable* table = new TNameTable( aName, aPhonetAddress, aFlags );

        ASSERT_RESET_ALWAYS( table, EISINameRecordsMemAllocFailure | EDISINameRecordsTraceId << KClassIdentifierShift );
    	iNameTableArray.Append( table );

        }
    return error;
    }

TUint8 DNameRecords::RemoveName( const TUint32 aName )
	{
	C_TRACE( ( _T( "DNameRecords::RemoveName name:0x%x  ->" ), aName ) );
    TUint8 error = PN_NAME_UNKNOWN;
    TInt i = 0;
    while( i<iNameTableArray.Count() )
        {
        if( iNameTableArray[i]->iName == aName)
            {
            error = PN_NAME_NOT_ALLOWED;
            if( iNameTableArray[i]->iFlags == PN_NAME_UNDEF )
                {
            	delete iNameTableArray[i];;
            	iNameTableArray.Remove(i);
            	iNameTableArray.Compress();
                error = PN_NAME_OK;
                }
            break;
            }
        i++;
        } //while

    return error;
    }

TInt32 DNameRecords::LookupPhonetAddress( const TUint32 aName, TUint16* aPhonetAddress )
	{
	C_TRACE( ( _T( "DNameRecords::LookupPhonetAddress name:0x%x  ->" ), aName ) );
	TInt32 error = KErrNotFound;
    TInt i = 0;

    while( i<iNameTableArray.Count() )
        {
        C_TRACE( ( _T( "DNameRecords::LookupPhonetAddress ind:0x%x " ), i ) );
        if( iNameTableArray[i]->iName == aName)
            {
            C_TRACE( ( _T( "DNameRecords::LookupPhonetAddress found phonet address: 0x%x" ), iNameTableArray[i]->iPhonetAddress ) );
            *aPhonetAddress = iNameTableArray[i]->iPhonetAddress;
            error = KErrNone;
            break;
            }
        i++;
        }//while

    return error;
    }

RArray <TNameTable*>* DNameRecords::GetNameTable()
    {
	C_TRACE( ( _T( "DNameRecords::GetNameTable ->" ) ) );
	return &iNameTableArray ;
    }

TInt32 DNameRecords::NameQuery( const TUint32 aName, const TUint32 aMask, RArray <TNameTable*> *aResultsArray )
	{
	C_TRACE( ( _T( "DNameRecords::AddName name:0x%x mask:%d &aResultsArray: 0x%x ->" ), aName, aMask, &aResultsArray ) );

    TInt i = 0;
    TInt32 count = 0;//KErrNotFound
    while( i<iNameTableArray.Count() )
        {
        if( aName == iNameTableArray[i]->iName ||
        	(aName & aMask ) == ( iNameTableArray[i]->iName & aMask ) )
            {
            aResultsArray->Append( iNameTableArray[i] );
            count++;
            }
        i++;
        }//while
    return count;
	}

