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
* Description:  Test extension for kernelisimessage
*
*/



#include <platform.h>
#include <kernel/kern_priv.h>
#include <kernel/kernel.h>

#include <pn_const.h>
#include <lightisi.h>
#include <iscnokiadefinitions.h>
#include <commisi.h>
#include <disakernelif.h>
#include  "kisitester.h"
#include  "kisi_trace.h"
#include "kisitester_handler.h"
#include <isikernel.h>
#include <phonetisi.h>

// ---------------------------------------------------------
// 

const TInt KGetChannelInfoLength 		= 2;
const TInt KPnMsgMessageId = 9;
// Start of user data area in Phonet msg
const TInt KPnMsgUserDataBase = 10;

// ========================== OTHER EXPORTED FUNCTIONS =========================
// -----------------------------------------------------------------------------
// KisiTester::KisiTester
// Constructor.
// -----------------------------------------------------------------------------
//    Constructor
//

KisiTester::KisiTester()
        : DBase()
        ,iReceiveDfc(KisiTester::ReceiveDfc, static_cast<TAny*>(this), Kern::DfcQue0(), KDefaultDfcPriority)
        ,iIsaKernelIfInitDfc( KisiTester::IsaKernelIfInit2, static_cast<TAny*>(this), Kern::DfcQue0(), KDefaultDfcPriority)
        ,iIsaKernelIfInitDfc2(KisiTester::RegisterToIsaKernelIfDfc, static_cast<TAny*>(this), Kern::DfcQue0(), KDefaultDfcPriority)
        ,iIsaKernelIf(NULL)
        ,iIsaKernelIf2(NULL)
        ,iReceiveMsg(NULL)
        ,iSendMsg(NULL)
        ,iStatus(0)
        ,iStatus2(0)
        ,iNeededLength(KDefaultMsgSize)
        ,iCallBackPtr(NULL)
    {
    COMPONENT_TRACE(("KISITESTER::KISITESTER CONTRUCTOR() entering")); 

    COMPONENT_TRACE(("KISITESTER::KISITESTER CONTRUCTOR() returning"));         
    }   

   
// -----------------------------------------------------------------------------
// KisiTester::~KisiTester
// Destructor
// -----------------------------------------------------------------------------
//
KisiTester::~KisiTester()
    {
    COMPONENT_TRACE(("KISITESTER::~KISITESTER() - STARTING"));
        delete iIsaKernelIf;
        delete iIsaKernelIf2;
//        delete iReceiveMsg;
    COMPONENT_TRACE(("KISITESTER::~KISITESTER - RETURNING"));
    }

// ---------------------------------------------------------
// KisiTester::TestSet8Bit
// ---------------------------------------------------------
void KisiTester::TestSet8Bit()
{
    COMPONENT_TRACE(("KISITESTER::TEST__SET8BIT() - STARTING")); 

        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 20 );

        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL2, 0x01 );
        msg.Set8bit( 19, 0x0f );

        TBuf8<20> readyMsg = msg.Complete();

        if( testMsg[ 19 ] != 0x0f || testMsg.Compare( readyMsg ) != KErrNone )
            {
            COMPONENT_TRACE(("KisiTester::TestSet8Bit() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSet8Bit() PASSED"));
            }

    COMPONENT_TRACE(("KISITESTER::TEST_SET8BIT - RETURN VOID"));
}

// ---------------------------------------------------------
// KisiTester::TestSet16Bit
// ---------------------------------------------------------
void KisiTester::TestSet16Bit()
{
    COMPONENT_TRACE(("KISITESTER::TEST_SET16BIT() - STARTING")); 

        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 18 );

        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        
        msg.Set16bit( 16, 0x1324 );
        TBuf8<20> readyMsg = msg.Complete();

        if( testMsg[ 16 ] != 0x13 || testMsg[ 17 ] != 0x24 )
            {
            COMPONENT_TRACE(("KisiTester::TestSet16Bit() FAILED"));
            }
      
        else if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            COMPONENT_TRACE(("KisiTester::TestSet16Bit() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSet16Bit() PASSED"));
            }
    COMPONENT_TRACE(("KISITESTER::TEST_SET16BIT - RETURN VOID"));
}

// ---------------------------------------------------------
// KisiTester::TestSet32it
// ---------------------------------------------------------
void KisiTester::TestSet32Bit()
{
    COMPONENT_TRACE(("KISITESTER::TEST_SET32BIT() - STARTING")); 

        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 16 );

        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        
        msg.Set32bit( 12, 0x11223344 );
        TBuf8<20> readyMsg = msg.Complete();

        if( testMsg[ 12 ] != 0x11 || testMsg[ 13 ] != 0x22 ||
            testMsg[ 14 ] != 0x33 || testMsg[ 15 ] != 0x44 )
            {
            COMPONENT_TRACE(("KisiTester::TestSet32Bit() FAILED"));
            }
        if( testMsg.Compare( readyMsg ) != KErrNone )
            {
            COMPONENT_TRACE(("KisiTester::TestSet32Bit() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSet32it() PASSED"));
            }
    COMPONENT_TRACE(("KISITESTER::TEST_SET32BIT - RETURN VOID"));
}

// ---------------------------------------------------------
// KisiTester::TestCopyData
// ---------------------------------------------------------
void KisiTester::TestCopyData()
{
    COMPONENT_TRACE(("KISITESTER::TEST_COPYDATA() - STARTING"));
    
	      TBuf8<8> temp;
  	    temp.Append( 0x11 );
    	  temp.Append( 0x12 );
    	  temp.Append( 0x13 );
    	  temp.Append( 0x14 );
    	  temp.Append( 0x15 );
    	  temp.Append( 0x16 );
    	  temp.Append( 0x17 );
    	  temp.Append( 0x18 );

	      TBuf8<20> testMsg;
    	  TIsiKernelSend msg( testMsg, 18 );
        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        
    	  msg.CopyData( 10, temp );
     
  	    TBuf8<20> readyMsg = msg.Complete();

    	  if( testMsg[ 10] != 0x11 || testMsg[11] != 0x12 ||
      	    testMsg[12] != 0x13 || testMsg[13] != 0x14 ||
        	  testMsg[14] != 0x15 || testMsg[ 15 ] != 0x16 ||
        	  testMsg[16] != 0x17 || testMsg[ 17 ] != 0x18 )
        	  {
              COMPONENT_TRACE(("KisiTester::TestCopyData() FAILED"));
        	  }        	        
    	   else if( testMsg.Compare( readyMsg ) != KErrNone )
      	    {
              COMPONENT_TRACE(("KisiTester::TestCopyData() FAILED"));
        	  }
     	  else
      	    {
        	  	COMPONENT_TRACE(("KisiTester::TestCopyData() PASSED"));
        	  }  
        
    COMPONENT_TRACE(("KISITESTER::TEST_COPYDATA - RETURN VOID"));
}
    
