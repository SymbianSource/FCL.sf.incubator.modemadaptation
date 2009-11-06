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


#include <pn_const.h>
#include "isikernel.h"
#include "kernelisimessage_trace.h"
#include <phonetisi.h>
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "isikerneltraces.h"
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



// -----------------------------------------------------------------------------
// TIsiKernel::operator=
// -----------------------------------------------------------------------------
 EXPORT_C TIsiKernel& TIsiKernel::operator=
        ( 
        const TIsiKernel&
        )
    {
        
    COMPONENT_TRACE( ( _T( "TIsiKernel::operator=" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNEL_OPERATOR, "TIsiKernel::operator=" );
    return ( *this );

    }
 
// -----------------------------------------------------------------------------
// TIsiKernel::TIsiKernel
// Constructor.
// -----------------------------------------------------------------------------

EXPORT_C TIsiKernel::TIsiKernel()
    {
        
    COMPONENT_TRACE( ( _T( "TIsiKernel::TIsiKernel()" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNEL_TISIKERNEL, "TIsiKernel::TIsiKernel constructor" );
    
    }
  
// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::FindSubBlockOffsetById
// -----------------------------------------------------------------------------
EXPORT_C TInt TIsiKernelReceiveC::FindSubBlockOffsetById 
        ( 
        const TUint aSbStartOffset,
        const TUint aSubblockId,
        const TSubBlockType aType,
        TUint &aSubBlockOffset
        ) const
    {

     COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetById entering, (sbStart 0x%x, sbId 0x%x, aType %d  )" ), aSbStartOffset, aSubblockId, aType ) );  
     OstTraceExt3( TRACE_NORMAL, TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiKernelReceiveC::FindSubBlockOffsetById - enter;sbStart=%u;sbId=%u;aType=%x;", aSbStartOffset, aSubblockId, ( TUint )&( aType ) );
     
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
        COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetById used with msg which doesn't contains any subblocks, (retval %d)" ), retval ) );
        OstTrace1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiKernelReceiveC::FindSubBlockOffsetById used with msg which doesn't contains any subblocks;retval=%d", retval );       
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
                    ASSERT_RESET_ALWAYS( 0, "KernelIsiMessage: OFFSET Invalid", EKernelIsiMessageInvalidSubBlockType );
                    break;
                    }
                }

            if ( subblock_len == 0 )
                {
                OstTrace0( TRACE_NORMAL, DUP2_TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiKernelReceiveC::FindSubBlockOffsetById has found corrupted data, sb_len = 0 ->stop searching!" );
                COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetById has found corrupted data, sb_len = 0 ->stop searching!" ) ) );  
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

    OstTrace0( TRACE_NORMAL, DUP3_TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYID, "TIsiKernelReceiveC::FindSubBlockOffsetById returning" );
    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetById - return, aSubBlockOffset = 0x%x" ), finder_offset ) );

    return retval;
   
    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::FindSubBlockOffsetByIndex
// -----------------------------------------------------------------------------
EXPORT_C TInt TIsiKernelReceiveC::FindSubBlockOffsetByIndex
        ( 
        const TUint aSbStartOffset,
        const TUint aSubblockIndex,
        const TSubBlockType aType,
        TUint &aSubBlockOffset
        ) const
    { 

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetByIndex entering, (sbStart 0x%x, sbIndex 0x%x, aType %d  )" ), aSbStartOffset, aSubblockIndex, aType ) );
    OstTraceExt3( TRACE_NORMAL, TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiKernelReceiveC::FindSubBlockOffsetByIndex - enter;aSbStartOffset=%u;aSubblockIndex=%u;aType=%x;", aSbStartOffset, aSubblockIndex, ( TUint )&( aType ));

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
        OstTrace1( TRACE_NORMAL, DUP2_TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiKernelReceiveC::FindSubBlockOffsetByIndex used with msg which doesn't contains any subblocks;retval=%d", retval );      
        COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetByIndex used with msg which doesn't contains any subblocks, (retval %d)" ), retval ) );            
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
                ASSERT_RESET_ALWAYS( 0, "KernelIsiMessage: OFFSET Invalid", EKernelIsiMessageInvalidSubBlockType );
                break;
                }
            }

        if ( subblock_len == 0 )
            {
            OstTrace0( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiKernelReceiveC::FindSubBlockOffsetByIndex has found corrupted data, sb_len = 0 ->stop searching!" );
            COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetByIndex has found corrupted data, sb_len = 0 ->stop searching" ) ) );           
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

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::FindSubblockOffsetByIndex - return, aSubBlockOffset = 0x%x" ), finder_offset ) );
    OstTrace1( TRACE_NORMAL, DUP3_TISIKERNELRECEIVEC_FINDSUBBLOCKOFFSETBYINDEX, "TIsiKernelReceiveC::FindSubBlockOffsetByIndex - return;finder_offset=%d", finder_offset );

    return retval;
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::Get16bit
// -----------------------------------------------------------------------------
EXPORT_C TUint16 TIsiKernelReceiveC::Get16bit
        (
        const TUint aOffset 
        ) const
    {

    OstTrace1( TRACE_NORMAL, TISIKERNELRECEIVEC_GET16BIT, "TIsiKernelReceiveC::Get16bit - enter;aOffset=%u", aOffset );
    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::Get16bit entering, aOffset 0x%x" ), aOffset ) );

    ASSERT_RESET_ALWAYS( ((iBuffer.Length() - 1) > aOffset), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );    
    TRACE_ASSERT ( aOffset % 2 == 0 ); //Invalid offset

    TUint16 retval( KSixteenBitZero );
    TUint16 byte1( KSixteenBitZero );
    TUint16 byte2( KSixteenBitZero );

    byte1 = iBuffer[ aOffset ];
    byte2 = iBuffer[ aOffset + 1 ];
    retval = TUint16( ( byte1 << 8 ) | byte2 );  
   
    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::Get16bit - return, Return 16bit value = 0x%x" ), retval ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GET16BIT, "TIsiKernelReceiveC::Get16bit - return; retval=%hu", retval );

    return retval;

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::Get32bit
// -----------------------------------------------------------------------------
EXPORT_C TUint32 TIsiKernelReceiveC::Get32bit
        (
        const TUint aOffset
        ) const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::Get32bit entering, aOffset = 0x%x" ) , aOffset ) );
    OstTrace1( TRACE_NORMAL, TISIKERNELRECEIVEC_GET32BIT, "TIsiKernelReceiveC::Get32bit - enter;aOffset=%u", aOffset );
    
    ASSERT_RESET_ALWAYS( ( (iBuffer.Length() - 3) > aOffset ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
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

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::Get32bit - return, Return 32bit value = 0x%x" ), retval ) );
    OstTrace1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GET32BIT, "TIsiKernelReceiveC::Get32bit - return;retval=%d", retval );
    
    return retval;

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::Get8bit
// -----------------------------------------------------------------------------
EXPORT_C TUint8 TIsiKernelReceiveC::Get8bit
        (
        const TUint aOffset 
        ) const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::Get8bit entering, aOffset = 0x%x" ) , aOffset ) );
    OstTrace1( TRACE_NORMAL, TISIKERNELRECEIVEC_GET8BIT, "TIsiKernelReceiveC::Get8bit - enter;aOffset=%u", aOffset );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() > aOffset ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::Get8bit - return, return value = 0x%x" ) , iBuffer[aOffset] ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GET8BIT, "TIsiKernelReceiveC::Get8bit - return 8bit value=%hhu", iBuffer[aOffset] );
    
    return iBuffer[ aOffset ];

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::GetData
// -----------------------------------------------------------------------------
EXPORT_C const TPtrC8 TIsiKernelReceiveC::GetData
        (
        const TUint aOffset,
        const TInt aLength
        ) const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetData entering, aOffset = 0x%x" ), aOffset ) );
    OstTraceExt2( TRACE_NORMAL, TISIKERNELRECEIVEC_GETDATA, "TIsiKernelReceiveC::GetData - enter;aOffset=%u;aLength=%d", aOffset, aLength );
                    
    ASSERT_RESET_ALWAYS( ( ( iBuffer.Length() - aLength ) >= aOffset), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= aLength ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    TRACE_ASSERT ( aLength > 0 ); // Invalid length, should be >0

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetData - return, offset = 0x%x, length = 0x%x" ), aOffset, aLength ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GETDATA, "TIsiKernelReceiveC::GetData - return;offset=%u;data length=%d", aOffset, aLength );
    
    return TPtrC8( iBuffer.Mid( aOffset, aLength ) );

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::TIsiKernelReceiveC
// -----------------------------------------------------------------------------
EXPORT_C TIsiKernelReceiveC::TIsiKernelReceiveC
        (
        const TDesC8& aBuf
        )
        :TIsiKernel(),
        iBuffer( aBuf )
    {
    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::TIsiKernelReceiveC constructor" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELRECEIVEC_TISIKERNELRECEIVEC, "TIsiKernelReceiveC::TIsiKernelReceiveC constructor" );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::GetBuffer
// -----------------------------------------------------------------------------
EXPORT_C const TPtrC8 TIsiKernelReceiveC::GetBuffer
        () const
    {
    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetBuffer" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELRECEIVEC_GETBUFFER, "TIsiKernelReceiveC::GetBuffer" );
    
    return iBuffer;

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::GetReceiverDevice
// -----------------------------------------------------------------------------
EXPORT_C TUint8 TIsiKernelReceiveC::GetReceiverDevice
        () const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetReceiverDevice - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELRECEIVEC_GETRECEIVERDEVICE, "TIsiKernelReceiveC::GetReceiverDevice - enter" );
    
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), "KernelIsiMessage: Invalid buffer length", EKernelIsiMessageInvalidOffset );

    TUint8 receiverDevice = GET_RECEIVER_DEV(iBuffer);

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetReceiverDevice - return, receiverDevice = 0x%x" ), receiverDevice ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GETRECEIVERDEVICE, "TIsiKernelReceiveC::GetReceiverDevice - return;receiverDevice=%hhu", receiverDevice );
    
    return receiverDevice;

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::GetSenderDevice
// -----------------------------------------------------------------------------
EXPORT_C TUint8 TIsiKernelReceiveC::GetSenderDevice
        () const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetSenderDevice - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELRECEIVEC_GETSENDERDEVICE, "TIsiKernelReceiveC::GetSenderDevice - enter" );
    

    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), "KernelIsiMessage: Invalid buffer length", EKernelIsiMessageInvalidOffset );

    TUint8 senderDevice = GET_SENDER_DEV(iBuffer);
                
    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetSenderDevice - return, senderDevice = 0x%x" ), senderDevice ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GETSENDERDEVICE, "TIsiKernelReceiveC::GetSenderDevice - return;senderDevice=%hhu", senderDevice );

    return senderDevice;

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::GetReceiverObject
// -----------------------------------------------------------------------------
EXPORT_C TUint16 TIsiKernelReceiveC::GetReceiverObject
        () const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetReceiverObject - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELRECEIVEC_GETRECEIVEROBJECT, "TIsiKernelReceiveC::GetReceiverObject - enter" );
    
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), "KernelIsiMessage: Invalid buffer length", EKernelIsiMessageInvalidOffset );

    TUint16 receiverObject = static_cast<TUint16>GET_RECEIVER_OBJ(iBuffer);
                
    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetReceiverObject - return, receiverObject = 0x%x" ), receiverObject ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GETRECEIVEROBJECT, "TIsiKernelReceiveC::GetReceiverObject - return;receiverObject=%hu", receiverObject );
    
    return receiverObject;

    }


// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::GetSenderObject
// -----------------------------------------------------------------------------
EXPORT_C TUint16 TIsiKernelReceiveC::GetSenderObject
        () const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetSenderObject - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELRECEIVEC_GETSENDEROBJECT, "TIsiKernelReceiveC::GetSenderObject - enter" );
    
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), "KernelIsiMessage: Invalid buffer length", EKernelIsiMessageInvalidOffset );

    TUint16 senderObject = static_cast<TUint16>GET_SENDER_OBJ( iBuffer );

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetSenderObject - return, senderObject = 0x%x" ), senderObject ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GETSENDEROBJECT, "TIsiKernelReceiveC::GetSenderObject - return;senderObject=%hhu", senderObject );
    
    return senderObject;

    }

