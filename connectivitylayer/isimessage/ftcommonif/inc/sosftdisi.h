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


#ifndef _SYMBIAN_ISI_SOSFTDISI_H_
#define _SYMBIAN_ISI_SOSFTDISI_H_
/**/
/* Extended resources of type: 'PN_RES_TYPE_SOS_FIELD_TEST' */


#if defined(NCP_COMMON_CELLMO_BRANCH_SUPPORT )\
 && (NCP_COMMON_CELLMO_BRANCH_SUPPORT!=NCP_COMMON_CELLMO_BRANCH_0641\
 &&  NCP_COMMON_CELLMO_BRANCH_SUPPORT!=NCP_COMMON_CELLMO_BRANCH_0711\
 &&  NCP_COMMON_CELLMO_BRANCH_SUPPORT!=NCP_COMMON_CELLMO_BRANCH_0741)
#define PN_SOS_WLAN_EXTENDED     \
            PN_RES(PN_RES_TYPE_SOS_FIELD_TEST,1)    /* 0x0103E0  [S] Symbian Wlan Server */
#define PN_SOS_DVBH_EXTENDED     \
            PN_RES(PN_RES_TYPE_SOS_FIELD_TEST,2)    /* 0x0203E0  [S] Symbian DVB-H Server */
#define PN_SOS_FTD_CALLDROP_EXTENDED     \
            PN_RES(PN_RES_TYPE_SOS_FIELD_TEST,3)    /* 0x0303E0  [S] FieldTest CALLDROP Server */            
#endif


/**/
#define SOS_FTD_ISI_VERSION_Z	 1
#define SOS_FTD_ISI_VERSION_Y	 1


//CONSTANTS
#define SOS_FTD_MAX_STR_LEN                               	0xFFF4                                            	
#define MAX_FTD_DATA_ITEMS                                	0xFF                                              	
#define SOS_FTD_ANY_SIZE                                  	0x01                                                 	


//TABLES

//ConstantTable for SOS_FTD_DATA_STATUSES
#define SOS_FTD_DATA_OK                                   	0x00                                                 	
#define SOS_FTD_DATA_FAIL                                 	0x01                                                 	
#define SOS_FTD_DATA_NOT_ACTIVATED                        	0x02                                                 	
#define SOS_FTD_DATA_NOT_SUPPORTED                        	0x03                                                 	
#define SOS_FTD_DATA_NOT_AVAILABLE                        	0x04                                                 	

//ConstantTable for SOS_FTD_STRING_TYPE
#define SOS_FTD_STRING_ASCII                              	0x00                                                 	
#define SOS_FTD_STRING_UNICODE                            	0x01                                                 	

//ConstantTable for SOS_FTD_UID
#define KPSFIELDTESTUID                                   	0x102824C5                                        	

//ConstantTable for SOS_FTD_EXECUTE_TYPE
#define SOS_FTD_EXECUTE_WITH_INPUT                        	0x01                                                 	
#define SOS_FTD_EXECUTE_WITHOUT_INPUT                     	0x02                                                 	

//ConstantTable for SOS_FTD_CONSTANTS_TABLE_BYTE
#define SOS_FTD_INPUT_PROPERTY                            	0x01                                                 	
#define SOS_FTD_OUTPUT_PROPERTY                           	0x02                                                 	
#define SOS_FTD_BYTE_VALUE_NOT_DEFINED                    	0x00                                                 	

//ConstantTable for SOS_FTD_CONSTANTS_TABLE_STRING
#define SOS_FTD_STRING_VALUE_NOT_DEFINED                  	0x00                                                 	

//ConstantTable for SOS_FTD_CONSTANTS_TABLE_BYTEARRAY
#define SOS_FTD_BYTEARRAY_VALUE_NOT_DEFINED               	0x00                                                 	

//ConstantTable for SOS_FTD_CONSTANTS_TABLE_WORD
#define SOS_FTD_WORD_VALUE_NOT_DEFINED                    	0x00                                                 	

//ConstantTable for SOS_FTD_CONSTANTS_TABLE_DWORD
#define SOS_FTD_DWORD_VALUE_NOT_DEFINED                   	0x00                                                 	


