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
* Name      : PN_AT_MODEM [ 0x8E ] Resource AT Modem Server
* Version   : 003.001
*
*/


#ifndef _SYMBIAN_ISI_AT_MODEMISI_H_
#define _SYMBIAN_ISI_AT_MODEMISI_H_

#define AT_MODEM_SERVER_ISI_VERSION_Z	 3
#define AT_MODEM_SERVER_ISI_VERSION_Y	 1


//CONSTANTS
#define AT_MODEM_ANY_SIZE                                 	0x01                                                 	


//TABLES

//ConstantTable for AT_MODEM_RESULT_STATUS
#define AT_MODEM_RESULT_ERROR                             	0x00                                                 	
#define AT_MODEM_RESULT_OK                                	0x01                                                 	

//ConstantTable for AT_MODEM_FLOW_CONTROL
#define AT_MODEM_FLOW_CONTROL_NONE                        	0x00                                                 	
#define AT_MODEM_FLOW_CONTROL_SW                          	0x01                                                 	
#define AT_MODEM_FLOW_CONTROL_HW                          	0x02                                                 	

//ConstantTable for AT_MODEM_TYPE_OF_SIGNAL_DETECTED
#define AT_MODEM_SIGNAL_DTR_OFF                           	0x00                                                 	
#define AT_MODEM_SIGNAL_DTR_ON                            	0x01                                                 	
#define AT_MODEM_SIGNAL_ESCAPE_DETECTED                   	0x02                                                 	

//ConstantTable for AT_MODEM_EXTRACT_STATUS
#define AT_MODEM_NO_EXTRACT                               	0x00                                                 	
#define AT_MODEM_WITH_EXTRACT                             	0x01                                                 	

//ConstantTable for AT_MODEM_PARITY_STATUS
#define AT_MODEM_PARITY_OFF                               	0x00                                                 	
#define AT_MODEM_PARITY_ON                                	0x01                                                 	

//ConstantTable for AT_MODEM_PARITY_BIT
#define AT_MODEM_PARITY_BIT_ODD                           	0x00                                                 	
#define AT_MODEM_PARITY_BIT_UNKNOWN                       	0x01                                                 	
#define AT_MODEM_PARITY_BIT_EVEN                          	0x02                                                 	
#define AT_MODEM_PARITY_BIT_NONE                          	0x03                                                 	
#define AT_MODEM_PARITY_BIT_SPACE                         	0x04                                                 	
#define AT_MODEM_PARITY_BIT_MARK                          	0x05                                                 	

//ConstantTable for AT_MODEM_AUTOBAUD_STATUS
#define AT_MODEM_AUTOBAUD_OFF                             	0x00                                                 	
#define AT_MODEM_AUTOBAUD_ON                              	0x01                                                 	

//ConstantTable for AT_MODEM_DCD_STATUS
#define AT_MODEM_DCD_OFF                                  	0x00                                                 	
#define AT_MODEM_DCD_ON                                   	0x01                                                 	

//ConstantTable for AT_MODEM_DSR_STATUS
#define AT_MODEM_DSR_OFF                                  	0x00                                                 	
#define AT_MODEM_DSR_ON                                   	0x01                                                 	

//ConstantTable for AT_MODEM_ECHO_STATUS
#define AT_MODEM_ECHO_OFF                                 	0x00                                                 	
#define AT_MODEM_ECHO_ON                                  	0x01                                                 	

//ConstantTable for AT_MODEM_REDIRECT_RESULT_STATUS
#define AT_MODEM_REDIRECT_RESULT_ERROR                    	0x00                                                 	
#define AT_MODEM_REDIRECT_RESULT_OK                       	0x01                                                 	

