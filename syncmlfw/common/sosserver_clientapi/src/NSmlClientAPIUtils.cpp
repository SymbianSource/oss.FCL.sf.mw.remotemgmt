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
* Description:  Implementation for CClientSessionData objects
*
*/


#include <s32mem.h>
#include <featmgr.h>
#include <badesca.h> // CDesCArray

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include "NSmlClientAPIUtils.h"
#include "NSmlClientAPIDefs.h"
#include "nsmlsosserverdefs.h"
#include "NSmlTransportHandler.h"


// ============================ MEMBER FUNCTIONS ===============================

//
// CNSmlClientAPIFeatureHandler
//

// -----------------------------------------------------------------------------
// NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL()
// Leaves with KErrNotSupported, if data sync is not supported.
// -----------------------------------------------------------------------------
//
void NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL()
	{
	if ( ! FeatureManager::FeatureSupported( KFeatureIdSyncMlDs ) )
		{
		_DBG_FILE("CNSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL: NOT SUPPORTED !");
		User::Leave( KErrNotSupported );
		}
	}
	
// -----------------------------------------------------------------------------
// NSmlClientAPIFeatureHandler::LeaveIfDevManNotSupportedL()
// Leaves with KErrNotSupported, if device management is not 
// supported.
// -----------------------------------------------------------------------------
//
void NSmlClientAPIFeatureHandler::LeaveIfDevManNotSupportedL()
	{
	if ( ! FeatureManager::FeatureSupported( KFeatureIdSyncMlDm ) )
		{
		_DBG_FILE("CNSmlClientAPIFeatureHandler::LeaveIfDevManNotSupportedL: NOT SUPPORTED !");
		User::Leave( KErrNotSupported );
		}
	}

// -----------------------------------------------------------------------------
// NSmlClientAPIFeatureHandler::LeaveIfUsageTypeNotSupportedL(
// Leaves with KErrNotSupported, if the given usage type is 
// not supported.
// -----------------------------------------------------------------------------
//
void NSmlClientAPIFeatureHandler::LeaveIfUsageTypeNotSupportedL( TSmlUsageType aUsageType )
	{
	switch ( aUsageType )
		{
		case ESmlDataSync:
			NSmlClientAPIFeatureHandler::LeaveIfDataSyncNotSupportedL();
			break;
		case ESmlDevMan:
			NSmlClientAPIFeatureHandler::LeaveIfDevManNotSupportedL();
			break;
		default:
			break;
		}
	}
	

//
// CClientSessionData
//


// -----------------------------------------------------------------------------
// CClientSessionData::CClientSessionData()
// -----------------------------------------------------------------------------
//
CClientSessionData::CClientSessionData() 
	: iDataPtr( 0, NULL, 0)
	{
	}
	
// -----------------------------------------------------------------------------
// CClientSessionData::~CClientSessionData()
// -----------------------------------------------------------------------------
//
CClientSessionData::~CClientSessionData() 
	{
	delete iBuffer;
	iBuffer = NULL;
	}
	
// -----------------------------------------------------------------------------
// CClientSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CClientSessionData::InternalizeL( RReadStream& /*aStream*/ )
	{
	}
	
// -----------------------------------------------------------------------------
// CClientSessionData::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CClientSessionData::ExternalizeL( RWriteStream& /*aStream*/ ) const
	{
	}

// -----------------------------------------------------------------------------
// CClientSessionData::DataBuffer()
// Returns reference to data buffer
// -----------------------------------------------------------------------------
//
CBufBase& CClientSessionData::DataBuffer()
    {
    return *iBuffer;
    }
// -----------------------------------------------------------------------------
// CClientSessionData::ReadIntegersToArrayL()
// Reads integers from iBuffer to aArray.
// -----------------------------------------------------------------------------
//
void CClientSessionData::ReadIntegersToArrayL( RArray<TInt>& aArray )
	{
	// read data using a stream
    RDesReadStream stream( iDataPtr );
	CleanupClosePushL( stream );
    
    TInt count = stream.ReadInt8L();

	// append inttegers to array
    for ( TInt i = 0; i < count; ++i )
        {
        aArray.AppendL( stream.ReadInt32L() );
        }

    CleanupStack::PopAndDestroy(); // stream
	}
	
// -----------------------------------------------------------------------------
// CClientSessionData::SetBufferSizeL()
// -----------------------------------------------------------------------------
//
void CClientSessionData::SetBufferSizeL( TInt aSize ) 
	{
	if ( iBuffer )
		{
		ClearDataBuffer();
		}
		
	iBuffer = CBufFlat::NewL( KDefaultNSmlBufferGranularity );	
	if ( aSize != -1 )
	    {
	    iBuffer->ResizeL( aSize );    
	    }
	}

// -----------------------------------------------------------------------------
// CClientSessionData::ClearDataBuffer()
// -----------------------------------------------------------------------------
//
void CClientSessionData::ClearDataBuffer() 
	{
	delete iBuffer;
	iBuffer = NULL;
	}
			
// -----------------------------------------------------------------------------
// CClientSessionData::DataBuffer()
// -----------------------------------------------------------------------------
//
TPtr8& CClientSessionData::DataBufferPtr() 
	{
	iDataPtr.Set( iBuffer->Ptr(0) );
	return iDataPtr;
	}

