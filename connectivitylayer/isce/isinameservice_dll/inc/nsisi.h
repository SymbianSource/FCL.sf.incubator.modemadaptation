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

#ifndef _SYMBIAN_ISI_NSISI_H_
#define _SYMBIAN_ISI_NSISI_H_

#define NAME_SERVICE_ISI_VERSION_Z	 1
#define NAME_SERVICE_ISI_VERSION_Y	 2

#define PN_NAMESERVICE                                      219 /* 0xDB  [PN] Name Service Messages */
#define PN_MEDIA_ROUTING_REQ                                0x00
//TABLES

//BitmaskTables for PN_NAME_RECORD_ATTR
#define PN_NAME_UNDEF                                     	0x01                                                 	
#define PN_NAME_NOCHG                                     	0x02                                                 	

//ConstantTable for NAME_SERVICE_CC
#define PN_NAME_OK                                        	0x00                                                 	
#define PN_NAME_NOT_ALLOWED                               	0x01                                                 	
#define PN_NAME_UNKNOWN                                   	0x03                                                 	


//STRUCTURES


//Definition for PN_NAME_SRV_ITEM_STR
#define PN_NAME_SRV_ITEM_STR_OFFSET_NAME                  	0 //size 4 byte(s)
#define PN_NAME_SRV_ITEM_STR_OFFSET_DEV                   	4 //size 1 byte(s)
#define PN_NAME_SRV_ITEM_STR_OFFSET_OBJ                   	5 //size 1 byte(s)
#define PN_NAME_SRV_ITEM_STR_OFFSET_FLAGS                 	6 //size 1 byte(s)
#define PN_NAME_SRV_ITEM_STR_OFFSET_RESERVED              	7 //size 1 byte(s)
#define SIZE_PN_NAME_SRV_ITEM_STR                         	8


//MESSAGES


//Definition for PNS_NAME_QUERY_REQ
#define PNS_NAME_QUERY_REQ                                	0x01                                                 	
#define PNS_NAME_QUERY_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define PNS_NAME_QUERY_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define PNS_NAME_QUERY_REQ_OFFSET_RESERVED1               	2 //size 1 byte(s)
#define PNS_NAME_QUERY_REQ_OFFSET_RESERVED2               	3 //size 1 byte(s)
#define PNS_NAME_QUERY_REQ_OFFSET_NAME                    	4 //size 4 byte(s)
#define PNS_NAME_QUERY_REQ_OFFSET_BITMASK                 	8 //size 4 byte(s)
#define SIZE_PNS_NAME_QUERY_REQ                           	12


