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
* Name      : PN_GSS [ 0x32 ] Resource GSM Stack Server
* Version   : 008.002
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_GSSISI_H_
#define _SYMBIAN_ISI_GSSISI_H_

#define GSM_STACK_SERVER_ISI_VERSION_Z	 8
#define GSM_STACK_SERVER_ISI_VERSION_Y	 2


//CONSTANTS
#define GSS_FILL                                          	0x00                                                 	
#define GSS_FTD_FIRST_SUB_BLOCK                           	0x3200                                            	
#define GSS_FTD_FIRST_WORD_SUB_BLOCK                      	0x3400                                            	
#define GSS_FTD_FIRST_DWORD_SUB_BLOCK                     	0x3600                                            	
#define GSS_FTD_FIRST_STRING_SUB_BLOCK                    	0x3800                                            	
#define GSS_GENERAL_ISI_DATA_HDR_LEN                      	0x02                                                 	
#define GSS_DATA_IND_LENGTH                               	0x04                                                 	
#define GSS_SUBBLOCK_HEADER_LENGTH                        	0x04                                                 	
#define GSS_CS_MSG_HDR_LENGTH                             	0x04                                                 	
#define GSS_IND_DATA_MAX_SIZE                             	0x258                                             	
#define GSS_TIMING_ADVANCE_NOT_AVAILABLE                  	0xFF                                              	
#define GSS_SGW_IP_ADDRESS_LENGTH                         	0x10                                              	
#define GSS_UNC_IP_ADDRESS_LENGTH                         	0x10                                              	
#define GSS_SRV_NO_FQDN                                   	0x00                                                 	
#define GSS_ANY_SIZE                                      	0x01                                                 	


//TABLES