// ---------------------------------------------------------
// KisiTester::TestConstructors
// ---------------------------------------------------------
void KisiTester::TestConstructors()
{
    COMPONENT_TRACE(("KISITESTER::TEST_CONSTRUCTORS() - STARTING"));
    

        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg ); //TIsiKernelSend msg( testMsg, KTwenty ); 
        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        
        TBuf8<20> readyMsg = msg.Complete();

        	  
        TBuf8<20> testMsg2;
        TIsiKernelSend msg2( testMsg2, 20 ); 
        msg2.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg2.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg2.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg2.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg2.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg2.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        
        TBuf8<20> readyMsg2 = msg2.Complete();

        if( testMsg[ ISI_HEADER_OFFSET_MESSAGEID ] != TEST_MSG_REQ || testMsg.Compare( readyMsg ) != KErrNone ||
        		testMsg2[ ISI_HEADER_OFFSET_MESSAGEID ] != TEST_MSG_REQ || testMsg2.Compare( readyMsg2 ) != KErrNone )
            {
              COMPONENT_TRACE(("KisiTester::TestConstructors() FAILED"));
            }
     	  else
      	    {
        	  	COMPONENT_TRACE(("KisiTester::TestConstructors() PASSED"));
        	  }        	       
        

    COMPONENT_TRACE(("KISITESTER::TEST_CONSTRUCTORS - RETURN VOID"));
}

// ---------------------------------------------------------
// KisiTester::TestComplete
// ---------------------------------------------------------
void KisiTester::TestComplete()
{       
    COMPONENT_TRACE(("KISITESTER::TEST_COMPLETE - STARTING"));	
    
        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 20 );

        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );    
        msg.Set8bit( 10, 0x01 );
        msg.Set8bit( 11, 0x02 );
        
        TBuf8<20> readyMsg = msg.Complete();

        TBuf8<20> testMsg2;
        TIsiKernelSend msg2( testMsg2 );

        msg2.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg2.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg2.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg2.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg2.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg2.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg2.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg2.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );    
        msg2.Set8bit( 10, 0x01 );
        msg2.Set8bit( 11, 0x02 );
        
        TBuf8<20> readyMsg2 = msg2.Complete();

            if( testMsg[ 0 ] != TEST_MSG_MEDIA || testMsg[ 1 ] != 0x04 || testMsg2[ 0 ] != TEST_MSG_MEDIA || testMsg2[ 1 ] != 0x04 )  
            {
             COMPONENT_TRACE(("KisiTester::TestComplete() FAILED"));
            }   
            else if ( testMsg[ ISI_HEADER_OFFSET_TRANSID ] != TEST_MSG_TRANSID || testMsg[ ISI_HEADER_OFFSET_MESSAGEID ] != TEST_MSG_REQ 
            || testMsg2[ ISI_HEADER_OFFSET_TRANSID ] != TEST_MSG_TRANSID || testMsg2[ ISI_HEADER_OFFSET_MESSAGEID ] != TEST_MSG_REQ ) 
            {
              COMPONENT_TRACE(("KisiTester::TestComplete() FAILED"));
            }    
            else if ( testMsg[ 4 ] != 0x0e || testMsg[ 5 ] != 0x00 
            || testMsg2[ 4 ] != 0x06 || testMsg2[ 5 ] != 0x00 )  //length
            {
              COMPONENT_TRACE(("KisiTester::TestComplete() FAILED"));
            }                         
            else if( testMsg.Compare( readyMsg ) != KErrNone || testMsg2.Compare( readyMsg2 ) != KErrNone)
            {
              COMPONENT_TRACE(("KisiTester::TestComplete() FAILED"));
            }  
            else if( readyMsg.Length() != 20 || readyMsg2.Length() != 12 )
            {
              COMPONENT_TRACE(("KisiTester::TestComplete() FAILED")); 
            }            
        	  else
      	    {
        	  	COMPONENT_TRACE(("KisiTester::TestComplete() PASSED"));
        	  }       
    
    COMPONENT_TRACE(("KISITESTER::TEST_COMPLETE - RETURN VOID"));
    
}

// ---------------------------------------------------------
// KisiTester::TestSetReceiverDevice
// ---------------------------------------------------------
void KisiTester::TestSetReceiverDevice()
{       
		
		COMPONENT_TRACE(("KISITESTER::TEST_SETRECEIVERDEVICE - STARTING"));	
		
        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 20 );

       
        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x00 );
        
        msg.SetReceiverDevice( PN_DEV_FIRST_FIXED );        
        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );                

				msg.Complete();

        if( testMsg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] != 0x04 )
            {
            COMPONENT_TRACE(("KisiTester::TestSetReceiverDevice() FAILED, device 0x%x", testMsg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ]));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSetReceiverDevice() PASSED, device 0x%x", testMsg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ]));
            }     
    
    COMPONENT_TRACE(("KISITESTER::TEST_SETRECEIVERDEVICE - RETURN VOID"));
    
}	
	
// ---------------------------------------------------------
// KisiTester::TestSetSenderDevice
// ---------------------------------------------------------
void KisiTester::TestSetSenderDevice()
{       
	
	COMPONENT_TRACE(("KISITESTER::TEST_SETSENDERDEVICE - STARTING"));	
	
        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 20 );
      
        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x00 );        
        
        msg.SetSenderDevice( PN_DEV_FIRST_FIXED );        
        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );        

        msg.Complete();

        if( testMsg[ ISI_HEADER_OFFSET_SENDERDEVICE ] != 0x04 )
            {
            COMPONENT_TRACE(("KisiTester::TestSetSenderDevice() FAILED, device 0x%x", testMsg[ ISI_HEADER_OFFSET_SENDERDEVICE ]));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSetSenderDevice() PASSED, device 0x%x", testMsg[ ISI_HEADER_OFFSET_SENDERDEVICE ]));
            }     
    
    COMPONENT_TRACE(("KISITESTER::TEST_SETSENDERDEVICE - RETURN VOID"));
    
}		
	
   
// ---------------------------------------------------------
// KisiTester::TestSetReceiverObject
// ---------------------------------------------------------
void KisiTester::TestSetReceiverObject()
{       

	COMPONENT_TRACE(("KISITESTER::TEST_SETRECEIVEROBJECT - STARTING"));
	
        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 20 );

        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        
        msg.SetSenderDevice( PN_DEV_FIRST_FIXED );
        msg.SetReceiverObject( TEST_MSG_RECEIVEROBJECT ); //2+8, 2 TO DEVICE        
        
        //msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        //msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );
				msg.Complete();

        if( testMsg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ] != 0x02 && testMsg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ] != 0x02)            
            {
            COMPONENT_TRACE(("KisiTester::TestSetReceiverObject() FAILED, receiverobject 0x%x, receiverobject 0x%x", testMsg[ ISI_HEADER_OFFSET_RECEIVEROBJECT ], testMsg[ ISI_HEADER_OFFSET_RECEIVERDEVICE ]));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSetReceiverObject() PASSED"));
            }     
    
    COMPONENT_TRACE(("KISITESTER::TEST_SETRECEIVEROBJECT - RETURN VOID"));
    
}	
	     
