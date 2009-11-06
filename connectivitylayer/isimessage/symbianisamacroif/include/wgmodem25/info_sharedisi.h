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
* Name      : PN_MODEM_INFO [ 0xC5 ] Resource Modem Info Server shared definitions
* Version   : 001.002
*
*/


#ifndef _SYMBIAN_ISI_INFO_SHAREDISI_H_
#define _SYMBIAN_ISI_INFO_SHAREDISI_H_

#define MODEM_INFO_SERVER_SHARED_DEFINITIONS_ISI_VERSION_Z	 1
#define MODEM_INFO_SERVER_SHARED_DEFINITIONS_ISI_VERSION_Y	 2


//CONSTANTS
#define INFO_ANY_SIZE                                     	0x01                                                 	
#define INFO_SB_APE_HW_VER_LEN                            	0x05                                                 	
#define INFO_SB_APE_ADSP_VER_LEN                          	0x05                                                 	


//TABLES

//ConstantTable for INFO_CONTENT_LOCATION
#define UDA_APE                                           	0x01                                                 	
#define HDD_APE                                           	0x02                                                 	
#define MC_APE                                            	0x04                                                 	

//ConstantTable for INFO_ROFS_ID
#define INFO_APE_ROFS_VARIANT1                            	0x01                                                 	
#define INFO_APE_ROFS_VARIANT2                            	0x02                                                 	
#define INFO_APE_ROFS_VARIANT3                            	0x03                                                 	
#define INFO_APE_ROFS_VARIANT4                            	0x04                                                 	
#define INFO_APE_ROFS_VARIANT5                            	0x05                                                 	
#define INFO_APE_ROFS_VARIANT6                            	0x06                                                 	


//SUBBLOCKS