//ConstantTable for FtdTable
#define FTD_SB_RX_LEVEL                                   	0x3200                                            	
#define FTD_SB_TIME_SLOT                                  	0x3201                                            	
#define FTD_SB_TIMING_ADV                                 	0x3202                                            	
#define FTD_SB_RX_QUALITY                                 	0x3203                                            	
#define FTD_SB_RADIO_LINK_TIMEOUT                         	0x3204                                            	
#define FTD_SB_MAX_RAND_ACC_TRA                           	0x3205                                            	
#define FTD_SB_BSIC                                       	0x3206                                            	
#define FTD_SB_LAST_CALL_REL_REAS                         	0x3207                                            	
#define FTD_SB_CELL_RESEL_OFFSET                          	0x3208                                            	
#define FTD_SB_TEMP_OFFSET                                	0x3209                                            	
#define FTD_SB_HOPP_CH                                    	0x320A                                            	
#define FTD_SB_MOB_ALLOC_INDEX                            	0x320B                                            	
#define FTD_SB_HSN                                        	0x320C                                            	
#define FTD_SB_RX_LEVEL_1_NEIGH                           	0x320D                                            	
#define FTD_SB_RX_LEVEL_2_NEIGH                           	0x320E                                            	
#define FTD_SB_RX_LEVEL_3_NEIGH                           	0x320F                                            	
#define FTD_SB_RX_LEVEL_4_NEIGH                           	0x3210                                            	
#define FTD_SB_RX_LEVEL_5_NEIGH                           	0x3211                                            	
#define FTD_SB_RX_LEVEL_6_NEIGH                           	0x3212                                            	
#define FTD_SB_RX_LEVEL_7_NEIGH                           	0x3213                                            	
#define FTD_SB_RX_LEVEL_8_NEIGH                           	0x3214                                            	
#define FTD_SB_T3212                                      	0x3215                                            	
#define FTD_SB_T3212_TIMEOUT                              	0x3216                                            	
#define FTD_SB_PRP                                        	0x3217                                            	
#define FTD_SB_LAST_NORMAL_LOC_UP_REASON                  	0x3218                                            	
#define FTD_SB_LAST_IMSI_ATT_FAIL_REASON                  	0x3219                                            	
#define FTD_SB_RESET_HANDOVERS                            	0x321A                                            	
#define FTD_SB_RESET_TEST_COUNTERS                        	0x321B                                            	
#define FTD_SB_LAST_MO_SMS_FAIL                           	0x321C                                            	
#define FTD_SB_LAST_MT_SMS_FAIL                           	0x321D                                            	
#define FTD_SB_DTX_STATUS_CHANGE                          	0x321E                                            	
#define FTD_SB_CELL_BARR_IND_STATUS_CHANGE                	0x321F                                            	
#define FTD_SB_RESET_TEST_TIMERS                          	0x3220                                            	
#define FTD_SB_MAIN_CH_PLACE                              	0x3221                                            	
#define FTD_SB_MAIN_CH_PWR_CONTROL                        	0x3222                                            	
#define FTD_SB_PWR_CONTROL_TS_0                           	0x3223                                            	
#define FTD_SB_PWR_CONTROL_TS_1                           	0x3224                                            	
#define FTD_SB_PWR_CONTROL_TS_2                           	0x3225                                            	
#define FTD_SB_PWR_CONTROL_TS_3                           	0x3226                                            	
#define FTD_SB_PWR_CONTROL_TS_4                           	0x3227                                            	
#define FTD_SB_PWR_CONTROL_TS_5                           	0x3228                                            	
#define FTD_SB_PWR_CONTROL_TS_6                           	0x3229                                            	
#define FTD_SB_PWR_CONTROL_TS_7                           	0x322A                                            	
#define FTD_SB_PH_TAI                                     	0x322B                                            	
#define FTD_SB_SAME_RA_AS_SRV_CELL_1_NEIGH                	0x322C                                            	
#define FTD_SB_SAME_RA_AS_SRV_CELL_2_NEIGH                	0x322D                                            	
#define FTD_SB_SAME_RA_AS_SRV_CELL_3_NEIGH                	0x322E                                            	
#define FTD_SB_SAME_RA_AS_SRV_CELL_4_NEIGH                	0x322F                                            	
#define FTD_SB_SAME_RA_AS_SRV_CELL_5_NEIGH                	0x3230                                            	
#define FTD_SB_SAME_RA_AS_SRV_CELL_6_NEIGH                	0x3231                                            	
#define FTD_SB_CELL_BAR_ACC_2_1_NEIGH                     	0x3232                                            	
#define FTD_SB_CELL_BAR_ACC_2_2_NEIGH                     	0x3233                                            	
#define FTD_SB_CELL_BAR_ACC_2_3_NEIGH                     	0x3234                                            	
#define FTD_SB_CELL_BAR_ACC_2_4_NEIGH                     	0x3235                                            	
#define FTD_SB_CELL_BAR_ACC_2_5_NEIGH                     	0x3236                                            	
#define FTD_SB_CELL_BAR_ACC_2_6_NEIGH                     	0x3237                                            	
#define FTD_SB_EXC_ACC_1_NEIGH                            	0x3238                                            	
#define FTD_SB_EXC_ACC_2_NEIGH                            	0x3239                                            	
#define FTD_SB_EXC_ACC_3_NEIGH                            	0x323A                                            	
#define FTD_SB_EXC_ACC_4_NEIGH                            	0x323B                                            	
#define FTD_SB_EXC_ACC_5_NEIGH                            	0x323C                                            	
#define FTD_SB_EXC_ACC_6_NEIGH                            	0x323D                                            	
#define FTD_SB_CELL_BAR_ACC_2                             	0x323E                                            	
#define FTD_SB_EXC_ACC                                    	0x323F                                            	
#define FTD_SB_GPRS_TIMING_ADV                            	0x3240                                            	
#define FTD_SB_GPRS_RXLEV_ACC_MIN                         	0x3241                                            	
#define FTD_SB_GPRS_CELL_RESEL_HYST                       	0x3242                                            	
#define FTD_SB_RA_RESEL_HYST                              	0x3243                                            	
#define FTD_SB_C31_HYST                                   	0x3244                                            	
#define FTD_SB_C32_QUAL                                   	0x3245                                            	
#define FTD_SB_RAND_ACC_RETRY                             	0x3246                                            	
#define FTD_SB_GPRS_RX_LEVEL_SERV                         	0x3247                                            	
#define FTD_SB_GPRS_RX_LEVEL_1_NEIGH                      	0x3248                                            	
#define FTD_SB_GPRS_RX_LEVEL_2_NEIGH                      	0x3249                                            	
#define FTD_SB_GPRS_RX_LEVEL_3_NEIGH                      	0x324A                                            	
#define FTD_SB_GPRS_RX_LEVEL_4_NEIGH                      	0x324B                                            	
#define FTD_SB_GPRS_RX_LEVEL_5_NEIGH                      	0x324C                                            	
#define FTD_SB_GPRS_RX_LEVEL_6_NEIGH                      	0x324D                                            	
#define FTD_SB_EGPRS_SUPPORT                              	0x324E                                            	
#define FTD_SB_EGPRS_PACKET_CHANNEL_REQUEST               	0x324F                                            	
#define FTD_SB_EGPRS_BEP_PER                              	0x3250                                            	
#define FTD_SB_PH_MSCR                                    	0x3251                                            	
#define FTD_SB_PH_SGSNR                                   	0x3252                                            	
#define FTD_SB_SMS_SEND_FAIL_CAUSE_GPRS                   	0x3253                                            	
#define FTD_SB_SMS_REC_FAIL_CAUSE_GPRS                    	0x3254                                            	
#define FTD_SB_GPRS_SUPPORT                               	0x3255                                            	
#define FTD_SB_PH_SI_STATUS_IND                           	0x3258                                            	
#define FTD_SB_PH_EXT_UTBF                                	0x3259                                            	
#define FTD_SB_PH_CCN_ACTIVE                              	0x325A                                            	
#define FTD_SB_PH_PFC_FEATURE_MODE                        	0x325B                                            	
#define FTD_SB_CURR_BAND                                  	0x325C                                            	
#define FTD_SB_LAST_USED_BAND_READ                        	0x325D                                            	
#define FTD_SB_SELECTED_RAT_READ                          	0x325E                                            	
#define FTD_SB_GPRS_RX_LEVEL                              	0x325F                                            	
#define FTD_SB_TOGGLE_TRANS_STATUS_CHANGE                 	0x3260                                            	
#define FTD_SB_RX_LEVEL_FULL                              	0x3261                                            	
#define FTD_SB_RX_LEVEL_SUB                               	0x3262                                            	
#define FTD_SB_RX_QUALITY_FULL                            	0x3263                                            	
#define FTD_SB_RX_QUALITY_SUB                             	0x3264                                            	
#define FTD_SB_SYSTEM_INFO_BITS1                          	0x3265                                            	
#define FTD_SB_SYSTEM_INFO_BITS2                          	0x3266                                            	
#define FTD_SB_SYSTEM_INFO_BITS3                          	0x3267                                            	
#define FTD_SB_SYSTEM_INFO_BITS4                          	0x3268                                            	
#define FTD_SB_SYSTEM_INFO_BITS5                          	0x3269                                            	
#define FTD_SB_SYSTEM_INFO_BITS6                          	0x326A                                            	
#define FTD_SB_SYSTEM_INFO_BITS7                          	0x326B                                            	
#define FTD_SB_SYSTEM_INFO_BITS8                          	0x326C                                            	
#define FTD_SB_SYSTEM_INFO_BITS9                          	0x326D                                            	
#define FTD_SB_SYSTEM_INFO_BITS10                         	0x326E                                            	
#define FTD_SB_DTM_MAX_LAPDM                              	0x326F                                            	
#define FTD_SB_UMA_PREF_MODE_SET                          	0x3270                                            	
#define FTD_SB_UMA_PREF_MODE_NR_READ                      	0x3271                                            	
#define GSS_FTD_LAST_BYTE_SUB_BLOCK                       	0x3272                                            	
#define FTD_SB_PENALTY_TIME                               	0x3400                                            	
#define FTD_SB_LAC                                        	0x3401                                            	
#define FTD_SB_CELL_ID                                    	0x3402                                            	
#define FTD_SB_T_RESEL                                    	0x3403                                            	
#define FTD_SB_NEIGH_PSW_ATT                              	0x3405                                            	
#define FTD_SB_NEIGH_SYNC_ATT                             	0x3406                                            	
#define FTD_SB_NEIGH_BCCH_ATT                             	0x3407                                            	
#define FTD_SB_NEIGH_BCCH_EXT_ATT                         	0x3408                                            	
#define FTD_SB_NORMAL_LOC_UP_COUNT                        	0x3409                                            	
#define FTD_SB_SUCC_LOC_UP_COUNT                          	0x340A                                            	
#define FTD_SB_LOC_UP_COUNT_IMSI                          	0x340B                                            	
#define FTD_SB_SUCC_LOC_UP_COUNT_IMSI                     	0x340C                                            	
#define FTD_SB_HO_GSM_GSM                                 	0x340D                                            	
#define FTD_SB_HO_DCS_DCS                                 	0x340E                                            	
#define FTD_SB_HO_GSM_DCS                                 	0x340F                                            	
#define FTD_SB_HO_DCS_GSM                                 	0x3410                                            	
#define FTD_SB_FAILED_HO_GSM_GSM                          	0x3411                                            	
#define FTD_SB_FAILED_HO_DCS_DCS                          	0x3412                                            	
#define FTD_SB_FAILED_HO_GSM_DCS                          	0x3413                                            	
#define FTD_SB_FAILED_HO_DCS_GSM                          	0x3414                                            	
#define FTD_SB_BACK_TO_PREV_GSM_GSM                       	0x3415                                            	
#define FTD_SB_BACK_TO_PREV_DCS_DCS                       	0x3416                                            	
#define FTD_SB_BACK_TO_PREV_GSM_DCS                       	0x3417                                            	
#define FTD_SB_BACK_TO_PREV_DCS_GSM                       	0x3418                                            	
#define FTD_SB_INTRA_HO_GSM_GSM                           	0x3419                                            	
#define FTD_SB_INTRA_HO_DCS_DCS                           	0x341A                                            	
#define FTD_SB_INTRA_HO_GSM_DCS                           	0x341B                                            	
#define FTD_SB_INTRA_HO_DCS_GSM                           	0x341C                                            	
#define FTD_SB_FAILED_INTRA_HO_GSM_GSM                    	0x341D                                            	
#define FTD_SB_FAILED_INTRA_HO_DCS_DCS                    	0x341E                                            	
#define FTD_SB_FAILED_INTRA_HO_GSM_DCS                    	0x341F                                            	
#define FTD_SB_FAILED_INTRA_HO_DCS_GSM                    	0x3420                                            	
#define FTD_SB_INTRA_BACK_TO_PREV_GSM_GSM                 	0x3421                                            	
#define FTD_SB_INTRA_BACK_TO_PREV_DCS_DCS                 	0x3422                                            	
#define FTD_SB_INTRA_BACK_TO_PREV_GSM_DCS                 	0x3423                                            	
#define FTD_SB_INTRA_BACK_TO_PREV_DCS_GSM                 	0x3424                                            	
#define FTD_SB_T200_EXP_MS_GSM                            	0x3425                                            	
#define FTD_SB_T200_EXP_BS_GSM                            	0x3426                                            	
#define FTD_SB_T200_EXP_MS_DCS                            	0x3427                                            	
#define FTD_SB_T200_EXP_BS_DCS                            	0x3428                                            	
#define FTD_SB_MO_CALL_COUNT                              	0x3429                                            	
#define FTD_SB_MO_CALL_OK_COUNT                           	0x342A                                            	
#define FTD_SB_MT_CALL_COUNT                              	0x342B                                            	
#define FTD_SB_MT_CALL_OK_COUNT                           	0x342C                                            	
#define FTD_SB_MO_SMS_COUNT                               	0x342D                                            	
#define FTD_SB_MO_SMS_OK_COUNT                            	0x342E                                            	
#define FTD_SB_MT_SMS_COUNT                               	0x342F                                            	
#define FTD_SB_MT_SMS_OK_COUNT                            	0x3430                                            	
#define FTD_SB_CB_SCHEDULE_MSG                            	0x3431                                            	
#define FTD_SB_TR1M_COUNT                                 	0x3432                                            	
#define FTD_SB_TR2M_COUNT                                 	0x3433                                            	
#define FTD_SB_TRAM_COUNT                                 	0x3434                                            	
#define FTD_SB_TC1M_COUNT                                 	0x3435                                            	
#define FTD_SB_TC2M_COUNT                                 	0x3436                                            	
#define FTD_SB_CB_SCHEDULE_COUNT                          	0x3437                                            	
#define FTD_SB_POWER_ON_TIMER                             	0x3438                                            	
#define FTD_SB_SERV_TIMER                                 	0x3439                                            	
#define FTD_SB_NSPS_TIMER                                 	0x343A                                            	
#define FTD_SB_TRANSMIT_TIMER                             	0x343B                                            	
#define FTD_SB_SMS_MO_ATT_GPRS                            	0x343C                                            	
#define FTD_SB_SMS_MO_GPRS_OK                             	0x343D                                            	
#define FTD_SB_SMS_MT_ATT_GPRS                            	0x343E                                            	
#define FTD_SB_SMS_MT_GPRS_OK                             	0x343F                                            	
#define GSS_FTD_LAST_WORD_SUB_BLOCK                       	0x3440                                            	
#define FTD_SB_LAST_REG                                   	0x3600                                            	
#define FTD_SB_TMSI                                       	0x3601                                            	
#define FTD_SB_1ST_FORBIDDEN_NW                           	0x3602                                            	
#define FTD_SB_2ND_FORBIDDEN_NW                           	0x3603                                            	
#define FTD_SB_3RD_FORBIDDEN_NW                           	0x3604                                            	
#define FTD_SB_4TH_FORBIDDEN_NW                           	0x3605                                            	
#define FTD_SB_LAST_USED_BAND_SET                         	0x3606                                            	
#define FTD_SB_SELECTED_RAT_SET                           	0x3607                                            	
#define FTD_SB_RX_TIMESLOT_ALLOC                          	0x3608                                            	
#define FTD_SB_TX_TIMESLOT_ALLOC                          	0x3609                                            	
#define FTD_SB_1_EQUIV_PLMN                               	0x360A                                            	
#define FTD_SB_2_EQUIV_PLMN                               	0x360B                                            	
#define FTD_SB_3_EQUIV_PLMN                               	0x360C                                            	
#define FTD_SB_4_EQUIV_PLMN                               	0x360D                                            	
#define FTD_SB_5_EQUIV_PLMN                               	0x360E                                            	
#define FTD_SB_6_EQUIV_PLMN                               	0x360F                                            	
#define FTD_SB_7_EQUIV_PLMN                               	0x3610                                            	
#define FTD_SB_8_EQUIV_PLMN                               	0x3611                                            	
#define FTD_SB_9_EQUIV_PLMN                               	0x3612                                            	
#define FTD_SB_10_EQUIV_PLMN                              	0x3613                                            	
#define FTD_SB_11_EQUIV_PLMN                              	0x3614                                            	
#define FTD_SB_12_EQUIV_PLMN                              	0x3615                                            	
#define FTD_SB_13_EQUIV_PLMN                              	0x3616                                            	
#define FTD_SB_14_EQUIV_PLMN                              	0x3617                                            	
#define FTD_SB_15_EQUIV_PLMN                              	0x3618                                            	
#define FTD_SB_16_EQUIV_PLMN                              	0x3619                                            	
#define FTD_SB_DTM_CS_TSLOT_CONF                          	0x361A                                            	
#define FTD_SB_DTM_PS_DL_CONF                             	0x361B                                            	
#define FTD_SB_DTM_PS_UL_CONF                             	0x361C                                            	
#define FTD_SB_NO_PSW_FOUND_GSM                           	0x361D                                            	
#define FTD_SB_NO_PSW_FOUND_DCS                           	0x361E                                            	
#define FTD_SB_SYNC_GSM                                   	0x361F                                            	
#define FTD_SB_SYNC_DCS                                   	0x3620                                            	
#define FTD_SB_RESEL_GSM_GSM                              	0x3621                                            	
#define FTD_SB_RESEL_DCS_DCS                              	0x3622                                            	
#define FTD_SB_RESEL_GSM_DCS                              	0x3623                                            	
#define FTD_SB_RESEL_DCS_GSM                              	0x3624                                            	
#define FTD_SB_UMA_1_AVAIL_PLMN                           	0x3625                                            	
#define FTD_SB_UMA_2_AVAIL_PLMN                           	0x3626                                            	
#define FTD_SB_UMA_3_AVAIL_PLMN                           	0x3627                                            	
#define FTD_SB_UMA_4_AVAIL_PLMN                           	0x3628                                            	
#define FTD_SB_UMA_5_AVAIL_PLMN                           	0x3629                                            	
#define FTD_SB_UMA_6_AVAIL_PLMN                           	0x362A                                            	
#define GSS_FTD_LAST_DWORD_SUB_BLOCK                      	0x362B                                            	
#define FTD_SB_CARRIER                                    	0x3800                                            	
#define FTD_SB_HOPPING                                    	0x3801                                            	
#define FTD_SB_TX_LEVEL                                   	0x3802                                            	
#define FTD_SB_C1                                         	0x3803                                            	
#define FTD_SB_TYPE_OF_CURR_CH                            	0x3804                                            	
#define FTD_SB_C2                                         	0x3805                                            	
#define FTD_SB_PAGING_MODE                                	0x3806                                            	
#define FTD_SB_ROAMING_IND                                	0x3807                                            	
#define FTD_SB_C1_1_NEIGH                                 	0x3808                                            	
#define FTD_SB_C1_2_NEIGH                                 	0x3809                                            	
#define FTD_SB_C1_3_NEIGH                                 	0x380A                                            	
#define FTD_SB_C1_4_NEIGH                                 	0x380B                                            	
#define FTD_SB_C1_5_NEIGH                                 	0x380C                                            	
#define FTD_SB_C1_6_NEIGH                                 	0x380D                                            	
#define FTD_SB_C1_7_NEIGH                                 	0x380E                                            	
#define FTD_SB_C1_8_NEIGH                                 	0x380F                                            	
#define FTD_SB_C2_1_NEIGH                                 	0x3810                                            	
#define FTD_SB_C2_2_NEIGH                                 	0x3811                                            	
#define FTD_SB_C2_3_NEIGH                                 	0x3812                                            	
#define FTD_SB_C2_4_NEIGH                                 	0x3813                                            	
#define FTD_SB_C2_5_NEIGH                                 	0x3814                                            	
#define FTD_SB_C2_6_NEIGH                                 	0x3815                                            	
#define FTD_SB_C2_7_NEIGH                                 	0x3816                                            	
#define FTD_SB_C2_8_NEIGH                                 	0x3817                                            	
#define FTD_SB_CIPH                                       	0x381A                                            	
#define FTD_SB_DTX_VALUE                                  	0x381B                                            	
#define FTD_SB_IMSI_ATT                                   	0x381C                                            	
#define FTD_SB_DTX_STATUS                                 	0x381D                                            	
#define FTD_SB_DTX_DEF                                    	0x381E                                            	
#define FTD_SB_DTX_FROM_BS                                	0x381F                                            	
#define FTD_SB_CC_CAUSE                                   	0x3820                                            	
#define FTD_SB_MM_CAUSE                                   	0x3821                                            	
#define FTD_SB_RR_CAUSE                                   	0x3822                                            	
#define FTD_SB_LOCATION_AREA_INFO_1                       	0x3823                                            	
#define FTD_SB_LOCATION_AREA_INFO_2                       	0x3824                                            	
#define FTD_SB_LOCATION_AREA_INFO_3                       	0x3825                                            	
#define FTD_SB_LOCATION_AREA_INFO_4                       	0x3826                                            	
#define FTD_SB_LOCATION_AREA_INFO_5                       	0x3827                                            	
#define FTD_SB_LOCATION_AREA_INFO_6                       	0x3828                                            	
#define FTD_SB_LOCATION_AREA_INFO_7                       	0x3829                                            	
#define FTD_SB_LOCATION_AREA_INFO_8                       	0x382A                                            	
#define FTD_SB_CELL_PRIORITY_1                            	0x382B                                            	
#define FTD_SB_CELL_PRIORITY_2                            	0x382C                                            	
#define FTD_SB_CELL_PRIORITY_3                            	0x382D                                            	
#define FTD_SB_CELL_PRIORITY_4                            	0x382E                                            	
#define FTD_SB_CELL_PRIORITY_5                            	0x382F                                            	
#define FTD_SB_CELL_PRIORITY_6                            	0x3830                                            	
#define FTD_SB_CELL_PRIORITY_7                            	0x3831                                            	
#define FTD_SB_CELL_PRIORITY_8                            	0x3832                                            	
#define FTD_SB_CELL_BARR_IND_STATUS                       	0x3833                                            	
#define FTD_SB_C1S                                        	0x3834                                            	
#define FTD_SB_C2S                                        	0x3835                                            	
#define FTD_SB_HOPP_STATUS                                	0x3836                                            	
#define FTD_SB_TEST_TIMER_STATE                           	0x3837                                            	
#define FTD_SB_TOGGLE_TRANS_STATUS                        	0x3838                                            	
#define FTD_SB_PH_SI13_LOCATION                           	0x3839                                            	
#define FTD_SB_PH_USF                                     	0x383A                                            	
#define FTD_SB_PBCCH_HOPPING                              	0x383B                                            	
#define FTD_SB_PCCCH_HOPPING                              	0x383C                                            	
#define FTD_SB_GPRS_HOPPING                               	0x383D                                            	
#define FTD_SB_GPRS_TYPE_OF_CURR_CH                       	0x383E                                            	
#define FTD_SB_GMM_STATE                                  	0x383F                                            	
#define FTD_SB_PRIOR_CLASS_SERV                           	0x3840                                            	
#define FTD_SB_C32_SERV                                   	0x3841                                            	
#define FTD_SB_PRIOR_CLASS_1_NEIGH                        	0x3842                                            	
#define FTD_SB_PRIOR_CLASS_2_NEIGH                        	0x3843                                            	
#define FTD_SB_PRIOR_CLASS_3_NEIGH                        	0x3844                                            	
#define FTD_SB_PRIOR_CLASS_4_NEIGH                        	0x3845                                            	
#define FTD_SB_PRIOR_CLASS_5_NEIGH                        	0x3846                                            	
#define FTD_SB_PRIOR_CLASS_6_NEIGH                        	0x3847                                            	
#define FTD_SB_C32_1_NEIGH                                	0x3848                                            	
#define FTD_SB_C32_2_NEIGH                                	0x3849                                            	
#define FTD_SB_C32_3_NEIGH                                	0x384A                                            	
#define FTD_SB_C32_4_NEIGH                                	0x384B                                            	
#define FTD_SB_C32_5_NEIGH                                	0x384C                                            	
#define FTD_SB_C32_6_NEIGH                                	0x384D                                            	
#define FTD_SB_INT_PRO_MODE                               	0x384E                                            	
#define FTD_SB_CARRIER_1_NEIGH                            	0x384F                                            	
#define FTD_SB_CARRIER_2_NEIGH                            	0x3850                                            	
#define FTD_SB_CARRIER_3_NEIGH                            	0x3851                                            	
#define FTD_SB_CARRIER_4_NEIGH                            	0x3852                                            	
#define FTD_SB_CARRIER_5_NEIGH                            	0x3853                                            	
#define FTD_SB_CARRIER_6_NEIGH                            	0x3854                                            	
#define FTD_SB_CARRIER_7_NEIGH                            	0x3855                                            	
#define FTD_SB_CARRIER_8_NEIGH                            	0x3856                                            	
#define FTD_SB_BCCH_CARRIER_SERV                          	0x3857                                            	
#define FTD_SB_GPRS_CARRIER_SERV                          	0x3858                                            	
#define FTD_SB_GPRS_CARRIER                               	0x3859                                            	
#define FTD_SB_GPRS_CARRIER_1_NEIGH                       	0x385A                                            	
#define FTD_SB_GPRS_CARRIER_2_NEIGH                       	0x385B                                            	
#define FTD_SB_GPRS_CARRIER_3_NEIGH                       	0x385C                                            	
#define FTD_SB_GPRS_CARRIER_4_NEIGH                       	0x385D                                            	
#define FTD_SB_GPRS_CARRIER_5_NEIGH                       	0x385E                                            	
#define FTD_SB_GPRS_CARRIER_6_NEIGH                       	0x385F                                            	
#define FTD_SB_LAST_CALL_REL_DIRECTION                    	0x3860                                            	
#define FTD_SB_ENAB_OR_DISAB_TEST_TIMERS                  	0x3861                                            	
#define FTD_SB_REG_CC                                     	0x3862                                            	
#define FTD_SB_REG_MNC                                    	0x3863                                            	
#define FTD_SB_INT_PRO_MODE_TOGGLE                        	0x3864                                            	
#define FTD_SB_DTM_SUPP                                   	0x3865                                            	
#define FTD_SB_UMA_PREF_MODE_TXT_READ                     	0x3866                                            	
#define FTD_SB_UMA_MCC                                    	0x3867                                            	
#define FTD_SB_UMA_MNC                                    	0x3868                                            	
#define FTD_SB_UMA_RAT_INFO                               	0x3869                                            	
#define FTD_SB_UMA_INFO                                   	0x386A                                            	
#define FTD_SB_UMA_DTM_SUPPORT                            	0x386B                                            	
#define FTD_SB_UMA_GPRS_AVAIL                             	0x386C                                            	
#define FTD_SB_UMA_NMO                                    	0x386D                                            	
#define FTD_SB_UMA_ECMP                                   	0x386E                                            	
#define GSS_FTD_LAST_SUB_BLOCK                            	0x386F                                            	

