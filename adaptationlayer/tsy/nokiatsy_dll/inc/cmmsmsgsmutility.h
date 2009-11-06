/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _CMMSMSGSMUTILITY_H_
#define _CMMSMSGSMUTILITY_H_

#include <e32base.h>
#include <ctsy/serviceapi/cmmsmsutility.h>

//  CONSTANTS
const TUint8 KSmsMTIDeliverOrDeliverReport              =   0x00;
const TUint8 KSmsMTISubmitOrSubmitReport                =   0x01;
const TUint8 KSmsMTIStatusReportOrCommand               =   0x02;
const TUint8 KSmsMTIUndefined                           =   0x03;
const TUint8 KSmsValidityPeriodMaxLength                =   7;
const TUint8 KSmsValidityPeriodFormatVPF                =   0x18;
const TUint8 KSmsDelRepParamIndExtensionBit             =   0x01;
const TUint8 KSimSbSmsGsmIsiMsgMaxLength				=   233;
const TInt8  KSimSmsFirstLocIndex						=	1;

// SMS_GSM_PARAM_INDICATOR sub blocks Parameter Indicators
const TUint8 KSmsParametersIndProtocolId                =   0x01;
const TUint8 KSmsParametersIndDataCodingScheme          =   0x02;
const TUint8 KSmsParametersIndUserData                  =   0x04;
const TUint8 KSmsParametersIndMask                      =   0x07;

// Max. length of SMS service center name. Copied from etelmm.h, this constant is
// defined there within an anonymous enum (i.e. we cannot use it directly).
const TUint8 KMaxSmspTextSize = 30;

//Max length of network proveder name
const TUint8 KOperatorNameMaxLength = 25;

//Orange network id
const TUint8 KSmsMncOrange = 33;

//UK country codes
const TUint16 KSmsMccUk1 = 234;
const TUint16 KSmsMccUk2 = 235;

//Constants for Orange Moses orginator address handling
const TUint8 KSmsOrangeMosesDisplayAddr         =   0x01;
const TUint8 KSmsOrangeMosesEmptyAddr           =   0x02;

const TUint8 KSmsOrangeMosesAddrLength          =   0x0B; 
const TUint8 KSmsOrangeMosesTypeOfAddr          =   0xD0;
const TUint8 KSmsOrangeMosesAddrValue1          =   0x40;
const TUint8 KSmsOrangeMosesAddrValue2          =   0x4D;
const TUint8 KSmsOrangeMosesAddrValue3          =   0x64;
const TUint8 KSmsOrangeMosesAddrValue4          =   0xD3;
const TUint8 KSmsOrangeMosesAddrValue5          =   0x50;
const TUint8 KSmsOrangeMosesAddrValue6          =   0x00;

// All memory locations are erased
const TUint8 KDeleteAllSmsMessagesIndex = 0;

//Info length is ignored for WCDMA CBS Message
const TUint8 KInfoLengthIgnored = 0xFF;

//Cbs Message Type
const TUint8 KCbsMessageType = 1;

//Number Maximun of SubBlocks in a WCDMA CBS Notification
const TUint8 KWcdmaCbsNumberOfSbMax = 15;

#endif  // _CMMSMSGSMUTILITY_H_
            
// End of File


