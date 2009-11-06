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



#ifndef _TERMINAL_PROFILE_CONF_H_
#define _TERMINAL_PROFILE_CONF_H_

//first byte
#define TERM_PROFILE_PROFILE_DOWNLOAD                       0x01
#define TERM_PROFILE_SMS_PP_DD                              0x02
#define TERM_PROFILE_CB_DD                                  0x04
#define TERM_PROFILE_MENU_SELECTION                         0x08
#define TERM_PROFILE_SMS_PP_DD_9EXX                         0x10
#define TERM_PROFILE_TIMER_MANAGEMENT_EXPIRATION            0x20
#define TERM_PROFILE_CC_USSD_STRING                         0x40
#define TERM_PROFILE_CC_ENVELOPE_DURING_REDIAL              0x80

//second byte
#define TERM_PROFILE_COMMAND_RESULT                         0x01
#define TERM_PROFILE_CC_BY_SIM                              0x02
#define TERM_PROFILE_CC_CELL_ID                             0x04
#define TERM_PROFILE_MO_SMS_CONTROL                         0x08
#define TERM_PROFILE_CC_ALPHA_ID                            0x10
#define TERM_PROFILE_UCS2_ENTRY_SUPP                        0x20
#define TERM_PROFILE_UCS2_DISP_SUPP                         0x40
#define TERM_PROFILE_DISP_TEXT_EXTENSION                    0x80

//third byte
#define TERM_PROFILE_DISP_TEXT                              0x01
#define TERM_PROFILE_GET_INKEY                              0x02
#define TERM_PROFILE_GET_INPUT                              0x04
#define TERM_PROFILE_MORE_TIME                              0x08
#define TERM_PROFILE_PLAY_TONE                              0x10
#define TERM_PROFILE_POLL_INTERVAL                          0x20
#define TERM_PROFILE_POLLING_OFF                            0x40
#define TERM_PROFILE_REFRESH                                0x80

//fourth byte
#define TERM_PROFILE_SELECT_ITEM                            0x01
#define TERM_PROFILE_SEND_SMS                               0x02
#define TERM_PROFILE_SEND_SS                                0x04
#define TERM_PROFILE_SEND_USSD                              0x08
#define TERM_PROFILE_SETUP_CALL                             0x10
#define TERM_PROFILE_SETUP_MENU                             0x20
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_IMEI                0x40
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_NMR                 0x80

//fifth byte
#define TERM_PROFILE_SET_UP_EVENT_LIST                      0x01
#define TERM_PROFILE_MT_CALL                                0x02
#define TERM_PROFILE_CALL_CONNECTED                         0x04
#define TERM_PROFILE_CALL_DISCONNECTED                      0x08
#define TERM_PROFILE_LOCATION_STATUS                        0x10
#define TERM_PROFILE_USER_ACTIVITY                          0x20
#define TERM_PROFILE_IDLE_SCREEN_AVAIL                      0x40
#define TERM_PROFILE_CARD_READER_STATUS                     0x00

//sixth byte
#define TERM_PROFILE_LANGUAGE_SELECTION                     0x01
#define TERM_PROFILE_BROWSER_TERMINATION                    0x02
#define TERM_PROFILE_DATA_AVAILABLE                         0x04
#define TERM_PROFILE_CHANNEL_STATUS                         0x08
#define TERM_PROFILE_ACCESS_TECHNOLOGY                      0x00
#define TERM_PROFILE_DISPLAY_PARAMETERS_CHANGED             0x00
#define TERM_PROFILE_LOCAL_CONNECTION                       0x00
#define TERM_PROFILE_NETWORK_SEARCH_MODE                    0x00

//seventh byte
#define TERM_PROFILE_POWER_ON_CARD                          0x00
#define TERM_PROFILE_POWER_OFF_CARD                         0x00
#define TERM_PROFILE_PERFORM_CARD_APDU                      0x00
#define TERM_PROFILE_GET_READER_STATUS                      0x00
#define TERM_PROFILE_GET_READER_STATUS_ID                   0x00

//eighth byte
#define TERM_PROFILE_TIMER_MANAGEMENT_START_STOP            0x01
#define TERM_PROFILE_TIMER_MANAGEMENT_GET_VALUE             0x02
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_DATE_TIME           0x04
#define TERM_PROFILE_GET_INKEY_BINARY                       0x08
#define TERM_PROFILE_IDLE_MODE_TEXT                         0x10
#define TERM_PROFILE_RUN_AT_COMMAND                         0x00
#define TERM_PROFILE_SETUP_CALL_SECOND_ALPHA                0x40
#define TERM_PROFILE_CC_SECOND_CCP                          0x80