//ConstantTable for GSS_OPERATION
#define GSS_CS_STATUS_GET                                 	0x00                                                 	
#define GSS_CS_TASKS_SUSPEND                              	0x01                                                 	
#define GSS_CS_LOCAL_INFO_GET                             	0x03                                                 	
#define GSS_POWER_CLASS_CHANGE                            	0x04                                                 	
#define GSS_BAND_INFO_GET                                 	0x05                                                 	
#define GSS_SELECTED_RAT_WRITE                            	0x0E                                                 	
#define GSS_ATK_TIMING_ADVANCE_GET                        	0x96                                              	
#define GSS_UMA_PREF_MODE_READ                            	0x97                                              	
#define GSS_UMA_PREF_MODE_WRITE                           	0x98                                              	
#define GSS_PROV_SGW_UNC_WRITE                            	0x99                                              	
#define GSS_PROV_SGW_UNC_READ                             	0x9A                                              	
#define GSS_GAN_CONNECT                                   	0x9B                                              	
#define GSS_SELECTED_RAT_READ                             	0x9C                                              	
#define GSS_SELECTED_BANDS_WRITE                          	0x9D                                              	
#define GSS_SELECTED_BANDS_READ                           	0x9E                                              	
#define GSS_CELL_BARRING_STATUS_READ                      	0x9F                                              	

