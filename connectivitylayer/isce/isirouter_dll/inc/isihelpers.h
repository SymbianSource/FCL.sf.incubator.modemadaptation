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



#ifndef ISI_HELPERS_H
#define ISI_HELPERS_H
//  INCLUDES
#include <pn_const.h>                   // For PN_HEADER_SIZE
#include <commisi.h>                    // For SIZE_COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP
#include <phonetisi.h>                  // For ISI_HEADER_SIZE


#ifndef PN_DEV_OWN
#define PN_DEV_OWN 0 // PN_DEV_HOST
#endif // PN_DEV_OWN

#ifndef PN_OBJ_ROUTING_REQ
#define PN_OBJ_ROUTING_REQ 0x00
#endif // PN_OBJ_ROUTING_REQ

#ifndef PN_MEDIA_ROUTING_REQ
#define PN_MEDIA_ROUTING_REQ 0x00
#endif // PN_MEDIA_ROUTING_REQ

#ifndef PN_OBJ_ROUTER
#define PN_OBJ_ROUTER PN_OBJ_ROUTING_REQ
#endif // PN_OBJ_ROUTER


#ifndef PN_DEV_MODEM
#define PN_DEV_MODEM        0x60
#endif // PN_DEV_MODEM

#ifndef PN_MEDIA_MODEM_HOST_IF
#define PN_MEDIA_MODEM_HOST_IF  0x26 // Media to/from PN_DEV_MODEM
#endif // PN_MEDIA_MODEM_HOST_IF

#ifndef PN_DEV_PC
#define PN_DEV_PC       0x10
#endif // PN_MEDIA_TEST

// not real only for testing
#ifndef PN_DEV_DUMMYIST
// SET equla to PN_DEV_TEST 0x3c
#define PN_DEV_DUMMYIST     0x3c 
#endif // PN_DEV_DUMMYIST

#ifndef PN_MEDIA_TEST
#define PN_MEDIA_TEST       0xBB
#endif // PN_MEDIA_TEST
// not real

// <- end 

// MACROS

#if defined ( __WINS__ )
#define MESSAGELENGTH_LSB 5
#define MESSAGELENGTH_MSB 4
#else // __WINS__
#ifndef ISI_LENGTH_BIG_ENDIAN
#define MESSAGELENGTH_LSB 4
#define MESSAGELENGTH_MSB 5
#else  //ISI_LENGTH_BIG_ENDIAN
#define MESSAGELENGTH_LSB 5
#define MESSAGELENGTH_MSB 4
#endif // ISI_LENGTH_BIG_ENDIAN
#endif // __WINS__

#define IS_DEV_HOST( dev )            ( ( dev & 0xF0 ) == 0 )
#define GET_RECEIVER_DEV( msg )       ( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0xFC )
#define GET_SENDER_DEV( msg )         ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0xFC )
//OK (WINS?)
#define GET_RECEIVER_OBJ( msg )       ( IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] ) ? \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] + ( ( msg [ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0x0F ) << 8 ) ) : \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] + ( ( msg [ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0x03 ) << 8 ) ) )
//OK (WINS?)
#define GET_SENDER_OBJ( msg )         ( IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] ) ? \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_SENDEROBJECT ] + ( ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0x0F ) << 8 ) ) : \
                                      ( ( TUint16 )msg[ ISI_HEADER_OFFSET_SENDEROBJECT ] + ( ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0x03 ) << 8 ) ) )
#define SET_RECEIVER_DEV( msg, dev )  ( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE] = ( ( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] & 0x03 ) | ( ( dev ) & 0xFC ) ) )
#define SET_SENDER_DEV( msg, dev )    ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE] = ( ( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] & 0x03 ) | ( ( dev ) & 0xFC ) ) )
#define SET_RECEIVER_OBJ( msg, o )    msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] = ( IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] ) ) ? \
                                      GET_RECEIVER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x0F ) : GET_RECEIVER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x03 ), \
                                      msg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] = ( TUint8 )( ( o ) & 0x00FF )
#define SET_SENDER_OBJ( msg, o )      msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] = ( IS_DEV_HOST( msg[ ISI_HEADER_OFFSET_SENDERDEVICE ] ) ) ? \
                                      GET_SENDER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x0F ) : GET_SENDER_DEV( msg ) | ( ( ( o ) >> 8 ) & 0x03 ), \
                                      msg[ ISI_HEADER_OFFSET_SENDEROBJECT ] = ( TUint8 )( ( o ) & 0x00FF )

#define SET_LENGTH( msg,len )         msg[ MESSAGELENGTH_MSB ] = ( TUint8 )( ( len ) >> 8 );msg[ MESSAGELENGTH_LSB ] = ( TUint8 )( len );
#define GET_LENGTH( msg )             ( ( ( TUint16 )( ( msg[ MESSAGELENGTH_MSB ] ) << 8 ) ) | msg[ MESSAGELENGTH_LSB ] )


/* PUTB8 macro writes byte value to given address.
 * This macro is used mainly by other PUTBXX macros.
 */
#define PUTB8(p,v) \
    {(*(TUint8 *)(p)) = ((TUint8)(v));}


/* PUTB16 macro writes 16-bit value in Big Endian format
 * to given address. 16-bit value is written as two separate
 * bytes, and so this macro can write 16-bit value to whatever
 * address, regardless of the processor alignment restrictions
 */
#define PUTB16(p,v) \
    {PUTB8((p),(TUint16)(v)>>8); PUTB8((TUint8*)(p)+1,v);}


/* PUTB32 macro writes 32-bit value in Big Endian format
 * to given address. 32-bit value is written as four separate
 * bytes, and so this macro can write 32-bit value to whatever
 * address, regardless of the processor alignment restrictions
 */
#define PUTB32(p,v) \
    {PUTB16((p),(TUint32)(v)>>16); PUTB16((TUint8*)(p)+2,(TUint32)(v));}


/**
 *    Big Endian to local endian type
 */
 /* GETB8 macro returns byte value from given address.
 * This macro is used mainly by other GETBXX macros.
 */
#define GETB8(p) \
    (*(TUint8 *)(p))


/* GETB16 macro reads 16-bit value in Big Endian format
 * from given address. 16-bit value is read as two separate
 * bytes, and so this macro can read 16-bit value from whatever
 * address, regardless of the processor alignment restrictions
 */
#define GETB16(p) \
    (((TUint16) GETB8(p)<<8) | (TUint16) GETB8((TUint8 *)(p)+1))


/* GETB32 macro reads 32-bit value in Big Endian format
 * from given address. 32-bit value is read as four separate
 * bytes, and so this macro can read 32-bit value from whatever
 * address, regardless of the processor alignment restrictions
 */
#define GETB32(p) \
    (((TUint32) GETB16(p)<<16) | (TUint32) GETB16((TUint8 *)(p)+2))


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif // ISI_HELPERS_H
// End of File
