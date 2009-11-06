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



//- Include Files  ----------------------------------------------------------
#include "kisitester_handler.h"
#include "kisitester.h"
#include "kisi_trace.h"


//- Namespace ---------------------------------------------------------------


//- Using -------------------------------------------------------------------


//- External Data -----------------------------------------------------------


//- External Function Prototypes --------------------------------------------


//- Constants ---------------------------------------------------------------


//- Macros ------------------------------------------------------------------


//- Global and Local Variables ----------------------------------------------


//- Local Function Prototypes -----------------------------------------------


//===========================================================================

//- Local Functions ---------------------------------------------------------


//===========================================================================

//- Member Functions --------------------------------------------------------

//---------------------------------------------------------------------------
//
//    Constructor
//
KisiTesterHandler::KisiTesterHandler()
        : DBase()
        ,iKisiTester(NULL)
   {
    COMPONENT_TRACE(("KISITESTERHANDLER::KisiTesterHandler constructor") );
   }


//---------------------------------------------------------------------------
/**
    Destructor. For kernel extensions this is never called!
*/
KisiTesterHandler::~KisiTesterHandler()
    {
    COMPONENT_TRACE(("KISITESTERHANDLER::~KisiTesterHandler destructor") );
    delete iKisiTester;
    }


//---------------------------------------------------------------------------
/**
    Second phase constructor. Creates KisiTester.

    @return KErrNone, if successful
*/
TInt KisiTesterHandler::DoCreate()
    {
    COMPONENT_TRACE(("KisiTesterHandler::DoCreate") );
  	TInt ret(KErrNoMemory);

	  iKisiTester = new KisiTester();
	  if(iKisiTester)
		    {
        COMPONENT_TRACE(("KisiTesterHandler::DoCreate calling StartTesting()") );			  
				iKisiTester->StartTesting();
				
        COMPONENT_TRACE(("KisiTesterHandler::DoCreate calling Init()") );			
	      ret = iKisiTester->Init();
	       
	       if(ret==KErrNone)
		        {
			      iKisiTester->Subscribe(this);
		        }	
 	      }			
 	  else delete iKisiTester;

    COMPONENT_TRACE(("KisiTesterHandler::DoCreate returning") );		
    return ret;

    }
    

//---------------------------------------------------------------------------
// Callback method, executed after Message received to theinto next state.
// @param  aArg    Pointer to Phonet message.
//---------------------------------------------------------------------------
void KisiTesterHandler::CallBack(TAny* aArg)
    {
    COMPONENT_TRACE(("KisiTesterHandler::CallBack() ") );		    
	__ASSERT_DEBUG( aArg!=NULL, Kern::Printf("KisiTesterHandler::CallBack() Parameter is NULL!!") );
	
	//DPnMsg* pnMsg = static_cast<DPnMsg*>(aArg);
//	if( pnMsg && 
	//   (pnMsg->Ptr()->Length() > KPnMsgUserDataBase+KTkbdMsgKeyCode) && (pnMsg->MessageId() == KTkbdMessageId ))
	  //{
		//TUint8 keyCode = pnMsg->UserData()[KTkbdMsgKeyCode];
		//CharacterIn(keyCode);
   	//	}
	    COMPONENT_TRACE(("KisiTesterHandler::CallBack() returning") );	
	}
	
	
//===========================================================================

//- Global Functions --------------------------------------------------------

//---------------------------------------------------------------------------
/**
    The entry point for a standard extension. Creates KisiTesterHandler extension.

    @return KErrNone, if successful
*/
DECLARE_STANDARD_EXTENSION()    //lint !e960 !e1717 ¤/#((& Symbian
    {

    COMPONENT_TRACE(( "KISITESTERHANDLER::Starting KisiTesterHandler extension") );
	
	  KisiTesterHandler* theKisiTesterHandler(NULL);
	  
    TInt ret( KErrNoMemory );

    theKisiTesterHandler = new KisiTesterHandler;

    if ( theKisiTesterHandler )
        {
        ret = theKisiTesterHandler->DoCreate();        
        }
		else
				{
				delete theKisiTesterHandler;
				}				
		
    COMPONENT_TRACE(( "KISITESTERHANDLER::KisiTesterHandler extension returning") );
    		
    return ret;

    }


//- Namespace ---------------------------------------------------------------

// End of File
