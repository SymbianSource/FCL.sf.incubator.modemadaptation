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
* Name      : PN_PIPE [ 0xD9 ] Resource CM Pipe Service
* Version   : 002.003
*
*/


#ifndef _SYMBIAN_ISI_PIPEISI_H_
#define _SYMBIAN_ISI_PIPEISI_H_

#define CM_PIPE_SERVICE_ISI_VERSION_Z	 2
#define CM_PIPE_SERVICE_ISI_VERSION_Y	 3


//CONSTANTS
#define CM_PIPE_ANY_SIZE                                  	0x01                                                 	


//STRUCTURES


//Definition for CM_PIPE_CREATE_REQ
#define CM_PIPE_CREATE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_STATEAFTERCREATION      	2 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_PRIORITY                	3 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FIRSTPEPDEV             	4 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FIRSTPEPOBJ             	5 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FIRSTPEPTYPE            	6 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_FILLERBYTE1             	7 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SECONDPEPDEV            	8 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SECONDPEPOBJ            	9 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_SECONDPEPTYPE           	10 //size 1 byte(s)
#define CM_PIPE_CREATE_REQ_OFFSET_NSB                     	11 //size 1 byte(s)
#define SIZE_CM_PIPE_CREATE_REQ                           	12


//Definition for CM_PIPE_CREATE_RESP
#define CM_PIPE_CREATE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_CREATE_RESP_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_CREATE_RESP                          	6


//Definition for CM_PIPE_CREATED_IND
#define CM_PIPE_CREATED_IND_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_CREATED_IND_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_CREATED_IND_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_CREATED_IND_OFFSET_NSB                    	3 //size 1 byte(s)
#define SIZE_CM_PIPE_CREATED_IND                          	4


//Definition for CM_PEP_CONNECT_REQ
#define CM_PEP_CONNECT_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_STATEAFTERCONNECT       	3 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_OTHERPEPTYPE            	4 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_FILLERBYTE1             	5 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_FILLERBYTE2             	6 //size 1 byte(s)
#define CM_PEP_CONNECT_REQ_OFFSET_NSB                     	7 //size 1 byte(s)
#define SIZE_CM_PEP_CONNECT_REQ                           	8


//Definition for CM_PEP_CONNECT_RESP
#define CM_PEP_CONNECT_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_FILLERBYTE1            	4 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_FILLERBYTE2            	5 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_FILLERBYTE3            	6 //size 1 byte(s)
#define CM_PEP_CONNECT_RESP_OFFSET_NSB                    	7 //size 1 byte(s)
#define SIZE_CM_PEP_CONNECT_RESP                          	8


//Definition for CM_PEP_CTRL_REQ
#define CM_PEP_CTRL_REQ_OFFSET_UTID                       	0 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_SUBFUNCTION                	1 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PIPEHANDLE                 	2 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PEPTYPE                    	3 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PEPCTRLID                  	4 //size 1 byte(s)
#define CM_PEP_CTRL_REQ_OFFSET_PEPCTRLDATA                	5 //size 1 byte(s)
#define SIZE_CM_PEP_CTRL_REQ                              	6
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CM_PEP_CTRL_RESP
#define CM_PEP_CTRL_RESP_OFFSET_UTID                      	0 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_SUBFUNCTION               	1 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_PIPEHANDLE                	2 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_PEPCTRLID                 	3 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_PEPTYPE                   	4 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_ERRORCODE                 	5 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_FILLERBYTE1               	6 //size 1 byte(s)
#define CM_PEP_CTRL_RESP_OFFSET_FILLERBYTE2               	7 //size 1 byte(s)
#define SIZE_CM_PEP_CTRL_RESP                             	8


//Definition for CM_PIPE_ENABLE_REQ
#define CM_PIPE_ENABLE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_ENABLE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_ENABLE_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PIPE_ENABLE_REQ_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_CM_PIPE_ENABLE_REQ                           	4


//Definition for CM_PIPE_ENABLE_RESP
#define CM_PIPE_ENABLE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_ENABLE_RESP_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_ENABLE_RESP                          	6


//Definition for CM_PIPE_ENABLED_IND
#define CM_PIPE_ENABLED_IND_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_ENABLED_IND_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_ENABLED_IND_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_ENABLED_IND_OFFSET_FILLERBYTE1            	3 //size 1 byte(s)
#define SIZE_CM_PIPE_ENABLED_IND                          	4


//Definition for CM_PEP_ENABLE_REQ
#define CM_PEP_ENABLE_REQ_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PEP_ENABLE_REQ_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PEP_ENABLE_REQ_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PEP_ENABLE_REQ_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_CM_PEP_ENABLE_REQ                            	4


//Definition for CM_PEP_ENABLE_RESP
#define CM_PEP_ENABLE_RESP_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PEP_ENABLE_RESP_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PEP_ENABLE_RESP_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PEP_ENABLE_RESP_OFFSET_ERRORCODE               	3 //size 1 byte(s)
#define SIZE_CM_PEP_ENABLE_RESP                           	4