//ConstantTable for GSS_COMMAND_QUALIFIER
#define GSS_CELL_INFO_CMD                                 	0x00                                                 	
#define GSS_LONG_CELL_INFO_CMD                            	0x01                                                 	

//ConstantTable for GSS_POWERCLASS
#define GSS_POWER_CLASS_1                                 	0x01                                                 	
#define GSS_POWER_CLASS_2                                 	0x02                                                 	
#define GSS_POWER_CLASS_3                                 	0x03                                                 	
#define GSS_POWER_CLASS_4                                 	0x04                                                 	
#define GSS_POWER_CLASS_5                                 	0x05                                                 	
#define GSS_POWER_CLASS_NO_CHANGE                         	0xFD                                              	
#define GSS_POWER_CLASS_DEFAULT                           	0xFE                                              	
#define GSS_POWER_CLASS_NOT_IN_USE                        	0xFF                                              	

//ConstantTable for GSS_CAUSE
#define GSS_SERVICE_FAIL                                  	0x01                                                 	
#define GSS_SERVICE_NOT_ALLOWED                           	0x02                                                 	
#define GSS_SERVICE_FAIL_CS_INACTIVE                      	0x03                                                 	

//BitmaskTables for GSS_GSM_BAND_INFO
#define GSM_900_BAND_SUPPORT                              	0x01                                                 	
#define GSM_1800_BAND_SUPPORT                             	0x02                                                 	
#define GSM_1900_BAND_SUPPORT                             	0x04                                                 	
#define GSM_850_BAND_SUPPORT                              	0x08                                                 	