//ConstantTable for AT_MODEM_DTE_RATE
#define AT_MODEM_CONST_DTE_RATE_AUTOBAUDING               	0x00                                                 	
#define AT_MODEM_CONST_DTE_RATE_300                       	0x01                                                 	
#define AT_MODEM_CONST_DTE_RATE_600                       	0x02                                                 	
#define AT_MODEM_CONST_DTE_RATE_1200                      	0x03                                                 	
#define AT_MODEM_CONST_DTE_RATE_2400                      	0x04                                                 	
#define AT_MODEM_CONST_DTE_RATE_4800                      	0x05                                                 	
#define AT_MODEM_CONST_DTE_RATE_7200                      	0x06                                                 	
#define AT_MODEM_CONST_DTE_RATE_9600                      	0x07                                                 	
#define AT_MODEM_CONST_DTE_RATE_14400                     	0x08                                                 	
#define AT_MODEM_CONST_DTE_RATE_19200                     	0x09                                                 	
#define AT_MODEM_CONST_DTE_RATE_28800                     	0x0A                                                 	
#define AT_MODEM_CONST_DTE_RATE_38400                     	0x0B                                                 	
#define AT_MODEM_CONST_DTE_RATE_57600                     	0x0C                                                 	
#define AT_MODEM_CONST_DTE_RATE_115200                    	0x0D                                                 	
#define AT_MODEM_CONST_DTE_RATE_230400                    	0x0E                                                 	
#define AT_MODEM_CONST_DTE_RATE_460800                    	0x0F                                                 	
#define AT_MODEM_CONST_DTE_RATE_1M                        	0x10                                              	
#define AT_MODEM_CONST_DTE_RATE_2M                        	0x11                                              	
#define AT_MODEM_CONST_DTE_RATE_4M                        	0x12                                              	
#define AT_MODEM_CONST_DTE_RATE_UNDEFINED                 	0x13                                              	


//SUBBLOCKS


//Definition for AT_MODEM_SB_DTE_PARAMS
#define AT_MODEM_SB_DTE_PARAMS                            	0x00                                                 	
#define AT_MODEM_SB_DTE_PARAMS_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define AT_MODEM_SB_DTE_PARAMS_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define AT_MODEM_SB_DTE_PARAMS_OFFSET_PARITY              	2 //size 1 byte(s)
#define AT_MODEM_SB_DTE_PARAMS_OFFSET_DATABITS            	3 //size 1 byte(s)
#define AT_MODEM_SB_DTE_PARAMS_OFFSET_STOPBITS            	4 //size 1 byte(s)
#define AT_MODEM_SB_DTE_PARAMS_OFFSET_DTERATE             	5 //size 1 byte(s)
#define AT_MODEM_SB_DTE_PARAMS_OFFSET_FILLERBYTE1         	6 //size 2 byte(s)
#define SIZE_AT_MODEM_SB_DTE_PARAMS                       	8


//Definition for AT_MODEM_SB_FLOW_CTRL
#define AT_MODEM_SB_FLOW_CTRL                             	0x01                                                 	
#define AT_MODEM_SB_FLOW_CTRL_OFFSET_SUBBLOCKID           	0 //size 1 byte(s)
#define AT_MODEM_SB_FLOW_CTRL_OFFSET_SUBBLOCKLENGTH       	1 //size 1 byte(s)
#define AT_MODEM_SB_FLOW_CTRL_OFFSET_FLOWCTRLOUTWARDDCE   	2 //size 1 byte(s)
#define AT_MODEM_SB_FLOW_CTRL_OFFSET_FLOWCTRLINWARDDTE    	3 //size 1 byte(s)
#define AT_MODEM_SB_FLOW_CTRL_OFFSET_EXTRACT              	4 //size 1 byte(s)
#define AT_MODEM_SB_FLOW_CTRL_OFFSET_FILLERBYTE1          	5 //size 3 byte(s)
#define SIZE_AT_MODEM_SB_FLOW_CTRL                        	8


//Definition for AT_MODEM_SB_ESCAPE_PARAMS
#define AT_MODEM_SB_ESCAPE_PARAMS                         	0x02                                                 	
#define AT_MODEM_SB_ESCAPE_PARAMS_OFFSET_SUBBLOCKID       	0 //size 1 byte(s)
#define AT_MODEM_SB_ESCAPE_PARAMS_OFFSET_SUBBLOCKLENGTH   	1 //size 1 byte(s)
#define AT_MODEM_SB_ESCAPE_PARAMS_OFFSET_ESCAPECHAR       	2 //size 1 byte(s)
#define AT_MODEM_SB_ESCAPE_PARAMS_OFFSET_ESCAPEGUARDPERIOD	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SB_ESCAPE_PARAMS                    	4


