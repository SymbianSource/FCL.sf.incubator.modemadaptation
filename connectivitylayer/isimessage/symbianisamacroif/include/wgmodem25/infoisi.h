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
* Name      : PN_MODEM_INFO [ 0xC5 ] Resource Modem Info Server
* Version   : 000.005
*
*/


#ifndef _SYMBIAN_ISI_INFOISI_H_
#define _SYMBIAN_ISI_INFOISI_H_

#define MODEM_INFO_SERVER_ISI_VERSION_Z	 0
#define MODEM_INFO_SERVER_ISI_VERSION_Y	 5


//CONSTANTS
#define INFO_SB_SN_IMEI_PLAIN_STR_LEN                     	0x10                                              	
#define INFO_SB_SN_IMEI_SPARE_TO_NET_STR_LEN              	0x08                                                 	
#define INFO_SB_SN_IMEI_SV_TO_NET_STR_LEN                 	0x09                                                 	
#define INFO_ANY_SIZE                                     	0x01                                                 	
#define INFO_BAND_ID_TABLE_MAX_CNT                        	0x20                                              	


//TABLES

//ConstantTable for FtdTable
#define FTD_SB_MCU_VERSION                                	0x101                                             	
#define FTD_SB_PPM_VERSION                                	0x102                                             	
#define FTD_SB_VERSION_DATE                               	0x103                                             	
#define FTD_SB_DSP_VERSION                                	0x104                                             	
#define FTD_SB_MCU_CHECKSUM                               	0x105                                             	
#define FTD_SB_HW_VERSION                                 	0x106                                             	
#define FTD_SB_TEXT_VERSION                               	0x107                                             	

//ConstantTable for INFO_GENERAL_SYM
#define INFO_OK                                           	0x00                                                 	
#define INFO_FAIL                                         	0x01                                                 	
#define INFO_NO_NUMBER                                    	0x02                                                 	
#define INFO_NOT_SUPPORTED                                	0x03                                                 	

//ConstantTable for INFO_SN_TARGET_IDENTIFIER
#define INFO_SN_IMEI_PLAIN                                	0x41                                              	
#define INFO_SN_IMEI_SPARE_TO_NET                         	0x42                                              	
#define INFO_SN_IMEI_SV_TO_NET                            	0x43                                              	

//ConstantTable for INFO_BATCH_TYPE
#define INFO_PP_FEATURE                                   	0x00                                                 	
#define INFO_PP_ALL_FEATURES                              	0x01                                                 	

//ConstantTable for INFO_BAND_POS
#define INFO_GSM_POS                                      	0x00                                                 	
#define INFO_WCDMA_POS                                    	0x01                                                 	
#define INFO_WCDMA_RX_DIV_POS                             	0x02                                                 	
#define INFO_EUTRAN_POS                                   	0x03                                                 	

//BitmaskTables for INFO_VERSION_TARGETS
#define INFO_MCUSW                                        	0x01                                                 	

//BitmaskTables for INFO_RAT_ID
#define INFO_NO_RAT                                       	0x00                                                 	
#define INFO_RAT_GSM                                      	0x01                                                 	
#define INFO_RAT_WCDMA                                    	0x02                                                 	
#define INFO_RAT_WCDMA_RX_DIV                             	0x04                                                 	
#define INFO_RAT_EUTRAN                                   	0x08                                                 	

//BitmaskTables for INFO_GSM_BAND
#define INFO_NO_GSM                                       	0x00                                                 	
#define INFO_GSM850                                       	0x01                                                 	
#define INFO_GSM900                                       	0x02                                                 	
#define INFO_GSM1800                                      	0x04                                                 	
#define INFO_GSM1900                                      	0x08                                                 	

