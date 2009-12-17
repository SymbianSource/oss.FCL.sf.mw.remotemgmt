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
* Description:  Client module of DS Host Servers
*
*/


#include <e32math.h>
#include <vtoken.h>
#include <f32file.h>
#include "nsmldbcaps.h"
#include "nsmlfilter.h"
//#include "nsmlconstants.h"

const TInt KItemSize( 128 );
const TInt KNSmlMaxObjSize( 786432 );
const TSmlDbItemUid KFailDeleteUid = 13;
const TSmlDbItemUid KFailReplaceUid = 13;
const TSmlDbItemUid KFailMoveUid = 13;
const TSmlDbItemUid KInvalidParent = 113;
const TSmlDbItemUid KFieldLevelReplace = 14;
const TInt KMakeBatchFail = 5;

const TUid KContacts = { 0x101F6DDD };
const TUid KAgenda   = { 0x101F6DDE };
const TUid KEmpty1 = { 0x101F6FFF };
const TUid KEmpty2 = { 0x101F6FFE };
const TUid KUidNotValid = { 0x101F6FFD };

_LIT( KNSmlUnicodeContactsLocalDbName, "C:\\System\\Data\\Contacts.cdb" );
_LIT( KNSmlUnicodeAgenda1LocalDbName, "C:\\System\\Data\\Agenda1.cdb" );
_LIT( KNSmlUnicodeAgenda2LocalDbName, "C:\\System\\Data\\Agenda2.cdb" );
_LIT( KNSmlUnicodeEmpty1LocalDbName, "C:\\System\\Data\\Empty1.cdb" );
_LIT( KNSmlUnicodeEmpty2LocalDbName, "C:\\System\\Data\\Empty2.cdb" );
_LIT( KReceivedDataFile, "C:\ReceivedData.txt" );

_LIT8( KWriteSeparator, "\r\n==============================================\r\n" );
_LIT8( KNSmlContactsLocalDbName, "C:\\System\\Data\\Contacts.cdb" );
_LIT8( KNSmlAgenda1LocalDbName, "C:\\System\\Data\\Agenda1.cdb" );
_LIT8( KNSmlAgenda2LocalDbName, "C:\\System\\Data\\Agenda2.cdb" );
_LIT8( KNSmlEmpty1LocalDbName, "C:\\System\\Data\\Empty1.cdb" );
_LIT8( KNSmlEmpty2LocalDbName, "C:\\System\\Data\\Empty21.cdb" );
_LIT8( KNSmlvCard30Name, "text/vcard" );
_LIT8( KNSmlvCard30Ver, "3.0" );
_LIT8( KNSmlvCard21Name, "text/x-vcard" );
_LIT8( KNSmlvCard21Ver, "2.1" );

_LIT8( KVersitTokenHOME, "HOME"  );
_LIT8( KVersitTokenWORK, "WORK"  );
_LIT8( KVersitTokenCELL, "CELL"  );
_LIT8( KVersitTokenPAGER,"PAGER" );
_LIT8( KVersitTokenFAX,  "FAX"   );
_LIT8( KVersitTokenVOICE,"VOICE" );

_LIT8( KNSmlvCalendar, "text/x-vcalendar" );
_LIT8( KNSmlvCalendarVer, "1.0" );
#define KNSmlvCalendarVersion TVersion(1,0,0);

_LIT8( KNSmliCalendar, "text/calendar" );
_LIT8( KNSmliCalendarVer, "2.0" );
#define KNSmliCalendarVersion TVersion(2,0,0);

