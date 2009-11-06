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



#ifndef IAD_HELPERS_H
#define IAD_HELPERS_H
//  INCLUDES

// MACROS

#if defined ( __WINS__ )
#define MESSAGELENGTH_LSB 5
#define MESSAGELENGTH_MSB 4
#else
#define MESSAGELENGTH_LSB 4
#define MESSAGELENGTH_MSB 5
#endif

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

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

#endif // IAD_HELPERS_H
// End of File