//Definition for CM_PIPE_DATA
#define CM_PIPE_DATA_OFFSET_UTID                          	0 //size 1 byte(s)
#define CM_PIPE_DATA_OFFSET_SUBFUNCTION                   	1 //size 1 byte(s)
#define CM_PIPE_DATA_OFFSET_PIPEHANDLE                    	2 //size 1 byte(s)
#define CM_PIPE_DATA_OFFSET_DATA                          	3 //size 1 byte(s)
#define SIZE_CM_PIPE_DATA                                 	4
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CM_PIPE_REMOVED_IND
#define CM_PIPE_REMOVED_IND_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_REMOVED_IND_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_REMOVED_IND                          	6


//Definition for CM_PIPE_STATUS_IND_REQ
#define CM_PIPE_STATUS_IND_REQ_OFFSET_UTID                	0 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_REQ_OFFSET_SUBFUNCTION         	1 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_REQ_OFFSET_PIPEHANDLE          	2 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_REQ_OFFSET_FILLERBYTE1         	3 //size 1 byte(s)
#define SIZE_CM_PIPE_STATUS_IND_REQ                       	4


//Definition for CM_PIPE_STATUS_IND_RESP
#define CM_PIPE_STATUS_IND_RESP_OFFSET_UTID               	0 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_RESP_OFFSET_SUBFUNCTION        	1 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_RESP_OFFSET_PIPEHANDLE         	2 //size 1 byte(s)
#define CM_PIPE_STATUS_IND_RESP_OFFSET_ERRORCODE          	3 //size 1 byte(s)
#define SIZE_CM_PIPE_STATUS_IND_RESP                      	4


//Definition for CM_PEP_STATUS_IND
#define CM_PEP_STATUS_IND_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_PEPTYPE                  	3 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_INDICATIONID             	4 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_FILLERBYTE1              	5 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_FILLERBYTE2              	6 //size 1 byte(s)
#define CM_PEP_STATUS_IND_OFFSET_INDICATIONDATA           	7 //size 1 byte(s)
#define SIZE_CM_PEP_STATUS_IND                            	8
//NOTE: Definition contains array(s) or sequence(s). SIZE_ and offsets following array or sequence might be defined incorrectly.


//Definition for CM_PIPE_RESET_REQ
#define CM_PIPE_RESET_REQ_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PIPE_RESET_REQ_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PIPE_RESET_REQ_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PIPE_RESET_REQ_OFFSET_PIPESTATEAFTERRESET      	3 //size 1 byte(s)
#define SIZE_CM_PIPE_RESET_REQ                            	4


//Definition for CM_PIPE_RESET_RESP
#define CM_PIPE_RESET_RESP_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_ERRORCODE               	3 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_PEP1ERRORCODE           	4 //size 1 byte(s)
#define CM_PIPE_RESET_RESP_OFFSET_PEP2ERRORCODE           	5 //size 1 byte(s)
#define SIZE_CM_PIPE_RESET_RESP                           	6


//Definition for CM_PIPE_RESET_IND
#define CM_PIPE_RESET_IND_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PIPE_RESET_IND_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PIPE_RESET_IND_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PIPE_RESET_IND_OFFSET_FILLERBYTE1              	3 //size 1 byte(s)
#define SIZE_CM_PIPE_RESET_IND                            	4


//Definition for CM_PEP_RESET_REQ
#define CM_PEP_RESET_REQ_OFFSET_UTID                      	0 //size 1 byte(s)
#define CM_PEP_RESET_REQ_OFFSET_SUBFUNCTION               	1 //size 1 byte(s)
#define CM_PEP_RESET_REQ_OFFSET_PIPEHANDLE                	2 //size 1 byte(s)
#define CM_PEP_RESET_REQ_OFFSET_PIPESTATEAFTERRESET       	3 //size 1 byte(s)
#define SIZE_CM_PEP_RESET_REQ                             	4


//Definition for CM_PEP_RESET_RESP
#define CM_PEP_RESET_RESP_OFFSET_UTID                     	0 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_SUBFUNCTION              	1 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_PIPEHANDLE               	2 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_ERRORCODE                	3 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_PEP1ERRORCODE            	4 //size 1 byte(s)
#define CM_PEP_RESET_RESP_OFFSET_PEP2ERRORCODE            	5 //size 1 byte(s)
#define SIZE_CM_PEP_RESET_RESP                            	6


//Definition for CM_PIPE_REMOVE_REQ
#define CM_PIPE_REMOVE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PIPE_REMOVE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PIPE_REMOVE_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PIPE_REMOVE_REQ_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_CM_PIPE_REMOVE_REQ                           	4


