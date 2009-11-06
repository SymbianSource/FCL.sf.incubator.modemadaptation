/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include "ber_tlv.h"        // ber-tlv classes
#include "satutil.h"        // sat utility class

#include "tsylogger.h"      // for logging
#include <tisi.h>           // isi message
#include <uiccisi.h>        // UICC server
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "ber_tlvtraces.h"
#endif


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TTlvBase::Begin
// Start ber-tlv construction
// -----------------------------------------------------------------------------
//
void TTlvBase::Begin
        (
        TUint8 aTag //tag
        )
    {
    OstTrace0( TRACE_DETAILED, TTLVBASE_BEGIN, "TTlvBase::Begin" );
    TFLOGSTRING("TSY: TTlvBase::Begin");
    iData.Zero();
    iData.Append( aTag );
    iData.Append( 0 );
    }

// -----------------------------------------------------------------------------
// TTlvBase::AddTag
// Start new simple tlv
// -----------------------------------------------------------------------------
//
void TTlvBase::AddTag
        (
        TUint8 aTag //tag
        )
    {
    OstTrace0( TRACE_DETAILED, TTLVBASE_ADDTAG, "TTlvBase::AddTag" );
    TFLOGSTRING("TSY: TTlvBase::AddTag");
    iData.Append( aTag );           // tag
    iData.Append( 0 );              // length
    iLenIndex = iData.Length() - 1; // index to tag length
    }

// -----------------------------------------------------------------------------
// TTlvBase::AddByte
// Add byte data to simple tlv
// -----------------------------------------------------------------------------
//
void TTlvBase::AddByte
        (
        TUint8 aValue
        )
    {
    OstTrace0( TRACE_DETAILED, TTLVBASE_ADDBYTE, "TTlvBase::AddByte" );
    TFLOGSTRING("TSY: TTlvBase::AddByte");
    iData.Append( aValue );
    iData[iLenIndex]++;
    }

// -----------------------------------------------------------------------------
// TTlvBase::End
// Add desctiptor data to simple tlv
// -----------------------------------------------------------------------------
//
void TTlvBase::AddData
        (
        const TDesC8& aValue
        )
    {
    OstTrace0( TRACE_DETAILED, TTLVBASE_ADDDATA, "TTlvBase::AddData" );
    TFLOGSTRING("TSY: TTlvBase::AddData");
    iData.Append( aValue );
    iData[iLenIndex] = TUint8(iData[iLenIndex] + aValue.Length());
    }


// -----------------------------------------------------------------------------
// TTlvBase::End
// End ber-tlv construction. Adds extra length byte to all simple
// TLVs that are longer than KMaxOneByteLength (0x7f) and
// returns descriptor to the new valid ber-tlv.
// -----------------------------------------------------------------------------
//
const TDesC8& TTlvBase::End()
    {
    OstTrace0( TRACE_DETAILED, TTLVBASE_END, "TTlvBase::End" );
    TFLOGSTRING("TSY: TTlvBase::End");

    _LIT8( KLenTag, "\x81" );

    TInt i( 3 ); // index to tag length

    // scan through all lenghts and insert 'length tag'
    while ( i < iData.Length() )
        {
        if ( KMaxOneByteLength < iData[i] )
            {
            iData.Insert( i, KLenTag );
            i++;
            }
        i += iData[i] + 2; // jump to next tag length
        }

    // calculate total ber-tlv length
    iData[1] = TUint8( iData.Length() - 2 );
    // insert 'length tag'
    if ( KMaxOneByteLength < iData[1] )
        {
        iData.Insert( 1, KLenTag );
        }

    // done, return ber-tlv
    return iData;
    }

// -----------------------------------------------------------------------------
// TTlvBase::GetDataWithoutTopLevelTag
// End TLV construction. Adds extra length byte to all simpleV.
// TLVs that are longer than KMaxOneByteLength (0x7f) and
// returns descriptor to the valid tlvs. To be used without
// Begin(), nor End() in situations where TLV simple tags are
// used without top level tag.
// -----------------------------------------------------------------------------
//
const TDesC8& TTlvBase::GetDataWithoutTopLevelTag()
    {
    OstTrace0( TRACE_DETAILED, TTLVBASE_GETDATAWITHOUTTOPLEVELTAG, "TTlvBase::GetDataWithoutTopLevelTag" );
    TFLOGSTRING("TSY: TTlvBase::GetDataWithoutTopLevelTag");

    _LIT8( KLenTag, "\x81" );

    TInt i( 1 ); // index to tag length

    // scan through all lenghts and insert 'length tag'
    while ( i < iData.Length() )
        {
        if ( KMaxOneByteLength < iData[i] )
            {
            iData.Insert( i, KLenTag );
            i++;
            }
        i += iData[i] + 2; // jump to next tag length
        }

    // done, return ber-tlv
    return iData;

    }

// -----------------------------------------------------------------------------
// CBerTlv::CBerTlv
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBerTlv::CBerTlv()
    {
    OstTrace0( TRACE_DETAILED, CBERTLV_CBERTLV, "CBerTlv::CBerTlv" );
    TFLOGSTRING("TSY: CBerTlv::CBerTlv");
    //none
    }

// -----------------------------------------------------------------------------
// CBerTlv::TlvByTagValue
// Find TLV by tag value. aTlv is set to point to found TLV.
// If there are more than one TLV with the same tag then
// itemNbr can be used to select which one is returned.
// If no tags are found return KErrNotFound else KErrNone.
// -----------------------------------------------------------------------------
//
TInt CBerTlv::TlvByTagValue
        (
        CTlv* aTlv,                 //TLV
        TUint8 aTlvTagValue,        //TLV identifier
        TInt aItemNbr                //item number
        )
    {
    OstTrace0( TRACE_DETAILED, CBERTLV_TLVBYTAGVALUE, "CBerTlv::TlvByTagValue" );
    TFLOGSTRING("TSY: CBerTlv::TlvByTagValue");
    TInt currentTlv( 0 );
    TInt tlvLength( 0 );
    TInt index( 0 );

    //tlv start index
    TInt ind( KCommonTlvHeaderLength );
    TUint8 berTlvLength( 0 );
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        berTlvLength = iData[KTlvLengthStartPosition + 1];
        ind = ind + 1;
        }
    else
        {
        berTlvLength = iData[KTlvLengthStartPosition];
        }

    for ( TInt i = 0; i < berTlvLength; i += tlvLength )
        {
        //first determine if the length of the TLV is coded with 1 or 2 bytes.
        if ( KTwoByteLengthCoding == iData[ind + 1] )
            {
            //length is coded with 2 bytes -> 1 extra byte required to be
            //added to total length. Also TLV header bytes (2) must be added
            //to total length
            tlvLength = iData[ind + 2] + KTlvHeaderLength + 1;

            }
        else
            {
            //TLV header bytes (2) must be added to total length
            tlvLength = iData[ind + 1] + KTlvHeaderLength;
            }

        currentTlv = iData[ind]&KTagValueMask;

        //check if TLV in current index is the one that we are searching for
        if ( aTlvTagValue == currentTlv )
            {
            if( index == aItemNbr )
                {
                // check that len of tlv object to be search doesn't
                // go outside BER-TLV object
                if ( ( ind + tlvLength ) <= GetSize() )
                    {
                    // len of tlv object to be search is inside
                    // len of BER-TLV object
                    TPtrC8 data( iData.Mid( ind , tlvLength ) );
                    aTlv->SetData( data );
                    return KErrNone;
                    }
                else
                    {
                    // len of tlv object to be search goes
                    // over len of BER-TLV object, let's return
                    // error value.
                    return KErrNotFound;
                    }
                }
            else
                {
                index++;
                }
            }
        ind += tlvLength;

        }

    return KErrNotFound;

    }

