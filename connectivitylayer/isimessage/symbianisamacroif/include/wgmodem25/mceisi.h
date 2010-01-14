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
* Name      : PN_MODEM_MCE [ 0xC2 ] Resource Modem Control Entity
* Version   : 001.001
*
* Documentation: www.wirelessmodemapi.com
*
*/


#ifndef _SYMBIAN_ISI_MCEISI_H_
#define _SYMBIAN_ISI_MCEISI_H_

#define MODEM_CONTROL_ENTITY_ISI_VERSION_Z	 1
#define MODEM_CONTROL_ENTITY_ISI_VERSION_Y	 2


//TABLES

//ConstantTable for MCE_STATUS_INFO
#define MCE_OK                                            	0x00                                                 	
#define MCE_FAIL                                          	0x01                                                 	
#define MCE_ALREADY_ACTIVE                                	0x06                                                 	
#define MCE_TRANSITION_ONGOING                            	0x16                                              	
#define MCE_RESTART_REQUIRED                              	0x17                                              	

//ConstantTable for MCE_MODEM_STATE
#define MCE_NORMAL                                        	0x00                                                 	
#define MCE_LOCAL                                         	0x01                                                 	
#define MCE_SW_RESET                                      	0x80                                              	
#define MCE_POWER_OFF                                     	0x81                                              	

//ConstantTable for MCE_ACTION
#define MCE_START                                         	0x03                                                 	
#define MCE_READY                                         	0x04                                                 	

//ConstantTable for MCE_ACTION_SELECT
#define MCE_DISABLE                                       	0x00                                                 	
#define MCE_ENABLE                                        	0x01                                                 	

//ConstantTable for MCE_RF_STATE
#define MCE_RF_OFF                                        	0x00                                                 	
#define MCE_RF_ON                                         	0x01                                                 	


//MESSAGES


//Definition for MCE_MODEM_STATE_IND
#define MCE_MODEM_STATE_IND                               	0x00                                                 	
#define MCE_MODEM_STATE_IND_OFFSET_TRANSID                	0 //size 1 byte(s)
#define MCE_MODEM_STATE_IND_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define MCE_MODEM_STATE_IND_OFFSET_STATE                  	2 //size 1 byte(s)
#define MCE_MODEM_STATE_IND_OFFSET_ACTION                 	3 //size 1 byte(s)
#define SIZE_MCE_MODEM_STATE_IND                          	4


//Definition for MCE_MODEM_STATE_QUERY_REQ
#define MCE_MODEM_STATE_QUERY_REQ                         	0x01                                                 	
#define MCE_MODEM_STATE_QUERY_REQ_OFFSET_TRANSID          	0 //size 1 byte(s)
#define MCE_MODEM_STATE_QUERY_REQ_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define MCE_MODEM_STATE_QUERY_REQ_OFFSET_FILLERBYTE1      	2 //size 1 byte(s)
#define MCE_MODEM_STATE_QUERY_REQ_OFFSET_FILLERBYTE2      	3 //size 1 byte(s)
#define SIZE_MCE_MODEM_STATE_QUERY_REQ                    	4


//Definition for MCE_MODEM_STATE_QUERY_RESP
#define MCE_MODEM_STATE_QUERY_RESP                        	0x02                                                 	
#define MCE_MODEM_STATE_QUERY_RESP_OFFSET_TRANSID         	0 //size 1 byte(s)
#define MCE_MODEM_STATE_QUERY_RESP_OFFSET_MESSAGEID       	1 //size 1 byte(s)
#define MCE_MODEM_STATE_QUERY_RESP_OFFSET_CURRENT         	2 //size 1 byte(s)
#define MCE_MODEM_STATE_QUERY_RESP_OFFSET_TARGET          	3 //size 1 byte(s)
#define SIZE_MCE_MODEM_STATE_QUERY_RESP                   	4


//Definition for MCE_RF_STATE_IND
#define MCE_RF_STATE_IND                                  	0x05                                                 	
#define MCE_RF_STATE_IND_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define MCE_RF_STATE_IND_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define MCE_RF_STATE_IND_OFFSET_TARGET                    	2 //size 1 byte(s)
#define MCE_RF_STATE_IND_OFFSET_ACTION                    	3 //size 1 byte(s)
#define SIZE_MCE_RF_STATE_IND                             	4