//Definition for INFO_SB_LCD_VERSION
#define INFO_SB_LCD_VERSION                               	0x57                                              	
#define INFO_SB_LCD_VERSION_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define INFO_SB_LCD_VERSION_OFFSET_SUBBLOCKLEN            	1 //size 1 byte(s)
#define INFO_SB_LCD_VERSION_OFFSET_FILLERBYTE1            	2 //size 1 byte(s)
#define INFO_SB_LCD_VERSION_OFFSET_STRLEN                 	3 //size 1 byte(s)
#define INFO_SB_LCD_VERSION_OFFSET_LCDVERSNSTRC8          	4 //size 1 byte(s)
#define INFO_SB_LCD_VERSION_OFFSET_FILLERBYTE2            	5 //size 1 byte(s)
#define INFO_SB_LCD_VERSION_OFFSET_FILLERBYTE3            	6 //size 1 byte(s)
#define INFO_SB_LCD_VERSION_OFFSET_FILLERBYTE4            	7 //size 1 byte(s)
#define SIZE_INFO_SB_LCD_VERSION                          	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_FLIP_MCUSW_VERSION
#define INFO_SB_FLIP_MCUSW_VERSION                        	0x62                                              	
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_SUBBLOCKID      	0 //size 1 byte(s)
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_SUBBLOCKLEN     	1 //size 1 byte(s)
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_FILLERBYTE1     	2 //size 1 byte(s)
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_STRLEN          	3 //size 1 byte(s)
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_MCUSWVERSNC8    	4 //size 1 byte(s)
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_FILLERBYTE2     	5 //size 1 byte(s)
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_FILLERBYTE3     	6 //size 1 byte(s)
#define INFO_SB_FLIP_MCUSW_VERSION_OFFSET_FILLERBYTE4     	7 //size 1 byte(s)
#define SIZE_INFO_SB_FLIP_MCUSW_VERSION                   	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_BOOTCODE_VER
#define INFO_SB_BOOTCODE_VER                              	0x68                                              	
#define INFO_SB_BOOTCODE_VER_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define INFO_SB_BOOTCODE_VER_OFFSET_SUBBLOCKLEN           	1 //size 1 byte(s)
#define INFO_SB_BOOTCODE_VER_OFFSET_FILLERBYTE1           	2 //size 1 byte(s)
#define INFO_SB_BOOTCODE_VER_OFFSET_STRLEN                	3 //size 1 byte(s)
#define INFO_SB_BOOTCODE_VER_OFFSET_IMAGEVER              	4 //size 1 byte(s)
#define INFO_SB_BOOTCODE_VER_OFFSET_FILLERBYTE2           	5 //size 1 byte(s)
#define INFO_SB_BOOTCODE_VER_OFFSET_FILLERBYTE3           	6 //size 1 byte(s)
#define INFO_SB_BOOTCODE_VER_OFFSET_FILLERBYTE4           	7 //size 1 byte(s)
#define SIZE_INFO_SB_BOOTCODE_VER                         	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_VARIANT_VER
#define INFO_SB_VARIANT_VER                               	0x6A                                              	
#define INFO_SB_VARIANT_VER_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define INFO_SB_VARIANT_VER_OFFSET_SUBBLOCKLEN            	1 //size 1 byte(s)
#define INFO_SB_VARIANT_VER_OFFSET_FILLERBYTE1            	2 //size 1 byte(s)
#define INFO_SB_VARIANT_VER_OFFSET_STRLEN                 	3 //size 1 byte(s)
#define INFO_SB_VARIANT_VER_OFFSET_IMAGEVER               	4 //size 1 byte(s)
#define INFO_SB_VARIANT_VER_OFFSET_FILLERBYTE2            	5 //size 1 byte(s)
#define INFO_SB_VARIANT_VER_OFFSET_FILLERBYTE3            	6 //size 1 byte(s)
#define INFO_SB_VARIANT_VER_OFFSET_FILLERBYTE4            	7 //size 1 byte(s)
#define SIZE_INFO_SB_VARIANT_VER                          	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_APE_TEST_VER
#define INFO_SB_APE_TEST_VER                              	0x6B                                              	
#define INFO_SB_APE_TEST_VER_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define INFO_SB_APE_TEST_VER_OFFSET_SUBBLOCKLEN           	1 //size 1 byte(s)
#define INFO_SB_APE_TEST_VER_OFFSET_FILLERBYTE1           	2 //size 1 byte(s)
#define INFO_SB_APE_TEST_VER_OFFSET_STRLEN                	3 //size 1 byte(s)
#define INFO_SB_APE_TEST_VER_OFFSET_IMAGEVER              	4 //size 1 byte(s)
#define INFO_SB_APE_TEST_VER_OFFSET_FILLERBYTE2           	5 //size 1 byte(s)
#define INFO_SB_APE_TEST_VER_OFFSET_FILLERBYTE3           	6 //size 1 byte(s)
#define INFO_SB_APE_TEST_VER_OFFSET_FILLERBYTE4           	7 //size 1 byte(s)
#define SIZE_INFO_SB_APE_TEST_VER                         	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_APE_HW_VERSION
#define INFO_SB_APE_HW_VERSION                            	0x6E                                              	
#define INFO_SB_APE_HW_VERSION_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define INFO_SB_APE_HW_VERSION_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define INFO_SB_APE_HW_VERSION_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define INFO_SB_APE_HW_VERSION_OFFSET_STRLEN              	3 //size 1 byte(s)
#define INFO_SB_APE_HW_VERSION_OFFSET_APEHWVERSION        	4 //size 1 byte(s)
#define INFO_SB_APE_HW_VERSION_OFFSET_FILLERBYTE2         	5 //size 1 byte(s)
#define INFO_SB_APE_HW_VERSION_OFFSET_FILLERBYTE3         	6 //size 1 byte(s)
#define INFO_SB_APE_HW_VERSION_OFFSET_FILLERBYTE4         	7 //size 1 byte(s)
#define SIZE_INFO_SB_APE_HW_VERSION                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_APE_ADSP_SW
#define INFO_SB_APE_ADSP_SW                               	0x6F                                              	
#define INFO_SB_APE_ADSP_SW_OFFSET_SUBBLOCKID             	0 //size 1 byte(s)
#define INFO_SB_APE_ADSP_SW_OFFSET_SUBBLOCKLEN            	1 //size 1 byte(s)
#define INFO_SB_APE_ADSP_SW_OFFSET_FILLERBYTE1            	2 //size 1 byte(s)
#define INFO_SB_APE_ADSP_SW_OFFSET_STRLEN                 	3 //size 1 byte(s)
#define INFO_SB_APE_ADSP_SW_OFFSET_SWVERSION              	4 //size 1 byte(s)
#define INFO_SB_APE_ADSP_SW_OFFSET_FILLERBYTE2            	5 //size 1 byte(s)
#define INFO_SB_APE_ADSP_SW_OFFSET_FILLERBYTE3            	6 //size 1 byte(s)
#define INFO_SB_APE_ADSP_SW_OFFSET_FILLERBYTE4            	7 //size 1 byte(s)
#define SIZE_INFO_SB_APE_ADSP_SW                          	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_CAMERA_VER
#define INFO_SB_CAMERA_VER                                	0x70                                              	
#define INFO_SB_CAMERA_VER_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define INFO_SB_CAMERA_VER_OFFSET_SUBBLOCKLEN             	1 //size 1 byte(s)
#define INFO_SB_CAMERA_VER_OFFSET_FILLERBYTE1             	2 //size 1 byte(s)
#define INFO_SB_CAMERA_VER_OFFSET_STRLEN                  	3 //size 1 byte(s)
#define INFO_SB_CAMERA_VER_OFFSET_CAMERAVER               	4 //size 1 byte(s)
#define INFO_SB_CAMERA_VER_OFFSET_FILLERBYTE2             	5 //size 1 byte(s)
#define INFO_SB_CAMERA_VER_OFFSET_FILLERBYTE3             	6 //size 1 byte(s)
#define INFO_SB_CAMERA_VER_OFFSET_FILLERBYTE4             	7 //size 1 byte(s)
#define SIZE_INFO_SB_CAMERA_VER                           	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_CAMERA_DETAILED_VERSION_INFO
#define INFO_SB_CAMERA_DETAILED_VERSION_INFO              	0x75                                              	
#define INFO_SB_CAMERA_DETAILED_VERSION_INFO_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define INFO_SB_CAMERA_DETAILED_VERSION_INFO_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define INFO_SB_CAMERA_DETAILED_VERSION_INFO_OFFSET_MADEBY	2 //size 1 byte(s)
#define INFO_SB_CAMERA_DETAILED_VERSION_INFO_OFFSET_VERSION	3 //size 1 byte(s)
#define INFO_SB_CAMERA_DETAILED_VERSION_INFO_OFFSET_MODEL 	4 //size 2 byte(s)
#define INFO_SB_CAMERA_DETAILED_VERSION_INFO_OFFSET_FILLERBYTE1	6 //size 6 byte(s)
#define SIZE_INFO_SB_CAMERA_DETAILED_VERSION_INFO         	12