//Definition for AT_MODEM_SB_AUTOBAUD
#define AT_MODEM_SB_AUTOBAUD                              	0x03                                                 	
#define AT_MODEM_SB_AUTOBAUD_OFFSET_SUBBLOCKID            	0 //size 1 byte(s)
#define AT_MODEM_SB_AUTOBAUD_OFFSET_SUBBLOCKLENGTH        	1 //size 1 byte(s)
#define AT_MODEM_SB_AUTOBAUD_OFFSET_PARITY                	2 //size 1 byte(s)
#define AT_MODEM_SB_AUTOBAUD_OFFSET_AUTOBAUD              	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SB_AUTOBAUD                         	4


//Definition for AT_MODEM_SB_DTE_SIGNALS
#define AT_MODEM_SB_DTE_SIGNALS                           	0x04                                                 	
#define AT_MODEM_SB_DTE_SIGNALS_OFFSET_SUBBLOCKID         	0 //size 1 byte(s)
#define AT_MODEM_SB_DTE_SIGNALS_OFFSET_SUBBLOCKLENGTH     	1 //size 1 byte(s)
#define AT_MODEM_SB_DTE_SIGNALS_OFFSET_DCDSIGNAL          	2 //size 1 byte(s)
#define AT_MODEM_SB_DTE_SIGNALS_OFFSET_DSRSIGNAL          	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SB_DTE_SIGNALS                      	4


//Definition for AT_MODEM_SB_ECHO_VALUE
#define AT_MODEM_SB_ECHO_VALUE                            	0x05                                                 	
#define AT_MODEM_SB_ECHO_VALUE_OFFSET_SUBBLOCKID          	0 //size 1 byte(s)
#define AT_MODEM_SB_ECHO_VALUE_OFFSET_SUBBLOCKLENGTH      	1 //size 1 byte(s)
#define AT_MODEM_SB_ECHO_VALUE_OFFSET_ECHOVALUE           	2 //size 1 byte(s)
#define AT_MODEM_SB_ECHO_VALUE_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SB_ECHO_VALUE                       	4


//Definition for AT_MODEM_SB_SIGNAL_DETECTED
#define AT_MODEM_SB_SIGNAL_DETECTED                       	0x06                                                 	
#define AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_SUBBLOCKID     	0 //size 1 byte(s)
#define AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_SUBBLOCKLENGTH 	1 //size 1 byte(s)
#define AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_TYPEOFSIGNALDETECTED	2 //size 1 byte(s)
#define AT_MODEM_SB_SIGNAL_DETECTED_OFFSET_FILLERBYTE1    	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SB_SIGNAL_DETECTED                  	4


//MESSAGES


//Definition for AT_MODEM_CONNECT_REQ
#define AT_MODEM_CONNECT_REQ                              	0x00                                                 	
#define AT_MODEM_CONNECT_REQ_OFFSET_TID                   	0 //size 1 byte(s)
#define AT_MODEM_CONNECT_REQ_OFFSET_MESSAGEID             	1 //size 1 byte(s)
#define AT_MODEM_CONNECT_REQ_OFFSET_DTEID                 	2 //size 1 byte(s)
#define AT_MODEM_CONNECT_REQ_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_AT_MODEM_CONNECT_REQ                         	4


//Definition for AT_MODEM_CONNECT_RESP
#define AT_MODEM_CONNECT_RESP                             	0x01                                                 	
#define AT_MODEM_CONNECT_RESP_OFFSET_TID                  	0 //size 1 byte(s)
#define AT_MODEM_CONNECT_RESP_OFFSET_MESSAGEID            	1 //size 1 byte(s)
#define AT_MODEM_CONNECT_RESP_OFFSET_DTEID                	2 //size 1 byte(s)
#define AT_MODEM_CONNECT_RESP_OFFSET_RESULT               	3 //size 1 byte(s)
#define SIZE_AT_MODEM_CONNECT_RESP                        	4