//STRUCTURES


//Definition for SOS_FTD_INPUT_PROPERTY_KEY
#define SOS_FTD_INPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY1    	0 //size 1 byte(s)
#define SOS_FTD_INPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY2    	1 //size 1 byte(s)
#define SOS_FTD_INPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY3    	2 //size 1 byte(s)
#define SOS_FTD_INPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY4    	3 //size 1 byte(s)
#define SIZE_SOS_FTD_INPUT_PROPERTY_KEY                   	4


//Definition for SOS_FTD_OUTPUT_PROPERTY_KEY
#define SOS_FTD_OUTPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY1   	0 //size 1 byte(s)
#define SOS_FTD_OUTPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY2   	1 //size 1 byte(s)
#define SOS_FTD_OUTPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY3   	2 //size 1 byte(s)
#define SOS_FTD_OUTPUT_PROPERTY_KEY_OFFSET_PROPERTYKEY4   	3 //size 1 byte(s)
#define SIZE_SOS_FTD_OUTPUT_PROPERTY_KEY                  	4


//SUBBLOCKS


//Definition for SOS_FTD_DATA_BYTE
#define SOS_FTD_DATA_BYTE                                 	0x01                                                 	
#define SOS_FTD_DATA_BYTE_OFFSET_ID                       	0 //size 2 byte(s)
#define SOS_FTD_DATA_BYTE_OFFSET_LENGTH                   	2 //size 2 byte(s)
#define SOS_FTD_DATA_BYTE_OFFSET_DATA                     	4 //size 1 byte(s)
#define SOS_FTD_DATA_BYTE_OFFSET_FILLERBYTE1              	5 //size 1 byte(s)
#define SOS_FTD_DATA_BYTE_OFFSET_FILLERBYTE2              	6 //size 1 byte(s)
#define SOS_FTD_DATA_BYTE_OFFSET_FILLERBYTE3              	7 //size 1 byte(s)
#define SIZE_SOS_FTD_DATA_BYTE                            	8


//Definition for SOS_FTD_DATA_WORD
#define SOS_FTD_DATA_WORD                                 	0x02                                                 	
#define SOS_FTD_DATA_WORD_OFFSET_ID                       	0 //size 2 byte(s)
#define SOS_FTD_DATA_WORD_OFFSET_LENGTH                   	2 //size 2 byte(s)
#define SOS_FTD_DATA_WORD_OFFSET_DATA                     	4 //size 2 byte(s)
#define SOS_FTD_DATA_WORD_OFFSET_FILLERBYTE1              	6 //size 1 byte(s)
#define SOS_FTD_DATA_WORD_OFFSET_FILLERBYTE2              	7 //size 1 byte(s)
#define SIZE_SOS_FTD_DATA_WORD                            	8


//Definition for SOS_FTD_DATA_ALL
#define SOS_FTD_DATA_ALL                                  	0x03                                                 	
#define SOS_FTD_DATA_ALL_OFFSET_ID                        	0 //size 2 byte(s)
#define SOS_FTD_DATA_ALL_OFFSET_LENGTH                    	2 //size 2 byte(s)
#define SIZE_SOS_FTD_DATA_ALL                             	4


//Definition for SOS_FTD_DATA_DWORD
#define SOS_FTD_DATA_DWORD                                	0x04                                                 	
#define SOS_FTD_DATA_DWORD_OFFSET_ID                      	0 //size 2 byte(s)
#define SOS_FTD_DATA_DWORD_OFFSET_LENGTH                  	2 //size 2 byte(s)
#define SOS_FTD_DATA_DWORD_OFFSET_DATA                    	4 //size 4 byte(s)
#define SIZE_SOS_FTD_DATA_DWORD                           	8


