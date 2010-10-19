/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef CMMPACKETCONTEXTMESSHANDLERLIST_H
#define CMMPACKETCONTEXTMESSHANDLERLIST_H

// INCLUDES
#include <etelpckt.h>
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "iscapi.h"

// CONSTANTS
_LIT(KStringExternal,   "External");  // First external context
_LIT(KStringExternal2,  "External2"); // Second external context

const TUint8 KTUint8NotDefined = 0xFF;
const TInt KTIntNotDefined = 0xFF;

// MACROS
    // None

// DATA TYPES
    // Context Info List contains TMmContextInfo elements.
    class TMmContextInfo
        {
        public:
            TUint8 iContextId;
            TUint8 iChannelId;
            TUint8 iObjectId;

            TInfoName iContextName;
            TInfoName iHostContextName;

            TUint8 iPipeHandle;
            TInt iPipeStatus;

            TUint8 iContextType;

            TUint8 iCreateContextIdTransId;
            TUint8 iPipeCreateTransId;
            TUint8 iGpdsLlConfigureTraId;
            TUint8 iRemovePipeTraId;

            TBool iInitialiseStarted;

            TInt iContextConfigurationType;

            RPacketContext::TContextConfigGPRS iConfig;
            RPacketContext::TContextConfigR99_R4 iConfig99;

            TUint8 iActivationTraId;
            TUint8 iPipeResetTraId;
            TUint8 iDeleteTraId;

            TBool iAddDNSAddress;
            TBool iAddPCSCFAddress;

            RIscApi iIscApi;
            TBool iChannelOpened;

            TBool iLoanReturning;
            TUint8 iExtDevId;
            TUint8 iExtObjId;
        };

// FUNCTION PROTOTYPES
    // None

// FORWARD DECLARATIONS
    // None