//Definition for INFO_SB_APE_BT_VERSION
#define INFO_SB_APE_BT_VERSION                            	0x71                                              	
#define INFO_SB_APE_BT_VERSION_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define INFO_SB_APE_BT_VERSION_OFFSET_SUBBLOCKLEN         	1 //size 1 byte(s)
#define INFO_SB_APE_BT_VERSION_OFFSET_FILLERBYTE1         	2 //size 1 byte(s)
#define INFO_SB_APE_BT_VERSION_OFFSET_STRLEN              	3 //size 1 byte(s)
#define INFO_SB_APE_BT_VERSION_OFFSET_BTVERSION           	4 //size 1 byte(s)
#define INFO_SB_APE_BT_VERSION_OFFSET_FILLERBYTE2         	5 //size 1 byte(s)
#define INFO_SB_APE_BT_VERSION_OFFSET_FILLERBYTE3         	6 //size 1 byte(s)
#define INFO_SB_APE_BT_VERSION_OFFSET_FILLERBYTE4         	7 //size 1 byte(s)
#define SIZE_INFO_SB_APE_BT_VERSION                       	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_CONTENT_PACK_VER
#define INFO_SB_CONTENT_PACK_VER                          	0x72                                              	
#define INFO_SB_CONTENT_PACK_VER_OFFSET_SUBBLOCKID        	0 //size 1 byte(s)
#define INFO_SB_CONTENT_PACK_VER_OFFSET_SUBBLOCKLEN       	1 //size 1 byte(s)
#define INFO_SB_CONTENT_PACK_VER_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define INFO_SB_CONTENT_PACK_VER_OFFSET_STRLEN            	3 //size 1 byte(s)
#define INFO_SB_CONTENT_PACK_VER_OFFSET_PACKAGEVER        	4 //size 1 byte(s)
#define INFO_SB_CONTENT_PACK_VER_OFFSET_FILLERBYTE2       	5 //size 1 byte(s)
#define INFO_SB_CONTENT_PACK_VER_OFFSET_FILLERBYTE3       	6 //size 1 byte(s)
#define INFO_SB_CONTENT_PACK_VER_OFFSET_FILLERBYTE4       	7 //size 1 byte(s)
#define SIZE_INFO_SB_CONTENT_PACK_VER                     	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_APE_CUI_LCD_VERSION
#define INFO_SB_APE_CUI_LCD_VERSION                       	0x9F                                              	
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_FILLERBYTE1    	2 //size 1 byte(s)
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_VERSIONSTRINGLEN	3 //size 1 byte(s)
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_VERSIONSTRING  	4 //size 1 byte(s)
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_FILLERBYTE2    	5 //size 1 byte(s)
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_FILLERBYTE3    	6 //size 1 byte(s)
#define INFO_SB_APE_CUI_LCD_VERSION_OFFSET_FILLERBYTE4    	7 //size 1 byte(s)
#define SIZE_INFO_SB_APE_CUI_LCD_VERSION                  	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_APE_IPDC_VERSION
#define INFO_SB_APE_IPDC_VERSION                          	0xA1                                              	
#define INFO_SB_APE_IPDC_VERSION_OFFSET_SUBBLOCKID        	0 //size 1 byte(s)
#define INFO_SB_APE_IPDC_VERSION_OFFSET_SUBBLOCKLEN       	1 //size 1 byte(s)
#define INFO_SB_APE_IPDC_VERSION_OFFSET_FILLERBYTE1       	2 //size 1 byte(s)
#define INFO_SB_APE_IPDC_VERSION_OFFSET_VERSIONSTRINGLEN  	3 //size 1 byte(s)
#define INFO_SB_APE_IPDC_VERSION_OFFSET_VERSIONSTRING     	4 //size 1 byte(s)
#define INFO_SB_APE_IPDC_VERSION_OFFSET_FILLERBYTE2       	5 //size 1 byte(s)
#define INFO_SB_APE_IPDC_VERSION_OFFSET_FILLERBYTE3       	6 //size 1 byte(s)
#define INFO_SB_APE_IPDC_VERSION_OFFSET_FILLERBYTE4       	7 //size 1 byte(s)
#define SIZE_INFO_SB_APE_IPDC_VERSION                     	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_APE_DVB_H_RX_HW_VER
#define INFO_SB_APE_DVB_H_RX_HW_VER                       	0xA2                                              	
#define INFO_SB_APE_DVB_H_RX_HW_VER_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define INFO_SB_APE_DVB_H_RX_HW_VER_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define INFO_SB_APE_DVB_H_RX_HW_VER_OFFSET_FILLERBYTE1    	2 //size 2 byte(s)
#define INFO_SB_APE_DVB_H_RX_HW_VER_OFFSET_DVBHRXHWVER    	4 //size 4 byte(s)
#define SIZE_INFO_SB_APE_DVB_H_RX_HW_VER                  	8


