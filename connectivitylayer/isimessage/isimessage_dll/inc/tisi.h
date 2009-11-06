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


#ifndef TISI_H
#define TISI_H


// INCLUDE FILES
#include <e32std.h>
#include <phonetisi.h>


// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
//None


// FORWARD DECLARATIONS
class TIsiSend;
class TIsiReceiveC;
class TIsiSubBlock;
class TIsiUtility;
class TIsi;


enum TSubBlockType
{

    EIsiSubBlockTypeId8Len8 = 1,
    EIsiSubBlockTypeId8Len16,
    EIsiSubBlockTypeId16Len8,
    EIsiSubBlockTypeId16Len16,
    EIsiSubBlockTypeId8Len32

};



// CLASS DECLARATION

/**
*  TIsi
*  This class implements the TIsi
*/
class TIsi
{

    // Functions
    public:

        /**
        * Copies data into this TIsi object replacing any existing data.
        * @param TIsi&
        * @return TIsi&
        */
        IMPORT_C TIsi& operator=( const TIsi& );

    protected:

        /**
        * Default C++ constructor.
        * @param None
        * @return 
        */
        inline TIsi(){}; 


            
    // Data
    protected:


};

class TIsiReceiveC : public TIsi
{

    // Functions
    public:

        /**
        * Used to receive a message, subblock or data.
        * @param const TDesC8& aBuf
        */
        IMPORT_C TIsiReceiveC( const TDesC8& aBuf );

        /**
        * Get 16-bit data (TUint16) from message.
        * @param const TUint aOffset
        * @return TUint16
        */
        IMPORT_C TUint16 Get16bit( const TUint aOffset ) const;

        /**
        * Get 32-bit data (TUint32) from message.
        * @param const TUint aOffset
        * @return TUint32
        */
        IMPORT_C TUint32 Get32bit( const TUint aOffset ) const;

        /**
        * Get 8-bit data (TUint8) from message.
        * @param const TUint aOffset
        * @return TUint8
        */
        IMPORT_C TUint8 Get8bit( const TUint aOffset ) const;

        /**
        * Get TPtr8 descriptor to message data. 
        * @param const TUint aOffset
        * @param const TInt aLength
        * @return const TPtrC8
        */
        IMPORT_C const TPtrC8 GetData( const TUint aOffset, const TInt aLength ) const; 

        /**
        * Find subblock offset within message by using subblock id.
        * @param TUint aSbStartOffset, TUint aSubblockId, TSubBlockType aType, TUint &aSubBlockOffset
        * @return TInt
        */
       IMPORT_C TInt FindSubBlockOffsetById ( const TUint aSbStartOffset,
                                         const TUint aSubblockId,
                                         const TSubBlockType aType,
                                         TUint &aSubBlockOffset ) const;

        /**
        * Find subblock offset within message by using subblock index
        * @param TUint aSbStartOffset, TUint aSubblockId, TSubBlockType aType, TUint &aSubBlockOffset
        * @return TInt
        */
        IMPORT_C TInt FindSubBlockOffsetByIndex( const TUint aSbStartOffset,
                                            const TUint aSubblockIndex,
                                            const TSubBlockType aType,
                                            TUint &aSubBlockOffset ) const;

        /**
        * Get TPtrC8 descriptor to whole isi message.
        * @return const TPtrC8
        */
        IMPORT_C const TPtrC8 GetBuffer() const;

        /**
        * Get 6-bit receiver device from message.
        * @return TUint8
        */
        IMPORT_C TUint8 GetReceiverDevice( ) const;

        /**
        * Get 6-bit sender device from message.
        * @return TUint8
        */
        IMPORT_C TUint8 GetSenderDevice( ) const;

        /**
        * Gets 10-bit receiver object from message.
        * @return TUint16
        */
        IMPORT_C TUint16 GetReceiverObject( ) const;

        /**
        * Gets 10-bit sender object from message.
        * @return TUint16
        */
        IMPORT_C TUint16 GetSenderObject( ) const;

        /**
        * Gets 24-bit extended resource id from message.
        * @return TUint32
        */
        IMPORT_C TUint32 GetExtendedResourceId( ) const;



    // Data
    protected:

        // For message.
        const TDesC8&       iBuffer;

};

class TIsiSend : public TIsi
{
    
    // Functions
    public:

        /**
        * Adds message length and check fillers to the end of message,
        * message buffer content and length are modified
        * Must be used when creating a new message to be send.
        * Checks also that critical information for the message is set.
        * Critical information:Resource, MessageId.
        * Returns the reference to the message to be send.
        * @param None
        * @return TDes8&
        */
        IMPORT_C TDes8& Complete();