// ---------------------------------------------------------
// KisiTester::TestSetSenderObject
// ---------------------------------------------------------
void KisiTester::TestSetSenderObject()
{       

	COMPONENT_TRACE(("KISITESTER::TEST_SETSENDEROBJECT - STARTING"));
	
        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 20 );
        
        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        
        msg.SetSenderDevice( PN_DEV_FIRST_FIXED );
        msg.SetSenderObject( TEST_MSG_SENDEROBJECT );
        
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        //msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        //msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );
        msg.Set8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL1, 0x00 );        
        
				msg.Complete();

        if( testMsg[ ISI_HEADER_OFFSET_SENDEROBJECT ] != 0x03 && testMsg[ ISI_HEADER_OFFSET_SENDERDEVICE ] != 0x00)        
            {
            COMPONENT_TRACE(("KisiTester::TestSetSenderObject() FAILED, senderobject 0x%x, senderdevice 0x%x", testMsg[ ISI_HEADER_OFFSET_SENDEROBJECT ], testMsg[ ISI_HEADER_OFFSET_SENDERDEVICE ]));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSetSenderObject() PASSED"));
            }     
    
    COMPONENT_TRACE(("KISITESTER::TEST_SETSENDEROBJECT - RETURN VOID"));
    
}		
		     
       
  
// ---------------------------------------------------------
// KisiTester::TestSetExtendedResourceId
// ---------------------------------------------------------
void KisiTester::TestSetExtendedResourceId()
{       
	
    COMPONENT_TRACE(("KISITESTER::TEST_SETEXTENDEDRESOURCEID - STARTING"));
    
        TBuf8<20> testMsg;
        TIsiKernelSend msg( testMsg, 20 );
        
//#define ISI_HEADER_OFFSET_TYPE	        10
//#define ISI_HEADER_OFFSET_SUBTYPE	      11
//Media	Receiver	Sender	PN_PREFIX	Length	Rec. Obj	Send.Obj	UTID	MsgID	Type	SubType
//Extended resource id =  (subtype <<16) + (Type<<8) +  (PN_PREFIX)

     
        msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, TEST_MSG_MEDIA );
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x04 );
        msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x04 );        
        msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, TEST_MSG_RESOURCEID );
        //ISI_HEADER_OFFSET_LENGTH
        msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x06 );        
        msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x06 );
        msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, TEST_MSG_TRANSID );
        msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, TEST_MSG_REQ  );

				#define PN_TESTSRV3 PN_RES(0xF3,0x02)   //EXTENDEDRESOURCE -> 0x02f3e0     
        msg.SetExtendedResourceId( PN_TESTSRV3 );             
				msg.Complete();

        if( testMsg[ ISI_HEADER_OFFSET_RESOURCEID ] != 0xE0 || testMsg[ ISI_HEADER_OFFSET_TYPE ] != 0xF3
        || testMsg[ ISI_HEADER_OFFSET_SUBTYPE ] != 0x02 )
            {
            COMPONENT_TRACE(("KisiTester::TestSetExtendedResourceId() FAILED, type =  0x%x, subtype =  0x%x, prefix =  0x%x", testMsg[ ISI_HEADER_OFFSET_TYPE ],testMsg[ ISI_HEADER_OFFSET_SUBTYPE ], testMsg[ ISI_HEADER_OFFSET_RESOURCEID ]));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestSetExtendedResourceId() PASSED"));
            }     
    
    COMPONENT_TRACE(("KISITESTER::TEST_SETEXTENDEDRESOURCEID - RETURN VOID"));
    
}		
		    

// ---------------------------------------------------------
// KisiTester::TestGet8Bit
// ---------------------------------------------------------
void KisiTester::TestGet8Bit()
{
    COMPONENT_TRACE(("KISITESTER::TEST_GET8BIT() - STARTING")); 

        TBuf8<20> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiKernelReceiveC msg( testMsg );

        TUint8 byteNine = msg.Get8bit( ISI_HEADER_OFFSET_MESSAGEID );
        TUint8 byteEightteen  = msg.Get8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL9 );
        TUint8 byteNineteen = msg.Get8bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL10 );

        if( testMsg[ 9 ] != byteNine || testMsg[ 18  ] != byteEightteen  || 
            testMsg[ 19 ] != byteNineteen )
            {
            COMPONENT_TRACE(("KisiTester::TestGet8Bit() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGet8Bit() PASSED"));
            }

    COMPONENT_TRACE(("KISITESTER::TEST_GET8BIT - RETURN VOID"));
}

// ---------------------------------------------------------
// KisiTester::TestGet16Bit
// ---------------------------------------------------------
void KisiTester::TestGet16Bit()
{
    COMPONENT_TRACE(("KISITESTER::TEST_GET16BIT() - STARTING")); 

        TBuf8<20> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x14 );

        TIsiKernelReceiveC msg( testMsg );

        TUint16 byteEighteen = msg.Get16bit( ISI_HEADER_SIZE + TEST_MSG_REQ_OFFSET_FILL9 );
        TUint16 byteEighteenComp = Get16BitFromTestMessage( testMsg.Ptr(), 18 );
        
        if( byteEighteenComp != byteEighteen || testMsg[18]!= 0x13 || testMsg[19]!=0x14 )
            {
            COMPONENT_TRACE(("KisiTester::TestGet16Bit() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGet16Bit() PASSED"));
            }

    COMPONENT_TRACE(("KISITESTER::TEST_GET16BIT - RETURN VOID"));
}

// ---------------------------------------------------------
// KisiTester::TestGet32Bit
// ---------------------------------------------------------
void KisiTester::TestGet32Bit()
{
    COMPONENT_TRACE(("KISITESTER::TEST_GET32BIT() - STARTING")); 


        TBuf8<20> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );
        

        TIsiKernelReceiveC msg( testMsg );

        TUint32 byteSixteen = msg.Get32bit( 16 );

        if( 0x10 != testMsg[16] || 0x11 != testMsg[17] || 0x12 != testMsg[18]  || 0x13 != testMsg[19]  )
            {
            COMPONENT_TRACE(("KisiTester::TestGet32Bit() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGet32Bit() PASSED"));
            }

    COMPONENT_TRACE(("KISITESTER::TEST_GET32BIT - RETURN VOID"));
}

    
// ---------------------------------------------------------
// KisiTester::TestGetData
// ---------------------------------------------------------
void KisiTester::TestGetData()
{
    COMPONENT_TRACE(("KISITESTER::TEST_GETDATA() - STARTING")); 


        TBuf8<20> testMsg;
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x04 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0b );
        testMsg.Append( 0x05 );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x0c );
        testMsg.Append( 0x0d );
        testMsg.Append( 0x0e );
        testMsg.Append( 0x0f );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );
        testMsg.Append( 0x12 );
        testMsg.Append( 0x13 );

        TIsiKernelReceiveC msg( testMsg );

        TPtrC8 byteZero = msg.GetData( 0, 20 );
        TPtrC8 byteTwelve = msg.GetData( 12, 8 );

        TPtrC8 byteZeroComp = testMsg.Mid( 0, 20 );
        TPtrC8 byteTenComp = testMsg.Mid( 12, 8 );
        if( byteZeroComp != byteZero ||  byteTenComp != byteTwelve )
            {
            COMPONENT_TRACE(("KisiTester::TestGetData() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetData() PASSED"));
            }

    COMPONENT_TRACE(("KISITESTER::TEST_GETDATA - RETURN VOID"));
}

    
// ---------------------------------------------------------
// KisiTester::TestFindSubBlockOffsetById_1
// ---------------------------------------------------------
void KisiTester::TestFindSubBlockOffsetById_1(const TSubBlockType aType)
{
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYID_1( aType:%d ) - STARTING", aType)); 

     TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

    if( EIsiSubBlockTypeId8Len8 == aType )
        {
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x08 ); //len 4bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        }
    else if( EIsiSubBlockTypeId8Len16 == aType )
        {
        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x08 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        testMsg.Append( 0x17 ); 
        testMsg.Append( 0x18 );
        }      
    else if( EIsiSubBlockTypeId8Len32 == aType )
        {
        testMsg.Append( 0x05 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x01 ); //data
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 ); //data
        }            
        TIsiKernelReceiveC msg( testMsg );
        TInt temp(KErrNone);
        TUint aSubBlockOffset = 0;        
        temp = msg.FindSubBlockOffsetById( 0x0c, 0x05, aType, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetById_1( aType: %d ) FAILED", aType));
            }

        else if( temp == KErrNone )
            {
            if ( testMsg[ aSubBlockOffset] != 0x05 ) //aSubBlockOffset should be 12
                {
            COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetById_1( aSubBlockOffset:0x%x ) FAILED",aSubBlockOffset ));
                }
            else
                {
                COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetById_1( aSubBlockOffset: 0x%x ) PASSED", aSubBlockOffset));
                }
            }
            
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYID_1 ( aType:%d ) - RETURN VOID", aType));
    
}
   