//BitmaskTables for INFO_WCDMA_BAND
#define INFO_NO_WCDMA                                     	0x00                                                 	
#define INFO_WCDMA_FDD_BAND1                              	0x01                                                 	
#define INFO_WCDMA_FDD_BAND2                              	0x02                                                 	
#define INFO_WCDMA_FDD_BAND3                              	0x04                                                 	
#define INFO_WCDMA_FDD_BAND4                              	0x08                                                 	
#define INFO_WCDMA_FDD_BAND5                              	0x10                                              	
#define INFO_WCDMA_FDD_BAND6                              	0x20                                              	
#define INFO_WCDMA_FDD_BAND7                              	0x40                                              	
#define INFO_WCDMA_FDD_BAND8                              	0x80                                              	
#define INFO_WCDMA_FDD_BAND9                              	0x100                                             	
#define INFO_WCDMA_FDD_BAND10                             	0x200                                             	
#define INFO_WCDMA_FDD_BAND11                             	0x400                                             	
#define INFO_WCDMA_FDD_BAND12                             	0x800                                             	
#define INFO_WCDMA_FDD_BAND13                             	0x1000                                            	
#define INFO_WCDMA_FDD_BAND14                             	0x2000                                            	
#define INFO_WCDMA_FDD_BAND15                             	0x4000                                            	
#define INFO_WCDMA_FDD_BAND16                             	0x8000                                            	
#define INFO_WCDMA_FDD_BAND17                             	0x10000                                           	
#define INFO_WCDMA_FDD_BAND18                             	0x20000                                           	
#define INFO_WCDMA_FDD_BAND19                             	0x40000                                           	
#define INFO_WCDMA_FDD_BAND20                             	0x80000                                           	
#define INFO_WCDMA_FDD_BAND21                             	0x100000                                          	
#define INFO_WCDMA_FDD_BAND22                             	0x200000                                          	
#define INFO_WCDMA_FDD_BAND23                             	0x400000                                          	
#define INFO_WCDMA_FDD_BAND24                             	0x800000                                          	
#define INFO_WCDMA_FDD_BAND25                             	0x1000000                                         	
#define INFO_WCDMA_FDD_BAND26                             	0x2000000                                         	
#define INFO_WCDMA_FDD_BAND27                             	0x4000000                                         	
#define INFO_WCDMA_FDD_BAND28                             	0x8000000                                         	
#define INFO_WCDMA_FDD_BAND29                             	0x10000000                                        	
#define INFO_WCDMA_FDD_BAND30                             	0x20000000                                        	
#define INFO_WCDMA_FDD_BAND31                             	0x40000000                                        	
#define INFO_WCDMA_FDD_BAND32                             	0x80000000                                        	

//BitmaskTables for INFO_WCDMA_RX_DIV_BAND
#define INFO_NO_WCDMA_RX_DIV                              	0x00                                                 	
#define INFO_WCDMA_RX_DIV_FDD_BAND1                       	0x01                                                 	
#define INFO_WCDMA_RX_DIV_FDD_BAND2                       	0x02                                                 	
#define INFO_WCDMA_RX_DIV_FDD_BAND3                       	0x04                                                 	
#define INFO_WCDMA_RX_DIV_FDD_BAND4                       	0x08                                                 	
#define INFO_WCDMA_RX_DIV_FDD_BAND5                       	0x10                                              	
#define INFO_WCDMA_RX_DIV_FDD_BAND6                       	0x20                                              	
#define INFO_WCDMA_RX_DIV_FDD_BAND7                       	0x40                                              	
#define INFO_WCDMA_RX_DIV_FDD_BAND8                       	0x80                                              	
#define INFO_WCDMA_RX_DIV_FDD_BAND9                       	0x100                                             	
#define INFO_WCDMA_RX_DIV_FDD_BAND10                      	0x200                                             	
#define INFO_WCDMA_RX_DIV_FDD_BAND11                      	0x400                                             	
#define INFO_WCDMA_RX_DIV_FDD_BAND12                      	0x800                                             	
#define INFO_WCDMA_RX_DIV_FDD_BAND13                      	0x1000                                            	
#define INFO_WCDMA_RX_DIV_FDD_BAND14                      	0x2000                                            	
#define INFO_WCDMA_RX_DIV_FDD_BAND15                      	0x4000                                            	
#define INFO_WCDMA_RX_DIV_FDD_BAND16                      	0x8000                                            	
#define INFO_WCDMA_RX_DIV_FDD_BAND17                      	0x10000                                           	
#define INFO_WCDMA_RX_DIV_FDD_BAND18                      	0x20000                                           	
#define INFO_WCDMA_RX_DIV_FDD_BAND19                      	0x40000                                           	
#define INFO_WCDMA_RX_DIV_FDD_BAND20                      	0x80000                                           	
#define INFO_WCDMA_RX_DIV_FDD_BAND21                      	0x100000                                          	
#define INFO_WCDMA_RX_DIV_FDD_BAND22                      	0x200000                                          	
#define INFO_WCDMA_RX_DIV_FDD_BAND23                      	0x400000                                          	
#define INFO_WCDMA_RX_DIV_FDD_BAND24                      	0x800000                                          	
#define INFO_WCDMA_RX_DIV_FDD_BAND25                      	0x1000000                                         	
#define INFO_WCDMA_RX_DIV_FDD_BAND26                      	0x2000000                                         	
#define INFO_WCDMA_RX_DIV_FDD_BAND27                      	0x4000000                                         	
#define INFO_WCDMA_RX_DIV_FDD_BAND28                      	0x8000000                                         	
#define INFO_WCDMA_RX_DIV_FDD_BAND29                      	0x10000000                                        	
#define INFO_WCDMA_RX_DIV_FDD_BAND30                      	0x20000000                                        	
#define INFO_WCDMA_RX_DIV_FDD_BAND31                      	0x40000000                                        	
#define INFO_WCDMA_RX_DIV_FDD_BAND32                      	0x80000000                                        	