//Definition for MCE_RF_STATE_REQ
#define MCE_RF_STATE_REQ                                  	0x03                                                 	
#define MCE_RF_STATE_REQ_OFFSET_TRANSID                   	0 //size 1 byte(s)
#define MCE_RF_STATE_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define MCE_RF_STATE_REQ_OFFSET_ACTION                    	2 //size 1 byte(s)
#define MCE_RF_STATE_REQ_OFFSET_FILLERBYTE1               	3 //size 1 byte(s)
#define SIZE_MCE_RF_STATE_REQ                             	4


//Definition for MCE_RF_STATE_RESP
#define MCE_RF_STATE_RESP                                 	0x04                                                 	
#define MCE_RF_STATE_RESP_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define MCE_RF_STATE_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define MCE_RF_STATE_RESP_OFFSET_STATUS                   	2 //size 1 byte(s)
#define MCE_RF_STATE_RESP_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_MCE_RF_STATE_RESP                            	4


//Definition for MCE_RF_STATE_QUERY_REQ
#define MCE_RF_STATE_QUERY_REQ                            	0x06                                                 	
#define MCE_RF_STATE_QUERY_REQ_OFFSET_TRANSID             	0 //size 1 byte(s)
#define MCE_RF_STATE_QUERY_REQ_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define MCE_RF_STATE_QUERY_REQ_OFFSET_FILLERBYTE1         	2 //size 2 byte(s)
#define SIZE_MCE_RF_STATE_QUERY_REQ                       	4


//Definition for MCE_RF_STATE_QUERY_RESP
#define MCE_RF_STATE_QUERY_RESP                           	0x07                                                 	
#define MCE_RF_STATE_QUERY_RESP_OFFSET_TRANSID            	0 //size 1 byte(s)
#define MCE_RF_STATE_QUERY_RESP_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define MCE_RF_STATE_QUERY_RESP_OFFSET_CURRENT            	2 //size 1 byte(s)
#define MCE_RF_STATE_QUERY_RESP_OFFSET_TARGET             	3 //size 1 byte(s)
#define SIZE_MCE_RF_STATE_QUERY_RESP                      	4


//Definition for MCE_POWER_OFF_REQ
#define MCE_POWER_OFF_REQ                                 	0x08                                                 	
#define MCE_POWER_OFF_REQ_OFFSET_TRANSID                  	0 //size 1 byte(s)
#define MCE_POWER_OFF_REQ_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define MCE_POWER_OFF_REQ_OFFSET_FILLERBYTE1              	2 //size 2 byte(s)
#define SIZE_MCE_POWER_OFF_REQ                            	4


//Definition for MCE_POWER_OFF_RESP
#define MCE_POWER_OFF_RESP                                	0x09                                                 	
#define MCE_POWER_OFF_RESP_OFFSET_TRANSID                 	0 //size 1 byte(s)
#define MCE_POWER_OFF_RESP_OFFSET_MESSAGEID               	1 //size 1 byte(s)
#define MCE_POWER_OFF_RESP_OFFSET_STATUS                  	2 //size 1 byte(s)
#define MCE_POWER_OFF_RESP_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_MCE_POWER_OFF_RESP                           	4


//Definition for MCE_RESET_REQ
#define MCE_RESET_REQ                                     	0x0A                                                 	
#define MCE_RESET_REQ_OFFSET_TRANSID                      	0 //size 1 byte(s)
#define MCE_RESET_REQ_OFFSET_MESSAGEID                    	1 //size 1 byte(s)
#define MCE_RESET_REQ_OFFSET_FILLERBYTE1                  	2 //size 2 byte(s)
#define SIZE_MCE_RESET_REQ                                	4