//Definition for AT_MODEM_DISCONNECT_REQ
#define AT_MODEM_DISCONNECT_REQ                           	0x02                                                 	
#define AT_MODEM_DISCONNECT_REQ_OFFSET_TID                	0 //size 1 byte(s)
#define AT_MODEM_DISCONNECT_REQ_OFFSET_MESSAGEID          	1 //size 1 byte(s)
#define AT_MODEM_DISCONNECT_REQ_OFFSET_DTEID              	2 //size 1 byte(s)
#define AT_MODEM_DISCONNECT_REQ_OFFSET_FILLERBYTE1        	3 //size 1 byte(s)
#define SIZE_AT_MODEM_DISCONNECT_REQ                      	4


//Definition for AT_MODEM_DISCONNECT_RESP
#define AT_MODEM_DISCONNECT_RESP                          	0x03                                                 	
#define AT_MODEM_DISCONNECT_RESP_OFFSET_TID               	0 //size 1 byte(s)
#define AT_MODEM_DISCONNECT_RESP_OFFSET_MESSAGEID         	1 //size 1 byte(s)
#define AT_MODEM_DISCONNECT_RESP_OFFSET_DTEID             	2 //size 1 byte(s)
#define AT_MODEM_DISCONNECT_RESP_OFFSET_RESULT            	3 //size 1 byte(s)
#define SIZE_AT_MODEM_DISCONNECT_RESP                     	4


//Definition for AT_MODEM_CMD_REQ
#define AT_MODEM_CMD_REQ                                  	0x04                                                 	
#define AT_MODEM_CMD_REQ_OFFSET_TID                       	0 //size 1 byte(s)
#define AT_MODEM_CMD_REQ_OFFSET_MESSAGEID                 	1 //size 1 byte(s)
#define AT_MODEM_CMD_REQ_OFFSET_DTEID                     	2 //size 1 byte(s)
#define AT_MODEM_CMD_REQ_OFFSET_FILLERBYTE1               	3 //size 1 byte(s)
#define AT_MODEM_CMD_REQ_OFFSET_LENGTH                    	4 //size 2 byte(s)
#define AT_MODEM_CMD_REQ_OFFSET_CMDLINEBUFFER             	6 //size 1 byte(s)
#define AT_MODEM_CMD_REQ_OFFSET_FILLERBYTE2               	7 //size 1 byte(s)
#define SIZE_AT_MODEM_CMD_REQ                             	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for AT_MODEM_CMD_RESP
#define AT_MODEM_CMD_RESP                                 	0x05                                                 	
#define AT_MODEM_CMD_RESP_OFFSET_TID                      	0 //size 1 byte(s)
#define AT_MODEM_CMD_RESP_OFFSET_MESSAGEID                	1 //size 1 byte(s)
#define AT_MODEM_CMD_RESP_OFFSET_DTEID                    	2 //size 1 byte(s)
#define AT_MODEM_CMD_RESP_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define AT_MODEM_CMD_RESP_OFFSET_LENGTH                   	4 //size 2 byte(s)
#define AT_MODEM_CMD_RESP_OFFSET_CMDLINEBUFFER            	6 //size 1 byte(s)
#define AT_MODEM_CMD_RESP_OFFSET_FILLERBYTE2              	7 //size 1 byte(s)
#define SIZE_AT_MODEM_CMD_RESP                            	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for AT_MODEM_DATA_REDIRECT_IND
#define AT_MODEM_DATA_REDIRECT_IND                        	0x06                                                 	
#define AT_MODEM_DATA_REDIRECT_IND_OFFSET_TID             	0 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_IND_OFFSET_MESSAGEID       	1 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_IND_OFFSET_DTEID           	2 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_IND_OFFSET_DEVICEID        	3 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_IND_OFFSET_SERVERID        	4 //size 2 byte(s)
#define AT_MODEM_DATA_REDIRECT_IND_OFFSET_FILLERBYTE1     	6 //size 2 byte(s)
#define SIZE_AT_MODEM_DATA_REDIRECT_IND                   	8


