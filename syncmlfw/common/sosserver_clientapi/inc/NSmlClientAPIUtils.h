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
* Description: 
*		Declaration file for client API's utilities
*
*/


#ifndef __CLIENTAPIUTILS_H__
#define __CLIENTAPIUTILS_H__

#include <e32std.h>
#include <SyncMLDef.h>
#include <SyncMLClient.h>
#include <SyncMLDataFilter.h>
#include "NSmlHistoryArray.h"
#include "NSmlTransportHandler.h"

//
// class for reacting to feature support
//
class NSmlClientAPIFeatureHandler
	{
	public:
		static void LeaveIfDataSyncNotSupportedL();
		static void LeaveIfDevManNotSupportedL();
		static void LeaveIfUsageTypeNotSupportedL( TSmlUsageType aUsageType );
	
	private:
		NSmlClientAPIFeatureHandler(); // instantiation prohibited
	};

//
// base class for session data
//
class CClientSessionData : public CBase
	{
	public:
		CClientSessionData();
		virtual ~CClientSessionData();
		virtual void InternalizeL( RReadStream& aStream );
		virtual void ExternalizeL( RWriteStream& aStream ) const;		

		void ReadIntegersToArrayL( RArray<TInt>& aArray ); // from buffer to array
		
		void SetBufferSizeL( TInt aSize = -1 );			
		void ClearDataBuffer();			
		TPtr8& DataBufferPtr();
		CBufBase& DataBuffer();
		static void PanicIfAlreadyCreated( CClientSessionData* aPtr );

	private:
		CBufBase* iBuffer;
		TPtr8	iDataPtr;
	};


//
// data for history log
//
class CHistoryLogSessionData : public CClientSessionData
	{
	public:
		static CHistoryLogSessionData* NewL();
		static CHistoryLogSessionData* NewLC();
		virtual ~CHistoryLogSessionData();
		virtual void InternalizeL( RReadStream& aStream );
		
		inline CNSmlHistoryArray& History() const;
		inline const TSmlProfileId Profile() const;
		
		void SetProfile( const TSmlProfileId aProfileId );
	
	private:
		CHistoryLogSessionData();
		void ConstructL();
		
	private:
		CNSmlHistoryArray*	iHistoryArray;
		TSmlProfileId 		iProfileId;
	};


//
// data for all sync jobs (ds and dm)
//
class CJobBaseSessionData : public CClientSessionData
	{
	public:
		virtual ~CJobBaseSessionData();
		virtual void InternalizeL( RReadStream& aStream );
		
		void WriteIntegersToBufferL( const RArray<TInt>* aArray = NULL ); // from array to buffer
		
		inline TSmlJobId Identifier() const;
		inline TSmlProfileId Profile() const;
		
		void AddParamL( const TInt aParam );
		void SetIdentifier( const TSmlJobId aJobId );
		void SetProfile( const TSmlProfileId aProfileId );
		
	protected:
		CJobBaseSessionData(); // created in child classes
	
	private:
		TSmlProfileId 	iProfileId;
		RArray<TInt>	iParams;
		TSmlJobId 		iJobId;
	};


//
// data for ds sync jobs
//
class CDataSyncJobSessionData : public CJobBaseSessionData
	{
	public:
		CDataSyncJobSessionData();
		virtual ~CDataSyncJobSessionData();
		virtual void InternalizeL( RReadStream& aStream );
	
		inline const RArray<TSmlTaskId>& TaskIds() const;
		void SetTasksL( const RArray<TSmlTaskId>& aTasks );
		
	private:
		RArray<TSmlTaskId> iTasks;
	};


//
// data for dm sync jobs
//
class CDevManJobSessionData : public CJobBaseSessionData
	{
	public:
		CDevManJobSessionData();
		virtual ~CDevManJobSessionData();
		virtual void InternalizeL( RReadStream& aStream );
	};