//Definition for MCE_RESET_RESP
#define MCE_RESET_RESP                                    	0x0B                                                 	
#define MCE_RESET_RESP_OFFSET_TRANSID                     	0 //size 1 byte(s)
#define MCE_RESET_RESP_OFFSET_MESSAGEID                   	1 //size 1 byte(s)
#define MCE_RESET_RESP_OFFSET_STATUS                      	2 //size 1 byte(s)
#define MCE_RESET_RESP_OFFSET_FILLERBYTE1                 	3 //size 1 byte(s)
#define SIZE_MCE_RESET_RESP                               	4


//Definition for MCE_WATCHDOG_CONTROL_REQ
#define MCE_WATCHDOG_CONTROL_REQ                          	0x0C                                                 	
#define MCE_WATCHDOG_CONTROL_REQ_OFFSET_TRANSID           	0 //size 1 byte(s)
#define MCE_WATCHDOG_CONTROL_REQ_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define MCE_WATCHDOG_CONTROL_REQ_OFFSET_ACTION            	2 //size 1 byte(s)
#define MCE_WATCHDOG_CONTROL_REQ_OFFSET_FILLERBYTE1       	3 //size 1 byte(s)
#define SIZE_MCE_WATCHDOG_CONTROL_REQ                     	4


//Definition for MCE_WATCHDOG_CONTROL_RESP
#define MCE_WATCHDOG_CONTROL_RESP                         	0x0D                                                 	
#define MCE_WATCHDOG_CONTROL_RESP_OFFSET_TRANSID          	0 //size 1 byte(s)
#define MCE_WATCHDOG_CONTROL_RESP_OFFSET_MESSAGEID        	1 //size 1 byte(s)
#define MCE_WATCHDOG_CONTROL_RESP_OFFSET_STATUS           	2 //size 1 byte(s)
#define MCE_WATCHDOG_CONTROL_RESP_OFFSET_FILLERBYTE1      	3 //size 1 byte(s)
#define SIZE_MCE_WATCHDOG_CONTROL_RESP                    	4


//Definition for MCE_SLEEP_CONTROL_REQ
#define MCE_SLEEP_CONTROL_REQ                             	0x0E                                                 	
#define MCE_SLEEP_CONTROL_REQ_OFFSET_TRANSID              	0 //size 1 byte(s)
#define MCE_SLEEP_CONTROL_REQ_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define MCE_SLEEP_CONTROL_REQ_OFFSET_TYPE                 	2 //size 1 byte(s)
#define MCE_SLEEP_CONTROL_REQ_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_MCE_SLEEP_CONTROL_REQ                        	4


//Definition for MCE_SLEEP_CONTROL_RESP
#define MCE_SLEEP_CONTROL_RESP                            	0x0F                                                 	
#define MCE_SLEEP_CONTROL_RESP_OFFSET_TRANSID             	0 //size 1 byte(s)
#define MCE_SLEEP_CONTROL_RESP_OFFSET_MESSAGEID           	1 //size 1 byte(s)
#define MCE_SLEEP_CONTROL_RESP_OFFSET_STATUS              	2 //size 1 byte(s)
#define MCE_SLEEP_CONTROL_RESP_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_MCE_SLEEP_CONTROL_RESP                       	4


//Definition for MCE_DVFS_CONTROL_REQ
#define MCE_DVFS_CONTROL_REQ                              	0x10                                              	
#define MCE_DVFS_CONTROL_REQ_OFFSET_TRANSID               	0 //size 1 byte(s)
#define MCE_DVFS_CONTROL_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define MCE_DVFS_CONTROL_REQ_OFFSET_TYPE                  	2 //size 1 byte(s)
#define MCE_DVFS_CONTROL_REQ_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_MCE_DVFS_CONTROL_REQ                         	4


//Definition for MCE_DVFS_CONTROL_RESP
#define MCE_DVFS_CONTROL_RESP                             	0x11                                              	
#define MCE_DVFS_CONTROL_RESP_OFFSET_TRANSID              	0 //size 1 byte(s)
#define MCE_DVFS_CONTROL_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define MCE_DVFS_CONTROL_RESP_OFFSET_STATUS               	2 //size 1 byte(s)
#define MCE_DVFS_CONTROL_RESP_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_MCE_DVFS_CONTROL_RESP                        	4

#endif