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

           Message definition file for autogeneration
   

    1.2    Functional description

           This file is the message definition file for the SOS FTD.


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

#include "pn_const.h"
/* Message ID's */

#define SOS_FTD_ACTIVATE_REQ                0x0000
#define SOS_FTD_ACTIVATE_RESP               0x0001
#define SOS_FTD_DEACTIVATE_REQ              0x0002
#define SOS_FTD_DEACTIVATE_RESP             0x0003
#define SOS_FTD_EXECUTE_REQ                 0x0004
#define SOS_FTD_EXECUTE_RESP                0x0005
#define SOS_FTD_VERSION_GET_REQ             0x0006
#define SOS_FTD_VERSION_GET_RESP            0x0007

/* Sub block ID's */

#define SOS_FTD_DATA_STATUS                 0x0000
#define SOS_FTD_DATA_BYTE                   0x0001
#define SOS_FTD_DATA_WORD                   0x0002
#define SOS_FTD_DATA_ALL                    0x0003
#define SOS_FTD_DATA_DWORD                  0x0004
#define SOS_FTD_DATA_STRING                 0x0005
#define SOS_FTD_DATA_BYTEARRAY              0x0006

/* Constants */

#define MAX_FTD_DATA_ITEMS                      255
#define SOS_FTD_MAX_STR_LEN                     65524
#define SOS_FTD_INPUT_PROPERTY                  0x01
#define SOS_FTD_OUTPUT_PROPERTY                 0x02
#define SOS_FTD_BYTE_VALUE_NOT_DEFINED          0x00
#define SOS_FTD_WORD_VALUE_NOT_DEFINED          0x0000
#define SOS_FTD_DWORD_VALUE_NOT_DEFINED         0x00000000
#define SOS_FTD_STRING_VALUE_NOT_DEFINED        0x00
#define SOS_FTD_BYTEARRAY_VALUE_NOT_DEFINED     0x00

#define KPSFIELDTESTUID                         0x102824C5


/* MACROS */

#define PN_SOS_FTD_EXTENDED  \
            PN_RES(PN_RES_TYPE_SOS_FIELD_TEST, 1)


#define SOS_SB_HDR \
    REFERENCE(16, BE, id, "Data item ID", SB_ID) \
    REFERENCE(16, BE, length, "Item length", SB_LENGTH)

#define SOS_SB_STATUS_HDR \
    REFERENCE(16, BE, id, "Status item ID", SB_ID) \
    REFERENCE(16, BE, length, "Status item length", SB_LENGTH)


#ifndef PN_RES_TYPE_SOS_FIELD_TEST_VERSION
#define PN_RES_TYPE_SOS_FIELD_TEST_VERSION
#define PN_RES_TYPE_SOS_FIELD_TEST_VERSION_Z      001
#define PN_RES_TYPE_SOS_FIELD_TEST_VERSION_Y      001
#endif

/* --------------------------------------------------------------------- */
/* Project / Document information */
/* --------------------------------------------------------------------- */

/* Project / Document properties */
PROPERTY_STRING(PROPERTY_PROJECT_SITE_NAME,                 "")
PROPERTY_STRING(PROPERTY_PROJECT_NAME,                      "")
PROPERTY_STRING(PROPERTY_PROJECT_OWNER_NAME,                "")
PROPERTY_STRING(PROPERTY_PROJECT_CONFIDENTIALITY,           "")

/* ISI header properties */
PROPERTY_STRING(PROPERTY_ISIHDR_TITLE,                      "SOS FTD, include file")
PROPERTY_STRING(PROPERTY_ISIHDR_FILENAME,                   "sos_ftd_isi.h")
/*PROPERTY_STRING(PROPERTY_ISIHDR_FILENAME_16,                "-")*/
PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_PROJECT_DB,       "")
PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_PROJECT_PATH,     "global_interface/ISI/Protocol")
/*PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_PROJECT_PATH_16,  "-")*/
PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_INSTANCE_NAME,    "-")
/*PROPERTY_STRING(PROPERTY_ISIHDR_CONTINUUS_INSTANCE_NAME_16, "-")*/