//BitmaskTables for INFO_EUTRAN_BAND
#define INFO_NO_EUTRAN                                    	0x00                                                 	
#define INFO_EUTRAN_BAND1                                 	0x01                                                 	
#define INFO_EUTRAN_BAND2                                 	0x02                                                 	
#define INFO_EUTRAN_BAND3                                 	0x04                                                 	
#define INFO_EUTRAN_BAND4                                 	0x08                                                 	
#define INFO_EUTRAN_BAND5                                 	0x10                                              	
#define INFO_EUTRAN_BAND6                                 	0x20                                              	
#define INFO_EUTRAN_BAND7                                 	0x40                                              	
#define INFO_EUTRAN_BAND8                                 	0x80                                              	
#define INFO_EUTRAN_BAND9                                 	0x100                                             	
#define INFO_EUTRAN_BAND10                                	0x200                                             	
#define INFO_EUTRAN_BAND11                                	0x400                                             	
#define INFO_EUTRAN_BAND12                                	0x800                                             	
#define INFO_EUTRAN_BAND13                                	0x1000                                            	
#define INFO_EUTRAN_BAND14                                	0x2000                                            	
#define INFO_EUTRAN_BAND15                                	0x4000                                            	
#define INFO_EUTRAN_BAND16                                	0x8000                                            	
#define INFO_EUTRAN_BAND17                                	0x10000                                           	
#define INFO_EUTRAN_BAND18                                	0x20000                                           	
#define INFO_EUTRAN_BAND19                                	0x40000                                           	
#define INFO_EUTRAN_BAND20                                	0x80000                                           	
#define INFO_EUTRAN_BAND21                                	0x100000                                          	
#define INFO_EUTRAN_BAND22                                	0x200000                                          	
#define INFO_EUTRAN_BAND23                                	0x400000                                          	
#define INFO_EUTRAN_BAND24                                	0x800000                                          	
#define INFO_EUTRAN_BAND25                                	0x1000000                                         	
#define INFO_EUTRAN_BAND26                                	0x2000000                                         	
#define INFO_EUTRAN_BAND27                                	0x4000000                                         	
#define INFO_EUTRAN_BAND28                                	0x8000000                                         	
#define INFO_EUTRAN_BAND29                                	0x10000000                                        	
#define INFO_EUTRAN_BAND30                                	0x20000000                                        	
#define INFO_EUTRAN_BAND31                                	0x40000000                                        	
#define INFO_EUTRAN_BAND32                                	0x80000000                                        	


//SUBBLOCKS