// -----------------------------------------------------------------------------
// TIsiKernelReceiveC::GetExtendedResourceId
// -----------------------------------------------------------------------------
EXPORT_C TUint32 TIsiKernelReceiveC::GetExtendedResourceId
        () const
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetExtendedResourceId - entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELRECEIVEC_GETEXTENDEDRESOURCEID, "TIsiKernelReceiveC::GetExtendedResourceId - enter" );
    
    TUint32 extendedResourceId(0x00000000);

    //Check that extended resource id available and not reading over the buffer
    ASSERT_RESET_ALWAYS( (iBuffer[ISI_HEADER_OFFSET_RESOURCEID] == PN_PREFIX), "KernelIsiMessage: Invalid buffer", EKernelIsiMessageInvalidResourceId );
    ASSERT_RESET_ALWAYS( (iBuffer.Length() > ISI_HEADER_OFFSET_MESSAGEID), "KernelIsiMessage: Invalid buffer", EKernelIsiMessageInvalidResourceId );

    TUint8 temp(iBuffer[ ISI_HEADER_OFFSET_SUBTYPE ]);
    TUint32 subType = static_cast<TUint32>( temp << 16 );
    TUint32 type = static_cast<TUint32>( iBuffer[ ISI_HEADER_OFFSET_TYPE ] << 8 );
                                                                  
    extendedResourceId = static_cast<TUint32>(PN_PREFIX) | subType | type ;

    COMPONENT_TRACE( ( _T ( "TIsiKernelReceiveC::GetExtendedResourceId - return, extendedResourceId = 0x%x" ), extendedResourceId ) );
    OstTrace1( TRACE_NORMAL, DUP1_TISIKERNELRECEIVEC_GETEXTENDEDRESOURCEID, "TIsiKernelReceiveC::GetExtendedResourceId - return;extendedResourceId=%u", extendedResourceId );
    
    return extendedResourceId;

    }