_LIT8( KVersitVarTokenVCALENDAR8, "VCALENDAR");
_LIT8( KVersitVarTokenVEVENT8, "VEVENT");
_LIT8( KVersitVarTokenVTODO8, "VTODO");
_LIT8( KVersitVarTokenXEPOCSOUND, "TYPE" );
_LIT8( KVersitVarTokenXEPOCAGENDAENTRYTYPE, "X-EPOCAGENDAENTRYTYPE" );
_LIT8( KVCalTokenPUBLIC8,"PUBLIC" );
_LIT8( KVCalTokenPRIVATE8,"PRIVATE" );
_LIT8( KVCalTokenCONFIDENTIAL8,"CONFIDENTIAL" );

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CNSmlDSHostClient
// C++ constructor.
// ------------------------------------------------------------------------------------------------
CNSmlDSHostClient::CNSmlDSHostClient()
	{
	iBatchMode = EFalse;
	iBatchCount = 0;
	iReadCount = 0;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ConstrucL
// ConstrucL
// ------------------------------------------------------------------------------------------------
void CNSmlDSHostClient::ConstructL()
	{
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SessionForDataProvider
// Return session to correct server that has current Data Provider.
// ------------------------------------------------------------------------------------------------
TInt CNSmlDSHostClient::SessionForDataProvider( const TSmlDataProviderId /*aId*/ ) const
	{	
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient - public methods
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::NewL
// Creates new instance of CNSmlDSHostClient based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostClient* CNSmlDSHostClient::NewL()
	{
	CNSmlDSHostClient* self = NewLC();
	CleanupStack::Pop();
	return self;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::NewLC
// Creates new instance of CNSmlDSHostClient based class.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostClient* CNSmlDSHostClient::NewLC()
	{
	CNSmlDSHostClient* self = new (ELeave) CNSmlDSHostClient();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::~CNSmlDSHostClient
// C++ Destructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDSHostClient::~CNSmlDSHostClient()
	{
	iBatchResults.Close();
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CreateDataProvidersL
// Creates Data Providers.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostClient::CreateDataProvidersL( const RArray<TSmlDataProviderId>& aIds, RArray<TInt>& aResultArray )
	{
	for ( TInt i = 0; i < aIds.Count(); i++ )
		{
		aResultArray.Append( KErrNone )	;
		}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CreateAllDataProvidersL
// Creates all possible Data Providers.
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDSHostClient::CreateAllDataProvidersL( RArray<TSmlDataProviderId>& aIds )
	{
	aIds.Append( KContacts.iUid );
	aIds.Append( KAgenda.iUid );
	aIds.Append( KEmpty1.iUid );
	aIds.Append( KEmpty2.iUid );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SupportsOperationL
// Asks if Data Provider supports some operation.
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDSHostClient::SupportsOperationL( TUid aOpId, TSmlDataProviderId aId, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	
	if ( aId == KContacts.iUid )
		{
		if ( aOpId == KUidSmlSupportTransaction )
			{
			return ETrue;		
			}

		if ( aOpId == KUidSmlSupportSuspendResume )
			{
			return EFalse;		
			}

		if ( aOpId == KUidSmlSupportBatch )
			{
			return ETrue;		
			}
		}
	else if ( aId == KAgenda.iUid )
		{
		if ( aOpId == KUidSmlSupportTransaction )
			{
			return EFalse;		
			}

		if ( aOpId == KUidSmlSupportSuspendResume )
			{
			return EFalse;		
			}

		if ( aOpId == KUidSmlSupportBatch )
			{
			return EFalse;		
			}
		}

	return EFalse;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::StoreFormatL
// Creates Data Store format of Data Provider.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDbCaps* CNSmlDSHostClient::StoreFormatL( const TSmlDataProviderId aId, TInt& aResultCode )
	{
	CNSmlDbCaps* dbCaps = CNSmlDbCaps::NewLC();
	
	if ( aId == KContacts.iUid || aId == KEmpty1.iUid || aId == KEmpty2.iUid )
		{
		dbCaps->SetSourceRefL( KNSmlContactsLocalDbName );
		dbCaps->SetSyncType( CNSmlDbCaps::EAllTypes );
		dbCaps->SetRxPrefL (KNSmlvCard21Name(), KNSmlvCard21Ver() );
		dbCaps->AddRxL( KNSmlvCard30Name(), KNSmlvCard30Ver() );
		dbCaps->SetTxPrefL( KNSmlvCard21Name(), KNSmlvCard21Ver() );
		dbCaps->SetSupportHierarchicalSyncL();
		
		TInt pc( 0 );
		CNSmlCtCap* cap = dbCaps->AddCtCapLC(); ++pc;
		cap->SetCtTypeL( KNSmlvCard21Name() );
		cap->SetVerCtL( KNSmlvCard21Ver() );
		CNSmlDevInfProp* pr = cap->AddDevInfPropLC( KVersitTokenBEGIN() ); ++pc;
		pr->AddValEnumL( KVersitTokenVCARD() );
		pr = cap->AddDevInfPropLC( KVersitTokenEND() ); ++pc;
		pr->AddValEnumL( KVersitTokenVCARD() );
		pr = cap->AddDevInfPropLC( KVersitTokenVERSION() ); ++pc;
		pr->AddValEnumL( KNSmlvCard21Ver() );

		cap->AddDevInfPropLC( KVersitTokenREV() ); ++pc;
		cap->AddDevInfPropLC( KVersitTokenN() ); ++pc;

		pr = cap->AddDevInfPropLC( KVersitTokenADR() ); ++pc;
		pr->AddParamLC( KVersitTokenHOME() ); ++pc;
		pr->AddParamLC( KVersitTokenWORK() ); ++pc;

		pr = cap->AddDevInfPropLC( KVersitTokenTEL() ); ++pc;
		pr->AddParamLC( KVersitTokenHOME() ); ++pc;
		pr->AddParamLC( KVersitTokenWORK() ); ++pc;
		pr->AddParamLC( KVersitTokenCELL() ); ++pc;
		pr->AddParamLC( KVersitTokenPAGER() ); ++pc;
		pr->AddParamLC( KVersitTokenFAX() ); ++pc;

		cap->AddDevInfPropLC( KVersitTokenFN() ); ++pc;

		pr = cap->AddDevInfPropLC( KVersitTokenEMAIL() ); ++pc;
		pr->AddParamLC( KVersitTokenINTERNET() ); ++pc;
		pr->AddParamLC( KVersitTokenHOME() ); ++pc;
		pr->AddParamLC( KVersitTokenWORK() ); ++pc;

		pr = cap->AddDevInfPropLC( KVersitTokenURL() ); ++pc;
		pr->AddParamLC( KVersitTokenHOME() ); ++pc;
		pr->AddParamLC( KVersitTokenWORK() ); ++pc;

		cap->AddDevInfPropLC( KVersitTokenNOTE() ); ++pc;

		cap->AddDevInfPropLC( KVersitTokenTITLE() ); ++pc;
		cap->AddDevInfPropLC( KVersitTokenORG() ); ++pc;

		cap->AddDevInfPropLC( KVersitTokenPHOTO() ); ++pc;
		cap->AddDevInfPropLC( KVersitTokenBDAY() ); ++pc;

		CleanupStack::PopAndDestroy( pc );
		}
	else
		{
		dbCaps->SetSourceRefL( KNSmlAgenda1LocalDbName );
		dbCaps->SetSyncType( CNSmlDbCaps::EAllTypes );
		dbCaps->SetRxPrefL(KNSmlvCalendar(), KNSmlvCalendarVer() );
		dbCaps->AddRxL( KNSmliCalendar(), KNSmliCalendarVer() );
		dbCaps->SetTxPrefL( KNSmlvCalendar(), KNSmlvCalendarVer() );	
		
        TInt pc( 0 );
        CNSmlCtCap* cap = dbCaps->AddCtCapLC(); pc++;
        cap->SetCtTypeL( KNSmlvCalendar() );
        cap->SetVerCtL( KNSmlvCalendarVer() );
        CNSmlDevInfProp* pr = cap->AddDevInfPropLC(KVersitTokenBEGIN()); pc++;
        pr->AddValEnumL(KVersitVarTokenVCALENDAR8());
        pr->AddValEnumL(KVersitVarTokenVEVENT8());
        pr->AddValEnumL(KVersitVarTokenVTODO8());
        pr = cap->AddDevInfPropLC(KVersitTokenEND()); pc++;
        pr->AddValEnumL(KVersitVarTokenVCALENDAR8());
        pr->AddValEnumL(KVersitVarTokenVEVENT8());
        pr->AddValEnumL(KVersitVarTokenVTODO8());

        pr = cap->AddDevInfPropLC(KVersitTokenVERSION()); pc++;
        pr->AddValEnumL( KNSmlvCalendarVer() );

        cap->AddDevInfPropLC(KVersitTokenUID()); pc++;
        cap->AddDevInfPropLC(KVersitTokenSUMMARY()); pc++;
        cap->AddDevInfPropLC(KVersitTokenDESCRIPTION()); pc++;
        cap->AddDevInfPropLC(KVersitTokenDTEND()); pc++;
        cap->AddDevInfPropLC(KVersitTokenDTSTART()); pc++;

        pr = cap->AddDevInfPropLC(KVersitTokenAALARM()); pc++;

        pr = cap->AddDevInfPropLC(KVersitTokenCLASS()); pc++;
        pr->AddValEnumL(KVCalTokenPUBLIC8());
        pr->AddValEnumL(KVCalTokenPRIVATE8());
        pr->AddValEnumL(KVCalTokenCONFIDENTIAL8());

        cap->AddDevInfPropLC(KVersitTokenCOMPLETED()); pc++;
        cap->AddDevInfPropLC(KVersitTokenLOCATION()); pc++;
        cap->AddDevInfPropLC(KVersitTokenDCREATED()); pc++;
        cap->AddDevInfPropLC(KVersitTokenLASTMODIFIED()); pc++;
        cap->AddDevInfPropLC(KVersitTokenPRIORITY()); pc++;
        cap->AddDevInfPropLC(KVersitTokenSTATUS()); pc++;
        cap->AddDevInfPropLC(KVersitTokenRRULE()); pc++;
        cap->AddDevInfPropLC(KVersitTokenDUE()); pc++;
        cap->AddDevInfPropLC(KVersitTokenEXDATE()); pc++;
        cap->AddDevInfPropLC(KVersitVarTokenXEPOCAGENDAENTRYTYPE()); pc++;
        
        CleanupStack::PopAndDestroy( pc );
		}
		
	aResultCode = KErrNone;
	CleanupStack::Pop(); // dbCaps
	
	return dbCaps;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ListStoresL
// Creates list of Data Store names of Data Provider.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ListStoresL( CDesCArray* aNameList, const TSmlDataProviderId aId, TInt& aResultCode )
	{
	if ( aId == KContacts.iUid )
		{
		aNameList->AppendL( KNSmlUnicodeContactsLocalDbName );
		}
	else if ( aId == KAgenda.iUid )
		{
		aNameList->AppendL( KNSmlUnicodeAgenda1LocalDbName );
		aNameList->AppendL( KNSmlUnicodeAgenda2LocalDbName );
		}
	else if ( aId == KEmpty1.iUid )
		{
		aNameList->AppendL( KNSmlUnicodeEmpty1LocalDbName );
		}
	else 
		{
		aNameList->AppendL( KNSmlUnicodeEmpty2LocalDbName );
		}
		
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DefaultStoreL
// Creates default name for Data Store of Data Provider.
// ------------------------------------------------------------------------------------------------	
EXPORT_C HBufC* CNSmlDSHostClient::DefaultStoreL( const TSmlDataProviderId aId, TInt& aResultCode )
	{
	HBufC* storeName;
	
	if ( aId == KContacts.iUid )
		{
		storeName = KNSmlUnicodeContactsLocalDbName().AllocL();
		}
	else if ( aId == KAgenda.iUid )
		{
		storeName = KNSmlUnicodeAgenda1LocalDbName().AllocL();
		}
	else if ( aId == KEmpty1.iUid )
		{
		storeName = KNSmlUnicodeEmpty1LocalDbName().AllocL();
		}
	else
		{
		storeName = KNSmlUnicodeEmpty2LocalDbName().AllocL();
		}
		
	aResultCode = KErrNone;
	
	return storeName;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SupportedServerFiltersL
// This method returns the set of filters that can be used to send to the Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C const RPointerArray<RSmlFilterDef> CNSmlDSHostClient::SupportedServerFiltersL( const TSmlDataProviderId /*aId*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SupportedServerFiltersL
// This method returns the set of filters that can be used to send to the Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C const RPointerArray<RSmlFilterDef> CNSmlDSHostClient::SupportedServerFiltersL( const CNSmlDbCaps& /*aServerDataStoreFormat*/, const TSmlDataProviderId /*aId*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::GenerateRecordFilterQueryLC
// This method generates a record filter query to be sent to the SyncML server for the provided filter.
// ------------------------------------------------------------------------------------------------	
EXPORT_C HBufC* CNSmlDSHostClient::GenerateRecordFilterQueryLC( const RSmlFilter& /*aFilter*/, TDes& /*aFilterMimeType*/, TDes& /*aFilterMimeVersion*/, const TSmlDataProviderId /*aId*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::GenerateFieldFilterQueryL
// This method generates a field filter query to be sent to the SyncML server for the provided filter.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::GenerateFieldFilterQueryL( const RSmlFilter& /*aFilter*/, RPointerArray<CSmlDataProperty>& /*aProperties*/, const TSmlDataProviderId /*aId*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}

EXPORT_C void CNSmlDSHostClient::GetFilterL( const TSmlDataProviderId aId, const TDesC& aStoreName, const RPointerArray<CSyncMLFilter>& aFilterArray, CNSmlFilter*& aFilter, TSyncMLFilterMatchType aMatchType, TInt& aResultCode )
    {
    aFilter->SetFilterMetaTypeL( _L8( "METATYPE" ) );
    aFilter->SetFilterTypeL( _L8( "EXCLUSIVE" ) );
    aFilter->SetRecordL( _L8( "RECORD METATYPE" ) , _L8( "RECORD DATA" ) );
    aFilter->SetFieldMetaTypeL( _L8( "FIELD METATYPE" ) );
    
    aResultCode = KErrNone;    
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::OpenL
// Opens the data store specified by aStoreName asynchronously.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::OpenL( const TSmlDataProviderId aId, const TDesC& /*aStoreName*/, const TDesC& /*aServerId*/, const TDesC& /*aRemoteDB*/, TInt& aResultCode )
	{
	if ( aId == KUidNotValid.iUid )
		{
		aResultCode = KErrNotFound;
		}
	else
		{
		aResultCode = KErrNone;
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CancelRequestL
// Cancel the current asynchronous request.
// ------------------------------------------------------------------------------------------------	
IMPORT_C void CNSmlDSHostClient::CancelRequestL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::BeginTransactionL
// Starts the transaction mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::BeginTransactionL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitTransactionL
// Method will be called at the end of a successful transaction.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitTransactionL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::RevertTransactionL
// Method will be called to abort an ongoing transaction.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::RevertTransactionL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::BeginBatchL
// Starts the batch mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::BeginBatchL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	iBatchMode = ETrue;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitBatchL
// Method will be called at the end of the batch mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitBatchL( RArray<TInt>& aResultArray, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{	
	iBatchMode = EFalse;
	TBool lastOpSucceeded( EFalse );
	
	// make batch fail
	if ( iBatchCount == KMakeBatchFail )
		{
		aResultCode = KErrGeneral;
		return;
		}
	
	for ( TInt i = 0; i < iBatchCount; i++ )
		{
/*		TTime time;
		time.HomeTime();
		TInt64 seed = time.Int64();
		
		TInt result = Math::Rand( seed ) % 2 + 1;
		aResultArray.Append( result );*/
/*		if ( lastOpSucceeded )
			{
			aResultArray.Append( KErrNotFound );
			lastOpSucceeded = EFalse;
			}
		else
			{
			aResultArray.Append( KErrNone );
			lastOpSucceeded = ETrue;
			}*/
			aResultArray.Append( iBatchResults[i] );
		}
		
	iBatchMode = EFalse;
	iBatchResults.Reset();
	
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CancelBatchL
// Method will be called to abort an ongoing batch mode.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CancelBatchL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	iBatchMode = EFalse;
	iBatchResults.Reset();
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SetRemoteDataStoreFormatL
// Sets the Sync Partner Data Format.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SetRemoteDataStoreFormatL( const CNSmlDbCaps& /*aServerDataStoreFormat*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SetRemoteMaxObjectSizeL
// Sets the SyncML server Sync Partner maximum object size.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SetRemoteMaxObjectSizeL( TInt /*aServerMaxObjectSize*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::MaxObjectSizeL
// Gets the Data Store maximum object size which is reported to the SyncML partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C TInt CNSmlDSHostClient::MaxObjectSizeL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	return KNSmlMaxObjSize;
	}	

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::OpenItemL
// Opens item at Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::OpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent, HBufC8*& aMimeType, HBufC8*& aMimeVer, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	if ( aUid == KContacts.iUid )
		{
		aFieldChange = EFalse;	
		aSize = KItemSize;
		aParent = KNullDataItemId;
		aMimeType = KNSmlvCard21Name().AllocL();
		aMimeVer = KNSmlvCard21Ver().AllocL();
		}
	else
		{
		aFieldChange = EFalse;	
		aSize = KItemSize;
		aParent = KNullDataItemId;
		aMimeType = KNSmlvCalendar().AllocL();
		aMimeVer = KNSmlvCalendarVer().AllocL();
		}
	
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CreateItemL
// Creates new item to Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CreateItemL( TSmlDbItemUid& aUid, TInt /*aSize*/, TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& /*aMimeVer*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	
	if ( aMimeType.Find( _L8("invalid") ) != KErrNotFound )
		{
		aResultCode = KErrNotSupported;
		return;
		}
		
	if ( aParent == KInvalidParent )
		{
		aResultCode = KErrPathNotFound;
		return;
		}
		
	TTime time;
	time.HomeTime();
	TInt64 seed = time.Int64();
	
	aUid = Math::Rand( seed ) % 4096 + 1;
	
	if ( iBatchMode )
		{
		++iBatchCount;
		aResultCode = KErrNone;
		
		if ( aParent == KInvalidParent )
			{
			iBatchResults.Append( KErrPathNotFound );
			}
		else
			{
			iBatchResults.Append( KErrNone );
			}
		}
	else
		{
		if ( aParent == KInvalidParent )
			{
			aResultCode = KErrPathNotFound;
			}
		else
			{
			aResultCode = KErrNone;
			}
		}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ReplaceItemL
// Replaces old item at Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ReplaceItemL( TSmlDbItemUid aUid, TInt /*aSize*/, TSmlDbItemUid aParent, TBool aFieldChange, TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	if ( iBatchMode )
		{
		++iBatchCount;
		
		if ( aUid == KFailReplaceUid )
			{
			iBatchResults.Append( KErrNotFound );
			}
		else if ( aUid == KFieldLevelReplace && !aFieldChange )
			{
			iBatchResults.Append( KErrNotFound );
			}
		else if ( aUid != KFieldLevelReplace && aFieldChange )
			{
			iBatchResults.Append( KErrNotFound );
			}
		else
			{
			iBatchResults.Append( KErrNone );
			}

		aResultCode = KErrNone;
		
		return;
		}

	// make replace to fail for testing purposes
	if ( aUid == KFailReplaceUid )
		{
		aResultCode = KErrNotFound;
		}
	else if ( aUid == KFieldLevelReplace && !aFieldChange )
		{
		aResultCode = KErrNotFound;
		}
	else if ( aUid != KFieldLevelReplace && aFieldChange )
		{
		aResultCode = KErrNotFound;
		}
	else
		{
		if ( aParent == KInvalidParent )
			{
			aResultCode = KErrPathNotFound;
			}
		else
			{
			aResultCode = KErrNone;
			}
		}				
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ReadItemL
// Reads data from item at Data Store. Item must be opened before this method can be called.
// This method is called until aBuffer is not used totally or method leaves with KErrEof.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ReadItemL( TDes8& aBuffer, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	for ( TInt i = 0; i < aBuffer.MaxSize(); i++ )
		{
		if ( iReadCount == 0 )
			{
			aBuffer.Append( 'X' );
			}
		else
			{
			aBuffer.Append( 'Y' );
			}
		}
		
	++iReadCount;
	aResultCode = KErrNone;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::WriteItemL
// Writes data to item to Data Provider. CreateItemL or ReplaceItemL method must be called before
// this method can be called. This method is called until all data to current item is written.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::WriteItemL( const TDesC8& aData, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	RFs fSession;
	RFile file;
	
	User::LeaveIfError( fSession.Connect() );
	TInt result = file.Open( fSession, KReceivedDataFile, EFileWrite|EFileShareAny );
	
	if ( result == KErrNotFound )
		{
		file.Create( fSession, KReceivedDataFile, EFileWrite|EFileShareAny );
		}

	TInt pos( 0 );
	file.Seek( ESeekEnd, pos );
	file.Write( KWriteSeparator );
	file.Write( aData );
			
	file.Close();
	fSession.Close();
	
	if ( iBatchMode )
		{
		++iBatchCount;
		iBatchResults.Append( KErrNone );
		}
		
	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitItemL
// After item is written to Data Provider it can be saved to the Data Store.
// This method can be called just after WriteItemL method.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitItemL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	if ( iBatchMode )
		{
		++iBatchCount;
		iBatchResults.Append( KErrNone );
		}

	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CloseItemL
// Closes opened item.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CloseItemL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::MoveItemL
// Moves item to new location.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::MoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	if ( iBatchMode )
		{
		++iBatchCount;
		
		if ( aUid == KFailMoveUid )
			{
			iBatchResults.Append( KErrNotFound );
			}
		else
			{
			iBatchResults.Append( KErrNone );
			}
			
		aResultCode = KErrNone;

		return;
		}
		
	// make move to fail for testing purposes
	if ( aUid == KFailMoveUid )
		{
		aResultCode = KErrNotFound;
		}
	else
		{
		if ( aNewParent == KInvalidParent )
			{
			aResultCode = KErrPathNotFound;
			}
		else
			{
			aResultCode = KErrNone;
			}
		}
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DeleteItemL
// Deletes one item at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::DeleteItemL( TSmlDbItemUid aUid, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	if ( iBatchMode )
		{
		++iBatchCount;
		
		if ( aUid == KFailDeleteUid )
			{
			iBatchResults.Append( KErrNotFound );
			}
		else
			{
			iBatchResults.Append( KErrNone );
			}

		aResultCode = KErrNone;
		
		return;
		}

	// make delete to fail for testing purposes
	if ( aUid == KFailDeleteUid )
		{
		aResultCode = KErrNotFound;
		}
	else
		{
		aResultCode = KErrNone;
		}
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SoftDeleteItemL
// Soft deletes one item at Data Store.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SoftDeleteItemL( TSmlDbItemUid /*aUid*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	if ( iBatchMode )
		{
		++iBatchCount;
		iBatchResults.Append( KErrNone );
		}

	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DeleteAllItemsL
// Deletes all items at Data Store permanently.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::DeleteAllItemsL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::HasSyncHistoryL
// Checks if the Data Store has sync history. If not then slow sync is proposed to Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C TBool CNSmlDSHostClient::HasSyncHistoryL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	return EFalse;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::AddedItemsL
// The Data Provider returns UIDs of items that are added after previous synchronization.
// If the Data Provider uses hierarchical synchronization then added folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::AddedItemsL( RNSmlDbItemModificationSet& /*aUidSet*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::DeletedItemsL
// The Data Provider returns UIDs of items that are deleted after previous synchronization.
// If the Data Provider uses hierarchical synchronization then deleted items must be placed
// first to UID set and folders after items (from leaves to root).
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::DeletedItemsL( RNSmlDbItemModificationSet& /*aUidSet*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::SoftDeleteItemsL
// The Data Provider returns UIDs of items that are soft deleted after previous synchronization.
// If the Data Provider uses hierarchical synchronization then soft deleted items must be placed
// first to UID set and folders after items (from leaves to root).
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::SoftDeleteItemsL( RNSmlDbItemModificationSet& /*aUidSet*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ModifiedItemsL
// The Data Provider returns UIDs of items that are modified after previous synchronization.
// If the Data Provider uses hierarchical synchronization then modified folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ModifiedItemsL( RNSmlDbItemModificationSet& /*aUidSet*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::MovedItemsL
// The Data Provider returns UIDs of items that are moved after previous synchronization.
// If the Data Provider uses hierarchical synchronization then moved folders must be placed
// first (from root to leaves) to UID set and finally items.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::MovedItemsL( RNSmlDbItemModificationSet& /*aUidSet*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode ) const
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::AllItemsL
// The Data Provider returns UIDs of items that are added, deleted, modified, softdeleted
// or moved after previous synchronization.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::AllItemsL( RNSmlDbItemModificationSet& aUidSet, const TSmlDataProviderId aId, const TDesC& aStoreName, TInt& aResultCode ) const
	{
	if ( aId == KAgenda.iUid )
		{
		if ( aStoreName == KNSmlUnicodeAgenda1LocalDbName )
			{
			TNSmlDbItemModification modification( 1, TNSmlDbItemModification::ENSmlDbItemAdd );
			aUidSet.AddItem ( modification );
			}
		}

	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::ResetChangeInfoL
// Reset change info from the Data Provider. The result of this method is that the Data Provider
// sends just ADD commands to Sync Partner.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::ResetChangeInfoL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}
		
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitChangeInfoL
// This method is called after some changes are synchronized to Sync Partner. If some changes
// were synchronized correctly then those UIDs are included to aItems.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitChangeInfoL( MSmlDataItemUidSet& /*aItems*/, const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostClient::CommitChangeInfoL
// This method is called after some changes are synchronized to Sync Partner. This method is used if
// all changes were synchronized correctly.
// ------------------------------------------------------------------------------------------------	
EXPORT_C void CNSmlDSHostClient::CommitChangeInfoL( const TSmlDataProviderId /*aId*/, const TDesC& /*aStoreName*/, TInt& aResultCode )
	{
	aResultCode = KErrNone;
	}

EXPORT_C TNSmlDPInformation* CNSmlDSHostClient::DataProviderInformationL( const TSmlDataProviderId /*aId*/, TInt& /*aResultCode*/ ) const
	{
	return NULL;
	}
	