// CLASS DECLARATION
/**
*  Contains information list for Packet Context DOS TSY context objects.
*  This class provides the methods for the Packet Context Message Handler
*  List object (CMmPacketContextList class) to keep track of opened contexts.
*  When a Context object is opened it should be put on the Service's Context
*  List.
*/
class CMmPacketContextMesshandlerList : public CBase
    {
    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        * @return CMmPacketContextMesshandlerList*: pointer to the packet
        *     context message handler list
        */
        static CMmPacketContextMesshandlerList* NewL();

        /**
        * Destructor.
        */
        ~CMmPacketContextMesshandlerList();

    public: // New functions

        /**
        * Adds context info to the context info list.
        * @param aObjectId: object id
        * @param aChannelId: channel id (proxy id)
        * @param aContextName: name of the context
        * @param aHostContextName: name of the host context
        * @return TInt: KErrNone or error code
        *     (KErrArgument: Maximum number of contexts already
        *     created or channel id not found from the proxy id list)
        */
        TInt AddObject(
            const TInt aObjectId,
            const TUint8 aChannelId,
            const TInfoName& aContextName,
            const TInfoName& aHostContextName );

        /**
        * Removes context info from the context info list.
        * @param aContextId: context id to find information to be removed.
        * @return TInt: KErrNone or error code
        *     (KErrArgument: Context not in context info list or
        *     channel id not found from the proxy id list)
        */
        TInt RemoveObject(
            TUint8 aContextId );

        /**
        * Returns number of created contexts.
        * @return TInt: number of created contexts in the context info list
        */
        TInt NumberOfContexts() const;

        /**
        * Sets context id to the context info list.
        * @param aContextId: context id
        * @param aTransactionId: transaction id
        * @return TInt: KErrNone or error code
              (KErrNotFound: context not found from the list)
        */
        TInt SetContextId(
            const TUint8 aContextId,
            const TUint8 aTransactionId );

        /**
        * Gets context type by context name.
        * @param aContextName: name of the context
        * @return TUint8: context type
        */
        TUint8 GetContextTypeByName(
             const TInfoName* const aContextName );

        /**
        * Gets context name by context id.
        * @param aContextId: context id
        * @param aContextName: name of the context
        * @return TInt: KErrNone or error code
        *     (KErrArgument: context not found, invalid context id)
        */
        TInt GetContextNameByContextId(
            const TUint8 aContextId,
            TInfoName& aContextName );

        /**
        * Gets host context name by context name.
        * @param aContextName: name of the context
        * @return TInfoName: name of the host context
        */
        TInfoName GetHostCidName(
            TInfoName* aContextName ) const;

        /**
        * Gets pipehandle by context name.
        * @param aContextName: name of the context
        * @return TUint8: pipe handle
        *     (KInvalidPipeHandle if not found)
        */
        TUint8 PipeHandleByContextName(
            const TInfoName* aContextName );

        /**
        * Gets proxy id by context name.
        * @param aContextName: name of the context
        * @return TUint8: proxy id (channel id)
        */
        TUint8 ProxyIdByContextName(
            const TInfoName* aContextName );

        /**
        * Gets proxyid by pipehandle.
        * @param aPipeHandle: pipe handle
        * @return TUint8: proxy id (channel id)
        */
        TUint8 ProxyIdByPipeHandle( TUint8 aPipeHandle );

        /**
        * Opens channel with the specified id.
        * @param aChannelId channel ID
        * @return TInt KErrNone or system-wide error code
        */
        TInt OpenChannel( TUint8 aChannelId );

        /**
        * Closes channel with the specified id.
        * @param aChannelId channel ID
        * @return TInt KErrNone or system-wide error code
        */
        TInt CloseChannel( TUint8 aChannelId );

        /*
        * Get context name by proxy id.
        * @param aProxyId: proxy id (channel id)
        * @param aContextName: name of the context
        * @return TInt: KErrNone or error code
        *     (KErrArgument: context not found, invalid proxy id)
        */
        TInt ContextNameByProxyId(
            const TUint8 aProxyId,
            TInfoName& aContextName );

        /**
        * Sets iCreateContextIdTransId of the specified context by context name.
        * @param aTransactionId: transaction id
        * @param aContextName: name of the context
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetCreateContextIdTransId(
            const TUint8 aTransactionId,
            const TInfoName* aContextName );

        /**
        * Sets iPipeCreateTransId of the specified context by context name.
        * @param aTransactionId: transaction id
        * @param aContextName: name of the context
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetPipeCreateTransId(
            const TUint8 aTransactionId,
            const TInfoName* aContextName );

        /**
        * Sets iGpdsLlConfigureTraId (Local Link) of the specified context
        * by context id.
        * @param aTransactionId: transaction id
        * @param aContextId: context id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetGpdsLlConfigureTraId(
            const TUint8 aTransactionId,
            const TUint8 aContextId );

        /**
        * Sets the iPipeRemoveTraId specified by pipe handle.
        * @param aTransactionId: transaction id
        * @param aPipeHandle: pipe handle
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetRemovePipeTraId(
            const TUint8 aTransactionId,
            const TInt aPipeHandle );

        /**
        * Sets the iActivationTraId specified by context id.
        * @param aContextId: context id
        * @param aTransactionId: transaction id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetActivationTraId(
            const TUint8 aContextId,
            const TUint8 aTransactionId );

        /**
        * Gets transaction used in activation
        * @param aContextId: context id
        * @return TUint8: activation transaction id
        *     (KUInt8NotDefined if not found)
        */
        TUint8 GetActivationTraId(
            TUint8 aContextId );

        /**
        * Gets and resets transaction used in activation
        * @param aContextId: context id
        * @return TUint8: activation transaction id
        *     (KUInt8NotDefined if not found)
        */
        TUint8 GetAndResetActivationTraId(
            const TUint8 aContextId );

        /**
        * Sets the iDeleteTraId specified by context id.
        * @param aContextId: context id
        * @param aTransactionId: transaction id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetDeletionTraId(
            const TUint8 aContextId,
            const TUint8 aTransactionId );

        /**
        * Gets and resets transaction used in deletion
        * @param aContextId: context id
        * @return TUint8: delete transaction id
        *     (KUInt8NotDefined if not found)
        */
        TUint8 GetAndResetDeletionTraId(
            const TUint8 aContextId );

        /**
        * Sets pipe reset transaction id for specific context by context id.
        * @param aContextId: context id
        * @param aTransId: transaction id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetPipeResetTraId(
            const TUint8 aContextId,
            const TUint8 aTransId );

        /**
        * Checks and resets transaction id used in pipe reset
        * @param aTransId: transaction id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt CheckAndResetPipeResetTraId(
            const TUint8 aTransId );

        /**
        * Gets CreateContextId TransActionId of the specified context by
        * pipe handle
        * @param aPipeHandle: pipe handle
        * @return TUint8: create context id transaction id
        *     (KUInt8NotDefined if not found)
        */
        TUint8 CreateContextIdTransIdByPipeHandle(
            const TInt aPipeHandle );

        /**
        * Gets CreateContextIdTransId of the specified context by ContextId
        * @param aContextId: context id
        * @return TUint8: create context id transaction id
        *     (KUInt8NotDefined if not found)
        */
        TUint8 CreateContextIdTraIdByContextId(
            const TUint8 aContextId );

        /**
        * Gets Pipehandle of the specified context by context id
        * @param aContextId: context id
        * @return TUint8: pipe handle
        *     (KInvalidPipeHandle if not found)
        */
        TUint8 PipeHandleByContextId(
            const TUint8 aContextId );

        /**
        * Checks if transaction id created early match with given.
        * @param aTransId: transaction id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: if not found)
        */
        TInt PipeCreateTransIdOkByTransId(
            const TUint8 aTransId );

        /**
        * Checks if transaction id created early match with given.
        * @param aTransId: transaction id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: if not found)
        */
        TInt CreateContextIdTraIdOK(
            const TUint8 aTransId);

        /**
        * Checks if transaction id created early match with given.
        * @param aTransId: transaction id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: if not found)
        */
        TInt GpdsLlConfigureTraIdOK(
            const TUint8 aTransId );

        /**
        * Sets pipehandle specified by pipe create transaction Id.
        * @param aTransactionId: transaction id
        * @param aPipeHandle: pipe handle
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetPipeHandle(
            const TUint8 aTransactionId,
            const TUint8 aPipeHandle );

        /**
        * Gets context Id specified by context name.
        * @param aContextName: context name
        * @return TUint8: context id
        *     (GPDS_CID_VOID: context not found)
        */
        TUint8 GetContextIdByContextName(
            TInfoName* const aContextName );

        /**
        * Reset pipe handle and status by pipehandle.
        * @param aPipeHandle: pipe handle
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt ResetPipeHandleAndStatus(
            const TUint8 aPipeHandle );

        /**
        * Sets initialisation member for specific context by context Id.
        * @param aContextId: context id
        * @param aFlag: initialisation member value flag
        * @return TBool: ETrue if set, EFalse if context not found
        */
        TBool SetInitialiseMember(
            const TUint8 aContextId,
            const TBool aFlag );

        /**
        * Gets initialisation member for specific context by pipehandle.
        * @param aPipeHandle: pipe handle
        * @return TBool: initialisation member value
        *     (EFalse also if context not found)
        */
        TBool GetInitialiseMember(
            const TInt aPipeHandle );

        /**
        * Gets contextId of the specified context by Pipehandle.
        * @param aPipeHandle: pipe handle
        * @return TUint8: context id
        *     (GPDS_CID_VOID: context not found)
        */
        TUint8 ContextIdByPipeHandle(
            const TInt aPipeHandle );

        /**
        * Reset Context id to GPDS_CID_VOID.
        * @param aContextId: context id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt RemoveContextId(
            const TUint8 aContextId );

        /**
        * Sets configuration type by contextId.
        * @param aContextId: context id
        * @param aConfigType: context configuration type
        * @return TBool: ETrue if set, EFalse if context not found
        */
        TBool SetContextConfigurationType(
            const TUint8 aContextId,
            const TInt aConfigType );

        /**
        * Gets configuration type by contextId.
        * @param aContextId: context id
        * @return TInt: context configuration type
        */
        TInt GetContextConfigurationType(
            const TUint8 aContextId );

        /**
        * Saves configuration for specific context.
        * @param aContextId: context id
        * @param aConfigurationType: context configuration type
        * @param aConfig: TPacketDataConfigBase::KConfigGPRS configuration
        * @param aConfig99: TPacketDataConfigBase::KConfigRel99Rel4 configuration
        * @return TInt: KErrNone or error code
        *     (KErrArgument: context not found, invalid context id
        *     or confguration type not supported)
        */
        TInt SaveConfig(
            const TUint8 aContextId,
            const TInt aConfigurationType,
            RPacketContext::TContextConfigGPRS* aConfig,
            RPacketContext::TContextConfigR99_R4* aConfig99 );

        /**
        * Get configuration for context by context Id and configuration type.
        * @param aContextId: context id
        * @param aConfigurationType: context configuration type
        * @return TPacketDataConfigBase*: pointer to the configuration
        *     (TPacketDataConfigBase::KConfigGPRS or
        *     RPacketContext::TContextConfigR99_R4)
        */
        TPacketDataConfigBase* GetConfig(
            const TUint8 aContextId,
            const TInt aConfigurationType );

        /**
        * Sets pipe status specified by pipe handle.
        * @param aPipeHandle: pipe handle
        * @param aPipeStatus: pipe status
        * @return TBool: ETrue if set, EFalse if context not found
        */
        TBool SetPipeStatus(
            const TUint8 aPipeHandle,
            const TInt aPipeStatus );

        /**
        * Gets pipe status specified by context Id.
        * @param aContextId: context id
        * @return TInt: pipe status
        *     (PN_PIPE_DISABLE also if context not found)
        */
        TInt GetPipeStatus(
            const TUint8 aContextId );

        /**
        * Gets proxy Id specified by context Id.
        * @param aContextId: context id
        * @return TUint8: proxy id (channel id)
        *     (GPDS_CID_VOID also if context not found)
        */
        TUint8 ProxyIdByContextId(
            const TUint8 aContextId );

        /**
        * Sets context Id and name for dial-up context.
        * @param aContextId: context id
        */
        void SetDialUpContextId(
            const TUint8 aContextId );

        /**
        * Gets context Id for dial-up context.
        * @return TUint8: dial-up context id
        */
        TUint8 GetDialUpContextId();

        /**
        * Gets dial-up context name specified by context Id.
        * @param aContextId: context id
        * @param aContextName: context name
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt DialUpContextName(
            const TUint8 aContextId,
            TInfoName& aContextName );

        /**
        * Gets dial-up context id specified by context name.
        * @param aContextName: context name
        * @return TUint8: context id
        *     (GPDS_CID_VOID also if context name is invalid)
        */
        TUint8 GetDialUpContextIdByName(
            TInfoName* const aContextName );

        /**
        * Removes buffered information of the context specified by name.
        * @param aContextName: context name
        */
        void RemoveObjectByName(
            TInfoName* aContextName );

        /**
        * Adds information to specified context about DNS
        * @param aContextId: context id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetDNSAddressInfoToContextInfo(
            TUint8 aContextId );

        /**
        * Gets information to specified context about DNS
        * @param aContextId: context id
        * @return TBool: DNS related information value
        *     (EFalse also if context not found)
        */
        TBool GetDNSAddressInfoToContextInfo(
            TUint8 aContextId );

        /**
        * Adds information to specified context about PCSCF
        * @param aContextId: context id
        * @return TInt: KErrNone or error code
        *     (KErrNotFound: context not found)
        */
        TInt SetPCSCFAddressInfoToContextInfo(
            TUint8 aContextId );

        /**
        * Gets information to specified context about PCSCF
        * @param aContextId: context id
        * @return TBool: PCSCF related information value
        *     (EFalse also if context not found)
        */
        TBool GetPCSCFAddressInfoToContextInfo(
            TUint8 aContextId );

        /**
        * Gets context Id specified by context list index.
        * @param aListIndex: list index
        * @return TUint8: context id (GPDS_CID_VOID if context not found)
        */
        TUint8 ContextIdByListIndex( const TInt aListIndex );

        /**
        * Gets context type by context id.
        * @param aContextId: id of the context
        * @return TUint8: context type
        */
        TUint8 GetContextTypeById( const TUint8 aContextId );

        /**
        * Sets boolean to indicate returning of pipe loan.
        * @param aPipeHandle: pipe handle
        * @param aFlag: ETrue if returning loan, otherwise EFalse
        * @return TBool: ETrue if set, EFalse if context not found
        */
        TBool SetLoanReturning(
            TUint8 aPipeHandle,
            TBool aFlag );

        /**
        * Gets boolean indicating about pipe loan returning.
        * @param aPipeHandle: pipe handle
        * @return TBool: ETrue if set, EFalse if context not found
        */
        TBool IsLoanReturning(
            TUint8 aPipeHandle ) const;

        /**
        * Sets deviceId and objectId of loaning pep.
        * @param aChannelId: channel id
        * @param aDevId: device id
        * @param aObjId: object id
        * @return TInt: KErrNone if succesful,
        * KErrNotFound if no matching channels in context list.
        */
        TInt SetExternalDevice( 
            TUint8 aChannelId, 
            TUint8 aDevId, 
            TUint8 aObjId );

        /**
        * Gets deviceId and objectId of loaning pep.
        * @param aChannelId: channel id
        * @param aDevId: device id
        * @param aObjId: object id
        * @return TInt: KErrNone if succesful,
        * KErrNotFound if no matching channels in context list.
        */
        TInt GetExternalDevice( 
            TUint8 aChannelId, 
            TUint8& aDevId, 
            TUint8& aObjId );


        /**
        * Initialize context info list
        * @param aContextId: id of the context
        * @return none
        */
        void InitializeContextInfoList( const TUint8 aContextId );

    public: // Functions from base classes
        // None

    protected: // New functions
        // None

    protected: // Functions from base classes
        // None

    private: // New functions

        /**
        * C++ default constructor.
        */
        CMmPacketContextMesshandlerList();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Initialize used lists and attributes.
        */
        void InitAttributesAndLists();

    public: // Data
        // None

    protected: // Data
        // None

    private: // Data

        // A context info list.
        TMmContextInfo iContextInfoArray[KMmMaxNumberOfContexts];

        // Number of created contexts in context info list.
        TInt iNumberOfCreatedContexts;

        // Dial-up context Id
        TUint8 iDialUpContextId;

        // Dial-up context Name
        TInfoName iDialUpContextName;

        // Secondary Dial-up context Id
        TUint8 iSecondaryDialUpContextId;

        // Secondary Dial-up context Name
        TInfoName iSecondaryDialUpContextName;

    public: // Friend classes
        // None

    protected: // Friend classes
        // None

    private: // Friend classes
        // None

    };

#endif // CMMPACKETCONTEXTLIST_H

//  End of file