// ---------------------------------------------------------
// KisiTester::TestFindSubBlockOffsetById_2
// ---------------------------------------------------------
void KisiTester::TestFindSubBlockOffsetById_2(const TSubBlockType aType)
{
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYID_2( aType:%d ) - STARTING", aType)); 

     TBuf8<20> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

   if( EIsiSubBlockTypeId16Len8 == aType )
       {
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x02 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );
        }      
    else if( EIsiSubBlockTypeId16Len16 == aType )
        {
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );
        }            
        TIsiKernelReceiveC msg( testMsg );
        TInt temp(KErrNone);
        TUint aSubBlockOffset = 0;        
        temp = msg.FindSubBlockOffsetById( 0x0c, 0x0201, aType, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetById_2( %x ) FAILED", aType));
            }

        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 ) //aSubBlockOffset should be 12
                {
            COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetById_2( 0x%x ) FAILED",aSubBlockOffset ));
                }
            else
                {
                COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetById_2( aSubBlockOffset: 0x%x) PASSED", aSubBlockOffset));
                }
            }
            
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYID_2 ( aType:%d ) - RETURN VOID", aType));
    
}

    
// ---------------------------------------------------------
// KisiTester::TestFindSubBlockOffsetByIndex_1
// ---------------------------------------------------------
void KisiTester::TestFindSubBlockOffsetByIndex_1(const TSubBlockType aType)
{
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYINDEX_1( aType:%d ) - STARTING", aType)); 

     TBuf8<34> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

    if( EIsiSubBlockTypeId8Len8 == aType )
        {
        testMsg.Append( 0x03 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x0A ); //len 8bit
        testMsg.Append( 0x12 ); 
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x06 ); //len 8bit
        testMsg.Append( 0x13 ); 
        testMsg.Append( 0x14 );
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );

        }
    else if( EIsiSubBlockTypeId8Len16 == aType )
        {
        testMsg.Append( 0x03 ); //sb_start = sb_id, offset 0x0c = 12
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x0A ); //len
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );
        testMsg.Append( 0x13 );

        testMsg.Append( 0x05 ); //sb_start = sb_id, offset 16
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x06 ); //len 16bit
        testMsg.Append( 0x15 ); 
        testMsg.Append( 0x16 );
        }      
    else if( EIsiSubBlockTypeId8Len32 == aType )
        {
        testMsg.Append( 0x02 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0A ); //len
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 );
                
        testMsg.Append( 0x05 ); //sb_start 8bit
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //filler
        testMsg.Append( 0x00 ); //len 32bit
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x00 ); //len
        testMsg.Append( 0x0c ); //len
        testMsg.Append( 0x03 ); //data
        testMsg.Append( 0x04 );

        }            
        
        TIsiKernelReceiveC msg( testMsg );
        TInt temp(KErrNone);
        TUint aSubBlockOffset( 0 );
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, aType, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetByIndex_1( aType: %d ) FAILED", aType));
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x05 || aSubBlockOffset != 22 ) //aSubBlockOffset should be 16
                {
            COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetByIndex_1( aSubBlockOffset: 0x%x ) FAILED", aSubBlockOffset));
                }
            else
                {
                COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetByIndex_1 ( aSubBlockOffset: 0x%x ) PASSED",aSubBlockOffset));
                }
            }
            
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYINDEX_1 ( aType:%d ) - RETURN VOID", aType));
    
}

// ---------------------------------------------------------
// KisiTester::TestFindSubBlockOffsetByIndex_2
// ---------------------------------------------------------
void KisiTester::TestFindSubBlockOffsetByIndex_2(const TSubBlockType aType)
{
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYINDEX_2( aType:%d )  - STARTING", aType)); 

     TBuf8<24> testMsg;
        testMsg.Append( 0x00 );
        testMsg.Append( 0x01 );
        testMsg.Append( 0x02 );
        testMsg.Append( 0x03 );
        testMsg.Append( 0x00 );
        testMsg.Append( 0x0a );
        testMsg.Append( 0x06 );
        testMsg.Append( 0x07 );
        testMsg.Append( 0x08 );
        testMsg.Append( 0x09 );
        testMsg.Append( 0x10 );
        testMsg.Append( 0x11 );

    if( EIsiSubBlockTypeId16Len8 == aType )
        {
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x04 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x08 ); //len 8bit
        testMsg.Append( 0x08 ); //data
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );
        }
    else if( EIsiSubBlockTypeId16Len16 == aType )
        {
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x04 ); //len
        
        testMsg.Append( 0x02 ); //sb_start 16bit
        testMsg.Append( 0x01 ); //sb_start 16bit
        testMsg.Append( 0x00 ); //len 16bit
        testMsg.Append( 0x08 ); //len
        testMsg.Append( 0x03 ); 
        testMsg.Append( 0x06 );
        testMsg.Append( 0x05 ); 
        testMsg.Append( 0x06 );
        }                 
        
        TIsiKernelReceiveC msg( testMsg );
        TInt temp(KErrNone);
        TUint aSubBlockOffset( 0 );
        temp=msg.FindSubBlockOffsetByIndex( 0x0c, 2, aType, aSubBlockOffset);

        if( temp == KErrNotFound )
            {
            COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetByIndex_2( aType: %d ) FAILED", aType));
            }
        else if( temp == KErrNone )
            {
            if ( testMsg[aSubBlockOffset] != 0x02 || testMsg[aSubBlockOffset+1] != 0x01 || aSubBlockOffset != 16 ) 
                {
                COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetByIndex_2( 0x%x ) FAILED", aSubBlockOffset));
                }
            else
                {
                COMPONENT_TRACE(("KisiTester::TestFindSubBlockOffsetByIndex_2( aSubBlockOffset: 0x%x) PASSED", aSubBlockOffset));
                }
            }
            
    COMPONENT_TRACE(("KISITESTER::TEST_FINDSUBBLOCKOFFSETBYINDEX_2( aType:%d ) - RETURN VOID", aType));
    
}

