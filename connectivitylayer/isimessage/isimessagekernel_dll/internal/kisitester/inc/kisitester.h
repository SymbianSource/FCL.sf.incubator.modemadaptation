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
* Description:  Test for kernelisimessage
*
*/



#ifndef _KISITESTER_H_
#define _KISITESTER_H_

// INCLUDES
#include<isikernel.h>



// CONSTANTS
#define TEST_MSG_REQ 0x0A
#define TEST_MSG_RESP 0x0B

#define TEST_MSG_MEDIA 0x01
#define TEST_MSG_RECEIVERDEVICE 0x12
#define TEST_MSG_SENDERDEVICE 0x13
#define TEST_MSG_RESOURCEID 0x04
#define TEST_MSG_LENGTH 0x0014
#define TEST_MSG_RECEIVEROBJECT 0x0102
#define TEST_MSG_SENDEROBJECT 0x0103
#define TEST_MSG_TRANSID 0x09

#define TEST_MSG_EXTENDEDRESOURCE 0x02F3E0

#define TEST_MSG_REQ_OFFSET TRANSACTIONID 0
#define TEST_MSG_REQ_OFFSET_MESSAGE_ID 1
#define TEST_MSG_REQ_OFFSET_FILL1 2
#define TEST_MSG_REQ_OFFSET_FILL2 3
#define TEST_MSG_REQ_OFFSET_FILL3 4
#define TEST_MSG_REQ_OFFSET_FILL4 5
#define TEST_MSG_REQ_OFFSET_FILL5 6
#define TEST_MSG_REQ_OFFSET_FILL6 7
#define TEST_MSG_REQ_OFFSET_FILL7 8
#define TEST_MSG_REQ_OFFSET_FILL8 9
#define TEST_MSG_REQ_OFFSET_FILL9 10
#define TEST_MSG_REQ_OFFSET_FILL10 11


/** 2000 millisecond pulse time (2.0 sec) */
const TUint KIsaKernelIfQuery   = 2000000;
/** Default ISI message size */
const TUint16 KDefaultMsgSize		= 128;
/** Default DFC queue priority */
const TUint8 KDefaultDfcPriority    = 3; 
/** Max number of subscribers */
const TUint16 KMaxNumberOfSubscribers	= 128;


//- Macros ------------------------------------------------------------------


//- External Data -----------------------------------------------------------


//- Variables ---------------------------------------------------------------


//- Forward Declarations ----------------------------------------------------

class DIsaKernelIf;
class TIsiKernelSend;
class TIsiKernelReceiveC;
// CLASS DECLARATION

/**
*  kernelisimessage kernel extension class.
*
*/


class KisiTester : public DBase
    {
    public: // Constructors and destructor
        KisiTester();
        virtual ~KisiTester();
  		  TInt Init( );        
        void Subscribe(TAny* aCallBack);
     	  void StartTesting();  
        void TestSet8Bit();
        void TestSet16Bit();
        void TestSet32Bit();  		
        void TestCopyData();
        void TestConstructors();
        void TestComplete();
    
        void TestSetReceiverDevice();        
        void TestSetSenderDevice();
        void TestSetReceiverObject();
        void TestSetSenderObject();

        void TestSetExtendedResourceId();	
    
        void TestGet8Bit();
        void TestGet16Bit();
        void TestGet32Bit();
        void TestGetData();
				void TestGetBuffer();
				TUint16 Get16BitFromTestMessage(const TUint8 *aMessage, TInt aIndex);
				    
        void TestFindSubBlockOffsetById_1(const TSubBlockType aType);
        void TestFindSubBlockOffsetById_2(const TSubBlockType aType);    
        void TestFindSubBlockOffsetByIndex_1(const TSubBlockType aType);
        void TestFindSubBlockOffsetByIndex_2(const TSubBlockType aType);        
        void TestGetReceiverDevice();        
        void TestGetSenderDevice();
        void TestGetReceiverObject();
        void TestGetSenderObject();

        void TestGetExtendedResourceId();
    
        void TestGetReceiverDevice_crashtest();     	  

 private: // Data
    		void StartTimer();
				void StartTimer2();
        static void IsaKernelIfInit(TAny* aPtr);
        static void IsaKernelIfInit2(TAny* aPtr);
        static void RegisterToIsaKernelIfDfc(TAny* aPtr);
        static void ReceiveDfc(TAny* aPtr);
        static void CheckResponse(TDesC8* aPnMsg);
        /** DFC which is used for Receiving ISI-messages */
        TDfc iReceiveDfc;
        /** DFC which is used for initializing ISA Kernel IF interface*/
        TDfc iIsaKernelIfInitDfc;
        TDfc iIsaKernelIfInitDfc2;
        /** Pointer to Isa Kernel IF (sending and receiving ISI-messages) */
        DIsaKernelIf* iIsaKernelIf;       
        DIsaKernelIf* iIsaKernelIf2;       
        /** Pointer to received message */
    		TDes8* iReceiveMsg;
    		TDes8* iReceiveMsg2;
    		TDes8* iSendMsg;   		        
        /** TImer which is used for ISA Kernel IF init */
        TTickLink iIsaKernelIfInitTimer;
        TTickLink iIsaKernelIfInitTimer2;
        /** ISA Kernel IF Initialisation status */
        TInt iStatus;
        TInt iStatus2;
        /** Length variable for ISA Kernel interface usage */
        TUint16 iNeededLength;
     		/** Pointer to KisiTesterHandler class*/
		    TAny* iCallBackPtr;

    }; 
  

//- Global Function Prototypes ----------------------------------------------


//- Inline Functions --------------------------------------------------------


//- Namespace ---------------------------------------------------------------

#endif // _KISITESTER_H_

// End of File
