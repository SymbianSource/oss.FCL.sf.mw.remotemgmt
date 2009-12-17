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
* Description:  Phone specific data
*
*/


#include <f32file.h>	
#include <s32file.h>
#include <etel.h>
#include <etelmm.h>
#include <sysutil.h>
#include <data_caging_path_literals.hrh>
#include <nsmlconstants.h>
#include <nsmlphoneinfo.h>
#include <nsmlunicodeconverter.h>

// CONSTANTS
_LIT( KNSmlCommonAgentPanic, "NSmlCommonAgent" );
#if defined( __WINS__ )
_LIT( KNSmlIMEIFileName, "imei.txt");
const TInt KNSmlMaxIMEIFileName = 256;
const TInt KNSmlIMEIFileDrive = EDriveC;
#endif

// ---------------------------------------------------------
// CNSmlPhoneInfo::CNSmlPhoneInfo
// Constructor, nothing special in here.
// ---------------------------------------------------------
//
CNSmlPhoneInfo::CNSmlPhoneInfo()
	{
	}

// ---------------------------------------------------------
// CNSmlPhoneInfo::ConstructL()
// Two-way construction. Constructor may leave in EPOC.
// ---------------------------------------------------------
//
void CNSmlPhoneInfo::ConstructL()
	{
	}

// ---------------------------------------------------------
// CNSmlPhoneInfo::~CNSmlPhoneInfo()
// Destructor
// ---------------------------------------------------------
//

CNSmlPhoneInfo::~CNSmlPhoneInfo()
	{
	delete iSwVersion;
	}
// ---------------------------------------------------------
// CNSmlPhoneInfo::NewL()
// Creates new instance of CNSmlPhoneInfo. 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
//
EXPORT_C CNSmlPhoneInfo* CNSmlPhoneInfo::NewL()
	{
	CNSmlPhoneInfo* self = CNSmlPhoneInfo::NewLC();
	CleanupStack::Pop();
	return( self );
	}

// ---------------------------------------------------------
// CNSmlPhoneInfo::NewLC()
// Creates new instance of CNSmlPhoneInfo 
// Leaves instance pointer to CleanupStack.
// ---------------------------------------------------------
//
EXPORT_C CNSmlPhoneInfo* CNSmlPhoneInfo::NewLC()
	{
	CNSmlPhoneInfo* self = new (ELeave) CNSmlPhoneInfo();
	CleanupStack::PushL( self );
	self->ConstructL();
	return( self );
	}
