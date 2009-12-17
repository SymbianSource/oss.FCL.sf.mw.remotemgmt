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
* Description:  Alert info
*
*/


#include <SyncMLHistory.h>
#include <SyncMLAlertInfo.h>

#include <nsmlconstants.h>

// ---------------------------------------------------------
// CSmlAlertInfo::NewLC()
// Two phase constructor
// ---------------------------------------------------------		
EXPORT_C CSmlAlertInfo* CSmlAlertInfo::NewLC()
	{
	CSmlAlertInfo* self = new (ELeave) CSmlAlertInfo();
	CleanupStack::PushL( self );
	return self;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::CSmlAlertInfo()
// Constructor
// ---------------------------------------------------------	
EXPORT_C CSmlAlertInfo::CSmlAlertInfo()
: iVendorSpecificInfo( NULL )
	{
	
	}

// ---------------------------------------------------------
// CSmlAlertInfo::~CSmlAlertInfo()
// Destructor
// ---------------------------------------------------------	
EXPORT_C CSmlAlertInfo::~CSmlAlertInfo()
	{
	iTaskIds.Reset();
	iTaskIds.Close();
	
	iTaskSyncTypes.Reset();
	iTaskSyncTypes.Close();
	
	delete iVendorSpecificInfo;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::InternalizeL(RReadStream& aStream)
// Reads alert info from given stream
// ---------------------------------------------------------		
EXPORT_C void CSmlAlertInfo::InternalizeL(RReadStream& aStream)
	{
	iErrorCode = aStream.ReadInt32L();
	iJobControl = (CSmlAlertInfo::TJobControl) aStream.ReadInt32L();
	iProtocol = (TSmlUsageType) aStream.ReadInt32L();
	iSessionId = aStream.ReadInt32L();
	iProfile = aStream.ReadInt32L();
	iTransport = aStream.ReadInt32L();
	
	TInt vendorLength = aStream.ReadInt32L();
	iVendorSpecificInfo = HBufC8::NewL(vendorLength);
	TPtr8 ptrVendor = iVendorSpecificInfo->Des();
	aStream.ReadL( ptrVendor, vendorLength );
	
	TInt count = aStream.ReadInt32L();
	
	for (TInt i = 0; i < count; i++)
		{
		TInt id = aStream.ReadInt32L();
		iTaskIds.AppendL(id);
		
		TSmlSyncType syncType = ( TSmlSyncType )aStream.ReadInt32L();
		iTaskSyncTypes.AppendL( syncType );
		}
	}

// ---------------------------------------------------------
// CSmlAlertInfo::ExternalizeL(RWriteStream& aStream) const
// Writes alert info to given stream
// ---------------------------------------------------------		
EXPORT_C void CSmlAlertInfo::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iErrorCode);
	aStream.WriteInt32L((TInt) iJobControl);
	aStream.WriteInt32L((TInt) iProtocol);
	aStream.WriteInt32L(iSessionId);
	aStream.WriteInt32L(iProfile);
	aStream.WriteInt32L(iTransport);
	aStream.WriteInt32L( iVendorSpecificInfo->Length() );
	aStream.WriteL(iVendorSpecificInfo->Des());
		
	TInt count = iTaskIds.Count();
	aStream.WriteInt32L(count);
	
	for (TInt i = 0; i < count; i++)
		{
		aStream.WriteInt32L(iTaskIds[i]);
		aStream.WriteInt32L(iTaskSyncTypes[i]);
		}
	}

// ---------------------------------------------------------
// CSmlAlertInfo::ErrorCode() const
// Returns error code
// ---------------------------------------------------------	
EXPORT_C TInt CSmlAlertInfo::ErrorCode() const
	{
	return iErrorCode;	
	}

