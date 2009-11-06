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



#ifndef DPFLOWCTRL_H
#define DPFLOWCTRL_H

// INCLUDES
#include <iscapi.h>

#include "dpobserver.h" // observer base class

// CONSTANTS
// none

// MACROS
// none

// DATA TYPES
// none

// FUNCTION PROTOTYPES
// none

// FORWARD DECLARATIONS
// none

// CLASS DECLARATION

/**
*  CDpFlowCtrl controls flow between DataPort (DP) and DCS.
*  Dp2Dcs flow control is used, when DCS enables or disables flow control.
*  Dcs2Dp flow control is used, when low and high watermarks are reached.
*  Dcs2Dp flow control is enabled, when DataPort or Isc Api's proxy reached
*  high water mark. Dcs2Dp flow control is disabled, when DataPort reaches low
*  water mark, if Isc Api's proxy has not reached high water mark. Dcs2Dp flow
*  control is also disabled, when Isc Api's proxy reaches low water mark.
*/
class CDpFlowCtrl : public CActive
    {

    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * Reset FlowCtrlStatus to low watermark reached and 
        * adds CActive Object to scheduler
        * @param aDataPort: Dataport main class
        */
        CDpFlowCtrl( CDpDataPort& aDataPort );

        /**
        * Destructor.
        */
        ~CDpFlowCtrl();

    public:  // New methods

        /**
        * This method returns Dp2Dcs flow control status.
        * @return TFlowControl: Dp2Dcs flow control status
        */
        inline TFlowControl FlowCtrlDp2Dcs();

        /**
        * This method returns Dcs2Dp flow control status.
        * @return TFlowControl: Dcs2Dp flow control status
        */
        inline TFlowControl FlowCtrlDcs2Dp();

        /**
        * This method gets Dp2Dcs flow control status.
        * @param aFlowControl: On return, aFlowControl contains
        * flow control status.
        */
        void GetDp2DcsFlowControlStatus( TFlowControl& aFlowControl );

        /**
        * This method enables/disables flow control change notifying. This
        * method also completes notify request, if notifying is enabled,
        * and previously notified Dp2Dcs flow control status differs from
        * current status.
        * @param aEnableNotify: Is flow control enabled?
        */
        void SetFlowControlNotify( const TBool aEnableNotify );

        /**
        * This method sets Dcs2Dp flow OFF and sends flow control change to
        * DCS, if Isc Api's proxy has reached low water mark. This method
        * notifies flow control observers. This method also notifies the C32
        * client, if the client has ordered flow control change notification.
        */
        void WaterMarkLowReached();

        /**
        * This method sets Dcs2Dp flow ON and sends flow control change to
        * DCS. This method notifies flow control observers. This method also
        * notifies the C32 client, if the client has ordered flow control
        * change notification.
        */
        void WaterMarkHighReached();

        /**
        * This method notifies the C32 client about Dp2Dcs flow control change,
        * when starting DataPort.
        */
        void NotifyClientDatapotReadyforDteFlow();

        /**
        * This method requests Isc Api pipe data message buffer status.
        * Result is handled in RunL():
        */
        void RequestIscFlowControlNotification();

        /**
        * Attach observer to CDpSubject.
        * @param aObserver: Observer
        */
        void Attach( CDpObserver& aObserver );

        /**
        * Detach observer from CDpSubject.
        * @param aObserver: Observer
        */
        void Detach( CDpObserver& aObserver );

        /**
        * Notify all observers of CDpSubject object.
        */
        void Notify();

    public:  // Methods from base classes

        // From CActive

        /**
        * From CActive This method cancels pending request with KErrCancel.
        */
        void DoCancel();

        /**
        * From CActive This method sets water mark according Isc Api' proxy's
        * flow control status, if Dcs2Dp flow control status is not already
        * set. Isc Api pipe data message buffer request is renewed.
        */
        void RunL();

        /**
        * From CActive This method handles leave in RunL().
        * @param aError: Leave reason
        * @return TInt: Leave reason is returned in error code
        */
        TInt RunError( TInt aError );

    protected:  // New methods

        //none

    protected:  // Methods from base classes

        //none

    private: // Methods

        /**
        * This method informs client about the flow control change.
        */
        void InformFlowControlToClient();

    public:  // Data

        // none

    protected:  // Data

        // none

    private:  // Data

        // Flow control subject
        CDpSubject iDpSubject;

        // Serial protocol port
        CDpDataPort& iDataPort;

        // Isc Api message buffer status. We request this status change
        // from Isc Api to get information from Isc Api pipe data
        // message buffer status.
        TInt iIscFlowCtrlStatus;

        // Pipe interface (PIF)
        CDpPif& iPifDcs;

        // Dp2Dcs flow control status
        // TFlowControl values:
        // EFlowControlOn: Flow control is ON, ie data is inhibited
        // EFlowControlOff: Flow control is OFF, ie data is being transmitted
        // Related to ISC flow control as it is from Dte TO Dcs
        TFlowControl iFlowCtrlDp2Dcs;

        // Dcs2Dp flow control status
        // Related to Dataport flow control as it is from Dcs TO Dte
        //this means that Flow control is ON ( Dataport cannot received more data)
        // when Dataport internal buffer are full
        // and flow control is OFF when there is room into Dataport internal buffer¨
        // and Dataport can received more data.
        TFlowControl iFlowCtrlDcs2Dp;

        // Last flow control status sent to Dataport client
        TFlowControl iLastFlowControlSentToClient;

        // Has DTE ordered flow control notify
        TBool iFlowControlNotify;

        // EFalse until whe have notified DTE first time (startup)
        TBool iIsDataportReadyforDteFlow;

    };

#include "dpflowctrl.inl" // inline methods

#endif  // DPFLOWCTRL_H

// End of File