//Definition for INFO_SB_APE_DVB_H_RX_SW_VER
#define INFO_SB_APE_DVB_H_RX_SW_VER                       	0xA3                                              	
#define INFO_SB_APE_DVB_H_RX_SW_VER_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define INFO_SB_APE_DVB_H_RX_SW_VER_OFFSET_SUBBLOCKLEN    	1 //size 1 byte(s)
#define INFO_SB_APE_DVB_H_RX_SW_VER_OFFSET_FILLERBYTE1    	2 //size 2 byte(s)
#define INFO_SB_APE_DVB_H_RX_SW_VER_OFFSET_DVBHRXSWVER    	4 //size 4 byte(s)
#define SIZE_INFO_SB_APE_DVB_H_RX_SW_VER                  	8


//Definition for INFO_SB_APE_DVB_H_RX_BOOTLOADER_VER
#define INFO_SB_APE_DVB_H_RX_BOOTLOADER_VER               	0xA4                                              	
#define INFO_SB_APE_DVB_H_RX_BOOTLOADER_VER_OFFSET_SUBBLOCKID	0 //size 1 byte(s)
#define INFO_SB_APE_DVB_H_RX_BOOTLOADER_VER_OFFSET_SUBBLOCKLEN	1 //size 1 byte(s)
#define INFO_SB_APE_DVB_H_RX_BOOTLOADER_VER_OFFSET_FILLERBYTE1	2 //size 2 byte(s)
#define INFO_SB_APE_DVB_H_RX_BOOTLOADER_VER_OFFSET_DVBHRXBOOTLVER	4 //size 4 byte(s)
#define SIZE_INFO_SB_APE_DVB_H_RX_BOOTLOADER_VER          	8