// ---------------------------------------------------------
// KisiTester::TestGetBuffer
// ---------------------------------------------------------
void KisiTester::TestGetBuffer()
    {
    	    	
    COMPONENT_TRACE(("KISITESTER::TEST_GETBUFFER - RETURN VOID"));
   
    TBuf8<16> testMsg;
    testMsg.Append( 0x00 ); 
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x02 ); 
    testMsg.Append( 0x03 ); 
    testMsg.Append( 0x00 ); 
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x02 ); 
    testMsg.Append( 0x03 );
    
    TIsiKernelReceiveC msg( testMsg);
    TPtrC8 pointer =  msg.GetBuffer();
    //check length
    TUint msgLength(0);
    msgLength = pointer.Length();

        if( msgLength != 8 )
            {
            COMPONENT_TRACE(("KisiTester::TestGetBuffer() FAILED"));
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetBuffer() PASSED"));	
            }
 
    COMPONENT_TRACE(("KISITESTER::TESTGETBUFFER - RETURN VOID"));

    }

	
// ---------------------------------------------------------
// KisiTester::TestGetReceiverDevice
// ---------------------------------------------------------
void KisiTester::TestGetReceiverDevice()
{       
		
		COMPONENT_TRACE(("KISITESTER::TEST_GETRECEIVERDEVICE - STARTING"));	
			
    TBuf8<16> testMsg;
    testMsg.Append( 0x01 ); 
    testMsg.Append( PN_DEV_FIRST_FIXED ); //device
    testMsg.Append( 0x02 ); //device
    testMsg.Append( 0xE0 ); //pn_prefix
    testMsg.Append( 0x00 ); //len
    testMsg.Append( 0x01 ); //len
    testMsg.Append( 0x02 ); //object
    testMsg.Append( 0x03 ); //object
    testMsg.Append( 0x00 ); //trans_id
    testMsg.Append( 0x00 ); //msg_id
    testMsg.Append( 0x01 ); //type
    testMsg.Append( 0x02 ); //subtype
    testMsg.Append( 0x03 );  
    
   
    TIsiKernelReceiveC msg( testMsg);
    TUint8 receiverDevice = msg.GetReceiverDevice();
    
        if( receiverDevice != 0x04 )
            {
            COMPONENT_TRACE(("KisiTester::TestGetReceiverDevice() FAILED, receiverDevice = 0x%x",receiverDevice) );
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetReceiverDevice() PASSED, receiverDevice = 0x%x",receiverDevice) );
            } 
    
    COMPONENT_TRACE(("KISITESTER::TEST_GETRECEIVERDEVICE - RETURN VOID"));
    
}	


	
// ---------------------------------------------------------
// KisiTester::TestGetReceiverDevice_crashtest
// ---------------------------------------------------------
void KisiTester::TestGetReceiverDevice_crashtest()
{       
		
		COMPONENT_TRACE(("KISITESTER::TestGetReceiverDevice_crashtest - STARTING"));	
			
    TBuf8<2> testMsg;
    testMsg.Append( 0x01 ); 
    testMsg.Append( PN_DEV_FIRST_FIXED ); //device
   /* testMsg.Append( 0x02 ); //device
    testMsg.Append( 0xE0 ); //pn_prefix
    testMsg.Append( 0x00 ); //len
    testMsg.Append( 0x01 ); //len
    testMsg.Append( 0x02 ); //object
    testMsg.Append( 0x03 ); //object
    testMsg.Append( 0x00 ); //trans_id
    testMsg.Append( 0x00 ); //msg_id
    testMsg.Append( 0x01 ); //type
    testMsg.Append( 0x02 ); //subtype
    testMsg.Append( 0x03 );  */
    
   
    TIsiKernelReceiveC msg( testMsg);
    TUint8 receiverDevice = msg.GetReceiverDevice();
    
        if( receiverDevice != 0x04 )
            {
            COMPONENT_TRACE(("KisiTester::TestGetReceiverDevice_crashtest() FAILED, receiverDevice = 0x%x",receiverDevice) );
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetReceiverDevice_crashtest() PASSED, receiverDevice = 0x%x",receiverDevice) );
            } 
    
    COMPONENT_TRACE(("KISITESTER::TestGetReceiverDevice_crashtest - RETURN VOID"));
    
}	

// ---------------------------------------------------------
// KisiTester::TestGetSenderDevice
// ---------------------------------------------------------
void KisiTester::TestGetSenderDevice()
{       
		
		COMPONENT_TRACE(("KISITESTER::TEST_GETSENDERDEVICE - STARTING"));	
			
    TBuf8<16> testMsg;
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x02 ); //device
    testMsg.Append( PN_DEV_FIRST_FIXED ); //device
    testMsg.Append( 0xE0 ); //pn_prefix
    testMsg.Append( 0x00 ); //len
    testMsg.Append( 0x01 ); //len
    testMsg.Append( 0x02 ); //object
    testMsg.Append( 0x03 ); //object
    testMsg.Append( 0x00 ); //trans_id
    testMsg.Append( 0x00 ); //msg_id
    testMsg.Append( 0x01 ); //type
    testMsg.Append( 0x02 ); //subtype
    testMsg.Append( 0x03 );  
    
   
    TIsiKernelReceiveC msg( testMsg);
    TUint8 senderDevice = msg.GetSenderDevice();
    
        if( senderDevice != 0x04 )
            {
            COMPONENT_TRACE(("KisiTester::TestGetSenderDevice() FAILED, senderDevice = 0x%x",senderDevice) );
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetSenderDevice() PASSED, senderDevice = 0x%x",senderDevice) );
            } 
    
    COMPONENT_TRACE(("KISITESTER::TEST_GETSENDERDEVICE - RETURN VOID"));
    
}	
	
// ---------------------------------------------------------
// KisiTester::TestGetReceiverObject
// ---------------------------------------------------------
void KisiTester::TestGetReceiverObject()
{       

	COMPONENT_TRACE(("KISITESTER::TEST_GETRECEIVEROBJECT - STARTING"));
	
			
    TBuf8<16> testMsg;
  
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x03 ); //receiverdevice
    testMsg.Append( 0x12 ); //senderdevice
    testMsg.Append( 0xE0 ); //pn_prefix / RESOURCE
    testMsg.Append( 0x00 ); //len
    testMsg.Append( 0x01 ); //len
    testMsg.Append( 0x02 ); //object
    testMsg.Append( 0x05 ); //object
    testMsg.Append( 0x00 ); //trans_id
    testMsg.Append( 0x00 ); //msg_id
    testMsg.Append( 0x01 ); //type
    testMsg.Append( 0x02 ); //subtype
    testMsg.Append( 0x03 );        
    
   
    TIsiKernelReceiveC msg( testMsg);
    TUint16 receiverobject = msg.GetReceiverObject();
    
      if( receiverobject != 0x0302 )
            {
            COMPONENT_TRACE(("KisiTester::TestGetReceiverObject() FAILED, receiverobject = 0x%x",receiverobject) );
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetReceiverObject() PASSED, receiverobject = 0x%x",receiverobject) );
            } 	    
    
    COMPONENT_TRACE(("KISITESTER::TEST_GETRECEIVEROBJECT - RETURN VOID"));
    
}	

