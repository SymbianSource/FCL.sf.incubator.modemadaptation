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
* Name      : PN_MODEM_LCS [ 0x90 ] Resource Modem LCS Server
* Version   : 006.000
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_LOCATION_WMISI_H_
#define _SYMBIAN_ISI_LOCATION_WMISI_H_

#define MODEM_LCS_SERVER_ISI_VERSION_Z	 7
#define MODEM_LCS_SERVER_ISI_VERSION_Y	 0


//CONSTANTS
#define MODEM_LCS_MAX_EXT_GEO_INFO_LENGTH                 	0x14                                              	
#define MODEM_LCS_NOT_AVAILABLE                           	0xFFFF                                            	
#define MODEM_LCS_ABSENT                                  	0xFF                                              	
#define MODEM_LCS_RRLP_UNDEFINED_ACCURACY                 	0xFF                                              	
#define MODEM_LCS_SS_MAX_CODEWORD_LEN                     	0x16                                              	
#define MODEM_LCS_ANY_SIZE                                	0x01                                                 	
#define MODEM_LCS_NO_REF_FRAME                            	0xFFFF                                            	
#define MODEM_LCS_MAX_MEAS_AMOUNT                         	0x10                                              	
#define MODEM_LCS_MAX_MEAS_SET_COUNT                      	0x03                                                 	
#define MODEM_LCS_ASN1_MAX_LENGTH                         	0x2000                                            	
#define MODEM_LCS_MAX_USSD_LEN                            	0x48                                              	
#define MODEM_LCS_MAX_CLIENT_EXT_ID_BYTES                 	0x10                                              	
#define MODEM_LCS_SS_MAX_REQUESTOR_ID_LEN                 	0x48                                              	
#define MODEM_LCS_UNDEFINED_GPS_TOW                       	0xFFFFFFFF                                        	
#define MODEM_LCS_RRLP_NO_ASSISTANCE_NEEDED               	0x00                                                 	
#define MODEM_LCS_RRC_UNDEFINED_ACCURACY                  	0xFF                                              	
#define MODEM_LCS_UNKNOWN_UNCERTAINTY_VALUE               	0xFFFF                                            	
#define MODEM_LCS_NOT_FOR_A_SPECIFIC_MEASUREMENT          	0x00                                                 	
#define MODEM_LCS_GPS_INVALID_TIMING_ADVANCE              	0xFF                                              	
#define MODEM_LCS_UMTS_UNDEFINED_RTT                      	0xFFFF                                            	


//TABLES

//ConstantTable for MODEM_LCS_RRC_SFN_TOW_UNCERTAINTY
#define MODEM_LCS_RRC_LESS_THAN_10                        	0x00                                                 	
#define MODEM_LCS_RRC_GRTR_THAN_10                        	0x01                                                 	
#define MODEM_LCS_RRC_UNCERTAINTY_UNKNOWN                 	0x02                                                 	

//ConstantTable for MODEM_LCS_RRC_NAVIGATION_TYPES
#define MODEM_LCS_RRC_NEW_SAT_AND_NEW_MODEL               	0x00                                                 	
#define MODEM_LCS_RRC_OLD_SAT_AND_OLD_MODEL               	0x01                                                 	
#define MODEM_LCS_RRC_OLD_SAT_AND_NEW_MODEL               	0x02                                                 	
#define MODEM_LCS_RRC_REV2                                	0x03                                                 	
#define MODEM_LCS_RRC_REV                                 	0x04                                                 	

//ConstantTable for MODEM_LCS_RRC_ASTNC_SOURCE_TYPES

//ConstantTable for MODEM_LCS_RRLP_REF_BTS_RELATION
#define MODEM_LCS_RRLP_REF1_TO_12_AND_2_TO_3              	0x00                                                 	
#define MODEM_LCS_RRLP_REF1_TO_13_AND_2_TO_2              	0x01                                                 	
#define MODEM_LCS_RRLP_REF1_TO_1_AND_2_TO_23              	0x02                                                 	
#define MODEM_LCS_RRLP_DIRECT_RELATION                    	0xFF                                              	

//ConstantTable for MODEM_LCS_RRLP_METHOD_TYPE
#define MODEM_LCS_RRLP_ASSISTED_METHOD                    	0x00                                                 	
#define MODEM_LCS_RRLP_BASED_METHOD                       	0x01                                                 	
#define MODEM_LCS_RRLP_BASED_PREFERRED_METHOD             	0x02                                                 	
#define MODEM_LCS_RRLP_ASSISTED_PREFERRED_METHOD          	0x03                                                 	

//ConstantTable for MODEM_LCS_RRLP_POSITIONING_METHOD
#define MODEM_LCS_RRLP_EOTD_METHOD                        	0x00                                                 	
#define MODEM_LCS_RRLP_GPS_METHOD                         	0x01                                                 	
#define MODEM_LCS_RRLP_EOTD_OR_GPS_METHOD                 	0x02                                                 	

//ConstantTable for MODEM_LCS_RRLP_MULTIPLE_SETS
#define MODEM_LCS_RRLP_MULTIPLE_SETS                      	0x00                                                 	
#define MODEM_LCS_RRLP_NO_MULTIPLE_SETS                   	0x01                                                 	

//ConstantTable for MODEM_LCS_RRLP_ENV_CHARACTER
#define MODEM_LCS_RRLP_HEAVY_MP_AND_NLOS                  	0x00                                                 	
#define MODEM_LCS_RRLP_LIGHT_MP_AND_LOS                   	0x01                                                 	
#define MODEM_LCS_RRLP_MIXED_OR_UNDEFINED_ENV             	0x02                                                 	
#define MODEM_LCS_RRLP_UNDEFINED_ENV_FIELD                	0xFF                                              	

//ConstantTable for MODEM_LCS_RRLP_FIX_TYPE
#define MODEM_LCS_RRLP_FIX_TYPE_2D                        	0x00                                                 	
#define MODEM_LCS_RRLP_FIX_TYPE_3D                        	0x01                                                 	

//ConstantTable for MODEM_LCS_RRLP_LOCATION_ERROR
#define MODEM_LCS_RRLP_UNDEFINED_ERROR                    	0x00                                                 	
#define MODEM_LCS_RRLP_EOTD_NOT_ENOUGH_BTS                	0x01                                                 	
#define MODEM_LCS_RRLP_GPS_NOT_ENOUGH_SATS                	0x02                                                 	
#define MODEM_LCS_RRLP_EOTD_LC_ASTNC_MSNG                 	0x03                                                 	
#define MODEM_LCS_RRLP_EOTD_ASTNC_MSNG                    	0x04                                                 	
#define MODEM_LCS_RRLP_GPS_LOC_CALC_ASTNC_MSNG            	0x05                                                 	
#define MODEM_LCS_RRLP_GPS_ASTNC_MSNG                     	0x06                                                 	
#define MODEM_LCS_RRLP_UNSUPPORTED_METHOD                 	0x07                                                 	
#define MODEM_LCS_RRLP_NOT_PROCESSED                      	0x08                                                 	
#define MODEM_LCS_RRLP_GPS_REF_BTS_NOT_SERV_BTS           	0x09                                                 	
#define MODEM_LCS_RRLP_EOTD_REF_BTS_NOT_SERV_BTS          	0x0A                                                 	
#define MODEM_LCS_RRLP_MSMNTS_TERM_AO_RRPR                	0xFE                                              	
#define MODEM_LCS_RRLP_MSMNTS_TERM_AO_REQ                 	0xFF                                              	

//ConstantTable for MODEM_LCS_RRLP_GPS_MULTIPATH_INDICATOR
#define MODEM_LCS_RRLP_GPS_MP_NOT_MEASURED                	0x00                                                 	
#define MODEM_LCS_RRLP_GPS_MP_LOW                         	0x01                                                 	
#define MODEM_LCS_RRLP_GPS_MP_MEDIUM                      	0x02                                                 	
#define MODEM_LCS_RRLP_GPS_MP_HIGH                        	0x03                                                 	

//ConstantTable for MODEM_LCS_GPS_NAVIGATION_TYPES
#define MODEM_LCS_GPS_NEW_SAT_AND_NEW_MODEL               	0x00                                                 	
#define MODEM_LCS_GPS_OLD_SAT_AND_OLD_MODEL               	0x01                                                 	
#define MODEM_LCS_GPS_OLD_SAT_AND_NEW_MODEL               	0x02                                                 	

//ConstantTable for MODEM_LCS_RRC_ERROR_CODE
#define MODEM_LCS_RRC_ERROR_SUCCESS                       	0x00                                                 	
#define MODEM_LCS_RRC_ERROR_UNSPECIFIED                   	0x01                                                 	
#define MODEM_LCS_RRC_ERROR_MISSING_DATA                  	0x02                                                 	
#define MODEM_LCS_RRC_ERROR_UNSUPPORTED_MEASUREMENT       	0x03                                                 	
#define MODEM_LCS_RRC_ERROR_INVALID_CONFIGURATION         	0x04                                                 	
#define MODEM_LCS_RRC_ERROR_CONFIGURATION_INCOMPLETE      	0x05                                                 	

//ConstantTable for MODEM_LCS_RRC_MEASUREMENT_COMMAND
#define MODEM_LCS_RRC_MEAS_SETUP                          	0x00                                                 	
#define MODEM_LCS_RRC_MEAS_MODIFY                         	0x01                                                 	
#define MODEM_LCS_RRC_MEAS_RELEASE                        	0x02                                                 	

//ConstantTable for MODEM_LCS_RRC_POSITIONING_METHOD
#define MODEM_LCS_RRC_OTDOA_METHOD                        	0x00                                                 	
#define MODEM_LCS_RRC_GPS_METHOD                          	0x01                                                 	
#define MODEM_LCS_RRC_OTDOA_OR_GPS_METHOD                 	0x02                                                 	
#define MODEM_LCS_RRC_CELLID                              	0x03                                                 	

//ConstantTable for MODEM_LCS_RRC_ENV_CHARACTER
#define MODEM_LCS_RRC_HEAVY_MP_AND_NLOS                   	0x00                                                 	
#define MODEM_LCS_RRC_LIGHT_MP_AND_LOS                    	0x01                                                 	
#define MODEM_LCS_RRC_MIXED_OR_UNDEFINED_ENV              	0x02                                                 	
#define MODEM_LCS_RRC_UNDEFINED_ENV_FIELD                 	0xFF                                              	

//ConstantTable for MODEM_LCS_RRC_REPORT_CRITERIA
#define MODEM_LCS_RRC_REPORT_EVENT                        	0x00                                                 	
#define MODEM_LCS_RRC_REPORT_PERIODIC                     	0x01                                                 	
#define MODEM_LCS_RRC_REPORT_NO_REPORTING                 	0x02                                                 	

//ConstantTable for MODEM_LCS_RRC_REPORTING_QUANTITY
#define MODEM_LCS_RRC_QUANTITY_R_1                        	0x00                                                 	
#define MODEM_LCS_RRC_QUANTITY_R_2                        	0x01                                                 	
#define MODEM_LCS_RRC_QUANTITY_R_4                        	0x02                                                 	
#define MODEM_LCS_RRC_QUANTITY_R_8                        	0x03                                                 	
#define MODEM_LCS_RRC_QUANTITY_R_16                       	0x04                                                 	
#define MODEM_LCS_RRC_QUANTITY_R_32                       	0x05                                                 	
#define MODEM_LCS_RRC_QUANTITY_R_64                       	0x06                                                 	
#define MODEM_LCS_RRC_QUANTITY_R_INFINITY                 	0x07                                                 	

