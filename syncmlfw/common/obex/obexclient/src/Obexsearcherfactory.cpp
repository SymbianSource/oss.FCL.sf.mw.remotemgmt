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
* Description:  Obex searcher factory
*
*/



// INCLUDE FILES
#include    "Obexsearcherfactory.h"
#include    "Btobexsearcher.h"
#include    "Irdaobexsearcher.h"

_LIT( KObexSearcherFactory, "Search factory" );
const TInt KNotSupportedPanic = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CObexSearcherFactory::CreateObexSearcherL()
// -----------------------------------------------------------------------------
//
CObexSearcherBase* CObexSearcherFactory::CreateObexSearcherL(
	TNSmlObexTransport aTransport )
	{
	
	if ( aTransport == EObexBt )
		{
		return CBTObexSearcher::NewL( TBTConnInfo() );
		}

	if ( aTransport == EObexIr )
		{
		return CIrDAObexSearcher::NewL();
		}

	//Panic if unsupported media queried
	User::Panic( KObexSearcherFactory, KNotSupportedPanic );

	return NULL; //Never executed but suppresses compiler warning...
	}

// -----------------------------------------------------------------------------
// CObexSearcherFactory::CreateBTSearcherL()
// -----------------------------------------------------------------------------
//
CObexSearcherBase* CObexSearcherFactory::CreateBTSearcherL(
	const TBTConnInfo& aBTConnInfo )
	{
	CBTObexSearcher* btSearcher = CBTObexSearcher::NewL( aBTConnInfo );
	return btSearcher;
	}

//  End of File  
