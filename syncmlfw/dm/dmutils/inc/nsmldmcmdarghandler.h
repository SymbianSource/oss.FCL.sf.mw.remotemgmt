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



#ifndef __NSMLDMCMDARGHANDLER_H
#define __NSMLDMCMDARGHANDLER_H

#include <e32std.h>
#include <e32base.h>
#include <smldmadapter.h>


// ===========================================================================
// struct TNSmlDmStatusElement
// ===========================================================================
/**
* TNSmlDmStatusElement element contais plug-in adapter command status
* information and information for mapping it to correct status element
* in DM protocoll
*
*  @since
*/
struct TNSmlDmStatusElement
	{
	
	/**
	* Reference to status element
	*/
	TInt32 iStatusRef;

	/**
	* Status enumeration from plug-in adapter
	*/
	MSmlDmAdapter::TError iStatus;
	};

// ===========================================================================
// CNSmlDmResultElement
// ===========================================================================
/**
* CNSmlDmResultElement is container class and contais information for
* generating result element in DM protocoll
*
*  @since
*/
class CNSmlDmResultElement : public CBase
	{
	public:
	/**
	* Destructor
	*/
	IMPORT_C virtual ~CNSmlDmResultElement();

	/**
	* Reference to result element
	*/
	TInt32 iResultRef;

	/**
	* Result data
	*/
	CBufBase* iResult;

	/**
	* Mime type of data
	*/
	HBufC8* iMimeType;

	/**
	* True if result is streamed large object
	*/
	TBool iStreamed;
	};

// ===========================================================================
// NSmlDmCmdArgHandler
// ===========================================================================
/**
* NSmlDmCmdArgHandler contains just static methods for packeting data to flat
* flat stream and parsing back to separate descriptors for transferring data
* between host client and server.
*
*  @since
*/
class NSmlDmCmdArgHandler
	{
	public:

	/**
	* Packets command arguments to stream
	* @param	aWriteStream	Stream to write following parameters
	* @param	aLuid			Luid
	* @param	aData			Data
	* @param	aURI			Uri
	* @param	aType 			Mime type
	*/
	IMPORT_C static void PacketDataL(RWriteStream& aWriteStream,
		const TDesC8& aLuid,
		const TDesC8& aData,
		const TDesC8& aURI,
		const TDesC8& aType);
		
	/**
	* Parses command arguments from stream
	* @param	aReadStream		Stream for reading following parameters
	* @param	aLuid			Luid
	* @param	aData			Data
	* @param	aURI			Uri
	* @param	aType 			Mime type
	*/
	IMPORT_C static void ParseDataL(RReadStream& aReadStream,
		HBufC8*& aLuid,
		HBufC8*& aData,
		HBufC8*& aURI,
		HBufC8*& aType);

	/**
	* Packets Get command arguments to stream
	* @param	aWriteStream			Stream to write following parameters
	* @param	aLuid					Luid
	* @param	aURI					Uri
	* @param	aType 					Mime type
	* @param	aPreviousURISegementList List of earlier made mappings
	*/
	IMPORT_C static void PacketFetchArgumentsL(RWriteStream& aWriteStream,
		const TDesC8& aLuid,
		const TDesC8& aURI,
		const TDesC8& aType,
		const CArrayFixFlat<TSmlDmMappingInfo>& aPreviousURISegementList);
		
	/**
	* Parses Get command arguments from stream
	* @param	aReadStream				Stream for reading following parameters
	* @param	aLuid					Luid
	* @param	aURI					Uri
	* @param	aType 					Mime type
	* @param	aPreviousURISegementList List of earlier made mappings
	* @param	aLuids					Temporary strorage for TPtrC8 type
	*									obejcts in TSmlDmMappingInfo structs
	*/
	IMPORT_C static void ParseFetchArgumentsL(RReadStream& aReadStream,
		HBufC8*& aLuid,
		HBufC8*& aURI,
		HBufC8*& aType,
		CArrayFixFlat<TSmlDmMappingInfo>& aPreviousURISegementList,
		RPointerArray<HBufC8>& aLuids);
	
	/**
	* Packets mappings to stream
	* @param	aWriteStream			Stream to write following parameters
	* @param	aURIs					Array of Uris
	* @param	aLuids					Array of luids
	*/
	IMPORT_C static void PacketMappingsL(RWriteStream& aWriteStream,
		const CArrayPtrSeg<HBufC8>& aURIs,
		const CArrayPtrSeg<HBufC8>& aLuids );
		
	/**
	* Parse mappings from stream
	* @param	aReadStream				Stream for reading following parameters
	* @param	aURIs					Array of Uris
	* @param	aLuids					Array of luids
	*/
	IMPORT_C static void ParseMappingsL(RReadStream& aReadStream,
		CArrayPtrSeg<HBufC8>& aURIs,
		CArrayPtrSeg<HBufC8>& aLuids );

	/**
	* Packets statuses to stream
	* @param	aWriteStream			Stream to write following parameters
	* @param	aStatusArray			Array of status elements
	*/
	IMPORT_C static void PacketStatusesL(RWriteStream& aWriteStream,
		const RArray<TNSmlDmStatusElement>& aStatusArray);
		
	/**
	* Parse statuses from stream
	* @param	aReadStream				Stream for reading following parameters
	* @param	aStatusArray			Array of status elements
	*/
	IMPORT_C static void ParseStatusesL(RReadStream& aReadStream,
		RArray<TNSmlDmStatusElement>& aStatusArray );

	/**
	* Packets results to stream
	* @param	aWriteStream			Stream to write following parameters
	* @param	aResults				Array of result elements
	*/
	IMPORT_C static void PacketResultsL(RWriteStream& aWriteStream,
		const RPointerArray<CNSmlDmResultElement>& aResults);
		
	/**
	* Parse results from stream
	* @param	aReadStream				Stream for reading following parameters
	* @param	aResults				Array of result elements
	*/
	IMPORT_C static void ParseResultsL(RReadStream& aReadStream,
		RPointerArray<CNSmlDmResultElement>& aResults);
	
	/**
	* Packets Copy command arguments to stream
	* @param	aWriteStream	Stream to write following parameters
	* @param	aTargetLuid		Target luid
	* @param	aTargetURI		Target uri
	* @param	aSourceLuid		Source luid
	* @param	aSourceURI		Source uri
	* @param	aType 			Mime type
	*/
	IMPORT_C static void PacketCopyArgumentsL(RWriteStream& aWriteStream,
		const TDesC8& aTargetLuid,
		const TDesC8& aTargetURI,
		const TDesC8& aSourceLuid,
		const TDesC8& aSourceURI,
		const TDesC8& aType);
		
	/**
	* Parses Copy command arguments from stream
	* @param	aReadStream		Stream for reading following parameters
	* @param	aTargetLuid		Target luid
	* @param	aTargetURI		Target uri
	* @param	aSourceLuid		Source luid
	* @param	aSourceURI		Source uri
	* @param	aType 			Mime type
	*/
	IMPORT_C static void ParseCopyArgumentsL(RReadStream& aReadStream,
		HBufC8*& aTargetLuid,
		HBufC8*& aTargetURI,
		HBufC8*& aSourceLuid,
		HBufC8*& aSourceURI,
		HBufC8*& aType);
	};


#endif // __NSMLDMCMDARGHANDLER_H