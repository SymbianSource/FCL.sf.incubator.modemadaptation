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

#ifndef __ISICLTRANSCEIVER_H__
#define __ISICLTRANSCEIVER_H__

#include "misilinkrouterif.h"           // For MISILinkRouterIf
#include "isiinternaldefs.h"            // For KMaxNumOfObjId
#include "misirouterservicerouterif.h"  // For MISIRouterServiceRouterIf

class MISIRouterLinkIf;
class DISIRouter;
class DIsiRouterService;
class DISIMsgQueue;

NONSHARABLE_CLASS( DISICLTransceiver ) : public DBase, public MISILinkRouterIf, public MISIRouterServiceRouterIf
    {

    public:

        DISICLTransceiver( DISIRouter& aRouter, TDfcQue* aThreadPtr );

        ~DISICLTransceiver();

        // See comments from MISILinkRouterIf
        TInt RouteISIMessage( TDes8& aMessage, TBool aDynamicSenderCheckNeeded = ETrue );
        
        void ReceiveISIMessage( const TDesC8& aMessage, const TUint8 aTrxId );

        // See comments from MISILinkRouterIf
        void StateChanged();

        // From MISIRouterServiceRouterIf
        void GetDeviceConnectionStates( RArray<TUint8>& aDeviceIdentifierList, const TBool aConnectedDevice );

    private:

        static void StateChangedDfc( TAny* aPtr );
        
        static void RxQueueDfc( TAny* aPtr );

        TInt ValidateISIMessage( TDes8& aMessage );

        void ReadStateChanges();

        // Responsibility to deallocate the message is transferred.
        TInt SendCommIsaEntityNotReachableResp( TDes8& aNotDeliveredMessage );
        
        TBool DynamicDevice( const TUint8 aDevId );
        
        void UpdateDynamicDeviceTable( const TUint8 aDynamicDevId, const TUint8 aStaticDevId );
        
        void DynamicToStaticDevice( TUint8& aRxDev );

        NONSHARABLE_CLASS( DISIDevice ) : public DBase
            {
            public:

                DISIDevice( const TUint8 aDeviceIdentifier, MISIRouterLinkIf* aLink );
                ~DISIDevice();

                MISIRouterLinkIf* GetLink();
                TUint8 GetDeviceIdentifier();
                TBool GetDeviceState();
                TBool GetDeviceOldState();
                void SetDeviceOldState( TBool aConnected );
                // Shall be called before accessing operations.
                void LockDeviceFM();
                // Shall be called after accessing operations.
                void FreeDeviceFM();

            private:

                // Owned, synch internally
                NFastMutex*                 iShDeviceMutex;
                // Not owned, synch by above mutex.
                MISIRouterLinkIf*   iShLink;
                // Synch by above mutex.
                TUint8              iShDeviceIdentifier;
                // Synch by above mutex.
                TBool               iShOldDeviceConnectedState;

            };
            
                //private:
        NONSHARABLE_CLASS( TDynamicDevice )
            {
            public:
                TDynamicDevice( const TUint8 aDynamicDevId, TUint8 aStaticDevId ){iDynamicDevId = aDynamicDevId; iStaticDevId = aStaticDevId;};
                TUint8 iDynamicDevId;
                TUint8 iStaticDevId;
            };

        // Additional coding convention iSh-prefix means a member variable shared between N threads.
        // Owned, called in N thread context
        RPointerArray<DISIDevice> iShDevices;
        // Called in N thread context
        DISIRouter&               iShRouter;
        // Owned, no need to synch only accessed from below DFC function.
        DIsiRouterService*        iRouterService;
        // Owned, accessed from N threads no need to synch synch handled internally in TDfc class 
        TDfc*                     iShStateChangedDfc;
        
        TDfc*                     iRxQueueDfc;
        
        RArray< TDynamicDevice* > iDynamicDeviceTable;
        
        DISIMsgQueue*             iRxQueue;
        
        static DMutex*            iDynamicDeviceTableMutex;
       
        TUint8                    iDevPcLastSendTrxId;
        TUint8                    iDevPcLastActiveTrxId;
    };

#endif /* __ISICLTRANSCEIVER_H__ */

