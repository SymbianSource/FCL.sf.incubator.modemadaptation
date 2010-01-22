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


#include "tisi.h"
#include "isimessagetrace.h"
#include <pn_const.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "tisiTraces.h"
#endif



//  LOCAL CONSTANTS AND MACROS  

const TInt KMessageHeaderLength ( 6 );
const TUint16 KSixteenBitZero ( 0x0000 );
const TUint32 KThirtytwoBitZero ( 0x00000000 );
const TInt KMinSbLength ( 4 );
const TUint8 KPaddingByte ( 0x00 );

#define IS_DEV_HOST( dev )            ( ( dev & 0xF0 ) == 0 )
#define GET_RECEIVER_DEV( msg )       ( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0xFC )
#define GET_SENDER_DEV( msg )         ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0xFC )
#define GET_RECEIVER_OBJ( msg )       IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] ) ? \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] + ( ( msg [ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0x0F ) << 8 ) ) : \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] + ( ( msg [ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0x03 ) << 8 ) )
#define GET_SENDER_OBJ( msg )         IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] ) ? \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_SENDEROBJECT ] + ( ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0x0F ) << 8 ) ) : \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_SENDEROBJECT ] + ( ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0x03 ) << 8 ) )
#define SET_RECEIVER_DEV( msg, dev )  ( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE] = ( ( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0x03 ) | ( ( dev ) & 0xFC ) ) )
#define SET_SENDER_DEV( msg, dev )    ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE] = ( ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0x03 ) | ( ( dev ) & 0xFC ) ) )
#define SET_RECEIVER_OBJ( msg, o )    msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] = ( IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] ) ) ? \
                                      GET_RECEIVER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x0F ) : GET_RECEIVER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x03 ), \
                                      msg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] = ( TUint8 )( ( o ) & 0x00FF )
#define SET_SENDER_OBJ( msg, o )      msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] = ( IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] ) ) ? \
                                      GET_SENDER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x0F ) : GET_SENDER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x03 ), \
                                      msg[ ISI_HEADER_OFFSET_SENDEROBJECT ] = ( TUint8 )( ( o ) & 0x00FF )
// Panic code literals
_LIT( KInvalidOffset, "IsiMessage: OFFSET Invalid" );
_LIT( KInvalidValue, "IsiMessage: Invalid Value" );
_LIT( KInvalidSubBlockType, "IsiMessage: SBTYPE Invalid");
_LIT( KInvalidResourceId, "IsiMessage: Invalid ExtendedResourceId");

// -----------------------------------------------------------------------------
// TIsi::operator=
// -----------------------------------------------------------------------------
EXPORT_C TIsi& TIsi::operator=
        ( 
        const TIsi&
        )
    {

    C_TRACE( ( _T( "TIsi::operator=" ) ) );
    OstTrace0( TRACE_NORMAL, TISI_OPERATOR, "TIsi::operator=" );

    return ( *this );

    }


