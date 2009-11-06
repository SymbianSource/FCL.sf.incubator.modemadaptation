/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef PEP_COMM_H
#define PEP_COMM_H

/*  CONSTANTS  */

/*  These constants are defined in PhoNet Comm PEP Specification  */

/*  COMM PEP Control IDs  */
#define     PEP_COMM_CTRL_ID_PARAM           0x01
#define     PEP_COMM_CTRL_ID_FLOW_CTRL       0x02
#define     PEP_COMM_CTRL_ID_BREAK           0x03
#define     PEP_COMM_CTRL_ID_AUTOBAUD        0x04
#define     PEP_COMM_CTRL_ID_ESCAPE          0x05
#define     PEP_COMM_CTRL_ID_ECHO            0x06
#define     PEP_COMM_CTRL_ID_SEND_PNP        0x07
#define     PEP_COMM_CTRL_ID_DTR_STATUS      0x08

/*  COMM PEP Indication IDs  */
#define     PEP_COMM_IND_ID_PARAM            0x01
#define     PEP_COMM_IND_ID_FLOW_CTRL        0x02
#define     PEP_COMM_IND_ID_V24_CHANGE       0x04
#define     PEP_COMM_IND_ID_BREAK            0x05
#define     PEP_COMM_IND_ID_ESCAPE           0x07
#define     PEP_COMM_IND_ID_PNP              0x08

/*  COMM PEP General Symbol Information  */
#define     PEP_COMM_DATA_BITS_7             0x01
#define     PEP_COMM_DATA_BITS_8             0x02
#define     PEP_COMM_DATA_BITS_5             0x03
#define     PEP_COMM_DATA_BITS_6             0x04

#define     PEP_COMM_STOP_BITS_1             0x01
#define     PEP_COMM_STOP_BITS_2             0x02

#define     PEP_COMM_PARITY_NONE             0x01
#define     PEP_COMM_PARITY_ODD              0x02
#define     PEP_COMM_PARITY_EVEN             0x03
#define     PEP_COMM_PARITY_MARK             0x04
#define     PEP_COMM_PARITY_SPACE            0x05

#define     PEP_COMM_FLOW_CTRL_SW            0x01
#define     PEP_COMM_FLOW_CTRL_HW            0x02
#define     PEP_COMM_FLOW_CTRL_BOTH          0x03
#define     PEP_COMM_FLOW_CTRL_NONE          0x04
#define     PEP_COMM_FLOW_CTRL_EXTRACT       0x05
#define     PEP_COMM_FLOW_CTRL_NO_EXTRACT    0x06

#define     PEP_COMM_DATA_WITH_PARITY        0x01
#define     PEP_COMM_DATA_WITH_NO_PARITY     0x02

#define     PEP_COMM_AUTOBAUD_ON             0x01
#define     PEP_COMM_AUTOBAUD_OFF            0x02

#define     PEP_COMM_BREAK_DESTRUCT          0x01
#define     PEP_COMM_BREAK_EXPEDITED         0x02
#define     PEP_COMM_BREAK_NON_EXPEDITED     0x03
#define     PEP_COMM_BREAK_IGNORE            0x04

#define     PEP_COMM_SIGNAL_DSR_ON           0x01
#define     PEP_COMM_SIGNAL_DSR_OFF          0x02
#define     PEP_COMM_SIGNAL_RI_ON            0x03
#define     PEP_COMM_SIGNAL_RI_OFF           0x04
#define     PEP_COMM_SIGNAL_DTR_ON           0x05
#define     PEP_COMM_SIGNAL_DTR_OFF          0x06
#define     PEP_COMM_SIGNAL_DCD_ON           0x07
#define     PEP_COMM_SIGNAL_DCD_OFF          0x08

#define     PEP_COMM_ECHO_ON                 0x01
#define     PEP_COMM_ECHO_OFF                0x02

enum TDPRate
    {
    EPEP_COMM_RATE_AUTOBAUD = 0x00,
    EPEP_COMM_RATE_300,
    EPEP_COMM_RATE_600,
    EPEP_COMM_RATE_1200,
    EPEP_COMM_RATE_2400,
    EPEP_COMM_RATE_4800,
    EPEP_COMM_RATE_7200,
    EPEP_COMM_RATE_9600,
    EPEP_COMM_RATE_14400,
    EPEP_COMM_RATE_19200,
    EPEP_COMM_RATE_28800,
    EPEP_COMM_RATE_38400,
    EPEP_COMM_RATE_57600,
    EPEP_COMM_RATE_115200,
    EPEP_COMM_RATE_230400,
    EPEP_COMM_RATE_460800,
    EPEP_COMM_RATE_1M,
    EPEP_COMM_RATE_2M,
    EPEP_COMM_RATE_4M,
    EPEP_COMM_RATE_UNDEFINED
    };

/*  DATA TYPES  */

/*  Control messages  */

typedef struct
    {
    byte data_bits; /* Number of data bits                   */
    byte stop_bits; /* Number of stop bits                   */
    byte rate;      /* Rate                                  */
    byte parity;    /* Parity                                */
    } PEP_COMM_CTRL_ID_PARAM_STR;

typedef struct
    {
    byte outwards;  /* Flow control mode outwards            */
    byte inwards;   /* Flow control mode inwards             */
    byte extract;   /* Extraction of flow control characters */
    } PEP_COMM_CTRL_ID_FLOW_CTRL_STR;

typedef struct
    {
    byte type;      /* Break type                            */
    byte length;    /* Break length in 10 ms units           */
    } PEP_COMM_CTRL_ID_BREAK_STR;

typedef struct
    {
    byte parity;    /* Parity bit when in 7 bit mode         */
    byte mode;      /* On/off                                */
    } PEP_COMM_CTRL_ID_AUTOBAUD_STR;

typedef struct
    {
    byte character; /* Escape character                      */
    byte time;      /* Escape time in 20 ms units            */
    } PEP_COMM_CTRL_ID_ESCAPE_STR;

typedef struct
    {
    byte mode;      /* Echo on/off                           */
    } PEP_COMM_CTRL_ID_ECHO_STR;

/*  Indication messages  */

typedef struct
    {
    byte data_bits; /* Number of data bits                   */
    byte stop_bits; /* Number of stop bits                   */
    byte rate;      /* Rate                                  */
    byte parity;    /* Parity                                */
    } PEP_COMM_IND_ID_PARAM_STR;

typedef struct
    {
    byte outwards;  /* Flow control mode outwards            */
    byte inwards;   /* Flow control mode inwards             */
    byte extract;   /* Extraction of flow control characters */
    } PEP_COMM_IND_ID_FLOW_CTRL_STR;

typedef struct
    {
    byte new_state; /* New state of V.24 line                */
    } PEP_COMM_IND_ID_V24_CHANGE_STR;

#endif // PEP_COMM_H

//  End of File
