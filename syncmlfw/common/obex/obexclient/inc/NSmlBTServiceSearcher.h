/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  SyncML Obex client
*
*/


#ifndef __NSMLBTSERVICESEARCHER_H__
#define __NSMLBTSERVICESEARCHER_H__

#include <e32base.h>
#include <bttypes.h>
#include <btextnotifiers.h>
#include <e32std.h>

#include "NSmlSdpAttributeParser.h"
#include "NSmlSdpAttributeNotifier.h"
#include "Btconninfo.h"

/** BTServiceSearcher application panic codes */
enum TBTServiceSearcherPanics 
    {
    EBTServiceSearcherNextRecordRequestComplete = 1,
    EBTServiceSearcherAttributeRequestResult,
    EBTServiceSearcherAttributeRequestComplete,
    EBTServiceSearcherInvalidControlIndex,
    EBTServiceSearcherProtocolRead,
    EBTServiceSearcherAttributeRequest,
    EBTServiceSearcherSdpRecordDelete
    };

inline void Panic(TBTServiceSearcherPanics aReason)
    { 
    User::Panic(_L("ServiceSearcher"), aReason);
    }


class CNSmlBTServiceSearcher : public CBase,
                           public MSdpAgentNotifier,
                           public MSdpAttributeNotifier
    {
public:
    virtual ~CNSmlBTServiceSearcher();
    void SelectDeviceByDiscoveryL( TRequestStatus& aObserverRequestStatus );
    void FindServiceL( TRequestStatus& iObserverRequestStatus );

    const TBTDevAddr& BTDevAddr();

    const TBTDeviceResponseParams& ResponseParams();

protected:
    CNSmlBTServiceSearcher( const TBTConnInfo& aBTConnInfo );
    virtual void Finished( TInt aError = KErrNone );
    virtual TBool HasFinishedSearching() const;
    TBool HasFoundService() const;

protected: // abstract methods
    virtual const TSdpAttributeParser::TSdpAttributeList& ProtocolList() const = 0;
    virtual void FoundElementL( TInt aKey, CSdpAttrValue& aValue ) = 0;

public: // from MSdpAgentNotifier
    void NextRecordRequestComplete( TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount );
    void AttributeRequestResult( TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue );
    void AttributeRequestComplete( TSdpServRecordHandle aHandle, TInt aError );

private:
    void NextRecordRequestCompleteL( TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount );
    void AttributeRequestResultL( TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue );
    void AttributeRequestCompleteL( TSdpServRecordHandle, TInt aError );

private:
    TRequestStatus* iStatusObserver;    
    TBool iIsDeviceSelectorConnected;
//    RNotifier iDeviceSelector;
    TBTDeviceResponseParamsPckg iResponse;
    CSdpAgent* iAgent;
    CSdpSearchPattern* iSdpSearchPattern;
    TBool iHasFoundService;
	TBTConnInfo iBTConnInfo;
    };

#endif // __NSMLBTSERVICESEARCHER_H__