// ---------------------------------------------------------
// CSmlAlertInfo::JobControl() const
// Returns job control
// ---------------------------------------------------------	
EXPORT_C CSmlAlertInfo::TJobControl CSmlAlertInfo::JobControl() const
	{
	return iJobControl;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::Protocol() const
// Returns the usage type
// ---------------------------------------------------------		
EXPORT_C TSmlUsageType CSmlAlertInfo::Protocol() const
	{
	return iProtocol;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SessionId() const
// Returns the session identifier
// ---------------------------------------------------------	
EXPORT_C TInt CSmlAlertInfo::SessionId() const
	{
	return iSessionId;
	}
	
// ---------------------------------------------------------
// CSmlAlertInfo::Profile() const
// Returns the profile identifier
// ---------------------------------------------------------	
EXPORT_C TSmlProfileId CSmlAlertInfo::Profile() const
	{
	return iProfile;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::Transport() const
// Returns the transport identifier
// ---------------------------------------------------------	
EXPORT_C TSmlTransportId CSmlAlertInfo::Transport() const
	{
	return iTransport;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::Connection() const
// Returns the connection identifier
// ---------------------------------------------------------	
EXPORT_C TSmlConnectionId CSmlAlertInfo::Connection() const
	{
	return iConnection;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::TaskIds() const
// Returns reference to task id array
// ---------------------------------------------------------	
EXPORT_C const RArray<TInt>& CSmlAlertInfo::TaskIds() const
	{
	return iTaskIds;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::TaskIds() const
// Returns reference to sync type array
// ---------------------------------------------------------		
EXPORT_C const RArray<TSmlSyncType>& CSmlAlertInfo::TaskSyncTypes() const
	{
	return iTaskSyncTypes;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::VendorSpecificInfo() const
// Returns vendor specific information
// ---------------------------------------------------------		
EXPORT_C const TDesC8& CSmlAlertInfo::VendorSpecificInfo() const
	{
	if ( iVendorSpecificInfo )
		{
		return *iVendorSpecificInfo;	
		}
	return KNullDesC8;
	}	
	
// ---------------------------------------------------------
// CSmlAlertInfo::CreateSession() const
// Returns whether to create a session
// ---------------------------------------------------------
EXPORT_C TBool CSmlAlertInfo::CreateSession() const
	{
		
	if ( iJobControl == EDoNotCreateJob )
		{
		return EFalse;
		}
	
	return ETrue;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::Reset()
// Resets alert info to default values
// ---------------------------------------------------------
EXPORT_C void CSmlAlertInfo::Reset()
	{
	iErrorCode = KErrNone;
	iJobControl = EDoNotCreateJob;
	iProtocol = ESmlDataSync;
	iSessionId = 0;
	iProfile = KNSmlNullId;
	iTransport = 0;
	iTaskIds.Reset();
	iTaskSyncTypes.Reset();
	delete iVendorSpecificInfo;
	iVendorSpecificInfo = NULL;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetErrorCode(TInt aError)
// Sets error code
// ---------------------------------------------------------	
EXPORT_C void CSmlAlertInfo::SetErrorCode(TInt aError)
	{
	iErrorCode = aError;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetJobControl(TJobControl aJobControl)
// Sets job control value
// ---------------------------------------------------------	
EXPORT_C void CSmlAlertInfo::SetJobControl(TJobControl aJobControl)
	{
	iJobControl = aJobControl;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetProtocolType(TSmlUsageType aProtocol)
// Sets usage type
// ---------------------------------------------------------
EXPORT_C void CSmlAlertInfo::SetProtocolType(TSmlUsageType aProtocol)
	{
	iProtocol = aProtocol;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetSessionId(TInt aId)
// Sets session id
// ---------------------------------------------------------	
EXPORT_C void CSmlAlertInfo::SetSessionId(TInt aId)
	{
	iSessionId = aId;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetProfileId(TSmlProfileId aProfileId)
// Sets profile identifier
// ---------------------------------------------------------		
EXPORT_C void CSmlAlertInfo::SetProfileId(TSmlProfileId aProfileId)
	{
	iProfile = aProfileId;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetTransportId(TSmlTransportId aTransportId)
// Sets transport identifier
// ---------------------------------------------------------	
EXPORT_C void CSmlAlertInfo::SetTransportId(TSmlTransportId aTransportId)
	{
	iTransport = aTransportId;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetConnectionId(TSmlConnectionId aConnectionId)
// Sets connection identifier
// ---------------------------------------------------------	
EXPORT_C void CSmlAlertInfo::SetConnectionId(TSmlConnectionId aConnectionId)
	{
	iConnection = aConnectionId;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::TaskIds()
// Return reference to Task identifier array
// ---------------------------------------------------------	
EXPORT_C RArray<TInt>& CSmlAlertInfo::TaskIds()
	{
	return iTaskIds;
	}
	
// ---------------------------------------------------------
// CSmlAlertInfo::TaskSyncTypes()
// Return reference to Task sync type array
// ---------------------------------------------------------	
EXPORT_C RArray<TSmlSyncType>& CSmlAlertInfo::TaskSyncTypes()
	{
	return iTaskSyncTypes;
	}

// ---------------------------------------------------------
// CSmlAlertInfo::SetVendorSpecificInfoL(const TDesC8& aVendor)
// Sets vendor specific information
// ---------------------------------------------------------	
EXPORT_C void CSmlAlertInfo::SetVendorSpecificInfoL(const TDesC8& aVendor)
	{
	HBufC8* vendor = aVendor.AllocL();
	
	delete iVendorSpecificInfo;
	
	iVendorSpecificInfo = vendor;
	}

// ---------------------------------------------------------
// SetConfirmation(TBool aConfirmation)
// Sets confirmation to create job
// ---------------------------------------------------------		
EXPORT_C void CSmlAlertInfo::SetConfirmation(TBool aConfirmation )
	{
	
	if ( aConfirmation )
		{
		iJobControl = ECreateJob;
		}
	else
		{
		iJobControl = EDoNotCreateJob;
		}
	}
	
// ---------------------------------------------------------
// CSmlAlertInfo::SetJobControl(TJobControl aJobControl)
// Sets job control value
// ---------------------------------------------------------	
EXPORT_C void CSmlAlertInfo::SetUimode(TInt aUimode)
	{
	iUimode = aUimode;
	}
	
// ---------------------------------------------------------
// CSmlAlertInfo::SetJobControl(TJobControl aJobControl)
// Sets job control value
// ---------------------------------------------------------	
EXPORT_C TInt CSmlAlertInfo::GetUimode()
	{
	return iUimode;
	}	
//End of File