//ConstantTable for MODEM_LCS_RRC_REPORTING_INTERVAL
#define MODEM_LCS_RRC_INTERVAL_RIL_0                      	0x00                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_0_25                   	0x01                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_0_50                   	0x02                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_1                      	0x03                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_2                      	0x04                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_3                      	0x05                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_4                      	0x06                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_6                      	0x07                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_8                      	0x08                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_12                     	0x09                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_16                     	0x0A                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_20                     	0x0B                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_24                     	0x0C                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_28                     	0x0D                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_32                     	0x0E                                                 	
#define MODEM_LCS_RRC_INTERVAL_RIL_64                     	0x0F                                                 	

//ConstantTable for MODEM_LCS_RRC_MEAS_ERROR
#define MODEM_LCS_RRC_NOT_ENOUGH_OTDOA_CELLS              	0x00                                                 	
#define MODEM_LCS_RRC_NOT_ENOUGH_GPS_SATELLITES           	0x01                                                 	
#define MODEM_LCS_RRC_ASSISTANCE_DATA_MISSING             	0x02                                                 	
#define MODEM_LCS_RRC_METHOD_NOT_SUPPORTED                	0x03                                                 	
#define MODEM_LCS_RRC_UNDEFINED_ERROR                     	0x04                                                 	
#define MODEM_LCS_RRC_REQUEST_DENIED_BY_USER              	0x05                                                 	
#define MODEM_LCS_RRC_NOT_PROCESSED_AND_TIMEOUT           	0x06                                                 	
#define MODEM_LCS_RRC_REFERENCE_CELL_NOT_SERVING_CELL     	0x07                                                 	
#define MODEM_LCS_RRC_GPS_TIMING_OF_CELL_FRAMES_FAIL      	0x08                                                 	

//ConstantTable for MODEM_LCS_RRC_REF_TIME_TYPE
#define MODEM_LCS_RRC_UTRAN_GPS_REF_TIME                  	0x00                                                 	
#define MODEM_LCS_RRC_GPS_REF_TIME_ONLY                   	0x01                                                 	
#define MODEM_LCS_RRC_CELL_TIMING                         	0x02                                                 	

//ConstantTable for MODEM_LCS_RRC_MULTIPATH_INDICATOR
#define MODEM_LCS_RRC_MP_NOT_MEASURED                     	0x00                                                 	
#define MODEM_LCS_RRC_MP_LOW                              	0x01                                                 	
#define MODEM_LCS_RRC_MP_MEDIUM                           	0x02                                                 	
#define MODEM_LCS_RRC_MP_HIGH                             	0x03                                                 	

//ConstantTable for MODEM_LCS_RRC_POSITION_TYPE
#define MODEM_LCS_RRC_POINT                               	0x00                                                 	
#define MODEM_LCS_RRC_POINT_UNCER_CIRCLE                  	0x01                                                 	
#define MODEM_LCS_RRC_POINT_UNCERT_ELLIPSE                	0x02                                                 	
#define MODEM_LCS_RRC_POINT_ALTITUDE                      	0x03                                                 	
#define MODEM_LCS_RRC_POINT_ALTITUDE_UNCERT               	0x04                                                 	

//ConstantTable for MODEM_LCS_RRC_LATITUDE_SIGN
#define MODEM_LCS_RRC_NORTH                               	0x00                                                 	
#define MODEM_LCS_RRC_SOUTH                               	0x01                                                 	

//ConstantTable for MODEM_LCS_RRC_ALTITUDE_DIRECTION
#define MODEM_LCS_RRC_HEIGHT                              	0x00                                                 	
#define MODEM_LCS_RRC_DEPTH                               	0x01                                                 	

//ConstantTable for MODEM_LCS_RRC_MEASUREMENT_INTERVAL
#define MODEM_LCS_RRC_INTERVAL_E5                         	0x00                                                 	
#define MODEM_LCS_RRC_INTERVAL_E15                        	0x01                                                 	
#define MODEM_LCS_RRC_INTERVAL_E60                        	0x02                                                 	
#define MODEM_LCS_RRC_INTERVAL_E300                       	0x03                                                 	
#define MODEM_LCS_RRC_INTERVAL_E900                       	0x04                                                 	
#define MODEM_LCS_RRC_INTERVAL_E1800                      	0x05                                                 	
#define MODEM_LCS_RRC_INTERVAL_E3600                      	0x06                                                 	
#define MODEM_LCS_RRC_INTERVAL_E7200                      	0x07                                                 	

//ConstantTable for MODEM_LCS_RRC_7A_THRESHHOLD
#define MODEM_LCS_RRC_7A_PC10                             	0x00                                                 	
#define MODEM_LCS_RRC_7A_PC20                             	0x01                                                 	
#define MODEM_LCS_RRC_7A_PC30                             	0x02                                                 	
#define MODEM_LCS_RRC_7A_PC40                             	0x03                                                 	
#define MODEM_LCS_RRC_7A_PC50                             	0x04                                                 	
#define MODEM_LCS_RRC_7A_PC100                            	0x05                                                 	
#define MODEM_LCS_RRC_7A_PC200                            	0x06                                                 	
#define MODEM_LCS_RRC_7A_PC300                            	0x07                                                 	
#define MODEM_LCS_RRC_7A_PC500                            	0x08                                                 	
#define MODEM_LCS_RRC_7A_PC1000                           	0x09                                                 	
#define MODEM_LCS_RRC_7A_PC2000                           	0x0A                                                 	
#define MODEM_LCS_RRC_7A_PC5000                           	0x0B                                                 	
#define MODEM_LCS_RRC_7A_PC10000                          	0x0C                                                 	
#define MODEM_LCS_RRC_7A_PC20000                          	0x0D                                                 	
#define MODEM_LCS_RRC_7A_PC50000                          	0x0E                                                 	
#define MODEM_LCS_RRC_7A_PC100000                         	0x0F                                                 	

//ConstantTable for MODEM_LCS_RRC_7B_THRESHHOLD
#define MODEM_LCS_RRC_7B_C0_25                            	0x00                                                 	
#define MODEM_LCS_RRC_7B_C0_5                             	0x01                                                 	
#define MODEM_LCS_RRC_7B_C1                               	0x02                                                 	
#define MODEM_LCS_RRC_7B_C2                               	0x03                                                 	
#define MODEM_LCS_RRC_7B_C3                               	0x04                                                 	
#define MODEM_LCS_RRC_7B_C4                               	0x05                                                 	
#define MODEM_LCS_RRC_7B_C5                               	0x06                                                 	
#define MODEM_LCS_RRC_7B_C10                              	0x07                                                 	
#define MODEM_LCS_RRC_7B_C20                              	0x08                                                 	
#define MODEM_LCS_RRC_7B_C50                              	0x09                                                 	
#define MODEM_LCS_RRC_7B_C100                             	0x0A                                                 	
#define MODEM_LCS_RRC_7B_C200                             	0x0B                                                 	
#define MODEM_LCS_RRC_7B_C500                             	0x0C                                                 	
#define MODEM_LCS_RRC_7B_C1000                            	0x0D                                                 	
#define MODEM_LCS_RRC_7B_C2000                            	0x0E                                                 	
#define MODEM_LCS_RRC_7B_C5000                            	0x0F                                                 	

//ConstantTable for MODEM_LCS_RRC_7C_THRESHHOLD
#define MODEM_LCS_RRC_7C_MS1                              	0x00                                                 	
#define MODEM_LCS_RRC_7C_MS2                              	0x01                                                 	
#define MODEM_LCS_RRC_7C_MS3                              	0x02                                                 	
#define MODEM_LCS_RRC_7C_MS5                              	0x03                                                 	
#define MODEM_LCS_RRC_7C_MS10                             	0x04                                                 	
#define MODEM_LCS_RRC_7C_MS20                             	0x05                                                 	
#define MODEM_LCS_RRC_7C_MS50                             	0x06                                                 	
#define MODEM_LCS_RRC_7C_MS100                            	0x07                                                 	

//ConstantTable for MODEM_LCS_RRC_EVENT_TYPE
#define MODEM_LCS_RRC_EVENT_7A                            	0x00                                                 	
#define MODEM_LCS_RRC_EVENT_7B                            	0x01                                                 	
#define MODEM_LCS_RRC_EVENT_7C                            	0x02                                                 	

//ConstantTable for MODEM_LCS_RRC_METHOD_TYPE
#define MODEM_LCS_RRC_ASSISTED_METHOD                     	0x00                                                 	
#define MODEM_LCS_RRC_BASED_METHOD                        	0x01                                                 	
#define MODEM_LCS_RRC_BASED_PREFERRED_METHOD              	0x02                                                 	
#define MODEM_LCS_RRC_ASSISTED_PREFERRED_METHOD           	0x03                                                 	

//ConstantTable for MODEM_LCS_SS_VERTICAL_COORDINATE_REQUEST
#define MODEM_LCS_SS_VERT_COORD_REQ                       	0x00                                                 	
#define MODEM_LCS_SS_VERT_COORD_NOT_REQ                   	0x01                                                 	

//ConstantTable for MODEM_LCS_SS_RESPONSE_TIME
#define MODEM_LCS_SS_LOW_DELAY                            	0x00                                                 	
#define MODEM_LCS_SS_DELAY_TOLERATED                      	0x01                                                 	

//ConstantTable for MODEM_LCS_SS_NUMBER_TYPE
#define MODEM_LCS_SS_NBR_TYPE_UNKNOWN                     	0x00                                                 	
#define MODEM_LCS_SS_NBR_TYPE_INTERNATIONAL               	0x01                                                 	
#define MODEM_LCS_SS_NBR_TYPE_NATIONAL                    	0x02                                                 	
#define MODEM_LCS_SS_NBR_TYPE_NETWORK_SPECIFIC            	0x03                                                 	
#define MODEM_LCS_SS_NBR_TYPE_SUBSCRIBER                  	0x04                                                 	
#define MODEM_LCS_SS_NBR_TYPE_ALPHANUMERIC                	0x05                                                 	
#define MODEM_LCS_SS_NBR_TYPE_ABBREVIATED                 	0x06                                                 	

//ConstantTable for MODEM_LCS_SS_NUMBER_PLAN
#define MODEM_LCS_SS_NBR_PLAN_UNKNOWN                     	0x00                                                 	
#define MODEM_LCS_SS_NBR_PLAN_ISDN_TELEPHONY              	0x01                                                 	
#define MODEM_LCS_SS_NBR_PLAN_DATA                        	0x03                                                 	
#define MODEM_LCS_SS_NBR_PLAN_TELEX                       	0x04                                                 	
#define MODEM_LCS_SS_NBR_PLAN_LAND_MOBILE                 	0x06                                                 	
#define MODEM_LCS_SS_NBR_PLAN_NATIONAL                    	0x08                                                 	
#define MODEM_LCS_SS_NBR_PLAN_PRIVATE                     	0x09                                                 	

//ConstantTable for MODEM_LCS_SS_ESTIMATE_TYPES

//ConstantTable for MODEM_LCS_SS_METHOD
#define MODEM_LCS_SS_MS_BASED_EOTD                        	0x00                                                 	
#define MODEM_LCS_SS_MS_ASSISTED_EOTD                     	0x01                                                 	
#define MODEM_LCS_SS_ASSISTED_GPS                         	0x02                                                 	

//ConstantTable for MODEM_LCS_SS_PRIVACY_VERIFICATION
#define MODEM_LCS_SS_PERMISSION_DENIED                    	0x00                                                 	
#define MODEM_LCS_SS_PERMISSION_GRANTED                   	0x01                                                 	