//ConstantTable for GSS_CS_STATE
#define GSS_CS_IDLE                                       	0x00                                                 	
#define GSS_CS_SIGNALLING                                 	0x01                                                 	
#define GSS_CS_CALL                                       	0x02                                                 	
#define GSS_CS_PACKET_DATA                                	0x03                                                 	

//ConstantTable for GSS_CS_REGISTRATION_STATUS
#define GSS_CS_STATUS_SERV                                	0x80                                              	
#define GSS_CS_STATUS_CAMPED_ON_CELL                      	0x0C                                                 	
#define GSS_CS_STATUS_IDLE                                	0x0D                                                 	
#define GSS_CS_STATUS_RSSI_MEASURING                      	0x0E                                                 	
#define GSS_CS_STATUS_PSW_WAIT                            	0x10                                              	
#define GSS_CS_STATUS_SYNC_WAIT                           	0x11                                              	
#define GSS_CS_STATUS_SCANNING                            	0x12                                              	
#define GSS_CS_STATUS_ALL_CCCH                            	0x13                                              	
#define GSS_CS_STATUS_SDCCH                               	0x14                                              	
#define GSS_CS_STATUS_TCH                                 	0x15                                              	
#define GSS_CS_STATUS_INIT                                	0x16                                              	

//BitmaskTables for GSS_RAT_INFO_FUNCTIONS
#define GSS_GSM_RAT                                       	0x01                                                 	
#define GSS_UMTS_RAT                                      	0x02                                                 	
#define GSS_EUTRAN_RAT                                    	0x04                                                 	
#define GSS_DUAL_RAT                                      	0x03                                                 	

