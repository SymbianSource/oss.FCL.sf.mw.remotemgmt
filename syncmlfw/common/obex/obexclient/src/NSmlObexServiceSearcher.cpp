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


#include <bt_sock.h>
#include "NSmlObexClient.h"
#include "NSmlObexServiceSearcher.h"
#include "NSmlObexClientConstants.h"


//------------------------------------------------------------
// TSdpAttributeParser::SSdpAttributeNode
//------------------------------------------------------------
const TSdpAttributeParser::SSdpAttributeNode gObexProtocolListData[] = 
    {
        { TSdpAttributeParser::ECheckType, ETypeDES },
            { TSdpAttributeParser::ECheckType, ETypeDES },
                { TSdpAttributeParser::ECheckValue, ETypeUUID, KL2CAP },
            { TSdpAttributeParser::ECheckEnd },
            { TSdpAttributeParser::ECheckType, ETypeDES },
                { TSdpAttributeParser::ECheckValue, ETypeUUID, KRFCOMM },
                { TSdpAttributeParser::EReadValue, ETypeUint, KRfcommChannel },
            { TSdpAttributeParser::ECheckEnd },
            { TSdpAttributeParser::ECheckType, ETypeDES },
                { TSdpAttributeParser::ECheckValue, ETypeUUID, KBtProtocolIdOBEX },
            { TSdpAttributeParser::ECheckEnd },
        { TSdpAttributeParser::ECheckEnd },
    { TSdpAttributeParser::EFinished }
    };
//------------------------------------------------------------
// TStaticArrayC<TSdpAttributeParser::SSdpAttributeNode>
//------------------------------------------------------------
const TStaticArrayC<TSdpAttributeParser::SSdpAttributeNode> gObexProtocolList =
CONSTRUCT_STATIC_ARRAY_C(
    gObexProtocolListData
);
//------------------------------------------------------------
// CNSmlObexServiceSearcher::NewL()
//------------------------------------------------------------
CNSmlObexServiceSearcher* CNSmlObexServiceSearcher::NewL( const TBTConnInfo& aBTConnInfo )
    {
    CNSmlObexServiceSearcher* self = CNSmlObexServiceSearcher::NewLC( aBTConnInfo );
    CleanupStack::Pop(self);
    return self;
    }
//------------------------------------------------------------
// CNSmlObexServiceSearcher::NewLC()
//------------------------------------------------------------
CNSmlObexServiceSearcher* CNSmlObexServiceSearcher::NewLC( const TBTConnInfo& aBTConnInfo )
    {
    CNSmlObexServiceSearcher* self = new (ELeave) CNSmlObexServiceSearcher( aBTConnInfo );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
//------------------------------------------------------------
// CNSmlObexServiceSearcher::CNSmlObexServiceSearcher()
//------------------------------------------------------------
CNSmlObexServiceSearcher::CNSmlObexServiceSearcher( const TBTConnInfo& aBTConnInfo )
: CNSmlBTServiceSearcher( aBTConnInfo ),
  iPort(-1)
    {
    }
//------------------------------------------------------------
// CNSmlObexServiceSearcher::~CNSmlObexServiceSearcher()
//------------------------------------------------------------
CNSmlObexServiceSearcher::~CNSmlObexServiceSearcher()
    {
    }
//------------------------------------------------------------
// CNSmlObexServiceSearcher::ConstructL()
//------------------------------------------------------------
void CNSmlObexServiceSearcher::ConstructL()
    {
    }
//------------------------------------------------------------
// TSdpAttributeParser::TSdpAttributeList& CNSmlObexServiceSearcher::ProtocolList()
//------------------------------------------------------------
const TSdpAttributeParser::TSdpAttributeList& CNSmlObexServiceSearcher::ProtocolList() const
    {
    return gObexProtocolList;
    }
//------------------------------------------------------------
// CNSmlObexServiceSearcher::FoundElementL(TInt aKey, CSdpAttrValue& aValue)
//------------------------------------------------------------
void CNSmlObexServiceSearcher::FoundElementL(TInt aKey, CSdpAttrValue& aValue)
    {
	//DBG_ARGS8( "FoundElementL: %d", aValue.Uint() );

    __ASSERT_ALWAYS(aKey == static_cast<TInt>(KRfcommChannel), Panic(EBTObjectExchangeProtocolRead));
    iPort = aValue.Uint();
    }
//------------------------------------------------------------
// CNSmlObexServiceSearcher::Port()
//------------------------------------------------------------
TInt CNSmlObexServiceSearcher::Port()
    {
    return iPort;
    }