//ConstantTable for MODEM_LCS_SS_REQ_RESP_TYPES
#define MODEM_LCS_SS_LOCATION_GET                         	0x00                                                 	
#define MODEM_LCS_SS_LOCATION_SEND                        	0x01                                                 	
#define MODEM_LCS_SS_ASSISTANCE_DATA                      	0x02                                                 	
#define MODEM_LCS_SS_DECIPHERING_KEYS                     	0x03                                                 	
#define MODEM_LCS_SS_PRIVACY_VER                          	0x04                                                 	
#define MODEM_LCS_SS_STOP                                 	0x05                                                 	
#define MODEM_LCS_SS_DISCONNECT                           	0x06                                                 	
#define MODEM_LCS_SS_PRIVACY_VER_TIMEOUT                  	0x07                                                 	
#define MODEM_LCS_SS_ERROR                                	0x09                                                 	

//ConstantTable for MODEM_LCS_SS_SERVICE_TYPE_ID
#define MODEM_LCS_SS_SERV_TYPE_EMERG_SERV                 	0x00                                                 	
#define MODEM_LCS_SS_SERV_TYPE_EMERG_ALERT_SERV           	0x01                                                 	
#define MODEM_LCS_SS_SERV_TYPE_PERSON_TRACKING            	0x02                                                 	
#define MODEM_LCS_SS_SERV_TYPE_FLEET_MGMT                 	0x03                                                 	
#define MODEM_LCS_SS_SERV_TYPE_ASSET_MGMT                 	0x04                                                 	
#define MODEM_LCS_SS_SERV_TYPE_TRAFF_CONG_REPORT          	0x05                                                 	
#define MODEM_LCS_SS_SERV_TYPE_ROADSIDE_ASSISTANCE        	0x06                                                 	
#define MODEM_LCS_SS_SERV_TYPE_ROUTING_TO_NEAR_COMM_ENT   	0x07                                                 	
#define MODEM_LCS_SS_SERV_TYPE_NAVIGATION                 	0x08                                                 	
#define MODEM_LCS_SS_SERV_TYPE_CITY_SIGHTSEEING           	0x09                                                 	
#define MODEM_LCS_SS_SERV_TYPE_LOCALIZED_ADVERTISING      	0x0A                                                 	
#define MODEM_LCS_SS_SERV_TYPE_MOBILE_YELLOW_PAGES        	0x0B                                                 	
#define MODEM_LCS_SS_SERV_TYPE_TRAF_AND_PUB_TRANSP_INFO   	0x0C                                                 	
#define MODEM_LCS_SS_SERV_TYPE_WEATHER                    	0x0D                                                 	
#define MODEM_LCS_SS_SERV_TYPE_ASSET_AND_SERV_FINDING     	0x0E                                                 	
#define MODEM_LCS_SS_SERV_TYPE_GAMING                     	0x0F                                                 	
#define MODEM_LCS_SS_SERV_TYPE_FIND_YOUR_FRIEND           	0x10                                              	
#define MODEM_LCS_SS_SERV_TYPE_DATING                     	0x11                                              	
#define MODEM_LCS_SS_SERV_TYPE_CHATTING                   	0x12                                              	
#define MODEM_LCS_SS_SERV_TYPE_ROUTE_FINDING              	0x13                                              	
#define MODEM_LCS_SS_SERV_TYPE_WHERE_AM_I                 	0x14                                              	

//ConstantTable for MODEM_LCS_SS_LOCATION_TYPE
#define MODEM_LCS_SS_LOCATION_TYPE_CURRENT                	0x00                                                 	
#define MODEM_LCS_SS_LOCATION_TYPE_CURRENTORLAST          	0x01                                                 	
#define MODEM_LCS_SS_LOCATION_TYPE_INITIAL                	0x02                                                 	

//ConstantTable for MODEM_LCS_SS_RESPONSE_STATUS
#define MODEM_LCS_SS_SERVICE_GRANTED                      	0x00                                                 	
#define MODEM_LCS_SS_SERVICE_DENIED                       	0x01                                                 	
#define MODEM_LCS_SS_INVALID_REQUEST                      	0x05                                                 	

//ConstantTable for MODEM_LCS_SS_ERROR_CODES
#define MODEM_LCS_SS_SS_SUBSCRIPTION_VIOLATION            	0x13                                              	
#define MODEM_LCS_SS_FACILITY_NOT_SUPPORTED               	0x15                                              	
#define MODEM_LCS_SS_SYSTEM_FAILURE                       	0x22                                              	
#define MODEM_LCS_SS_DATA_MISSING                         	0x23                                              	
#define MODEM_LCS_SS_UNEXPECTED_DATA_VALUE                	0x24                                              	
#define MODEM_LCS_SS_MODEM_LCS_POSITION_METHOD_FAILURE    	0x36                                              	

//ConstantTable for MODEM_LCS_SS_ERROR_ADDITIONAL_INDICATION
#define MODEM_LCS_SS_SHAPE_OF_LOCATION_ESTIMATE_NOT_SUPP  	0x01                                                 	
#define MODEM_LCS_SS_DEFAULT_VALUE                        	0x00                                                 	

//ConstantTable for MODEM_LCS_SS_MISC_SS_ERRORS
#define MODEM_LCS_SS_MM_RELEASE                           	0x01                                                 	
#define MODEM_LCS_SS_MM_ERROR                             	0x0B                                                 	
#define MODEM_LCS_SS_CS_INACTIVE                          	0x0F                                                 	
#define MODEM_LCS_SS_SS_NOT_AVAILABLE                     	0x12                                              	

//ConstantTable for MODEM_LCS_SS_PSEUDONYM_REQUESTED
#define MODEM_LCS_SS_PSEUDONYM_NOT_REQUESTED              	0x00                                                 	
#define MODEM_LCS_SS_PSEUDONYM_REQUESTED                  	0x01                                                 	

//ConstantTable for MODEM_LCS_LOCATION_TYPE
#define MODEM_LCS_LOCATION_TYPE_CURRENT                   	0x00                                                 	
#define MODEM_LCS_LOCATION_TYPE_CURRENTORLAST             	0x01                                                 	
#define MODEM_LCS_LOCATION_TYPE_INITIAL                   	0x02                                                 	

//ConstantTable for MODEM_LCS_NOTIFICATION_TYPE
#define MODEM_LCS_NOTIFICATION_INFORMATION                	0x00                                                 	
#define MODEM_LCS_NOTIFICATION_VERIFICATION               	0x01                                                 	

//ConstantTable for MODEM_LCS_NOTIFICATION_RESULT
#define MODEM_LCS_NOTIFICATION_REJECTED                   	0x00                                                 	
#define MODEM_LCS_NOTIFICATION_ACCEPTED                   	0x01                                                 	

//ConstantTable for MODEM_LCS_PRIV_INFO_TYPE
#define MODEM_LCS_PRIV_NOT_APPLICABLE                     	0x00                                                 	
#define MODEM_LCS_PRIV_PHONE_NUMBER                       	0x01                                                 	
#define MODEM_LCS_PRIV_LOGICAL_NAME                       	0x02                                                 	
#define MODEM_LCS_PRIV_EMAIL                              	0x03                                                 	
#define MODEM_LCS_PRIV_URL                                	0x04                                                 	
#define MODEM_LCS_PRIV_SIP_URL                            	0x05                                                 	
#define MODEM_LCS_PRIV_IMS_PUB_IDENT                      	0x06                                                 	

//ConstantTable for MODEM_LCS_HW_PIN_CTRL_ID

//ConstantTable for MODEM_LCS_HW_PIN_CTRL_STATE

//ConstantTable for MODEM_LCS_W_OPTIONAL_TAG
#define MODEM_LCS_W_OPTIONAL_NOT_PRESENT                  	0x00                                                 	
#define MODEM_LCS_W_OPTIONAL_PRESENT                      	0x01                                                 	

//ConstantTable for MODEM_LCS_W_CS_STATUS
#define MODEM_LCS_W_CS_SUCCESS                            	0x00                                                 	
#define MODEM_LCS_W_CS_FAILURE_ORIG_STATE_RESTORED        	0x01                                                 	
#define MODEM_LCS_MODEM_SPECIFIC_ERROR                    	0xFF                                              	

//ConstantTable for MODEM_LCS_RELEASE
#define MODEM_LCS_RELEASE_5                               	0x05                                                 	
#define MODEM_LCS_RELEASE_6                               	0x06                                                 	
#define MODEM_LCS_RELEASE_7                               	0x07                                                 	

//ConstantTable for MODEM_LCS_SUPPORT_STATUS_CODE
#define MODEM_LCS_SUPPORT_STARTED                         	0x00                                                 	
#define MODEM_LCS_SUPPORT_NOT_STARTED                     	0x01                                                 	
#define MODEM_LCS_SUPPORT_ALREADY_ACTIVATED               	0x02                                                 	
#define MODEM_LCS_SUPPORT_CANNOT_BE_ACTIVATED             	0x04                                                 	

//ConstantTable for MODEM_LCS_FRAME_TRIGGER_SUCCESS_CODES
#define MODEM_LCS_NET_CAUSE_OK                            	0x00                                                 	
#define MODEM_LCS_CAUSE_COMMUNICATION_ERROR               	0x01                                                 	
#define MODEM_LCS_CAUSE_SERVER_BUSY                       	0x08                                                 	
#define MODEM_LCS_CAUSE_REQUEST_INTERRUPTED               	0x12                                              	
//#define MODEM_LCS_MODEM_SPECIFIC_ERROR                  	0xFF                  	 //Already defined as 0xFF

//ConstantTable for MODEM_LCS_UE_POSITION_TECHNOLOGY
#define MODEM_LCS_UE_POS_TECHNOLOGY_AGPS                  	0x00                                                 	

//ConstantTable for MODEM_LCS_MS_POSITION_TECHNOLOGY
#define MODEM_LCS_MS_POS_TECHNOLOGY_AGPS                  	0x00                                                 	

//ConstantTable for MODEM_LCS_RRC_CURRENT_STATE_STATUS_TYPE
#define MODEM_LCS_RRC_STATE_STATUS_TYPE_CELL_PCH          	0x06                                                 	
#define MODEM_LCS_RRC_STATE_STATUS_TYPE_URA_PCH           	0x05                                                 	
#define MODEM_LCS_RRC_STATE_STATUS_TYPE_CELL_DCH          	0x04                                                 	
#define MODEM_LCS_RRC_STATE_STATUS_TYPE_CELL_FACH         	0x03                                                 	
#define MODEM_LCS_RRC_STATE_STATUS_TYPE_IDLE_FACH         	0x02                                                 	
#define MODEM_LCS_RRC_STATE_STATUS_TYPE_IDLE_PCH          	0x01                                                 	
#define MODEM_LCS_RRC_STATE_STATUS_TYPE_IDLE              	0x00                                                 	

//ConstantTable for MODEM_LCS_RRC_MEAS_SUSPEND_CONTROL
#define MODEM_LCS_RRC_STATE_MEAS_CONTINUE                 	0x00                                                 	
#define MODEM_LCS_RRC_STATE_MEAS_SUSPEND                  	0x01                                                 	

//BitmaskTables for MODEM_LCS_REQUIRED_DECODING
#define MODEM_LCS_ALL_ON                                  	0xFF                                              	
#define MODEM_LCS_ASN1_DECODING                           	0x01                                                 	
#define MODEM_LCS_ASN1_RAW_DATA                           	0x02                                                 	

//BitmaskTables for MODEM_LCS_POSITIONING_SUPPORT
#define MODEM_LCS_SUPPORT_STANDALONE_GPS                  	0x01                                                 	
#define MODEM_LCS_SUPPORT_MS_BASED_GPS                    	0x02                                                 	
#define MODEM_LCS_SUPPORT_MS_ASSISTED_GPS                 	0x04                                                 	
#define MODEM_LCS_SUPPORT_UE_GPS_TIMING_OF_CELL_FRAMES    	0x08                                                 	

