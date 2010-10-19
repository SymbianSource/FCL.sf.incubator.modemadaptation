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
* Name      : pn_const.h
*
*/


#ifndef PN_CONST_H
#define PN_CONST_H


/* === Media === */
#define PN_MEDIA_XTI                    0x10  /* Trace interface                     */
#define PN_MEDIA_MODEM_HOST_IF          0x26  /* Modem-host interface                */

/* === Devices === */
#define PN_DEV_HOST                     0x00  /* Host                                */
#define PN_DEV_MODEM                    0x60  /* Modem                               */

/* === Objects === */
#define PN_OBJ_ROUTING_REQ              0x00  /* Resource-based routing              */
#define PN_OBJ_EVENT_MULTICAST          0x20  /* Indications are sent to this object */
#define PN_OBJ_PEP_GPDS                 0x30  /* Pipe-End-Point, General Packet Data Service    */

/* === Resources/Servers === */
#define PN_SMS                          0x02  /* Short Message Services              */
#define PN_SS                           0x06  /* Supplementary Services              */
#define PN_COMMGR                       0x10  /* Indication subscription messages    */
#define PN_CSD                          0x16  /* Circuit Switched Data               */
#define PN_GPDS                         0x31  /* General Packet Radio System server  */
#define PN_GSS                          0x32  /* GSM Stack server                    */
#define PN_UICC                         0x8C  /* Universal Integrated Circuit Card   */
#define PN_AT_MODEM                     0x8E  /* Modem AT server                     */
#define PN_MODEM_LCS                    0x90  /* Modem LCS server                    */
#define PN_MODEM_TEST                   0x91  /* Modem Test server                   */
#define PN_MODEM_NVD                    0x92  /* Modem Non Volatile Data server      */
#define PN_MODEM_PERM                   0x93  /* Modem PERManent Data server         */
#define PN_RF_HAL                       0xB7  /* RF control and tuning server        */
#define PN_MODEM_MCE                    0xC2  /* Modem MCE server                    */
#define PN_MODEM_MON                    0xC3  /* Modem Monitor server                */
#define PN_MODEM_INFO                   0xC5  /* Modem Info Server                   */
#define PN_MODEM_NETWORK                0xC8  /* Modem NET server                    */
#define PN_MODEM_CALL                   0xC9  /* Modem CALL server                   */
#define PN_MODEM_GAN                    0xCB  /* 3GPP GAN Protocol Stack server      */
#define PN_PIPE                         0xD9  /* Pipe messages                       */
#define PN_COMMON_RF_TEST               0xEC  /* Common RF test server               */
#define PN_WCDMA_RF_TEST                0xEE  /* WCDMA RF test server                */
#define PN_GSM_RF_TEST                  0xF1  /* GSM test server                     */

/* === NOTE! Next definitions are temporary System resources from non MeXe pn_const === */

#define PN_HEADER_SIZE                  0x06

#define PN_DEV_GLOBAL                   0xFF /* 255 */

#define PN_PREFIX                       224 /* 0xE0 */

/* Macro handling extended resource id's */
#define PN_RES(Type, SubType)    (PN_PREFIX  + (Type << 8) + (SubType <<16))

//#define PN_NUMBER_OF_EXTEND_RES       1 /* Number of extended resources */

#define PN_SOS_DMC_EXTENDED             PN_RES(PN_RES_TYPE_SOS,2)   /* 0x0202E0  [S] Symbian Device Mode Controller */


#endif /* PN_CONST_H */

/* End of File */