// -----------------------------------------------------------------------------
// TIsiReceiveC::FindSubBlockOffsetById
// -----------------------------------------------------------------------------
EXPORT_C TInt TIsiReceiveC::FindSubBlockOffsetById 
        ( 
        const TUint aSbStartOffset,
        const TUint aSubblockId,
        const TSubBlockType aType,
        TUint &aSubBlockOffset
        ) const
    {

    C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetById entering, (sbStart 0x%x, sbId 0x%x, aType %d  )" ), aSbStartOffset, aSubblockId, aType ) );
    OstTraceExt3( TRACE_NORMAL, TISIRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiReceiveC::FindSubBlockOffsetById entering;sbStart=%u;sbId=%u;aType=%x;", aSbStartOffset, aSubblockId, ( TUint )&( aType ));   
    
    TInt finder_offset( aSbStartOffset );
    TInt length( iBuffer.Length() );

    // Message has to be divisible by four, when it has any subblocks
    TRACE_ASSERT ( length %  4 == 0 ); //Invalid length
    TRACE_ASSERT ( finder_offset % 4 == 0 ); //Invalid offset

    TUint subblock_id( 0 );
    TUint subblock_len( 0 );
    TBool found( EFalse );
    TInt retval( KErrNotFound );

    if( ( length - KMinSbLength ) < finder_offset )
        {
            C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetById used with msg which doesn't contains any subblocks, (retval %d)" ), retval ) );
            OstTrace1( TRACE_NORMAL, DUP1_TISIRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiReceiveC::FindSubBlockOffsetById used with msg which doesn't contains any subblocks;retval=%d", retval );
        }
    else
        {        
        do
            {
            switch( aType )
                {
                case EIsiSubBlockTypeId8Len8:
                   {
                  subblock_id = this->Get8bit( finder_offset );
                  subblock_len = this->Get8bit( finder_offset + 1 );
                  break;
                  }
                case EIsiSubBlockTypeId8Len16:
                    {
                    subblock_id = this->Get8bit( finder_offset );
                    subblock_len = this->Get16bit( finder_offset + 2 );
                    break;
                    }
                case EIsiSubBlockTypeId16Len8:
                    {
                    subblock_id = this->Get16bit( finder_offset );
                    subblock_len = this->Get8bit( finder_offset + 2 );
                    break;
                    }
                case EIsiSubBlockTypeId16Len16:
                    {
                    subblock_id = this->Get16bit( finder_offset );
                    subblock_len = this->Get16bit( finder_offset + 2 );
                    break;
                    }
                case EIsiSubBlockTypeId8Len32:
                    {
                    subblock_id = this->Get8bit( finder_offset );
                    subblock_len = this->Get32bit( finder_offset + 4 );
                    break;
                    }
                default:
                    {
                    User::Panic( KInvalidSubBlockType, KErrArgument );
                    break;
                    }
                }

            if ( subblock_len == 0 )
                {
                C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetById has found corrupted data, sb_len = 0 ->stop searching!" ) ) );           
                OstTrace0( TRACE_NORMAL, DUP2_TISIRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiReceiveC::FindSubBlockOffsetById has found corrupted data, sb_len = 0 ->stop searching!" );
                found = ETrue;
                retval = KErrNotFound;
                }

            if ( aSubblockId == subblock_id )
                {
                found = ETrue;
                aSubBlockOffset = finder_offset;
                retval = KErrNone;
                }
            else
                {
                finder_offset+= subblock_len;
                }
            } while ( !found && ( finder_offset + KMinSbLength <= length ) );
        }

    C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetById returning, aSbStartOffset = 0x%x" ), finder_offset ) );
    OstTrace1( TRACE_NORMAL, DUP3_TISIRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiReceiveC::FindSubBlockOffsetById - return found sbStartOffset=%d", finder_offset );   

    return retval;
   
    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::FindSubBlockOffsetByIndex
// -----------------------------------------------------------------------------
EXPORT_C TInt TIsiReceiveC::FindSubBlockOffsetByIndex
        ( 
        const TUint aSbStartOffset,
        const TUint aSubblockIndex,
        const TSubBlockType aType,
        TUint &aSubBlockOffset
        ) const
    { 

    C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetByIndex entering, (sbStart 0x%x, sbIndex 0x%x, aType %d  )" ), aSbStartOffset, aSubblockIndex, aType ) );
    OstTraceExt3( TRACE_NORMAL, TISIRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiReceiveC::FindSubBlockOffsetByIndex;sbStart=%u;subblockIndex=%u;aType=%x;", aSbStartOffset, aSubblockIndex, ( TUint )&( aType ) );

    TInt finder_offset( aSbStartOffset );
    TInt length( iBuffer.Length() );
    
    // Message has to be divisible by four, when it has any subblocks
    TRACE_ASSERT ( length % 4 == 0 ); //Invalid length
    TRACE_ASSERT ( finder_offset % 4 == 0 ); // Invalid offset

    TUint subblock_len ( 0 );
    TUint index ( 0 );
    TBool found ( EFalse );
    TInt retval( KErrNotFound );

    if( ( length - KMinSbLength ) < finder_offset )
        {
        C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetByIndex used with msg which doesn't contains any subblocks, (retval %d)" ), retval ) );     
        OstTrace1( TRACE_NORMAL, DUP1_TISIRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiReceiveC::FindSubBlockOffsetByIndex used with msg which doesn't contains any subblocks;retval=%d", retval );
        }
    else
        {        
        do
            {
            index++;
            switch( aType )
            {
            case EIsiSubBlockTypeId8Len8:
                {
                subblock_len = this->Get8bit( finder_offset + 1 );
                break;
                }
            case EIsiSubBlockTypeId8Len16:
                {
                subblock_len = this->Get16bit( finder_offset + 2 );
                break;
                }
            case EIsiSubBlockTypeId16Len8:
                {
                subblock_len = this->Get8bit( finder_offset + 2 );
                break;
                }
            case EIsiSubBlockTypeId16Len16:
                {
                subblock_len = this->Get16bit( finder_offset + 2 );
                break;
                }
            case EIsiSubBlockTypeId8Len32:
                {
                subblock_len = this->Get32bit( finder_offset + 4 );
                break;
                }
            default:
                {
                User::Panic( KInvalidSubBlockType, KErrArgument );
                break;
                }
            }

        if ( subblock_len == 0 )
            {
            C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetByIndex has found corrupted data, sb_len = 0 ->stop searching" ) ) );           
            OstTrace0( TRACE_NORMAL, DUP2_TISIRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiReceiveC::FindSubBlockOffsetByIndex has found corrupted data, sb_len = 0 ->stop searching!" );
            found = ETrue;
            retval = KErrNotFound;
            }

        if( index == aSubblockIndex )
            {
            found = ETrue;
            aSubBlockOffset = finder_offset;
            retval = KErrNone;
            }
        else
            {
            finder_offset+=subblock_len;
            }
        } while( !found && ( index <= aSubblockIndex ) && (finder_offset + KMinSbLength <= length ) );
    }

    C_TRACE( ( _L ( "TIsiReceiveC::FindSubblockOffsetByIndex returning, aSbStartOffset = 0x%x" ), finder_offset ) );
    OstTrace1( TRACE_NORMAL, DUP3_TISIRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiReceiveC::FindSubBlockOffsetByIndex - return found sbStartOffset=%d", finder_offset );

    return retval;
    
    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::Get16bit
// -----------------------------------------------------------------------------
EXPORT_C TUint16 TIsiReceiveC::Get16bit
        (
        const TUint aOffset 
        ) const
    {

    C_TRACE( ( _L ( "TIsiReceiveC::Get16bit entering, aOffset 0x%x" ), aOffset ) );
    OstTrace1( TRACE_NORMAL, TISIRECEIVEC_GET16BIT, "TIsiReceiveC::Get16bit - enter;aOffset=%u", aOffset );

    ASSERT_RESET_ALWAYS( ((iBuffer.Length() - 1) > aOffset), KInvalidOffset, KErrOverflow );
    TRACE_ASSERT ( aOffset % 2 == 0 ); //Invalid offset

    TUint16 retval( KSixteenBitZero );
    TUint16 byte1( KSixteenBitZero );
    TUint16 byte2( KSixteenBitZero );

    byte1 = iBuffer[ aOffset ];
    byte2 = iBuffer[ aOffset + 1 ];
    retval = TUint16( ( byte1 << 8 ) | byte2 );  
   
    C_TRACE( ( _L ( "TIsiReceiveC::Get16bit - return, return 16bit value = 0x%x" ), retval ) );
   OstTraceExt1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GET16BIT, "TIsiReceiveC::Get16bit - return retval=%hu", retval );
   
    return retval;

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::Get32bit
// -----------------------------------------------------------------------------
EXPORT_C TUint32 TIsiReceiveC::Get32bit
        (
        const TUint aOffset
        ) const
    {

    C_TRACE( ( _L ( "TIsiReceiveC::Get32bit entering, aOffset = 0x%x" ) , aOffset ) );
    OstTrace1( TRACE_NORMAL, TISIRECEIVEC_GET32BIT, "TIsiReceiveC::Get32bit - enter;aOffset=%u", aOffset );
    

    ASSERT_RESET_ALWAYS( ( iBuffer.Length() - 3 > aOffset ), KInvalidOffset, KErrOverflow );
    TRACE_ASSERT ( aOffset % 4 == 0 ); // Invalid offset

    TUint32 retval( KThirtytwoBitZero );
    TUint32 byte1( KThirtytwoBitZero );
    TUint32 byte2( KThirtytwoBitZero );
    TUint32 byte3( KThirtytwoBitZero );
    TUint32 byte4( KThirtytwoBitZero );
    
    byte1 = iBuffer[ aOffset ];
    byte2 = iBuffer[ aOffset + 1 ];
    byte3 = iBuffer[ aOffset + 2 ];
    byte4 = iBuffer[ aOffset + 3 ];

    retval = TUint32( byte1 << 24 );
    retval = TUint32( retval | ( byte2 << 16 ));
    retval = TUint32( retval | ( byte3 << 8 ));
    retval = TUint32( retval | byte4 );

    C_TRACE( ( _L ( "TIsiReceiveC::Get32bit - return, Return 32bit value = 0x%x" ), retval ) );
    OstTrace1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GET32BIT, "TIsiReceiveC::Get32bit - return retval=%d", retval );

    return retval;

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::Get8bit
// -----------------------------------------------------------------------------
EXPORT_C TUint8 TIsiReceiveC::Get8bit
        (
        const TUint aOffset 
        ) const
    {

    C_TRACE( ( _L ( "TIsiReceiveC::Get8bit entering, aOffset = 0x%x" ) , aOffset ) );
    OstTrace1( TRACE_NORMAL, TISIRECEIVEC_GET8BIT, "TIsiReceiveC::Get8bit - enter;aOffset=%u", aOffset );
    

    ASSERT_RESET_ALWAYS( ( iBuffer.Length() > aOffset ), KInvalidOffset, KErrOverflow );

    C_TRACE( ( _L ( "TIsiReceiveC::Get8bit - return, return value = 0x%x" ) , iBuffer[aOffset] ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GET8BIT, "TIsiReceiveC::Get8bit - return iBuffer[aOffset]=%hhu", iBuffer[aOffset] );
    
    return iBuffer[ aOffset ];

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::GetData
// -----------------------------------------------------------------------------
EXPORT_C const TPtrC8 TIsiReceiveC::GetData
        (
        const TUint aOffset,
        const TInt aLength
        ) const
    {

    C_TRACE( ( _L ( "TIsiReceiveC::GetData entering, aOffset = 0x%x" ), aOffset ) );
    OstTrace1( TRACE_NORMAL, TISIRECEIVEC_GETDATA, "TIsiReceiveC::GetData - enter;aOffset=%u", aOffset );
    
    ASSERT_RESET_ALWAYS( ( ( iBuffer.Length() - aLength ) >= aOffset), KInvalidOffset, KErrOverflow );
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= aLength ), KInvalidOffset, KErrOverflow );

    TRACE_ASSERT ( aLength > 0 ); // Invalid length, should be >0

    C_TRACE( ( _L ( "TIsiReceiveC::GetData - return, aOffset = 0x%x, aLength = 0x%x" ), aOffset, aLength ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISIRECEIVEC_GETDATA, "TIsiReceiveC::GetData - return aOffset=%u;aLength=%u", aOffset, aLength );
    
    return TPtrC8( iBuffer.Mid( aOffset, aLength ) );

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::TIsiReceiveC
// -----------------------------------------------------------------------------
EXPORT_C TIsiReceiveC::TIsiReceiveC
        (
        const TDesC8& aBuf
        )
        :TIsi(),
        iBuffer( aBuf )
    {
    C_TRACE( ( _T ( "TIsiReceiveC::TIsiReceiveC constructor" ) ) );
    OstTrace0( TRACE_NORMAL, TISIRECEIVEC_TISIRECEIVEC, "TIsiReceiveC::TIsiReceiveC constructor" );
    

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::GetBuffer
// -----------------------------------------------------------------------------
EXPORT_C const TPtrC8 TIsiReceiveC::GetBuffer
        () const
    {
    C_TRACE( ( _T ( "TIsiReceiveC::GetBuffer" ) ) );
    OstTrace0( TRACE_NORMAL, TISIRECEIVEC_GETBUFFER, "TIsiReceiveC::GetBuffer" );
    
    return iBuffer;

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::GetReceiverDevice
// -----------------------------------------------------------------------------
EXPORT_C TUint8 TIsiReceiveC::GetReceiverDevice
        () const
    {

    C_TRACE( ( _T ( "TIsiReceiveC::GetReceiverDevice - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIRECEIVEC_GETRECEIVERDEVICE, "TIsiReceiveC::GetReceiverDevice - enter" );
    
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), KInvalidOffset, KErrOverflow);

    TUint8 receiverDevice = GET_RECEIVER_DEV(iBuffer);

    C_TRACE( ( _T ( "TIsiReceiveC::GetReceiverDevice - return, receiverDevice = 0x%x" ), receiverDevice ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GETRECEIVERDEVICE, "TIsiReceiveC::GetReceiverDevice - return receiverDevice=%hhu", receiverDevice );
    
    return receiverDevice;

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::GetSenderDevice
// -----------------------------------------------------------------------------
EXPORT_C TUint8 TIsiReceiveC::GetSenderDevice
        () const
    {

    C_TRACE( ( _T ( "TIsiReceiveC::GetSenderDevice - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIRECEIVEC_GETSENDERDEVICE, "TIsiReceiveC::GetSenderDevice - enter" );
    
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), KInvalidOffset, KErrOverflow );

    TUint8 senderDevice = GET_SENDER_DEV(iBuffer);
                
    C_TRACE( ( _T ( "TIsiReceiveC::GetSenderDevice - return, senderDevice = 0x%x" ), senderDevice ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GETSENDERDEVICE, "TIsiReceiveC::GetSenderDevice - return senderDevice=%hhu", senderDevice );
    
    return senderDevice;

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::GetReceiverObject
// -----------------------------------------------------------------------------
EXPORT_C TUint16 TIsiReceiveC::GetReceiverObject
        () const
    {

    C_TRACE( ( _T ( "TIsiReceiveC::GetReceiverObject - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIRECEIVEC_GETRECEIVEROBJECT, "TIsiReceiveC::GetReceiverObject - enter" );
    
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), KInvalidOffset, KErrOverflow );

    TUint16 receiverObject = static_cast<TUint16>GET_RECEIVER_OBJ(iBuffer);
                
    C_TRACE( ( _T ( "TIsiReceiveC::GetReceiverObject - return, receiverObject = 0x%x" ), receiverObject ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GETRECEIVEROBJECT, "TIsiReceiveC::GetReceiverObject - return receiverObject=%hu", receiverObject );
    
    return receiverObject;

    }


// -----------------------------------------------------------------------------
// TIsiReceiveC::GetSenderObject
// -----------------------------------------------------------------------------
EXPORT_C TUint16 TIsiReceiveC::GetSenderObject
        () const
    {

    C_TRACE( ( _T ( "TIsiReceiveC::GetSenderObject - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIRECEIVEC_GETSENDEROBJECT, "TIsiReceiveC::GetSenderObject - enter" );
    
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID),  KInvalidOffset, KErrOverflow );

    TUint16 senderObject = static_cast<TUint16>GET_SENDER_OBJ( iBuffer );

    C_TRACE( ( _T ( "TIsiReceiveC::GetSenderObject - return, senderObject = 0x%x" ), senderObject ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GETSENDEROBJECT, "TIsiReceiveC::GetSenderObject - return senderObject=%hu", senderObject );
    
    return senderObject;

    }

// -----------------------------------------------------------------------------
// TIsiReceiveC::GetExtendedResourceId
// -----------------------------------------------------------------------------
EXPORT_C TUint32 TIsiReceiveC::GetExtendedResourceId
        () const
    {

    C_TRACE( ( _T ( "TIsiReceiveC::GetExtendedResourceId - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIRECEIVEC_GETEXTENDEDRESOURCEID, "TIsiReceiveC::GetExtendedResourceId - enter" );
    
    TUint32 extendedResourceId(0x00000000);

    //Check that extended resource id available and not reading over the buffer
    ASSERT_RESET_ALWAYS( (iBuffer[ISI_HEADER_OFFSET_RESOURCEID] == PN_PREFIX), KInvalidResourceId, KErrOverflow );
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), KInvalidResourceId, KErrOverflow );

    TUint8 temp(iBuffer[ ISI_HEADER_OFFSET_SUBTYPE ]);
    TUint32 subType = static_cast<TUint32>( temp << 16 );
    TUint32 type = static_cast<TUint32>( iBuffer[ ISI_HEADER_OFFSET_TYPE ] << 8 );
                                                                  
    extendedResourceId = static_cast<TUint32>(PN_PREFIX) | subType | type ;

    C_TRACE( ( _T ( "TIsiReceiveC::GetExtendedResourceId - return, extendedResourceId = 0x%x" ), extendedResourceId ) );
    OstTrace1( TRACE_NORMAL, DUP1_TISIRECEIVEC_GETEXTENDEDRESOURCEID, "TIsiReceiveC::GetExtendedResourceId - return extendedResourceId=%u", extendedResourceId );
    
    return extendedResourceId;

    }

//-------------------------------------------------------------------------------
// TIsiSend::Complete 
// -----------------------------------------------------------------------------
EXPORT_C TDes8& TIsiSend::Complete
        (
        // None
        )
    {

    C_TRACE( ( _L ( "TIsiSend::Complete entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISISEND_COMPLETE, "TIsiSend::Complete - enter" );
    
    // Set msg length in hex.
    TUint16 length( TUint16( iBuffer.Length() - KMessageHeaderLength ) ); 
    TUint16 calcFinalLength( TUint16( iFinalLength - KMessageHeaderLength ) );

    C_TRACE( ( _T ( "TIsiSend::Complete, length: 0x%x, calcfinallength: 0x%x" ), length, calcFinalLength ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISISEND_COMPLETE, "TIsiSend::Complete;length=%hu;calcFinalLength=%hu", length, calcFinalLength );
    
    #if defined( __WINS__ ) || defined( __WINSCW__ )
        {
         if( iFinalLength == 0 )    // TIsiSend alternative constructor used
            {
                TUint8 byte1 = ( TUint8 )( ( length & 0xff00 ) >> 8 );
                TUint8 byte2 = ( TUint8 )( length & 0x00ff );        
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH, byte1 );        // BE
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH + 1, byte2 );    // BE
            }
         else
            {
                TUint8 byte1 = ( TUint8 )( ( calcFinalLength & 0xff00 ) >> 8 );
                TUint8 byte2 = ( TUint8 )( calcFinalLength & 0x00ff );        
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH, byte1 );        // BE
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH + 1, byte2 );    // BE               
                iBuffer.SetLength( iFinalLength );
            }
        }
    #else
        {
         if( iFinalLength == 0 )    // TIsiSend alternative constructor used
                {
                TUint8 byte1 = ( TUint8 )( ( length & 0xff00 ) >> 8 );
                TUint8 byte2 = ( TUint8 )( length & 0x00ff );        
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH, byte2 );        // LE
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH + 1, byte1 );    // LE
                }
            else
                {
                TUint8 byte1 = ( TUint8 )( ( calcFinalLength & 0xff00 ) >> 8 );
                TUint8 byte2 = ( TUint8 )( calcFinalLength & 0x00ff );        
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH, byte2 );        // LE
                this->Set8bit( ISI_HEADER_OFFSET_LENGTH + 1, byte1 );    // LE               
                iBuffer.SetLength( iFinalLength );
                }
        }
    #endif    

    C_TRACE( ( _T ( "TIsiSend::Complete, byte2: 0x%x, byte1: 0x%x" ), iBuffer[5], iBuffer[4] ) );
    OstTraceExt2( TRACE_NORMAL, DUP2_TISISEND_COMPLETE, "TIsiSend::Complete;byte2=%hhu;byte1=%hhu", iBuffer[5], iBuffer[4] );
    
    C_TRACE( ( _L ( "TIsiSend::Complete - return, buffer length = 0x%x, iFinalLength = 0x%x" ), length, iFinalLength ) );
    OstTraceExt2( TRACE_NORMAL, DUP3_TISISEND_COMPLETE, "TIsiSend::Complete - return buffer length=%hu;iFinalLength=%u", length, iFinalLength );
    
    return iBuffer;

    }

// -----------------------------------------------------------------------------
// TIsiSend::CopyData
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::CopyData
        (
        const TUint aOffset,
        const TDesC8& aData
        )
    {

    C_TRACE( ( _L ( "TIsiSend::CopyData entering, offset = 0x%x" ), aOffset ) );
    OstTrace1( TRACE_NORMAL, TISISEND_COPYDATA, "TIsiSend::CopyData - enter;aOffset=%u", aOffset );

    TInt length( aData.Length() );
       
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + length ), KInvalidOffset, KErrOverflow );
    
    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + length );     
  
    ASSERT_RESET_ALWAYS( ( ( iBuffer.Length() - length ) >= aOffset ), KInvalidOffset, KErrOverflow );
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= length ), KInvalidOffset, KErrOverflow );
       
    TRACE_ASSERT ( length > 0); // Invalid descriptor, buffer underflow 
    
    // Add byte by byte starting from offset place until the whole data is set.
    for( TInt i( 0 ); i < length; i++ )
        {
        this->Set8bit( ( aOffset + i ), aData.Ptr()[ i ] );
        }

    OstTraceExt2( TRACE_NORMAL, DUP1_TISISEND_COPYDATA, "TIsiSend::CopyData - return aOffset=%u;length=%d", aOffset, length );
    C_TRACE( ( _L ( "TIsiSend::CopyData - return, offset = 0x%x, data length = 0x%x " ), aOffset, length ) );
    
    }

// -----------------------------------------------------------------------------
// TIsiSend::TIsiSend
// -----------------------------------------------------------------------------
EXPORT_C TIsiSend::TIsiSend
        (
        TDes8& aBuf,
        const TUint aLength
        )
        :TIsi(),
        iBuffer( aBuf ),
        iFinalLength( aLength ) // this should be same as msg length
    {

    C_TRACE( ( _L ( "TIsiSend::TIsiSend entering, aLength = 0x%x " ), aLength ) ) ;
    OstTrace1( TRACE_NORMAL, TISISEND_TISISEND, "TIsiSend::TIsiSend - enter;aLength=%u", aLength );
    
    // Length check
	ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aLength ), KInvalidOffset, KErrOverflow );

    // Set the message to contain zeros to it's length.
    iBuffer.FillZ( aLength );

    // Set the length to zero.
    iBuffer.Zero();
    
    C_TRACE( ( _L ( "TIsiSend::TIsiSend - return. iFinalLength = 0x%x" ), iFinalLength ) ) ;
    OstTrace1( TRACE_NORMAL, DUP1_TISISEND_TISISEND, "TIsiSend::TIsiSend - return iFinalLength=%u", iFinalLength );
    
    }

// -----------------------------------------------------------------------------
// TIsiSend::TIsiSend alternative constructor
// -----------------------------------------------------------------------------
EXPORT_C TIsiSend::TIsiSend
        (
        TDes8& aBuf
        )
        :TIsi(),
        iBuffer( aBuf ),
        iFinalLength( 0 )
    {

    C_TRACE( ( _L ( "TIsiSend::TIsiSend constructor without length, entering" ) ) );
    OstTrace0( TRACE_NORMAL, DUP2_TISISEND_TISISEND, "TIsiSend::TIsiSend constructor without length - enter" );
    
    // Set the message to contain zeros to it's max length.
    iBuffer.FillZ( iBuffer.MaxLength() );

    // Set the length to zero.
    iBuffer.Zero();
    
    C_TRACE( ( _L ( "TIsiSend::TIsiSend constructor without length - return. iFinalLength = 0x%x" ), iFinalLength ) ) ;
    OstTrace1( TRACE_NORMAL, DUP3_TISISEND_TISISEND, "TIsiSend::TIsiSend constructor without length - return iFinalLength=%u", iFinalLength );
    
    }

// -----------------------------------------------------------------------------
// TIsiSend::Set16bit
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::Set16bit
        (
        const TUint aOffset,
        const TUint16 aData
        )
    {
    C_TRACE( ( _L ( "TIsiSend::Set16bit entering, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, TISISEND_SET16BIT, "TIsiSend::Set16bit - enter;aOffset=%u;aData=%hu", aOffset, aData );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + 2 ), KInvalidOffset, KErrOverflow );
    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + 2 ); 
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() -1 >= aOffset ), KInvalidOffset, KErrOverflow );
    
    TRACE_ASSERT ( aOffset % 2 == 0 ); // Invalid offset

    TUint8 byte1 = ( TUint8 )( ( aData & 0xff00 ) >> 8 );
    TUint8 byte2 = ( TUint8 )( aData & 0x00ff );

    this->Set8bit( aOffset, byte1 );     
    this->Set8bit( aOffset + 1, byte2 ); 

    C_TRACE( ( _L ( "TIsiSend::Set16bit - return, aOffset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISISEND_SET16BIT, "TIsiSend::Set16bit - return;aOffset=%u;aData=%hu", aOffset, aData );
    
    }

// -----------------------------------------------------------------------------
// TIsiSend::Set32bit
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::Set32bit
        (
        const TUint aOffset,
        const TUint32 aData
        )
    {

    C_TRACE( ( _L ( "TIsiSend::Set32bit entering, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );   
    OstTraceExt2( TRACE_NORMAL, TISISEND_SET32BIT, "TIsiSend::Set32bit - enter;aOffset=%u;aData=%hhu", aOffset, ( TUint )( aData ) );
        
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + 4  ), KInvalidOffset, KErrOverflow );
    
    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + 4 ); 
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() -3 >= aOffset ), KInvalidOffset, KErrOverflow );
    
    TRACE_ASSERT ( aOffset % 4 == 0 ); // Invalid offset

    TUint8 byte1 = ( TUint8 )( ( aData & 0xff000000 ) >> 24 );
    TUint8 byte2 = ( TUint8 )( ( aData & 0x00ff0000 ) >> 16 );
    TUint8 byte3 = ( TUint8 )( ( aData & 0x0000ff00 ) >> 8 );
    TUint8 byte4 = ( TUint8 )( aData & 0x000000ff );

    this->Set8bit( aOffset, byte1 );       
    this->Set8bit( aOffset + 1, byte2 );  
    this->Set8bit( aOffset + 2, byte3 ); 
    this->Set8bit( aOffset + 3, byte4 ); 

    C_TRACE( ( _L ( "TIsiSend::Set32bit - return, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISISEND_SET32BIT, "TIsiSend::Set32bit - return;aOffset=%u;aData=%hhu", aOffset, ( TUint )( aData ) );
    
    }

// -----------------------------------------------------------------------------
// TIsiSend::Set8bit
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::Set8bit
        (
        const TUint aOffset,
        const TUint8 aData
        )
    {

    C_TRACE( ( _L ( "TIsiSend::Set8bit entering, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, TISISEND_SET8BIT, "TIsiSend::Set8bit - enter;aOffset=%u;aData=%hhu", aOffset, aData );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + 1 ), KInvalidOffset, KErrOverflow );

    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= aOffset ), KInvalidOffset, KErrOverflow );

    iBuffer[ aOffset ] = aData;

    C_TRACE( ( _L ( "TIsiSend::Set8bit - return, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISISEND_SET8BIT, "TIsiSend::Set8bit - return;aOffset=%u;aData=%hhu", aOffset, aData );
    
    }

// -----------------------------------------------------------------------------
// TIsiSend::SetReceiverDevice
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::SetReceiverDevice
        (
        const TUint8 aReceiverDevice
        )
    {

    C_TRACE( ( _T ( "TIsiSend::SetReceiverDevice entering - receiverDevice = 0x%x" ), aReceiverDevice ) );
    OstTraceExt1( TRACE_NORMAL, TISISEND_SETRECEIVERDEVICE, "TIsiSend::SetReceiverDevice - enter;aReceiverDevice=%hhu", aReceiverDevice );
    
    //Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), KInvalidOffset, KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_RECEIVERDEVICE ) iBuffer.SetLength( ISI_HEADER_OFFSET_RECEIVERDEVICE + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_RECEIVERDEVICE ), KInvalidOffset, KErrOverflow );

    iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] = SET_RECEIVER_DEV(iBuffer, aReceiverDevice);
        
    C_TRACE( ( _T ( "TIsiSend::SetReceiverDevice return - receiverDevice = 0x%x "), iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISISEND_SETRECEIVERDEVICE, "TIsiSend::SetReceiverDevice - return; receiverDevice=%hhu", iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] );
    
    }

// -----------------------------------------------------------------------------
// TIsiSend::SetSenderDevice
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::SetSenderDevice
        (
        const TUint8 aSenderDevice
        )
    {

    C_TRACE( ( _T ( "TIsiSend::SetSenderDevice entering - senderDevice = 0x%x" ), aSenderDevice ) );
    OstTraceExt1( TRACE_NORMAL, TISISEND_SETSENDERDEVICE, "TIsiSend::SetSenderDevice - enter;aSenderDevice=%hhu", aSenderDevice );
    
    //Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), KInvalidOffset, KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_SENDERDEVICE ) iBuffer.SetLength( ISI_HEADER_OFFSET_SENDERDEVICE + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_SENDERDEVICE ), KInvalidOffset, KErrOverflow );

    iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] = SET_SENDER_DEV( iBuffer, aSenderDevice );
        
    C_TRACE( ( _T ( "TIsiSend::SetSenderDevice return - senderDevice = 0x%x "),  iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] ) );        
    OstTraceExt1( TRACE_NORMAL, DUP1_TISISEND_SETSENDERDEVICE, "TIsiSend::SetSenderDevice - return;senderDevice=%hhu", iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] );

    }

// -----------------------------------------------------------------------------
// TIsiSend::SetReceiverObject
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::SetReceiverObject
        ( 
        const TUint16 aReceiverObject
        )
    {

    C_TRACE( ( _T ( "TIsiSend::SetReceiverObject entering- receiverObject= 0x%x" ), aReceiverObject ) );
    OstTraceExt1( TRACE_NORMAL, TISISEND_SETRECEIVEROBJECT, "TIsiSend::SetReceiverObject - enter;aReceiverObject=%hu", aReceiverObject );
    
    // Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), KInvalidOffset, KErrOverflow );

    ASSERT_RESET_ALWAYS( ( aReceiverObject < 0x3FF ), KInvalidValue, KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_RECEIVEROBJECT ) iBuffer.SetLength( ISI_HEADER_OFFSET_RECEIVEROBJECT + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_RECEIVEROBJECT ), KInvalidOffset, KErrOverflow );
              
    SET_RECEIVER_OBJ(iBuffer, aReceiverObject);

    C_TRACE( ( _T ( "TIsiSend::SetReceiverObject - set receiverDevice = 0x%x "), iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] ) );    
    OstTraceExt1( TRACE_NORMAL, DUP1_TISISEND_SETRECEIVEROBJECT, "TIsiSend::SetReceiverObject - set receiverDevice;receiverDevice=%hhu", iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] );

    C_TRACE( ( _T ( "TIsiSend::SetReceiverObject return - receiverObject = 0x%x "), iBuffer[ISI_HEADER_OFFSET_RECEIVEROBJECT] ) );
    OstTraceExt1( TRACE_NORMAL, DUP2_TISISEND_SETRECEIVEROBJECT, "TIsiSend::SetReceiverObject - return;receiverObject=%hhu", iBuffer[ISI_HEADER_OFFSET_RECEIVEROBJECT] );

    }

// -----------------------------------------------------------------------------
// TIsiSend::SetSenderObject
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::SetSenderObject
        ( 
        const TUint16 aSenderObject
        )
    {

    C_TRACE( ( _T ( "TIsiSend::SetSenderObject entering- senderObject= 0x%x" ), aSenderObject ) );
    OstTraceExt1( TRACE_NORMAL, TISISEND_SETSENDEROBJECT, "TIsiSend::SetSenderObject - enter;aSenderObject=%hhx", aSenderObject );
    
    // Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), KInvalidOffset, KErrOverflow );
    ASSERT_RESET_ALWAYS( ( aSenderObject < 0x3FF ), KInvalidValue, KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_SENDEROBJECT ) iBuffer.SetLength( ISI_HEADER_OFFSET_SENDEROBJECT + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_SENDEROBJECT ), KInvalidOffset, KErrOverflow );

    SET_SENDER_OBJ(iBuffer, aSenderObject);

    C_TRACE( ( _T ( "TIsiSend::SetSenderObject -  senderDevice = 0x%x" ), iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISISEND_SETSENDEROBJECT, "TIsiSend::SetSenderObject - senderDevice;senderDevice=%hhx", iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] );

    C_TRACE( ( _T ( "TIsiSend::SetSenderObject return - senderObject = 0x%x" ), iBuffer[ISI_HEADER_OFFSET_SENDEROBJECT] ) );        
    OstTraceExt1( TRACE_NORMAL, DUP2_TISISEND_SETSENDEROBJECT, "TIsiSend::SetSenderObject - return;senderObject=%hhx", iBuffer[ISI_HEADER_OFFSET_SENDEROBJECT] );
    

    }

// -----------------------------------------------------------------------------
// TIsiSend::SetExtendedResourceId
// -----------------------------------------------------------------------------
EXPORT_C void TIsiSend::SetExtendedResourceId
        (
        const TUint32 aExtendedResourceId
        )
    {

    C_TRACE( ( _T ( "TIsiSend::SetExtendedResourceId entering - extendedResourceId = 0x%x" ), aExtendedResourceId ) );
    OstTrace1( TRACE_NORMAL, TISISEND_SETEXTENDEDRESOURCEID, "TIsiSend::SetExtendedResourceId - enter;aExtendedResourceId=%u", aExtendedResourceId );
    
    TUint8 prefix( aExtendedResourceId & 0xFF );
                           
    //Check that extended resource id available and not reading over the buffer
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_SUBTYPE + 1 ), KInvalidOffset, KErrOverflow );
    ASSERT_RESET_ALWAYS( ( aExtendedResourceId < 0xFFFFFF ), KInvalidResourceId, KErrOverflow );
    ASSERT_RESET_ALWAYS( ( prefix == PN_PREFIX ), KInvalidValue, KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_SUBTYPE ) iBuffer.SetLength( ISI_HEADER_OFFSET_SUBTYPE + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_SUBTYPE ),  KInvalidOffset, KErrOverflow );
            
    TUint8 type = (( aExtendedResourceId >> 8 ) & 0xFF );       
    TUint8 subType = (( aExtendedResourceId >> 16 ) & 0xFF );                                       

    iBuffer[ ISI_HEADER_OFFSET_TYPE ] = type;
    iBuffer[ ISI_HEADER_OFFSET_SUBTYPE ] = subType; 
    iBuffer[ ISI_HEADER_OFFSET_RESOURCEID ] = prefix;
                
    OstTraceExt3( TRACE_NORMAL, DUP1_TISISEND_SETEXTENDEDRESOURCEID, "TIsiSend::SetExtendedResourceId - return;type=%hhu;subType=%hhu;prefix=%hhu", type, subType, prefix );
    C_TRACE( ( _T ( "TIsiSend::SetExtendedResourceId return - type = 0x%x, subtype = 0x%x, prefix = 0x%x" ), type, subType, prefix ) );        

    }


// -----------------------------------------------------------------------------
// TIsiSubBlock::TIsiSubBlock
// -----------------------------------------------------------------------------
EXPORT_C TIsiSubBlock::TIsiSubBlock
        (
        TDes8& aMsg, 
        TUint16 aSbId,
        const TSubBlockType aType
        ):  iSubBlockStart( aMsg.Length() ),
            iMsg(aMsg),
            iSubBlockType(aType)
    {
    C_TRACE( ( _L ( "TIsiSubBlock::TIsiSubBlock entering - sbId = 0x%x, subBlockType = %d" ), aSbId, aType ) );        
    OstTraceExt2( TRACE_NORMAL, TISISUBBLOCK_TISISUBBLOCK, "TIsiSubBlock::TIsiSubBlock - enter;aSbId=%hu;aType=%x", aSbId, ( TUint )&( aType ) );
    
    
    TUint8 byte1(0x00);
    TUint8 byte2(0x00);
    
     
        switch( iSubBlockType )
            {
            case EIsiSubBlockTypeId8Len8:
                {
                  iMsg.Append( aSbId ); //sbId 8 bit
                  iMsg.Append( 0x00); // length 8 bit
                break;
                }
            case EIsiSubBlockTypeId8Len16:
                {
                  iMsg.Append( aSbId );
                  iMsg.Append( 0x00 ); //filler
                  iMsg.Append( 0x00 ); // length 16 bit
                  iMsg.Append( 0x00 ); // length 16 bit
                break;
                }
            case EIsiSubBlockTypeId16Len8:
                {
                  byte1 = ( TUint8 )( ( aSbId & 0xff00 ) >> 8 );
                  byte2 = ( TUint8 )( aSbId & 0x00ff ); 

                  iMsg.Append(byte1); // sbId 16 bit
                  iMsg.Append(byte2); // sbId 16 bit
                  iMsg.Append(0x00); // lenght 8 bit
                break;
                }
            case EIsiSubBlockTypeId16Len16:
                {
                  byte1 = ( TUint8 )( ( aSbId & 0xff00 ) >> 8 );
                  byte2 = ( TUint8 )( aSbId & 0x00ff );

                  iMsg.Append(byte1); // sbId 16 bit
                  iMsg.Append(byte2); // sbId 16 bit
                  iMsg.Append(0x00); // lenght 16 bit
                  iMsg.Append(0x00); // length 16 bit
                break;
                }
            case EIsiSubBlockTypeId8Len32:
                {
                  iMsg.Append(aSbId); // sbId 8 bit
                  iMsg.Append(0x00); // filler
                  iMsg.Append(0x00); // filler 
                  iMsg.Append(0x00); // filler 
                  iMsg.Append(0x00); // length 32 bit 
                  iMsg.Append(0x00); // length 32 bit 
                  iMsg.Append(0x00); // length 32 bit 
                  iMsg.Append(0x00); // length 32 bit 
                break;
                }
            default:
                {
                User::Panic( KInvalidSubBlockType, KErrArgument );
                break;
                }
            }

    C_TRACE( ( _L ( "TIsiSubBlock::TIsiSubBlock returning, sbId = 0x%x, subBlockType = %d" ), aSbId, aType ) );    
    OstTraceExt2( TRACE_NORMAL, DUP1_TISISUBBLOCK_TISISUBBLOCK, "TIsiSubBlock::TIsiSubBlock - return;aSbId=%hu;aType=%x", aSbId, ( TUint )&( aType ) );

    }

// -----------------------------------------------------------------------------
// TIsiSubBlock::CompleteSubBlock
// -----------------------------------------------------------------------------
EXPORT_C TDes8& TIsiSubBlock::CompleteSubBlock
        (
        ) 
    {
    C_TRACE( ( _L ( "TIsiSubBlock::CompleteSubBlock entering " ) ) );   
    OstTrace0( TRACE_NORMAL, TISISUBBLOCK_COMPLETESUBBLOCK, "TIsiSubBlock::CompleteSubBlock - enter" );
    
    TUint8 byte1( 0x00 );
    TUint8 byte2( 0x00 );
    TUint8 byte3( 0x00 );
    TUint8 byte4( 0x00 );
    TUint16 length16(0x0000);
    TUint32 length32(0x00000000);

    // make sb length divisible by 4
    while( (iMsg.Length() -iSubBlockStart ) % 4 )
        {
        iMsg.Append( KPaddingByte );
        }

    // update subblock length
    switch( iSubBlockType )
        {
        case EIsiSubBlockTypeId8Len8:
            {
              iMsg[iSubBlockStart+1] = TUint8( iMsg.Length()-iSubBlockStart );
            break;
            }
        case EIsiSubBlockTypeId8Len16:
            {
              length16 = TUint16( iMsg.Length()-iSubBlockStart );
              byte1 = ( TUint8 )( ( length16 & 0xff00 ) >> 8 );
              byte2 = ( TUint8 )( length16 & 0x00ff );
              iMsg[iSubBlockStart+2] = byte1;
              iMsg[iSubBlockStart+3] = byte2;
            break;
            }
        case EIsiSubBlockTypeId16Len8:
            {
              iMsg[iSubBlockStart+2] = TUint8( iMsg.Length()-iSubBlockStart );
            break;
            }
        case EIsiSubBlockTypeId16Len16:
            {
              length16 = TUint16( iMsg.Length()-iSubBlockStart );
              byte1 = ( TUint8 )( ( length16 & 0xff00 ) >> 8 );
              byte2 = ( TUint8 )( length16 & 0x00ff );
              iMsg[iSubBlockStart+2] = byte1;
              iMsg[iSubBlockStart+3] = byte2;
            break;
            }
        case EIsiSubBlockTypeId8Len32:
            {
              length32 = TUint32( iMsg.Length()-iSubBlockStart );
              byte1 = ( TUint8 )( ( length32 & 0xff000000 ) >> 24 );
              byte2 = ( TUint8 )( ( length32 & 0x00ff0000 ) >> 16 );
              byte3 = ( TUint8 )( ( length32 & 0x0000ff00 ) >> 8 );
              byte4 = ( TUint8 )( length32 & 0x000000ff );
              iMsg[iSubBlockStart+4] = byte1;
              iMsg[iSubBlockStart+5] = byte2;
              iMsg[iSubBlockStart+6] = byte3;
              iMsg[iSubBlockStart+7] = byte4;
            break;
            }
        default:
            {
            User::Panic( KInvalidSubBlockType, KErrArgument );
            break;
            }
        }
        
    C_TRACE( ( _L ( "TIsiSubBlock::CompleteSubBlock returning " ) ) );   
    OstTrace0( TRACE_NORMAL, DUP1_TISISUBBLOCK_COMPLETESUBBLOCK, "TIsiSubBlock::CompleteSubBlock - return" );    
    
    return iMsg;

    }


// -----------------------------------------------------------------------------
// TIsiUtility::CopyFromBigEndian
// -----------------------------------------------------------------------------
EXPORT_C void TIsiUtility::CopyFromBigEndian
        (
        const TDesC8& aSource, 
        TDes16& aTarget
        ) 
    {

    C_TRACE( ( _L ( "TIsiUtility::CopyFromBigEndian entering " ) ) ); 
    OstTrace0( TRACE_NORMAL, TISIUTILITY_COPYFROMBIGENDIAN, "TIsiUtility::CopyFromBigEndian - enter" );
    
    TUint length(0);

    //Checks that data length are acceptable
    if ( aSource.Length() > aTarget.MaxSize() )
        {
        length = aTarget.MaxSize();
        }
    else
        {
        length = ( aSource.Length() / 2 ) * 2;
        }

    //Append source to target
    for (TUint i = 0; i < length; i += 2 )
       {
        aTarget.Append(TUint16( aSource[i + 1] | ( aSource[i] << 8) )); 
        }
        
    C_TRACE( ( _L ( "TIsiUtility::CopyFromBigEndian returning. length =  0x%x" ), length ) );      
    OstTrace1( TRACE_NORMAL, DUP1_TISIUTILITY_COPYFROMBIGENDIAN, "TIsiUtility::CopyFromBigEndian - return;length=%u", length );
    
    }


// -----------------------------------------------------------------------------
// TIsiUtility::CopyToBigEndian
// -----------------------------------------------------------------------------
EXPORT_C void TIsiUtility::CopyToBigEndian
        (
        const TDesC16& aSource, 
        TDes8& aTarget
        ) 
    {
    C_TRACE( ( _L ( "TIsiUtility::CopyToBigEndian entering " ) ) ); 
    OstTrace0( TRACE_NORMAL, TISIUTILITY_COPYTOBIGENDIAN, "TIsiUtility::CopyToBigEndian - enter" );
    
    TUint length(0);

    //Checks that data length are acceptable
    if ( aSource.Size() > aTarget.MaxSize() )
        {
        length = ( aTarget.MaxSize() / 2 );
        }
    else
        {
        length = aSource.Length();
        }

    //Append source to target
    for (TUint i = 0; i < length; i ++ )
       {
        aTarget.Append( TUint8(aSource[i] >> 8) ); 
        aTarget.Append( TUint8(aSource[i] & 0xff) );
        }
        
    C_TRACE( ( _L ( "TIsiUtility::CopyToBigEndian returning length =  0x%x" ), length ) );   
    OstTrace1( TRACE_NORMAL, DUP1_TISIUTILITY_COPYTOBIGENDIAN, "TIsiUtility::CopyToBigEndian - return;length=%u", length );
    

    }



//end of file

