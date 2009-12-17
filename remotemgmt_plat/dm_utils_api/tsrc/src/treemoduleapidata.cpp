/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/
/*#include <cntdb.h>
#include <cntitem.h>
#include <s32file.h>
#include <s32mem.h>
#include <f32file.h>
*/
//#include <nsmldmmodule.h>
//#include "test.h"
//#include "nsmldebug.h"
//#include <e32def.h>
//#include <utf.h>
//#include <smldmadapter.h>

#include "treemoduleapidata.h"



// DM-Matcher Callback function

CMSmlDmCallbackTest::CMSmlDmCallbackTest()
{
	
}

CMSmlDmCallbackTest::~CMSmlDmCallbackTest()
{
	
}

CMSmlDmCallbackTest*  CMSmlDmCallbackTest:: NewL()
{
	CMSmlDmCallbackTest* self = new (ELeave) CMSmlDmCallbackTest();
	return self;
}

void CMSmlDmCallbackTest :: SetResultsL( TInt /*aResultsRef */, CBufBase& /*aObject*/,
							  const TDesC8& /*aType */)
{
	
}

void CMSmlDmCallbackTest :: SetResultsL( TInt /*aResultsRef */, RReadStream*& /* aStream */,
							  const TDesC8& /* aType */ )
{
	
}


void CMSmlDmCallbackTest :: SetStatusL( TInt /* aStatusRef */,
							 MSmlDmAdapter::TError /* aErrorCode */)
{
	
}


void CMSmlDmCallbackTest :: SetMappingL( const TDesC8& /* aURI */, const TDesC8& /* aLUID */ )
{
	
}


void CMSmlDmCallbackTest :: FetchLinkL( const TDesC8& /*aURI */, CBufBase& aData,
							 MSmlDmAdapter::TError& aStatus )
{	
	aStatus = MSmlDmAdapter::EOk;
	
  //	_LIT8(KaURI,"NTMSAP/NAPDef");
   //	_LIT8(KaURI, "./AP/NTMSAP2194660/NAPDef");
   _LIT8(KaURI,"AP/NTMSAP2194660");
   	TBufC8<30> URI( KaURI );
	
	aData.InsertL(0, URI);
}


HBufC8* CMSmlDmCallbackTest :: GetLuidAllocL( const TDesC8& /*aURI */ )
{
	HBufC8 *tmp = HBufC8::NewLC(4);
	TPtr8 tmpPtr = tmp->Des();
	tmpPtr.AppendNum(10);
	CleanupStack::Pop();  
			
	return tmp;
}