//BitmaskTables for MODEM_LCS_POSITIONING_SUPPORT_RRLP
//#define MODEM_LCS_SUPPORT_STANDALONE_GPS                	0x00                   	 //Already defined as 0x01
//#define MODEM_LCS_SUPPORT_MS_BASED_GPS                  	0x00                   	 //Already defined as 0x02
//#define MODEM_LCS_SUPPORT_MS_ASSISTED_GPS               	0x00                   	 //Already defined as 0x04

//BitmaskTables for MODEM_LCS_POSITIONING_SUPPORT_RRC
//#define MODEM_LCS_SUPPORT_STANDALONE_GPS                	0x00                   	 //Already defined as 0x01
//#define MODEM_LCS_SUPPORT_MS_BASED_GPS                  	0x00                   	 //Already defined as 0x02
//#define MODEM_LCS_SUPPORT_MS_ASSISTED_GPS               	0x00                   	 //Already defined as 0x04
//#define MODEM_LCS_SUPPORT_UE_GPS_TIMING_OF_CELL_FRAMES  	0x00                   	 //Already defined as 0x08


//STRUCTURES


//Definition for MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ
#define MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ_OFFSET_SATID    	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ_OFFSET_TLMRESERVED	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ_OFFSET_TLM      	2 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ_OFFSET_ANTISPOOF	4 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ_OFFSET_ALERT    	5 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ_OFFSET_FILLERBYTE1	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRLP_GPS_TOW_ASSIST_SEQ            	8


//Definition for MODEM_LCS_RRLP_GPS_MEAS_SEQ
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_SVID           	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_SVCNO          	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_SVCODEPHWH     	2 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_SVCODEPHFR     	4 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_PSDOPPLER      	6 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_MULPATHIND     	8 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_PSRANGERMSER   	9 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_MEAS_SEQ_OFFSET_FILLERBYTE1    	10 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRLP_GPS_MEAS_SEQ                  	12


//Definition for MODEM_LCS_SAT_DATA_SEQ
#define MODEM_LCS_SAT_DATA_SEQ_OFFSET_SATELLITEID         	0 //size 1 byte(s)
#define MODEM_LCS_SAT_DATA_SEQ_OFFSET_IODE                	1 //size 1 byte(s)
#define MODEM_LCS_SAT_DATA_SEQ_OFFSET_FILLERBYTE1         	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_SAT_DATA_SEQ                       	4


//Definition for MODEM_LCS_PSEUDO_RANGE_DATA_SEQ
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_SATELLITEID	0 //size 1 byte(s)
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_SATELLITECNO	1 //size 1 byte(s)
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_WHOLECHIPS 	2 //size 2 byte(s)
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_FRACTIONALCHIPS	4 //size 2 byte(s)
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_DOPPLER    	6 //size 2 byte(s)
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_MULTIPATHINDICATOR	8 //size 1 byte(s)
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_PRRMSERROR 	9 //size 1 byte(s)
#define MODEM_LCS_PSEUDO_RANGE_DATA_SEQ_OFFSET_FILLERBYTE1	10 //size 2 byte(s)
#define SIZE_MODEM_LCS_PSEUDO_RANGE_DATA_SEQ              	12


//Definition for MODEM_LCS_W_OPTIONAL_PSC
#define MODEM_LCS_W_OPTIONAL_PSC_OFFSET_PRISRCD           	0 //size 2 byte(s)
#define SIZE_MODEM_LCS_W_OPTIONAL_PSC                     	2


//Definition for MODEM_LCS_W_PSC
#define MODEM_LCS_W_PSC_OFFSET_PRISRCD                    	0 //size 2 byte(s)
#define SIZE_MODEM_LCS_W_PSC                              	2


//Definition for MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ
#define MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ_OFFSET_SATID      	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ_OFFSET_IODE       	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ_OFFSET_UDRE       	2 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ_OFFSET_RRC        	3 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ_OFFSET_PRC        	4 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ_OFFSET_FILLERBYTE1	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRLP_GPS_SAT_LIST_SEQ              	8


//Definition for MODEM_LCS_RRLP_MEAS_INFO_SEQ
#define MODEM_LCS_RRLP_MEAS_INFO_SEQ_OFFSET_GPSTOW        	0 //size 4 byte(s)
#define MODEM_LCS_RRLP_MEAS_INFO_SEQ_OFFSET_FRAMENUMBER   	4 //size 2 byte(s)
#define MODEM_LCS_RRLP_MEAS_INFO_SEQ_OFFSET_FILLERBYTE1   	6 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEAS_INFO_SEQ_OFFSET_NUMPRM        	7 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEAS_INFO_SEQ_OFFSET_MEASURES      	8 //size 12 byte(s)
#define SIZE_MODEM_LCS_RRLP_MEAS_INFO_SEQ                 	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_RRLP_NAV_LIST_SEQ
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_SATID          	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_SATSTATUS      	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_CODEONL2       	2 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_URA            	3 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_SVHEALTH       	4 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_L2PDATAFLAG    	5 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_IODC           	6 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_SF1RESERVED1   	8 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_SF1RESERVED2   	12 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_SF1RESERVED3   	16 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_SF1RESERVED4   	20 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_TOC            	22 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_TGD            	24 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_AF2            	25 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_AF1            	26 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_AF0            	28 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_CRS            	32 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_DELTAN         	34 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_M0             	36 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_CUC            	40 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_CUS            	42 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_E              	44 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_APOWERHALF     	48 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_TOE            	52 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_FITFLAG        	54 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_AODA           	55 //size 1 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_CIC            	56 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_CIS            	58 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_OMEGA0         	60 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_I0             	64 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_CRC            	68 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_IDOT           	70 //size 2 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_OMEGA          	72 //size 4 byte(s)
#define MODEM_LCS_RRLP_NAV_LIST_SEQ_OFFSET_OMEGADOT       	76 //size 4 byte(s)
#define SIZE_MODEM_LCS_RRLP_NAV_LIST_SEQ                  	80


//Definition for MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_SVID    	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_ADDDOPPPRESENT	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_DOPPLER0	2 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_DOPPLER1	4 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_DOPPLERUNCERTAINTY	5 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_CODEPHASE	6 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_INTCODEPHASE	8 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_GPSBITNUMBER	9 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_CODEPHASESEARCHWINDOW	10 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_ADDANGLEPRESENT	12 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_AZIMUTH 	13 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_ELEVATION	14 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ_OFFSET_FILLERBYTE1	15 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRLP_GPS_ACQUIS_LIST_SEQ           	16


//Definition for MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_SATID  	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_TOA    	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_E      	2 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_DELTAI 	4 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_OMEGADOT	6 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_SVHEALTH	8 //size 1 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_FILLERBYTE1	9 //size 3 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_APOWERHALF	12 //size 4 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_OMEGA0 	16 //size 4 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_OMEGA  	20 //size 4 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_M0     	24 //size 4 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_AF0    	28 //size 2 byte(s)
#define MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ_OFFSET_AF1    	30 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRLP_GPS_ALMANAC_LIST_SEQ          	32


//Definition for MODEM_LCS_RRC_ACQUIS_LIST_SEQ
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_SATID        	0 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_ADDDOPPPRESENT	1 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_DOPPLER0     	2 //size 2 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_DOPPLER1     	4 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_FILLERBYTE1  	5 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_DOPPLERUNCERTAINTY	6 //size 2 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_CODEPHASE    	8 //size 2 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_INTCODEPHASE 	10 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_GPSBITNUMBER 	11 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_CODEPHASESEARCHWINDOW	12 //size 2 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_ADDANGLEPRESENT	14 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_AZIMUTH      	15 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_ELEVATION    	16 //size 1 byte(s)
#define MODEM_LCS_RRC_ACQUIS_LIST_SEQ_OFFSET_FILLERBYTE2  	17 //size 3 byte(s)
#define SIZE_MODEM_LCS_RRC_ACQUIS_LIST_SEQ                	20


//Definition for MODEM_LCS_RRC_ALMANAC_LIST_SEQ
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_DATAID      	0 //size 1 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_SATID       	1 //size 1 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_TOA         	2 //size 1 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_FILLERBYTE1 	3 //size 1 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_E           	4 //size 2 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_DELTAI      	6 //size 2 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_OMEGADOT    	8 //size 2 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_SVHEALTH    	10 //size 1 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_FILLERBYTE2 	11 //size 1 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_APOWERHALF  	12 //size 4 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_OMEGA0      	16 //size 4 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_OMEGA       	20 //size 4 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_M0          	24 //size 4 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_AF0         	28 //size 2 byte(s)
#define MODEM_LCS_RRC_ALMANAC_LIST_SEQ_OFFSET_AF1         	30 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRC_ALMANAC_LIST_SEQ               	32


//Definition for MODEM_LCS_RRC_NAV_LIST_SEQ
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_SATID           	0 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_SATSTATUS       	1 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_CODEONL2        	2 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_URA             	3 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_SVHEALTH        	4 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_L2PDATAFLAG     	5 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_IODC            	6 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_SF1RESERVED1    	8 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_SF1RESERVED2    	12 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_SF1RESERVED3    	16 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_SF1RESERVED4    	20 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_TOC             	22 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_TGD             	24 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_AF2             	25 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_AF1             	26 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_AF0             	28 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_CRS             	32 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_DELTAN          	34 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_M0              	36 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_CUC             	40 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_CUS             	42 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_E               	44 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_APOWERHALF      	48 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_TOE             	52 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_FITFLAG         	54 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_AODA            	55 //size 1 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_CIC             	56 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_CIS             	58 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_OMEGA0          	60 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_I0              	64 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_CRC             	68 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_IDOT            	70 //size 2 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_OMEGA           	72 //size 4 byte(s)
#define MODEM_LCS_RRC_NAV_LIST_SEQ_OFFSET_OMEGADOT        	76 //size 4 byte(s)
#define SIZE_MODEM_LCS_RRC_NAV_LIST_SEQ                   	80


//Definition for MODEM_LCS_RRC_SAT_LIST_SEQ
#define MODEM_LCS_RRC_SAT_LIST_SEQ_OFFSET_SATID           	0 //size 1 byte(s)
#define MODEM_LCS_RRC_SAT_LIST_SEQ_OFFSET_IODE            	1 //size 1 byte(s)
#define MODEM_LCS_RRC_SAT_LIST_SEQ_OFFSET_UDRE            	2 //size 1 byte(s)
#define MODEM_LCS_RRC_SAT_LIST_SEQ_OFFSET_RRC             	3 //size 1 byte(s)
#define MODEM_LCS_RRC_SAT_LIST_SEQ_OFFSET_PRC             	4 //size 2 byte(s)
#define MODEM_LCS_RRC_SAT_LIST_SEQ_OFFSET_FILLERBYTE1     	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRC_SAT_LIST_SEQ                   	8


//Definition for MODEM_LCS_RRC_TOW_ASSIST_SEQ
#define MODEM_LCS_RRC_TOW_ASSIST_SEQ_OFFSET_SATID         	0 //size 1 byte(s)
#define MODEM_LCS_RRC_TOW_ASSIST_SEQ_OFFSET_TLMRESERVED   	1 //size 1 byte(s)
#define MODEM_LCS_RRC_TOW_ASSIST_SEQ_OFFSET_TLM           	2 //size 2 byte(s)
#define MODEM_LCS_RRC_TOW_ASSIST_SEQ_OFFSET_ANTISPOOF     	4 //size 1 byte(s)
#define MODEM_LCS_RRC_TOW_ASSIST_SEQ_OFFSET_ALERT         	5 //size 1 byte(s)
#define MODEM_LCS_RRC_TOW_ASSIST_SEQ_OFFSET_FILLERBYTE1   	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRC_TOW_ASSIST_SEQ                 	8


