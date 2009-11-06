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
* Name      : Product Profile bits
* Version   : 4
*
*/


#ifndef _PRODUCT_PROFILE_DEFINITIONS_H_
#define _PRODUCT_PROFILE_DEFINITIONS_H_


#define INFO_PP_FALSE 0
#define INFO_PP_TRUE  1

#define INFO_PP_POC_SUPPORT 35

#define INFO_PP_WLAN_SUPPORT 264

#define INFO_PP_ALS 1

#define INFO_PP_ATK_MO_SM_CONTROL 158

#define INFO_PP_CAMERA_FEA 156

#define INFO_PP_LOCATION_METHOD 288
#define INFO_PP_LOCATION_METHOD_BLUETOOTH_GPS            0x0001
#define INFO_PP_LOCATION_METHOD_STANDALONE_GPS           0x0002
#define INFO_PP_LOCATION_METHOD_MO_ADR                   0x0004
#define INFO_PP_LOCATION_METHOD_MO_LR                    0x0008
#define INFO_PP_LOCATION_METHOD_MTLR_STANDALONE_GPS      0x0010
#define INFO_PP_LOCATION_METHOD_MTLR_UE_MS_BASED_AGPS    0x0020
#define INFO_PP_LOCATION_METHOD_MTLR_UE_MS_ASSISTED_AGPS 0x0040

#define INFO_PP_LOCATION_COMMUNICATION_CHANNEL 289
#define INFO_PP_LOCATION_COMM_USER_PLANE                 0x0001
#define INFO_PP_LOCATION_COMM_CTRL_PLANE                 0x0002
#define INFO_PP_LOCATION_COMM_PRIORITY_PLANE             0x0004
#define INFO_PP_LOCATION_COMM_ADAPT_PLANE                0x0008

#define INFO_PP_WCDMA_HS_CHANNEL_SUPPORT                        294
#define INFO_PP_WCDMA_HS_CHANNEL_SUPPORT_AS_CATEGORY_CONFIGURED 	0
#define INFO_PP_WCDMA_HS_CHANNEL_SUPPORT_HSUPA_DISABLED		1
#define INFO_PP_WCDMA_HS_CHANNEL_SUPPORT_HSDPA_HSUPA_DISABLED  	2
#define INFO_PP_HSXPA_USER_SETTING_CONTROL                      309
#define INFO_PP_HSXPA_USER_SETTING_CONTROL_MASK_INIT	   0x01 
#define INFO_PP_HSXPA_USER_SETTING_CONTROL_MASK_MENU	   0x10

#define INFO_PP_EONS_FEAT            214
#define INFO_PP_EONS_FEAT_0            0
#define INFO_PP_EONS_FEAT_1            1
#define INFO_PP_EONS_FEAT_2            2
#define INFO_PP_EONS_FEAT_3            3

//                        0     Phone will always assume that there is a name
//                              for HPLMN in PNN#1, without checking rules
//                              from EF-OPL first. Operator do not need to
//                              configure any rules for HPLMN, if there is no
//                              need to show different names in different parts
//                              of their network.
//
//                        1     Phone will check if there are rules for HPLMN
//                              in EF-OPL list before using PNN#1 for HPLMN.
//                              Enhancement to 1, but it makes configuration of
//                              rules for national roaming partners a bit
//                              harder for operator.
//
//                        2     Phone will never assume that there is a name
//                              for HPLMN in PNN#1. It will show EONS name for
//                              HPLMN only when it is found from EF-OPL, except
//                              when EF-OPL does not exist at all.
//
//                        3     Phone will never assume that there is a name
//                              for HPLMN in PNN#1. It will show EONS name for
//                              HPLMN only when it is found from EF-OPL, except
//                              when EF-OPL does not exist at all or when only
//                              one PNN record exists in the card.

#define INFO_PP_ENHANCED_NETWORK_SELECTION 61

// Runtime Control in Thermal Management software for Certification Tests
#define INFO_PP_DISABLE_THERMAL_MANAGER              334

#endif