//Definition for PNS_NAME_QUERY_RESP
#define PNS_NAME_QUERY_RESP                               	0x02                                                 	
#define PNS_NAME_QUERY_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define PNS_NAME_QUERY_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define PNS_NAME_QUERY_RESP_OFFSET_TOTALMATCHES           	2 //size 2 byte(s)
#define PNS_NAME_QUERY_RESP_OFFSET_MATCHESINMSG           	4 //size 2 byte(s)
#define PNS_NAME_QUERY_RESP_OFFSET_RESERVED1              	6 //size 1 byte(s)
#define PNS_NAME_QUERY_RESP_OFFSET_RESERVED2              	7 //size 1 byte(s)
#define PNS_NAME_QUERY_RESP_OFFSET_NAMEENTRYTBL           	8 //size 8 byte(s)
#define SIZE_PNS_NAME_QUERY_RESP                          	16
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PNS_NAME_ADD_IND
#define PNS_NAME_ADD_IND                                  	0x03                                                 	
#define PNS_NAME_ADD_IND_OFFSET_UTID                      	0 //size 1 byte(s)
#define PNS_NAME_ADD_IND_OFFSET_SUBFUNCTION               	1 //size 1 byte(s)
#define PNS_NAME_ADD_IND_OFFSET_TOTALMATCHES              	2 //size 2 byte(s)
#define PNS_NAME_ADD_IND_OFFSET_MATCHESINMSG              	4 //size 2 byte(s)
#define PNS_NAME_ADD_IND_OFFSET_RESERVED1                 	6 //size 1 byte(s)
#define PNS_NAME_ADD_IND_OFFSET_RESERVED2                 	7 //size 1 byte(s)
#define PNS_NAME_ADD_IND_OFFSET_NAMEENTRYTBL              	8 //size 8 byte(s)
#define SIZE_PNS_NAME_ADD_IND                             	16
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PNS_NAME_REMOVE_IND
#define PNS_NAME_REMOVE_IND                               	0x04                                                 	
#define PNS_NAME_REMOVE_IND_OFFSET_UTID                   	0 //size 1 byte(s)
#define PNS_NAME_REMOVE_IND_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define PNS_NAME_REMOVE_IND_OFFSET_TOTALMATCHES           	2 //size 2 byte(s)
#define PNS_NAME_REMOVE_IND_OFFSET_MATCHESINMSG           	4 //size 2 byte(s)
#define PNS_NAME_REMOVE_IND_OFFSET_RESERVED1              	6 //size 1 byte(s)
#define PNS_NAME_REMOVE_IND_OFFSET_RESERVED2              	7 //size 1 byte(s)
#define PNS_NAME_REMOVE_IND_OFFSET_NAMEENTRYTBL           	8 //size 8 byte(s)
#define SIZE_PNS_NAME_REMOVE_IND                          	16
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for PNS_NAME_ADD_REQ
#define PNS_NAME_ADD_REQ                                  	0x05                                                 	
#define PNS_NAME_ADD_REQ_OFFSET_UTID                      	0 //size 1 byte(s)
#define PNS_NAME_ADD_REQ_OFFSET_SUBFUNCTION               	1 //size 1 byte(s)
#define PNS_NAME_ADD_REQ_OFFSET_RESERVED1                 	2 //size 1 byte(s)
#define PNS_NAME_ADD_REQ_OFFSET_RESERVED2                 	3 //size 1 byte(s)
#define PNS_NAME_ADD_REQ_OFFSET_NAMEENTRY                 	4 //size 8 byte(s)
#define SIZE_PNS_NAME_ADD_REQ                             	12
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.

//Definition for PNS_NAME_ADD_RESP
#define PNS_NAME_ADD_RESP                                 	0x06                                                 	
#define PNS_NAME_ADD_RESP_OFFSET_UTID                     	0 //size 1 byte(s)
#define PNS_NAME_ADD_RESP_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define PNS_NAME_ADD_RESP_OFFSET_REASON                   	2 //size 1 byte(s)
#define PNS_NAME_ADD_RESP_OFFSET_RESERVED                 	3 //size 1 byte(s)
#define SIZE_PNS_NAME_ADD_RESP                            	4


//Definition for PNS_NAME_REMOVE_REQ
#define PNS_NAME_REMOVE_REQ                               	0x07                                                 	
#define PNS_NAME_REMOVE_REQ_OFFSET_UTID                   	0 //size 1 byte(s)
#define PNS_NAME_REMOVE_REQ_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define PNS_NAME_REMOVE_REQ_OFFSET_RESERVED1              	2 //size 1 byte(s)
#define PNS_NAME_REMOVE_REQ_OFFSET_RESERVED2              	3 //size 1 byte(s)
#define PNS_NAME_REMOVE_REQ_OFFSET_NAME                   	4 //size 4 byte(s)
#define SIZE_PNS_NAME_REMOVE_REQ                          	8


//Definition for PNS_NAME_REMOVE_RESP
#define PNS_NAME_REMOVE_RESP                              	0x08                                                 	
#define PNS_NAME_REMOVE_RESP_OFFSET_UTID                  	0 //size 1 byte(s)
#define PNS_NAME_REMOVE_RESP_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define PNS_NAME_REMOVE_RESP_OFFSET_REASON                	2 //size 1 byte(s)
#define PNS_NAME_REMOVE_RESP_OFFSET_RESERVED1             	3 //size 1 byte(s)
#define SIZE_PNS_NAME_REMOVE_RESP                         	4

#endif