//Definition for MODEM_LCS_RRC_EVENT_SEQ
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_EVENTQUANTITY      	0 //size 1 byte(s)
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_REPORTFIRST        	1 //size 1 byte(s)
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_MEASUREMENTINTERVAL	2 //size 1 byte(s)
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_VALUE7A            	3 //size 1 byte(s)
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_VALUE7B            	4 //size 1 byte(s)
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_VALUE7C            	5 //size 1 byte(s)
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_FILLERBYTE1        	6 //size 1 byte(s)
#define MODEM_LCS_RRC_EVENT_SEQ_OFFSET_EVENTTYPE          	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRC_EVENT_SEQ                      	8


//SUBBLOCKS


//Definition for MODEM_LCS_SB_RRLP_MULTIPLE_SETS
#define MODEM_LCS_SB_RRLP_MULTIPLE_SETS                   	0x400                                             	
#define MODEM_LCS_SB_RRLP_MULTIPLE_SETS_OFFSET_SBID       	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_MULTIPLE_SETS_OFFSET_SBLEN      	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_MULTIPLE_SETS_OFFSET_MEASUREMENTSETS	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MULTIPLE_SETS_OFFSET_REFERENCEBTSS	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MULTIPLE_SETS_OFFSET_BTSSSETSRELATION	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MULTIPLE_SETS_OFFSET_FILLERBYTE1	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_MULTIPLE_SETS              	8


//Definition for MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_REQ
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_REQ            	0x407                                             	
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_REQ_OFFSET_SBID	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_REQ_OFFSET_SBLEN	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_REQ       	4


//Definition for MODEM_LCS_SB_RRLP_REF_TIME_UNC
#define MODEM_LCS_SB_RRLP_REF_TIME_UNC                    	0x408                                             	
#define MODEM_LCS_SB_RRLP_REF_TIME_UNC_OFFSET_SBID        	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_UNC_OFFSET_SBLEN       	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_UNC_OFFSET_GPSREFTIMEUNC	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_UNC_OFFSET_FILLERBYTE1 	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_REF_TIME_UNC               	8


//Definition for MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS                	0x409                                             	
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_OFFSET_SBID    	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_OFFSET_SBLEN   	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_OFFSET_GPSTOWSUBMS	4 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_OFFSET_REFFRAMEMSB	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_OFFSET_DELTATOW	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_OFFSET_GPSREFTIMEUNC	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS_OFFSET_FILLERBYTE1	9 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_TIME_ASSIST_MEAS           	12


//Definition for MODEM_LCS_SB_RRLP_REQUIRED_RESPONSE_TIME
#define MODEM_LCS_SB_RRLP_REQUIRED_RESPONSE_TIME          	0x40A                                             	
#define MODEM_LCS_SB_RRLP_REQUIRED_RESPONSE_TIME_OFFSET_SBID	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REQUIRED_RESPONSE_TIME_OFFSET_SBLEN	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REQUIRED_RESPONSE_TIME_OFFSET_REQUIREDRESPONSETIME	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REQUIRED_RESPONSE_TIME_OFFSET_FILLERBYTE1	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_REQUIRED_RESPONSE_TIME     	8


//Definition for MODEM_LCS_SB_RRLP_EXT_REF_ID
#define MODEM_LCS_SB_RRLP_EXT_REF_ID                      	0x406                                             	
#define MODEM_LCS_SB_RRLP_EXT_REF_ID_OFFSET_SBID          	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_EXT_REF_ID_OFFSET_SBLEN         	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_EXT_REF_ID_OFFSET_EXTTRANID     	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_EXT_REF_ID_OFFSET_SMLCCODE      	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_EXT_REF_ID_OFFSET_FILLERBYTE1   	9 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_EXT_REF_ID                 	12


//Definition for MODEM_LCS_SB_RRLP_POSITIONING_INSTR
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR               	0x402                                             	
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_SBID   	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_SBLEN  	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_METHODTYPE	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_POSITIONINGMETHOD	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_RESPONSETIME	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_ACCURACY	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_MULTIPLESETS	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_ENVCHARACTER	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_POSITIONING_INSTR_OFFSET_FILLERBYTE1	10 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_POSITIONING_INSTR          	12


//Definition for MODEM_LCS_SB_RRC_LOCATION_INFO
#define MODEM_LCS_SB_RRC_LOCATION_INFO                    	0x501                                             	
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_SBID        	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_SBLEN       	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_REFTIMETYPE 	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_FILLERBYTE1 	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_CELLFRTIMEMS	6 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_CELLFRTIMELS	8 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_SFN         	12 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_SCRAMBLINGCODE	14 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_GPSREFTIMEONLY	16 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_ESTTYPE     	20 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_UNSEMIMAJOR 	21 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_UNSEMIMINOR 	22 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_LATITUDESIGN	23 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_LATITUDE    	24 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_LONGITUDE   	28 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_UNALTITUDE  	32 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_ALTITUDEDIR 	33 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_ALTITUDE    	34 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_UNCODE      	36 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_ORIENTATION 	37 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_CONFIDENCE  	38 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_LOCATION_INFO_OFFSET_FILLERBYTE2 	39 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_LOCATION_INFO               	40


//Definition for MODEM_LCS_SB_SS_CODEWORD
#define MODEM_LCS_SB_SS_CODEWORD                          	0x211                                             	
#define MODEM_LCS_SB_SS_CODEWORD_OFFSET_SBID              	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CODEWORD_OFFSET_SBLEN             	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CODEWORD_OFFSET_FILLERBYTE1       	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CODEWORD_OFFSET_STRINGLEN         	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CODEWORD_OFFSET_CODEWORDSTRING    	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_SS_CODEWORD                     	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for MODEM_LCS_SB_RRLP_MEAS_INFO
#define MODEM_LCS_SB_RRLP_MEAS_INFO                       	0x403                                             	
#define MODEM_LCS_SB_RRLP_MEAS_INFO_OFFSET_SBID           	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_INFO_OFFSET_SBLEN          	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_INFO_OFFSET_FILLERBYTE1    	4 //size 3 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_INFO_OFFSET_MEASUREMENTSETS	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_INFO_OFFSET_MEASSETS       	8 //size 20 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_MEAS_INFO                  	28
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_REF_TIME
#define MODEM_LCS_SB_RRLP_REF_TIME                        	0x40B                                             	
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_SBID            	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_SBLEN           	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_GPSTOW          	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_GPSWEEK         	8 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_GSMTIMEPRESENT  	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_BSIC            	11 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_BCCHCARRIER     	12 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_TN              	14 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_BN              	15 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_FNM             	16 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_FILLERBYTE1     	20 //size 3 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_NTA             	23 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REF_TIME_OFFSET_GPSTOWASSIST    	24 //size 8 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_REF_TIME                   	32
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_REF_LOCATION
#define MODEM_LCS_SB_RRLP_REF_LOCATION                    	0x40C                                             	
#define MODEM_LCS_SB_RRLP_REF_LOCATION_OFFSET_SBID        	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_LOCATION_OFFSET_SBLEN       	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_REF_LOCATION_OFFSET_BYTECOUNT   	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_REF_LOCATION_OFFSET_SHAPEBYTES  	5 //size 14 byte(s)
#define MODEM_LCS_SB_RRLP_REF_LOCATION_OFFSET_FILLERBYTE1 	19 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_REF_LOCATION               	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS                	0x40D                                             	
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS_OFFSET_SBID    	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS_OFFSET_SBLEN   	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS_OFFSET_GPSTOW  	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS_OFFSET_STATUS  	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS_OFFSET_FILLERBYTE1	9 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS_OFFSET_NSAT    	11 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS_OFFSET_SATLIST 	12 //size 8 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_DGPS_CORRECTIONS           	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_NAVIGATION
#define MODEM_LCS_SB_RRLP_NAVIGATION                      	0x40E                                             	
#define MODEM_LCS_SB_RRLP_NAVIGATION_OFFSET_SBID          	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_NAVIGATION_OFFSET_SBLEN         	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_NAVIGATION_OFFSET_FILLERBYTE1   	4 //size 3 byte(s)
#define MODEM_LCS_SB_RRLP_NAVIGATION_OFFSET_NSAT          	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_NAVIGATION_OFFSET_SATLIST       	8 //size 80 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_NAVIGATION                 	88
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_IONOSPHERIC
#define MODEM_LCS_SB_RRLP_IONOSPHERIC                     	0x40F                                             	
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_SBID         	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_SBLEN        	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_ALFA0        	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_ALFA1        	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_ALFA2        	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_ALFA3        	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_BETA0        	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_BETA1        	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_BETA2        	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_IONOSPHERIC_OFFSET_BETA3        	11 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_IONOSPHERIC                	12


//Definition for MODEM_LCS_SB_RRLP_UTC
#define MODEM_LCS_SB_RRLP_UTC                             	0x410                                             	
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_SBID                 	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_SBLEN                	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCA1                	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCA0                	8 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCTOT               	12 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCWNT               	13 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCDELTATLS          	14 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCWNLSF             	15 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCDN                	16 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_UTCDELTATLSF         	17 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_UTC_OFFSET_FILLERBYTE1          	18 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_UTC                        	20


//Definition for MODEM_LCS_SB_RRLP_ALMANAC
#define MODEM_LCS_SB_RRLP_ALMANAC                         	0x411                                             	
#define MODEM_LCS_SB_RRLP_ALMANAC_OFFSET_SBID             	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_ALMANAC_OFFSET_SBLEN            	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_ALMANAC_OFFSET_FILLERBYTE1      	4 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_ALMANAC_OFFSET_ALMANACWNA       	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ALMANAC_OFFSET_NSAT             	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ALMANAC_OFFSET_SATLIST          	8 //size 32 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_ALMANAC                    	40
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_ACQUISITION
#define MODEM_LCS_SB_RRLP_ACQUISITION                     	0x412                                             	
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_SBID         	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_SBLEN        	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_GPSTOW       	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_GSMTIMEPRESENT	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_BSIC         	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_BCCHCARRIER  	10 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_FNM          	12 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_TN           	16 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_BN           	17 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_FILLERBYTE1  	18 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_NSAT         	19 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_ACQUISITION_OFFSET_SATLIST      	20 //size 16 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_ACQUISITION                	36
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_BAD_SAT
#define MODEM_LCS_SB_RRLP_BAD_SAT                         	0x413                                             	
#define MODEM_LCS_SB_RRLP_BAD_SAT_OFFSET_SBID             	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_BAD_SAT_OFFSET_SBLEN            	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_BAD_SAT_OFFSET_NSAT             	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_BAD_SAT_OFFSET_LIST             	5 //size 16 byte(s)
#define MODEM_LCS_SB_RRLP_BAD_SAT_OFFSET_FILLERBYTE1      	21 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_BAD_SAT                    	24
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_ASN1
#define MODEM_LCS_SB_ASN1                                 	0x300                                             	
#define MODEM_LCS_SB_ASN1_OFFSET_SBID                     	0 //size 2 byte(s)
#define MODEM_LCS_SB_ASN1_OFFSET_SBLEN                    	2 //size 2 byte(s)
#define MODEM_LCS_SB_ASN1_OFFSET_FILLERBYTE1              	4 //size 2 byte(s)
#define MODEM_LCS_SB_ASN1_OFFSET_POSLEN                   	6 //size 2 byte(s)
#define MODEM_LCS_SB_ASN1_OFFSET_POS                      	8 //size 1 byte(s)
#define MODEM_LCS_SB_ASN1_OFFSET_FILLERBYTE2              	9 //size 1 byte(s)
#define MODEM_LCS_SB_ASN1_OFFSET_FILLERBYTE3              	10 //size 1 byte(s)
#define MODEM_LCS_SB_ASN1_OFFSET_FILLERBYTE4              	11 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_ASN1                            	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_SS_CLIENT_NAME
#define MODEM_LCS_SB_SS_CLIENT_NAME                       	0x20F                                             	
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_SBID           	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_SBLEN          	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_FILLERBYTE1    	4 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_PRIVINFOTYPE   	6 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_NAMELEN        	7 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_USSDSTRING     	8 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_FILLERBYTE2    	10 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_NAME_OFFSET_FILLERBYTE3    	11 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_SS_CLIENT_NAME                  	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for MODEM_LCS_SB_SS_CLIENT_EXT_ID
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID                     	0x20E                                             	
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_SBID         	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_SBLEN        	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_TYPEOFNUMBER 	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_FILLERBYTE1  	5 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_DATALEN      	7 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_CLIENTEXTERNALID	8 //size 2 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_FILLERBYTE2  	10 //size 1 byte(s)
#define MODEM_LCS_SB_SS_CLIENT_EXT_ID_OFFSET_FILLERBYTE3  	11 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_SS_CLIENT_EXT_ID                	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for MODEM_LCS_SB_SS_NOTIFICATION_INFO
#define MODEM_LCS_SB_SS_NOTIFICATION_INFO                 	0x210                                             	
#define MODEM_LCS_SB_SS_NOTIFICATION_INFO_OFFSET_SBID     	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_NOTIFICATION_INFO_OFFSET_SBLEN    	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_NOTIFICATION_INFO_OFFSET_LOCATIONTYPE	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_NOTIFICATION_INFO_OFFSET_NOTIFICATIONTYPE	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_NOTIFICATION_INFO_OFFSET_RESULT   	6 //size 1 byte(s)
#define MODEM_LCS_SB_SS_NOTIFICATION_INFO_OFFSET_FILLERBYTE1	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_SS_NOTIFICATION_INFO            	8