//Definition for INFO_SB_SN_IMEI_PLAIN
#define INFO_SB_SN_IMEI_PLAIN                             	0x41                                              	
#define INFO_SB_SN_IMEI_PLAIN_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define INFO_SB_SN_IMEI_PLAIN_OFFSET_SUBBLOCKLEN          	1 //size 1 byte(s)
#define INFO_SB_SN_IMEI_PLAIN_OFFSET_FILLERBYTE1          	2 //size 1 byte(s)
#define INFO_SB_SN_IMEI_PLAIN_OFFSET_STRLEN               	3 //size 1 byte(s)
#define INFO_SB_SN_IMEI_PLAIN_OFFSET_IMEIPLAINU8          	4 //size 16 byte(s)
#define SIZE_INFO_SB_SN_IMEI_PLAIN                        	20
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_SN_IMEI_SPARE_TO_NET
#define INFO_SB_SN_IMEI_SPARE_TO_NET                      	0x42                                              	
#define INFO_SB_SN_IMEI_SPARE_TO_NET_OFFSET_SUBBLOCKID    	0 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SPARE_TO_NET_OFFSET_SUBBLOCKLEN   	1 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SPARE_TO_NET_OFFSET_FILLERBYTE1   	2 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SPARE_TO_NET_OFFSET_STRLEN        	3 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SPARE_TO_NET_OFFSET_IMEISPAREU8   	4 //size 8 byte(s)
#define SIZE_INFO_SB_SN_IMEI_SPARE_TO_NET                 	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_SN_IMEI_SV_TO_NET
#define INFO_SB_SN_IMEI_SV_TO_NET                         	0x43                                              	
#define INFO_SB_SN_IMEI_SV_TO_NET_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SV_TO_NET_OFFSET_SUBBLOCKLEN      	1 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SV_TO_NET_OFFSET_FILLERBYTE1      	2 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SV_TO_NET_OFFSET_STRLEN           	3 //size 1 byte(s)
#define INFO_SB_SN_IMEI_SV_TO_NET_OFFSET_IMEISVU8         	4 //size 9 byte(s)
#define INFO_SB_SN_IMEI_SV_TO_NET_OFFSET_FILLERBYTE2      	13 //size 3 byte(s)
#define SIZE_INFO_SB_SN_IMEI_SV_TO_NET                    	16
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_MCUSW_VERSION
#define INFO_SB_MCUSW_VERSION                             	0x48                                              	
#define INFO_SB_MCUSW_VERSION_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define INFO_SB_MCUSW_VERSION_OFFSET_SUBBLOCKLEN          	1 //size 1 byte(s)
#define INFO_SB_MCUSW_VERSION_OFFSET_FILLERBYTE1          	2 //size 1 byte(s)
#define INFO_SB_MCUSW_VERSION_OFFSET_STRLEN               	3 //size 1 byte(s)
#define INFO_SB_MCUSW_VERSION_OFFSET_MCUSWVERSNC8         	4 //size 1 byte(s)
#define INFO_SB_MCUSW_VERSION_OFFSET_FILLERBYTE2          	5 //size 1 byte(s)
#define INFO_SB_MCUSW_VERSION_OFFSET_FILLERBYTE3          	6 //size 1 byte(s)
#define INFO_SB_MCUSW_VERSION_OFFSET_FILLERBYTE4          	7 //size 1 byte(s)
#define SIZE_INFO_SB_MCUSW_VERSION                        	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_PP_DATA
#define INFO_SB_PP_DATA                                   	0x74                                              	
#define INFO_SB_PP_DATA_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define INFO_SB_PP_DATA_OFFSET_FILLERBYTE1                	1 //size 1 byte(s)
#define INFO_SB_PP_DATA_OFFSET_SUBBLOCKLEN                	2 //size 2 byte(s)
#define INFO_SB_PP_DATA_OFFSET_FILLERBYTE2                	4 //size 2 byte(s)
#define INFO_SB_PP_DATA_OFFSET_PPFEACNT                   	6 //size 2 byte(s)
#define INFO_SB_PP_DATA_OFFSET_PPFEAVAL                   	8 //size 4 byte(s)
#define SIZE_INFO_SB_PP_DATA                              	12
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for INFO_SB_RAT_BAND_INFO
#define INFO_SB_RAT_BAND_INFO                             	0xF000                                            	
#define INFO_SB_RAT_BAND_INFO_OFFSET_SBID                 	0 //size 2 byte(s)
#define INFO_SB_RAT_BAND_INFO_OFFSET_SBLEN                	2 //size 1 byte(s)
#define INFO_SB_RAT_BAND_INFO_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define INFO_SB_RAT_BAND_INFO_OFFSET_RATS                 	4 //size 4 byte(s)
#define INFO_SB_RAT_BAND_INFO_OFFSET_BANDS                	8 //size 128 byte(s)
#define SIZE_INFO_SB_RAT_BAND_INFO                        	136
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//MESSAGES


