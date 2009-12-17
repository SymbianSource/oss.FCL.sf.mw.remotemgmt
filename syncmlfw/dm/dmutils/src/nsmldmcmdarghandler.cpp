/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DM Utilities
*
*/


#include <s32mem.h>
#include <smldmadapter.h>
#include "nsmldmcmdarghandler.h"




// ===========================================================================
// NSmlDmCmdArgHandler
// ===========================================================================

// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::PacketDataL()
// Packet elements to stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::PacketDataL(RWriteStream& aWriteStream,
	const TDesC8& aLuid,
	const TDesC8& aData,
	const TDesC8& aURI,
	const TDesC8& aType)
	{
	aWriteStream.WriteUint16L(aLuid.Length());
	aWriteStream.WriteL(aLuid);
	aWriteStream.WriteUint16L(aData.Length());
	aWriteStream.WriteL(aData);
	aWriteStream.WriteUint16L(aURI.Length());
	aWriteStream.WriteL(aURI);
	aWriteStream.WriteUint16L(aType.Length());
	aWriteStream.WriteL(aType);
	}

// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::ParseDataL()
// Parse data from stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::ParseDataL(RReadStream& aReadStream,
	HBufC8*& aLuid,
	HBufC8*& aData,
	HBufC8*& aURI,
	HBufC8*& aType)
	{
	TUint16 length16;
	length16 = aReadStream.ReadUint16L();
	aLuid = HBufC8::NewLC(length16);
	TPtr8 luidPtr= aLuid->Des();
	aReadStream.ReadL(luidPtr,length16);
	
	length16 = aReadStream.ReadUint16L();
	aData = HBufC8::NewLC(length16);
	TPtr8 dataPtr= aData->Des();
	aReadStream.ReadL(dataPtr,length16);

	length16 = aReadStream.ReadUint16L();
	aURI = HBufC8::NewLC(length16);
	TPtr8 uriPtr= aURI->Des();
	aReadStream.ReadL(uriPtr,length16);

	length16 = aReadStream.ReadUint16L();
	aType = HBufC8::NewLC(length16);
	TPtr8 typePtr= aType->Des();
	aReadStream.ReadL(typePtr,length16);
	
	CleanupStack::Pop(4); //aLuid,aData,aURI,aType
	}
	
// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::PacketFetchArgumentsL()
// Packet fetch arguments to stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::PacketFetchArgumentsL(
	RWriteStream& aWriteStream,
	const TDesC8& aLuid,
	const TDesC8& aURI,
	const TDesC8& aType,
	const CArrayFixFlat<TSmlDmMappingInfo>& aPreviousURISegementList)
	{
	aWriteStream.WriteUint16L(aLuid.Length());
	aWriteStream.WriteL(aLuid);
	aWriteStream.WriteUint16L(aURI.Length());
	aWriteStream.WriteL(aURI);
	aWriteStream.WriteUint16L(aType.Length());
	aWriteStream.WriteL(aType);
	TUint8 count=aPreviousURISegementList.Count();
	aWriteStream.WriteUint8L(count);
	for(TUint8 i=0;i<count;i++)
		{
		aWriteStream.WriteUint8L(
			aPreviousURISegementList.At(i).iURISeg.Length());
			
		aWriteStream.WriteL(aPreviousURISegementList.At(i).iURISeg);
		
		aWriteStream.WriteUint8L(
			aPreviousURISegementList.At(i).iURISegLUID.Length());
			
		aWriteStream.WriteL(aPreviousURISegementList.At(i).iURISegLUID);
		}
	}

// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::ParseFetchArgumentsL()
// Parse fetch arguments from stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::ParseFetchArgumentsL(
	RReadStream& aReadStream,
	HBufC8*& aLuid,
	HBufC8*& aURI,
	HBufC8*& aType,
	CArrayFixFlat<TSmlDmMappingInfo>& aPreviousURISegementList,
	RPointerArray<HBufC8>& aLuids)
	{
	TUint16 length16;
	length16 = aReadStream.ReadUint16L();
	aLuid = HBufC8::NewLC(length16);
	TPtr8 luidPtr = aLuid->Des();
	aReadStream.ReadL(luidPtr,length16);

	length16 = aReadStream.ReadUint16L();
	aURI = HBufC8::NewLC(length16);
	TPtr8 uriPtr = aURI->Des();
	aReadStream.ReadL(uriPtr,length16);
	
	length16 = aReadStream.ReadUint16L();
	aType = HBufC8::NewLC(length16);
	TPtr8 typePtr = aType->Des();
	aReadStream.ReadL(typePtr,length16);
	
	TUint8 count = aReadStream.ReadUint8L();
	TUint8 length8;
	
	for(TInt i=0;i<count;i++)
		{
		length8 = aReadStream.ReadUint8L();
		HBufC8 *uriSeg =HBufC8::NewLC(length8);
		TPtr8 uriSegPtr = uriSeg->Des();
		aReadStream.ReadL(uriSegPtr,length8);

		length8 = aReadStream.ReadUint8L();
		HBufC8 *luid =HBufC8::NewLC(length8);
		TPtr8 luidPtr = luid->Des();
		aReadStream.ReadL(luidPtr,length8);
		aLuids.AppendL(luid);
		CleanupStack::Pop(); //luid

		TSmlDmMappingInfo mapInfo;
		mapInfo.iURISeg = TBufC8<KSmlMaxURISegLen>(uriSegPtr);
		mapInfo.iURISegLUID.Set(*luid);
		aPreviousURISegementList.AppendL(mapInfo);
		CleanupStack::PopAndDestroy(); //uriSeg
		}
	CleanupStack::Pop(3); //aLuid,aURI,aType
	}
		

// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::PacketMappingsL()
// Packet mappings to stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::PacketMappingsL(
	RWriteStream& aWriteStream,
	const CArrayPtrSeg<HBufC8>& aURIs,
	const CArrayPtrSeg<HBufC8>& aLuids )
	{

	if(aURIs.Count()!=aLuids.Count())
		{
		aWriteStream.WriteUint16L(0);
		return;
		}

	aWriteStream.WriteUint16L(aURIs.Count());
	for(TInt i=0;i<aURIs.Count();i++)
		{
		aWriteStream.WriteUint16L(aURIs.At(i)->Length());
		aWriteStream.WriteL(*aURIs.At(i));
		aWriteStream.WriteUint16L(aLuids.At(i)->Length());
		aWriteStream.WriteL(*aLuids.At(i));
		}
	}
	
// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::ParseMappingsL()
// Parse mappings from stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::ParseMappingsL(
	RReadStream& aReadStream,
	CArrayPtrSeg<HBufC8>& aURIs,
	CArrayPtrSeg<HBufC8>& aLuids )
	{
	TUint16 mapCount = aReadStream.ReadUint16L();
	
	for(TUint16 i=0;i<mapCount;i++)
		{
		TUint16 length;
		length = aReadStream.ReadUint16L();
		HBufC8* uri = HBufC8::NewLC(length);
		TPtr8 uriPtr = uri->Des();
		aReadStream.ReadL(uriPtr,length);
		CleanupStack::Pop(); //uri
		aURIs.AppendL(uri);

		length = aReadStream.ReadUint16L();
		HBufC8* luid = HBufC8::NewLC(length);
		TPtr8 luidPtr = luid->Des();
		aReadStream.ReadL(luidPtr,length);
		CleanupStack::Pop(); //luid
		aLuids.AppendL(luid);
		}
	}


// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::PacketStatusesL()
// Packet statuses to stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::PacketStatusesL(RWriteStream& aWriteStream,
	const RArray<TNSmlDmStatusElement>& aStatusArray)
	{
	TUint16 count = aStatusArray.Count();
	aWriteStream.WriteUint16L(count);
	for(TInt i=0;i<count;i++)
		{
		aWriteStream.WriteUint16L(aStatusArray[i].iStatusRef);
		aWriteStream.WriteUint8L(aStatusArray[i].iStatus);
		}
	}
	
// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::ParseStatusesL()
// Parse statuses from stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::ParseStatusesL(RReadStream& aReadStream,
	RArray<TNSmlDmStatusElement>& aStatusArray )
	{
	TUint16 count = aReadStream.ReadUint16L();
	
	for(TUint16 i=0;i<count;i++)
		{
		TNSmlDmStatusElement element;
		element.iStatusRef = aReadStream.ReadUint16L();
		element.iStatus = (MSmlDmAdapter::TError) aReadStream.ReadUint8L();
		aStatusArray.AppendL(element);
		}
	}



// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::PacketResultsL()
// Packet results to stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::PacketResultsL(RWriteStream& aWriteStream,
	const RPointerArray<CNSmlDmResultElement>& aResults)
	{
	TUint16 count = aResults.Count();
	aWriteStream.WriteUint16L(count);

	for(TInt i=0;i<count;i++)
		{
		aWriteStream.WriteUint16L(aResults[i]->iResultRef);
		aWriteStream.WriteUint32L(aResults[i]->iResult->Size());
		aWriteStream.WriteL(aResults[i]->iResult->Ptr(0),aResults[i]->iResult->Size());
		aWriteStream.WriteUint16L(aResults[i]->iMimeType->Length());
		aWriteStream.WriteL(*aResults[i]->iMimeType);
		}
	}
	
// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::ParseResultsL()
// Parse results from stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::ParseResultsL(RReadStream& aReadStream,
	RPointerArray<CNSmlDmResultElement>& aResults)
	{
	TUint16 count = aReadStream.ReadUint16L();
	
	for(TUint16 i=0;i<count;i++)
		{
		CNSmlDmResultElement* element = new (ELeave) CNSmlDmResultElement();
		CleanupStack::PushL(element);
		element->iResultRef = aReadStream.ReadUint16L();
		TUint32 len32 = aReadStream.ReadUint32L();
		if(len32)
			{
			element->iResult = CBufFlat::NewL(len32);
			}
		else
			{
			element->iResult = CBufFlat::NewL(8);
			}
		element->iResult->ResizeL(len32);
		TPtr8 resultPtr = element->iResult->Ptr(0);
		aReadStream.ReadL(resultPtr,len32);

		TUint16 len16 = aReadStream.ReadUint16L();
		element->iMimeType = HBufC8::NewL(len16);
		TPtr8 mimePtr = element->iMimeType->Des();
		aReadStream.ReadL(mimePtr,len16);
		aResults.AppendL(element);	
		CleanupStack::Pop();  //element
		}
	}


// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::PacketCopyArgumentsL()
// Packet copy arguments to stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::PacketCopyArgumentsL(
	RWriteStream& aWriteStream,
	const TDesC8& aTargetLuid,
	const TDesC8& aTargetURI,
	const TDesC8& aSourceLuid,
	const TDesC8& aSourceURI,
	const TDesC8& aType)
	{
	aWriteStream.WriteUint16L(aTargetLuid.Length());
	aWriteStream.WriteL(aTargetLuid);
	aWriteStream.WriteUint16L(aTargetURI.Length());
	aWriteStream.WriteL(aTargetURI);
	aWriteStream.WriteUint16L(aSourceLuid.Length());
	aWriteStream.WriteL(aSourceLuid);
	aWriteStream.WriteUint16L(aSourceURI.Length());
	aWriteStream.WriteL(aSourceURI);
	aWriteStream.WriteUint16L(aType.Length());
	aWriteStream.WriteL(aType);
	}

// ---------------------------------------------------------------------------
// NSmlDmCmdArgHandler::ParseCopyArgumentsL()
// Parse copy arguments from stream
// ---------------------------------------------------------------------------
EXPORT_C void NSmlDmCmdArgHandler::ParseCopyArgumentsL(
	RReadStream& aReadStream,
	HBufC8*& aTargetLuid,
	HBufC8*& aTargetURI,
	HBufC8*& aSourceLuid,
	HBufC8*& aSourceURI,
	HBufC8*& aType)
	{
	TUint16 length16;

	length16 = aReadStream.ReadUint16L();
	aTargetLuid = HBufC8::NewLC(length16);
	TPtr8 targetLuidPtr= aTargetLuid->Des();
	aReadStream.ReadL(targetLuidPtr,length16);
	
	length16 = aReadStream.ReadUint16L();
	aTargetURI = HBufC8::NewLC(length16);
	TPtr8 targetUriPtr= aTargetURI->Des();
	aReadStream.ReadL(targetUriPtr,length16);

	length16 = aReadStream.ReadUint16L();
	aSourceLuid = HBufC8::NewLC(length16);
	TPtr8 sourceLuidPtr= aSourceLuid->Des();
	aReadStream.ReadL(sourceLuidPtr,length16);
	
	length16 = aReadStream.ReadUint16L();
	aSourceURI = HBufC8::NewLC(length16);
	TPtr8 sourceUriPtr= aSourceURI->Des();
	aReadStream.ReadL(sourceUriPtr,length16);

	length16 = aReadStream.ReadUint16L();
	aType = HBufC8::NewLC(length16);
	TPtr8 typePtr= aType->Des();
	aReadStream.ReadL(typePtr,length16);

	//POP aTargetLuid,aTargetURI,aSourceLuid,aSourceURI,aType
	CleanupStack::Pop(5); 
	}


// ===========================================================================
// NSmlDmCmdArgHandler
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmResultElement::~CNSmlDmResultElement()
// ---------------------------------------------------------------------------
EXPORT_C CNSmlDmResultElement::~CNSmlDmResultElement()
	{
	delete iResult;
	delete iMimeType;
	}