// ---------------------------------------------------------
// KisiTester::TestGetSenderObject
// ---------------------------------------------------------
void KisiTester::TestGetSenderObject()
{       

	COMPONENT_TRACE(("KISITESTER::TEST_GETSENDEROBJECT - STARTING"));
	
			
    TBuf8<16> testMsg;
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x03 ); //receiverdevice
    testMsg.Append( 0x12 ); //senderdevice
    testMsg.Append( 0xE0 ); //pn_prefix / RESOURCE
    testMsg.Append( 0x00 ); //len
    testMsg.Append( 0x01 ); //len
    testMsg.Append( 0x05 ); //object
    testMsg.Append( 0x08 ); //object
    testMsg.Append( 0x00 ); //trans_id
    testMsg.Append( 0x00 ); //msg_id
    testMsg.Append( 0x01 ); //type
    testMsg.Append( 0x02 ); //subtype
    testMsg.Append( 0x03 );   
    
   
    TIsiKernelReceiveC msg( testMsg);
    TUint16 senderObject = msg.GetSenderObject();
    
        if( senderObject != 0x0208 )
            {
            COMPONENT_TRACE(("KisiTester::TestGetSenderObject() FAILED, senderObject = 0x%x",senderObject) );
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetSenderObject() PASSED, senderObject = 0x%x",senderObject) );
            } 	    
    
    COMPONENT_TRACE(("KISITESTER::TEST_GETSENDEROBJECT - RETURN VOID"));
    
}	

     
// ---------------------------------------------------------
// KisiTester::TestGetExtendedResourceId
// ---------------------------------------------------------
void KisiTester::TestGetExtendedResourceId()
{       
	
    COMPONENT_TRACE(("KISITESTER::TEST_GETEXTENDEDRESOURCEID - STARTING"));
    
			#define PN_RES(Type, SubType)    (PN_PREFIX  + (Type << 8) + (SubType <<16))

    TBuf8<16> testMsg;
    testMsg.Append( 0x01 ); 
    testMsg.Append( 0x12 ); //receiverdevice
    testMsg.Append( 0x03 ); //senderdevice
    testMsg.Append( 0xE0 ); //pn_prefix / RESOURCE
    testMsg.Append( 0x00 ); //length
    testMsg.Append( 0x01 ); //length
    testMsg.Append( 0x03 ); //receiverobject
    testMsg.Append( 0x08 ); //senderobject
    testMsg.Append( 0x00 );  //trans_id
    testMsg.Append( 0x02 ); //msg_id
    testMsg.Append( 0x02 ); //type 
    testMsg.Append( 0xF3 ); //subtype
   
    TIsiKernelReceiveC msg( testMsg);
    TUint32 extendedResourceId = msg.GetExtendedResourceId();
    
  //EXTENDEDRESOURCE -> 0xf302e0     
    
        if( extendedResourceId != 0xF302E0 )
            {
            COMPONENT_TRACE(("KisiTester::TestGetExtendedResourceId() FAILED, extendedResourceId = 0x%x",extendedResourceId) );
            }
        else
            {
            COMPONENT_TRACE(("KisiTester::TestGetExtendedResourceId() PASSED, extendedResourceId = 0x%x",extendedResourceId) );
            }     
    
    COMPONENT_TRACE(("KISITESTER::TEST_GETEXTENDEDRESOURCEID - RETURN VOID"));
    
}
    
// ---------------------------------------------------------
// KisiTester::Get16BitFromTestMessage
// ---------------------------------------------------------
TUint16 KisiTester::Get16BitFromTestMessage
        (
        const TUint8 *aMessage,
        TInt aIndex
        )
    {

    TUint16  retval( 0x0000 );
    TUint16 byte1( 0x0000 );
    TUint16 byte2( 0x0000 );
    byte1 = aMessage[ aIndex ];
    byte2 = aMessage[ aIndex + 1 ];

        retval = TUint16( byte1 << 8 );      
        retval = TUint16( retval | byte2 );

    return retval;

    }
        
    
// ---------------------------------------------------------
// KisiTester::StartTesting
// ---------------------------------------------------------
void KisiTester::StartTesting()
    {
    COMPONENT_TRACE(("KISITESTER::STARTTESTING()"));
    
    
    TestSet8Bit();
    TestSet16Bit();
    TestSet32Bit();  
		
    TestCopyData();
    TestConstructors();
    TestComplete();
    
    TestSetReceiverDevice();        
    TestSetSenderDevice();
    TestSetReceiverObject();
    TestSetSenderObject();

    TestSetExtendedResourceId();	
    
    TestGet8Bit();
    TestGet16Bit();
    TestGet32Bit();
    TestGetData();
    
    TestFindSubBlockOffsetById_1(EIsiSubBlockTypeId8Len8);
    TestFindSubBlockOffsetById_1(EIsiSubBlockTypeId8Len16);
    TestFindSubBlockOffsetById_1(EIsiSubBlockTypeId8Len32);
    
    TestFindSubBlockOffsetById_2(EIsiSubBlockTypeId16Len8);
    TestFindSubBlockOffsetById_2(EIsiSubBlockTypeId16Len16);
    
    TestFindSubBlockOffsetByIndex_1(EIsiSubBlockTypeId8Len8);
    TestFindSubBlockOffsetByIndex_1(EIsiSubBlockTypeId8Len16);
    TestFindSubBlockOffsetByIndex_1(EIsiSubBlockTypeId8Len32);
    
    TestFindSubBlockOffsetByIndex_2(EIsiSubBlockTypeId16Len8);
    TestFindSubBlockOffsetByIndex_2(EIsiSubBlockTypeId16Len16);
    
    TestGetReceiverDevice();        
    TestGetSenderDevice();
    TestGetReceiverObject();
    TestGetSenderObject();

    TestGetExtendedResourceId();
    
   // TestGetReceiverDevice_crashtest();	    

    COMPONENT_TRACE(("KISITESTER::STARTTESTING - RETURN VOID"));
    }    

   
//----------------------------------------------------
//    Init
//---------------------------------------------------

TInt KisiTester::Init()
	{
  COMPONENT_TRACE(("KisiTester::Init entering") );		
	TInt ret(KErrNone);
	
	COMPONENT_TRACE(("KisiTester::Init calling StartTimer()") );	
	StartTimer();
	
	COMPONENT_TRACE(("KisiTester::Init returning") );	
	return ret;
	}


//----------------------------------------------------------------------------
//    StartTimer
//    Start timer that tries to open channel to ISA Kernel If every KIsaKernelIfQuery period.  
//    Return Values:  None
//-----------------------------------------------------------------------------
void KisiTester::StartTimer()
    {
	COMPONENT_TRACE(("KisiTester::StartTimer() entering"));
	
  	COMPONENT_TRACE(("KisiTester::StartTimer() calling timer.Cancel()"));
    iIsaKernelIfInitTimer.Cancel();
    COMPONENT_TRACE(("KisiTester::StartTimer() calling timer.Periodic()"));
    iIsaKernelIfInitTimer.Periodic( KIsaKernelIfQuery, IsaKernelIfInit, this );
       
    
    COMPONENT_TRACE(("KisiTester::StartTimer() returning"));  
    }


	
