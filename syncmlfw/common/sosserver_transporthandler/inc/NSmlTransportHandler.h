/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declaration file for transport handler
*
*/


#ifndef __TRANSPORTHANDLER_H__
#define __TRANSPORTHANDLER_H__

#include <SyncMLTransportProperties.h>

// constants
_LIT8( KNSmlIAPId,			"NSmlIapId" );
_LIT8( KNSmlIAPId2,			"NSmlIapId2" );
_LIT8( KNSmlIAPId3, 		"NSmlIapId3" );
_LIT8( KNSmlIAPId4, 		"NSmlIapId4" );
_LIT8( KNSmlUseRoaming, 	"NSmlUseRoaming" );
_LIT8( KNSmlHTTPAuth, 		"NSmlHTTPAuth" );
_LIT8( KNSmlHTTPUsername,	"NSmlHTTPUsername" );
_LIT8( KNSmlHTTPPassword, 	"NSmlHTTPPassword" );

_LIT( KNSmlDirAndResource,	"z:NSmlTransport.rsc" );

const TInt KNSmlPropertiesArrayGranularity = 8;

// handler for transport properties
class CNSmlTransportHandler : public CBase
	{
	public:
	
		/**
		* Two-phase construction.
        * static CNSmlTransportHandler* NewL( const TSmlUsageType aUsageType )
        * @param aUsageType Indicates whether the object is constructed for DS or DM.
		* @return New instance of this class.
        */
		IMPORT_C static CNSmlTransportHandler* NewL( const TSmlUsageType aUsageType );
		
		/**
		* Destructor.
        * ~CNSmlTransportHandler()
		* @return - 
        */
		IMPORT_C ~CNSmlTransportHandler();

		/**
		* Creates transport property list for the given transport.
        * void CreateL( TSmlTransportId aId )
        * @param aId The id of the transport whose properties are being handled.
		* @return -
        */
		IMPORT_C void CreateL( TSmlTransportId aId );
		
		/**
		* Return the display name of this instance. CreateL must have been called successfully
		* before this method is executed.
        * TDesC& DisplayName()
		* @return The display name.
        */
		IMPORT_C TDesC& DisplayName();
		
		/**
		* Reset the member data (display name and properties).
        * void Reset()
		* @return -
        */
		IMPORT_C void Reset();
		
		/**
		* Returns the index of the given property relative to 1.
        * TInt Find( const TDesC8& aKeyword ) const
        * @param aKeyWord the name of the property that is searched.
		* @return Index relative to 1. Zero if not found.
        */
		IMPORT_C TInt Find( const TDesC8& aKeyword ) const;
		
		/**
		* Constructor.
        * CSyncMLTransportPropertiesArray& Array()
		* @return -
        */
		IMPORT_C CSyncMLTransportPropertiesArray& Array();
		
	private:
		
		CNSmlTransportHandler( const TSmlUsageType aUsageType );
		void ConstructL();
		
		void CreateInternetPropertiesL();
		void CreateBluetoothPropertiesL();
		void CreateIrdaPropertiesL();
		void CreateUsbPropertiesL();
		
		void AddPropertyL( const TDesC8& aName, TSyncMLTransportPropertyDataType aType );
		void GetDisplayNameL( TInt aResourceId );
		
	private:

		HBufC* iDisplayName;
		TSmlUsageType iUsageType;
		CSyncMLTransportPropertiesArray* iProperties;
	};

#endif // __TRANSPORTHANDLER_H__