//Definition for CM_PIPE_REMOVE_RESP
#define CM_PIPE_REMOVE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_PEP1ERRORCODE          	4 //size 1 byte(s)
#define CM_PIPE_REMOVE_RESP_OFFSET_PEP2ERRORCODE          	5 //size 1 byte(s)
#define SIZE_CM_PIPE_REMOVE_RESP                          	6


//Definition for CM_PEP_DISCONNECT_REQ
#define CM_PEP_DISCONNECT_REQ_OFFSET_UTID                 	0 //size 1 byte(s)
#define CM_PEP_DISCONNECT_REQ_OFFSET_SUBFUNCTION          	1 //size 1 byte(s)
#define CM_PEP_DISCONNECT_REQ_OFFSET_PIPEHANDLE           	2 //size 1 byte(s)
#define CM_PEP_DISCONNECT_REQ_OFFSET_FILLERBYTE1          	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISCONNECT_REQ                        	4


//Definition for CM_PEP_DISCONNECT_RESP
#define CM_PEP_DISCONNECT_RESP_OFFSET_UTID                	0 //size 1 byte(s)
#define CM_PEP_DISCONNECT_RESP_OFFSET_SUBFUNCTION         	1 //size 1 byte(s)
#define CM_PEP_DISCONNECT_RESP_OFFSET_PIPEHANDLE          	2 //size 1 byte(s)
#define CM_PEP_DISCONNECT_RESP_OFFSET_ERRORCODE           	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISCONNECT_RESP                       	4


//Definition for CM_PIPE_REDIRECT_REQ
#define CM_PIPE_REDIRECT_REQ_OFFSET_UTID                  	0 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_STATEAFTERCREATION    	3 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_OLDPEPDEV             	4 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_OLDPEPOBJ             	5 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_OLDPEPTYPE            	6 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_FILLERBYTE1           	7 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPDEV     	8 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPOBJ     	9 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_REPLACEMENTPEPTYPE    	10 //size 1 byte(s)
#define CM_PIPE_REDIRECT_REQ_OFFSET_NSB                   	11 //size 1 byte(s)
#define SIZE_CM_PIPE_REDIRECT_REQ                         	12


//Definition for CM_PIPE_REDIRECT_RESP
#define CM_PIPE_REDIRECT_RESP_OFFSET_UTID                 	0 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_SUBFUNCTION          	1 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_PIPEHANDLE           	2 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_ERRORCODE            	3 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_PEP1ERRORCODE        	4 //size 1 byte(s)
#define CM_PIPE_REDIRECT_RESP_OFFSET_PEP2ERRORCODE        	5 //size 1 byte(s)
#define SIZE_CM_PIPE_REDIRECT_RESP                        	6


//Definition for CM_PIPE_REDIRECTED_IND
#define CM_PIPE_REDIRECTED_IND_OFFSET_UTID                	0 //size 1 byte(s)
#define CM_PIPE_REDIRECTED_IND_OFFSET_SUBFUNCTION         	1 //size 1 byte(s)
#define CM_PIPE_REDIRECTED_IND_OFFSET_PIPEHANDLE          	2 //size 1 byte(s)
#define CM_PIPE_REDIRECTED_IND_OFFSET_NSB                 	3 //size 1 byte(s)
#define SIZE_CM_PIPE_REDIRECTED_IND                       	4


//Definition for CM_PEP_DISABLE_REQ
#define CM_PEP_DISABLE_REQ_OFFSET_UTID                    	0 //size 1 byte(s)
#define CM_PEP_DISABLE_REQ_OFFSET_SUBFUNCTION             	1 //size 1 byte(s)
#define CM_PEP_DISABLE_REQ_OFFSET_PIPEHANDLE              	2 //size 1 byte(s)
#define CM_PEP_DISABLE_REQ_OFFSET_FILLERBYTE1             	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISABLE_REQ                           	4


//Definition for CM_PEP_DISABLE_RESP
#define CM_PEP_DISABLE_RESP_OFFSET_UTID                   	0 //size 1 byte(s)
#define CM_PEP_DISABLE_RESP_OFFSET_SUBFUNCTION            	1 //size 1 byte(s)
#define CM_PEP_DISABLE_RESP_OFFSET_PIPEHANDLE             	2 //size 1 byte(s)
#define CM_PEP_DISABLE_RESP_OFFSET_ERRORCODE              	3 //size 1 byte(s)
#define SIZE_CM_PEP_DISABLE_RESP                          	4


//Definition for CM_PIPE_DISABLED_IND
#define CM_PIPE_DISABLED_IND_OFFSET_UTID                  	0 //size 1 byte(s)
#define CM_PIPE_DISABLED_IND_OFFSET_SUBFUNCTION           	1 //size 1 byte(s)
#define CM_PIPE_DISABLED_IND_OFFSET_PIPEHANDLE            	2 //size 1 byte(s)
#define CM_PIPE_DISABLED_IND_OFFSET_FILLERBYTE1           	3 //size 1 byte(s)
#define SIZE_CM_PIPE_DISABLED_IND                         	4

#endif