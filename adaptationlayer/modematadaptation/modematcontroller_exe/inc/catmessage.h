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



#ifndef CATMESSAGE_H
#define CATMESSAGE_H

class CModemAtSession;

/**
  * CAtMessage is used, when sending data from session to CModemAtHandler. CAtMessage stores
  * message data. Class is used by CModemAtSession. 
  */
class CAtMessage  : public CBase
    {
public:    
    
    /** Constructor.
     * @param aMessagetype AT-command or NVRAM-status
     * @param aCmd AT-command
     */
    CAtMessage( const TUint8 aMessageType, CModemAtSession* aSession, const RMessage2& aRequest );
    
    /** Destructor.
     *
     */
    ~CAtMessage();
    
    /** GetBuffer
     * @return Pointer to the buffer
     */
    TPtr8 GetBuffer();
    
    /** GetMessageType
     * @return messagetype
     */
    TUint8 GetMessageType();

    /** ReceiveATResponse
     * @param aResponse Response to AT-command
     * @param aComplete Intermediate data is not completed until it has a response. AT command is completed.
     * @return
     */
    void ReceiveATResponse( const TDesC8& aResponse, TBool aComplete );
	  
	  /** CancelRequest
     * 
     */
	  void CancelRequest();
	  
	  /** GetSession
     * @return RMessage2 session pointer
     */
    CModemAtSession* GetSession();

private:    
    HBufC8* iMessage;
    TUint8 iMessageType;
    CModemAtSession* iSession;
    RMessage2 iRequest;
    TPtr8 iMessagePtr;
    };

#endif // CATMESSAGE_H

