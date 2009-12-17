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
* Description:  Implementation file for client API utilities' getters
*
*/


// -----------------------------------------------------------------------------
// CHistoryLogSessionData::History()
// -----------------------------------------------------------------------------
//
CNSmlHistoryArray& CHistoryLogSessionData::History() const	
	{
	return *iHistoryArray;
	}
	
// -----------------------------------------------------------------------------
// CHistoryLogSessionData::Profile()
// -----------------------------------------------------------------------------
//
const TSmlProfileId CHistoryLogSessionData::Profile() const	
	{
	return iProfileId;
	}

// -----------------------------------------------------------------------------
// CJobBaseSessionData::Identifier()
// -----------------------------------------------------------------------------
//
TSmlJobId CJobBaseSessionData::Identifier() const
	{
	return iJobId;
	}
	
// -----------------------------------------------------------------------------
// CJobBaseSessionData::Profile()
// -----------------------------------------------------------------------------
//
TSmlProfileId CJobBaseSessionData::Profile() const
	{
	return iProfileId;
	}

// -----------------------------------------------------------------------------
// CDataSyncJobSessionData::TaskIds()
// -----------------------------------------------------------------------------
//
const RArray<TSmlTaskId>& CDataSyncJobSessionData::TaskIds() const
	{
	return iTasks;
	}
		
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::Type()
// -----------------------------------------------------------------------------
//
TSmlUsageType CProfileBaseSessionData::Type() const
	{
	return iUsageType;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::DeleteAllowed()
// -----------------------------------------------------------------------------
//
TBool CProfileBaseSessionData::DeleteAllowed() const
	{
	return iDeleteAllowed;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::ProfileLocked()
// -----------------------------------------------------------------------------
//	
TBool CProfileBaseSessionData::ProfileLocked() const
	{
	return iProfileLocked;
	}	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::OpenMode()
// -----------------------------------------------------------------------------
//
TSmlOpenMode CProfileBaseSessionData::OpenMode() const
	{
	return iOpenMode;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::ServerId()
// -----------------------------------------------------------------------------
//
const TDesC8& CProfileBaseSessionData::ServerId() const
	{
	return *iServerId;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::UserName()
// -----------------------------------------------------------------------------
//
const TDesC8& CProfileBaseSessionData::UserName() const
	{
	return *iUserName;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::Password()
// -----------------------------------------------------------------------------
//
const TDesC8& CProfileBaseSessionData::Password() const
	{
	return *iPassword;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::CreatorId()
// -----------------------------------------------------------------------------
//
TSmlCreatorId CProfileBaseSessionData::CreatorId() const
	{
	return iCreatorId;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::Identifier()
// -----------------------------------------------------------------------------
//
TSmlProfileId CProfileBaseSessionData::Identifier() const
	{
	return iProfileId;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::DisplayName()
// -----------------------------------------------------------------------------
//
const TDesC& CProfileBaseSessionData::DisplayName() const
	{
	return *iDisplayName;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::ServerPassword()
// -----------------------------------------------------------------------------
//
const TDesC8& CProfileBaseSessionData::ServerPassword() const
	{
	return *iServerPassword;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::SanAction()
// -----------------------------------------------------------------------------
//
TSmlServerAlertedAction CProfileBaseSessionData::SanAction() const
	{
	return iSanAction;
	}
	
// -----------------------------------------------------------------------------
// CProfileBaseSessionData::ProtocolVersion()
// -----------------------------------------------------------------------------
//
TSmlProtocolVersion CProfileBaseSessionData::ProtocolVersion() const
	{
	return iProtocolVersion;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::Enabled()
// -----------------------------------------------------------------------------
//
TBool CTaskSessionData::Enabled() const
	{
	return iEnabled;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::IsReadOnly()
// -----------------------------------------------------------------------------
//
TBool CTaskSessionData::IsReadOnly() const
	{
	return iReadOnly;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::Identifier()
// -----------------------------------------------------------------------------
//
TSmlTaskId CTaskSessionData::Identifier() const
	{
	return iTaskId;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::Profile()
// -----------------------------------------------------------------------------
//
TSmlProfileId CTaskSessionData::Profile() const
	{
	return iProfileId;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::CreatorId()
// -----------------------------------------------------------------------------
//
TSmlCreatorId CTaskSessionData::CreatorId() const
	{
	return iCreatorId;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::DisplayName()
// -----------------------------------------------------------------------------
//
const TDesC& CTaskSessionData::DisplayName() const
	{
	return *iDisplayName;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::DefaultSyncType()
// -----------------------------------------------------------------------------
//
TSmlSyncType CTaskSessionData::DefaultSyncType() const
	{
	return iDefaultSyncType;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::ServerDataSource()
// -----------------------------------------------------------------------------
//
const TDesC& CTaskSessionData::ServerDataSource() const
	{
	return *iServerDataSource;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::ClientDataSource()
// -----------------------------------------------------------------------------
//
const TDesC& CTaskSessionData::ClientDataSource() const
	{
	return *iClientDataSource;
	}
	
// -----------------------------------------------------------------------------
// CTaskSessionData::DataProvider()
// -----------------------------------------------------------------------------
//
TSmlDataProviderId CTaskSessionData::DataProvider() const
	{
	return iDataProviderId;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::FilterMatchType()
// -----------------------------------------------------------------------------
//
TSyncMLFilterMatchType CTaskSessionData::FilterMatchType() const
	{
	return iFilterMatchType;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::FilterChangeInfo()
// -----------------------------------------------------------------------------
//
TSyncMLFilterChangeInfo CTaskSessionData::FilterChangeInfo() const
	{
	return iFilterChangeInfo;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::FilterBufSize() const
// -----------------------------------------------------------------------------
//	
TInt CTaskSessionData::FilterBufSize() const
	{
	return iFilterBufSize;
	}

// -----------------------------------------------------------------------------
// CTaskSessionData::SetFilterBufSize( TInt aBufferSize )
// -----------------------------------------------------------------------------
//	
void CTaskSessionData::SetFilterBufSize( TInt aBufferSize )
	{
	iFilterBufSize = aBufferSize;
	}
		
// -----------------------------------------------------------------------------
// CConnectionSessionData::Priority()
// -----------------------------------------------------------------------------
//
TUint CConnectionSessionData::Priority() const
	{
	return iPriority;
	}

// -----------------------------------------------------------------------------
// CConnectionSessionData::RetryCount()
// -----------------------------------------------------------------------------
//
TUint CConnectionSessionData::RetryCount() const
	{
	return iRetryCount;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::Profile()
// -----------------------------------------------------------------------------
//
TSmlProfileId CConnectionSessionData::Profile() const
	{
	return iProfileId;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::OpenMode()
// -----------------------------------------------------------------------------
//
TSmlOpenMode CConnectionSessionData::OpenMode() const
	{
	return iOpenMode;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::ServerURI()
// -----------------------------------------------------------------------------
//
const TDesC8& CConnectionSessionData::ServerURI() const
	{
	return *iServerURI;
	}
	
// -----------------------------------------------------------------------------
// CConnectionSessionData::Identifier()
// -----------------------------------------------------------------------------
//
TSmlTransportId CConnectionSessionData::Identifier() const
	{
	return iId;
	}

// -----------------------------------------------------------------------------
// CTransportSessionData::DisplayName()
// -----------------------------------------------------------------------------
//
const TDesC& CTransportSessionData::DisplayName() const
	{
	return iHandler->DisplayName();
	}
	
// -----------------------------------------------------------------------------
// CTransportSessionData::Identifier()
// -----------------------------------------------------------------------------
//
TSmlTransportId CTransportSessionData::Identifier() const
	{
	return iId;
	}
	
// -----------------------------------------------------------------------------
// CTransportSessionData::Properties()
// -----------------------------------------------------------------------------
//
const CSyncMLTransportPropertiesArray& CTransportSessionData::Properties() const
	{
	return iHandler->Array();
	}

// -----------------------------------------------------------------------------
// CDataProviderSessionData::Identifier()
// -----------------------------------------------------------------------------
//
TSmlDataProviderId CDataProviderSessionData::Identifier() const
	{
	return iId;
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::Version()
// -----------------------------------------------------------------------------
//
TVersion CDataProviderSessionData::Version() const
	{
	return iVersion;
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::DisplayName()
// -----------------------------------------------------------------------------
//
const TDesC& CDataProviderSessionData::DisplayName() const
	{
	return *iDisplayName;
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::MimeTypeCount()
// -----------------------------------------------------------------------------
//
const TInt CDataProviderSessionData::MimeTypeCount() const
	{
	return iMimeTypes.Count();
	}

// -----------------------------------------------------------------------------
// CDataProviderSessionData::MimeType()
// -----------------------------------------------------------------------------
//
const TDesC& CDataProviderSessionData::MimeType( TInt aIndex ) const
	{
	return *iMimeTypes[ aIndex ];
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::MimeVersion()
// -----------------------------------------------------------------------------
//
const TDesC& CDataProviderSessionData::MimeVersion( TInt aIndex ) const
	{
	return *iMimeVersions[ aIndex ];
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::DefaultDataStoreName()
// -----------------------------------------------------------------------------
//
const TDesC& CDataProviderSessionData::DefaultDataStoreName() const
	{
	return *iDataStores[0];
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::AllowsMultipleDataStores()
// -----------------------------------------------------------------------------
//
TBool CDataProviderSessionData::AllowsMultipleDataStores() const
	{
	return iAllowMultipleDataStores;
	}
	
// -----------------------------------------------------------------------------
// CDataProviderSessionData::RequiredProtocolVersion()
// -----------------------------------------------------------------------------
//
TSmlProtocolVersion CDataProviderSessionData::RequiredProtocolVersion() const
	{
	return iRequiredProtocolVersion;
	}

            