//Definition for MODEM_LCS_SB_SS_REQUESTOR_ID
#define MODEM_LCS_SB_SS_REQUESTOR_ID                      	0x20D                                             	
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_SBID          	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_SBLEN         	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_PRIVINFOTYPE  	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_FILLERBYTE1   	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_STRINGLEN     	6 //size 2 byte(s)
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_REQUESTORSTRING	8 //size 2 byte(s)
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_FILLERBYTE2   	10 //size 1 byte(s)
#define MODEM_LCS_SB_SS_REQUESTOR_ID_OFFSET_FILLERBYTE3   	11 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_SS_REQUESTOR_ID                 	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for MODEM_LCS_SB_RRC_REF_TIME
#define MODEM_LCS_SB_RRC_REF_TIME                         	0x100                                             	
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_SBID             	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_SBLEN            	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_GPSTOW           	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_GPSWEEK          	8 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_UTRANTIMEPRESENT 	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_FILLERBYTE1      	11 //size 3 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_CELLFRAMETIMEMS  	14 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_CELLFRAMETIMELS  	16 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_SFN              	20 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_PRIMARYCPICH     	22 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_SFNTOWUNCERTAINTY	24 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_UDRIFTRATEPRESENT	25 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_TUTRANGPSDRIFTRATE	26 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_NTA              	27 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_OFFSET_GPSTOWASSIST     	28 //size 8 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_REF_TIME                    	36
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_REF_LOCATION
#define MODEM_LCS_SB_RRC_REF_LOCATION                     	0x101                                             	
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_SBID         	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_SBLEN        	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_LATITUDESIGN 	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_ALTITUDESIGN 	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_ALTITUDE     	6 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_LATITUDE     	8 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_LONGITUDE    	12 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_UNCERTAINTYSEMIMAJOR	16 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_UNCERTAINTYSEMIMINOR	17 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_ORIENTATIONMAJORAXIS	18 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_UNCERTAINTYALTITUDE	19 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_CONFIDENCE   	20 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_LOCATION_OFFSET_FILLERBYTE1  	21 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_REF_LOCATION                	24


//Definition for MODEM_LCS_SB_RRC_DGPS_CORRECTIONS
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS                 	0x102                                             	
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS_OFFSET_SBID     	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS_OFFSET_SBLEN    	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS_OFFSET_GPSTOW   	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS_OFFSET_STATUS   	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS_OFFSET_FILLERBYTE1	9 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS_OFFSET_NSAT     	11 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_DGPS_CORRECTIONS_OFFSET_SATLIST  	12 //size 8 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_DGPS_CORRECTIONS            	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_NAVIGATION
#define MODEM_LCS_SB_RRC_NAVIGATION                       	0x103                                             	
#define MODEM_LCS_SB_RRC_NAVIGATION_OFFSET_SBID           	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_NAVIGATION_OFFSET_SBLEN          	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_NAVIGATION_OFFSET_FILLERBYTE1    	4 //size 3 byte(s)
#define MODEM_LCS_SB_RRC_NAVIGATION_OFFSET_NSAT           	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_NAVIGATION_OFFSET_SATLIST        	8 //size 80 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_NAVIGATION                  	88
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_IONOSPHERIC
#define MODEM_LCS_SB_RRC_IONOSPHERIC                      	0x104                                             	
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_SBID          	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_SBLEN         	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_ALFA0         	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_ALFA1         	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_ALFA2         	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_ALFA3         	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_BETA0         	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_BETA1         	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_BETA2         	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_IONOSPHERIC_OFFSET_BETA3         	11 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_IONOSPHERIC                 	12


//Definition for MODEM_LCS_SB_RRC_UTC
#define MODEM_LCS_SB_RRC_UTC                              	0x105                                             	
#define MODEM_LCS_SB_RRC_UTC_OFFSET_SBID                  	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_SBLEN                 	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCA1                 	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCA0                 	8 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCTOT                	12 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCWNT                	13 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCDELTATLS           	14 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCWNLSF              	15 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCDN                 	16 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_UTCDELTATLSF          	17 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_UTC_OFFSET_FILLERBYTE1           	18 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_UTC                         	20


//Definition for MODEM_LCS_SB_RRC_ALMANAC
#define MODEM_LCS_SB_RRC_ALMANAC                          	0x106                                             	
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_SBID              	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_SBLEN             	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_FILLERBYTE1       	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_SVGLOBALHEALTHPRESENT	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_ALMANACWNA        	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_NSAT              	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_SATLIST           	8 //size 32 byte(s)
#define MODEM_LCS_SB_RRC_ALMANAC_OFFSET_SVGLOBALHEALTH    	40 //size 48 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_ALMANAC                     	88
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_ACQUISITION
#define MODEM_LCS_SB_RRC_ACQUISITION                      	0x107                                             	
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_SBID          	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_SBLEN         	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_GPSTOW        	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_UTRANTIMEPRESENT	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_NSAT          	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_CELLFRAMETIMEMS	10 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_CELLFRAMETIMELS	12 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_SFN           	16 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_PRIMARYCPICH  	18 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_ACQUISITION_OFFSET_SATLIST       	20 //size 20 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_ACQUISITION                 	40
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_BAD_SAT
#define MODEM_LCS_SB_RRC_BAD_SAT                          	0x108                                             	
#define MODEM_LCS_SB_RRC_BAD_SAT_OFFSET_SBID              	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_BAD_SAT_OFFSET_SBLEN             	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_BAD_SAT_OFFSET_NSAT              	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_BAD_SAT_OFFSET_LIST              	5 //size 16 byte(s)
#define MODEM_LCS_SB_RRC_BAD_SAT_OFFSET_FILLERBYTE1       	21 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_BAD_SAT                     	24
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_LOCATION_INFO
#define MODEM_LCS_SB_RRLP_LOCATION_INFO                   	0x405                                             	
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_SBID       	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_SBLEN      	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_GPSTOW     	4 //size 4 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_FRAMENUMBER	8 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_FIXTYPE    	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_BYTECOUNT  	11 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_POSEST     	12 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_FILLERBYTE1	13 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_FILLERBYTE2	14 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_LOCATION_INFO_OFFSET_FILLERBYTE3	15 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_LOCATION_INFO              	16
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO                 	0x401                                             	
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_SBID     	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_SBLEN    	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_ERRORREASON	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_FILLERBYTE1	5 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_NASSISTANCE	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_DATA     	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_FILLERBYTE2	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_FILLERBYTE3	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO_OFFSET_FILLERBYTE4	11 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_MEAS_ERROR_INFO            	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_POSITIONING_INSTR
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR                	0x500                                             	
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_SBID    	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_SBLEN   	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_MEASUREMENTID	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_MEASUREMENTCOMMAND	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_METHODTYPE	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_POSITIONINGMETHOD	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_HACCURACY	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_VACCURACY	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_GPSSFNTIMING	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_ENVCHARACTER	11 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_ADDDATA 	12 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_REPORTCRITERIA	13 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_REPORTQUANTITY	14 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_REPORTINTERVAL	15 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_NUMEVENTS	16 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_FILLERBYTE1	17 //size 3 byte(s)
#define MODEM_LCS_SB_RRC_POSITIONING_INSTR_OFFSET_EVENTS  	20 //size 8 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_POSITIONING_INSTR           	28
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_STATE
#define MODEM_LCS_SB_RRC_STATE                            	0x109                                             	
#define MODEM_LCS_SB_RRC_STATE_OFFSET_SBID                	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_STATE_OFFSET_SBLEN               	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_STATE_OFFSET_CURRENTRRCSTATE     	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_STATE_OFFSET_MEASSUSPENDCTRL     	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_STATE_OFFSET_FILLERBYTE1         	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_STATE                       	8


//Definition for MODEM_LCS_SB_RRC_MEAS_ERROR_INFO
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO                  	0x502                                             	
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_SBID      	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_SBLEN     	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_ERROR     	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_ADREQ     	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_ALMANACREQ	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_UTCMODELREQ	7 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_IONSPHERICMODELREQ	8 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_NAVIGATIONMODELREQ	9 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_DGPSCORRECTIONSREQ	10 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_REFERENCELOCATIONREQ	11 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_REFTIMEREQ	12 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_AAREQ     	13 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_RTINTREGRITYREQ	14 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_NAVIGATIONMODELADREQ	15 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_GPSWEEK   	16 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_GPSTOE    	18 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_TTOELIMIT 	19 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_FILLERBYTE1	20 //size 3 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_NBRSATELLITES	23 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_MEAS_ERROR_INFO_OFFSET_POSEST    	24 //size 4 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_MEAS_ERROR_INFO             	28
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_GPS_MEAS_INFO
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO                    	0x503                                             	
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_SBID        	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_SBLEN       	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_REFTIMETYPE 	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_FILLERBYTE1 	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_CELLFRTIMEMS	6 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_CELLFRTIMELS	8 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_SFN         	12 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_SCRAMBLINGCODE	14 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_GPSREFTIMEONLY	16 //size 4 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_FILLERBYTE2 	20 //size 3 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_NBRSATELLITES	23 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_GPS_MEAS_INFO_OFFSET_POSEST      	24 //size 12 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_GPS_MEAS_INFO               	36
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_RRC_REF_TIME_UNC
#define MODEM_LCS_SB_RRC_REF_TIME_UNC                     	0x504                                             	
#define MODEM_LCS_SB_RRC_REF_TIME_UNC_OFFSET_SBID         	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_UNC_OFFSET_SBLEN        	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_UNC_OFFSET_GPSREFTIMEUNC	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_REF_TIME_UNC_OFFSET_FILLERBYTE1  	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_REF_TIME_UNC                	8


//Definition for MODEM_LCS_SB_SS_QUALITY_OF_SERVICE
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE                	0x202                                             	
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_SBID    	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_SBLEN   	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_SSPARAMETERINDICATOR	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_HORIZONTALACCURACY	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_VERTICALCOORDREQ	6 //size 1 byte(s)
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_VERTICALACCURACY	7 //size 1 byte(s)
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_RESPONSETIMECATEGORY	8 //size 1 byte(s)
#define MODEM_LCS_SB_SS_QUALITY_OF_SERVICE_OFFSET_FILLERBYTE1	9 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_QUALITY_OF_SERVICE           	12


