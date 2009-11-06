/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Internal flags for NokiaTSY
*
*/



#ifndef NOKIATSY_INTERNAL_VARIATION_H
#define NOKIATSY_INTERNAL_VARIATION_H

//#define INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
/*
     Name                           : INTERNAL_RD_USIM_PHONEBOOK_GAS_AND_AAS
     Description                    : The flag is needed for RnD feature variation in NokiaTSY in NCP 5.1.
                                    : Flag to control support for Support for Additional number alpha strings
                                    : in USIM and Support for SIM based contact grouping.
     Flag Type(temporary/permanent) : temporary
     If feature is on               : NokiaTSY's support for GAS and AAS is enabled.
     If feature is off              : NokiaTSY's support for GAS and AAS is disabled.
     Affected modules               : NokiaTSY
     Other comments                 : Default setting is OFF.
                                    : If enabled also CommonTSY and CustomAPI flag
                                    : RD_USIM_PHONEBOOK_GAS_AND_AAS needs to be enabled. 
*/

//#define DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
/*
     Name                           : DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING_DOS
     Description                    : Flag to control dummy_pep feature
     Flag Type(temporary/permanent) : permanent
     If feature is on               : When flag is on DUMMY_NIF_PEP_FOR_PACKET_DATA is enabled.
     If feature is off              : When flag is off DUMMY_NIF_PEP_FOR_PACKET_DATA is disabled.
     Affected modules               : NokiaTSY
     Other comments                 : DUMMY_NIF flag should be off in builds and releases.
                                    : DUMMY_NIF flag should be off by default in builds.
                                    : DUMMY_NIF flag should be on if want test DUMMY_NIF_PEP_FOR_PACKET_DATA.
*/

//#define INTERNAL_TESTING_SMS_WITH_EMULATOR
/*
     Name                           : INTERNAL_TESTING_SMS_WITH_EMULATOR
     Description                    : Flag to mask out sending SMS_PP_ROUTING_REQ when SMS routing is activated by requesting
                                      EMmTsyActivateSmsRouting WHEN EMULATOR IS ATTACHED TO PHONE
                                      If emulator is used this flag MUST be defined to handle SMS routing correctly.
                                      This flag will disable SMS routing, when the flag is on it will not be possible to
                                      receive SMS except by emulator. When using emulator this flag should be defined ON in 
                                      the image used together with emulator.
     Flag Type(temporary/permanent) : permanent
     If feature is on               : When flag is on INTERNAL_TESTING_SMS_WITH_EMULATOR is enabled.
     If feature is off              : When flag is off INTERNAL_TESTING_SMS_WITH_EMULATOR is disabled.
     Affected modules               : NokiaTSY
     Other comments                 : This flag should be off in builds and releases.
                                    : This flag should be off by default in builds when emulator is NOT used
                                    : This flag should be on if build is made for EMULATOR USE.
*/

//#define INTERNAL_TESTING_CASW_INITIAL_DCH_RATE
/*
     Name                           : INTERNAL_TESTING_CASW_INITIAL_DCH_RATE
     Description                    : Initial DCH Rate is needed in the Streaming Application during stream negotiation with server.
                                      This NCP Feature flag is Hutchinson specific flag and need to be set OFF as default.
                                      Sets pointer subblock to GPDS_SHARED_INITIAL_DL_DCH_RATE (0xFC) subblock.
                                      At least NokiaTSY_TEST_CASES PDI_0009_701-703 should be run with this flag DEFINED and UNDEFINED
                                      Flag Type(temporary/permanent) : permanent
     If feature is on               : When flag is on INTERNAL_TESTING_CASW_INITIAL_DCH_RATE is enabled.
     If feature is off              : When flag is off INTERNAL_TESTING_CASW_INITIAL_DCH_RATE is disabled.
     Affected modules               : NokiaTSY
     Other comments                 : This flag should be off in builds and releases.
                                    : This flag should be off by default in builds when emulator is NOT used
                                    : This flag should be on if testing INTERNAL_TESTING_CASW_INITIAL_DCH_RATE.
*/

//#define INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT
/*
     Name                           : INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT
     Description                    : This flag is for testing features behind binary variation flag CASW_HSXPA_INDICATOR_SUPPORTED. 
                                      Flag Type(temporary/permanent) : temporary, 
                                      this flag can be removed when CASW_HSXPA_INDICATOR_SUPPORTED -flag is removed.                                      
     If feature is on               : When flag is on INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT is enabled.
     If feature is off              : When flag is off INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT is disabled.
     Affected modules               : NokiaTSY
     Other comments                 : This flag should be off in builds and releases.
                                    : This flag should be off by default in builds when emulator is NOT used
                                    : This flag should be on if testing INTERNAL_TESTING_CASW_HSXPA_INDICATOR_SUPPORT.
*/

//#define INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
/*
     Name                           : INTERNAL_TESTING_OLD_IMPLEMENTATION_FOR_UICC_TESTING
     Description                    : This flag is for testing UICC on 9.2 based environment. 
     If feature is on               : When flag is on PN_INFO, PN_NAME, PNS_PIPE, PNS_PEP and PN_MTC are supported.
     If feature is off              : When flag is off the new interfaces are used.
     Affected modules               : NokiaTSY
     Other comments                 : This flag should be on only with UICC testing on 9.2 based environment.
*/

#endif // NOKIATSY_INTERNAL_VARIATION_H

// End of File