//Definition for SOS_FTD_DATA_STRING
#define SOS_FTD_DATA_STRING                               	0x05                                                 	
#define SOS_FTD_DATA_STRING_OFFSET_ID                     	0 //size 2 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_LENGTH                 	2 //size 2 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_STRTYPE                	4 //size 1 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_FILLERBYTE1            	5 //size 1 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_STRINGLENGTH           	6 //size 2 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_STRING                 	8 //size 1 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_FILLERBYTE2            	9 //size 1 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_FILLERBYTE3            	10 //size 1 byte(s)
#define SOS_FTD_DATA_STRING_OFFSET_FILLERBYTE4            	11 //size 1 byte(s)
#define SIZE_SOS_FTD_DATA_STRING                          	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SOS_FTD_DATA_BYTEARRAY
#define SOS_FTD_DATA_BYTEARRAY                            	0x06                                                 	
#define SOS_FTD_DATA_BYTEARRAY_OFFSET_ID                  	0 //size 2 byte(s)
#define SOS_FTD_DATA_BYTEARRAY_OFFSET_LENGTH              	2 //size 2 byte(s)
#define SOS_FTD_DATA_BYTEARRAY_OFFSET_ARRAYLEN            	4 //size 1 byte(s)
#define SOS_FTD_DATA_BYTEARRAY_OFFSET_ARRAY               	5 //size 1 byte(s)
#define SOS_FTD_DATA_BYTEARRAY_OFFSET_FILLERBYTE1         	6 //size 1 byte(s)
#define SOS_FTD_DATA_BYTEARRAY_OFFSET_FILLERBYTE2         	7 //size 1 byte(s)
#define SIZE_SOS_FTD_DATA_BYTEARRAY                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SOS_FTD_DATA_STATUS
#define SOS_FTD_DATA_STATUS                               	0x00                                                 	
#define SOS_FTD_DATA_STATUS_OFFSET_ID                     	0 //size 2 byte(s)
#define SOS_FTD_DATA_STATUS_OFFSET_LENGTH                 	2 //size 2 byte(s)
#define SOS_FTD_DATA_STATUS_OFFSET_DATAITEMID             	4 //size 2 byte(s)
#define SOS_FTD_DATA_STATUS_OFFSET_STATUS                 	6 //size 1 byte(s)
#define SOS_FTD_DATA_STATUS_OFFSET_FILLERBYTE1            	7 //size 1 byte(s)
#define SIZE_SOS_FTD_DATA_STATUS                          	8


//MESSAGES


//Definition for SOS_FTD_VERSION_GET_REQ
#define SOS_FTD_VERSION_GET_REQ                           	0x06                                                 	
#define SOS_FTD_VERSION_GET_REQ_OFFSET_TRANSID            	0 //size 1 byte(s)
#define SOS_FTD_VERSION_GET_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define SIZE_SOS_FTD_VERSION_GET_REQ                      	2


//Definition for SOS_FTD_VERSION_GET_RESP
#define SOS_FTD_VERSION_GET_RESP                          	0x07                                                 	
#define SOS_FTD_VERSION_GET_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define SOS_FTD_VERSION_GET_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define SOS_FTD_VERSION_GET_RESP_OFFSET_ISIVERSIONZZZ     	2 //size 1 byte(s)
#define SOS_FTD_VERSION_GET_RESP_OFFSET_ISIVERSIONYYY     	3 //size 1 byte(s)
#define SIZE_SOS_FTD_VERSION_GET_RESP                     	4


//Definition for SOS_FTD_ACTIVATE_REQ
#define SOS_FTD_ACTIVATE_REQ                              	0x00                                                 	
#define SOS_FTD_ACTIVATE_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_REQ_OFFSET_MESSAGELENGTH         	2 //size 2 byte(s)
#define SOS_FTD_ACTIVATE_REQ_OFFSET_DATAITEMCOUNT         	4 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_REQ_OFFSET_FILLERBYTE1           	5 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_REQ_OFFSET_FILLERBYTE2           	6 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_REQ_OFFSET_FILLERBYTE3           	7 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_REQ_OFFSET_IDS                   	8 //size 2 byte(s)
#define SIZE_SOS_FTD_ACTIVATE_REQ                         	10
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SOS_FTD_ACTIVATE_RESP
#define SOS_FTD_ACTIVATE_RESP                             	0x01                                                 	
#define SOS_FTD_ACTIVATE_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_RESP_OFFSET_MESSAGELENGTH        	2 //size 2 byte(s)
#define SOS_FTD_ACTIVATE_RESP_OFFSET_DATAITEMCOUNT        	4 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_RESP_OFFSET_FILLERBYTE1          	5 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_RESP_OFFSET_FILLERBYTE2          	6 //size 1 byte(s)
#define SOS_FTD_ACTIVATE_RESP_OFFSET_FILLERBYTE3          	7 //size 1 byte(s)
#define SIZE_SOS_FTD_ACTIVATE_RESP                        	8