//Definition for MODEM_LCS_SB_SS_MLC_NUMBER
#define MODEM_LCS_SB_SS_MLC_NUMBER                        	0x203                                             	
#define MODEM_LCS_SB_SS_MLC_NUMBER_OFFSET_SBID            	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_MLC_NUMBER_OFFSET_SBLEN           	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_MLC_NUMBER_OFFSET_TYPEOFNUMBER    	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_MLC_NUMBER_OFFSET_NUMBERLENGTH    	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_MLC_NUMBER_OFFSET_NUMBER          	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_SS_MLC_NUMBER                   	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA
#define MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA               	0x204                                             	
#define MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA_OFFSET_SBID   	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA_OFFSET_SBLEN  	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA_OFFSET_LENOFGPSASSISTDATA	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA_OFFSET_GPSASSISTANCEDATA	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA_OFFSET_FILLERBYTE1	6 //size 1 byte(s)
#define MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA_OFFSET_FILLERBYTE2	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_SS_GPS_ASSISTANCE_DATA          	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_SS_ESTIMATED_LOCATION
#define MODEM_LCS_SB_SS_ESTIMATED_LOCATION                	0x205                                             	
#define MODEM_LCS_SB_SS_ESTIMATED_LOCATION_OFFSET_SBID    	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_ESTIMATED_LOCATION_OFFSET_SBLEN   	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_ESTIMATED_LOCATION_OFFSET_LOCATIONESTIMATE	4 //size 20 byte(s)
#define SIZE_MODEM_LCS_SB_SS_ESTIMATED_LOCATION           	24
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_SS_DECIPHERING_KEYS
#define MODEM_LCS_SB_SS_DECIPHERING_KEYS                  	0x206                                             	
#define MODEM_LCS_SB_SS_DECIPHERING_KEYS_OFFSET_SBID      	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_DECIPHERING_KEYS_OFFSET_SBLEN     	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_DECIPHERING_KEYS_OFFSET_DECIPHERINGKEYFLAG	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_DECIPHERING_KEYS_OFFSET_CURRENTDECIPHERINGKEY	5 //size 7 byte(s)
#define MODEM_LCS_SB_SS_DECIPHERING_KEYS_OFFSET_NEXTDECIPHERINGKEY	12 //size 7 byte(s)
#define MODEM_LCS_SB_SS_DECIPHERING_KEYS_OFFSET_FILLERBYTE1	19 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_SS_DECIPHERING_KEYS             	20
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_SB_SS_LOCATION_METHOD
#define MODEM_LCS_SB_SS_LOCATION_METHOD                   	0x207                                             	
#define MODEM_LCS_SB_SS_LOCATION_METHOD_OFFSET_SBID       	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_LOCATION_METHOD_OFFSET_SBLEN      	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_LOCATION_METHOD_OFFSET_LOCATIONMETHOD	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_LOCATION_METHOD_OFFSET_FILLERBYTE1	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_LOCATION_METHOD              	8


//Definition for MODEM_LCS_SB_SS_PRIVACY_VER
#define MODEM_LCS_SB_SS_PRIVACY_VER                       	0x208                                             	
#define MODEM_LCS_SB_SS_PRIVACY_VER_OFFSET_SBID           	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_VER_OFFSET_SBLEN          	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_VER_OFFSET_PRIVACYVERIFICATION	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_VER_OFFSET_FILLERBYTE1    	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_PRIVACY_VER                  	8


//Definition for MODEM_LCS_SB_SS_PSEUDONYM_INDICATOR
#define MODEM_LCS_SB_SS_PSEUDONYM_INDICATOR               	0x209                                             	
#define MODEM_LCS_SB_SS_PSEUDONYM_INDICATOR_OFFSET_SBID   	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PSEUDONYM_INDICATOR_OFFSET_SBLEN  	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PSEUDONYM_INDICATOR_OFFSET_PSEUDONYMINDICATOR	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_PSEUDONYM_INDICATOR_OFFSET_FILLERBYTE1	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_PSEUDONYM_INDICATOR          	8


//Definition for MODEM_LCS_SB_SS_SERVICE_TYPE_ID
#define MODEM_LCS_SB_SS_SERVICE_TYPE_ID                   	0x20C                                             	
#define MODEM_LCS_SB_SS_SERVICE_TYPE_ID_OFFSET_SBID       	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_SERVICE_TYPE_ID_OFFSET_SBLEN      	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_SERVICE_TYPE_ID_OFFSET_SERVICETYPEID	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_SERVICE_TYPE_ID_OFFSET_FILLERBYTE1	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_SERVICE_TYPE_ID              	8


//Definition for MODEM_LCS_SB_SS_MAX_LOCATION_AGE
#define MODEM_LCS_SB_SS_MAX_LOCATION_AGE                  	0x20A                                             	
#define MODEM_LCS_SB_SS_MAX_LOCATION_AGE_OFFSET_SBID      	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_MAX_LOCATION_AGE_OFFSET_SBLEN     	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_MAX_LOCATION_AGE_OFFSET_AGEOFLOCATION	4 //size 2 byte(s)
#define MODEM_LCS_SB_SS_MAX_LOCATION_AGE_OFFSET_FILLERBYTE1	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_SS_MAX_LOCATION_AGE             	8


//Definition for MODEM_LCS_SB_SS_LOCATION_TYPE
#define MODEM_LCS_SB_SS_LOCATION_TYPE                     	0x20B                                             	
#define MODEM_LCS_SB_SS_LOCATION_TYPE_OFFSET_SBID         	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_LOCATION_TYPE_OFFSET_SBLEN        	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_LOCATION_TYPE_OFFSET_LOCATIONTYPE 	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_LOCATION_TYPE_OFFSET_FILLERBYTE1  	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_LOCATION_TYPE                	8


//Definition for MODEM_LCS_SB_SS_INDICATE_ERROR
#define MODEM_LCS_SB_SS_INDICATE_ERROR                    	0x201                                             	
#define MODEM_LCS_SB_SS_INDICATE_ERROR_OFFSET_SBID        	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_INDICATE_ERROR_OFFSET_SBLEN       	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_INDICATE_ERROR_OFFSET_ERRORCODE   	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_INDICATE_ERROR_OFFSET_ADDITIONALINDICATION	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_INDICATE_ERROR_OFFSET_FILLERBYTE1 	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_SS_INDICATE_ERROR               	8


//Definition for MODEM_LCS_SB_SS_OTHER_ERROR
#define MODEM_LCS_SB_SS_OTHER_ERROR                       	0x200                                             	
#define MODEM_LCS_SB_SS_OTHER_ERROR_OFFSET_SBID           	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_OTHER_ERROR_OFFSET_SBLEN          	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_OTHER_ERROR_OFFSET_ERRORCODE      	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_OTHER_ERROR_OFFSET_FILLERBYTE1    	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_OTHER_ERROR                  	8


//Definition for MODEM_LCS_SB_RRC_SUPPORT_REGISTER
#define MODEM_LCS_SB_RRC_SUPPORT_REGISTER                 	0x600                                             	
#define MODEM_LCS_SB_RRC_SUPPORT_REGISTER_OFFSET_SBID     	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_REGISTER_OFFSET_SBLEN    	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_REGISTER_OFFSET_SUPPORTOFFER	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_REGISTER_OFFSET_REQUIREDDECODING	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_REGISTER_OFFSET_CAPABILITIES	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_REGISTER_OFFSET_FILLERBYTE1	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_SUPPORT_REGISTER            	8


//Definition for MODEM_LCS_SB_RRLP_SUPPORT_REGISTER
#define MODEM_LCS_SB_RRLP_SUPPORT_REGISTER                	0x601                                             	
#define MODEM_LCS_SB_RRLP_SUPPORT_REGISTER_OFFSET_SBID    	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_REGISTER_OFFSET_SBLEN   	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_REGISTER_OFFSET_SUPPORTOFFER	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_REGISTER_OFFSET_REQUIREDDECODING	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_REGISTER_OFFSET_CAPABILITIES	6 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_REGISTER_OFFSET_FILLERBYTE1	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_SUPPORT_REGISTER           	8


//Definition for MODEM_LCS_SB_SS_PRIVACY_SUPPORT_REGISTER
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_REGISTER          	0x602                                             	
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_REGISTER_OFFSET_SBID	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_REGISTER_OFFSET_SBLEN	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_REGISTER_OFFSET_SUPPORTOFFER	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_REGISTER_OFFSET_FILLERBYTE1	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SB_SS_PRIVACY_SUPPORT_REGISTER     	8


//Definition for MODEM_LCS_SB_RRC_SUPPORT_STATUS
#define MODEM_LCS_SB_RRC_SUPPORT_STATUS                   	0x603                                             	
#define MODEM_LCS_SB_RRC_SUPPORT_STATUS_OFFSET_SBID       	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_STATUS_OFFSET_SBLEN      	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_STATUS_OFFSET_SUPPORTSTATUSCODE	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_STATUS_OFFSET_RELEASE    	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRC_SUPPORT_STATUS_OFFSET_FILLERBYTE1	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_RRC_SUPPORT_STATUS              	8


//Definition for MODEM_LCS_SB_RRLP_SUPPORT_STATUS
#define MODEM_LCS_SB_RRLP_SUPPORT_STATUS                  	0x604                                             	
#define MODEM_LCS_SB_RRLP_SUPPORT_STATUS_OFFSET_SBID      	0 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_STATUS_OFFSET_SBLEN     	2 //size 2 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_STATUS_OFFSET_SUPPORTSTATUSCODE	4 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_STATUS_OFFSET_RELEASE   	5 //size 1 byte(s)
#define MODEM_LCS_SB_RRLP_SUPPORT_STATUS_OFFSET_FILLERBYTE1	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_RRLP_SUPPORT_STATUS             	8


//Definition for MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS            	0x605                                             	
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS_OFFSET_SBID	0 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS_OFFSET_SBLEN	2 //size 2 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS_OFFSET_SUPPORTSTATUSCODE	4 //size 1 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS_OFFSET_RELEASE	5 //size 1 byte(s)
#define MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS_OFFSET_FILLERBYTE1	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_SB_SS_PRIVACY_SUPPORT_STATUS       	8


//MESSAGES


//Definition for MODEM_LCS_GPS_REF_CLOCK_INFO_REQ
#define MODEM_LCS_GPS_REF_CLOCK_INFO_REQ                  	0x00                                                 	
#define MODEM_LCS_GPS_REF_CLOCK_INFO_REQ_OFFSET_TRANSID   	0 //size 1 byte(s)
#define MODEM_LCS_GPS_REF_CLOCK_INFO_REQ_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define MODEM_LCS_GPS_REF_CLOCK_INFO_REQ_OFFSET_SUBSCRIPTION	2 //size 1 byte(s)
#define MODEM_LCS_GPS_REF_CLOCK_INFO_REQ_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_GPS_REF_CLOCK_INFO_REQ             	4


//Definition for MODEM_LCS_GPS_REF_CLOCK_INFO_RESP
#define MODEM_LCS_GPS_REF_CLOCK_INFO_RESP                 	0x01                                                 	
#define MODEM_LCS_GPS_REF_CLOCK_INFO_RESP_OFFSET_TRANSID  	0 //size 1 byte(s)
#define MODEM_LCS_GPS_REF_CLOCK_INFO_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_GPS_REF_CLOCK_INFO_RESP_OFFSET_UNCERTAINTY	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_GPS_REF_CLOCK_INFO_RESP            	4


