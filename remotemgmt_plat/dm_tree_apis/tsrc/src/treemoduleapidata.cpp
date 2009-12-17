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



CNSmlTestElement::CNSmlTestElement(TDesC8& aUri,TDesC8& aData,TInt aStatusRef, NSmlCmdType aCmdType)
	{
	iUri = aUri.Alloc();
	iData = aData.Alloc();
	iStatusRef = aStatusRef;
	iCmdType = aCmdType;
	}

CNSmlTestElement::CNSmlTestElement(TDesC8& aUri,TInt aStatusRef, NSmlCmdType aCmdType)
	{
	iUri = aUri.Alloc();
	iStatusRef = aStatusRef;
	iCmdType = aCmdType;
	}

CNSmlTestElement::CNSmlTestElement(TDesC8& aUri,TInt aStatusRef,TInt aResultRef, NSmlCmdType aCmdType)
	{
	iUri = aUri.Alloc();
	iStatusRef = aStatusRef;
	iCmdType = aCmdType;
	iResultRef = aResultRef;
	
	}
	
CNSmlTestElement::~CNSmlTestElement()	
	{
	delete iUri;
	delete iData;
	}


// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
CNSmlDmCallbackTest::CNSmlDmCallbackTest()
	{
//	iFs.Connect();
//	_LIT( TstOutputName, "C:\\NSmlTestResult.txt" );
//	iFile.Replace(iFs, TstOutputName(), EFileShareAny|EFileWrite);
	}


// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
CNSmlDmCallbackTest::~CNSmlDmCallbackTest()
	{
//	iFs.Close;
	delete iServerId;
	iArray.ResetAndDestroy();
	}


CNSmlDmCallbackTest* CNSmlDmCallbackTest::NewL()
	{
	CNSmlDmCallbackTest* self = new (ELeave) CNSmlDmCallbackTest();
//	CleanupStack::PushL(self);
//	self->ConstructL();
//	CleanupStack::Pop();
	return self;
	}


void CNSmlDmCallbackTest::SetServerIdL(TDesC8& /*aServerId */)
	{
/*	iFile.Write(_L8("Set Server:  "));
	iFile.Write(aServerId);
	iFile.Write(_L8("\n\n"));
*/	}

void CNSmlDmCallbackTest::StartAtomicL()
	{
//	iFile.Write(_L8("Start Atomic\n\n"));
	}

void CNSmlDmCallbackTest::EndAtomicL()
	{
//	iFile.Write(_L8("End Atomic\n\n"));
	}

void CNSmlDmCallbackTest::RollBackL()
	{
//	iFile.Write(_L8("RollBack\n\n"));
	}

// FOTA
void CNSmlDmCallbackTest::MarkGenAlertsSent()
	{
//	iFile.Write(_L8("MarkGenAlertsSent called \r\n"));	
	}
