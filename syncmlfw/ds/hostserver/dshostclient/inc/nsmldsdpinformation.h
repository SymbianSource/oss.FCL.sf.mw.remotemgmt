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
* Description:  Utilities for DS Loader Framework
*
*/


#ifndef __NSMLDSDPINFORMATION_H__ 
#define __NSMLDSDPINFORMATION_H__

// INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include <SyncMLDef.h>
#include <SmlDataSyncDefs.h>

// FORWARD DECLARATIONS
class CSmlDataStoreFormat;
class RSmlFilterDef;
class MSmlSyncRelationship;
class CNSmlDbCaps;
class MSmlDataItemUidSet;
class CDesC8Array;

// DATA TYPES

// ------------------------------------------------------------------------------------------------
// Data provider information for SOS Server.
// Doesn't handle freeing of member variable pointers. 
//
// @lib nsmldshostclient.lib
// ------------------------------------------------------------------------------------------------
struct TNSmlDPInformation
	{
	public:
		TSmlDataProviderId iId;
		TVersion iVersion;
		TSmlProtocolVersion iProtocolVer;
		HBufC* iDisplayName;
		CDesC8Array* iMimeTypes;
		CDesC8Array* iMimeVersions;
	};
	
// CLASS DECLARATION

// ------------------------------------------------------------------------------------------------
// Class for transporting data provider information over IPC.
//
// @lib nsmldshostclient.lib
// ------------------------------------------------------------------------------------------------
class CNSmlDPInformation : public CBase
	{
	public:
		/**
		* Creates new instance of CNSmlDPInformation based class.
		* @return CNSmlDPInformation*. Newly created instance.
		*/
		IMPORT_C static CNSmlDPInformation* NewL();
		
		/**
		* Creates new instance of CNSmlDPInformation based class.
		* @return CNSmlDPInformation*. Newly created instance that is pushed to cleanup stack.
		*/
		IMPORT_C static CNSmlDPInformation* NewLC();
		
		/**
		* C++ Destructor.
		*/
		IMPORT_C ~CNSmlDPInformation();
		
		/**
		* sets data provider id.
		* @param TSmlDataProviderId. data provider id to set.
		*/
		IMPORT_C void SetId(TSmlDataProviderId aId) const;

		/**
		* Returns display name of data provider.
		* @return HBufC*. Display name
		*/
		IMPORT_C const HBufC* DisplayName() const;
		
		/**
		* Sets display name of data provider
		* @param aDisplayName. Display name
		*/
		IMPORT_C void SetDisplayNameL( const TDesC& aDisplayName );
		
		/**
		* Returns mime type of data provider.
		* @return CDesC8Array*. Mime types
		*/
		IMPORT_C const CDesC8Array* MimeTypes() const;
		
		/**
		* Sets mime type of data provider.
		* @param aMimeType. Mime types. Class takes the ownership
		*/
		IMPORT_C void SetMimeTypesL( CDesC8Array* aMimeTypes );
		
		/**
		* Returns mime version of data provider.
		* @return CDesC8Array*. Mime versions
		*/
		IMPORT_C const CDesC8Array* MimeVers() const;
		
		/**
		* Sets mime version of data provider
		* @param aMimeVersions. Mime version. Class takes the ownership
		*/
		IMPORT_C void SetMimeVersL( CDesC8Array* aMimeVersions );
		
		/**
		* Returns protocol version of data provider.
		* @return TSmlProtocolVersion. The protocol version supported.
		*/	
		IMPORT_C TSmlProtocolVersion ProtocolVersion() const;
		
		/**
		* Sets protocol version of data provider
		* @param aProtocol. The protocol version supported.
		*/
		IMPORT_C void SetProtocolVersion( TSmlProtocolVersion aProtocol );
		
		/**
		* Returns version number of data provider.
		* @return const const TVersion&. The version number.
		*/
		IMPORT_C const TVersion& Version() const;
		
		/**
		* Sets version of data provider
		* @param aProtocol. The protocol version supported.
		*/
		IMPORT_C void SetVersion( const TVersion& aVersion );
		
		/**
		* Returns all the information of data provider.
		* @return TNSmlDPInformation*. Ownership is changed to the caller.
		*/	
		IMPORT_C TNSmlDPInformation* InformationD();
		
		/**
		* This method externalizes class to stream.
		* @param aStream. Externalize is done to this stream.
		*/
		IMPORT_C void ExternalizeL( RWriteStream& aStream ) const;
		
		/**
		* This method internalizes class from stream.
		* @param aStream. Internalize is done from this stream.
		*/
		IMPORT_C void InternalizeL( RReadStream& aStream );

	private:
		/**
		* C++ constructor.
		*/	
		CNSmlDPInformation();
		
		/**
		* ConstrucL
		*/	
		void ConstructL();
		
	private:
		TNSmlDPInformation* iInfo;
	};
		

#endif // __NSMLDSDPINFORMATION_H__

// End of File