//BitmaskTables for GSS_GSM_BAND_VALUES
#define GSS_GSM_NO_BANDS                                  	0x00                                                 	
#define GSS_GSM_900_BAND_MASK                             	0x01                                                 	
#define GSS_GSM_1800_BAND_MASK                            	0x02                                                 	
#define GSS_GSM_1900_BAND_MASK                            	0x04                                                 	
#define GSS_GSM_850_BAND_MASK                             	0x08                                                 	
#define GSS_ALL_GSM_BANDS                                 	0xFF                                              	


//BitmaskTables for GSS_UMTS_BAND_VALUES
#define GSS_UMTS_NO_BANDS                                 	0x00                                                 	
#define GSS_UMTS_BAND_I_MASK                              	0x01                                                 	
#define GSS_UMTS_BAND_II_MASK                             	0x02                                                 	
#define GSS_UMTS_BAND_III_MASK                            	0x04                                                 	
#define GSS_UMTS_BAND_IV_MASK                             	0x08                                                 	
#define GSS_UMTS_BAND_V_MASK                              	0x10                                              	
#define GSS_UMTS_BAND_VI_MASK                             	0x20                                              	
#define GSS_UMTS_BAND_VII_MASK                            	0x40                                              	
#define GSS_UMTS_BAND_VIII_MASK                           	0x80                                              	
#define GSS_UMTS_BAND_IX_MASK                             	0x100                                             	
#define GSS_UMTS_BAND_X_MASK                              	0x200                                             	
#define GSS_UMTS_BAND_XI_MASK                             	0x400                                             	
#define GSS_UMTS_BAND_XII_MASK                            	0x800                                             	
#define GSS_UMTS_BAND_XIII_MASK                           	0x1000                                            	
#define GSS_UMTS_BAND_XIV_MASK                            	0x2000                                            	
#define GSS_UMTS_BAND_XV_MASK                             	0x4000                                            	
#define GSS_UMTS_BAND_XVI_MASK                            	0x8000                                            	
#define GSS_UMTS_BAND_XVII_MASK                           	0x10000                                           	
#define GSS_UMTS_BAND_XVIII_MASK                          	0x20000                                           	
#define GSS_UMTS_BAND_XIX_MASK                            	0x40000                                           	
#define GSS_UMTS_BAND_XX_MASK                             	0x80000                                           	
#define GSS_UMTS_BAND_XXI_MASK                            	0x100000                                          	
#define GSS_UMTS_BAND_XXII_MASK                           	0x200000                                          	
#define GSS_UMTS_ALL_BANDS                                	0xFFFFFFFF                                        	

//ConstantTable for TIMING_ADVANCE_STATUS
#define GSS_TIMING_ADVANCE_NOT_VALID                      	0x00                                                 	
#define GSS_TIMING_ADVANCE_VALID                          	0x01                                                 	

//ConstantTable for GSS_UMA_MODE_SELECTION
#define GSS_GERAN_ONLY                                    	0x00                                                 	
#define GSS_GERAN_PREFERRED                               	0x01                                                 	
#define GSS_UMAN_ONLY                                     	0x02                                                 	
#define GSS_UMAN_PREFERRED                                	0x03                                                 	

//ConstantTable for GSS_SGW_UNC_IP_ADDRESS
#define GSS_SRV_NO_IP_ADDRESS                             	0x00                                                 	
#define GSS_SRV_IP_ADDR_TYPE_IPv4                         	0x01                                                 	
#define GSS_SRV_IP_ADDR_TYPE_IPv6                         	0x02                                                 	

//ConstantTable for GSS_HSXPA_USER_SETTING
#define GSS_HSXPA_DISABLED                                	0x01                                                 	
#define GSS_HSXPA_ALLOWED                                 	0x02                                                 	

//ConstantTable for GSS_WRITE_STATUS
#define GSS_OK                                            	0x01                                                 	
#define GSS_FAIL                                          	0x02                                                 	

//ConstantTable for GSS_HSXPA_CAUSE
#define GSS_HSXPA_DISABLED_VIA_PP                         	0x01                                                 	

//ConstantTable for GSS_HAC_MODE
#define GSS_HAC_ACTIVE                                    	0x01                                                 	
#define GSS_HAC_INACTIVE                                  	0x02                                                 	


//SUBBLOCKS


//Definition for GSS_CS_LOCAL_INFO
#define GSS_CS_LOCAL_INFO                                 	0x01                                                 	
#define GSS_CS_LOCAL_INFO_OFFSET_SUBBLOCKID               	0 //size 1 byte(s)
#define GSS_CS_LOCAL_INFO_OFFSET_SUBBLOCKLENGTH           	1 //size 1 byte(s)
#define GSS_CS_LOCAL_INFO_OFFSET_COMMANDQUALIFIER         	2 //size 1 byte(s)
#define GSS_CS_LOCAL_INFO_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_GSS_CS_LOCAL_INFO                            	4


//Definition for GSS_POWER_CLASS
#define GSS_POWER_CLASS                                   	0x02                                                 	
#define GSS_POWER_CLASS_OFFSET_SUBBLOCKID                 	0 //size 1 byte(s)
#define GSS_POWER_CLASS_OFFSET_SUBBLOCKLENGTH             	1 //size 1 byte(s)
#define GSS_POWER_CLASS_OFFSET_GSMLOWERBANDSPOWERCLASS    	2 //size 1 byte(s)
#define GSS_POWER_CLASS_OFFSET_GSMUPPERBANDSPOWERCLASS    	3 //size 1 byte(s)
#define SIZE_GSS_POWER_CLASS                              	4


//Definition for GSS_CS_STATUS
#define GSS_CS_STATUS                                     	0x00                                                 	
#define GSS_CS_STATUS_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define GSS_CS_STATUS_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define GSS_CS_STATUS_OFFSET_REGISTRATIONSTATUS           	2 //size 1 byte(s)
#define GSS_CS_STATUS_OFFSET_FILLERBYTE1                  	3 //size 1 byte(s)
#define SIZE_GSS_CS_STATUS                                	4