// ---------------------------------------------------------
// CNSmlPhoneInfo::PhoneDataL()
// Manufacturer, version and IMEI from phone service
//
// ---------------------------------------------------------
EXPORT_C void CNSmlPhoneInfo::PhoneDataL( TPhoneIdType aIdType, TDes& aId ) const
	{
	aId.Zero();
	HBufC* modelId;
	modelId = KNSmlPhoneModelIdDefault().AllocLC();
	_LIT( KSmlPhoneManufacturer, "Unknown" );
#if defined( __WINS__ )
// phone services may work in emulator!
	_LIT( KSmlPhoneRevisionId, "1.0" );
	switch ( aIdType )
		{
		case EPhoneManufacturer:
			aId = KSmlPhoneManufacturer;
			break;
		case EPhoneModelId:
			aId = *modelId;
			break;
		case EPhoneRevisionId:
			aId = KSmlPhoneRevisionId;
			break;
		case EPhoneSerialNumber:
		    RFs fileServer;
			User::LeaveIfError(fileServer.Connect());
			CleanupClosePushL(fileServer);
		    HBufC* path = HBufC::NewLC(KNSmlMaxIMEIFileName);
		    TPtr pathPtr = path->Des();
		    fileServer.CreatePrivatePath(KNSmlIMEIFileDrive);
            User::LeaveIfError(fileServer.SetSessionToPrivate(KNSmlIMEIFileDrive));
            User::LeaveIfError(fileServer.SessionPath(pathPtr));
            TParse filename;
            User::LeaveIfError(filename.Set(KNSmlIMEIFileName(), path, NULL));
			_LIT(KSmlImeiMask, "IMEI:%05d%05d%05d");
			RFile file;
			TInt err = file.Open(fileServer, filename.FullName(), EFileShareAny);
			if (err == KErrNotFound)
				{
				User::LeaveIfError(file.Create(fileServer, filename.FullName(), EFileShareAny|EFileWrite));
				RFileWriteStream writer(file, 0);
				// practically unique IMEI for emulator use
				aId.Format(KSmlImeiMask(), User::TickCount(), User::TickCount()*3, User::TickCount()*7);
				writer << aId;
				writer.CommitL();
				writer.Close();
				}
			else if (err)
			    {
			    User::Leave(err);
			    }
			else
				{
				RFileReadStream reader(file, 0);
				reader >> aId;
				reader.Close();
				}
			CleanupClosePushL(file);
			CleanupStack::PopAndDestroy(3); // fileServer, file, path
			break;
		default:
			User::Panic( KNSmlCommonAgentPanic, KErrArgument );
			break;
		}
#else
	_LIT(KSmlImeiMask, "IMEI:%S");
	RTelServer server;
    User::LeaveIfError(server.Connect()); 
	CleanupClosePushL(server);
	TInt numPhones;
	User::LeaveIfError(server.EnumeratePhones(numPhones));
	if( numPhones > 0 )
		{
		RTelServer::TPhoneInfo phoneInfo;
		User::LeaveIfError(server.GetPhoneInfo(0, phoneInfo)); // Query first phone
		RMobilePhone phone;
		User::LeaveIfError(phone.Open(server, phoneInfo.iName));
		CleanupClosePushL(phone);
		User::LeaveIfError(phone.Initialise());	
		TUint32 identityCaps;
		phone.GetIdentityCaps( identityCaps );
		RMobilePhone::TMobilePhoneIdentityV1 id;
		TRequestStatus status;
		phone.GetPhoneId( status, id );
		User::WaitForRequest( status );
		User::LeaveIfError( status.Int() );
		switch ( aIdType )
			{
			case EPhoneManufacturer:
				if ( IsFlagSet( identityCaps, RMobilePhone::KCapsGetManufacturer ) )
					{
					aId = id.iManufacturer;
					}
				else
					{
					aId = KSmlPhoneManufacturer;
					}
				break;
			case EPhoneModelId:
				if ( IsFlagSet( identityCaps, RMobilePhone::KCapsGetModel ) )
					{
					aId = id.iModel;
					}
				else
					{
					aId = *modelId;
					}
				break;
			case EPhoneRevisionId:
				aId = id.iRevision;
				break;
			case EPhoneSerialNumber:
				aId.Format(KSmlImeiMask(), &id.iSerialNumber);
				break;
			default:
				User::Panic( KNSmlCommonAgentPanic, KErrArgument );
				break;	
			}
		aId.TrimRight();
		if ( aId.Length() > 0 )
			{
			if ( aId[aId.Length() - 1] == 0 )
				{
				aId.SetLength( aId.Length() - 1 );
				}	
			}
		CleanupStack::PopAndDestroy(); // phone
		}
	CleanupStack::PopAndDestroy(); // server
#endif // __WINS__
	CleanupStack::PopAndDestroy(); // modelID
	}
// ---------------------------------------------------------
// CNSmlPhoneInfo::SwVersionL()
// 
// ---------------------------------------------------------
EXPORT_C TPtrC8 CNSmlPhoneInfo::SwVersionL()
	{
	delete iSwVersion;
	iSwVersion = NULL;
    TBuf<KSysUtilVersionTextLength> swName;
	TChar separate('\n');
	SysUtil::GetSWVersion(swName);
	TInt index = swName.Locate(separate);
	
	if (index != KErrNotFound)
		{
		swName = swName.Mid(0, index);
		HBufC8* swVInUTF8 = NULL;
		NSmlUnicodeConverter::HBufC8InUTF8LC( swName, swVInUTF8 );
		CleanupStack::Pop();   // swVInUTF8
		iSwVersion = swVInUTF8;
		}
	else
		{
		iSwVersion = HBufC8::NewL( 0 );
		}
	
	return iSwVersion->Des();
	}

// ---------------------------------------------------------
// CNSmlPhoneInfo::IsFlagSet()
// 
// ---------------------------------------------------------
TBool CNSmlPhoneInfo::IsFlagSet( const TUint& aValue, const TUint& aBit ) const
	{
	return( (aValue & aBit) == aBit );
	}

