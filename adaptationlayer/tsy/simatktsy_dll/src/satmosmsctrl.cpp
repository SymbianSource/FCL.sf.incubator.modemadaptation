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



//  INCLUDE FILES
#include "satmosmsctrl.h"       // sat mo sms control class
#include "satmessaging.h"       // sat messaging class
#include "satmesshandler.h"     // sat message handler class
#include "ber_tlv.h"            // sat ber-tlv classes
#include "satutil.h"            // sat utility class

#include <pn_const.h>           // server id constants
#include <tisi.h>               // isi message
#include <uiccisi.h>            // UICC server
#include <smsisi.h>             // sms server
#include <atk_sharedisi.h>
#include "osttracedefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "satmosmsctrltraces.h"
#endif



// CONSTANTS
const TUint8 KReservedTonNpi1           = 0x7F;
const TUint8 KReservedTonNpi2           = 0xF7;
const TUint8 KMaxLengthAddr1Addr2       = 249;
const TUint8 KMaxLengthOfAtkSwDataNtf   = 255;
const TUint8 KMaxLengthOfResourceReq    = 255;
const TUint8 KSmControlAllowed          = 0;
const TUint8 KSmControlNotAllowed       = 1;
const TUint8 KSmControlAllowedWithModif = 2;

// ==================== MEMBER FUNCTIONS ====================================

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::CSatMoSmsCtrl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSatMoSmsCtrl::CSatMoSmsCtrl
        (
        CSatMessHandler*    aSatMessHandler,
        CTsySatMessaging*   aSatMessaging
        )
        :
        iSatMessHandler( aSatMessHandler ),
        iSatMessaging( aSatMessaging ),
        iWaitingForEnvelopeResp( EFalse ),
        iIsMoSmsCtrlActivated( EFalse ),
        iMoSmsCtrlEnvelopeTransactionId( KZero ),
        iMoSmsCtrlReqTransactionId( KZero ),
        iSenderObject( KZero )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_CSATMOSMSCTRL, "CSatMoSmsCtrl::CSatMoSmsCtrl" );
    }


// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSatMoSmsCtrl::ConstructL()
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_CONSTRUCTL, "CSatMoSmsCtrl::ConstructL" );
#ifdef INFO_PP_ATK_MO_SM_CONTROL
    // Check MO SMS control is supported by ME's Product Profile
    iSatMessHandler->InfoPpReadReq( INFO_PP_ATK_MO_SM_CONTROL );
#else
    // Check if MO-SMS control should be activated in SMS server
    // This is done by sending a request SimReadTableReq to Sim server, with
    // service type KSimStMoSmsSimControlAvail
    iSatMessHandler->SimReadTableReq( SIM_ST_MO_SMS_SIM_CONTROL_AVAIL );
#endif // INFO_PP_ATK_MO_SM_CONTROL
    }


// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatMoSmsCtrl* CSatMoSmsCtrl::NewL
        (
        CSatMessHandler*    aSatMessHandler,
        CTsySatMessaging*   aSatMessaging
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_NEWL, "CSatMoSmsCtrl::NewL" );
    TFLOGSTRING("TSY: CSatMoSmsCtrl::NewL");

    CSatMoSmsCtrl* self =
        new( ELeave ) CSatMoSmsCtrl( aSatMessHandler, aSatMessaging );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::~CSatMoSmsCtrl
// Destructor
// -----------------------------------------------------------------------------
//
CSatMoSmsCtrl::~CSatMoSmsCtrl()
    {
    OstTrace0( TRACE_NORMAL, DUP1_CSATMOSMSCTRL_CSATMOSMSCTRL, "CSatMoSmsCtrl::~CSatMoSmsCtrl" );
    TFLOGSTRING("TSY: CSatMoSmsCtrl::~CSatMoSmsCtrl");
    }


// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::IsActivated
// Returns ETrue if MO-SMS Control is activated
// -----------------------------------------------------------------------------
//
TBool CSatMoSmsCtrl::IsActivated()
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_ISACTIVATED, "CSatMoSmsCtrl::IsActivated" );
    TFLOGSTRING("TSY: CSatMoSmsCtrl::IsActivated");
    return iIsMoSmsCtrlActivated;
    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::Activate
// Set iIsMoSmsCtrlActivated to ETrue if MO-SMS Control has to be activated
// -----------------------------------------------------------------------------
//
void CSatMoSmsCtrl::Activate()
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_ACTIVATE, "CSatMoSmsCtrl::Activate" );
    TFLOGSTRING("TSY: CSatMoSmsCtrl::Activate");
    iIsMoSmsCtrlActivated = ETrue;
    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::Deactivate
// Set iIsMoSmsCtrlActivated to EFalse if MO-SMS Control has to be deactivated
// -----------------------------------------------------------------------------
//
void CSatMoSmsCtrl::Deactivate()
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_DEACTIVATE, "CSatMoSmsCtrl::Deactivate" );
    TFLOGSTRING("TSY: CSatMoSmsCtrl::Deactivate");
    iIsMoSmsCtrlActivated = EFalse;
    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::UiccCatRespEnvelopeReceived