//
// data for profiles
//
class CProfileBaseSessionData : public CClientSessionData
	{
	public:
		virtual ~CProfileBaseSessionData();
		virtual void InternalizeL( RReadStream& aStream );
		virtual void ExternalizeL( RWriteStream& aStream ) const;		
		
		inline TSmlUsageType Type() const;
		inline TBool DeleteAllowed() const;
		inline TSmlOpenMode OpenMode() const;
		inline const TDesC8& ServerId() const;
		inline const TDesC8& UserName() const;
		inline const TDesC8& Password() const;
		inline TSmlCreatorId CreatorId() const;
		inline TSmlProfileId Identifier() const;
		inline const TDesC& DisplayName() const;
		inline const TDesC8& ServerPassword() const;
		inline TSmlServerAlertedAction SanAction() const;
		inline TSmlProtocolVersion ProtocolVersion() const;
		inline TBool ProfileLocked() const;
		
		void SetType( const TSmlUsageType aUsageType );
		void SetCreatorId( const TSmlCreatorId aCreatorId );
		void SetIdentifier( const TSmlProfileId aProfileId );
		void SetDeleteAllowed( const TBool aDeleteAllowed );
		void SetProfileLock( const TBool aProfileLocked );
		void SetOpenMode( const TSmlOpenMode aOpenMode );
		void SetDisplayNameL( const TDesC& aDisplayName );
		void SetUserNameL( const TDesC8& aUserName );
		void SetPasswordL( const TDesC8& aPassword );
		void SetServerPasswordL( const TDesC8& aServerPassword );
		void SetServerIdL( const TDesC8& aServerId );
		void SetSanAction( const TSmlServerAlertedAction aSanAction );
		void SetProtocolVersionL( const TSmlProtocolVersion aVersion );
	
	protected:
		CProfileBaseSessionData(); // created in child classes
		void BaseConstructL( TBool aInitValues );
		void InitValuesL();
	  void SetProfileLockL(RReadStream& aStream );
		void GetProfileLockL(RWriteStream&aStream ) const ;
	private:
		TSmlServerAlertedAction	iSanAction;
		TSmlProtocolVersion		iProtocolVersion;
		TSmlUsageType 			iUsageType;
		TSmlCreatorId 			iCreatorId;
		TSmlProfileId 			iProfileId;
		TSmlOpenMode 			iOpenMode;
		HBufC* 					iDisplayName;
		HBufC8*					iServerPassword;
		HBufC8* 				iUserName;
		HBufC8* 				iPassword;
		HBufC8*					iServerId;
		TBool					iDeleteAllowed;
		TBool					iProfileLocked;
	};


//
// data for data sync profiles
//
class CDataSyncProfileSessionData : public CProfileBaseSessionData
	{
	public:
		static CDataSyncProfileSessionData* NewL( const TBool aInitValues = EFalse );
		static CDataSyncProfileSessionData* NewLC( const TBool aInitValues = EFalse );
		
		virtual void InternalizeL( RReadStream& aStream );
		virtual void ExternalizeL( RWriteStream& aStream ) const;		
		
		virtual ~CDataSyncProfileSessionData();
		
	private:
		CDataSyncProfileSessionData();
		void ConstructL( const TBool aInitValues );
	};


//
// data for device management profiles
//
class CDevManProfileSessionData : public CProfileBaseSessionData
	{
	public:
		static CDevManProfileSessionData* NewL( const TBool aInitValues = EFalse );
		static CDevManProfileSessionData* NewLC( const TBool aInitValues = EFalse );
		
		virtual void InternalizeL( RReadStream& aStream );
		virtual void ExternalizeL( RWriteStream& aStream ) const;		
		
		virtual ~CDevManProfileSessionData();
		
	private:
		CDevManProfileSessionData();
		void ConstructL( const TBool aInitValues );
	};


//
// data for ds tasks
//
class CTaskSessionData : public CClientSessionData
	{
	public:
		static CTaskSessionData* NewL( const TBool aInitValues = EFalse );
		static CTaskSessionData* NewLC( const TBool aInitValues = EFalse );
		virtual ~CTaskSessionData();
		virtual void InternalizeL( RReadStream& aStream );
		virtual void ExternalizeL( RWriteStream& aStream ) const;		

		inline TBool Enabled() const;
		inline TBool IsReadOnly() const;
		inline TSmlTaskId Identifier() const;
		inline TSmlProfileId Profile() const;
		inline TSmlCreatorId CreatorId() const;
		inline const TDesC& DisplayName() const;
		inline TSmlSyncType DefaultSyncType() const;
		inline const TDesC& ServerDataSource() const;
		inline const TDesC& ClientDataSource() const;
		inline TSmlDataProviderId DataProvider() const;
		
		inline TSyncMLFilterMatchType FilterMatchType() const;
		inline TSyncMLFilterChangeInfo FilterChangeInfo() const;
		
		void SetEnabled( const TBool aEnabled );
		void SetReadOnly( const TBool aReadOnly );
		void SetIdentifier( const TSmlTaskId aTaskId );
		void SetProfile( const TSmlProfileId aProfileId );
		void SetCreatorId( const TSmlCreatorId aCreatorId );
		void SetDisplayNameL( const TDesC& aDisplayName );
		void SetDefaultSyncType( TSmlSyncType aSyncType );
		void SetServerDataSourceL( const TDesC& aServerDataSource );
		void SetClientDataSourceL( const TDesC& aClientDataSource );
		void SetDataProvider( TSmlDataProviderId aDataProviderId );
		
		void SetFilterMatchType( TSyncMLFilterMatchType aFilterMatchType );
		void SetTaskSupportedFiltersL( TDesC8& aData );
		RPointerArray<CSyncMLFilter>& FilterArray();
		void CopyTaskSupportedFiltersL( RPointerArray<CSyncMLFilter>& aFilterArray );
		
		inline void SetFilterBufSize( TInt aBufferSize );
		inline TInt FilterBufSize() const;
		TInt FilterDataSize() const;
	private:
		CTaskSessionData();
		void ConstructL( const TBool aInitValues );
		void InitValuesL();
						
	private:
		TSmlDataProviderId	iDataProviderId;
		TSmlProfileId 		iProfileId;
		TSmlCreatorId 		iCreatorId;
		TSmlSyncType		iDefaultSyncType;
		TSmlTaskId 			iTaskId;
		HBufC*				iDisplayName;
		HBufC*				iServerDataSource;
		HBufC*				iClientDataSource;
		TBool				iReadOnly;
		TBool				iEnabled;
		
		TSyncMLFilterMatchType iFilterMatchType;
		TSyncMLFilterChangeInfo iFilterChangeInfo;
		RPointerArray<CSyncMLFilter> iFilterArray;
		
		TInt iFilterBufSize;
	};