        /**
        * Copy descriptor to message data,
        * message buffer content and length are modified.
        * @param const TUint aOffset
        * @param const TDesC8& aData
        * @return none
        */
        IMPORT_C void CopyData( const TUint aOffset, const TDesC8& aData );

        /**
        * Used to generate a message, subblock or datastruct. 
        * Descriptor aBuf content is filled with zeroes
        * and length is also reset to zero.
        * @param TDes8& aBuf
        * @param const TUint aLength
        */
        IMPORT_C TIsiSend( TDes8& aBuf, const TUint aLength );

        /**
        * Used to generate a message, subblock or datastruct. 
        * Descriptor aBuf content is filled with zeroes
        * and length is also reset to zero.        
        * @param TDes8& aBuf
        */
        IMPORT_C TIsiSend( TDes8& aBuf );

        /**
        * Set 16-bit data (TUint16) to message,
        * message buffer content and length are modified.
        * @param const TUint aOffset
        * @param const TUint16 aData
        * @return none
        */
        IMPORT_C void Set16bit( const TUint aOffset, const TUint16 aData );

        /**
        * Set 32-bit data (TUint) to message,
        * message buffer content and length are modified.
        * @param const TUint aOffset
        * @param const TUint32 aData
        * @return none
        */
        IMPORT_C void Set32bit( const TUint aOffset, const TUint32 aData );

        /**
        * Set 8-bit (TUint8) data to message,
        * message buffer content and length are modified.
        * @param const TUint aOffset
        * @param const TUint8 aData
        * @return none
        */
        IMPORT_C void Set8bit( const TUint aOffset, const TUint8 aData );

        /**
        * Set 6-bit (TUint8) receiver device to message,
        * message buffer content and length are modified.
        * @param const TUint8 aReceiverDevice
        * @return none
        */
        IMPORT_C void SetReceiverDevice( const TUint8 aReceiverDevice );        

        /**
        * Set 6-bit (TUint8) sender device to message,
        * message buffer content and length are modified.
        * @param const TUint8 aSenderDevice
        * @return none
        */
        IMPORT_C void SetSenderDevice( const TUint8 aSenderDevice );

        /**
        * Set 10-bit (TUint16) data to message,
        * message buffer content and length are modified.
        * @param const TUint16 aReceiverObject
        * @return none
        */
        IMPORT_C void SetReceiverObject( const TUint16 aReceiverObject );

        /**
        * Set 10-bit (TUint16) data to message,
        * message buffer content and length are modified.
        * @param const TUint16 aSenderObject
        * @return none
        */
        IMPORT_C void SetSenderObject( const TUint16 aSenderObject );

        /**
        * Set 24-bit (TUint32) extended resource id to message,
        * message buffer content and length are modified.
        * @param const TUint32 aExtendedResourceId
        * @return none
        * In the resource field in the PhoNet header is set to a fix value called PN_PREFIX telling 
        * that additional resource information can be found in the Type and SubType fields as follows:
        * Media	Receiver Sender	PN_PREFIX Length Rec.Obj Send.Obj UTID MsgID Type SubType
        */
        IMPORT_C void SetExtendedResourceId ( const TUint32 aExtendedResourceId );

    // Data
    protected:

        // For message.
        TDes8&      iBuffer;

    private:
        TUint       iFinalLength;

};

/**
*  TIsiSubBlock
*  This class implements the TIsiSubBlock
*/
class TIsiSubBlock
    {
    public:
        /**
        * Used to generate a subblock
        * @param TDes8& aMsg
        * @param TUint16 aSbId
        * @param const TSubBlockType aType
        */
        IMPORT_C TIsiSubBlock( TDes8& aMsg, TUint16 aSbId, const TSubBlockType aType );

        /**
        * Adds message length and check fillers to the end of message.
        * Returns the reference to the message.
        * @return TDes8&
        */
        IMPORT_C TDes8& CompleteSubBlock();

    protected:
        TInt   iSubBlockStart;
        TDes8& iMsg;
        TSubBlockType iSubBlockType;
        
    };

/**
*  TIsiUtility
*  This class implements the TIsiUtility
*/
class TIsiUtility
    {

    public:
        /**
        * Used for copying from unicode strings
        * @param const TDesC8& aSource
        * @param TDes16& aTarget
        */
        static IMPORT_C void CopyFromBigEndian( const TDesC8& aSource, TDes16& aTarget);

        /**
        * Used for copying to unicode strings
        * @param const TDesC16& aSource
        * @param TDes8& aTarget
        */
        static IMPORT_C void CopyToBigEndian( const TDesC16& aSource, TDes8& aTarget );

    protected:

    };

#endif