//-----------------------------------------------------------------------------
// Description:    IsaKernelIf initialization
//-----------------------------------------------------------------------------
void KisiTester::IsaKernelIfInit(TAny* aPtr)
    {
	COMPONENT_TRACE(("KisiTester::IsaKernelIfInit entering") );
    	
   KisiTester* isiTester = static_cast<KisiTester*> ( aPtr );
 
    __ASSERT_ALWAYS( isiTester != NULL, Kern::Fault("KisiTester::IsaKernelIfInit", KErrGeneral) );

	// Received message
//	KisiTester->iReceiveMsg = new DPnMsg( KReceiveMsg );

	COMPONENT_TRACE(("KisiTester::IsaKernelIfInit create new DIsaKernelIf; ") );
    // Create a DIsaKernelIf to access ISA services.
    isiTester->iIsaKernelIf = new DIsaKernelIf;

		
  if( isiTester->iIsaKernelIf != NULL )
        {
		COMPONENT_TRACE(("KisiTester::IsaKernelIfInit calling iIsaKernelIf::Open()") );

        TInt r = isiTester->iIsaKernelIf->Open( EIscNokiaTerminalKeyboard, isiTester->iStatus, &isiTester->iIsaKernelIfInitDfc );
        
        if( r != KErrNone )
            {
            // Open Fails -> Clean system and wait again
            COMPONENT_TRACE(("KisiTester::IsaKernelIfInit Open Fails -> Clean system and wait again") );
           // delete isiTester->iReceiveMsg;
            //isiTester->iReceiveMsg = NULL;
            delete isiTester->iIsaKernelIf;
            isiTester->iIsaKernelIf = NULL;
            }
       else // Everything ok
            {
            COMPONENT_TRACE(("KisiTester::IsaKernelIfInit -> Open return Ok!!!!") );
            // Cancel TTickLink timer
            COMPONENT_TRACE(("KisiTester::IsaKernelIfInit, cancelling timer") );            
            isiTester->iIsaKernelIfInitTimer.Cancel();
                
            }
        }
    else
        {
        COMPONENT_TRACE(("KisiTester::IsaKernelIfInit -> Memory Allocation Failed! clean system") );
        //delete isiTester->iReceiveMsg;
        //isiTester->iReceiveMsg = NULL;
        delete isiTester->iIsaKernelIf;
        isiTester->iIsaKernelIf = NULL;     
        }

    COMPONENT_TRACE(("KisiTester::IsaKernelIfInit() returning"));
    
    }
    
//-----------------------------------------------------------------------------
// Description:    IsaKernelIf2 initialization
//-----------------------------------------------------------------------------
void KisiTester::IsaKernelIfInit2(TAny* aPtr)
    {
	COMPONENT_TRACE(("KisiTester::IsaKernelIfInit2 entering") );
    	

   KisiTester* isiTester = static_cast<KisiTester*> ( aPtr );
 
    __ASSERT_ALWAYS( isiTester != NULL, Kern::Fault("KisiTester::IsaKernelIfInit2", KErrGeneral) );

	// Received message
//	KisiTester->iReceiveMsg = new DPnMsg( KReceiveMsg );

	COMPONENT_TRACE(("KisiTester::IsaKernelIfInit2 create new DIsaKernelIf; ") );
    // Create a DIsaKernelIf to access ISA services.
		isiTester->iIsaKernelIf2 = new DIsaKernelIf;
		
  if( isiTester->iIsaKernelIf2 != NULL )
        {
		COMPONENT_TRACE(("KisiTester::IsaKernelIfInit2 calling iIsaKernelIf::Open()") );
     
        TInt r2 = isiTester->iIsaKernelIf2->Open( EIscNokiaSosAdlServer, isiTester->iStatus2, &isiTester->iIsaKernelIfInitDfc2, PN_SOS_ADL );
        

        if( r2 != KErrNone )
            {
            // Open2 Fails -> Clean system and wait again
            COMPONENT_TRACE(("KisiTester::IsaKernelIfInit2 Open2 Fails -> Clean system and wait again") );
           // delete isiTester->iReceiveMsg;
           // isiTester->iReceiveMsg = NULL;
            delete isiTester->iIsaKernelIf2;
            isiTester->iIsaKernelIf2 = NULL;
            }
       else // Everything ok
            {
            COMPONENT_TRACE(("KisiTester::IsaKernelIfInit2 -> Open return Ok!!!!") );
            }            
        }
    else
        {
        COMPONENT_TRACE(("KisiTester::IsaKernelIfInit2 -> Memory Allocation Failed! clean system") );
        //delete isiTester->iReceiveMsg;
        //isiTester->iReceiveMsg = NULL;
        delete isiTester->iIsaKernelIf2;
        isiTester->iIsaKernelIf2 = NULL;     
        }

    COMPONENT_TRACE(("KisiTester::IsaKernelIfInit2() returning"));
    
    }
	
       