//Definition for INFO_SERIAL_NUMBER_READ_REQ
#define INFO_SERIAL_NUMBER_READ_REQ                       	0x00                                                 	
#define INFO_SERIAL_NUMBER_READ_REQ_OFFSET_TRANSID        	0 //size 1 byte(s)
#define INFO_SERIAL_NUMBER_READ_REQ_OFFSET_MESSAGEID      	1 //size 1 byte(s)
#define INFO_SERIAL_NUMBER_READ_REQ_OFFSET_TARGET         	2 //size 1 byte(s)
#define SIZE_INFO_SERIAL_NUMBER_READ_REQ                  	3


//Definition for INFO_SERIAL_NUMBER_READ_RESP
#define INFO_SERIAL_NUMBER_READ_RESP                      	0x01                                                 	
#define INFO_SERIAL_NUMBER_READ_RESP_OFFSET_TRANSID       	0 //size 1 byte(s)
#define INFO_SERIAL_NUMBER_READ_RESP_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define INFO_SERIAL_NUMBER_READ_RESP_OFFSET_STATUS        	2 //size 1 byte(s)
#define INFO_SERIAL_NUMBER_READ_RESP_OFFSET_SUBBLOCKCOUNT 	3 //size 1 byte(s)
#define SIZE_INFO_SERIAL_NUMBER_READ_RESP                 	4


//Definition for INFO_PP_CUSTOMER_DEFAULTS_REQ
#define INFO_PP_CUSTOMER_DEFAULTS_REQ                     	0x0F                                                 	
#define INFO_PP_CUSTOMER_DEFAULTS_REQ_OFFSET_TRANSID      	0 //size 1 byte(s)
#define INFO_PP_CUSTOMER_DEFAULTS_REQ_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define INFO_PP_CUSTOMER_DEFAULTS_REQ_OFFSET_CUSTOMERID   	2 //size 1 byte(s)
#define SIZE_INFO_PP_CUSTOMER_DEFAULTS_REQ                	3


//Definition for INFO_PP_CUSTOMER_DEFAULTS_RESP
#define INFO_PP_CUSTOMER_DEFAULTS_RESP                    	0x10                                              	
#define INFO_PP_CUSTOMER_DEFAULTS_RESP_OFFSET_TRANSID     	0 //size 1 byte(s)
#define INFO_PP_CUSTOMER_DEFAULTS_RESP_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define INFO_PP_CUSTOMER_DEFAULTS_RESP_OFFSET_STATUS      	2 //size 1 byte(s)
#define SIZE_INFO_PP_CUSTOMER_DEFAULTS_RESP               	3


//Definition for INFO_PP_DATA_READ_REQ
#define INFO_PP_DATA_READ_REQ                             	0x29                                              	
#define INFO_PP_DATA_READ_REQ_OFFSET_TRANSID              	0 //size 1 byte(s)
#define INFO_PP_DATA_READ_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define INFO_PP_DATA_READ_REQ_OFFSET_FILLER               	2 //size 1 byte(s)
#define INFO_PP_DATA_READ_REQ_OFFSET_SUBBLOCKCOUNT        	3 //size 1 byte(s)
#define SIZE_INFO_PP_DATA_READ_REQ                        	4


//Definition for INFO_PP_DATA_READ_RESP
#define INFO_PP_DATA_READ_RESP                            	0x2A                                              	
#define INFO_PP_DATA_READ_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define INFO_PP_DATA_READ_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define INFO_PP_DATA_READ_RESP_OFFSET_STATUS              	2 //size 1 byte(s)
#define INFO_PP_DATA_READ_RESP_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_INFO_PP_DATA_READ_RESP                       	4


//Definition for INFO_PP_DATA_WRITE_REQ
#define INFO_PP_DATA_WRITE_REQ                            	0x2B                                              	
#define INFO_PP_DATA_WRITE_REQ_OFFSET_TRANSID             	0 //size 1 byte(s)
#define INFO_PP_DATA_WRITE_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define INFO_PP_DATA_WRITE_REQ_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define INFO_PP_DATA_WRITE_REQ_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_INFO_PP_DATA_WRITE_REQ                       	4


