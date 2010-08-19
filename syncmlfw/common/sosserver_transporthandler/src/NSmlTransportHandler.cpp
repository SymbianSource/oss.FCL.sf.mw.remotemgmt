/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation for transport handler and related classes
*
*/


// INCLUDE FILES

#include <e32base.h>
#include <bautils.h> // BaflUtils
#include <barsc.h> // RResourceFile
#include <f32file.h>
#include <SyncMLTransportProperties.h>
#include <data_caging_path_literals.hrh> 
#include <nsmltransport.rsg>

#include <nsmlconstants.h>
#include "NSmlTransportHandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlTransportHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlTransportHandler* CNSmlTransportHandler::NewL( const TSmlUsageType aUsageType )
	{
	CNSmlTransportHandler* self = new (ELeave) CNSmlTransportHandler( aUsageType );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::~CNSmlTransportHandler
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlTransportHandler::~CNSmlTransportHandler()
	{
	delete iDisplayName;
	delete iProperties;
	}

// -----------------------------------------------------------------------------
// CNSmlTransportHandler::CreateL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlTransportHandler::CreateL( TSmlTransportId aId )
	{
	Reset();

	if ( aId == KUidNSmlMediumTypeInternet.iUid )
		{
		CreateInternetPropertiesL();
		}
	else if ( aId == KUidNSmlMediumTypeBluetooth.iUid )
		{
		CreateBluetoothPropertiesL();
		}
	else if ( aId == KUidNSmlMediumTypeIrDA.iUid )
		{
		CreateIrdaPropertiesL();
		}
	else if ( aId == KUidNSmlMediumTypeUSB.iUid )
		{
		CreateUsbPropertiesL();
		}
		
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::DisplayName
// -----------------------------------------------------------------------------
//
EXPORT_C TDesC& CNSmlTransportHandler::DisplayName()
	{
	return *iDisplayName;
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::Reset
// Deletes all properties from property array and display name.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlTransportHandler::Reset()
	{
	delete iDisplayName;
	iDisplayName = NULL;
	
	for ( TInt i = iProperties->Count()-1; i >= 0; --i )
		{
		iProperties->Delete(i);
		}
		
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::Find
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CNSmlTransportHandler::Find( const TDesC8& aKeyword ) const
	{
	return iProperties->Find( aKeyword );
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::Array
// -----------------------------------------------------------------------------
//
EXPORT_C CSyncMLTransportPropertiesArray& CNSmlTransportHandler::Array()
	{
	return *iProperties;
	}

// -----------------------------------------------------------------------------
// CNSmlTransportHandler::ConstructL
// Symbian 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlTransportHandler::ConstructL()
	{
	iProperties = new (ELeave) CSyncMLTransportPropertiesArray( KNSmlPropertiesArrayGranularity );
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::CNSmlTransportHandler
// -----------------------------------------------------------------------------
//
CNSmlTransportHandler::CNSmlTransportHandler( const TSmlUsageType aUsageType )
	: iUsageType( aUsageType )
	{
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::CreateInternetPropertiesL
// Adds internet properties to property array and gets the 
// internet display name from resources.
// -----------------------------------------------------------------------------
//
void CNSmlTransportHandler::CreateInternetPropertiesL()
	{
	GetDisplayNameL( R_QTN_NSML_TRANSPORT_HTTP );
	
	if ( iUsageType == ESmlDataSync ) // ds
		{
		AddPropertyL( KNSmlIAPId, EDataTypeNumber );
		AddPropertyL( KNSmlIAPId2, EDataTypeNumber );
		AddPropertyL( KNSmlIAPId3, EDataTypeNumber );
		AddPropertyL( KNSmlIAPId4, EDataTypeNumber );
		
		AddPropertyL( KNSmlUseRoaming, EDataTypeBoolean );
		AddPropertyL( KNSmlHTTPAuth, EDataTypeBoolean );
		
		AddPropertyL( KNSmlHTTPUsername, EDataTypeText8 );
		AddPropertyL( KNSmlHTTPPassword, EDataTypeText8 );
		}
	else // dm
		{
		AddPropertyL( KNSmlIAPId, EDataTypeNumber );
		AddPropertyL( KNSmlHTTPAuth, EDataTypeBoolean );
		AddPropertyL( KNSmlHTTPUsername, EDataTypeText8 );
		AddPropertyL( KNSmlHTTPPassword, EDataTypeText8 );
		}
	
	}

// -----------------------------------------------------------------------------
// CNSmlTransportHandler::CreateBluetoothPropertiesL
// -----------------------------------------------------------------------------
//
void CNSmlTransportHandler::CreateBluetoothPropertiesL()
	{
	GetDisplayNameL( R_QTN_NSML_TRANSPORT_BLUETOOTH );
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::CreateIrdaPropertiesL
// -----------------------------------------------------------------------------
//
void CNSmlTransportHandler::CreateIrdaPropertiesL()
	{
	GetDisplayNameL( R_QTN_NSML_TRANSPORT_IRDA );
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::CreateUsbPropertiesL
// -----------------------------------------------------------------------------
//
void CNSmlTransportHandler::CreateUsbPropertiesL()
	{
	GetDisplayNameL( R_QTN_NSML_TRANSPORT_USB );
	}
	
// -----------------------------------------------------------------------------
// CNSmlTransportHandler::AddPropertyL
// Adds a property with given values to iProperties.
// -----------------------------------------------------------------------------
//
void CNSmlTransportHandler::AddPropertyL( const TDesC8& aName, TSyncMLTransportPropertyDataType aType )
	{
	TSyncMLTransportPropertyInfo prop;
	prop.iName = aName;
	prop.iDataType = aType;
	
	iProperties->AddL( prop );
	}

// -----------------------------------------------------------------------------
// CNSmlTransportHandler::GetDisplayNameL
// Reads the given display name from resource file to iDisplayName.
// -----------------------------------------------------------------------------
//
void CNSmlTransportHandler::GetDisplayNameL( TInt aResourceId )
	{
	RFs	fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL(fs);

	TFileName fileName;
	TParse parse;
	parse.Set( KNSmlDirAndResource, &KDC_RESOURCE_FILES_DIR, NULL );
	fileName = parse.FullName();

	RResourceFile resourceFile;
	BaflUtils::NearestLanguageFile( fs, fileName );
	resourceFile.OpenL(fs,fileName);
	CleanupClosePushL(resourceFile);

	HBufC8* dataBuffer = resourceFile.AllocReadLC( aResourceId );
	TResourceReader reader;
	reader.SetBuffer(dataBuffer); 
	iDisplayName = reader.ReadHBufC16L();
	
	CleanupStack::PopAndDestroy(3); //fs, resourcefile, databuffer
	}