//----------------------------------------------------------------
// Register TkbdIsaIf to Phonet server through Isa Kernel IF. 
//	@param   aPtr    Pointer to KisiTester class.
//--------------------------------------------------------------
void KisiTester::RegisterToIsaKernelIfDfc(TAny* aPtr)
	{
    COMPONENT_TRACE(("KisiTester::RegisterToIsaKernelIfDfc entering()") );
    
    TInt retVal(KErrNone);	
    KisiTester* isiTester = static_cast<KisiTester*>( aPtr );
    __ASSERT_ALWAYS( isiTester != NULL, Kern::Fault("KisiTester::RegisterToIsaKernelIfDfc", KErrGeneral) );
	
	  COMPONENT_TRACE(("KisiTester::RegisterToIsaKernelIfDfc calling GetMsgBlock") );
	isiTester->iReceiveMsg = isiTester->iIsaKernelIf2->GetMsgBlock( KDefaultMsgSize );
    
	__ASSERT_ALWAYS( isiTester->iReceiveMsg!=NULL,
	Kern::Fault("KisiTester::RegisterToIsaKernelIfDfc() messageBlock is NULL! %d", __LINE__));  

    //Get channel info 
    TBuf8<KGetChannelInfoLength> info;
    retVal = isiTester->iIsaKernelIf2->GetChannelInfo(EIscNokiaSosAdlServer, info);
    __ASSERT_DEBUG( retVal == KErrNone, Kern::Printf("KisiTester::RegisterToIsaKernelIfDfc: GetChannelInfo Failed! %d", retVal) );

    // Driver can now start to receive messages from ISA side  
	  COMPONENT_TRACE(("KisiTester::RegisterToIsaKernelIfDfc calling Receive") );    
    retVal = isiTester->iIsaKernelIf->Receive( isiTester->iStatus, *(isiTester->iReceiveMsg), isiTester->iNeededLength, &(isiTester->iReceiveDfc) );        

	if ( retVal == KErrNone )
            {
			COMPONENT_TRACE(( "KisiTester::RegisterToIsaKernelIfDfc Receive ok, append event"));			       	
            TBuf8<2> events;
            events.Append( PN_SOS_ADL );
            events.Append( LIGHT_STATUS_CHANGED_IND );	// LIGHT_STATUS_CHANGED_IND
           						
			COMPONENT_TRACE(( "KisiTester::RegisterToIsaKernelIfDfc, call SubscribeEvents"));			            
            retVal = isiTester->iIsaKernelIf->SubscribeEvents( events );

				if ( retVal == KErrNone )
            {
			COMPONENT_TRACE(( "KisiTester::RegisterToIsaKernelIfDfc subscribe ok"));			       	
			
    
			  COMPONENT_TRACE(("KisiTester::RegisterToIsaKernelIfDfc calling GetMsgBlock") );
			   TDes8* iMessage = isiTester->iIsaKernelIf2->GetMsgBlock(KDefaultMsgSize);
		        
	  			TIsiKernelSend msg( *iMessage, ISI_HEADER_SIZE + SIZE_LIGHT_STATUS_CHANGED_IND );

  				msg.Set8bit( ISI_HEADER_OFFSET_MEDIA, 0x00 ); 
    			msg.Set8bit( ISI_HEADER_OFFSET_RECEIVERDEVICE, 0x00 ); //receiverdevice
    			msg.Set8bit( ISI_HEADER_OFFSET_SENDERDEVICE, 0x00 ); //senderdevice
    			msg.Set8bit( ISI_HEADER_OFFSET_RESOURCEID, PN_SOS_ADL ); //pn_prefix / RESOURCE
   
    			msg.Set8bit( ISI_HEADER_OFFSET_RECEIVEROBJECT, 0x00 ); //object
    			msg.Set8bit( ISI_HEADER_OFFSET_SENDEROBJECT, 0x01 ); //object
    			msg.Set8bit( ISI_HEADER_OFFSET_TRANSID, 0x00 ); //trans_id
    			msg.Set8bit( ISI_HEADER_OFFSET_MESSAGEID, LIGHT_STATUS_CHANGED_IND ); //msg_id
    			msg.Set8bit( ISI_HEADER_SIZE + LIGHT_STATUS_CHANGED_IND_OFFSET_CONTROLTARGET, 0x00 );
    			msg.Set8bit( ISI_HEADER_SIZE + LIGHT_STATUS_CHANGED_IND_OFFSET_LIGHTSTATE, 0x00 );
    
			    msg.Complete();

			  COMPONENT_TRACE(("KisiTester::RegisterToIsaKernelIfDfc calling SendIsiEvent") );
				retVal = isiTester->iIsaKernelIf2->SendIsiEvent( *iMessage );                 
				//retVal = isiTester->iIsaKernelIf->Send( *iMessage );  
    		    __ASSERT_DEBUG( retVal  == KErrNone, Kern::Fault("KisiTester::RegisterToIsaKernelIfDfc send", retVal ) );
	            }  
	          }
	    	else
			    {
				COMPONENT_TRACE(("KisiTester::RegisterToIsaKernelIfDfc() receive") ); 	    	
	    		}    	 
		
	 	      
 COMPONENT_TRACE(("KisiTester::RegisterToIsaKernelIfDfc() returning") );		
 }        
    
//-----------------------------------------------------------------------------
// Subscribe		
// @param aCallBack pointer to class of which CallBack method will be called
//-----------------------------------------------------------------------------
void KisiTester::Subscribe(TAny* aCallBack)
	{
	// Subscribe events
	COMPONENT_TRACE(("KisiTester::Subscribe entering") );
	iCallBackPtr = aCallBack;
	COMPONENT_TRACE(("KisiTester::Subscribe returning") );
	}


//-----------------------------------------
// Handle CheckResponse
// @param aPnMsg Received Phonet message
//----------------------------------------------
 void KisiTester::CheckResponse(TDesC8* aPnMsg)
	{
	  
	  COMPONENT_TRACE(("KisiTester::CheckResponse() entering") );
	  __ASSERT_ALWAYS( aPnMsg != NULL, Kern::Fault("KisiTester::CheckResponse", KErrGeneral) );

    TIsiKernelReceiveC viesti( *aPnMsg );
    // Determine the message we received
    if(aPnMsg->Length() > KPnMsgUserDataBase)
    {
	  //TPtr8 data( aPnMsg->UserData() );
		TUint8 reason(viesti.Get8bit(ISI_HEADER_SIZE + COMMON_MESSAGE_COMM_ISI_VERSION_GET_RESP_OFFSET_SUBMESSAGEID));
			
    switch ( reason )
	      {
	      case COMM_ISI_VERSION_GET_RESP:
				COMPONENT_TRACE(("KisiTester::CheckResponse() OK") );
	      break;
	        
	      default:
				COMPONENT_TRACE(("KisiTester::CheckResponse() NOK") );
	      break;
	      }
    }		
    else
		{
		COMPONENT_TRACE(("KisiTester::CheckResponse() FAILED") );
		}
				
    COMPONENT_TRACE(("KisiTester::CheckResponse() returning") );
	}

//--------------------------------------------------------------
//  Handle receiving indication/request response(s) messages  from Phonet server
// @param  aPtr    Pointer to DTkbdIsaIf class.
//----------------------------------------------------------------
void KisiTester::ReceiveDfc(TAny* aPtr)
{
	  COMPONENT_TRACE(("KisiTester::ReceiveDfc() entering") );

    KisiTester* isiTester = static_cast<KisiTester*>( aPtr );
    __ASSERT_ALWAYS( isiTester != NULL, Kern::Fault("KisiTester::ReceiveDfc Fault", KErrGeneral) );

		TDesC8& pup(*isiTester->iReceiveMsg);
		TIsiKernelReceiveC viestin( pup );

		// Determine the message we received
		if( viestin.Get16bit(4) >= KPnMsgMessageId)
			{
			COMPONENT_TRACE(("KisiTester::ReceiveDfc() message length 0x%x", viestin.Get16bit(4)) );
	
			TUint8 function(  viestin.Get8bit(ISI_HEADER_OFFSET_RESOURCEID) );
			COMPONENT_TRACE(("KisiTester::ReceiveDfc() resource 0x%x", function) );
	
			TUint8 messageId(  viestin.Get8bit(ISI_HEADER_OFFSET_MESSAGEID) );
			COMPONENT_TRACE(("KisiTester::ReceiveDfc() messageid 0x%x", messageId) );
	    	    
			// Handle PN resp
	    if ( function == PN_INFO && messageId == COMMON_MESSAGE_COMM_ISA_ENTITY_NOT_REACHABLE_RESP ) //PN_INFO, messageId 0cf0
	        {
	     		COMPONENT_TRACE(("KisiTester::ReceiveDfc() calling CheckResponse ") );
	        CheckResponse( isiTester->iReceiveMsg );
		    	}
	    // Other messages in callback
			else if(isiTester->iCallBackPtr)
					{
       		COMPONENT_TRACE(("KisiTester::ReceiveDfc() other messages in callback ") );
					static_cast<KisiTesterHandler*>(isiTester->iCallBackPtr)->CallBack(isiTester->iReceiveMsg);
	  			}
	    }
		
    COMPONENT_TRACE(("KisiTester::ReceiveDfc() calling receive ") );
    // Driver can now start to receive messages from ISA side
  
    TInt r = isiTester->iIsaKernelIf->Receive( isiTester->iStatus, *(isiTester->iReceiveMsg),
                                                    isiTester->iNeededLength, &(isiTester->iReceiveDfc) );
                                                    
    // Can assert always to show something is wrong because receive failed
    __ASSERT_ALWAYS( r == KErrNone, Kern::Fault("ReceiveDfc", r) );	

		COMPONENT_TRACE(("KisiTester::ReceiveDfc() returning") );	
}


        

 


//  End of File
