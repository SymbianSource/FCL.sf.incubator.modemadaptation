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
#ifndef S60_FTD_DVBH_VERSION
#define S60_FTD_DVBH_VERSION
#define S60_FTD_DVBH_VERSION_Z   1
#define S60_FTD_DVBH_VERSION_Y   0
#endif


    /* --------------------------------------------------------------------- */
    /* Project / Document information */
    /* --------------------------------------------------------------------- */
/*
PROPERTY_STRING(PROPERTY_PROJECT_SITE_NAME, "")
PROPERTY_STRING(PROPERTY_PROJECT_NAME, "CN")
PROPERTY_STRING(PROPERTY_PROJECT_OWNER_NAME, "")
PROPERTY_STRING(PROPERTY_PROJECT_CONFIDENTIALITY, "CONFIDENTIAL")

PROPERTY_STRING(PROPERTY_ISIHDR_TITLE, "")
PROPERTY_STRING(PROPERTY_ISIHDR_FILENAME, "srv_net_isi.h")
PROPERTY_STRING(PROPERTY_ISIHDR_FILENAME_16, "")
PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_PROJECT_DB, "ou1gif")
PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_PROJECT_PATH, "global_interface/ISI/Protocol")
PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_PROJECT_PATH_16, "")
PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_INSTANCE_NAME, "ou_mcu#2")

PROPERTY_STRING(PROPERTY_HTML_CONTINUUS_PROJECT_DB, "ou1gif")
PROPERTY_STRING(PROPERTY_HTML_CONTINUUS_PROJECT_NAME, "GIF_SPECS")
PROPERTY_STRING(PROPERTY_HTML_DIRECTORY_PATH, "GIF_SPECS/Protocol")
PROPERTY_STRING(PROPERTY_HTML_DOCUMENT_NUMBER, "DSX01942-EN")
PROPERTY_STRING(PROPERTY_HTML_FILENAME, "i_net_isi_m.html")
PROPERTY_STRING(PROPERTY_HTML_TITLE, "ISI NET Server Message Descriptions")
PROPERTY(PROPERTY_HTML_INCLUDE_INTRODUCTION)
*/

RES_BEGIN(RESOURCE_ISI, PN_SOS_DVBH_EXTENDED, S60DVBH, "S60 DVB-H",
          RES_VERSION_STRING(S60_FTD_DVBH_VERSION_Z, S60_FTD_DVBH_VERSION_Y), TRUE)

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
    ISI_VERSION_COMMENT(COMMENT_CHANGE_DATE,      "19-Oct-2007")
    ISI_VERSION_COMMENT(COMMENT_CHANGE_PERSON,    "")
    ISI_VERSION_COMMENT(COMMENT_PARAGRAPH,
        "Parturition.")
    ISI_VERSION_HISTORY("001.000")

    /* Field test display data */

    /* --------------------------------------------------------------------- */
    /* FTD Data Table                                                        */
    /* --------------------------------------------------------------------- */

    FTD_TBL_BEGIN("", "")

        PROPERTY( PROPERTY_FTD_IS_INPUT_DATA )
        FTD_VALUE_BEGIN( FTD_SB_DVB_H_1 , 0x0010, "", "" )
            VALUE( 32, NATIVE, reset_stats, "ftd_sb_dvb_h_1", DEC )
        FTD_VALUE_END

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_2,            0x0011,  8, DEC, "", "")

        FTD_TBL_ENTRY_STRING(FTD_SB_DVB_H_3,            0x0012, " ", " ")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_4,            0x0013,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_5,            0x0014,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_6,            0x0015,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_7,            0x0016,  8, DEC, "", "")

        FTD_TBL_ENTRY_STRING(FTD_SB_DVB_H_8,            0x0017, " ", " ")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_9,            0x0018,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_10,           0x0019,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_11,           0x001A,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_12,           0x001B,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_13,           0x001C,  8, DEC, "", "")

        FTD_TBL_ENTRY_STRING(FTD_SB_DVB_H_14,           0x001D, " ", " ")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_15,           0x001E,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_16,           0x001F,  8, DEC, "", "")

        FTD_TBL_ENTRY_STRING(FTD_SB_DVB_H_17,           0x0020, " ", " ")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_18,           0x0021,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_19,           0x0022,  8, DEC, "", "")

        FTD_TBL_ENTRY_STRING(FTD_SB_DVB_H_20,           0x0023, " ", " ")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_21,           0x0024,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_22,           0x0025,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_23,           0x0026,  8, DEC, "", "")

        FTD_TBL_ENTRY_VALUE (FTD_SB_DVB_H_24,           0x0027,  8, DEC, "", "")

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