// -----------------------------------------------------------------------------
// CClientSessionData::PanicIfAlreadyCreated()
// Panics the thread if the given pointer is not NULL, i.e.
// the object has been instantiated. This method is used 
// in session and subsession objects to check that they are not 
// open when the session is opened/created.
// -----------------------------------------------------------------------------
//
void CClientSessionData::PanicIfAlreadyCreated( CClientSessionData* aPtr )
	{
	if ( aPtr )
		{
		_DBG_FILE("CClientSessionData::PanicIfAlreadyCreated: ALREADY CREATED !");
		User::Panic(KNSmlClientAPIPanic, ESmlClientPanicSessionAlreadyOpen);
		}
	}


//
// CHistoryLogSessionData
//


// -----------------------------------------------------------------------------
// CHistoryLogSessionData::NewL()
// -----------------------------------------------------------------------------
//
CHistoryLogSessionData* CHistoryLogSessionData::NewL()
	{
	CHistoryLogSessionData* self = CHistoryLogSessionData::NewLC();
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CHistoryLogSessionData::NewLC()
// -----------------------------------------------------------------------------
//
CHistoryLogSessionData* CHistoryLogSessionData::NewLC()
	{
	CHistoryLogSessionData* self = new (ELeave) CHistoryLogSessionData();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CHistoryLogSessionData::~CHistoryLogSessionData()
// -----------------------------------------------------------------------------
//
CHistoryLogSessionData::~CHistoryLogSessionData()
	{
	delete iHistoryArray;
	}
	
// -----------------------------------------------------------------------------
// CHistoryLogSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CHistoryLogSessionData::InternalizeL( RReadStream& aStream )
	{
	iHistoryArray->InternalizeL( aStream );
	}
	
// -----------------------------------------------------------------------------
// CHistoryLogSessionData::SetProfile()
// -----------------------------------------------------------------------------
//
void CHistoryLogSessionData::SetProfile( const TSmlProfileId aProfileId )
	{
	iProfileId = aProfileId;
	}
	
// -----------------------------------------------------------------------------
// CHistoryLogSessionData::CHistoryLogSessionData()
// -----------------------------------------------------------------------------
//
CHistoryLogSessionData::CHistoryLogSessionData()
	: iProfileId( KNSmlNullId )
	{
	}
	
// -----------------------------------------------------------------------------
// CHistoryLogSessionData::ConstructL()
// -----------------------------------------------------------------------------
//
void CHistoryLogSessionData::ConstructL()
	{
	iHistoryArray = CNSmlHistoryArray::NewL();
	iHistoryArray->SetOwnerShip( ETrue );
	}
	

//
// CJobBaseSessionData
//

// -----------------------------------------------------------------------------
// CJobBaseSessionData::~CJobBaseSessionData()
// -----------------------------------------------------------------------------
//
CJobBaseSessionData::~CJobBaseSessionData()
	{
	iParams.Close();
	}
	
// -----------------------------------------------------------------------------
// CJobBaseSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CJobBaseSessionData::InternalizeL( RReadStream& aStream )
	{
	// read profile id
	iProfileId = aStream.ReadInt32L();
	}
	
// -----------------------------------------------------------------------------
// CJobBaseSessionData::WriteIntegersToBufferL()
// Writes the integers in iParams to iBuffer. If aArray is 
// present, it's count is first written to the buffer (after 
// the values in aArray) and then the integers in aArray.
// -----------------------------------------------------------------------------
//
void CJobBaseSessionData::WriteIntegersToBufferL( const RArray<TInt>* aArray )
	{
	TInt size = 0;
	
	if ( aArray )
		{
		size += aArray->Count() * KSizeofTInt32; // size of items
		size += KSizeofTInt8; // count of items
		}
		
	size += iParams.Count() * KSizeofTInt32;
	
	SetBufferSizeL( size );
	TPtr8 dataPtr = DataBufferPtr();

	// open stream to iBuffer
	RDesWriteStream stream;
	stream.Open( dataPtr );
	CleanupClosePushL( stream );

	// write integers in iParams
	for ( TInt i = 0; i < iParams.Count(); ++i )
		{
		stream.WriteInt32L( iParams[i] );
		}

	// write the count of items in aArray and then the items (if any)
	if ( aArray )
		{
		TInt count = aArray->Count();
		
		stream.WriteInt8L( count );
		
		for ( TInt j = 0; j < count; ++j )
			{
			stream.WriteInt32L( (*aArray)[j] );
			}
		}
		
	CleanupStack::PopAndDestroy(); // stream
	}

// -----------------------------------------------------------------------------
// CJobBaseSessionData::AddParamL()
// -----------------------------------------------------------------------------
//
void CJobBaseSessionData::AddParamL( const TInt aParam )
	{
	iParams.AppendL( aParam );
	}

// -----------------------------------------------------------------------------
// CJobBaseSessionData::SetIdentifier()
// -----------------------------------------------------------------------------
//
void CJobBaseSessionData::SetIdentifier( const TSmlJobId aJobId )
	{
	iJobId = aJobId;
	}
	
// -----------------------------------------------------------------------------
// CJobBaseSessionData::SetProfile()
// -----------------------------------------------------------------------------
//
void CJobBaseSessionData::SetProfile( const TSmlProfileId aProfileId )
	{
	iProfileId = aProfileId;
	}
	
// -----------------------------------------------------------------------------
// CJobBaseSessionData::CJobBaseSessionData()
// -----------------------------------------------------------------------------
//
CJobBaseSessionData::CJobBaseSessionData()
	: iJobId( KNSmlNullId )
	{
	}
	

//
// CDataSyncJobSessionData
//

// -----------------------------------------------------------------------------
// CDataSyncJobSessionData::CDataSyncJobSessionData()
// -----------------------------------------------------------------------------
//
CDataSyncJobSessionData::CDataSyncJobSessionData()
	: CJobBaseSessionData()
	{
	}
	
// -----------------------------------------------------------------------------
// CDataSyncJobSessionData::~CDataSyncJobSessionData()
// -----------------------------------------------------------------------------
//
CDataSyncJobSessionData::~CDataSyncJobSessionData()
	{
	iTasks.Close();
	}

// -----------------------------------------------------------------------------
// CDataSyncJobSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CDataSyncJobSessionData::InternalizeL( RReadStream& aStream )
	{
	// base class internalization first
	CJobBaseSessionData::InternalizeL( aStream );
	
	// read count and the task ids
	TInt count = aStream.ReadInt8L();
	
	for ( TInt i = 0; i < count; ++i )
		{
		iTasks.AppendL( aStream.ReadInt32L() );
		}
		
	}


// -----------------------------------------------------------------------------
// CDataSyncJobSessionData::SetTasksL()
// -----------------------------------------------------------------------------
//
void CDataSyncJobSessionData::SetTasksL( const RArray<TInt>& aTasks )
	{
	iTasks.Reset();
	
	for ( TInt i = 0; i < aTasks.Count(); ++i )
		{
		iTasks.AppendL( aTasks[i] );
		}
	}


//
// CDevManJobSessionData
//


// -----------------------------------------------------------------------------
// CDevManJobSessionData::CDevManJobSessionData()
// -----------------------------------------------------------------------------
//
CDevManJobSessionData::CDevManJobSessionData()
	: CJobBaseSessionData()
	{
	}
	
// -----------------------------------------------------------------------------
// CDevManJobSessionData::~CDevManJobSessionData()
// -----------------------------------------------------------------------------
//
CDevManJobSessionData::~CDevManJobSessionData()
	{
	}
	
// -----------------------------------------------------------------------------
// CDevManJobSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CDevManJobSessionData::InternalizeL( RReadStream& aStream )
	{
	CJobBaseSessionData::InternalizeL( aStream );
	}
	

//
// CProfileBaseSessionData
//

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::CProfileBaseSessionData()
// -----------------------------------------------------------------------------
//
CProfileBaseSessionData::CProfileBaseSessionData()
	: iSanAction( ESmlConfirmSync ), iCreatorId( KNSmlNullId ), iProfileId( KNSmlNullId ), 
	iOpenMode( ESmlOpenReadWrite ), iDeleteAllowed ( ETrue ),iProfileLocked ( EFalse )
	{
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::~CProfileBaseSessionData()
// -----------------------------------------------------------------------------
//
CProfileBaseSessionData::~CProfileBaseSessionData()
	{
	delete iUserName;
	delete iPassword;
	delete iDisplayName;
	delete iServerPassword;
	delete iServerId;
	}
		
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::InternalizeL( RReadStream& aStream )
	{
	iDisplayName = HBufC::NewL( aStream, aStream.ReadInt32L() );
	iUserName = HBufC8::NewL( aStream, aStream.ReadInt32L() );
	iPassword = HBufC8::NewL( aStream, aStream.ReadInt32L() );
	iServerId = HBufC8::NewL( aStream, aStream.ReadInt32L() );

	iSanAction = static_cast<TSmlServerAlertedAction>( aStream.ReadInt8L() );
	iCreatorId = aStream.ReadInt32L();
	
	iDeleteAllowed = aStream.ReadInt8L();
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt32L( iDisplayName->Size() );
	aStream << *iDisplayName;
	
	aStream.WriteInt32L( iUserName->Size() );
	aStream << *iUserName;
	
	aStream.WriteInt32L( iPassword->Size() );
	aStream << *iPassword;
	
	aStream.WriteInt32L( iServerId->Size() );
	aStream << *iServerId;
	
	aStream.WriteInt8L( iSanAction );
	aStream.WriteInt32L( iCreatorId );
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetType()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetType( TSmlUsageType aUsageType )
	{
	iUsageType = aUsageType;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetCreatorId()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetCreatorId( TSmlCreatorId aCreatorId )
	{
	iCreatorId = aCreatorId;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetIdentifier()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetIdentifier( TSmlProfileId aProfileId )
	{
	iProfileId = aProfileId;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetDeleteAllowed()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetDeleteAllowed( const TBool aDeleteAllowed )
	{
	iDeleteAllowed = aDeleteAllowed;
	}
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetProfileLock()
// -----------------------------------------------------------------------------
//	
void CProfileBaseSessionData::SetProfileLock( const TBool aProfileLocked )
	{
	iProfileLocked = aProfileLocked;
	}	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetOpenMode()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetOpenMode( TSmlOpenMode aOpenMode )
	{
	iOpenMode = aOpenMode;
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetDisplayNameL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetDisplayNameL( const TDesC& aDisplayName )
	{
	delete iDisplayName;
	iDisplayName = NULL;
	iDisplayName = aDisplayName.AllocL();
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetUserNameL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetUserNameL( const TDesC8& aUserName )
	{
	delete iUserName;
	iUserName = NULL;
	iUserName = aUserName.AllocL();
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetPasswordL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetPasswordL( const TDesC8& aPassword )
	{
	delete iPassword;
	iPassword = NULL;
	iPassword = aPassword.AllocL();
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetProfileLockL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetProfileLockL( RReadStream& aStream )
	{
	 iProfileLocked = aStream.ReadInt8L();
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::GetProfileLockL()
// -----------------------------------------------------------------------------
//	
void CProfileBaseSessionData::GetProfileLockL( RWriteStream& aStream ) const
	{
	 aStream.WriteInt8L( iProfileLocked );
	}	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetServerPasswordL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetServerPasswordL( const TDesC8& aServerPassword )
	{
	delete iServerPassword;
	iServerPassword = NULL;
	iServerPassword = aServerPassword.AllocL();
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetServerIdL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetServerIdL( const TDesC8& aServerId )
	{
	delete iServerId;
	iServerId = NULL;
	iServerId = aServerId.AllocL();
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetSanAction()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetSanAction( const TSmlServerAlertedAction aSanAction )
	{
	iSanAction = aSanAction;
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SetProtocolVersionL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::SetProtocolVersionL( const TSmlProtocolVersion aVersion )
	{
	if ( iUsageType == ESmlDevMan && aVersion != ESmlVersion1_1_2 )
		{
		User::Leave( KErrNotSupported );
		}
	
	iProtocolVersion = aVersion;
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::BaseConstructL()
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::BaseConstructL( TBool aInitValues )
	{
	if ( aInitValues )
		{
		InitValuesL();
		}
	}

// -----------------------------------------------------------------------------
// CProfileBaseSessionData::InitValuesL()
// Inits member variables so that they can be safely 
// externalized.
// -----------------------------------------------------------------------------
//
void CProfileBaseSessionData::InitValuesL()
	{
	iDisplayName = KNSmlInitialString().AllocL();
	iServerPassword = KNSmlInitialString8().AllocL();
	iUserName = KNSmlInitialString8().AllocL();
	iPassword = KNSmlInitialString8().AllocL();
	iServerId = KNSmlInitialString8().AllocL();
	}


//
// CDataSyncProfileSessionData
//

// -----------------------------------------------------------------------------
// CDataSyncProfileSessionData::NewL()
// -----------------------------------------------------------------------------
//
CDataSyncProfileSessionData* CDataSyncProfileSessionData::NewL( const TBool aInitValues )
	{
	CDataSyncProfileSessionData* self = CDataSyncProfileSessionData::NewLC( aInitValues );
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CDataSyncProfileSessionData::NewLC()
// -----------------------------------------------------------------------------
//
CDataSyncProfileSessionData* CDataSyncProfileSessionData::NewLC( const TBool aInitValues )
	{
	CDataSyncProfileSessionData* self = new (ELeave) CDataSyncProfileSessionData();
	CleanupStack::PushL( self );
	self->ConstructL( aInitValues );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CDataSyncProfileSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CDataSyncProfileSessionData::InternalizeL( RReadStream& aStream )
	{
	CProfileBaseSessionData::InternalizeL( aStream );
	SetProtocolVersionL( static_cast<TSmlProtocolVersion>( aStream.ReadInt8L() ) );
	SetServerPasswordL( KNSmlInitialString8() ); // not received in internalize, dm spesific
	}
	
// -----------------------------------------------------------------------------
// CDataSyncProfileSessionData::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CDataSyncProfileSessionData::ExternalizeL( RWriteStream& aStream ) const
	{
	CProfileBaseSessionData::ExternalizeL( aStream );
	aStream.WriteInt8L( ProtocolVersion() );
	aStream.CommitL();
	}
	
// -----------------------------------------------------------------------------
// CDataSyncProfileSessionData::~CDataSyncProfileSessionData()
// -----------------------------------------------------------------------------
//
CDataSyncProfileSessionData::~CDataSyncProfileSessionData()
	{
	}

// -----------------------------------------------------------------------------
// CDataSyncProfileSessionData::CDataSyncProfileSessionData()
// -----------------------------------------------------------------------------
//
CDataSyncProfileSessionData::CDataSyncProfileSessionData()
	: CProfileBaseSessionData()
	{
	SetType( ESmlDataSync );
	}

// -----------------------------------------------------------------------------
// CDataSyncProfileSessionData::ConstructL()
// -----------------------------------------------------------------------------
//
void CDataSyncProfileSessionData::ConstructL( const TBool aInitValues )
	{
	BaseConstructL( aInitValues );
	SetProtocolVersionL( ESmlVersion1_2 );
	}


//
// CDevManProfileSessionData
//

// -----------------------------------------------------------------------------
// CDevManProfileSessionData::NewL()
// -----------------------------------------------------------------------------
//
CDevManProfileSessionData* CDevManProfileSessionData::NewL( const TBool aInitValues )
	{
	CDevManProfileSessionData* self = CDevManProfileSessionData::NewLC( aInitValues );
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CDevManProfileSessionData::NewLC()
// -----------------------------------------------------------------------------
//
CDevManProfileSessionData* CDevManProfileSessionData::NewLC( const TBool aInitValues )
	{
	CDevManProfileSessionData* self = new (ELeave) CDevManProfileSessionData();
	CleanupStack::PushL( self );
	self->ConstructL( aInitValues );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CDevManProfileSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CDevManProfileSessionData::InternalizeL( RReadStream& aStream )
	{
	CProfileBaseSessionData::InternalizeL( aStream );
	SetProfileLockL(aStream);
	HBufC8* passwd = HBufC8::NewLC( aStream, aStream.ReadInt32L() );
	SetServerPasswordL( *passwd );
	CleanupStack::PopAndDestroy();
	}
	
// -----------------------------------------------------------------------------
// CDevManProfileSessionData::ExternalizeL(
// -----------------------------------------------------------------------------
//
void CDevManProfileSessionData::ExternalizeL( RWriteStream& aStream ) const
	{
	CProfileBaseSessionData::ExternalizeL( aStream );
	GetProfileLockL(aStream);
	aStream.WriteInt32L( ServerPassword().Size() );
	aStream << ServerPassword();
	aStream.CommitL();
	}
	
// -----------------------------------------------------------------------------
// CDevManProfileSessionData::~CDevManProfileSessionData()
// -----------------------------------------------------------------------------
//
CDevManProfileSessionData::~CDevManProfileSessionData()
	{
	}

// -----------------------------------------------------------------------------
// CDevManProfileSessionData::CDevManProfileSessionData()
// -----------------------------------------------------------------------------
//
CDevManProfileSessionData::CDevManProfileSessionData()
	: CProfileBaseSessionData()
	{
	SetType( ESmlDevMan );
	}
	
// -----------------------------------------------------------------------------
// CDevManProfileSessionData::ConstructL(
// -----------------------------------------------------------------------------
//
void CDevManProfileSessionData::ConstructL( const TBool aInitValues )
	{
	BaseConstructL( aInitValues );
	SetProtocolVersionL( ESmlVersion1_1_2 ); // not received in internalize
	}


//
// CTaskSessionData
//

// -----------------------------------------------------------------------------
// CTaskSessionData::NewL()
// -----------------------------------------------------------------------------
//
CTaskSessionData* CTaskSessionData::NewL( const TBool aInitValues )
	{
	CTaskSessionData* self = CTaskSessionData::NewLC( aInitValues );
	CleanupStack::Pop();
	return self;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::NewLC()
// -----------------------------------------------------------------------------
//
CTaskSessionData* CTaskSessionData::NewLC( const TBool aInitValues )
	{
	CTaskSessionData* self = new (ELeave) CTaskSessionData();
	CleanupStack::PushL( self );
	self->ConstructL( aInitValues );
	return self;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::~CTaskSessionData()
// -----------------------------------------------------------------------------
//
CTaskSessionData::~CTaskSessionData()
	{
	delete iDisplayName;
	delete iServerDataSource;
	delete iClientDataSource;
	
	iFilterArray.ResetAndDestroy();
	iFilterArray.Close();
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::InternalizeL( RReadStream& aStream )
	{
	iServerDataSource = HBufC::NewL( aStream, aStream.ReadInt32L() );
	iClientDataSource = HBufC::NewL( aStream, aStream.ReadInt32L() );
	
	iDataProviderId = aStream.ReadInt32L();
	iEnabled = aStream.ReadInt8L();
	iCreatorId = aStream.ReadInt32L();
	
	iDisplayName = HBufC::NewL( aStream, aStream.ReadInt32L() );

	iDefaultSyncType = static_cast<TSmlSyncType>( aStream.ReadInt8L() );
	
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::ExternalizeL(
// -----------------------------------------------------------------------------
//
void CTaskSessionData::ExternalizeL( RWriteStream& aStream ) const
	{
	aStream.WriteInt32L( iServerDataSource->Size() );
	aStream << *iServerDataSource;
	
	aStream.WriteInt32L( iClientDataSource->Size() );
	aStream << *iClientDataSource;
	
	aStream.WriteInt8L( iEnabled );
	aStream.WriteInt32L( iCreatorId );
	
	aStream.WriteInt32L( iDisplayName->Size() );
	aStream << *iDisplayName;

	aStream.WriteInt8L( iDefaultSyncType );
	
	aStream.WriteInt8L( iFilterMatchType );
	
	aStream.WriteInt32L( FilterBufSize() );

	aStream.WriteInt32L( iFilterArray.Count() );
	
	
	for (TInt i = 0; i < iFilterArray.Count(); i++)
		{
		CSyncMLFilter* filter = iFilterArray[i];
		filter->ExternalizeL( aStream );
		}
	
	aStream.CommitL();
	}
		
// -----------------------------------------------------------------------------
// CTaskSessionData::FilterDataSize()
// Returns the size of the buffer needed for externalizing 
// this object.
// -----------------------------------------------------------------------------
//
TInt CTaskSessionData::FilterDataSize() const
	{
	
	TInt size(0);
	
	//CSyncMLFilter
	for ( TInt index = 0; index < iFilterArray.Count(); index++ )
		{
		size += iFilterArray[index]->DataSize();
		}
		
	return size;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetEnabled()
// 
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetEnabled( const TBool aEnabled )
	{
	iEnabled = aEnabled;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetReadOnly()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetReadOnly( TBool aReadOnly )
	{
	iReadOnly = aReadOnly;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetIdentifier()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetIdentifier( const TSmlTaskId aTaskId )
	{
	iTaskId = aTaskId;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetProfile()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetProfile( const TSmlProfileId aProfileId )
	{
	iProfileId = aProfileId;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetCreatorId()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetCreatorId( const TSmlCreatorId aCreatorId )
	{
	iCreatorId = aCreatorId;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetDisplayNameL()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetDisplayNameL( const TDesC& aDisplayName )
	{
	delete iDisplayName;
	iDisplayName = NULL;
	iDisplayName = aDisplayName.AllocL();
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetDefaultSyncType()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetDefaultSyncType( const TSmlSyncType aSyncType )
	{
	iDefaultSyncType = aSyncType;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::SetFilterMatchType()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetFilterMatchType( TSyncMLFilterMatchType aFilterMatchType )
	{
	iFilterMatchType = aFilterMatchType;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetServerDataSourceL()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetServerDataSourceL( const TDesC& aServerDataSource )
	{
	delete iServerDataSource;
	iServerDataSource = NULL;
	iServerDataSource = aServerDataSource.AllocL();
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetClientDataSourceL()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetClientDataSourceL( const TDesC& aClientDataSource )
	{
	delete iClientDataSource;
	iClientDataSource = NULL;
	iClientDataSource = aClientDataSource.AllocL();
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetDataProvider()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetDataProvider( const TSmlDataProviderId aDataProviderId )
	{
	iDataProviderId = aDataProviderId;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::CTaskSessionData()
// -----------------------------------------------------------------------------
//
CTaskSessionData::CTaskSessionData()
	: iCreatorId( KNSmlNullId ), iTaskId( KNSmlNullId ), 
	iReadOnly( EFalse ), iEnabled( ETrue )
	{
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::ConstructL()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::ConstructL( const TBool aInitValues )
	{
	if ( aInitValues )
		{
		InitValuesL();
		}
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::InitValuesL()
// -----------------------------------------------------------------------------
//
void CTaskSessionData::InitValuesL()
	{
	iDisplayName = KNSmlInitialString().AllocL();
	iServerDataSource = KNSmlInitialString().AllocL();
	iClientDataSource = KNSmlInitialString().AllocL();
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::SetServerSupportedFiltersL( TDesC8& aData )
// -----------------------------------------------------------------------------
//
void CTaskSessionData::SetTaskSupportedFiltersL( TDesC8& aData )
	{
	RDesReadStream stream;
	stream.Open( aData );
	CleanupClosePushL( stream );
	
	//Clear filter buffer
	iFilterArray.ResetAndDestroy();
	
	iFilterChangeInfo = ( TSyncMLFilterChangeInfo ) stream.ReadInt8L();
	iFilterMatchType = ( TSyncMLFilterMatchType ) stream.ReadInt8L();
	TInt count = stream.ReadInt32L();
		
	for ( TInt i = 0; i < count; i++ )
		{
		CSyncMLFilter* filter = CSyncMLFilter::NewLC( stream );
		iFilterArray.AppendL( filter );
		CleanupStack::Pop(); //filter
		}
	CleanupStack::PopAndDestroy(); //stream
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::CopyTaskSupportedFiltersL( RPointerArray<CSyncMLFilter>& aFilterArray )
// Ownership is moved from caller
// -----------------------------------------------------------------------------
//
void CTaskSessionData::CopyTaskSupportedFiltersL( RPointerArray<CSyncMLFilter>& aFilterArray )
	{
	iFilterArray.ResetAndDestroy();
	
	while ( aFilterArray.Count() )
		{
		CSyncMLFilter* filter = aFilterArray[0];
		iFilterArray.AppendL( filter );
		aFilterArray.Remove(0);
		}
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::FilterArray()
// -----------------------------------------------------------------------------
//	
RPointerArray<CSyncMLFilter>& CTaskSessionData::FilterArray()
	{
	return iFilterArray;
	}
	
//
// CConnectionSessionData
//


// -----------------------------------------------------------------------------
// CConnectionSessionData::NewL()
// Two-phase construction. Set aInitValues to ETrue, when the 
// data is not internalized from stream. In that case data members
// are set to such values that this object can be successfully 
// externalized.
// -----------------------------------------------------------------------------
//
CConnectionSessionData* CConnectionSessionData::NewL( const TSmlTransportId aId, const TSmlUsageType aUsageType, const TBool aInitValues )
	{
	CConnectionSessionData* self = CConnectionSessionData::NewLC( aId, aUsageType, aInitValues );
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::NewLC()
// -----------------------------------------------------------------------------
//
CConnectionSessionData* CConnectionSessionData::NewLC( const TSmlTransportId aId, const TSmlUsageType aUsageType, const TBool aInitValues )
	{
	CConnectionSessionData* self = new (ELeave) CConnectionSessionData( aId );
	CleanupStack::PushL(self);
	self->ConstructL( aInitValues, aUsageType );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::~CConnectionSessionData()
// -----------------------------------------------------------------------------
//
CConnectionSessionData::~CConnectionSessionData()
	{
	iPropVals.ResetAndDestroy();
	iPropVals.Close();
	
	delete iHandler;
	delete iServerURI;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::InternalizeL( RReadStream& aStream )
	{
	iPropVals.ResetAndDestroy();
	delete iServerURI;
	iServerURI = NULL;
	
	// read server uri
	iServerURI = HBufC8::NewL( aStream, aStream.ReadInt32L() );
	
	// read each property
	TInt count = iHandler->Array().Count();
	for ( TInt i = 0; i < count; ++i )
		{
		TInt data = aStream.ReadInt32L();
		switch ( iHandler->Array()[i].iDataType )
			{
			
			case EDataTypeText8:
				{
				// data is the size of the string
				HBufC8* value = HBufC8::NewLC( aStream, data );
				iPropVals.AppendL( value );
				CleanupStack::Pop();
				}
				break;
				
			case EDataTypeNumber:
			case EDataTypeBoolean:
				{
				// data is the value, convert it to string
				HBufC8* value = HBufC8::NewLC( KNSmlMaxInt32Length );
				value->Des().Num( data );
				iPropVals.AppendL( value );
				CleanupStack::Pop();
				}
				break;
				
			default:
				_DBG_FILE("CConnectionSessionData::InternalizeL: UNKNOWN DATATYPE !");
				User::Panic( KNSmlClientAPIPanic, KErrUnknown );
			}
		
		}
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::ExternalizeL( RWriteStream& aStream ) const
	{
	// write transport id and server uri
	aStream.WriteInt32L( iId );
	aStream.WriteInt32L( iServerURI->Size() );
	aStream << *iServerURI;

	// write each property
	TInt count = iHandler->Array().Count();
	for ( TInt i = 0; i < count; ++i )
		{
		switch ( iHandler->Array()[i].iDataType )
			{
			
			case EDataTypeText8:
				{
				aStream.WriteInt32L( iPropVals[i]->Size() );
				aStream << *iPropVals[i];
				}
				break;
				
			case EDataTypeNumber:
			case EDataTypeBoolean:
				{
				TLex8 lex( *iPropVals[i] );
				TInt value = 0;
				lex.Val( value );
				aStream.WriteInt32L( value );
				}
				break;
				
			default:
				_DBG_FILE("CConnectionSessionData::ExternalizeL: UNKNOWN DATATYPE !");
				User::Panic( KNSmlClientAPIPanic, KErrUnknown );
			}
		
		}
	
	aStream.CommitL();
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::GetPropertyL()
// Finds and returns the value of the given property. If 
// it is not found, leaves with KErrNotFound.
// -----------------------------------------------------------------------------
//
const TDesC8& CConnectionSessionData::GetPropertyL( const TDesC8& aName ) const
	{
	TInt index = FindL( aName );
	return *iPropVals[ index ];
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::SetPropertyL()
// If this transport has a property with name aName, then 
// that property's value is set to aValue.
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::SetPropertyL( const TDesC8& aName, const TDesC8& aValue )
	{
	TInt index = FindL( aName );
	delete iPropVals[ index ];
	iPropVals[ index ] = NULL;
	iPropVals[ index ] = aValue.AllocL();
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::SetPriority()
// Not supported.
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::SetPriority( const TUint /*aPriority*/ )
	{
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::SetRetryCount()
// Not supported.
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::SetRetryCount( const TUint /*aRetryCount*/ )
	{
	}

// -----------------------------------------------------------------------------
// CConnectionSessionData::SetServerURIL()
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::SetServerURIL( const TDesC8& aServerURI )
	{
	delete iServerURI;
	iServerURI = NULL;
	iServerURI = aServerURI.AllocL();
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::SetOpenMode()
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::SetOpenMode( const TSmlOpenMode aOpenMode )
	{
	iOpenMode = aOpenMode;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::SetIdentifier()
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::SetIdentifier( const TSmlTransportId aId )
	{
	iId = aId;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::SetProfile()
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::SetProfile( const TSmlProfileId aProfileId )
	{
	iProfileId = aProfileId;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::FindL()
// Tries to find a property of name aName from iHandler. 
// If it is not found, leaves with KErrNotFound. Else returns 
// index of the property relative to zero.
// -----------------------------------------------------------------------------
//
TInt CConnectionSessionData::FindL( const TDesC8& aName ) const
	{
	TInt index = iHandler->Find( aName );
	if ( index == 0 )
		{
		User::Leave( KErrNotFound );
		}
	
	return index-1;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::InitValuesL()
// This method is used to set the data this object contains 
// to such initial values, that the values can be externalized 
// without errors. This method is used, when aInitValues 
// argument is set ETrue, when calling NewL. This should be done 
// when the data is not internalized from stream.
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::InitValuesL()
	{
	iServerURI = KNSmlInitialString8().AllocL();
	
	TInt count = iHandler->Array().Count();
	for ( TInt i = 0; i < count; ++i )
		{
		switch ( iHandler->Array()[i].iDataType )
			{
			case EDataTypeText8:
				iPropVals.AppendL( KNSmlInitialString8().AllocL() );
				break;
			case EDataTypeNumber:
			case EDataTypeBoolean:
				iPropVals.AppendL( KNSmlInitialZeroString8().AllocL() );
				break;
			default:
				_DBG_FILE("CConnectionSessionData::InitValuesL: UNKNOWN DATATYPE !");
				User::Panic( KNSmlClientAPIPanic, KErrUnknown );
				break;
			}
		}
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::ConstructL()
// -----------------------------------------------------------------------------
//
void CConnectionSessionData::ConstructL( const TBool aInitValues, const TSmlUsageType aUsageType )
	{
	iHandler = CNSmlTransportHandler::NewL( aUsageType );
	iHandler->CreateL( iId );
	
	if ( aInitValues )
		{
		InitValuesL();
		}
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::CConnectionSessionData()
// -----------------------------------------------------------------------------
//
CConnectionSessionData::CConnectionSessionData( const TSmlTransportId aId )
	:  iId( aId ), iProfileId( KNSmlNullId ), iOpenMode( ESmlOpenReadWrite )
	{
	}
	

//
// CTransportSessionData
//


// -----------------------------------------------------------------------------
// CTransportSessionData::NewL()
// -----------------------------------------------------------------------------
//
CTransportSessionData* CTransportSessionData::NewL( const TSmlTransportId aId, const TSmlUsageType aUsageType )
	{
	CTransportSessionData* self = new (ELeave) CTransportSessionData( aId );
	CleanupStack::PushL(self);
	self->ConstructL( aUsageType );
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CTransportSessionData::~CTransportSessionData()
// -----------------------------------------------------------------------------
//
CTransportSessionData::~CTransportSessionData()
	{
	delete iHandler;
	}
	
// -----------------------------------------------------------------------------
// CTransportSessionData::SetIdentifier()
// -----------------------------------------------------------------------------
//
void CTransportSessionData::SetIdentifier( const TSmlTransportId aId )
	{
	iId = aId;
	}
	
// -----------------------------------------------------------------------------
// CTransportSessionData::ConstructL()
// -----------------------------------------------------------------------------
//
void CTransportSessionData::ConstructL( const TSmlUsageType aUsageType )
	{
	if ( iHandler )
		{
		delete iHandler;
		iHandler = NULL;
		}
		
	iHandler = CNSmlTransportHandler::NewL( aUsageType );
	iHandler->CreateL( iId );
	}
	
// -----------------------------------------------------------------------------
// CTransportSessionData::CTransportSessionData()
// -----------------------------------------------------------------------------
//
CTransportSessionData::CTransportSessionData( const TSmlTransportId aId )
	: iId( aId )
	{
	}


//
// CDataProviderSessionData
//


// -----------------------------------------------------------------------------
// CDataProviderSessionData::CDataProviderSessionData()
// -----------------------------------------------------------------------------
//
CDataProviderSessionData::CDataProviderSessionData()
	: iId( KNSmlNullId ), iAllowMultipleDataStores( EFalse )
	{
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::~CDataProviderSessionData()
// -----------------------------------------------------------------------------
//
CDataProviderSessionData::~CDataProviderSessionData()
	{
	iDataStores.ResetAndDestroy();
	iDataStores.Close();

	iMimeTypes.ResetAndDestroy();
	iMimeTypes.Close();
	
	iMimeVersions.ResetAndDestroy();
	iMimeVersions.Close();
	
	delete iDisplayName;
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::InternalizeL()
// -----------------------------------------------------------------------------
//
void CDataProviderSessionData::InternalizeL( RReadStream& aStream )
	{
	iVersion.iMajor = aStream.ReadInt32L();
	iVersion.iMinor = aStream.ReadInt32L();
	iVersion.iBuild = aStream.ReadInt32L();
	
	iRequiredProtocolVersion = (TSmlProtocolVersion) aStream.ReadInt8L();
	
	iDisplayName = HBufC::NewL( aStream, aStream.ReadInt32L() );
	
	TInt mimeCount = aStream.ReadInt32L();
	for ( TInt i = 0; i < mimeCount; ++i )
		{
		TRAPD(err, iMimeTypes.AppendL( HBufC::NewL( aStream, aStream.ReadInt32L() ) ));
		if(err != KErrNone)
		    {
		    delete iDisplayName;
		    iDisplayName = NULL;
		    User::Leave( err );
		    }
		}
		
	for ( TInt j = 0; j < mimeCount; ++j )
		{
		TRAPD(err, iMimeVersions.AppendL( HBufC::NewL( aStream, aStream.ReadInt32L() ) ));
		if(err != KErrNone)
		    {
		    delete iDisplayName;
		    iDisplayName = NULL;
		    //rolling back the previous iMimeTypes append.
		    TInt total = iMimeTypes.Count();
		    for(TInt k = 1; k<= mimeCount; k++ )
		        {
		        if((total - k) >= 0)
		            {
		            iMimeTypes.Remove(total - k);
		            }
		        }
		    
		    total =  iMimeVersions.Count();
		    for(TInt l=1; l<j+1; l++)
		        {
		        if((total - l) >= 0)
		            {
		            iMimeVersions.Remove(total - l);
		            }
		        }
		    User::Leave( err );
		    }
		}
	
	TInt dataStoreCount = aStream.ReadInt8L();
	for ( TInt k = 0; k < dataStoreCount; ++k ) // default store first, then all stores
		{
		iDataStores.AppendL( HBufC::NewL( aStream, aStream.ReadInt32L() ) );
		}
	
	iAllowMultipleDataStores = aStream.ReadInt8L();
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::GetDataStoreNamesL()
// -----------------------------------------------------------------------------
//
void CDataProviderSessionData::GetDataStoreNamesL( CDesCArray& aArray ) const
	{
	aArray.Reset();
	
	// append the datastores. first slot is the default store, skip it
	// (the default store is also in the rest of the list)
	for ( TInt i = 1; i < iDataStores.Count(); ++i )
		{
		aArray.AppendL( *iDataStores[i] );
		}
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::SetIdentifier()
// -----------------------------------------------------------------------------
//
void CDataProviderSessionData::SetIdentifier( const TSmlDataProviderId aId )
	{
	iId = aId;
	}