// Handler function of incoming MO-SMS Control related data notification messages
// -----------------------------------------------------------------------------
//
TInt CSatMoSmsCtrl::UiccCatRespEnvelopeReceived
        (
        const TIsiReceiveC& aIsiMessage //Received data notification
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_UICCCATRESPENVELOPERECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived" );
    TFLOGSTRING("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived");
    TBuf8<KMaxLengthOfAtkSwDataNtf> addr1;
    TBuf8<KMaxLengthOfAtkSwDataNtf> addr2;
    iWaitingForEnvelopeResp = EFalse;

    TUint8 status( aIsiMessage.Get8bit(
        ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_STATUS ) );
    TBool simRespDataConsistent = ETrue;
    TUint8 resourceStatus = 0;
    TUint8 sw1( 0 );
    TUint8 sw2( 0 );
    TInt8 atkResult( 0 );
    TUint8 numSb( 3 ); // Mandatory no of subblocks
    TBuf8<SIZE_SMS_RESOURCE_REQ+SIZE_SMS_SB_RESOURCE+SIZE_SMS_SB_RESOURCE_SEQ_ID+
        SIZE_SMS_SB_ADDRESS+SMS_ADDRESS_MAX_LEN+SIZE_SMS_SB_TPDU_PACKING+
        SMS_TPDU_MAX_LEN+SIZE_SMS_SB_CAUSE> dataResp;
    TIsiSend resourceReq( dataResp );
    dataResp.AppendFill( KPadding, 1 ); // filler byte
    dataResp.Append( numSb ); // no of Subblocks

    if ( UICC_STATUS_OK == status )
        {
        TUint uiccSbApduOffset( 0 );
        if ( KErrNone == aIsiMessage.FindSubBlockOffsetById(
            ISI_HEADER_SIZE + SIZE_UICC_CAT_RESP,
            UICC_SB_APDU,
            EIsiSubBlockTypeId16Len16,
            uiccSbApduOffset ) )
            {
            TPtrC8 apduData;
            TUint16 apduLength( aIsiMessage.Get16bit(
                uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDULENGTH ) );
            apduData.Set( aIsiMessage.GetData(
                uiccSbApduOffset + UICC_SB_APDU_OFFSET_APDU,
                apduLength ) );
            // Status bytes are two last bytes in APDU
            sw1 = apduData[apduLength - 2];
            sw2 = apduData[apduLength - 1];
            // Get Call Control General Result
            atkResult = TSatUtility::Sw1Sw2Check( sw1, sw2 );

            if ( KError != atkResult ) // If ATK result is OK
                {
                TFLOGSTRING("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, Ok");
                OstTrace0( TRACE_NORMAL, DUP1_CSATMOSMSCTRL_ATKSWDATANTFRECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, Ok" );
                RSat::TAlphaId alphaId;
                alphaId.iAlphaId.Zero();
                TBool isAlphaIdEmpty( EFalse );

                if ( KMaxLengthOfAtkSwDataNtf < apduLength ) // If data length greater than 255
                    {
                    TFLOGSTRING("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, the length of Sim resp data is > 255" );
                    OstTrace0( TRACE_NORMAL, DUP2_CSATMOSMSCTRL_ATKSWDATANTFRECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, the length of Sim resp data is > 255" );
                    // Data is not consistent, so not allowed condition
                    if( KSmControlNotAllowed == apduData[0] )
                        {
                        resourceStatus = SMS_RESOURCE_DENIED;
                        }
                    else
                        {
                        resourceStatus  = SMS_RESOURCE_ALLOWED;
                        }
                    FormSmsResourceReqSb( resourceStatus, dataResp, addr1, addr2 );
                    // Send resp to SMS server
                    // Return to the calling function right now, even if it's not
                    // a good thing to have several returns in the same method.
                    // To avoid it, more if statements would be needed and
                    // this doesn't make it any better.
                    return ( iSatMessHandler->SendSmsResourceReq(
                        iMoSmsCtrlReqTransactionId,
                        dataResp,
                        iSenderObject ) );
                    }
                else if ( apduLength ) // If data length in non zero and less than 255
                    {
                    TFLOGSTRING("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, atkData.Length()!=0 ");
                    OstTrace0( TRACE_NORMAL, DUP3_CSATMOSMSCTRL_ATKSWDATANTFRECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, atkData.Length()!=0 " );

                    // Note: The alpha id can be provided by the SIM even if the addresses
                    // have not been modified.
                    ParseAtkSwDataNtf( apduData, addr1, addr2, alphaId, isAlphaIdEmpty );

                    if ( KSmControlAllowedWithModif == apduData[0] )
                        {
                        simRespDataConsistent = VerifySimRespData( addr1, addr2 );
                        }
                    if ( simRespDataConsistent )
                        {
                        if( KSmControlNotAllowed == apduData[0] )
                            {
                            resourceStatus = SMS_RESOURCE_DENIED;
                            }
                        else
                            {
                            resourceStatus  = SMS_RESOURCE_ALLOWED;
                            }
                        FormSmsResourceReqSb( resourceStatus, dataResp, addr1, addr2 );
                        TFLOGSTRING2("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, apduData[0]: %d",apduData[0] );
                        OstTrace1( TRACE_NORMAL, DUP4_CSATMOSMSCTRL_ATKSWDATANTFRECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, apduData[0]: %d", apduData[0] );
                       }
                    else
                       {
                       TFLOGSTRING("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, Sim response data not consistent" );
                       OstTrace0( TRACE_NORMAL, DUP5_CSATMOSMSCTRL_ATKSWDATANTFRECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, Sim response data not consistent" );
                       if( KSmControlNotAllowed == apduData[0] )
                           {
                           resourceStatus = SMS_RESOURCE_DENIED;
                           }
                       else
                           {
                           resourceStatus  = SMS_RESOURCE_ALLOWED;
                           }
                        FormSmsResourceReqSb( resourceStatus, dataResp, addr1, addr2 );

                        // Send resp to Sms server
                        // Return to the calling function right now, even if it's not
                        // a good thing to have several returns in the same method.
                        // To avoid it, more if statements would be needed and
                        // this doesn't make it any better.
                        return ( iSatMessHandler->SendSmsResourceReq(
                            iMoSmsCtrlReqTransactionId,
                            dataResp,
                            iSenderObject ) );
                        }
                    } // end of if statement for data length nonzero and less than 255
                else  // block for data length zero
                    {
                    TFLOGSTRING("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, atkData.Length()=0 ");
                    OstTrace0( TRACE_NORMAL, DUP6_CSATMOSMSCTRL_ATKSWDATANTFRECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, atkData.Length()=0" );
                    TUint8 resourceStatus( SMS_RESOURCE_ALLOWED );
                    FormSmsResourceReqSb( resourceStatus, dataResp, addr1, addr2 );
                    }// end of block with data length zero

                // If AlphaID is present, the client SatServer
                // must be notified in order to inform it that the ETel
                // API MO-SMS Control structure TMoSmControlV1 has been modified.
                if ( alphaId.iAlphaId.Length() || isAlphaIdEmpty )
                    {
                    // Inform client
                    // Control result must be mapped to ETel API values.
                    TUint8 controlResult;
                    switch ( apduData[0] )
                        {
                        case KSmControlAllowed:
                            {
                            controlResult = RSat::EAllowedNoModification;
                            break;
                            }
                        case KSmControlAllowedWithModif:
                            {
                            controlResult = RSat::EAllowedWithModifications;
                            break;
                            }
                        case KSmControlNotAllowed:
                            {
                            controlResult = RSat::ENotAllowed;
                            break;
                            }
                        default:
                            {
                            controlResult = RSat::EControlResultNotSet;
                            break;
                            }
                        }
                    iSatMessaging->NotifyClientAboutMoSmControlEvent(
                    alphaId.iAlphaId, (RSat::TControlResult) controlResult );
                    }
                }
            }
        else // else for UICC status not ok
            {
            TFLOGSTRING("TSY:CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, KAtkError = atkResult ");
            OstTrace0( TRACE_NORMAL, DUP7_CSATMOSMSCTRL_ATKSWDATANTFRECEIVED, "CSatMoSmsCtrl::UiccCatRespEnvelopeReceived, KAtkError = atkResult" );
            TUint8 resourceStatus = SMS_RESOURCE_DENIED;
            FormSmsResourceReqSb( resourceStatus, dataResp, addr1, addr2 );
            }
        } // end if if UICC status is ok

    // Send resp to Sms server
    return ( iSatMessHandler->SendSmsResourceReq(
        iMoSmsCtrlReqTransactionId,
        dataResp,
        iSenderObject ) );
    }


// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::SmsResourceIndReceived
// Request received from SMS server with the SMS parameters
// of the SMS to be sent by the Mobile Equipment.
// The response will tell whether the SMS can be sent
// to network or not.
// -----------------------------------------------------------------------------
//
void CSatMoSmsCtrl::SmsResourceIndReceived
        (
        const TIsiReceiveC& aIsiMessage //Received data notification
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_SMSRESOURCEINDRECEIVED, "CSatMoSmsCtrl::SmsResourceIndReceived" );
    TFLOGSTRING("TSY:CSatMoSmsCtrl::SmsResourceIndReceived");

    TBuf8<256> addressData1;
    TBuf8<256> addressData2;

    // Save the transaction id, which is going to
    // be re-used in the resp.
    iMoSmsCtrlReqTransactionId = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID );

    // Save the sender object id, in order to re-use it
    // in the response to this request
    iSenderObject = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_SENDEROBJECT );

    // Save Sequence id to send it in request message
    iSequenceId = aIsiMessage.Get8bit( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+
            SIZE_SMS_SB_RESOURCE+SMS_SB_RESOURCE_SEQ_ID_OFFSET_SEQUENCEID );

    // save resource id to send it in request mesage
    iResourceId = aIsiMessage.Get16bit( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+
                                         SMS_SB_RESOURCE_OFFSET_RESOURCES );


     // Save Service Centre Address Subblock to send it in request message if server has
     //denied to access
     iAddressSubblock = aIsiMessage.GetData( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+
                    SIZE_SMS_SB_RESOURCE_SEQ_ID+SMS_SB_ADDRESS_OFFSET_ADDRESSDATA ,
                    aIsiMessage.Get8bit(ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+SIZE_SMS_SB_RESOURCE_SEQ_ID+
                    SMS_SB_ADDRESS_OFFSET_ADDRESSDATALENGTH) );

     // Calculate Address Subblock length
     TUint8 addrSbLen = aIsiMessage.Get16bit( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+SIZE_SMS_SB_RESOURCE_SEQ_ID+
                                             SMS_SB_ADDRESS_OFFSET_SUBBLOCKLENGTH );

     // Save Destination address TPDU to send it in request message if server has
     // Denied the access for that address
     iUserDataSubblock = ( aIsiMessage.GetData( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+
                    SIZE_SMS_SB_RESOURCE_SEQ_ID+addrSbLen+
                    SMS_SB_TPDU_OFFSET_DATABYTES,
                    aIsiMessage.Get8bit( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+
                                 SIZE_SMS_SB_RESOURCE_SEQ_ID+addrSbLen+SMS_SB_TPDU_OFFSET_DATALENGTH )));


     //Check if location data is present. Otherwise response to SmsResourceInd
     //is always ALLOWED.
     if ( !iSatMessHandler->ServiceAvailable() )
         {
         TBuf8<KMaxLengthOfResourceReq> dataResp;
         TIsiSend resourceReq( dataResp );

         dataResp.AppendFill( KPadding, 1);       // filer byte
         dataResp.Append( 5 );                // no of Subblocks

         // Add first SB Resource Subblock
         TIsiSubBlock resource( dataResp, SMS_SB_RESOURCE ,
             EIsiSubBlockTypeId16Len16 );                      // Message ID and Subblock length

         dataResp.Append( iResourceId >> 8 );        // SMS Resource IDs MSB

         dataResp.Append( iResourceId );           // SMS resource IDs LSB

         dataResp.AppendFill( KPadding,2 );           // Filler Bytes
         resource.CompleteSubBlock();

         // Add 2nd resource sequence ID subblock

         TIsiSubBlock resourceSeqId( dataResp, SMS_SB_RESOURCE_SEQ_ID ,
             EIsiSubBlockTypeId16Len16 );
         dataResp.Append(iSequenceId);                // Sequence ID
         dataResp.AppendFill( KPadding, 3 );
         resourceSeqId.CompleteSubBlock();


         // Add Third subblock SMS_SB_RESOURCE_STATUS

         TIsiSubBlock resourceStatus( dataResp, SMS_SB_RESOURCE_STATUS ,
             EIsiSubBlockTypeId16Len16 );
         dataResp.Append( SMS_RESOURCE_ALLOWED );            // Resource status
         dataResp.AppendFill( KPadding, 3 );
         resourceStatus.CompleteSubBlock();

         // Add 4th subblock SMS_SB_ADDRESS (SMSC)
         TIsiSubBlock addressSb( dataResp, SMS_SB_ADDRESS ,
             EIsiSubBlockTypeId16Len16 );
         dataResp.Append( SMS_SMSC_ADDRESS );            // Addrees Type
         dataResp.Append( iAddressSubblock.Length() );
         dataResp.Append( iAddressSubblock );

         // Filler bytes calculation for SMSC address subblock
         TUint8 fillbytes = 0;
         while(( iAddressSubblock.Length()+ 6 + fillbytes )%4)
             {
             fillbytes++;
             }

         dataResp.AppendFill( KPadding,fillbytes );
         addressSb.CompleteSubBlock();

        // Add 5th Subblock SMS_SB_TPDU (Destination address)

         TIsiSubBlock userDataSb( dataResp, SMS_SB_TPDU ,
             EIsiSubBlockTypeId16Len16 );
         dataResp.Append( iUserDataSubblock.Length() );            // Addrees Length
         dataResp.Append( KPadding );
         dataResp.Append( iUserDataSubblock );

         // Filler bytes calculation for user data subblock
         fillbytes = 0;
         while(( iUserDataSubblock.Length()+ 6 + fillbytes )%4 )
             {
             fillbytes++;
             }

         dataResp.AppendFill( KPadding,fillbytes );
         userDataSb.CompleteSubBlock();

          TFLOGSTRING("TSY:CSatMoSmsCtrl::SmsResourceIndReceived: Not Allowed Condition, Send response to SMS server");
          OstTrace0( TRACE_NORMAL, DUP1_CSATMOSMSCTRL_SMSRESOURCEINDRECEIVED, "CSatMoSmsCtrl::SmsResourceIndReceived: Not Allowed conditon, Send response to SMS server" );

          // Send Req to Sms server with allowed status
          iSatMessHandler->SendSmsResourceReq(
              iMoSmsCtrlReqTransactionId,
              dataResp,
              iSenderObject );
         }
     else
         {
        if( iIsMoSmsCtrlActivated )    // Check is MO SMS is activated or not
            {
            TDes8* addressData = NULL;
            if(SMS_SB_ADDRESS == aIsiMessage.Get16bit( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+
                    SIZE_SMS_SB_RESOURCE_SEQ_ID+SMS_SB_ADDRESS_OFFSET_SUBBLOCKID ))
                {
                addressData = &addressData1;

                TPtrC8 phoneNumber = aIsiMessage.GetData( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+
                        SIZE_SMS_SB_RESOURCE_SEQ_ID+SMS_SB_ADDRESS_OFFSET_ADDRESSDATA,
                        aIsiMessage.Get8bit( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE+
                        SIZE_SMS_SB_RESOURCE_SEQ_ID+SMS_SB_ADDRESS_OFFSET_ADDRESSDATALENGTH ) );

                addressData->Append( phoneNumber.Mid( 0, phoneNumber.Length()) );

                }
            if(SMS_SB_TPDU == aIsiMessage.Get16bit( ISI_HEADER_SIZE+SIZE_SMS_RESOURCE_IND+SIZE_SMS_SB_RESOURCE
                    +SIZE_SMS_SB_RESOURCE_SEQ_ID+addrSbLen+SMS_SB_TPDU_OFFSET_SUBBLOCKID ))
                {
                addressData =&addressData2;

                // check for message type
                // Whether its a command type message or Submit type

                if(KSmsCommandType == ( iUserDataSubblock[0]& 0x03 ))
                    {
                    iMessageType = KSmsCommandType;
                    TPtrC8 phoneNumber = &iUserDataSubblock[5];

                    // Addition of two for Type of number semi octet
                    addressData->Append( phoneNumber.Mid( 1, (iUserDataSubblock[5]/2)+1 ));
                    }
                else if( KSmsSubmitType == ( iUserDataSubblock[0] & 0x03 ))
                    {
                    iMessageType = KSmsSubmitType;
                    TPtrC8 phoneNumber = &iUserDataSubblock[2];
                    // Addition of two for Type of number semi octet
                    addressData->Append( phoneNumber.Mid( 1, (iUserDataSubblock[2]/2)+1 ));
                    }
                }
            // Dialled Number String, the length has to be removed (first byte)

        iMoSmsCtrlEnvelopeTransactionId = iSatMessaging->GetTransactionId();

            TFLOGSTRING("TSY:CSatMoSmsCtrl::SmsResourceIndReceived, Send envelope");
            OstTrace0( TRACE_NORMAL, DUP3_CSATMOSMSCTRL_SMSRESOURCEINDRECEIVED, "CSatMoSmsCtrl::SmsResourceIndReceived, Send envelope" );

            // The envelope is sent if MO SMS is activated
            SendMoSmsCtrlEnvelope(
                iMoSmsCtrlEnvelopeTransactionId,
                addressData1,
                addressData2
                );
       }
        else        // If MO SMS is not activated
            {
            TBuf8<KMaxLengthOfResourceReq> dataResp;
            TIsiSend resourceReq( dataResp );

            dataResp.AppendFill( KPadding, 1 );       // filer byte
            dataResp.Append( 4 );                // no of Subblocks

            // Add first SB Resource Subblock
            TIsiSubBlock resource( dataResp, SMS_SB_RESOURCE ,
                EIsiSubBlockTypeId16Len16 );          // Message ID and Subblock length

            dataResp.Append( iResourceId >> 8 );        // SMS Resource IDs MSB

            dataResp.Append( iResourceId );             // SMS resource IDs LSB

            dataResp.AppendFill( KPadding, 2 );         // Filler Bytes
            resource.CompleteSubBlock();

            // Add 2nd resource sequence ID subblock

            TIsiSubBlock resourceSeqId( dataResp, SMS_SB_RESOURCE_SEQ_ID ,
                EIsiSubBlockTypeId16Len16 );
            dataResp.Append( iSequenceId );              // Sequence ID
            dataResp.AppendFill( KPadding, 3 );
            resourceSeqId.CompleteSubBlock();

            // Add Third subblock

            TIsiSubBlock resourceStatus( dataResp, SMS_SB_RESOURCE_STATUS ,
                EIsiSubBlockTypeId16Len16 );
            dataResp.Append( SMS_RESOURCE_DENIED );      // Resource status
            dataResp.AppendFill( KPadding, 3 );
            resourceStatus.CompleteSubBlock();

            // No address and User data Subblocks because Here resource ID is not the activated

            // Add cause subblock

            TIsiSubBlock sbCause( dataResp,SMS_SB_CAUSE,
                    EIsiSubBlockTypeId16Len16 );

            dataResp.Append( SMS_CAUSE_TYPE_COMMON );
            dataResp.Append( SMS_ERR_ROUTE_NOT_ALLOWED );
            dataResp.Append( KPadding, 2 );

            // Send resp to Sms server with not allowed status
            iSatMessHandler->SendSmsResourceReq(
                iMoSmsCtrlReqTransactionId,
                dataResp,
                iSenderObject );

            }
        }

    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::SendMoSmsCtrlEnvelope
// Send an envelope to SIM server in order to check wheter the SMS
// can be sent or not. The response will be used to send a response
// to SMS server
// -----------------------------------------------------------------------------
//
void CSatMoSmsCtrl::SendMoSmsCtrlEnvelope
        (
        TUint8 aTraId,
        TDes8& aAddressData1,
        TDes8& aAddressData2
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_SENDMOSMSCTRLENVELOPE, "CSatMoSmsCtrl::SendMoSmsCtrlEnvelope" );
    TFLOGSTRING("TSY:CSatMoSmsCtrl::SendMoSmsCtrlEnvelope");

    // Get location info
    const CSatMessHandler::TLocationInfo& locInfo
        = iSatMessHandler->LocationInfo();

    // A SwDataNtf is expected later
    iWaitingForEnvelopeResp = ETrue;

    TTlv envelope;

    envelope.Begin( KBerTlvMoShortMessageControlTag );
    //device identities
    envelope.AddTag( KTlvDeviceIdentityTag );
    envelope.AddByte( KMe );
    envelope.AddByte( KSim );
    // Address Data object 1
    // Ton and Npi is in the first byte of address data
    envelope.AddTag( KTlvAddressTag );
    envelope.AddData( aAddressData1 );
    // Address Data object 2
    // Ton and Npi is in the first byte of address data
    envelope.AddTag( KTlvAddressTag );
    envelope.AddData( aAddressData2 );
    // Location info
    envelope.AddTag(KTlvLocationInformationTag);

    // This part assumes that location info (NetCellInfoInd)
    // has been received
    // Mobile Country & Network Codes (MCC & MNC)
    envelope.AddData( locInfo.iOperatorCode );
    // Location Area Code (LAC)
    envelope.AddData( locInfo.iLac );
    // Cell Id
    envelope.AddData( locInfo.iCellId );


    // Send envelope only if its length is less than the maximum allowed
    // envelope length.

    iSatMessHandler->UiccCatReqEnvelope( aTraId, envelope.End() );

    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::ParseAtkSwDataNtf
// Extracts data from AtkSwDataNtf message data part.
// -----------------------------------------------------------------------------
//
void CSatMoSmsCtrl::ParseAtkSwDataNtf
        (
        TPtrC8& aAtkData,
        TDes8& aAddr1,
        TDes8& aAddr2,
        RSat::TAlphaId& aAlphaId,
        TBool& aEmptyAlphaId
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_PARSEATKSWDATANTF, "CSatMoSmsCtrl::ParseAtkSwDataNtf" );
    TFLOGSTRING("TSY:CSatMoSmsCtrl::ParseAtkSwDataNtf");

    // Input data:
    // aAtkData[0] : result
    // aAtkData[1][2] : length of BER-TLV, set n=1 if aAtkData[1]=0x81
    // aAtkData[2+n] : tag of first TLV
    // aAtkData[3+n][4+n] : length of first TLV, set nn=1 if aAtkData[3+n]=0x81
    // etc..

    // Comments drawn from SAT TS lab test cases:
    // - The SIM may insert a wild '0xD' character in addresses and therefore
    // the phone sw should prevent the sending of the SMS. At the time when this
    // comment is being written, it is not clear which part prevents it, and does
    // it really prevent it ? And this is not yet handled in SimAtkTsy.
    // - The SIM may insert a 0xF in addresses, and the phone sw should discard
    // all characters placed after the 0xF. At the time when this
    // comment is being written, it is not clear which part should do that,
    // and this is not yet handled in SimAtkTsy.

    // Check if the BER-TLV length is coded with 2 bytes.
    TInt indexInBerTlv( 1 );
    TUint8 tag( aAtkData[indexInBerTlv] );
    TUint8 lengthBerTlv( 0 );
    TInt n( (KTwoByteLengthCoding == tag)?1:0 );
    lengthBerTlv = aAtkData[1 + n];
    indexInBerTlv = 2 + n;

    // Extract TLVs and fill in output variables
    TBool address1_got( EFalse );
    while( lengthBerTlv )
        {
        // Get tag value without 'comprehension required' bit
        tag = aAtkData[indexInBerTlv]&0x7F;
        // Length of TLV with 2 bytes ?
        TInt nn( ( KTwoByteLengthCoding == aAtkData[indexInBerTlv+1] )?1:0 );
        // Length of TLV
        TInt lengthTlv( aAtkData[indexInBerTlv+nn+1] );
        // Check what TLV is received
        if ( ( KTlvAddressTag == tag )
            && ( !address1_got ) )
            {
            // RP address first
            address1_got = ETrue;
            // Copy data to output variable
            aAddr1.Copy( aAtkData.Mid( indexInBerTlv + nn + 2,
                lengthTlv ) );
            if ( (1<lengthTlv) &&
                 KReservedTonNpi1!=aAddr1[0] && KReservedTonNpi2!=aAddr1[0] )
                {
                CleanAddressData( aAddr1 );
                }
            }
        else if ( KTlvAddressTag == tag )
            {
            // TP address
            // Copy data to output variable
            aAddr2.Copy( aAtkData.Mid( indexInBerTlv + nn + 2,
                lengthTlv ) );
            if ( (1<lengthTlv) &&
                 KReservedTonNpi1!=aAddr2[0] && KReservedTonNpi2!=aAddr2[0] )
                {
                CleanAddressData( aAddr2 );
                }
            }
        else if ( KTlvAlphaIdentifierTag == tag )
            {
            // Alpha Id
            // Alpha id must be converted to TBuf16/unicode
            if ( lengthTlv )
                {
                TSatUtility::SetAlphaId( aAtkData.Mid( indexInBerTlv + nn + 2,
                    lengthTlv ), aAlphaId.iAlphaId );
                }
            else
                {
                aEmptyAlphaId = ETrue;
                }

            }
        else
            {
            TFLOGSTRING("TSY: CSatMoSmsCtrl::ParseAtkSwDataNtf, Tlv tag was not address/alpha id tag.");
            OstTrace0( TRACE_NORMAL, DUP1_CSATMOSMSCTRL_PARSEATKSWDATANTF, "CSatMoSmsCtrl::ParseAtkSwDataNtf, Tlv tag was not address/alpha id tag." );

            }

        // update length for the while loop
        lengthBerTlv -= ( lengthTlv+2+nn );
        // updata index for the while loop
        indexInBerTlv += ( lengthTlv+2+nn );
        }
    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::CleanAddressData
// This method cleans the input address data, so that all the bytes found
// after a 0xXF or a 0xFX are removed.
// -----------------------------------------------------------------------------
//
void CSatMoSmsCtrl::CleanAddressData
        (
        TDes8& aAddr
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_CLEANADDRESSDATA, "CSatMoSmsCtrl::CleanAddressData" );
    TFLOGSTRING("TSY: CSatMoSmsCtrl::CleanAddressData");
    // Starts from index 1, since index 0 is for TON/NPI
    for ( TInt i=1; i < aAddr.Length(); i++ )
        {
        if ( 0xF0==(aAddr[i]&0xF0) )
            {
            if ( ( i+1 ) < aAddr.Length() )
                {
                aAddr.Delete( i+1, aAddr.Length() - i - 1 );
                }
            break;
            }
        else if ( 0x0F==(aAddr[i]&0x0F) )
            {
            aAddr.Delete( i, aAddr.Length() - i );
            break;
            }
        }

    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::VerifySimRespData
// This method checks the consistency of the data sent by the SIM to the ME,
// when the call is allowed and modified.
// -----------------------------------------------------------------------------
//
TBool CSatMoSmsCtrl::VerifySimRespData
        (
        TDes8& aRPAddr,
        TDes8& aTPAddr
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_VERIFYSIMRESPDATA, "CSatMoSmsCtrl::VerifySimRespData" );
    TBool ret( ETrue );
    if ( ( 0==aRPAddr.Length() && 0==aTPAddr.Length() )
       || ( KMaxLengthAddr1Addr2 <= ( aRPAddr.Length()+aTPAddr.Length() ) )
       )
        {
        ret = EFalse;
        }
    else
        {
        // detect the presence of 'wild' values in the digits of the addresses
        for ( TInt i=0; i<aRPAddr.Length(); i++)
            {
            if ( ( 0xA<(0x0F&aRPAddr[i]) && 0xF>(0x0F&aRPAddr[i]) )
               ||( 0xA0<(0xF0&aRPAddr[i]) && 0xF0>(0xF0&aRPAddr[i]) ) )
                {
                ret = EFalse;
                break;
                }
            }
        for ( TInt i=0; i<aTPAddr.Length(); i++)
            {
            if ( ( 0x9<(0x0F&aTPAddr[i]) && 0xF>(0x0F&aTPAddr[i]) )
               ||( 0x90<(0xF0&aTPAddr[i]) && 0xF0>(0xF0&aTPAddr[i]) ) )
                {
                ret = EFalse;
                break;
                }
            }
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::FormSmsResourceReqSb
// Form subblock of SMS_RESOURCE_REQ
// -----------------------------------------------------------------------------
//

void CSatMoSmsCtrl::FormSmsResourceReqSb
       (const TUint8 status,
        TDes8& data,
        const TDes8& address1,
        const TDes8& address2
        )
    {
    // First three mandatory subblocks
    TIsiSubBlock resource( data, SMS_SB_RESOURCE ,
        EIsiSubBlockTypeId16Len16 );    // Message ID and Subblock length
    data.Append( iResourceId >> 8 );      // SMS resource IDs MSB same as received in indication
    data.Append( iResourceId );           // SMS resource IDs LSB
    data.AppendFill( KPadding, 2 );       // Filler Bytes
    resource.CompleteSubBlock();

    // Add Sequence ID subblock

    TIsiSubBlock resourceSeqId( data, SMS_SB_RESOURCE_SEQ_ID ,
        EIsiSubBlockTypeId16Len16 );
    data.Append( iSequenceId );           // Sequence ID same as received in indication message
    data.AppendFill( KPadding, 3 );
    resourceSeqId.CompleteSubBlock();

    // Add Resource Status subblock

    TIsiSubBlock resourceStatus( data, SMS_SB_RESOURCE_STATUS ,
        EIsiSubBlockTypeId16Len16 );
    data.Append( status );                // Resource status
    data.AppendFill( KPadding, 3 );
    resourceStatus.CompleteSubBlock();

    if(iIsMoSmsCtrlActivated)
        {

            //Resource is disallowed   ||    Resource is allowed without modification
        if((SMS_RESOURCE_ALLOWED != status)||(0 == address1.Length()))
              {
              // Increment subblock
              data[1]++;
              // Copy address subblocks from Indication message
              // Add Address Subblock
              TIsiSubBlock address( data, SMS_SB_ADDRESS ,
              EIsiSubBlockTypeId16Len16 );
              data.Append( SMS_SMSC_ADDRESS );            // Address type
              data.Append( iAddressSubblock.Length() );     // Address data length
              data.Append( iAddressSubblock );
              // Filler Bytes calculation
              TUint8 fillbytes = 0;
              if((6 + iAddressSubblock.Length())%4)
                  fillbytes = 4 - (( 6 + iAddressSubblock.Length())%4 );

              data.AppendFill( KPadding,fillbytes );
              address.CompleteSubBlock();

              // increment subblock
              data[1]++;
              // Add User Data Subblock
              TIsiSubBlock userData( data, SMS_SB_TPDU ,
                  EIsiSubBlockTypeId16Len16 );
              data.Append( iUserDataSubblock.Length() );        // data Length
              data.Append( KPadding );        // Filler Byte
              data.Append( iUserDataSubblock ); // Data Bytes

              // Filler Bytes calculation
              fillbytes = 0;
              if((6 + iUserDataSubblock.Length())%4)
                  fillbytes = 4-(( 6 + iUserDataSubblock.Length())%4 );

              data.AppendFill( KPadding,fillbytes );
              userData.CompleteSubBlock();

              }
        else
              {
              // when resource is allowed with modification
              // then copy SIM data in address subblock
              TUint8 fillBytes = 0;
              if ( address1.Length() )
                  {
                  // Add Address subblock
                  data[1]++;          // increase no of subblocks

                  TIsiSubBlock address( data, SMS_SB_ADDRESS ,
                      EIsiSubBlockTypeId16Len16 );
                  data.Append(SMS_SMSC_ADDRESS);            // Address Type

                  // Calculate needed number of filler bytes
                  while ( ( fillBytes + 6 + address1.Length() ) % 4 )
                      {
                      fillBytes++;
                      }
                  // Data in bytes
                  // First byte is the address length in 3GPP, GSM_0411 format
                  // "data length in bytes, including TON & NPI"
                  data.Append( address1.Length() );
                  // Actual address data
                  data.Append( address1 );
                  // Filler bytes
                  data.AppendFill( KPadding, fillBytes );

                  address.CompleteSubBlock();
                  }       // end of Service centre Address Subblock

                  // Add subblock for Address2 (Destination address, TP) if present
                  if ( address2.Length() )
                      {
                      // Add subblock
                      data[1]++;

                      // For User Data Subblock
                      TIsiSubBlock userDataSb( data, SMS_SB_TPDU,
                          EIsiSubBlockTypeId16Len16 );

                      // Calculate needed number of filler bytes
                      fillBytes = 0;
                      while ( ( fillBytes + 6 + address2.Length() ) % 4 )
                          {
                          fillBytes++;
                          }

                      data.Append( fillBytes + 6 + address2.Length() );        // Data length
                      data.Append( KPadding );                  // Filler Byte

                      // Check what type of TPDU to be sent
                      switch( iMessageType )
                          {
                          case KSmsCommandType:
                              {
                              // Data in bytes
                              // First byte is the address length in 3GPP, GSM_0340 format
                              // "number of useful semioctets ( = digits ), excluding TON & NPI "

                              if ( 1 == address2.Length() ||
                                 ( 1 < address2.Length() &&
                                 ( KReservedTonNpi1 == address2[0]
                                   || KReservedTonNpi2 == address2[0] ) )
                                 )
                                  {
                                  // Only Ton&Npi is present
                                  // or TON&NPI has a reserved value
                                  iUserDataSubblock[6] = 0x00;
                                  }
                              else if ( 0xF0 == ( address2[address2.Length() - 1] & 0xF0 ) )
                                  {
                                  // Odd length destination address
                                  // Destination address length ( in semioctets )
                                  iUserDataSubblock[5] = ( ( address2.Length() * 2 ) - 1 );
                                  }
                              else
                                  {
                                  // Even length destination address
                                  iUserDataSubblock[5] = ( address2.Length() * 2 );
                                  }


                              for( TUint8 i = 0; i < address2.Length(); i++)
                                  {
                                  iUserDataSubblock[i+6] = address2[i];
                                  }
                           break;
                              }
                          case KSmsSubmitType:
                              {
                               // Data in bytes
                               // First byte is the address length in 3GPP, GSM_0340 format
                               // "number of useful semioctets ( = digits ), excluding TON & NPI "
                               if ( 1 == address2.Length() ||
                                  ( 1 < address2.Length() &&
                                  ( KReservedTonNpi1 == address2[0]
                                  || KReservedTonNpi2 == address2[0] ) )
                                  )
                                   {
                                   // Only Ton&Npi is present
                                   // or TON6NPI has a reserved value
                                   iUserDataSubblock[3] = 0x00;
                                   }
                               else if ( 0xF0 == ( address2[address2.Length() - 1] & 0xF0 ) )
                                       {
                                       // Odd length destination address
                                       // Destination address lenght ( in semioctets )
                                       iUserDataSubblock[2] = ( ( address2.Length() * 2 ) - 1 );
                                       }
                                    else
                                       {
                                       // Even length destination address
                                       iUserDataSubblock[2] = ( address2.Length() * 2 );
                                       }
                               for(TUint8 i = 0; i<address2.Length(); i++)
                               {
                                    iUserDataSubblock[i+3] = address2[i];
                               }
                               break;
                              }
                          default:
                              {
                              break;
                              }
                          }
                      data.Append(iUserDataSubblock);

                      userDataSb.CompleteSubBlock();
                      }
              }
        if( SMS_RESOURCE_DENIED == status )
            {
            // Add SMS_SB_CAUSE subblock
            data[1]++;           //Increment no of subblocks
            TIsiSubBlock cause( data, SMS_SB_CAUSE ,
            EIsiSubBlockTypeId16Len16 );
            data.Append( SMS_CAUSE_TYPE_COMMON );            // Cause type
            data.Append( SMS_ERR_ROUTE_NOT_ALLOWED );    // Cause
            data.AppendFill( KPadding, 2 );
            cause.CompleteSubBlock();
            }
        }
    }
// -----------------------------------------------------------------------------
// CSatMoSmsCtrl::MessageReceived
// Handle received messages related to MO-SMS Control
// -----------------------------------------------------------------------------
//
TInt CSatMoSmsCtrl::MessageReceived
        (
        const TIsiReceiveC& aIsiMessage
        )
    {
    OstTrace0( TRACE_NORMAL, CSATMOSMSCTRL_MESSAGERECEIVED, "CSatMoSmsCtrl::MessageReceived" );
    TFLOGSTRING("TSY: CSatMoSmsCtrl::MessageReceived");
    // SMS server -> SimAtkTsy  : MO-SMS request sent to SAT (KSmsResourceReq)
    // UICC server <- SimAtkTsy  : Envelope sent to UICC server (KAtkEnvelopeNtf)
    // UICC server -> SimAtkTsy  : AtkSwNtf send to SAT, with modified (or not)
    //                            addresses and parameters  (KAtkSwDataNtf)
    // SMS server <- SimAtkTsy  : SAT sends final decision to SMS server
    //                            with modified (or not) addresses and parameters
    //                            (KSmsResourceResp)

    // Note: When SendSms proactive command is issued, and MO-SMS control
    // does not allow sending the SMS, then a terminal response with general result
    // "interaction with call control by SIM or MO short message control by SIM,
    // action not allowed".
    TUint8 resource = aIsiMessage.Get8bit( ISI_HEADER_OFFSET_RESOURCEID );

    if ( PN_SMS == resource )
        {
        switch ( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
            {
            case SMS_RESOURCE_IND:
                {
                SmsResourceIndReceived( aIsiMessage );
                break;
                }
            case SMS_RESOURCE_RESP:
                {
                // Take a log
                TFLOGSTRING("TSY:CSatMoSmsCtrl::SmsResourceRespReceived: Response for Resource Req, Receive response from SMS server");
                OstTrace0( TRACE_NORMAL, DUP1_CSATMOSMSCTRL_SMSRESOURCERESPRECEIVED, "CSatMoSmsCtrl::SmsResourceRespReceived: Response for Resource Req, Receive response from SMS server" );

                break;
                }
            default:
                {
                // none
                break;
                }
            }
        }
    else if ( PN_UICC == resource )
        {
        switch( aIsiMessage.Get8bit( ISI_HEADER_OFFSET_MESSAGEID ) )
            {
            case UICC_CAT_RESP:
                {
                // In case of envelope response handle the data
                TUint8 serviceType(
                    aIsiMessage.Get8bit(
                        ISI_HEADER_SIZE + UICC_CAT_RESP_OFFSET_SERVICETYPE ) );
                if ( UICC_CAT_ENVELOPE == serviceType )
                    {
                    if ( iWaitingForEnvelopeResp &&
                         ( iMoSmsCtrlEnvelopeTransactionId ==
                            aIsiMessage.Get8bit( ISI_HEADER_OFFSET_TRANSID ) ) )
                        {
                        // Set flag iWaitingForEnvelopeResp false
                        // since a response is received for the envelope
                        // that was sent for a received sms-pp message
                        iWaitingForEnvelopeResp = EFalse;
                        UiccCatRespEnvelopeReceived( aIsiMessage );
                        }
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    return KErrNone;
    }

// End of File