//Definition for SOS_FTD_DEACTIVATE_REQ
#define SOS_FTD_DEACTIVATE_REQ                            	0x02                                                 	
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_TRANSID             	0 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_MESSAGELENGTH       	2 //size 2 byte(s)
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_DATAITEMCOUNT       	4 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_FILLERBYTE1         	5 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_FILLERBYTE2         	6 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_FILLERBYTE3         	7 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_REQ_OFFSET_IDS                 	8 //size 2 byte(s)
#define SIZE_SOS_FTD_DEACTIVATE_REQ                       	10
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for SOS_FTD_DEACTIVATE_RESP
#define SOS_FTD_DEACTIVATE_RESP                           	0x03                                                 	
#define SOS_FTD_DEACTIVATE_RESP_OFFSET_TRANSID            	0 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_RESP_OFFSET_MESSAGELENGTH      	2 //size 2 byte(s)
#define SOS_FTD_DEACTIVATE_RESP_OFFSET_DATAITEMCOUNT      	4 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_RESP_OFFSET_FILLERBYTE1        	5 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_RESP_OFFSET_FILLERBYTE2        	6 //size 1 byte(s)
#define SOS_FTD_DEACTIVATE_RESP_OFFSET_FILLERBYTE3        	7 //size 1 byte(s)
#define SIZE_SOS_FTD_DEACTIVATE_RESP                      	8


//Definition for SOS_FTD_EXECUTE_REQ
#define SOS_FTD_EXECUTE_REQ                               	0x04                                                 	
#define SOS_FTD_EXECUTE_REQ_OFFSET_TRANSID                	0 //size 1 byte(s)
#define SOS_FTD_EXECUTE_REQ_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define SOS_FTD_EXECUTE_REQ_OFFSET_MESSAGELENGTH          	2 //size 2 byte(s)
#define SOS_FTD_EXECUTE_REQ_OFFSET_FILLERBYTE1            	4 //size 1 byte(s)
#define SOS_FTD_EXECUTE_REQ_OFFSET_EXECUTETYPE            	5 //size 1 byte(s)
#define SOS_FTD_EXECUTE_REQ_OFFSET_DATAITEMID             	6 //size 2 byte(s)
#define SOS_FTD_EXECUTE_REQ_OFFSET_INPUTDATA              	8 //size 4 byte(s)
#define SIZE_SOS_FTD_EXECUTE_REQ                          	12


//Definition for SOS_FTD_EXECUTE_RESP
#define SOS_FTD_EXECUTE_RESP                              	0x05                                                 	
#define SOS_FTD_EXECUTE_RESP_OFFSET_TRANSID               	0 //size 1 byte(s)
#define SOS_FTD_EXECUTE_RESP_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define SOS_FTD_EXECUTE_RESP_OFFSET_MESSAGELENGTH         	2 //size 2 byte(s)
#define SOS_FTD_EXECUTE_RESP_OFFSET_STATUSITEMID          	4 //size 2 byte(s)
#define SOS_FTD_EXECUTE_RESP_OFFSET_STATUSITEMLEN         	6 //size 2 byte(s)
#define SOS_FTD_EXECUTE_RESP_OFFSET_DATAITEMID            	8 //size 2 byte(s)
#define SOS_FTD_EXECUTE_RESP_OFFSET_DATASTATUS            	10 //size 1 byte(s)
#define SOS_FTD_EXECUTE_RESP_OFFSET_FILLERBYTE1           	11 //size 1 byte(s)
#define SIZE_SOS_FTD_EXECUTE_RESP                         	12

#endif