//
// data for connection
//
class CConnectionSessionData : public CClientSessionData
	{
	public:
		static CConnectionSessionData* NewL( const TSmlTransportId aId, const TSmlUsageType aUsageType, const TBool aInitValues = EFalse );
		static CConnectionSessionData* NewLC( const TSmlTransportId aId, const TSmlUsageType aUsageType, const TBool aInitValues = EFalse );
		virtual ~CConnectionSessionData();
		virtual void InternalizeL( RReadStream& aStream );
		virtual void ExternalizeL( RWriteStream& aStream ) const;		
		
		inline TUint Priority() const;
		inline TUint RetryCount() const;
		inline TSmlProfileId Profile() const;
		inline TSmlOpenMode OpenMode() const;
		inline const TDesC8& ServerURI() const;
		inline TSmlTransportId Identifier() const;

		const TDesC8& GetPropertyL( const TDesC8& aName ) const;
		void SetPropertyL( const TDesC8& aName, const TDesC8& aValue );
		
		void SetPriority( const TUint aPriority );
		void SetRetryCount( const TUint aRetryCount );
		void SetServerURIL( const TDesC8& aServerURI );
		void SetIdentifier( const TSmlTransportId aId );
		void SetOpenMode( const TSmlOpenMode aOpenMode );
		void SetProfile( const TSmlProfileId aProfileId );

	private:
		TInt FindL( const TDesC8& aName ) const;
		void InitValuesL();
		
		void ConstructL( const TBool aInitValues, const TSmlUsageType aUsageType );
		CConnectionSessionData( const TSmlTransportId aId );
		
	private:
		CNSmlTransportHandler*	iHandler;
		RPointerArray<HBufC8>	iPropVals;
		TSmlTransportId 		iId;
		TSmlProfileId 			iProfileId;
		TSmlOpenMode 			iOpenMode;
		HBufC8* 				iServerURI;
		TUint 					iPriority;
		TUint 					iRetryCount;
	};
	
	
//
// data for transport
//
class CTransportSessionData : public CClientSessionData
	{
	public:
		static CTransportSessionData* NewL( const TSmlTransportId aId, const TSmlUsageType aUsageType );
		virtual ~CTransportSessionData();
		
		inline const TDesC& DisplayName() const;
		inline TSmlTransportId Identifier() const;
		inline const CSyncMLTransportPropertiesArray& Properties() const;
		
		void SetIdentifier( const TSmlTransportId aId );
		
	private:
		void ConstructL( const TSmlUsageType aUsageType );
		CTransportSessionData( const TSmlTransportId aId );
		
	private:
		CNSmlTransportHandler*	iHandler;
		TSmlTransportId			iId;
	};


//
// data for data provider
//
class CDataProviderSessionData : public CClientSessionData
	{
	public:
		CDataProviderSessionData();
		virtual ~CDataProviderSessionData();
		virtual void InternalizeL( RReadStream& aStream );
		
		inline TSmlDataProviderId Identifier() const;
		inline TVersion Version() const;
		inline const TDesC& DisplayName() const;
		inline const TInt MimeTypeCount() const;
		inline const TDesC& MimeType( TInt aIndex ) const;
		inline const TDesC& MimeVersion( TInt aIndex ) const;
		inline const TDesC& DefaultDataStoreName() const;
		inline TBool AllowsMultipleDataStores() const;
		inline TSmlProtocolVersion RequiredProtocolVersion() const;
		
		void GetDataStoreNamesL( CDesCArray& aArray ) const;
		
		void SetIdentifier( const TSmlDataProviderId aId );
		
	private:
		RPointerArray<HBufC>	iDataStores; // default store is the first
		RPointerArray<HBufC>	iMimeTypes;
		RPointerArray<HBufC>	iMimeVersions;
		TSmlProtocolVersion		iRequiredProtocolVersion;
		TSmlDataProviderId 		iId;
		TVersion				iVersion;
		HBufC* 					iDisplayName;
		TBool 					iAllowMultipleDataStores;
	};

	
#include "NSmlClientAPIUtils.inl"

#endif      // __CLIENTAPIUTILS_H__
            