//Definition for MODEM_LCS_GPS_REF_CLOCK_INFO_IND
#define MODEM_LCS_GPS_REF_CLOCK_INFO_IND                  	0x02                                                 	
#define MODEM_LCS_GPS_REF_CLOCK_INFO_IND_OFFSET_TRANSID   	0 //size 1 byte(s)
#define MODEM_LCS_GPS_REF_CLOCK_INFO_IND_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define MODEM_LCS_GPS_REF_CLOCK_INFO_IND_OFFSET_UNCERTAINTY	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_GPS_REF_CLOCK_INFO_IND             	4


//Definition for MODEM_LCS_SUPPORT_CONTROL_REQ
#define MODEM_LCS_SUPPORT_CONTROL_REQ                     	0x03                                                 	
#define MODEM_LCS_SUPPORT_CONTROL_REQ_OFFSET_TRANSID      	0 //size 1 byte(s)
#define MODEM_LCS_SUPPORT_CONTROL_REQ_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define MODEM_LCS_SUPPORT_CONTROL_REQ_OFFSET_FILLERBYTE1  	2 //size 1 byte(s)
#define MODEM_LCS_SUPPORT_CONTROL_REQ_OFFSET_SBCOUNT      	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_SUPPORT_CONTROL_REQ                	4


//Definition for MODEM_LCS_SUPPORT_CONTROL_RESP
#define MODEM_LCS_SUPPORT_CONTROL_RESP                    	0x04                                                 	
#define MODEM_LCS_SUPPORT_CONTROL_RESP_OFFSET_TRANSID     	0 //size 1 byte(s)
#define MODEM_LCS_SUPPORT_CONTROL_RESP_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define MODEM_LCS_SUPPORT_CONTROL_RESP_OFFSET_FILLERBYTE1 	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_SUPPORT_CONTROL_RESP               	4


//Definition for MODEM_LCS_RRLP_ASTNC_IND
#define MODEM_LCS_RRLP_ASTNC_IND                          	0x05                                                 	
#define MODEM_LCS_RRLP_ASTNC_IND_OFFSET_TRANSID           	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_ASTNC_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_ASTNC_IND_OFFSET_RRLPREFNBR        	2 //size 1 byte(s)
#define MODEM_LCS_RRLP_ASTNC_IND_OFFSET_MOREASTNCCOMING   	3 //size 1 byte(s)
#define MODEM_LCS_RRLP_ASTNC_IND_OFFSET_FILLERBYTE1       	4 //size 3 byte(s)
#define MODEM_LCS_RRLP_ASTNC_IND_OFFSET_SBCOUNT           	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRLP_ASTNC_IND                     	8


//Definition for MODEM_LCS_RRLP_MEASURE_POS_REQUEST_IND
#define MODEM_LCS_RRLP_MEASURE_POS_REQUEST_IND            	0x06                                                 	
#define MODEM_LCS_RRLP_MEASURE_POS_REQUEST_IND_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_REQUEST_IND_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_REQUEST_IND_OFFSET_RRLPREFNBR	2 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_REQUEST_IND_OFFSET_SBCOUNT	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRLP_MEASURE_POS_REQUEST_IND       	4


//Definition for MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_REQ
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_REQ           	0x07                                                 	
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_REQ_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_REQ_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_REQ_OFFSET_RRLPREFNBR	2 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_REQ_OFFSET_SBCOUNT	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_REQ      	4


//Definition for MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_RESP
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_RESP          	0x08                                                 	
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_RESP_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_RESP_OFFSET_FILLERBYTE1	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRLP_MEASURE_POS_RESPONSE_RESP     	4


//Definition for MODEM_LCS_RESET_MS_POS_STORED_INFO_IND
#define MODEM_LCS_RESET_MS_POS_STORED_INFO_IND            	0x09                                                 	
#define MODEM_LCS_RESET_MS_POS_STORED_INFO_IND_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RESET_MS_POS_STORED_INFO_IND_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RESET_MS_POS_STORED_INFO_IND_OFFSET_POSTECHNOLOGY	2 //size 1 byte(s)
#define MODEM_LCS_RESET_MS_POS_STORED_INFO_IND_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_RESET_MS_POS_STORED_INFO_IND       	4


//Definition for MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND         	0x0B                                                 	
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND_OFFSET_MEASUREMENTID	3 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND_OFFSET_FILLERBYTE2	4 //size 3 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND_OFFSET_SBCOUNT	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_IND    	8


//Definition for MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_REQ
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_REQ         	0x0C                                                 	
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_REQ_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_REQ_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_REQ_OFFSET_MEASUREMENTID	2 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_REQ_OFFSET_ERROR	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_REQ    	4


//Definition for MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_RESP
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_RESP        	0x0D                                                 	
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_RESP_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_RESP_OFFSET_FILLERBYTE1	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRC_POS_MEASUREMENT_REQUEST_RESP   	4


//Definition for MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ             	0x0E                                                 	
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ_OFFSET_FINALMEASUREMENT	2 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ_OFFSET_MEASUREMENTID	3 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ_OFFSET_FILLERBYTE1	4 //size 3 byte(s)
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ_OFFSET_SBCOUNT	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_RRC_POS_MEASURED_RESULT_REQ        	8


//Definition for MODEM_LCS_RRC_POS_MEASURED_RESULT_RESP
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_RESP            	0x0F                                                 	
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_RESP_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RRC_POS_MEASURED_RESULT_RESP_OFFSET_FILLERBYTE1	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_RRC_POS_MEASURED_RESULT_RESP       	4


//Definition for MODEM_LCS_RESET_UE_POS_STORED_INFO_IND
#define MODEM_LCS_RESET_UE_POS_STORED_INFO_IND            	0x10                                              	
#define MODEM_LCS_RESET_UE_POS_STORED_INFO_IND_OFFSET_TRANSID	0 //size 1 byte(s)
#define MODEM_LCS_RESET_UE_POS_STORED_INFO_IND_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define MODEM_LCS_RESET_UE_POS_STORED_INFO_IND_OFFSET_POSTECHNOLOGY	2 //size 1 byte(s)
#define MODEM_LCS_RESET_UE_POS_STORED_INFO_IND_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_RESET_UE_POS_STORED_INFO_IND       	4


//Definition for MODEM_LCS_SS_REQ
#define MODEM_LCS_SS_REQ                                  	0x11                                              	
#define MODEM_LCS_SS_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define MODEM_LCS_SS_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define MODEM_LCS_SS_REQ_OFFSET_REQUESTTYPE               	2 //size 1 byte(s)
#define MODEM_LCS_SS_REQ_OFFSET_SBCOUNT                   	3 //size 1 byte(s)
#define MODEM_LCS_SS_REQ_OFFSET_SESSION                   	4 //size 1 byte(s)
#define MODEM_LCS_SS_REQ_OFFSET_FILLERBYTE1               	5 //size 3 byte(s)
#define SIZE_MODEM_LCS_SS_REQ                             	8


//Definition for MODEM_LCS_SS_RESP
#define MODEM_LCS_SS_RESP                                 	0x12                                              	
#define MODEM_LCS_SS_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define MODEM_LCS_SS_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define MODEM_LCS_SS_RESP_OFFSET_VALIDSESSIONID           	2 //size 1 byte(s)
#define MODEM_LCS_SS_RESP_OFFSET_SESSION                  	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_SS_RESP                            	4


//Definition for MODEM_LCS_SS_IND
#define MODEM_LCS_SS_IND                                  	0x21                                              	
#define MODEM_LCS_SS_IND_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define MODEM_LCS_SS_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define MODEM_LCS_SS_IND_OFFSET_SESSION                   	2 //size 1 byte(s)
#define MODEM_LCS_SS_IND_OFFSET_RESPONSESTATUS            	3 //size 1 byte(s)
#define MODEM_LCS_SS_IND_OFFSET_RESPONSETYPE              	4 //size 1 byte(s)
#define MODEM_LCS_SS_IND_OFFSET_FILLERBYTE1               	5 //size 2 byte(s)
#define MODEM_LCS_SS_IND_OFFSET_SBCOUNT                   	7 //size 1 byte(s)
#define SIZE_MODEM_LCS_SS_IND                             	8


//Definition for MODEM_LCS_SS_PRIVACY_IND
#define MODEM_LCS_SS_PRIVACY_IND                          	0x13                                              	
#define MODEM_LCS_SS_PRIVACY_IND_OFFSET_TRANSID           	0 //size 1 byte(s)
#define MODEM_LCS_SS_PRIVACY_IND_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define MODEM_LCS_SS_PRIVACY_IND_OFFSET_SESSION           	2 //size 1 byte(s)
#define MODEM_LCS_SS_PRIVACY_IND_OFFSET_SBCOUNT           	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_SS_PRIVACY_IND                     	4


//Definition for MODEM_LCS_SS_PRIVACY_TIMEOUT_IND
#define MODEM_LCS_SS_PRIVACY_TIMEOUT_IND                  	0x14                                              	
#define MODEM_LCS_SS_PRIVACY_TIMEOUT_IND_OFFSET_TRANSID   	0 //size 1 byte(s)
#define MODEM_LCS_SS_PRIVACY_TIMEOUT_IND_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define MODEM_LCS_SS_PRIVACY_TIMEOUT_IND_OFFSET_SESSION   	2 //size 1 byte(s)
#define MODEM_LCS_SS_PRIVACY_TIMEOUT_IND_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_MODEM_LCS_SS_PRIVACY_TIMEOUT_IND             	4


//Definition for MODEM_LCS_GSM_TIME_STAMP_REQ
#define MODEM_LCS_GSM_TIME_STAMP_REQ                      	0x17                                              	
#define MODEM_LCS_GSM_TIME_STAMP_REQ_OFFSET_TRANSID       	0 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_REQ_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_REQ_OFFSET_FILLERBYTE1   	2 //size 2 byte(s)
#define SIZE_MODEM_LCS_GSM_TIME_STAMP_REQ                 	4


//Definition for MODEM_LCS_GSM_TIME_STAMP_RESP
#define MODEM_LCS_GSM_TIME_STAMP_RESP                     	0x18                                              	
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_TRANSID      	0 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_BITNO        	2 //size 2 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_FRAMENUMBER  	4 //size 4 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_TIMESLOT     	8 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_TA           	9 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_BCCHCARRIER  	10 //size 2 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_BSIC         	12 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_SUCCESSCODE  	13 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_SUBERROR     	14 //size 1 byte(s)
#define MODEM_LCS_GSM_TIME_STAMP_RESP_OFFSET_FILLERBYTE1  	15 //size 1 byte(s)
#define SIZE_MODEM_LCS_GSM_TIME_STAMP_RESP                	16


//Definition for MODEM_LCS_WCDMA_TIME_STAMP_REQ
#define MODEM_LCS_WCDMA_TIME_STAMP_REQ                    	0x19                                              	
#define MODEM_LCS_WCDMA_TIME_STAMP_REQ_OFFSET_TRANSID     	0 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_REQ_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_REQ_OFFSET_FILLERBYTE1 	2 //size 2 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_REQ_OFFSET_BSIDFORATT  	4 //size 2 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_REQ_OFFSET_FILLERBYTE2 	6 //size 2 byte(s)
#define SIZE_MODEM_LCS_WCDMA_TIME_STAMP_REQ               	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for MODEM_LCS_WCDMA_TIME_STAMP_RESP
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP                   	0x20                                              	
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_TRANSID    	0 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_FILLERBYTE1	2 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_CELLDCH    	3 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_STATUS     	4 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_SUBERROR   	5 //size 1 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_SFN        	6 //size 2 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_SCRCFORATT 	8 //size 2 byte(s)
#define MODEM_LCS_WCDMA_TIME_STAMP_RESP_OFFSET_RTT        	10 //size 2 byte(s)
#define SIZE_MODEM_LCS_WCDMA_TIME_STAMP_RESP              	12
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.

#endif