//-------------------------------------------------------------------------------
// TIsiKernelSend::Complete 
// -----------------------------------------------------------------------------
EXPORT_C TDes8& TIsiKernelSend::Complete
        (
        // None
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Complete entering" ) ) );
    OstTrace0( TRACE_NORMAL, TISIKERNELSEND_COMPLETE, "TIsiKernelSend::Complete - enter" );
    
    // Set msg length in hex.
    TUint16 length( TUint16( iBuffer.Length() - KMessageHeaderLength ) ); 
    TUint16 calcFinalLength( TUint16( iFinalLength - KMessageHeaderLength ) );

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Complete, length: 0x%x, calcfinallength: 0x%x" ), length, calcFinalLength ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISIKERNELSEND_COMPLETE, "TIsiKernelSend::Complete;length=%hu;calcFinalLength=%hu", length, calcFinalLength );
    
    #if defined( __WINS__ ) || defined( __WINSCW__ )
        {
        if( iFinalLength == 0 )    // TIsiKernelSend alternative constructor used
            {               
            iBuffer[ ISI_HEADER_OFFSET_LENGTH ] = static_cast<TUint8>( (length & 0xff00) >> 8);   //BE             
            iBuffer[ ISI_HEADER_OFFSET_LENGTH + 1 ] = static_cast<TUint8>(length & 0x00ff);    //BE
            }
        else
            {
            iBuffer[ ISI_HEADER_OFFSET_LENGTH ] = static_cast<TUint8>( (calcFinalLength & 0xff00 ) >> 8);   //BE             
            iBuffer[ ISI_HEADER_OFFSET_LENGTH + 1 ] = static_cast<TUint8>(calcFinalLength & 0x00ff);    //BE
            iBuffer.SetLength( iFinalLength );                
            }
        }        
        
    #else
        {
        if( iFinalLength == 0 )    // TIsiKernelSend alternative constructor used
            {
            iBuffer[ ISI_HEADER_OFFSET_LENGTH +1 ] = static_cast<TUint8>( (length & 0xff00) >> 8);   //LE             
            iBuffer[ ISI_HEADER_OFFSET_LENGTH ] = static_cast<TUint8>(length & 0x00ff);    //LE               
           }
        else
            {              
            iBuffer[ ISI_HEADER_OFFSET_LENGTH +1 ] = static_cast<TUint8>( (calcFinalLength & 0xff00) >> 8);   //LE             
            iBuffer[ ISI_HEADER_OFFSET_LENGTH ] = static_cast<TUint8>(calcFinalLength & 0x00ff);    //LE
            iBuffer.SetLength( iFinalLength );                                
             }
        OstTraceExt2( TRACE_NORMAL, DUP2_TISIKERNELSEND_COMPLETE, "TIsiKernelSend::Complete;byte2=%hhu;byte1=%hhu", iBuffer[5], iBuffer[4] );
        COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Complete, byte2: 0x%x, byte1: 0x%x" ), iBuffer[5], iBuffer[4] ) );
        }
    #endif    

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Complete - return, buffer length = 0x%x, iFinalLength = 0x%x" ), length, iFinalLength ) );
    OstTraceExt2( TRACE_NORMAL, DUP3_TISIKERNELSEND_COMPLETE, "TIsiKernelSend::Complete - return;buffer length=%hu;iFinalLength=%u", length, iFinalLength );
    
    return iBuffer;

    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::CopyData
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::CopyData
        (
        const TUint aOffset,
        const TDesC8& aData
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::CopyData entering, offset = 0x%x" ), aOffset ) );
    OstTraceExt2( TRACE_NORMAL, TISIKERNELSEND_COPYDATA, "TIsiKernelSend::CopyData - enter;aOffset=%u;aData=%x", aOffset, ( TUint )&( aData ) );
    
    TInt length( aData.Length() );
       
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + length ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    
    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + length );     
  
    ASSERT_RESET_ALWAYS( ( ( iBuffer.Length() - length ) >= aOffset ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= length ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
       
    TRACE_ASSERT ( length > 0); // Length should be > 0
    
    // Add byte by byte starting from offset place until the whole data is set.
    for( TInt i( 0 ); i < length; i++ )
        {
        iBuffer[aOffset + i] = aData.Ptr()[ i ];
        }            
        
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::CopyData - return, offset = 0x%x, data length = 0x%x " ), aOffset, length ) );
    OstTraceExt2( TRACE_NORMAL, DUP2_TISIKERNELSEND_COPYDATA, "TIsiKernelSend::CopyData - return;aOffset=%hhu;data length=%hhd", aOffset, length );
        
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::TIsiKernelSend
// -----------------------------------------------------------------------------
EXPORT_C TIsiKernelSend::TIsiKernelSend
        (
        TDes8& aBuf,
        const TUint aLength
        )
        :TIsiKernel(),
        iBuffer( aBuf ),
        iFinalLength( aLength ) // this should be same as msg length
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::TIsiKernelSend entering, aLength = 0x%x " ), aLength ) ) ;
	OstTrace1( TRACE_NORMAL, TISIKERNELSEND_TISIKERNELSEND, "TIsiKernelSend::TIsiKernelSend - enter;aLength=%u", aLength );
	
    // Length check
	ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aLength ), "KernelIsiMessage: invalid offset in constructor", EKernelIsiMessageInvalidOffset);

    // Set the message to contain zeros to it's length.
    iBuffer.FillZ( aLength );

    // Set the length to zero.
    iBuffer.Zero();
    
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::TIsiKernelSend - return. iFinalLength = 0x%x" ), iFinalLength ) ) ;
    OstTrace1( TRACE_NORMAL, DUP1_TISIKERNELSEND_TISIKERNELSEND, "TIsiKernelSend::TIsiKernelSend - return;iFinalLength=%u", iFinalLength );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::TIsiKernelSend alternative constructor
// -----------------------------------------------------------------------------
EXPORT_C TIsiKernelSend::TIsiKernelSend
        (
        TDes8& aBuf
        )
        :TIsiKernel(),
        iBuffer( aBuf ),
        iFinalLength( 0 )
    {

    OstTrace0( TRACE_NORMAL, DUP2_TISIKERNELSEND_TISIKERNELSEND, "TIsiKernelSend::TIsiKernelSend constructor without length - enter" );
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::TIsiKernelSend constructor without length, entering" ) ) );
    
    // Set the message to contain zeros to it's max length.
    iBuffer.FillZ( iBuffer.MaxLength() );

    // Set the length to zero.
    iBuffer.Zero();
    
    OstTrace1( TRACE_NORMAL, DUP3_TISIKERNELSEND_TISIKERNELSEND, "TIsiKernelSend::TIsiKernelSend constructor without length - return;iFinalLength=%u", iFinalLength );
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::TIsiKernelSend constructor without length - return. iFinalLength = 0x%x" ), iFinalLength ) ) ;
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::Set16bit
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::Set16bit
        (
        const TUint aOffset,
        const TUint16 aData
        )
    {
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Set16bit entering - offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, TISIKERNELSEND_SET16BIT, "TIsiKernelSend::Set16bit - enter;aOffset=%u;aData=%hu", aOffset, aData );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + 2 ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + 2 ); 
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() -1 >= aOffset ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    
    TRACE_ASSERT ( aOffset % 2 == 0 ); // Invalid offset
    
    iBuffer[ aOffset ] = static_cast<TUint8>( (aData & 0xff00) >> 8);
    iBuffer[ aOffset + 1 ] = static_cast<TUint8>(aData & 0x00ff);

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Set16bit - return, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISIKERNELSEND_SET16BIT, "TIsiKernelSend::Set16bit - return;aOffset=%u;aData=%hu", aOffset, aData );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::Set32bit
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::Set32bit
        (
        const TUint aOffset,
        const TUint32 aData
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Set32bit entering - offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, TISIKERNELSEND_SET32BIT, "TIsiKernelSend::Set32bit - enter;aOffset=%u;aData=%u", aOffset, ( TUint )( aData ) );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + 4  ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );  

    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + 4 ); 
   
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() -3 >= aOffset ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    TRACE_ASSERT ( aOffset % 4 == 0 ); // Invalid offset

    iBuffer[ aOffset ]     = static_cast<TUint8>( (aData & 0xff000000) >> 24);
    iBuffer[ aOffset + 1 ] = static_cast<TUint8>( (aData & 0x00ff0000) >> 16);
    iBuffer[ aOffset + 2 ] = static_cast<TUint8>( (aData & 0x0000ff00) >> 8);
    iBuffer[ aOffset + 3 ] = static_cast<TUint8>(aData & 0x000000ff);

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Set32bit - return, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISIKERNELSEND_SET32BIT, "TIsiKernelSend::Set32bit - return;aOffset=%u;aData=%u", aOffset, ( TUint )( aData ) );    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::Set8bit
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::Set8bit
        (
        const TUint aOffset,
        const TUint8 aData
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Set8bit entering - offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, TISIKERNELSEND_SET8BIT, "TIsiKernelSend::Set8bit - enter;aOffset=%u;aData=%hhu", aOffset, aData );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= aOffset + 1 ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    
    if( iBuffer.Length() <= aOffset ) iBuffer.SetLength( aOffset + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= aOffset ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
  
    iBuffer[ aOffset ] = aData;    

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::Set8bit - return, offset = 0x%x, aData = 0x%x" ), aOffset, aData ) );
    OstTraceExt2( TRACE_NORMAL, DUP1_TISIKERNELSEND_SET8BIT, "TIsiKernelSend::Set8bit - return;aOffset=%u;aData=%hhu", aOffset, aData );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::SetReceiverDevice
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::SetReceiverDevice
        (
        const TUint8 aReceiverDevice
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetReceiverDevice entering - receiverDevice = 0x%x" ), aReceiverDevice ) );
    OstTraceExt1( TRACE_NORMAL, TISIKERNELSEND_SETRECEIVERDEVICE, "TIsiKernelSend::SetReceiverDevice - enter;aReceiverDevice=%hhu", aReceiverDevice );
    
    //Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_RECEIVERDEVICE ) iBuffer.SetLength( ISI_HEADER_OFFSET_RECEIVERDEVICE + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_RECEIVERDEVICE ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] = SET_RECEIVER_DEV(iBuffer, aReceiverDevice);
        
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetReceiverDevice return - set receiverDevice = 0x%x "), iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELSEND_SETRECEIVERDEVICE, "TIsiKernelSend::SetReceiverDevice - return;aReceiverDevice=%hhu", iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::SetSenderDevice
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::SetSenderDevice
        (
        const TUint8 aSenderDevice
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetSenderDevice entering - senderDevice = 0x%x" ), aSenderDevice ) );
    OstTraceExt1( TRACE_NORMAL, TISIKERNELSEND_SETSENDERDEVICE, "TIsiKernelSend::SetSenderDevice - enter;aSenderDevice=%hhu", aSenderDevice );
    
    //Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_SENDERDEVICE ) iBuffer.SetLength( ISI_HEADER_OFFSET_SENDERDEVICE + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_SENDERDEVICE ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] = SET_SENDER_DEV( iBuffer, aSenderDevice );
        
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetSenderDevice return - senderDevice = 0x%x "),  iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] ) );        
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELSEND_SETSENDERDEVICE, "TIsiKernelSend::SetSenderDevice - return;aSenderDevice=%hhu", iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::SetReceiverObject
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::SetReceiverObject
        ( 
        const TUint16 aReceiverObject
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetReceiverObject entering- receiverObject= 0x%x" ), aReceiverObject ) );
    OstTraceExt1( TRACE_NORMAL, TISIKERNELSEND_SETRECEIVEROBJECT, "TIsiKernelSend::SetReceiverObject - enter;aReceiverObject=%hu", aReceiverObject );
    
    // Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    ASSERT_RESET_ALWAYS( ( aReceiverObject < 0x3FF ), "KernelIsiMessage: Invalid Receiverobject", KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_RECEIVEROBJECT ) iBuffer.SetLength( ISI_HEADER_OFFSET_RECEIVEROBJECT + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_RECEIVEROBJECT ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    SET_RECEIVER_OBJ(iBuffer, aReceiverObject);

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetReceiverObject - set receiverDevice = 0x%x "), iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] ) );    
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELSEND_SETRECEIVEROBJECT, "TIsiKernelSend::SetReceiverObject;receiverDevice=%hhu", iBuffer[ISI_HEADER_OFFSET_RECEIVERDEVICE] );

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetReceiverObject return - receiverObject = 0x%x "), iBuffer[ISI_HEADER_OFFSET_RECEIVEROBJECT] ) );
    OstTraceExt1( TRACE_NORMAL, DUP2_TISIKERNELSEND_SETRECEIVEROBJECT, "TIsiKernelSend::SetReceiverObject - return;receiverObject=%hhu", iBuffer[ISI_HEADER_OFFSET_RECEIVEROBJECT] );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::SetSenderObject
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::SetSenderObject
        ( 
        const TUint16 aSenderObject
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetSenderObject entering- senderObject= 0x%x" ), aSenderObject ) );
    OstTraceExt1( TRACE_NORMAL, TISIKERNELSEND_SETSENDEROBJECT, "TIsiKernelSend::SetSenderObject - enter;aSenderObject=%hu", aSenderObject );
    
    // Check buffer length and given value
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_MESSAGEID ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    ASSERT_RESET_ALWAYS( ( aSenderObject < 0x3FF ), "KernelIsiMessage: Invalid SenderObject ", KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_SENDEROBJECT ) iBuffer.SetLength( ISI_HEADER_OFFSET_SENDEROBJECT + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_SENDEROBJECT ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );

    SET_SENDER_OBJ(iBuffer, aSenderObject);

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetSenderObject -  senderDevice = 0x%x" ), iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] ) );
    OstTraceExt1( TRACE_NORMAL, DUP1_TISIKERNELSEND_SETSENDEROBJECT, "TIsiKernelSend::SetSenderObject;senderDevice=%hu", iBuffer[ISI_HEADER_OFFSET_SENDERDEVICE] );

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetSenderObject return - senderObject = 0x%x" ), iBuffer[ISI_HEADER_OFFSET_SENDEROBJECT] ) );        
    OstTraceExt1( TRACE_NORMAL, DUP2_TISIKERNELSEND_SETSENDEROBJECT, "TIsiKernelSend::SetSenderObject - return;senderObject=%hu", iBuffer[ISI_HEADER_OFFSET_SENDEROBJECT] );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSend::SetExtendedResourceId
// -----------------------------------------------------------------------------
EXPORT_C void TIsiKernelSend::SetExtendedResourceId
        (
        const TUint32 aExtendedResourceId
        )
    {

    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetExtendedResourceId entering - extendedResourceId = 0x%x" ), aExtendedResourceId ) );
    OstTrace1( TRACE_NORMAL, TISIKERNELSEND_SETEXTENDEDRESOURCEID, "TIsiKernelSend::SetExtendedResourceId - enter;aExtendedResourceId=%u", ( TUint )( aExtendedResourceId ) );
    
    TUint8 prefix( aExtendedResourceId & 0xFF );
                           
    //Check that extended resource id available and not reading over the buffer
    ASSERT_RESET_ALWAYS( ( iBuffer.MaxLength() >= ISI_HEADER_OFFSET_SUBTYPE + 1 ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
    ASSERT_RESET_ALWAYS( ( aExtendedResourceId < 0xFFFFFF ), "KernelIsiMessage: Invalid ExtendedResourceId ", KErrOverflow );
    ASSERT_RESET_ALWAYS( ( prefix == PN_PREFIX ), "KernelIsiMessage: Invalid Pefix ", KErrOverflow );

    if( iBuffer.Length() <= ISI_HEADER_OFFSET_SUBTYPE ) iBuffer.SetLength( ISI_HEADER_OFFSET_SUBTYPE + 1 );
    
    ASSERT_RESET_ALWAYS( ( iBuffer.Length() >= ISI_HEADER_OFFSET_SUBTYPE ), "KernelIsiMessage: OFFSET Invalid", KErrOverflow );
            
    TUint8 type = (( aExtendedResourceId >> 8 ) & 0xFF );       
    TUint8 subType = (( aExtendedResourceId >> 16 ) & 0xFF );                                       

    iBuffer[ ISI_HEADER_OFFSET_TYPE ] = type;
    iBuffer[ ISI_HEADER_OFFSET_SUBTYPE ] = subType; 
    iBuffer[ ISI_HEADER_OFFSET_RESOURCEID ] = prefix;
                
    COMPONENT_TRACE( ( _T ( "TIsiKernelSend::SetExtendedResourceId return- type = 0x%x, subtype = 0x%x, prefix = 0x%x" ), type, subType, prefix ) );        
    OstTraceExt3( TRACE_NORMAL, DUP1_TISIKERNELSEND_SETEXTENDEDRESOURCEID, "TIsiKernelSend::SetExtendedResourceId - return;type=%hhu;subType=%hhu;prefix=%hhu", type, subType, prefix );
    
    }

// -----------------------------------------------------------------------------
// TIsiSubBlock::TIsiSubBlock
// -----------------------------------------------------------------------------
EXPORT_C TIsiKernelSubBlock::TIsiKernelSubBlock
        (
        TDes8& aMsg, 
        TUint16 aSbId,
        const TSubBlockType aType
        ):  iSubBlockStart( aMsg.Length() ),
            iMsg(aMsg),
            iSubBlockType(aType)
    {
    COMPONENT_TRACE( ( _T ( "TIsiKernelSubBlock::TIsiKernelSubBlock entering - sbId = 0x%x, subBlockType = %d" ), aSbId, aType ) );       
    OstTraceExt2( TRACE_NORMAL, DUP1_TISIKERNELSUBBLOCK_TISIKERNELSUBBLOCK, "TIsiKernelSubBlock::TIsiKernelSubBlock - enter;aSbId=%hu;subblockType=%x", aSbId, ( TUint )&( aType ) );
    
    
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
            byte1 = TUint8( ( aSbId & 0xff00 ) >> 8 );
            byte2 = TUint8( aSbId & 0x00ff ); 
            iMsg.Append(byte1); // sbId 16 bit
            iMsg.Append(byte2); // sbId 16 bit
            iMsg.Append(0x00); // lenght 8 bit
            break;
            }
        case EIsiSubBlockTypeId16Len16:
            {
            byte1 = TUint8( ( aSbId & 0xff00 ) >> 8 );
            byte2 = TUint8( aSbId & 0x00ff );
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
            ASSERT_RESET_ALWAYS( 0, "KernelIsiMessage: Invalid subblock type", EKernelIsiMessageInvalidValue);
            break;
            }
        }

    COMPONENT_TRACE( ( _T ( "TIsiKernelSubBlock::TIsiKernelSubBlock - return" ) ) );       
    OstTrace0( TRACE_NORMAL, DUP2_TISIKERNELSUBBLOCK_TISIKERNELSUBBLOCK, "TIsiKernelSubBlock::TIsiKernelSubBlock - return" );
    
    }

// -----------------------------------------------------------------------------
// TIsiKernelSubBlock::CompleteSubBlock
// -----------------------------------------------------------------------------
EXPORT_C TDes8& TIsiKernelSubBlock::CompleteSubBlock
        (
        ) 
    {
    COMPONENT_TRACE( ( _T ( "TIsiKernelSubBlock::CompleteSubBlock" ) ) );       
    OstTrace0( TRACE_NORMAL, TISIKERNELSUBBLOCK_COMPLETESUBBLOCK, "TIsiKernelSubBlock::CompleteSubBlock - enter" );
    
    TUint8 byte1( 0x00 );
    TUint8 byte2( 0x00 );
    TUint8 byte3( 0x00 );
    TUint8 byte4( 0x00 );
    TUint16 length16(0x0000);
    TUint32 length32(0x00000000);

    // Make sb length divisible by 4
    while( (iMsg.Length() -iSubBlockStart ) % 4 )
        {
        iMsg.Append( KPaddingByte );
        }

    // Update subblock length
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
            byte1 = TUint8( ( length16 & 0xff00 ) >> 8 );
            byte2 = TUint8( length16 & 0x00ff );
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
            byte1 = TUint8( ( length16 & 0xff00 ) >> 8 );
            byte2 = TUint8( length16 & 0x00ff );
            iMsg[iSubBlockStart+2] = byte1;
            iMsg[iSubBlockStart+3] = byte2;
            break;
            }
        case EIsiSubBlockTypeId8Len32:
            {
            length32 = TUint32( iMsg.Length()-iSubBlockStart );
            byte1 = TUint8( ( length32 & 0xff000000 ) >> 24 );
            byte2 = TUint8( ( length32 & 0x00ff0000 ) >> 16 );
            byte3 = TUint8( ( length32 & 0x0000ff00 ) >> 8 );
            byte4 = TUint8( length32 & 0x000000ff );
            iMsg[iSubBlockStart+4] = byte1;
            iMsg[iSubBlockStart+5] = byte2;
            iMsg[iSubBlockStart+6] = byte3;
            iMsg[iSubBlockStart+7] = byte4;
            break;
            }
        default:
            {
            ASSERT_RESET_ALWAYS( 0, "KernelIsiMessage: Invalid subblock type", EKernelIsiMessageInvalidValue);
            break;
            }
        }
        
    COMPONENT_TRACE( ( _T ( "TIsiKernelSubBlock::CompleteSubBlock - return" ) ) );       
    OstTrace0( TRACE_NORMAL, DUP1_TISIKERNELSUBBLOCK_COMPLETESUBBLOCK, "TIsiKernelSubBlock::CompleteSubBlock - return" );
    
    return iMsg;

    }

// ---------------------------------------------------------
// DECLARE_STANDARD_EXTENSION
//
// The entrypoint of KernelIsiMessage
//
// ---------------------------------------------------------
DECLARE_STANDARD_EXTENSION()
    {
    COMPONENT_TRACE( ( _T( "KERNELISIMESSAGE ENTERING" ) ) );
    OstTrace0( TRACE_NORMAL, _DECLARE_STANDARD_EXTENSION, "KernelIsiMessage entering::DECLARE_STANDARD_EXTENSION" );
    
    COMPONENT_TRACE( ( _T( "KERNELISIMESSAGE RETURN" ) ) );
    OstTrace0( TRACE_NORMAL, DUP1__DECLARE_STANDARD_EXTENSION, "KernelIsiMessage returning::DECLARE_STANDARD_EXTENSION" );
    
    return KErrNone;
    }

//end of file