// -----------------------------------------------------------------------------
// CBerTlv::TlvByTagValueMulti
// Find several TLVs by tag value. It is assumed that TLVs are
// sequential. aTlv is set to point to TLV sequence.
// If no tags are found return KErrNotFound else KErrNone.
// -----------------------------------------------------------------------------
//
TInt CBerTlv::TlvByTagValueMulti
        (
        CTlv* aTlv,                 //TLV to fill
        TUint8 aTlvTagValue         // tag of tlv to find
        )
    {
    OstTrace0( TRACE_DETAILED, CBERTLV_TLVBYTAGVALUEMULTI, "CBerTlv::TlvByTagValueMulti" );
    TFLOGSTRING("TSY: CBerTlv::TlvByTagValueMulti");

    TInt currentTlv( 0 );
    TInt16 tlvLength( 0 );
    TInt indMulti( 0 );
    TInt16 multiLength( 0 );
    //tlv start index
    TInt ind( KCommonTlvHeaderLength );
    TUint8 berTlvLength;
    //first determine if the length of the BerTLV is coded with 1 or 2 bytes.
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        berTlvLength = iData[KTlvLengthStartPosition + 1];
        ind = ind + 1;
        }
    else
        {
        berTlvLength = iData[KTlvLengthStartPosition];
        }

    for ( TInt i = 0; i < berTlvLength; i+= tlvLength)
        {
        //first determine if the length of the TLV is coded with 1 or 2 bytes.
        if ( KTwoByteLengthCoding == iData[ind + 1] )
            {
            //length is coded with 2 bytes -> real length is in second byte.
            // first byte is 81 and it is tag for 2 byte length coding.
            tlvLength = (TInt16) ( iData[ind + 0x02] + KTlvHeaderLength + 1 );
            }
        else
            {
            //TLV header bytes (2) must be added to total length
            tlvLength = (TInt16) ( iData[ind + 1]  + KTlvHeaderLength );
            }

        currentTlv = iData[ind]&KTagValueMask;

        //check if TLV in current index is the one that we are searching for
        if ( ( aTlvTagValue == currentTlv ) && ( multiLength == 0x00 ) )
            {
            // save fist tag position
            indMulti = ind;
            multiLength = tlvLength;

            }
        // add length
        if ( ( aTlvTagValue == currentTlv ) && ( indMulti != ind ) )
            {
            multiLength = (TInt16) ( multiLength + tlvLength );
            }

        // if last tag, return
        if ( (i + tlvLength) >= berTlvLength )
            {
            TPtrC8 data = ( iData.Mid( indMulti , multiLength ) );
            aTlv->SetData( data );
            return KErrNone;
            }
        ind += tlvLength;

        }

    return KErrNotFound;

    }


// -----------------------------------------------------------------------------
// CTlv::CTlv
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTlv::CTlv()
    {
    OstTrace0( TRACE_DETAILED, CTLV_CTLV, "CTlv::CTlv" );
    TFLOGSTRING("TSY: CTlv::CTlv");
    //none
    }