// FOTA end

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlDmCallbackTest::SetResultsL( TInt aResultsRef, const CBufBase& aObject, const TDesC8& /*aType */, const TDesC8& /*aFormat */,TInt aTotalSize )
	{
	const CBufBase* buf = &aObject;
//	if(aObject.Size()>1000)
	CNSmlTestElement* element = 0;
	TInt i=0;

	for(i=0;i<iArray.Count();i++)
		{
		if(aResultsRef==iArray[i]->iResultRef)
			{
			element = iArray[i];
			break;
			}
		}
	

	if(aTotalSize>1000)
		{
//		RFs aFileSession;
//		User::LeaveIfError(aFileSession.Connect());
//		CleanupClosePushL(aFileSession);
//		RFile file;
//		_LIT( TmpOutputName, "C:\\NSmlTmp.xml" );
		//if( aFile.Open(aFileSession, TmpOutputName(), EFileShareAny|EFileWrite) == KErrNone )
//		file.Replace(iFs, TmpOutputName(), EFileShareAny|EFileWrite);
//		CleanupClosePushL(file);
//		TInt pos(0);
//		file.Seek(ESeekEnd, pos);
//		HBufC8* tmp = HBufC8::NewLC(buf->Size());
/*		TPtr8 ptr = tmp->Des();
		buf->Read(0,ptr,buf->Size());
		TInt result = file.Write(ptr);
		CleanupStack::PopAndDestroy(2); // aFile, tmp
		User::LeaveIfError(result);*/
//		CleanupStack::PopAndDestroy(); // aFileSession
		}
	else
		{
		HBufC8 *tmp = HBufC8::NewLC(aObject.Size());
		TPtr8 tmpPtr = tmp->Des();
		buf->Read(0,tmpPtr,buf->Size());
		iObject.Append(tmpPtr);

		if(element)
			{
/*			HBufC *buf16 = HBufC::NewLC(tmp->Length());
			TPtr bufPtr = buf16->Des();
			CnvUtfConverter::ConvertToUnicodeFromUtf8( bufPtr, tmpPtr);

			HBufC *uri16 = HBufC::NewLC(element->iUri->Length());
			TPtr uriPtr = uri16->Des();
			CnvUtfConverter::ConvertToUnicodeFromUtf8( uriPtr, *element->iUri);
			
*/			
/*			iFile.Write(_L8("URI: "));
			iFile.Write(*element->iUri);
			iFile.Write(_L8("\n"));
			iFile.Write(_L8("ResultRef: "));
			TBuf8<4> num;
			num.Num(aResultsRef);
			iFile.Write(num);
			iFile.Write(_L8(" StatusRef: "));
			num.Num(element->iStatusRef);
			iFile.Write(num);
			iFile.Write(_L8("\n"));
			iFile.Write(_L8("Result: "));
			HBufC8* tmp = HBufC8::NewLC(buf->Size());
			TPtr8 ptr = tmp->Des();
			buf->Read(0,ptr,buf->Size());
			iFile.Write(ptr);
			iFile.Write(_L8("\n\n"));
			CleanupStack::PopAndDestroy(); //tmp
			*/

//			console->Printf(_L("URI: %S\n"), uriPtr);
//			console->Printf(_L("Results: %S\nResultsRef: %d"), buf16,aResultsRef);
//			console->Printf(_L("\n\n"));
//			console->Getch(); // get and ignore character
//			CleanupStack::PopAndDestroy(2); 
			}

		CleanupStack::PopAndDestroy(); //tmp
		}	
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlDmCallbackTest::SetStatusL( TInt aStatusRef, TInt aStatusCode )
	{
	CNSmlTestElement* element = 0;
	TInt i=0;

	iStatuscode = aStatusCode;
	for(i=0;i<iArray.Count();i++)
		{
		if(aStatusRef==iArray[i]->iStatusRef)
			{
			element = iArray[i];
			break;
			}
		}
	
	if(element)
		{
		switch(element->iCmdType)
			{
			case ETestAdd:
				//iFile.Write(_L8("Add"));
				break;
			case ETestGet:
				//iFile.Write(_L8("Get"));
				break;
			case ETestDelete:
				//iFile.Write(_L8("Delete"));
				break;
			case ETestReplace:
				//iFile.Write(_L8("Replace"));
				break;
			case ETestExec:
				//iFile.Write(_L8("Exec"));
				break;
			case ETestCopy:
				//iFile.Write(_L8("Copy"));
				break;
			}
	//	iFile.Write(_L8("\n"));
		if(element->iData)
			{
	/*		iFile.Write(_L8("Data: "));
			iFile.Write(*element->iData);
			iFile.Write(_L8("\n"));
	*/		}
	/*	iFile.Write(_L8("URI: "));
		iFile.Write(*element->iUri);
		iFile.Write(_L8("\n"));
		iFile.Write(_L8("StatusRef: "));
		TBuf8<4> num;
		num.Num(aStatusRef);
		iFile.Write(num);
		iFile.Write(_L8(" Status: "));
		num.Num(aStatusCode);
		iFile.Write(num);
		iFile.Write(_L8("\n\n"));
	*/	
		
//		console->Printf(_L("URI: %S\n"), element->iUri);
//		console->Printf(_L("Status code: %d\nStatusRef: %d"), aStatusCode,aStatusRef);
//		console->Printf(_L("\n\n"));
//		console->Getch(); // get and ignore character
//		delete element;
//		iArray.Remove(i);
		}

	}


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