/* HTML properties 
PROPERTY_STRING(PROPERTY_HTML_TITLE,                        "SOS FTD, file descriptions")
PROPERTY_STRING(PROPERTY_HTML_FILENAME,                     "i_sos_ftd.html")
PROPERTY_STRING(PROPERTY_HTML_DOCUMENT_NUMBER,              "-")
PROPERTY_STRING(PROPERTY_HTML_CONTINUUS_PROJECT_DB,         "")
PROPERTY_STRING(PROPERTY_HTML_CONTINUUS_PROJECT_NAME,       "")
PROPERTY_STRING(PROPERTY_HTML_DIRECTORY_PATH,               "")
PROPERTY(PROPERTY_HTML_INCLUDE_INTRODUCTION)
*/
PROPERTY(PROPERTY_ISIHDR_GENERATE_ALL_STRUCTURE_ELEMENTS)
RES_BEGIN(RESOURCE_ISI, PN_RES_TYPE_SOS_FIELD_TEST, SOS_FTD, "SOS FTD", 
    RES_VERSION_STRING(PN_RES_TYPE_SOS_FIELD_TEST_VERSION_Z, PN_RES_TYPE_SOS_FIELD_TEST_VERSION_Y), FALSE)


    /* --------------------------------------------------------------------- */
    /* File description */
    /* --------------------------------------------------------------------- */

    RES_COMMENT(COMMENT_PARAGRAPH, "This file includes SOS FTD definitions")

    /* --------------------------------------------------------------------- */
    /* ISI version history */
    /* --------------------------------------------------------------------- */