//Definition for GSS_CELL_INFO
#define GSS_CELL_INFO                                     	0x03                                                 	
#define GSS_CELL_INFO_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define GSS_CELL_INFO_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define GSS_CELL_INFO_OFFSET_MCCMNC                       	2 //size 3 byte(s)
#define GSS_CELL_INFO_OFFSET_TIMINGADVANCE                	5 //size 1 byte(s)
#define GSS_CELL_INFO_OFFSET_LAC                          	6 //size 2 byte(s)
#define GSS_CELL_INFO_OFFSET_CELLID                       	8 //size 2 byte(s)
#define GSS_CELL_INFO_OFFSET_FILLERBYTE1                  	10 //size 2 byte(s)
#define SIZE_GSS_CELL_INFO                                	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GSS_LONG_CELL_INFO
#define GSS_LONG_CELL_INFO                                	0x11                                              	
#define GSS_LONG_CELL_INFO_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_MCCMNC                  	2 //size 3 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_TIMINGADVANCE           	5 //size 1 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_LAC                     	6 //size 2 byte(s)
#define GSS_LONG_CELL_INFO_OFFSET_CELLID                  	8 //size 4 byte(s)
#define SIZE_GSS_LONG_CELL_INFO                           	12
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.


//Definition for GSS_BAND_INFO
#define GSS_BAND_INFO                                     	0x04                                                 	
#define GSS_BAND_INFO_OFFSET_SUBBLOCKID                   	0 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_SUBBLOCKLENGTH               	1 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_BANDINFO                     	2 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_GSMLOWERBANDSPOWERCLASS      	3 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_GSMUPPERBANDSPOWERCLASS      	4 //size 1 byte(s)
#define GSS_BAND_INFO_OFFSET_FILLERBYTE1                  	5 //size 3 byte(s)
#define SIZE_GSS_BAND_INFO                                	8


//Definition for GSS_RAT_INFO
#define GSS_RAT_INFO                                      	0x0B                                                 	
#define GSS_RAT_INFO_OFFSET_SUBBLOCKID                    	0 //size 1 byte(s)
#define GSS_RAT_INFO_OFFSET_SUBBLOCKLENGTH                	1 //size 1 byte(s)
#define GSS_RAT_INFO_OFFSET_FUNCTION                      	2 //size 1 byte(s)
#define GSS_RAT_INFO_OFFSET_FILLERBYTE1                   	3 //size 1 byte(s)
#define SIZE_GSS_RAT_INFO                                 	4


//Definition for GSS_ATK_TIMING_ADVANCE
#define GSS_ATK_TIMING_ADVANCE                            	0x0C                                                 	
#define GSS_ATK_TIMING_ADVANCE_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define GSS_ATK_TIMING_ADVANCE_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define GSS_ATK_TIMING_ADVANCE_OFFSET_TASTATUS            	2 //size 1 byte(s)
#define GSS_ATK_TIMING_ADVANCE_OFFSET_TIMINGADVANCE       	3 //size 1 byte(s)
#define SIZE_GSS_ATK_TIMING_ADVANCE                       	4


//Definition for GSS_UMA_PREF_MODE_INFO
#define GSS_UMA_PREF_MODE_INFO                            	0x0D                                                 	
#define GSS_UMA_PREF_MODE_INFO_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_INFO_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_INFO_OFFSET_UMAPREFMODEI        	2 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_INFO_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_GSS_UMA_PREF_MODE_INFO                       	4


//Definition for GSS_PROV_INFO_SB_IDS
#define GSS_PROV_INFO_SB_IDS                              	0x10                                              	
#define GSS_PROV_INFO_SB_IDS_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define GSS_PROV_INFO_SB_IDS_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define GSS_PROV_INFO_SB_IDS_OFFSET_SBIDCOUNT             	2 //size 1 byte(s)
#define GSS_PROV_INFO_SB_IDS_OFFSET_PROVSBID              	3 //size 1 byte(s)
#define SIZE_GSS_PROV_INFO_SB_IDS                         	4
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GSS_SGW_INFO
#define GSS_SGW_INFO                                      	0x0E                                                 	
#define GSS_SGW_INFO_OFFSET_SUBBLOCKID                    	0 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_FILLERBYTE1                   	1 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_SUBBLOCKLENGTH                	2 //size 2 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWIPADDRTYPE                 	4 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWIPADDRESS                  	5 //size 16 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWFQDNLENGTH                 	21 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_SGWFQDN                       	22 //size 1 byte(s)
#define GSS_SGW_INFO_OFFSET_FILLERBYTE2                   	23 //size 1 byte(s)
#define SIZE_GSS_SGW_INFO                                 	24
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GSS_UNC_INFO
#define GSS_UNC_INFO                                      	0x0F                                                 	
#define GSS_UNC_INFO_OFFSET_SUBBLOCKID                    	0 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE1                   	1 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_SUBBLOCKLENGTH                	2 //size 2 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCTCPPORT                    	4 //size 2 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCIPADDRTYPE                 	6 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCIPADDRESS                  	7 //size 16 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCFQDNLENGTH                 	23 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_UNCFQDN                       	24 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE2                   	25 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE3                   	26 //size 1 byte(s)
#define GSS_UNC_INFO_OFFSET_FILLERBYTE4                   	27 //size 1 byte(s)
#define SIZE_GSS_UNC_INFO                                 	28
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for GSS_SELECTED_UMTS_BAND_INFO
#define GSS_SELECTED_UMTS_BAND_INFO                       	0x12                                              	
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SUBBLOCKLENGTH 	1 //size 1 byte(s)
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_FILLERBYTE1    	2 //size 2 byte(s)
#define GSS_SELECTED_UMTS_BAND_INFO_OFFSET_SELECTEDUMTSBANDS	4 //size 4 byte(s)
#define SIZE_GSS_SELECTED_UMTS_BAND_INFO                  	8


//Definition for GSS_SELECTED_GSM_BAND_INFO
#define GSS_SELECTED_GSM_BAND_INFO                        	0x13                                              	
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_SUBBLOCKID      	0 //size 1 byte(s)
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_SUBBLOCKLENGTH  	1 //size 1 byte(s)
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_SELECTEDGSMBANDS	2 //size 1 byte(s)
#define GSS_SELECTED_GSM_BAND_INFO_OFFSET_FILLERBYTE1     	3 //size 1 byte(s)
#define SIZE_GSS_SELECTED_GSM_BAND_INFO                   	4


//Definition for GSS_REL_SIGNAL_LEVEL_INFO
#define GSS_REL_SIGNAL_LEVEL_INFO                         	0x14                                              	
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_SUBBLOCKLENGTH   	1 //size 1 byte(s)
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_RELSIGNALLEVEL   	2 //size 1 byte(s)
#define GSS_REL_SIGNAL_LEVEL_INFO_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_GSS_REL_SIGNAL_LEVEL_INFO                    	4