//Definition for AT_MODEM_UNSOLICITED_DATA_IND
#define AT_MODEM_UNSOLICITED_DATA_IND                     	0x07                                                 	
#define AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_TID          	0 //size 1 byte(s)
#define AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_DTEID        	2 //size 1 byte(s)
#define AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_FILLERBYTE1  	3 //size 1 byte(s)
#define AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_LENGTH       	4 //size 2 byte(s)
#define AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_UNSOLICITEDDATABUFFER	6 //size 1 byte(s)
#define AT_MODEM_UNSOLICITED_DATA_IND_OFFSET_FILLERBYTE2  	7 //size 1 byte(s)
#define SIZE_AT_MODEM_UNSOLICITED_DATA_IND                	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for AT_MODEM_INTERMEDIATE_DATA_IND
#define AT_MODEM_INTERMEDIATE_DATA_IND                    	0x08                                                 	
#define AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_TID         	0 //size 1 byte(s)
#define AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_MESSAGEID   	1 //size 1 byte(s)
#define AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_DTEID       	2 //size 1 byte(s)
#define AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_FILLERBYTE1 	3 //size 1 byte(s)
#define AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_LENGTH      	4 //size 2 byte(s)
#define AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_INTERMEDIATEDATABUFFER	6 //size 1 byte(s)
#define AT_MODEM_INTERMEDIATE_DATA_IND_OFFSET_FILLERBYTE2 	7 //size 1 byte(s)
#define SIZE_AT_MODEM_INTERMEDIATE_DATA_IND               	8
//NOTE: Definition contains variable size field(s). SIZE_ and offsets following variable field cannot be defined correctly.
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for AT_MODEM_SIGNAL_DETECTED_REQ
#define AT_MODEM_SIGNAL_DETECTED_REQ                      	0x09                                                 	
#define AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_TID           	0 //size 1 byte(s)
#define AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_MESSAGEID     	1 //size 1 byte(s)
#define AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_DTEID         	2 //size 1 byte(s)
#define AT_MODEM_SIGNAL_DETECTED_REQ_OFFSET_NOOFSUBBLOCKS 	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SIGNAL_DETECTED_REQ                 	4


//Definition for AT_MODEM_SIGNAL_DETECTED_RESP
#define AT_MODEM_SIGNAL_DETECTED_RESP                     	0x0A                                                 	
#define AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_TID          	0 //size 1 byte(s)
#define AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_MESSAGEID    	1 //size 1 byte(s)
#define AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_DTEID        	2 //size 1 byte(s)
#define AT_MODEM_SIGNAL_DETECTED_RESP_OFFSET_RESULT       	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SIGNAL_DETECTED_RESP                	4


//Definition for AT_MODEM_SIGNAL_IND
#define AT_MODEM_SIGNAL_IND                               	0x0B                                                 	
#define AT_MODEM_SIGNAL_IND_OFFSET_TID                    	0 //size 1 byte(s)
#define AT_MODEM_SIGNAL_IND_OFFSET_MESSAGEID              	1 //size 1 byte(s)
#define AT_MODEM_SIGNAL_IND_OFFSET_DTEID                  	2 //size 1 byte(s)
#define AT_MODEM_SIGNAL_IND_OFFSET_NOOFSUBBLOCKS          	3 //size 1 byte(s)
#define SIZE_AT_MODEM_SIGNAL_IND                          	4


//Definition for AT_MODEM_DATA_REDIRECT_RESULT_REQ
#define AT_MODEM_DATA_REDIRECT_RESULT_REQ                 	0x0C                                                 	
#define AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_TID      	0 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_DTEID    	2 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_RESULT_REQ_OFFSET_RESULT   	3 //size 1 byte(s)
#define SIZE_AT_MODEM_DATA_REDIRECT_RESULT_REQ            	4


//Definition for AT_MODEM_DATA_REDIRECT_RESULT_RESP
#define AT_MODEM_DATA_REDIRECT_RESULT_RESP                	0x0D                                                 	
#define AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_TID     	0 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_MESSAGEID	1 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_DTEID   	2 //size 1 byte(s)
#define AT_MODEM_DATA_REDIRECT_RESULT_RESP_OFFSET_RESULT  	3 //size 1 byte(s)
#define SIZE_AT_MODEM_DATA_REDIRECT_RESULT_RESP           	4

#endif