ISI_VERSION_COMMENT(COMMENT_TEXT, "The first version has been generated. ");
ISI_VERSION_HISTORY("000.001")


    /* --------------------------------------------------------------------- */
    /* Notes / references */
    /* --------------------------------------------------------------------- */


    /* --------------------------------------------------------------------- */
    /* Symbols */
    /* --------------------------------------------------------------------- */


    /* FTD definitions */

    /* --------------------------------------------------------------------- */
    /* Constant tables */
    /* --------------------------------------------------------------------- */


    CONST_TBL_BEGIN(SOS_FTD_DATA_STATUSES, 8, BE, "000.001", "")
        CONST_TBL_ENTRY(SOS_FTD_DATA_OK,                    0x00, 
            "FTD Data OK. ")
        CONST_TBL_ENTRY(SOS_FTD_DATA_FAIL,                  0x01, 
            "FTD Data fail.")
        CONST_TBL_ENTRY(SOS_FTD_DATA_NOT_ACTIVATED,         0x02, 
            "FTD data activated.")
        CONST_TBL_ENTRY(SOS_FTD_DATA_NOT_SUPPORTED,         0x03, 
            "FTD data not supported.")
        CONST_TBL_ENTRY(SOS_FTD_DATA_NOT_AVAILABLE,             0x04, 
            "FTD data NOT available.")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_STRING_TYPE, 8, BE, "000.001", "")
        CONST_TBL_ENTRY(SOS_FTD_STRING_ASCII,      0x00, "")
        CONST_TBL_ENTRY(SOS_FTD_STRING_UNICODE,    0x01, "")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_UID, 32, BE, "000.001", "")
        CONST_TBL_ENTRY(KPSFIELDTESTUID,      0x102824C5, "")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_EXECUTE_TYPE, 8, BE, "000.001", "")
        CONST_TBL_ENTRY(SOS_FTD_EXECUTE_WITH_INPUT,             0x01, "")
        CONST_TBL_ENTRY(SOS_FTD_EXECUTE_WITHOUT_INPUT,          0x02, "")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_CONSTANTS_TABLE_BYTE, 8, BE, "001.001", "")
        CONST_TBL_ENTRY(SOS_FTD_INPUT_PROPERTY,                  0x01, "")
        CONST_TBL_ENTRY(SOS_FTD_OUTPUT_PROPERTY,                 0x02, "")
        CONST_TBL_ENTRY(SOS_FTD_BYTE_VALUE_NOT_DEFINED,          0x00, "")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_CONSTANTS_TABLE_STRING, 8, BE, "001.001", "")
        CONST_TBL_ENTRY(SOS_FTD_STRING_VALUE_NOT_DEFINED,        0x00, "")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_CONSTANTS_TABLE_BYTEARRAY, 8, BE, "001.001", "")
        CONST_TBL_ENTRY(SOS_FTD_BYTEARRAY_VALUE_NOT_DEFINED,     0x00, "")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_CONSTANTS_TABLE_WORD, 16, BE, "001.001", "")
        CONST_TBL_ENTRY(SOS_FTD_WORD_VALUE_NOT_DEFINED,     0x0000, "")
    CONST_TBL_END

    CONST_TBL_BEGIN(SOS_FTD_CONSTANTS_TABLE_DWORD, 32, BE, "001.001", "")
        CONST_TBL_ENTRY(SOS_FTD_DWORD_VALUE_NOT_DEFINED,     0x00000000, "")
    CONST_TBL_END

    /* --------------------------------------------------------------------- */
    /* Sequences */
    /* --------------------------------------------------------------------- */

    SEQ_BEGIN(SOS_FTD_DATA_ARRAY, DONT_CREATE_STRUCT, "000.001", "")
        VALUE(8, BE, array, "Array", DEC)
    SEQ_END

    SEQ_BEGIN(SOS_FTD_DATA_STRING_ASCII, DONT_CREATE_STRUCT, "000.001", "")
        STRING_REF(STRING_ASCII_ZERO_TERMINATED, BE, string, "Data", STR_LEN, ISIHDR_RESERVE_SPACE)
    SEQ_END

    SEQ_BEGIN(SOS_FTD_DATA_STRING_UNICODE, DONT_CREATE_STRUCT, "000.001", "")
        STRING_REF(STRING_UNICODE_ZERO_TERMINATED, BE, string, "Data", STR_LEN, ISIHDR_RESERVE_SPACE)
    SEQ_END

    SEQ_BEGIN(SOS_FTD_STRING_SEQ, DONT_CREATE_STRUCT, "000.001","")
        VALUE(8,BE, string, "String", DEC)
    SEQ_END

    SEQ_BEGIN(SOS_FTD_WORD, DONT_CREATE_STRUCT, "", "")
        VALUE(16, BE, word, "Word", DEC)
    SEQ_END

    /* --------------------------------------------------------------------- */
    /* Sub blocks */
    /* --------------------------------------------------------------------- */
#define FTD_SB_16 \
    REFERENCE(16, BE, subblockID, "Sub Block ID", SB_ID) \
    REFERENCE(8, BE, subblock_length, "Sub Block Length", SB_LENGTH) 


    /* SOS_FTD_DATA_BYTE */