//Definition for GSS_THRESHOLD_INFO
#define GSS_THRESHOLD_INFO                                	0x15                                              	
#define GSS_THRESHOLD_INFO_OFFSET_SUBBLOCKID              	0 //size 1 byte(s)
#define GSS_THRESHOLD_INFO_OFFSET_SUBBLOCKLENGTH          	1 //size 1 byte(s)
#define GSS_THRESHOLD_INFO_OFFSET_THRESHOLD               	2 //size 1 byte(s)
#define GSS_THRESHOLD_INFO_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_GSS_THRESHOLD_INFO                           	4


//MESSAGES


//Definition for GSS_CS_SERVICE_REQ
#define GSS_CS_SERVICE_REQ                                	0x00                                                 	
#define GSS_CS_SERVICE_REQ_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define GSS_CS_SERVICE_REQ_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define GSS_CS_SERVICE_REQ_OFFSET_OPERATION               	2 //size 1 byte(s)
#define GSS_CS_SERVICE_REQ_OFFSET_NBROFSUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_GSS_CS_SERVICE_REQ                           	4


//Definition for GSS_CS_SERVICE_RESP
#define GSS_CS_SERVICE_RESP                               	0x01                                                 	
#define GSS_CS_SERVICE_RESP_OFFSET_TRANSID                	0 //size 1 byte(s)
#define GSS_CS_SERVICE_RESP_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define GSS_CS_SERVICE_RESP_OFFSET_OPERATION              	2 //size 1 byte(s)
#define GSS_CS_SERVICE_RESP_OFFSET_NBROFSUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_GSS_CS_SERVICE_RESP                          	4


//Definition for GSS_CS_SERVICE_FAIL_RESP
#define GSS_CS_SERVICE_FAIL_RESP                          	0x02                                                 	
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_TRANSID           	0 //size 1 byte(s)
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_OPERATION         	2 //size 1 byte(s)
#define GSS_CS_SERVICE_FAIL_RESP_OFFSET_CAUSE             	3 //size 1 byte(s)
#define SIZE_GSS_CS_SERVICE_FAIL_RESP                     	4


//Definition for GSS_CS_IND
#define GSS_CS_IND                                        	0x03                                                 	
#define GSS_CS_IND_OFFSET_TRANSID                         	0 //size 1 byte(s)
#define GSS_CS_IND_OFFSET_MESSAGEID                       	1 //size 1 byte(s)
#define GSS_CS_IND_OFFSET_CSSTATE                         	2 //size 1 byte(s)
#define GSS_CS_IND_OFFSET_FILLERBYTE1                     	3 //size 1 byte(s)
#define SIZE_GSS_CS_IND                                   	4


//Definition for GSS_POWER_CLASS_IND
#define GSS_POWER_CLASS_IND                               	0x0A                                                 	
#define GSS_POWER_CLASS_IND_OFFSET_TRANSID                	0 //size 1 byte(s)
#define GSS_POWER_CLASS_IND_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define GSS_POWER_CLASS_IND_OFFSET_FILLERBYTE1            	2 //size 1 byte(s)
#define GSS_POWER_CLASS_IND_OFFSET_NBROFSUBBLOCKS         	3 //size 1 byte(s)
#define SIZE_GSS_POWER_CLASS_IND                          	4


//Definition for GSS_SELECTED_RAT_IND
#define GSS_SELECTED_RAT_IND                              	0x14                                              	
#define GSS_SELECTED_RAT_IND_OFFSET_TRANSID               	0 //size 1 byte(s)
#define GSS_SELECTED_RAT_IND_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define GSS_SELECTED_RAT_IND_OFFSET_SELECTEDRAT           	2 //size 1 byte(s)
#define GSS_SELECTED_RAT_IND_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_GSS_SELECTED_RAT_IND                         	4


//Definition for GSS_UMA_PREF_MODE_IND
#define GSS_UMA_PREF_MODE_IND                             	0x15                                              	
#define GSS_UMA_PREF_MODE_IND_OFFSET_TRANSID              	0 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_IND_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_IND_OFFSET_UMAPREFMODE          	2 //size 1 byte(s)
#define GSS_UMA_PREF_MODE_IND_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_GSS_UMA_PREF_MODE_IND                        	4


//Definition for GSS_HSXPA_USER_SETTING_WRITE_REQ
#define GSS_HSXPA_USER_SETTING_WRITE_REQ                  	0x0F                                                 	
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_TRANSID   	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_HSXPAUSERSETTING	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_REQ_OFFSET_FILLERBYTE1	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_WRITE_REQ             	4


//Definition for GSS_HSXPA_USER_SETTING_WRITE_RESP
#define GSS_HSXPA_USER_SETTING_WRITE_RESP                 	0x10                                              	
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_TRANSID  	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_WRITESTATUS	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_WRITE_RESP_OFFSET_FAILCAUSE	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_WRITE_RESP            	4


//Definition for GSS_HSXPA_USER_SETTING_IND
#define GSS_HSXPA_USER_SETTING_IND                        	0x11                                              	
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_TRANSID         	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_MESSAGEID       	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_HSXPAUSERSETTING	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_IND_OFFSET_CAUSE           	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_IND                   	4


//Definition for GSS_HSXPA_USER_SETTING_READ_REQ
#define GSS_HSXPA_USER_SETTING_READ_REQ                   	0x12                                              	
#define GSS_HSXPA_USER_SETTING_READ_REQ_OFFSET_TRANSID    	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_REQ_OFFSET_MESSAGEID  	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_REQ_OFFSET_FILLERBYTE1	2 //size 2 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_READ_REQ              	4


//Definition for GSS_HSXPA_USER_SETTING_READ_RESP
#define GSS_HSXPA_USER_SETTING_READ_RESP                  	0x13                                              	
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_TRANSID   	0 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_MESSAGEID 	1 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_HSXPAUSERSETTING	2 //size 1 byte(s)
#define GSS_HSXPA_USER_SETTING_READ_RESP_OFFSET_CAUSE     	3 //size 1 byte(s)
#define SIZE_GSS_HSXPA_USER_SETTING_READ_RESP             	4


//Definition for GSS_HAC_MODE_WRITE_REQ
#define GSS_HAC_MODE_WRITE_REQ                            	0x17                                              	
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_TRANSID             	0 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_HACMODE             	2 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_REQ_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_GSS_HAC_MODE_WRITE_REQ                       	4


//Definition for GSS_HAC_MODE_WRITE_RESP
#define GSS_HAC_MODE_WRITE_RESP                           	0x18                                              	
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_TRANSID            	0 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_WRITESTATUS        	2 //size 1 byte(s)
#define GSS_HAC_MODE_WRITE_RESP_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_GSS_HAC_MODE_WRITE_RESP                      	4

#endif