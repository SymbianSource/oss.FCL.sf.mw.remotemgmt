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


#ifndef __NSMLOBEXSERVICESEARCHER_H__
#define __NSMLOBEXSERVICESEARCHER_H__

#include <e32base.h>
#include <e32std.h>
#include "NSmlBTServiceSearcher.h"
#include "NSmlObexClientConstants.h"


class CNSmlObexServiceSearcher : public CNSmlBTServiceSearcher
    {
public:

    static CNSmlObexServiceSearcher* NewL( const TBTConnInfo& aBTConnInfo );
    
	static CNSmlObexServiceSearcher* NewLC( const TBTConnInfo& aBTConnInfo );
    
	virtual ~CNSmlObexServiceSearcher();
    
	/**
	 *	Returns the port number.
	 */
	TInt Port();

protected:

    const TSdpAttributeParser::TSdpAttributeList& ProtocolList() const;

    /**
     *	Handle found element
     */
	virtual void FoundElementL(TInt aKey, CSdpAttrValue& aValue);

private:

    CNSmlObexServiceSearcher( const TBTConnInfo& aBTConnInfo );

    void ConstructL();

private:

    TInt iPort;
    };

#endif // __NSMLOBEXSERVICESEARCHER_H__