//Definition for INFO_PP_DATA_WRITE_RESP
#define INFO_PP_DATA_WRITE_RESP                           	0x2C                                              	
#define INFO_PP_DATA_WRITE_RESP_OFFSET_TRANSID            	0 //size 1 byte(s)
#define INFO_PP_DATA_WRITE_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define INFO_PP_DATA_WRITE_RESP_OFFSET_FILLERBYTE1        	2 //size 1 byte(s)
#define INFO_PP_DATA_WRITE_RESP_OFFSET_STATUS             	3 //size 1 byte(s)
#define SIZE_INFO_PP_DATA_WRITE_RESP                      	4


//Definition for INFO_PP_DATA_IND
#define INFO_PP_DATA_IND                                  	0x2D                                              	
#define INFO_PP_DATA_IND_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define INFO_PP_DATA_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define INFO_PP_DATA_IND_OFFSET_FILLERBYTE1               	2 //size 1 byte(s)
#define INFO_PP_DATA_IND_OFFSET_SUBBLOCKCOUNT             	3 //size 1 byte(s)
#define SIZE_INFO_PP_DATA_IND                             	4


//Definition for INFO_VERSION_READ_REQ
#define INFO_VERSION_READ_REQ                             	0x07                                                 	
#define INFO_VERSION_READ_REQ_OFFSET_TRANSID              	0 //size 1 byte(s)
#define INFO_VERSION_READ_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define INFO_VERSION_READ_REQ_OFFSET_TARGET               	2 //size 2 byte(s)
#define SIZE_INFO_VERSION_READ_REQ                        	4


//Definition for INFO_VERSION_READ_RESP
#define INFO_VERSION_READ_RESP                            	0x08                                                 	
#define INFO_VERSION_READ_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define INFO_VERSION_READ_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define INFO_VERSION_READ_RESP_OFFSET_STATUS              	2 //size 1 byte(s)
#define INFO_VERSION_READ_RESP_OFFSET_SUBBLOCKCOUNT       	3 //size 1 byte(s)
#define SIZE_INFO_VERSION_READ_RESP                       	4


//Definition for INFO_PRODUCT_RAT_BAND_READ_REQ
#define INFO_PRODUCT_RAT_BAND_READ_REQ                    	0x2E                                              	
#define INFO_PRODUCT_RAT_BAND_READ_REQ_OFFSET_TRANSID     	0 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_READ_REQ_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_READ_REQ_OFFSET_FILLERBYTE1 	2 //size 2 byte(s)
#define SIZE_INFO_PRODUCT_RAT_BAND_READ_REQ               	4


//Definition for INFO_PRODUCT_RAT_BAND_READ_RESP
#define INFO_PRODUCT_RAT_BAND_READ_RESP                   	0x2F                                              	
#define INFO_PRODUCT_RAT_BAND_READ_RESP_OFFSET_TRANSID    	0 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_READ_RESP_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_READ_RESP_OFFSET_STATUS     	2 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_READ_RESP_OFFSET_SUBBLOCKCOUNT	3 //size 1 byte(s)
#define SIZE_INFO_PRODUCT_RAT_BAND_READ_RESP              	4


//Definition for INFO_PRODUCT_RAT_BAND_WRITE_REQ
#define INFO_PRODUCT_RAT_BAND_WRITE_REQ                   	0x30                                              	
#define INFO_PRODUCT_RAT_BAND_WRITE_REQ_OFFSET_TRANSID    	0 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_WRITE_REQ_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_WRITE_REQ_OFFSET_FILLERBYTE1	2 //size 2 byte(s)
#define SIZE_INFO_PRODUCT_RAT_BAND_WRITE_REQ              	4


//Definition for INFO_PRODUCT_RAT_BAND_WRITE_RESP
#define INFO_PRODUCT_RAT_BAND_WRITE_RESP                  	0x31                                              	
#define INFO_PRODUCT_RAT_BAND_WRITE_RESP_OFFSET_TRANSID   	0 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_WRITE_RESP_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_WRITE_RESP_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define INFO_PRODUCT_RAT_BAND_WRITE_RESP_OFFSET_STATUS    	3 //size 1 byte(s)
#define SIZE_INFO_PRODUCT_RAT_BAND_WRITE_RESP             	4

#endif