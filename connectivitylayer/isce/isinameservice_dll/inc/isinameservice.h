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


#ifndef __ISINAMESERVICE_H__
#define __ISINAMESERVICE_H__

#include "misirouterobjectif.h"
#include "misicommunicationmanagerif.h"

#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
class DISINameServiceExtended;
#define ISCE_INTERNAL_EXTENDED_RESOURCE_ROUTING_SUPPORTED
#endif

class MISIObjectRouterIf;
class DNameRecords;

/*
* ISI router.
*/
NONSHARABLE_CLASS( DISINameService ) : public DBase, public MISIRouterObjectIf, public MISICommunicationManagerIf
    {

    public:

        /*
        * Default contructor
        */
        DISINameService();

        /*
        * Destructor
        */
        ~DISINameService();

        /*
        * See comments from MISIRouterObjectIf
        */
        void Receive( const TDesC8& aMessage );

        /*
        * See comments from MISICommunicationManagerIf
        */
        static TBool IsValidResource( const TDesC8& aMessage );

        /*
        * See comments from MISICommunicationManagerIf
        */
        static TUint8 GetDeviceIdentifierByResource( const TUint32 aResourceIdentifier );

        /*
        * Return device identifier for a given resource identifier
        * @param aResourceIdentifier
        * @return device identifier
        */
        TUint8 GetDeviceIdByResource( const TUint32 aResourceIdentifier );

        /*
        * Return device identifier for a given resource identifier
        * @param aMessage, message which contains the resource to check
        * @return ETrue if resource was valid
        */
        TBool HandleResourceValidationCheck( const TDesC8& aMessage );

        /*
        * Returns a pointer to self.
        * If pointer does not exist shall assert and throw a fault.
        * @return DISINameService*
        */
        static DISINameService* GetISINameServiceF();

        /*
        * Returns pointer to name records.
        * If pointer does not exist shall assert and throws a fault.
        * @return DNameRecords*
        */
        DNameRecords* GetNameRecordsF();

    private:

        /*
        * Construct COMM_ISA_ENTITY_NOT_REACHABLE_RESP message and
        * send message to router.
        */
        void BuildAndSendCommIsaEntityNotReachableResp( const TDesC8& aMsg );

        void HandlePnsNameMessage( const TDesC8& aMessage );
        void HandleNameQueryReq( const TDesC8& aPnsNameQueryReq );
        void HandleNameRemoveReq( const TDesC8& aPnsNameRemoveReq );
        void HandleNameAddReq( const TDesC8& aPnsNameAddReq );
        void HandleCommonMessage( const TDesC8& aCommonMessage );

        void SendCommServiceNotIdentifiedResp( TDes8& aCommServiceNotIdentifiedRespMsg );
        void SendCommIsiVersionGetResp( TDes8& aCommIsiVersionGetRespMsg );

        // Extended resource identifier support for NameService
        TUint32 GetResourceUsedInMessage( const TDesC8& aMessageToBeResourceRouted );

        void SetIsiHeaderForCommonResp( const TDesC8& aResponseMessage,
                                        const TDesC8& aRequestMessage );

        void SetIsiHeaderForResp( const TDesC8& aResponseMessage,
                                  const TDesC8& aRequestMessage,
                                  const TUint8 aResponseMessageIdentifier );

        void SetIsiHeaderForIndication( const TDesC8& aIndicationToModify,
                                        const TDesC8& aPnsNameRequest,
                                        const TUint8 aIndicationMessageIdentifier );

        void TxPnsNameAddInd( const TDesC8& aPnsNameAddReq,
                              const TUint32 aName,
                              const TUint16 aPhoNetAddress,
                              const TUint8 aFlags );

        void TxPnsNameRemoveInd( const TDesC8& aPnsNameRemoveReq,
                                 const TUint32 aName,
                                 const TUint16 aPhoNetAddress,
                                 const TUint8 aFlags );

#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    public:
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
        void SendToRouter( TDes8& aMessage );

     // Because accessed from MISICommunicationManagerIf::IsValidResource function.
     public:

        void LockNameServiceDMutex();
        void FreeNameServiceDMutex();

#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    private:
        DISINameServiceExtended* iISINameServiceExtended;
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
    private:

        // Not owned.
        MISIObjectRouterIf*     iRouter;
        // Owned
        static DMutex*          iNameServiceMutex;
        // Owned
        DNameRecords*           iNameRecords;
        // Owned
        static DISINameService* iThisptr; 
        TUint8                  iObjId;

    };

#ifdef ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE
class DISINameService;

/*
* ISI router.
*/
NONSHARABLE_CLASS( DISINameServiceExtended ) : public DBase
    {

    private: 
        
        enum TStateOfSubscription
            {
            ENotSubscribed = 0,
            ESubscribed,
            EDone,
            };

    public:

        /*
        * Default contructor
        */
        DISINameServiceExtended( DISINameService* aNameService );

        /*
        * Destructor
        */
        ~DISINameServiceExtended();

        /*
        * Responsibility to deallocate message is transferred.
        * @aPnsNameAddInd, message
        */
        void RxPnsNameAddInd( const TDesC8& aPnsNameAddInd );

        /*
        * Ownership of the message is not transferred.
        * @param aMessage, message where to check the device id
        * @return ETrue if own device
        */
        TBool IsRxDeviceOwnDevice( const TDesC8& aMessage );

        /*
        * Check is the ape commgr subscribe response message already received
        * @return ETrue if not received ape commgr subscribe resp already
        */
        TBool ApeCommgrSubscribeRespNotReceived();

        /*
        * @return ETrue if not subsribed name add ind message
        */
        TBool NotSubscribedNameAddIndAlready();

        /*
        * Receive the ape commgr subscribe response.
        * Ownership of the message is transferred.
        * @param aMessage
        */
        void RxApeCommgrSubscribeResp( const TDesC8& aMessage );

        /*
        * Subscribes pns name add ind
        */
        void SubscribePnsNameAddInd();

        /*
        * Modifies communication manager resource to contain also device identifier
        * [0]=PN_COMMGR [1]=Device Identifier [2]=don't care [3]=don't care
        * @param aCommgrResource, reference to resource, which value shall be modified
        * @param aDeviceIdentifier
        */
        void ModifyCommgrResource( TUint32& aCommgrResource, const TUint8 aDeviceIdentifier );

    private:

        void AddModemResources();
        
        //void AddPnDevDspMainResources();

        void HandleApeCommgrSubscribeResp( const TDesC8& aApeCommGrSubscribeResp );

        //void HandleNameAddInd( const TDesC8& aNameAddInd );

        //void SubscribeNameAddInd();

        // Not owned
        DISINameService*        iISINameService;
        TStateOfSubscription    iShNameAddIndSubscriptionState;

    };
#endif // ISCE_INTERNAL_WIDER_NAME_SERVICE_IN_USE


#endif /* __ISINAMESERVICE_H__ */