//Definition for INFO_SB_CONTENT_VERSIONS
#define INFO_SB_CONTENT_VERSIONS                          	0xA5                                              	
#define INFO_SB_CONTENT_VERSIONS_OFFSET_SUBBLOCKID        	0 //size 1 byte(s)
#define INFO_SB_CONTENT_VERSIONS_OFFSET_SUBBLOCKLEN       	1 //size 1 byte(s)
#define INFO_SB_CONTENT_VERSIONS_OFFSET_LOCATION          	2 //size 2 byte(s)
#define INFO_SB_CONTENT_VERSIONS_OFFSET_LENGTH            	4 //size 1 byte(s)
#define INFO_SB_CONTENT_VERSIONS_OFFSET_CONTVER           	5 //size 1 byte(s)
#define INFO_SB_CONTENT_VERSIONS_OFFSET_FILLERBYTE1       	6 //size 1 byte(s)
#define INFO_SB_CONTENT_VERSIONS_OFFSET_FILLERBYTE2       	7 //size 1 byte(s)
#define SIZE_INFO_SB_CONTENT_VERSIONS                     	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for INFO_SB_ROFS_VERSION
#define INFO_SB_ROFS_VERSION                              	0xA6                                              	
#define INFO_SB_ROFS_VERSION_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define INFO_SB_ROFS_VERSION_OFFSET_SUBBLOCKLEN           	1 //size 1 byte(s)
#define INFO_SB_ROFS_VERSION_OFFSET_ROFSID                	2 //size 2 byte(s)
#define INFO_SB_ROFS_VERSION_OFFSET_FILLERBYTE1           	4 //size 1 byte(s)
#define INFO_SB_ROFS_VERSION_OFFSET_LENGTH                	5 //size 1 byte(s)
#define INFO_SB_ROFS_VERSION_OFFSET_ROFSVER               	6 //size 1 byte(s)
#define INFO_SB_ROFS_VERSION_OFFSET_FILLERBYTE2           	7 //size 1 byte(s)
#define SIZE_INFO_SB_ROFS_VERSION                         	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.

#endif