//ninth byte
#define TERM_PROFILE_SUSTAINED_DISPLAY_TEXT                 0x01
#define TERM_PROFILE_SEND_DTMF                              0x02
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_BCCH                0x04
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_LANGUAGE            0x08
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_TIMING_ADVANCE      0x10
#define TERM_PROFILE_LANGUAGE_NOTIFICATION                  0x20
#define TERM_PROFILE_LAUNCH_BROWSER                         0x40
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_ACCESS_TECHNOLOGY   0x00

//tenth byte
#define TERM_PROFILE_SELECT_ITEM_SOFT_KEYS                  0x00
#define TERM_PROFILE_SETUP_MENU_SOFT_KEYS                   0x00

//eleventh byte
#define TERM_PROFILE_MAX_NO_SOFT_KEYS                       0x00

//twelfth byte
#define TERM_PROFILE_OPEN_CHANNEL                           0x01
#define TERM_PROFILE_CLOSE_CHANNEL                          0x02
#define TERM_PROFILE_RECEIVE_DATA                           0x04
#define TERM_PROFILE_SEND_DATA                              0x08
#define TERM_PROFILE_GET_CHANNEL_STATUS                     0x10
#define TERM_PROFILE_SERVICE_SEARCH                         0x00
#define TERM_PROFILE_GET_SERVICE_INFORMAION                 0x00
#define TERM_PROFILE_DECLARE_SERVICE                        0x00

//thirteenth byte
#define TERM_PROFILE_CSD_SUPPORT                            0x00
#define TERM_PROFILE_GPRS_SUPPORT                           0x02
#define TERM_PROFILE_BLUETOOTH_SUPPORT                      0x00
#define TERM_PROFILE_IRDA_SUPPORT                           0x00
#define TERM_PROFILE_RS232_SUPPORT                          0x00
#define TERM_PROFILE_NO_CHANNELS                            0x07

//fourteenth byte
#define TERM_PROFILE_NO_CHARS_DOWN                          0x05
#define TERM_PROFILE_SCREEN_SIZE_PARAM                      0x80

//fifteenth byte
#define TERM_PROFILE_NO_CHARS_ACROSS                        0x0F
#define TERM_PROFILE_VAR_SIZE_FONTS                         0x00

//sixteenth byte
#define TERM_PROFILE_DISPLAY_RESIZE                         0x00
#define TERM_PROFILE_TEXT_WRAPPING                          0x00
#define TERM_PROFILE_TEXT_SCROLLING                         0x00
#define TERM_PROFILE_WIDTH_REDUCTION_IN_MENU                0x00

//seventeenth byte
#define TERM_PROFILE_TCP                                    0x01
#define TERM_PROFILE_UDP                                    0x02

//eighteenth byte
#define TERM_PROFILE_DISPLAY_TXT_TIMEOUT                    0x00
#define TERM_PROFILE_GET_INKEY_TIMEOUT_HELP                 0x00
#define TERM_PROFILE_USB_SUPPORT                            0x00
#define TERM_PROFILE_GET_INKEY_TIMEOUT                      0x00
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_ESN_OF_ME           0x00
#define TERM_PROFILE_CALL_CONTROL_ON_GPRS                   0x00
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_IMEISV              0x00
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_SEARCH_MODE_CHANGE  0x00

//nineteenth byte
#define TERM_PROFILE_PROTOCOL_VERSION                       0x00

//twenty-first byte
#define TERM_PROFILE_WML                                    0x00
#define TERM_PROFILE_XHTML                                  0x00
#define TERM_PROFILE_HTML                                   0x00
#define TERM_PROFILE_CHTML                                  0x00

//twenty-second byte
#define TERM_PROFILE_UTRAN_PS_EXTENDED                      0x00
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_BATTERY_STATE       0x00
#define TERM_PROFILE_PLAY_TONE_MELODY_TONES                 0x00
#define TERM_PROFILE_SETUP_CALL_MULITI_MEDIA                0x00
#define TERM_PROFILE_TOOLKIT_GBA                            0x00
#define TERM_PROFILE_MULTI_MEDIA_MESSAGE                    0x00
#define TERM_PROFILE_SUBMIT_MULTI_MEDIA_MESSAGE             0x00
#define TERM_PROFILE_DISPLAY_MULTI_MEDIA_MESSAGE            0x00

//twenty-third byte
#define TERM_PROFILE_SET_FRAMES                             0x00
#define TERM_PROFILE_GET_FRAMES_STATUS                      0x00
#define TERM_PROFILE_MMS_NOTIFICATION_DOWNLOAD              0x00
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_MEID                0x00
#define TERM_PROFILE_PROVIDE_LOCAL_INFO_UTRAN_NMR           0x00
#define TERM_PROFILE_USSD_DOWNLOAD                          0x00

#endif

// End of file