/*
    COMMENT(COMMENT_TEXT, 
        "Should just be used as a template for FTD data item description, see reference ")
          COMMENT_LINK(LINK_NOTE, "/5/", "", "")
          PROPERTY(PROPERTY_ISIHDR_DONT_GENERATE)
    SEQ_BEGIN(SOS_FTD_DATA_BYTE, CREATE_STRUCT, "000.001", "")
        FTD_SB_16
        VALUE(8, BE, data, "Data", DEC)
    SEQ_END
*/

    /* SOS_FTD_INPUT_PROPERTY */

    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tFTD_SeqOfSOSFTDInputPropertyKey")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME, "")
    SEQ_BEGIN(SOS_FTD_INPUT_PROPERTY_KEY, CREATE_STRUCT, "001.001","")
        COMMENT(COMMENT_TEXT, "4th byte(MSB). Value = SOS_FTD_INPUT_PROPERTY = 0x01")
        VALUE(8, BE, property_key_1, "property key byte 1", DEC)
        COMMENT(COMMENT_TEXT, "3rd byte. Symbian data source id")
        VALUE(8, BE, property_key_2, "property key byte 2", DEC)
        COMMENT(COMMENT_TEXT, "2nd byte. PN_RES_TYPE_SOS_FIELD_TEST")
        VALUE(8, BE, property_key_3, "property key byte 3", DEC)
        COMMENT(COMMENT_TEXT, "1st byte(LSB). PN_PREFIX")
        VALUE(8, BE, property_key_4, "property key byte 4", DEC)
    SEQ_END

    /* SOS_FTD_OUTPUT_PROPERTY */

    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tFTD_SeqOfSOSFTDOutputPropertyKey")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME, "")
    SEQ_BEGIN(SOS_FTD_OUTPUT_PROPERTY_KEY, CREATE_STRUCT, "001.001","")
        COMMENT(COMMENT_TEXT, "4th byte(MSB). Value = SOS_FTD_OUTPUT_PROPERTY = 0x02")
        VALUE(8, BE, property_key_1, "property key byte 1", DEC)
        COMMENT(COMMENT_TEXT, "3rd byte. Symbian data source id")
        VALUE(8, BE, property_key_2, "property key byte 2", DEC)
        COMMENT(COMMENT_TEXT, "2nd byte. PN_RES_TYPE_SOS_FIELD_TEST")
        VALUE(8, BE, property_key_3, "property key byte 3", DEC)
        COMMENT(COMMENT_TEXT, "1st byte(LSB). PN_PREFIX")
        VALUE(8, BE, property_key_4, "property key byte 4", DEC)
    SEQ_END

    /* SOS_FTD_DATA_BYTE */

    SB_BEGIN(SOS_FTD_DATA_BYTE, "000.001", "")
        SOS_SB_HDR
        COMMENT(COMMENT_TEXT, "SOS FTD data.")
        COMMENT(COMMENT_TEXT, "SOS_FTD_BYTE_VALUE_NOT_DEFINED if SOS_FTD_EXECUTE_WITHOUT_INPUT.")
        VALUE(8, BE, data, "Data", DEC)
        FILLER_EX(filler1, 8, 1)
        FILLER_EX(filler2, 8, 1)
        FILLER_EX(filler3, 8, 1)
    SB_END

    /* SOS_FTD_DATA_WORD */

    SB_BEGIN(SOS_FTD_DATA_WORD, "000.001", "")
        SOS_SB_HDR
        COMMENT(COMMENT_TEXT, "SOS FTD data.")
        COMMENT(COMMENT_TEXT, "SOS_FTD_WORD_VALUE_NOT_DEFINED if SOS_FTD_EXECUTE_WITHOUT_INPUT.")
        VALUE(16, BE, data, "Data", DEC)
        FILLER_EX(filler1, 8, 1)
        FILLER_EX(filler2, 8, 1)
    SB_END


    /* SOS_FTD_DATA_ALL */

    SB_BEGIN(SOS_FTD_DATA_ALL, "000.001", "")
        SOS_SB_HDR
        COMMENT(COMMENT_TEXT, "SOS FTD data.")
    SB_END


    /* SOS_FTD_DATA_DWORD */

    SB_BEGIN(SOS_FTD_DATA_DWORD, "000.001", "")
        SOS_SB_HDR
        COMMENT(COMMENT_TEXT, "SOS FTD data.")
        COMMENT(COMMENT_TEXT, "SOS_FTD_DWORD_VALUE_NOT_DEFINED if SOS_FTD_EXECUTE_WITHOUT_INPUT.")
        VALUE(32, BE, data, "Data", DEC)
    SB_END


    /* SOS_FTD_DATA_STRING */

    SB_BEGIN(SOS_FTD_DATA_STRING, "000.001", "")
        SOS_SB_HDR
        REFERENCE_CONST(str_type, "String type", STR_TYPE, SOS_FTD_STRING_TYPE)
        FILLER_EX(filler1, 8, 1)
        COMMENT(COMMENT_TEXT, "SOS FTD string length in bytes.")
        ITEM_LIMITS(0, SOS_FTD_MAX_STR_LEN, LIMITS_NO_VALUE, LIMITS_NO_VALUE, LIMITS_NO_VALUE)
        REFERENCE(16, BE, string_length, "String length", N)
        COMMENT(COMMENT_TEXT, "SOS_FTD_STRING_VALUE_NOT_DEFINED if SOS_FTD_EXECUTE_WITHOUT_INPUT.")
        SEQ_OF_REF(string, "SOS FTD string",
            SOS_FTD_STRING_SEQ, N, ISIHDR_DONT_GENERATE)
        FILLER(FILLER_VARIABLE, 32)
    SB_END



    /* SOS_FTD_DATA_BYTEARRAY */

    SB_BEGIN(SOS_FTD_DATA_BYTEARRAY, "000.001", "")
        SOS_SB_HDR
        COMMENT(COMMENT_TEXT, "SOS FTD data bytearray")
        REFERENCE(8, BE, arraylen, "Array length", N)
        COMMENT(COMMENT_TEXT, "SOS_FTD_BYTEARRAY_VALUE_NOT_DEFINED if SOS_FTD_EXECUTE_WITHOUT_INPUT.")
        SEQ_OF_REF(array, "Byte array", SOS_FTD_DATA_ARRAY, N, ISIHDR_DONT_GENERATE)
        FILLER(FILLER_VARIABLE, 32)
    SB_END


    /* SOS_FTD_DATA_STATUS */

    SB_BEGIN(SOS_FTD_DATA_STATUS, "000.001", "")
        SOS_SB_STATUS_HDR
        COMMENT(COMMENT_TEXT, "SOS FTD dataitem ID")
        VALUE(16, BE, dataitem_id, "Dataitem ID", DEC)
        VALUE_CONST(status, "Data status", SOS_FTD_DATA_STATUSES)
        FILLER_EX(filler1, 8, 1)
    SB_END


    /* -------------------------------------------------------------------- */
    /* ISI Messages */
    /* -------------------------------------------------------------------- */

    /* SOS_FTD_VERSION_GET */

    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdVersionGetReq")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_VERSION_GET_REQ_HDR_LEN")
    MSG_BEGIN(SOS_FTD_VERSION_GET_REQ, MESSAGE_REQ, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a general message for querying the version of FTD")
        COMMON_MSG_HDR
    MSG_END

    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdVersionGetResp")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_VERSION_GET_RESP_HDR_LEN")
    MSG_BEGIN(SOS_FTD_VERSION_GET_RESP, MESSAGE_REQ, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a response message for querying the version of FTD")
        COMMON_MSG_HDR
        VALUE(8, BE, isi_version_zzz, "ISI Version Z", DEC)
        VALUE(8, BE, isi_version_yyy, "ISI Version Y", DEC)
    MSG_END

    MSG_PAIR(SOS_FTD_VERSION_GET_REQ, SOS_FTD_VERSION_GET_RESP)


    /* SOS_FTD_ACTIVATE_REQ */

    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_INPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdActivateReq")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_ACTIVATE_REQ_HDR_LEN")
    MSG_BEGIN(SOS_FTD_ACTIVATE_REQ, MESSAGE_REQ, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a general message for activate the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
        PROPERTY_VALUE(PROPERTY_ITEM_MAXIMUM_VALUE, MAX_FTD_DATA_ITEMS)
        REFERENCE(8, BE, data_item_count, "Number of FTD Data", N)
        FILLER_EX(pad1, 8, 1)
        FILLER_EX(pad2, 8, 1)
        FILLER_EX(pad3, 8, 1)
        COMMENT(COMMENT_TEXT, "Only required Sub Block IDs are added to request message.")
        COMMENT(COMMENT_TEXT, " Response will contain SOS_FTD_DATA_*.")
        SEQ_OF_REF(ids, "List of FTD data items", SOS_FTD_WORD, N, ISIHDR_DONT_GENERATE)
    MSG_END


    /* SOS_FTD_ACTIVATE_RESP */

    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_OUTPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdActivateResp")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_ACTIVATE_RESP_HDR_LEN")
    MSG_BEGIN(SOS_FTD_ACTIVATE_RESP, MESSAGE_RESP, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a response message for activate the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
        PROPERTY_VALUE(PROPERTY_ITEM_MAXIMUM_VALUE, MAX_FTD_DATA_ITEMS)
        REFERENCE(8, BE, data_item_count, "Number of FTD Data", N)
        FILLER_EX(pad1, 8, 1)
        FILLER_EX(pad2, 8, 1)
        FILLER_EX(pad3, 8, 1)
        SB_LST_REF_BEGIN(LIST_OF_SUBBLOCKS, sub_block_ids, "FTD data items", N,
        ISIHDR_MARKER_ANYSIZE)
            SB_LST_ENTRY(SOS_FTD_DATA_BYTE)
            SB_LST_ENTRY(SOS_FTD_DATA_WORD)
            SB_LST_ENTRY(SOS_FTD_DATA_ALL)
            SB_LST_ENTRY(SOS_FTD_DATA_DWORD)
            SB_LST_ENTRY(SOS_FTD_DATA_STRING)
            SB_LST_ENTRY(SOS_FTD_DATA_BYTEARRAY)
            SB_LST_ENTRY(SOS_FTD_DATA_STATUS)
        SB_LST_END
    MSG_END

    MSG_PAIR(SOS_FTD_ACTIVATE_REQ, SOS_FTD_ACTIVATE_RESP)


    /* SOS_FTD_DEACTIVATE_REQ */

    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_INPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdDeactivateReq")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_DEACTIVATE_REQ_HDR_LEN")
    MSG_BEGIN(SOS_FTD_DEACTIVATE_REQ, MESSAGE_REQ, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a general message for deactivate the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
        PROPERTY_VALUE(PROPERTY_ITEM_MAXIMUM_VALUE, MAX_FTD_DATA_ITEMS)
        REFERENCE(8, BE, data_item_count, "Number of FTD Data", N)
        FILLER_EX(pad1, 8, 1)
        FILLER_EX(pad2, 8, 1)
        FILLER_EX(pad3, 8, 1)
        COMMENT(COMMENT_TEXT, "Use data item IDs as parameter or deactivate all with value SOS_FTD_DATA_ALL.")
        SEQ_OF_REF(ids, "List of FTD data items", SOS_FTD_WORD, N, ISIHDR_DONT_GENERATE)
    MSG_END


    /* SOS_FTD_DEACTIVATE_RESP */

    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_OUTPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdDeactivateResp")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_DEACTIVATE_RESP_HDR_LEN")
    MSG_BEGIN(SOS_FTD_DEACTIVATE_RESP, MESSAGE_RESP, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a response message for deactivate the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
        PROPERTY_VALUE(PROPERTY_ITEM_MAXIMUM_VALUE, MAX_FTD_DATA_ITEMS)
        REFERENCE(8, BE, data_item_count, "Number of FTD Data", N)
        FILLER_EX(pad1, 8, 1)
        FILLER_EX(pad2, 8, 1)
        FILLER_EX(pad3, 8, 1)
        SB_LST_REF_BEGIN(LIST_OF_SUBBLOCKS, sub_block_ids, "N * Status data items", N,
        ISIHDR_MARKER_ANYSIZE)
            SB_LST_ENTRY(SOS_FTD_DATA_STATUS)
        SB_LST_END
    MSG_END

    MSG_PAIR(SOS_FTD_DEACTIVATE_REQ, SOS_FTD_DEACTIVATE_RESP)


    /* SOS_FTD_EXECUTE_REQ */

/*
    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_INPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdExecuteReq")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_EXECUTE_REQ_HDR_LEN")
    MSG_BEGIN(SOS_FTD_EXECUTE_REQ, MESSAGE_REQ, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a general message for execute the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
        PROPERTY_VALUE(PROPERTY_ITEM_MAXIMUM_VALUE, MAX_FTD_DATA_ITEMS)
        REFERENCE(8, BE, data_item_count, "Number of FTD Data", N)
        VALUE_CONST(execute_type, "Execute type", SOS_FTD_EXECUTE_TYPE)
        FILLER_EX(pad1, 8, 1)
        FILLER_EX(pad2, 8, 1)
        SB_LST_REF_BEGIN(LIST_OF_SUBBLOCKS, sub_block_ids, "List of execute data items", N,
        ISIHDR_MARKER_ANYSIZE)
            SB_LST_ENTRY(SOS_FTD_DATA_BYTE)
            SB_LST_ENTRY(SOS_FTD_DATA_WORD)
            SB_LST_ENTRY(SOS_FTD_DATA_ALL)
            SB_LST_ENTRY(SOS_FTD_DATA_DWORD)
            SB_LST_ENTRY(SOS_FTD_DATA_STRING)
            SB_LST_ENTRY(SOS_FTD_DATA_BYTEARRAY)
            SB_LST_ENTRY(SOS_FTD_DATA_STATUS)
        SB_LST_END
    MSG_END
*/
    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_INPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdExecuteReq")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_EXECUTE_REQ_HDR_LEN")
    MSG_BEGIN(SOS_FTD_EXECUTE_REQ, MESSAGE_REQ, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a general message for execute the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
/*
        PROPERTY_VALUE(PROPERTY_ITEM_MAXIMUM_VALUE, MAX_FTD_DATA_ITEMS)
        REFERENCE(8, BE, data_item_count, "Number of FTD Data", N)
*/
        FILLER_EX(pad1, 8, 1)
        VALUE_CONST(execute_type, "Execute type", SOS_FTD_EXECUTE_TYPE)
        VALUE(16, BE, data_item_id, "Data item ID", DEC)
        COMMENT(COMMENT_TEXT,
            "Only requred Input data if SOS_FTD_EXECUTE_TYPE = SOS_FTD_EXECUTE_TYPE_WITH_INPUT.")
        VALUE(32, BE, input_data, "Input data", DEC)
    MSG_END

    /* SOS_FTD_EXECUTE_RESP */
/*
    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_OUTPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdExecuteResp")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_EXECUTE_RESP_HDR_LEN")
    MSG_BEGIN(SOS_FTD_EXECUTE_RESP, MESSAGE_RESP, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a response message for execute the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
        PROPERTY_VALUE(PROPERTY_ITEM_MAXIMUM_VALUE, MAX_FTD_DATA_ITEMS)
        REFERENCE(8, BE, data_item_count, "Number of FTD Data", N)
        FILLER_EX(pad1, 8, 1)
        FILLER_EX(pad2, 8, 1)
        FILLER_EX(pad3, 8, 1)
        COMMENT(COMMENT_TEXT, "Only required Sub Block IDs are added to reponse message.")
        COMMENT(COMMENT_TEXT, " Response contains SOS_FTD_DATA_*.")
        SEQ_OF_REF(ids, "List of FTD data items", SOS_FTD_WORD, N, ISIHDR_DONT_GENERATE)
    MSG_END
*/
    COMMENT(COMMENT_TEXT, "Corresponding property key: SOS_FTD_OUTPUT_PROPERTY. ")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_NAME, "tSosFtdExecuteResp")
    PROPERTY_STRING(PROPERTY_ISIHDR_STRUCT_LENGTH_NAME,
        "SOS_FTD_EXECUTE_RESP_HDR_LEN")
    MSG_BEGIN(SOS_FTD_EXECUTE_RESP, MESSAGE_RESP, "000.001", "")
        COMMENT(COMMENT_TEXT,
            "This is a response message for execute the FTD")
        COMMON_MSG_HDR
        VALUE(16, BE, message_length, "Message Length", DEC)
        VALUE(16, BE, status_item_id, "Status item ID", DEC)
        VALUE(16, BE, status_item_len, "Status item length", DEC)
        VALUE(16, BE, data_item_id, "Dataitem ID", DEC)
        COMMENT(COMMENT_TEXT,
            "Constant from table SOS_FTD_DATA_STATUSES.")
        VALUE(8, BE, data_status, "Data status", DEC)
        FILLER_EX(pad1, 8, 1)

    MSG_END

    MSG_PAIR(SOS_FTD_EXECUTE_REQ, SOS_FTD_EXECUTE_RESP)

RES_END

/* End of file sos_ftd_isi_m.h */
