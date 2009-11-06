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



/* ------------------------------------------------------------------------- */


/*  1    ABSTRACT

    1.1    Module type

    Header file for the ISI Autogeneration Application


    1.2    Functional description

    This file is the message definition file for the Network Select Server


    1.3    Notes


    2    CONTENTS


    1    ABSTRACT
    1.1    Module type
    1.2    Functional description
    1.3    Notes

    2    CONTENTS

    3    CONSTANTS

    4    MACROS

    5    DATA TYPES

    6    CLASSES

    7    OBJECTS

    8    DATA STRUCTURES

    9    FUNCTION PROTOTYPES
*/


/*  3    CONSTANTS */


/* Message ID's */

/* default ISI version */
#ifndef S60_FTD_WLAN_VERSION
#define S60_FTD_WLAN_VERSION
#define S60_FTD_WLAN_VERSION_Z   1
#define S60_FTD_WLAN_VERSION_Y   0
#endif


    /* --------------------------------------------------------------------- */
    /* Project / Document information */
    /* --------------------------------------------------------------------- */

RES_BEGIN(RESOURCE_ISI, PN_SOS_WLAN_EXTENDED, S60WLAN, "S60 WLAN",
          RES_VERSION_STRING(S60_FTD_WLAN_VERSION_Z, S60_FTD_WLAN_VERSION_Y), TRUE)

    /* --------------------------------------------------------------------- */
    /* Component description */
    /* --------------------------------------------------------------------- */

    /* --------------------------------------------------------------------- */
    /* Version history                                                   */
    /* --------------------------------------------------------------------- */

    ISI_VERSION_COMMENT(COMMENT_CHANGE_REASON,    "First version of document.")
    ISI_VERSION_COMMENT(COMMENT_CHANGE_REFERENCE, "ADB for xxxxxxx.")
    /* state DRAFT, NON-DRAFT, PROPOSAL or APPROVED */
    ISI_VERSION_COMMENT(COMMENT_CHANGE_STATUS,    "NON-DRAFT")
    ISI_VERSION_COMMENT(COMMENT_CHANGE_DATE,      "")
    ISI_VERSION_COMMENT(COMMENT_CHANGE_PERSON,    "")
    ISI_VERSION_COMMENT(COMMENT_PARAGRAPH,
        "Parturition.")
    ISI_VERSION_HISTORY("001.000")

    /* Field test display data */

    /* --------------------------------------------------------------------- */
    /* FTD Data Table                                                        */
    /* --------------------------------------------------------------------- */


    FTD_TBL_BEGIN("", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_OWN_MAC_ADDR,                  0x0011,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_HW_VERSION,                    0x0012,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE(FTD_SB_WLAN_FW_VERSION,                     0x0013,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_SSID,                          0x0014,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_AP_MAC_ADDR,                   0x0015,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_CHANNEL,                       0x0016,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RSSI,                          0x0017,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE(FTD_SB_WLAN_RATES_11B,                      0x0018,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RATES_11G,                     0x0019,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_CAPABILITY_LSB,                0x001A,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_CAPABILITY_MSB,                0x001B,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_AV_TX_RATE,                    0x001C,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_AV_RX_RATE,                    0x001D,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE(FTD_SB_WLAN_UL_THROUGHPUT,                  0x001E,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_DL_THROUGHPUT,                 0x001F,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_UL_BYTES,                      0x0020,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE(FTD_SB_WLAN_DL_BYTES,                       0x0021,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_BLACKLIST_ADDR_1,              0x0022,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_BLACKLIST_ADDR_2,              0x0023,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE(FTD_SB_WLAN_BLACKLIST_ADDR_3,               0x0024,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_BLACKLIST_ADDR_4,              0x0025,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_SEC_MODE,                      0x0026,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_REG_DOMAIN,                    0x0027,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_CONNECTION_STATE,              0x0028,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_TX_POWER,                      0x002B,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_POWER_SAVE,                    0x002C,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_TX_FRAMES,                     0x002D,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RX_UC_FRAMES,                  0x002E,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RX_MC_FRAMES,                  0x002F,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_TX_ERRORS,                     0x0030,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_TX_RETRANS,                    0x0031,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RX_CRC_ERRORS,                 0x0032,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_CONNECTIONS,                   0x0033,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_CONNECTION_FAILS,              0x0034,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_ROAMINGS,                      0x0035,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_COVERAGE_LOSSES,               0x0036,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RESET_COUNTERS,                0x0037,  8, DEC, "", "")
 
        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RD_CONTROL_SET,                0x0038,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_RD_CONTROL_GET,                0x0039,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_ROAM_TIME_TOTAL,               0x003A,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_ROAM_TIME_DATA_PATH,           0x003B,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_WLAN_ROAM_CAUSE,                    0x003C,  8, DEC, "", "")

	FTD_TBL_END

    /* Constants */

    /* Symbol tables */

RES_END


/*  5    DATA TYPES */

/*  6    CLASSES */

/*  7    OBJECTS */

/*  8    DATA STRUCTURES */

/*  9    FUNCTION PROTOTYPES */


/*  End of file  */