// -----------------------------------------------------------------------------
// CTlv::GetShortInfo
// Returns the short info (one byte).
// Returned data depends on data type.
// -----------------------------------------------------------------------------
//
TUint8 CTlv::GetShortInfo
        (
        TTlvSpesificDataType aType //Info spesific data type
        )
    {
    OstTrace0( TRACE_DETAILED, CTLV_GETSHORTINFO, "CTlv::GetShortInfo" );
    TFLOGSTRING("TSY: CTlv::GetShortInfo");

    //information is generally at index 2. NOTE! This method should be optimised
    //after the code maturity is high enough.
    TInt ind ( KTlvDataAreaStartPosition );
    TUint8 offset( 0 );

    //first determine if the length of the TLV is coded with 1 or 2 bytes to
    //determine the offset
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        offset = 1;
        }

    //Start getting the contents of TLV data
    if ( ETLV_TonAndNpi == aType )
        {
        //TON/NPI is coded as for EFADN.
        ind = KTlvDataAreaStartPosition + offset;
        }
    else if ( ETLV_CommandNumber == aType )
        {
        //Command number
        //
        //For contents and coding, see subclause 6.5.1.
        ind = KTlvDataAreaStartPosition + offset;
        }
    else if ( ETLV_TypeOfCommand == aType )
        {
        //Type of command:
        //
        //Contents:
        // The Type of Command specifies the required interpretation of the
        // data objects which follow, and the required ME procedure.
        //
        //Coding:
        // See section 13.4
        // The ME shall respond to reserved values (i.e. values not listed)
        // with the result "Command type not understood".
        ind = KTlvDataAreaStartPosition + offset + 1;
        }
    else if ( ETLV_CommandQualifier == aType )
        {
        //Command Qualifier:
        //
        //Contents:
        // Qualifiers specific to the command. See 3GPP TS 11.14 V8.8.0,
        // chapter 12.6 Command details for more details.
        ind = KTlvDataAreaStartPosition + offset + 2;
        }
    else if ( ETLV_SourceDeviceIdentity == aType )
        {
        //Source device identity
        //
        //Contents:
        // the source device for information held in the data objects which
        // follow.
        //
        //Coding:
        // both Source and Destination device identities are coded as follows:
        //  '01'         = Keypad
        //  '02'         = Display
        //  '03'         = Earpiece
        //  '10' to '17' = Additional Card Reader x (0 to 7). Value assigned
        //                 by ME.
        //  '21' to '27' = Channel x (1 to 7). Value assigned by ME (if class
        //                 "e" is supported).
        //  '81'         = SIM
        //  '82'         = ME
        //  '83'         = Network
        //  All other values are reserved.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_DestinationDeviceIdentity == aType )
        {
        //Destination device identity
        //
        //Contents:
        // the destination device for information held in the data objects
        // which follow.
        //
        // NOTE: Only some combinations of Type of Command, Data Download type
        // and Device identities are allowed defined in clause 14.
        //
        //Coding:
        // both Source and Destination device identities are coded as follows:
        //  '01'         = Keypad
        //  '02'         = Display
        //  '03'         = Earpiece
        //  '10' to '17' = Additional Card Reader x (0 to 7). Value assigned
        //                 by ME.
        //  '21' to '27' = Channel x (1 to 7). Value assigned by ME (if class
        //                 "e" is supported).
        //  '81'         = SIM
        //  '82'         = ME
        //  '83'         = Network
        //  All other values are reserved.
        ind = KTlvDataAreaStartPosition + 1;
        }
    else if ( ETLV_TimeUnit == aType )
        {
        //Time unit
        //
        //Contents:
        // time unit used; minutes, seconds or tenths of seconds.
        //
        //Coding:
        //  '00'    Minutes
        //  '01'    Seconds
        //  '02'    Tenths of seconds
        //  All other values are reserved.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_TimeInteval == aType )
        {
        //Time interval
        //
        //Contents:
        // the length of time required, expressed in units.
        //
        //Coding:
        // The time interval is coded in integer multiples of the time unit
        // used. The range is from 1 unit to 255 units. The encoding is:
        //  '00':    reserved
        //  '01':    1 unit
        //  '02':    2 units
        //   :  :    :
        //  'FF':    255 units
        ind = KTlvDataAreaStartPosition + 1;
        }
    else if ( ETLV_IdentifierOfItem == aType )
        {
        //The identifier is a single byte between '01' and 'FF'. Each item
        //shall have a unique identifier within an Item list.
        ind = KTlvDataAreaStartPosition + offset;
        }
    else if ( ETLV_IdentifierOfItemChosen == aType )
        {
        //The identifier is a single byte between '01' and 'FF', exactly the
        //same as for the Item data object. A null item identifier is
        //coded '00'.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_MinimumLengthOfResponse == aType )
        {
        //The range of length is between '00' and 'FF'. A minimum length coding
        //of '00' indicates that there is no minimum length requirement; a
        //maximum length coding of 'FF' indicates that there is no maximum
        //length requirement. If a fixed length is required the minimum and
        //maximum values are identical.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_MaximumLengthOfResponse == aType )
        {
        //The range of length is between '00' and 'FF'. A minimum length coding
        //of '00' indicates that there is no minimum length requirement; a
        //maximum length coding of 'FF' indicates that there is no maximum
        //length requirement. If a fixed length is required the minimum and
        //maximum values are identical.
        ind = KTlvDataAreaStartPosition + 1;
        }
    else if ( ETLV_GeneralResult == aType )
        {
        //General result
        //
        //Contents:
        // General result specifies the result and indicates appropriate SIM
        // action. See 3GPP TS 11.14 V8.8.0, chapter 12.12 Result for more
        // details.
        ind = KTlvDataAreaStartPosition + offset;
        }
    else if ( ETLV_DataCodingScheme == aType )
        {
        //Data coding scheme is coded as for SMS Data coding scheme defined
        //in TS 23.038 [5].
        ind = KTlvDataAreaStartPosition + offset;
        }
    else if ( ETLV_Tone == aType )
        {
        //Tone
        //
        //Contents:
        // Tones can be either the standard supervisory tone, as defined in TS
        //02.40 [18], or proprietary tones defined by the ME manufacturer. The
        //code values for proprietary tones shall be supported by the ME. If
        //proprietary tones are not supported the ME shall map these codings to
        //tones that it can generate. The tones to be used are left as an
        //implementation decision by the manufacturer.
        //
        //Coding:
        // Standard supervisory tones:
        //  '01'    Dial tone
        //  '02'    Called subscriber busy
        //  '03'    Congestion
        //  '04'    Radio path acknowledge
        //  '05'    Radio path not available / Call dropped
        //  '06'    Error / Special information
        //  '07'    Call waiting tone
        //  '08'    Ringing tone
        // ME proprietary tones:
        //  '10'    General beep
        //  '11'    Positive acknowledgement tone
        //  '12'    Negative acknowledgement or error tone
        // All other values are reserved.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_NumberOfFiles == aType )
        {
        //Number of files:
        //This is the number of files that will be described in the following
        //list.
        ind = KTlvDataAreaStartPosition + offset;
        }
    else if ( ETLV_LocationStatus == aType )
        {
        //Location status
        //
        //Contents:
        // this data object indicates the current service state
        // of the MS.
        //  -   "Normal service" shall indicate that the MS is in a state
        //       where all requests for services are treated normally.
        //  -   "Limited service" shall indicate that the MS is in a state
        //       where only emergency call services are offered.
        //  -   "No service" shall indicate that the MS is in a state
        //       where no services are offered.
        //
        //Coding:
        // Each byte in the event list shall be coded with one of
        // the values below:
        //  -   '00' = Normal service
        //  -   '01' = Limited service
        //  -   '02' = No service
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_IconQualifier == aType )
        {
        //Icon qualifier:
        //Contents: The icon qualifier indicates to the ME how the icon is
        //to be used.
        //Coding:
        //bit 1:        0 = icon is self-explanatory, i.e. if displayed, it
        //                  replaces the alpha identifier or text string
        //              1 = icon is not self-explanatory, i.e. if
        //                  displayed, it shall be displayed together with
        //                  the alpha identifier or text string
        //bits 2?8      = 0 RFU.
        ind = KTlvDataAreaStartPosition;
    }
    else if ( ETLV_IconIdentifier == aType )
        {
        //Icon identifier:
        //Contents: The icon identifier addresses a record in EFIMG as
        //defined in TS 11.11 [20].
        //Coding:   Binary.
        ind = KTlvDataAreaStartPosition + 1;
        }
    else if ( ETLV_IconListQualifier == aType )
        {
        //Icon list qualifier:
        //Contents: The icon list qualifier indicates to the ME how the
        //icons are to be used.
        //Coding:
        //bit 1:        0 = icon is self-explanatory, i.e. if displayed, it
        //                  replaces the item text
        //              1 = icon is not self-explanatory, i.e. if displayed,
        //                  it shall be displayed together with the item text
        // bits 2?8 = 0 RFU.

        //All icons in the list shall be treated in the same manner by the
        //ME, i.e. either none of the icons in this list are displayed, or
        //for each item its related icon is displayed.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_CardReaderStatus == aType )
        {
        //Card reader status:
        //Contents :
        //This contains the identity of the card reader, and flags to
        //indicate the status of the reader with respect to:
        //- whether the card reader is removable or permanently connected;
        //- whether the card reader is present (this can only be false if
        //  the card reader is removable)
        //- whether the card reader present accepts ID-1 size cards (this
        //  can only be true if the card reader is present)
        //- whether there is a card present in the card reader (this can
        //  only be true if the card reader is present)
        //- whether power is being applied to the card (this can only be true if a card is present).

        //Coding :
        //The value of this byte indicates the identity and status of a
        //card reader.
        // bits 1-3       = identity of card reader x.
        // bit 4        0 = Card reader is not removable
        //              1 = Card reader is removable
        // bit 5        0 = Card reader is not present
        //              1 = Card reader is present
        // bit 6        0 = Card reader present is not ID-1 size
        //              1 = Card reader present is ID-1 size
        // bit 7        0 = No card present
        //              1 = Card is present in reader
        // bit 8        0 = No card power ON
        //              1 = Card in reader is power ON
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_CommandClassCLA == aType )
        {
        //This object contains the command APDU for Card x in the format
        //defined in ISO/IEC 7816?4 [25]. Command class CLA, instruction
        //code INS, P1 and P2 parameters, Lc, Data and Le are coded as
        //defined in ISO/IEC 7816?4 [25]. Extended lengths are not
        //supported.
        //Note: The maximum size of the value part of this Simple TLV
        //(value of X) is limited to 241 bytes, so the maximum length for
        //the Data (value of Lc) in a Case 3 type of APDU is 236 bytes.
        ind = KTlvDataAreaStartPosition + offset;
        }
    else if ( ETLV_CommandInstructionCodeIns == aType )
        {
        //This object contains the command APDU for Card x in the format
        //defined in ISO/IEC 7816?4 [25]. Command class CLA, instruction
        //code INS, P1 and P2 parameters, Lc, Data and Le are coded as
        //defined in ISO/IEC 7816?4 [25]. Extended lengths are not
        //supported.
        //Note: The maximum size of the value part of this Simple TLV
        //(value of X) is limited to 241 bytes, so the maximum length for
        //the Data (value of Lc) in a Case 3 type of APDU is 236 bytes.
        ind = KTlvDataAreaStartPosition + offset + 1;
        }
    else if ( ETLV_P1Parameter == aType )
        {
        //This object contains the command APDU for Card x in the format
        //defined in ISO/IEC 7816?4 [25]. Command class CLA, instruction
        //code INS, P1 and P2 parameters, Lc, Data and Le are coded as
        //defined in ISO/IEC 7816?4 [25]. Extended lengths are not
        //supported.
        //Note: The maximum size of the value part of this Simple TLV
        //(value of X) is limited to 241 bytes, so the maximum length for
        //the Data (value of Lc) in a Case 3 type of APDU is 236 bytes.
        ind = KTlvDataAreaStartPosition + offset + 2;
        }
    else if ( ETLV_P2Parameter == aType )
        {
        //This object contains the command APDU for Card x in the format
        //defined in ISO/IEC 7816?4 [25]. Command class CLA, instruction
        //code INS, P1 and P2 parameters, Lc, Data and Le are coded as
        //defined in ISO/IEC 7816?4 [25]. Extended lengths are not
        //supported.
        //Note: The maximum size of the value part of this Simple TLV
        //(value of X) is limited to 241 bytes, so the maximum length for
        //the Data (value of Lc) in a Case 3 type of APDU is 236 bytes.
        ind = KTlvDataAreaStartPosition + offset + 3;
        }
    else if ( ETLV_Lc == aType )
        {
        //This object contains the command APDU for Card x in the format
        //defined in ISO/IEC 7816?4 [25]. Command class CLA, instruction
        //code INS, P1 and P2 parameters, Lc, Data and Le are coded as
        //defined in ISO/IEC 7816?4 [25]. Extended lengths are not
        //supported.
        //Note: The maximum size of the value part of this Simple TLV
        //(value of X) is limited to 241 bytes, so the maximum length for
        //the Data (value of Lc) in a Case 3 type of APDU is 236 bytes.
        if ( KCApduLengthWithoutLc < GetLongInfo( ETLV_Lc ) )
            {
            ind = KTlvDataAreaStartPosition + offset + 4;
            }
        else
            {
            return 0;
            }
        }
    else if ( ETLV_Le == aType )
        {
        //This object contains the command APDU for Card x in the format
        //defined in ISO/IEC 7816?4 [25]. Command class CLA, instruction
        //code INS, P1 and P2 parameters, Lc, Data and Le are coded as
        //defined in ISO/IEC 7816?4 [25]. Extended lengths are not
        //supported.
        //Note: The maximum size of the value part of this Simple TLV
        //(value of X) is limited to 241 bytes, so the maximum length for
        //the Data (value of Lc) in a Case 3 type of APDU is 236 bytes.
        ind = KTlvDataAreaStartPosition + offset + 5 + GetShortInfo( ETLV_Lc );
        }
    else if ( ETLV_StatusWordSW1 == aType )
        {
        //This object contains the response APDU from Card x in the format
        //defined in ISO/IEC 7816?4 [25]. The R-APDU data and status words SW1
        //and SW2 are coded as defined in ISO/IEC 7816?4 [25]. It is possible
        //for no R-APDU data to be present; this is indicated by the length of
        //the data object.
        //Note: The maximum size of the value part of this Simple TLV (value of
        //X) is limited to 239 bytes, so the maximum length of the R-APDU data
        //is 237 bytes.
        ind = KTlvDataAreaStartPosition + offset
            + GetLength() - 2 + 1;
        }
    else if ( ETLV_StatusWordSW2 == aType )
        {
        //This object contains the response APDU from Card x in the format
        //defined in ISO/IEC 7816?4 [25]. The R-APDU data and status words SW1
        //and SW2 are coded as defined in ISO/IEC 7816?4 [25]. It is possible
        //for no R-APDU data to be present; this is indicated by the length of
        //the data object.
        //Note: The maximum size of the value part of this Simple TLV (value of
        //X) is limited to 239 bytes, so the maximum length of the R-APDU data
        //is 237 bytes.
        ind = KTlvDataAreaStartPosition + offset
            + GetLength() - 2 + 2;
        }
    else if ( ETLV_TimerIdentifier == aType )
        {
        //Timer identifier:
        //Contents: identifier of a timer
        //Coding:
        //'01'  Timer 1
        //'02'  Timer 2
        //'03'  Timer 3
        //'04'  Timer 4
        //'05'  Timer 5
        //'06'  Timer 6
        //'07'  Timer 7
        //'08'  Timer 8
        //All other values are reserved
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_BcRepeatIndicatorValues == aType )
        {
        //Contents : The BC repeat indicator is structured exactly as defined
        //in TS 04.08 [08], which may be alternate mode or sequential mode.
        //
        //Coding :  '01' = Alternate mode
        //          '03' = Sequential mode
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_MeStatus == aType )
        {
        //Coding of ME status:
        //'00' = ME is in the idle state
        //'01' = ME is not in idle state
        //'02' to'FF'= reserved values
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_TimingAdvance == aType )
        {
        //The Timing Advance is coded as for the Timing Advance information
        //element in TS 04.08 [8], starting at octet 2 (the IEI is removed, as
        //this information is duplicated by the data object tag).
        ind = KTlvDataAreaStartPosition + 1;
        }
    else if ( ETLV_BrowserIdentity == aType )
        {
        //Coding :
        //00 = Default Browser shall be used.
        //Other values are RFU.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_BrowserTerminationCause == aType )
        {
        //Coding:
        //00 = User Termination.
        //01 = Error Termination.
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_BearerType == aType )
        {
        //Bearer Type coding
        //'0x00': Bearer type not set
        //'0x01': CSD
        //'0x02': GPRS
        //'0x03': Default bearer for requested transport layer.
        //'0x04': Local link technology independent
        //'0x05': Bluetooth
        //'0x06': IrDA
        //'0x07': RS232
        //'0x10': USB
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_ChannelDataLength == aType )
        {
        //The Channel data length codes :
        //- either the number of bytes that are available in a channel buffer
        //  (Tx or Rx buffers negotiated during OPEN CHANNEL) using TERMINAL
        //  RESPONSE. Since the Tx or Rx buffer size can be larger than 255
        //  bytes, 'FF' means "more than 255 bytes are available".
        //- or the number of bytes that are requested in a RECEIVE DATA command
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_TypeOfAddress == aType )
        {
        //A null Local address shall be coded with Length = '00', and no Value
        //part. In that case, the ME shall request a dynamic address.
        //
        //Coding of Type of address: according to packet data protocol address
        //in TS 04.08 [8].
        //'21' = IPv4 address
        //'97' = IPv6 address
        //'others' = reserved
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_TransportProtocolType == aType )
        {
        //Transport protocol type coding:
        //- '01' : UDP (as defined in RFC 768 [33])
        //- '02' : TCP (as defined in RFC 793 [34])
        //all other value are reserved
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_UtranMeasurementQualifier == aType )
        {
        // UTRAN Measurement Qualifier coding:
        //'01' Intra-frequency measurements
        //'02' Inter-frequency measurements
        //'03' Inter-RAT (GSM) measurements
        // All other values are reserved
        ind = KTlvDataAreaStartPosition;
        }
    else if ( ETLV_RemoteEntityAddressCoding == aType )
        {
        // Remote Entity Address Coding Type:
        // '00' IEEE-802 48-bit address
        // '01' to 'FF' are reserved values
        ind = KTlvDataAreaStartPosition;
        }
    else
        {
        // Data type did not match with any of enumerations
        TFLOGSTRING2("TSY: CTlv::GetShortInfo, Unknown data type:0x%x", aType);
        OstTrace1( TRACE_DETAILED, DUP1_CTLV_GETSHORTINFO, "CTlv::GetShortInfo Unknown data type: %{TTlvSpesificDataType}", aType );

        }

    return iData[ind];

    }

// -----------------------------------------------------------------------------
// CTlv::GetData
// Returns the data. Returned data depends on data type.
// -----------------------------------------------------------------------------
//
TPtrC8 CTlv::GetData
        (
        TTlvSpesificDataType aType //Info spesific data type
        )
    {
    OstTrace0( TRACE_DETAILED, CTLV_GETDATA, "CTlv::GetData" );
    TFLOGSTRING("TSY: CTlv::GetData");

    //information is generally at index 2.
    TInt ind ( 2 );
    TUint8 offset( 0 );
    TInt length( 1 );

    //first determine if the length of the TLV is coded with 1 or 2 bytes.
    if ( KTwoByteLengthCoding == iData[KTlvLengthStartPosition] )
        {
        offset = 1;
        }

    //start getting TLV data elements
    if ( ETLV_DiallingNumberString == aType )
        {
        ind = KTlvDataAreaStartPosition + 1 + offset;
        //Dialling number string is coded as for EFADN, and may include
        //DTMF separators and DTMF digits, which the ME shall send in the
        //same way as for EFADN but without locally generating audible DTMF
        //tones to the user.
        //-----------------------------------------------------------
        // Byte(s)              | Description             |  Length  |
        //-----------------------------------------------------------
        // 1                    | Address tag             |  1       |
        // 2 to (Y-1)+2         | Length (X)              |  Y       |
        // (Y-1)+3              | TON and NPI             |  1       |
        // (Y-1)+4 to (Y-1)+X+2 | Dialling number string  |  X-1     |
        //------------------------------------------------------------
        length = GetLength() - 1;
        }
    else if ( ETLV_AlphaIdentifier == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //The alpha identifier is coded as for EFADN. See TS 11.11 [20] for
        //the coding of all EFs.
        length = GetLength();
        }
    else if ( ETLV_SubAddress == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Subaddress contains information as defined for this purpose in
        //TS 04.08 [8] (calling party subaddress or called party
        //subaddress). All information defined in TS 04.08 shall be given
        //in the value part of the data object, except the information
        //element identifier and the length of subaddress contents (which
        //is given by the length part of the data object).
        length = GetLength();
        }
    else if ( ETLV_CapabilityConfigurationParameters == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Capability configuration parameters are coded as for EFCCP. If
        //it is being provided by the SIM, the SIM shall supply all information
        //required to complete the Bearer Capability Information Element in the
        //Call Set-up message (see TS 04.08 [8]). Any unused bytes at the end
        //of the value part shall be coded 'FF'. See TS 11.11 [20] for the
        //coding of all EFs.
        //
        //NOTE: The second byte of this TLV contains the Length of the TLV and
        //the third byte contains the Length of the bearer capability contents,
        //followed by the actual contents.

        length = GetLength();
        }
    else if ( ETLV_CellBroadcastPage == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //The Cell Broadcast page is formatted in the same way as described
        //in TS 23.041 [7].
        length = GetLength();
        }
    else if ( ETLV_TextStringOfItem == aType )
        {
        ind = KTlvDataAreaStartPosition + offset + 1;
        //The text string is coded in the same way as the alpha identifier
        //for EFADN. Any unused bytes at the end of the value part shall be
        //coded 'FF'.
        length = GetLength() - 1;
        }
    else if ( ETLV_AdditionalInformationOnResult == aType )
        {
        ind = KTlvDataAreaStartPosition + offset + 1;
        //Contents: For the general result "Command performed successfully",
        //some proactive commands require additional information in the command
        //result. This is defined in the subclauses below. For the general
        //results '20', '21', '26', '34', '35', '37', '38' and '39' and '3A',
        //it is mandatory for the ME to provide a specific cause value as
        //additional information, as defined in the subclauses below. For the
        //other general results, the ME may optionally supply additional
        //information. If additional information is not supplied, then the
        //length of the value part of the data object need only contain the
        //general result.
        length = GetLength() - 1;
        }
    else if ( ETLV_SmsTPdu == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //The TPDU is formatted as described in TS 23.040 [6]. Where the TPDU
        //is being sent from the SIM to the ME (to be forwarded to the
        //network), and where it includes a TP-Message-Reference which is to be
        //incremented by the ME for every outgoing message, the
        //TP-Message-Reference as provided by the SIM need not be the valid
        //value. TP-Message-Reference shall be checked and corrected by the ME
        //to the value described in TS 23.040 [6].
        length = GetLength();
        }
    else if ( ETLV_SsOrUssdString == aType )
        {
        ind = KTlvDataAreaStartPosition + offset + 1;
        //SS or USSD control string are coded as for EFADN, where the ADN
        //record relates to a Supplementary Service Control string.
        //See TS 11.11 [20] for the coding of EFADN.
        length = GetLength() - 1;
        }
    else if ( ( ETLV_TextString == aType ) ||
        ( ETLV_DefaultText == aType ) )
        {
        ind = KTlvDataAreaStartPosition + offset + 1;
        //A null text string shall be coded with Length = '00', and no Value
        //part.
        length = GetLength() - 1;
        }
    else if ( ETLV_UssdString == aType )
        {
        ind = KTlvDataAreaStartPosition + offset + 1;
        //The coding of the USSD string is defined in TS 02.30 [4].
        length = GetLength() - 1;
        }
    else if ( ETLV_Files == aType )
        {
        ind = KTlvDataAreaStartPosition + offset + 1;
        //Files:
        //Full paths are given to files. Each of these shall be at least 4
        //octets in length (e.g. '3F002FE2' or '3F007F206FAD'). Each entry in
        //the file description is composed of two bytes, where the first byte
        //identifies the type of file (see TS 11.11). An entry in the file
        //description shall therefore always begin with '3FXX'. There can be
        //any number of Dedicated File entries between the Master File and
        //Elementary File. There shall be no delimiters between files, as this
        //is implied by the fact that the full path to any EF starts with
        //'3FXX' and ends with an Elementary type file.
        length = GetLength() - 1;
        }
    else if ( ETLV_MccAndMnc == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //The mobile country code (MCC), the mobile network code (MNC), the
        //location area code (LAC) and the cell ID are coded as in
        //TS 04.08 [8].
        length = KMccAndMncLength;
        }
    else if ( ETLV_Imei == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //The IMEI is coded as in TS 04.08 [8].
        length = GetLength();
        }
    else if ( ETLV_NetworkMeasurementResults == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //The Network Measurement Results are coded as for the Measurement
        //Results information element in TS 04.08 [8], starting at octet 2 (the
        //IEI is removed, as this information is duplicated by the data object
        //tag).
        length = GetLength();
        }
    else if ( ETLV_ItemsNextActionIndicator == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //Contents :
        //Each item of a list of items has a next action indicator coded on one
        //byte. The length of the Items Next Action Indicator list shall be the
        //number of items of the list of items (X shall be the number of items
        //in the list). The order of each item next action indicator, shall
        //reflect the order o the items in the list of items.

        //The Item Next action indicator gives the possible actions that will
        //be initiated by the SIM in case of selection by the user.

        //Coding :  If the value is equal to '00' or if the value is reserved
        //(that is, value not listed), the ME shall ignore the next action
        //indicator type.

        //See subclause 13.4 for further information.
        //Example :
        //For the following list of items :
        //          - item #1;
        //          - item #2;
        //          - item #3;
        //          - ....
        //          - item #n,
        //the Items Next Action Indicator (NAI) shall be as follows :
        //
        //  ------------------------------------------------------------
        // |    Tag    |   Length   |   NAI#1   |   NAI#2   |   NAI#3   |
        //  ------------------------------------------------------------

        length = GetLength();
        }
    else if ( ETLV_EventList == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Event list
        //Contents: A list of events, of variable length. Each byte in the list
        //defines an event. Each event type shall not appear more than once
        //within the list.
        //Coding: Each byte in the event list shall be coded with one of the
        //values below:
        // -    '00' = MT call
        // -    '01' = Call connected
        // -    '02' = Call disconnected
        // -    '03' = Location status
        // -    '04' = User activity
        // -    '05' = Idle screen available
        // -    '06' = Card reader status    (if class "a" is supported)
        // -    '07' = Language selection
        // -    '08' = Browser Termination    (if class "c" is supported)
        // -    '09' = Data available    (if class "e" is supported)
        // -    '0A' = Channel status   (if class "e" is supported)
        length = GetLength();
        }
    else if ( ETLV_Cause == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //The Cause data object is coded as for the Cause call control
        //information element in TS 04.08 [8], starting at octet 3 (the IEI and
        //Length information are removed, as this information is duplicated by
        //the data object tag and length). Radio Link Timeout is indicated by
        //the Cause data object having a value part of zero length (only the
        //Tag and Length components are sent).
        length = GetLength();
        }
    else if ( ETLV_TransactionIdentifier == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //Transaction identifier list
        //Contents: A list of transaction identifiers, of variable length. Each
        //byte in the list defines a transaction identifier. Each transaction
        //identifier shall not appear more than once within the list.
        //Coding: Each byte in the transaction identifier list shall be coded
        //as defined below:
        //bits 1 to 4   = RFU
        //bits 5 to 7   = value
        //bit  8        = flag
        //value and flag are coded as defined in TS 24.007 [23].
        length = GetLength();
        }
    else if ( ETLV_BcchChannelList == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //BCCH channel list
        //Contents: the list of absolute RF channels for BCCH carriers, as
        //known by the ME from the SYSTEM INFORMATION messages. The BCCH
        //channel list is composed of one to three BCCH channel sub lists, each
        //sub list is derived from the set of frequencies defined by reference
        //neighbour cells description information element or elements. In the
        //latter case the set is the union of the different subsets defined by
        //the neighbour cells description information elements
        //(see TS 04.08 [8]). The length of the BCCH channel list field depends
        //on the length of the received BCCH channel list derived from the
        //different SYSTEM INFORMATION messages to be considered.
        //Coding: Each ARFCN is represented by 10 bits. Spare bit(s) are to be
        //filled with 0.

        //         Bit 8   Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2  Bit 1
        //---------------------------------------------------------------------
        // Byte 1 |                        ARFCN#1 (high part)
        // Byte 2 |ARFCN#1 (low part)              ARFCN#2 (high part)
        // Byte 3 |        ARFCN#2 (low part)              ARFCN#3 (high part)
        // Byte n |                        n
        // ByteX-1|        ARFCN#m-1 (low part)            ARFCN#m (high part)
        // Byte X |                ARFCN#m (low part)              Spare  Spare
        //                                                          (0)     (0)

        //SIM applications should take into account that early implementations
        //of SIM application toolkit may have coded this field differently,
        //because of an inconsistancy between the content and the coding of
        //this element in previous versions of 11.14. The SIM is able to
        //identify MEs that are using the coding described above by evaluating
        //the indication "BCCH Channel List coding" in the TERMINAL PROFILE
        //command.
        length = GetLength();
        }
    else if ( ETLV_CallControlRequestedAction == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Call control requested action
        //Contents: The action given in response to the ENVELOPE (CALL
        //CONTROL). It may contain, in the same order as given by the SIM, the
        //address or SS string, the capability configuration parameters, the
        //called party sub-address and the alpha identifier.

        //Coding: as described in subclause 9.1.6, starting with the first
        //optional element given in the response data to the ENVELOPE (CALL
        //CONTROL).
        length = GetLength();
        }
    else if ( ETLV_IconIdentifierList == aType )
        {
        ind = KTlvDataAreaStartPosition + 1;
        //Icon identifier list:
        //Contents :
        //Each item of a list of items has an icon identifier coded on one
        //byte. The length of the Items icon identifier list shall be the
        //number of items of the list of items (X-1 shall be the number of
        //items in the list). The order of each item icon identifier, shall
        //reflect the order of the items in the list of items.
        //Each icon identifier addresses a record in EFIMG as defined in
        //TS 11.11 [20].

        //Coding :  Binary.
        //Example :
        //For the following list of items : - item #1
        //                                  - item #2
        //                                  - item #3
        //                                  - ....
        //                                  - item #n,

        //the Items icon identifier list shall be as follows :
        //---------------------------------------------------------
        // Tag | Length |icon         |icon         |icon         |
        //---------------------------------------------------------
        //     |        |identifier#1 |identifier#2 |identifier#3 |
        //---------------------------------------------------------
        length = GetLength() - 1;
        }
    else if ( ETLV_Atr == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //ATR:
        //Contents :
        //This is the Answer To Reset returned by the card.

        //Coding :
        //The coding of the Answer To Reset is defined in ISO/IEC 7816-3 [16].
        length = GetLength();
        }
    else if ( ETLV_Data == aType )
        {
        TUint8 dataLength( GetShortInfo( ETLV_Lc ) );
        if ( dataLength )
            {
            ind = KTlvDataAreaStartPosition + offset + 5;
            //This object contains the command APDU for Card x in the format
            //defined in ISO/IEC 7816?4 [25]. Command class CLA, instruction
            //code INS, P1 and P2 parameters, Lc, Data and Le are coded as
            //defined in ISO/IEC 7816?4 [25]. Extended lengths are not
            //supported.
            //Note: The maximum size of the value part of this Simple TLV
            //(value of X) is limited to 241 bytes, so the maximum length for
            //the Data (value of Lc) in a Case 3 type of APDU is 236 bytes.
            length = dataLength;
            }
        else
            {
            return 0;
            }
        }
    else if ( ETLV_RApduData == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //This object contains the response APDU from Card x in the format
        //defined in ISO/IEC 7816?4 [25]. The R-APDU data and status words SW1
        //and SW2 are coded as defined in ISO/IEC 7816?4 [25]. It is possible
        //for no R-APDU data to be present; this is indicated by the length of
        //the data object.
        //Note: The maximum size of the value part of this Simple TLV (value of
        //X) is limited to 239 bytes, so the maximum length of the R-APDU data
        //is 237 bytes.
        length = GetLength() - 2;
        }
    else if ( ETLV_TimerValue == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //Timer value:
        //Contents: value of a timer, expressed using the format hour, minute,
        //second.
        //Coding:
        // -    byte 3: hour; this byte is coded exactly in the same way as the
        //              hour field of the TP-Service-Centre-Time-Stamp in
        //              TS 23.040 [6].
        // -    byte 4: minute; this byte is coded exactly in the same way as
        //              the minute field of the TP-Service-Centre-Time-Stamp in
        //              TS 23.040 [6].
        // -    byte 5: second; this byte is coded exactly in the same way as
        //              the second field of the TP-Service-Centre-Time-Stamp in
        //              TS 23.040 [6].
        length = GetLength();
        }
    else if ( ETLV_DateTimeAndTimeZone == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //The Date-Time and Time zone is coded as for the Time Zone and Time
        //information element in TS 04.08 [8], starting at octet 2 (i.e. 1 byte
        //for year, month, day, hour, minute, second and time zone). Each byte
        //is encoded in exactly the same way as the corresponding field of the
        //TP-Service-Centre-Time-Stamp in TS 23.040 [6]. For the time zone
        //field, 'FF' indicates an unknown value.
        length = GetLength();
        }
    else if ( ETLV_AtCommand == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Contents :
        // The AT Command string is structured exactly as the AT Command line
        // as defined in GSM 27.007 [27], which may contain single or
        // concatenated AT commands.
        length = GetLength();
        }
    else if ( ETLV_AtResponse == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Contents:
        // The AT Response string is structured exactly as the response to a
        // command line as defined in GSM 27.007 [27], which may contain single
        // or concatenated responses appropriate to the issued AT command.
        // If the AT Response string is longer than the maximum length capable
        // of being transmitted to the SIM then the AT Response string shall be
        // truncated to this length by the ME.
        length = GetLength();
        }
    else if ( ETLV_DtmfString == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Contents : The DTMF string which can be single or multiple characters
        //is coded in BCD, in the same way as the Dialling number string
        //defined for EFADN in TS 11.11 [20].  It may include extended BCD
        //coding. There is no need for a DTMF control digit separator at the
        //beginning of the string, but if present it shall be interpreted as
        //PAUSE.
        length = GetLength();
        }
    else if ( ETLV_Url == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //A null URL shall be coded with Length = '00', and no Value part. In
        //that case, the ME shall use the default  URL.
        //
        //Coding :
        //The data used for the URL shall be coded as defined in [32] on using
        //the "SMS 7bits default alphabet" with bit 8 set to 0;
        length = GetLength();
        }
    else if ( ETLV_ListOfBearers == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //The ME shall use this list to choose which bearers are allowed in
        //order of priority.

        //Coding of the bearers :
        //'00' = SMS
        //'01' = CSD
        //'02' = USSD
        //'03' = GPRS
        //'04' to 'FF' = RFU.
        length = GetLength();
        }
    else if ( ETLV_PathToProvisioningFile == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Note : the path is the concatenation of file identifiers starting
        //from the Master File, e.g. : 3F007F206FXY...
        //The file shall contain a single unambiguous set of parameters
        //required to make the connection. The content of the file shall be
        //consistent with the format defined for provisioning information for
        //the requested type of browser.
        length = GetLength();
        }
    else if ( ETLV_BearerParameters == aType )
        {
        ind = KTlvDataAreaStartPosition + offset + 1;
        //see 3GPP spec: 12.52.1
        //12.52.1 Bearer parameters for CSD
        //12.52.2 Bearer parameters for GPRS / packet service
        //12.52.3 Default bearer
        length = GetLength() - 1;
        }
    else if ( ETLV_ChannelDataString == aType )
        {
        ind = KTlvDataAreaStartPosition + offset;
        //Contents:
        //The Channel data object contains application data read from or
        //written to a specific channel buffer in the ME.
        //Coding:
        //The Channel data string must be considered by the ME as binary coded
        //on 8 bits.
        length = GetLength();
        }
    else if ( ETLV_IdentifierOfCardReader == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //Coding :
        //The identifier of card reader is coded in hexadecimal.
        length = GetLength();
        }
    else if ( ETLV_Address == aType )
        {
        ind = KTlvDataAreaStartPosition + 1;
        //Coding of address: according to packet data protocol address in
        //TS 04.08 [8].
        //If type of address indicates IPv4, the Address information in octet 4
        //to octet 7 contains the IPv4 address. Bit 8 of octet 4 represents the
        //most significant bit of the IP address and bit 1 of octet 7 the least
        //significant bit .
        //If type of address indicates IPv6, the Address information in octet 4
        //to octet 19 contains the IPv6 address. Bit 8 of octet 4 represents
        //the most significant bit of the IP address and bit 1 of octet 19 the
        //least significant bit.
        length = GetLength() - 1;  // Length minus 1 because there is a byte
                                   //before the address data
        }
    else if ( ETLV_NetworkAccessName == aType )
        {
        ind = KTlvDataAreaStartPosition;
        //Content:
        //The Network Access Name is used to identify the Gateway entity, which
        //provides interworking with an external packet data network. For GPRS,
        //the Network Access Name is an APN.
        //Coding:   As defined in TS 23.003 [36].
        length = GetLength();
        }
    else if ( ETLV_AID == aType )
        {
        ind = KTlvDataAreaStartPosition;
        // Application identifier as defined in TS 31.110
        // Byte(s)        Description
        // 1            AID tag
        // 2            Length (X)
        // 3 to (X+2)    AID

        length = GetLength();
        }
    else if ( ETLV_RemoteEntityAddress == aType )
        {
        ind = KTlvDataAreaStartPosition + 1;
        // Remote Entity Address:
        // According to Coding Type
        // --------------------------------------------------
        // | Byte(s):     | Description:          | Length: |
        // --------------------------------------------------
        // | 1            | Item Tag              | 1       |
        // | 2 to Y+1     | Lenght (X+1)          | Y       |
        // | Y+2          | Coding Type           | 1       |
        // | Y+3 to Y+X+2 | Remote entity Address | X       |
        // --------------------------------------------------
        // Length minus 1 because there is a coding type byte
        // before the remote entity address data
        length = GetLength() - 1;
        }
    else
        {
        // Data type did not match
        TFLOGSTRING2("TSY: CTlv::GetData, Unknown data type: 0x%x", aType );
        OstTrace1( TRACE_DETAILED, DUP2_CTLV_GETDATA, "CTlv::GetData Unknown data type: %{TTlvSpesificDataType}", aType );

        }

    return iData.Mid( ind, length );

    }

// -----------------------------------------------------------------------------
// CTlv::GetLongInfo
// Returns the the data.
// Returned data depends on data type.
// -----------------------------------------------------------------------------
//
 TUint16 CTlv::GetLongInfo
        (
        TTlvSpesificDataType aType //Info spesific data type
        )
    {
    OstTrace0( TRACE_DETAILED, CTLV_GETLONGINFO, "CTlv::GetLongInfo" );
    TFLOGSTRING("TSY: CTlv::GetLongInfo");

    TUint16 ret( 0x0000 );

    if ( ETLV_LocationAreaCode == aType )
        {
        //The mobile country code (MCC), the mobile network code (MNC), the
        //location area code (LAC) and the cell ID are coded as
        //in TS 04.08 [8].
        TSatUtility::GetWord
            ( ret, iData, KTlvLengthStartPosition + KMccAndMncLength );
        }
    else if ( ETLV_CellIdentityValue == aType )
        {
        //The mobile country code (MCC), the mobile network code (MNC), the
        //location area code (LAC) and the cell ID are coded as
        //in TS 04.08 [8].
        TSatUtility::GetWord( ret, iData, KTlvLengthStartPosition
            + KMccAndMncLength + KLocationAreaCodeLength );
        }
    else if ( ETLV_Language == aType )
        {
        //Coding:
        // each language code is a pair of alpha-numeric characters, defined in
        // ISO 639 [29].  Each alpha-numeric character shall be coded on one
        // byte using the SMS default 7-bit coded alphabet as defined in
        // TS 23.038 [5] with bit 8 set to 0.
        TSatUtility::GetWord( ret, iData, KTlvDataAreaStartPosition );
        }
    else if ( ETLV_BufferSize == aType )
        {
        //Coding:
        // The Buffer size codes the number of bytes requested by the SIM in an
        // OPEN CHANNEL command or what the ME can offer the SIM (placed in
        // TERMINAL RESPONSE).
        TSatUtility::GetWord( ret, iData, KTlvDataAreaStartPosition );
        }
    else if ( ETLV_ChannelStatus == aType )
        {
        //Contents :
        //The Channel status is a string of binary coded characters.
        //Coding of byte 3:
        //bit 1 to 3:   Channel identifier : 1..7
        //              Channel identifier 0 means "No channel available"
        //bit 4 to 7:   RFU
        //bit 8:        0 = Link not established or PDP context not activated
        //              1 = Link established or PDP context activated
        //
        //Coding of byte 4:
        //- '00' = No further info can be given
        //- '01' = Not used
        //- '02' = Not used
        //- '03' = Not used
        //- '04' = Not used
        //- '05' = Link dropped
        //all other values are reserved for future use
        TSatUtility::GetWord( ret, iData, KTlvLengthStartPosition );
        }
    else if ( ETLV_PortNumber == aType )
        {
        //Port number coding: integer
        TSatUtility::GetWord( ret, iData, KTlvDataAreaStartPosition + 1 );
        }
    else
        {
        // Data type did not match
        TFLOGSTRING2("TSY: CTlv::GetLongInfo, Unknown data type:0x%x", aType );
        OstTrace1( TRACE_DETAILED, DUP1_CTLV_GETLONGINFO, "CTlv::GetLongInfo, Unknown data type: %{TTlvSpesificDataType}", aType );

        }
    return ret;

    }


// -----------------------------------------------------------------------------
// CBerTlv::BerTlv
// Set ber-tlv to proactive sim command ber-tlv in
// UICC_CAT_IND(UICC_CAT_FETCHED_CMD)  ISI message.
// -----------------------------------------------------------------------------
//
TInt CBerTlv::BerTlv( const TIsiReceiveC& aIsiMessage )
    {
    OstTrace0( TRACE_DETAILED, CBERTLV_BERTLV, "CBerTlv::BerTlv" );
    TFLOGSTRING("TSY: CBerTlv::BerTlv");

    TInt result ( KErrNotFound );
    TUint16 berTlvLength( 0 );

    // BER-TLV data is in subblock UICC_SB_APDU
    TUint uiccSbApduOffset( 0 );
    if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
        ISI_HEADER_SIZE + SIZE_UICC_CAT_IND,
        UICC_SB_APDU,
        EIsiSubBlockTypeId16Len16,
        uiccSbApduOffset ) )
        {
        berTlvLength = aIsiMessage.Get16bit(
            uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDULENGTH );

        // Check the proactive command tag
        if ( KBerTlvProactiveSimCommandTag == aIsiMessage.Get8bit(
            uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDU ) )
            {
            TPtrC8 data( aIsiMessage.GetData(
                uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDU,
                berTlvLength ) );
            SetData( data );
            result = KErrNone;
            }
        }
    return result;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================
// none

//End Of File
