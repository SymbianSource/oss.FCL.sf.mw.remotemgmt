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
* Description:  for dbcaps streaming and converting to CSmlDataStoreFormat.
*
*/


#include <stringpool.h>
#include <s32strm.h>

#include <nsmlunicodeconverter.h>
#include "nsmldbcaps.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"
#include "smldevinftags.h"
#include "NSmldbcapsSerializer.h"

// ------------------------------------------------------------------------------------------------
// Constants
// ------------------------------------------------------------------------------------------------
_LIT8( KNSmlFolderType, "application/vnd.omads-folder+xml" );
_LIT8( KNSmlFolderVersion, "1.2" );

const TInt KNSml32BitNumLength = 12;

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CNSmlFilterCapabilityData::NewLC()
// constructs CNSmlFilterCapabilityData
// ------------------------------------------------------------------------------------------------
TNSmlDbCapsSerializer::CNSmlFilterCapabilityData* TNSmlDbCapsSerializer::CNSmlFilterCapabilityData::NewLC()
	{
	CNSmlFilterCapabilityData* self = new ( ELeave ) CNSmlFilterCapabilityData();
	CleanupDeletePushL( self );
	return self;
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CNSmlFilterCapabilityData::~CNSmlFilterCapabilityData()
// C++ destructor
// ------------------------------------------------------------------------------------------------	
TNSmlDbCapsSerializer::CNSmlFilterCapabilityData::~CNSmlFilterCapabilityData()
	{
	iFilterCapabilities.ResetAndDestroy();
	iMimeVersion.Close();
	iMimeType.Close();
	
	for (TInt i = 0; i < iKeywordList.Count(); i++ )
		{
		iKeywordList[i].Close();
		}
	iKeywordList.Reset();
	
	for (TInt i = 0; i < iPropertyList.Count(); i++)
		{
		iPropertyList[i].Close();
		}
	iPropertyList.Reset();
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CNSmlFilterCapabilityData::MakeL()
// C++ destructor
// ------------------------------------------------------------------------------------------------		
void TNSmlDbCapsSerializer::CNSmlFilterCapabilityData::MakeL( 
	const RStringPool& aStringPool, const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr )
	{
	
	if ( &aFilterInfoArr == NULL )
		{
		return;
		}
	
	for( TInt i = 0 ; i < aFilterInfoArr.Count(); i++ )
		{
		const TNSmlFilterCapData& fcd = aFilterInfoArr[i];
		switch( fcd.iTag )
			{
			case TNSmlFilterCapData::ECtType:
				iMimeType = aStringPool.OpenFStringL( fcd.iValue );
				break;
			case TNSmlFilterCapData::EVerCt:
				iMimeVersion = aStringPool.OpenFStringL( fcd.iValue );
				break;
			case TNSmlFilterCapData::EFilterKeyword:
				{
				RString tmprstr( aStringPool.OpenStringL( fcd.iValue ) );
				CleanupClosePushL( tmprstr );
				iKeywordList.AppendL( tmprstr );
				CleanupStack::Pop(); //tmprstr
				}
				break;
			case TNSmlFilterCapData::EPropName:
				{
				RString tmprstr( aStringPool.OpenStringL( fcd.iValue ) );
				CleanupClosePushL( tmprstr );
				iPropertyList.AppendL( tmprstr );
				CleanupStack::Pop(); //tmprstr
				}
				break;
			default:
				User::Leave( KErrArgument );
			}
		}
	CSmlFilterCapability* fc = CSmlFilterCapability::NewLC();
	
	fc->SetMimeTypeL( iMimeType );
	fc->SetMimeVersionL( iMimeVersion );
	fc->SetPropertiesListL( iPropertyList );
	fc->SetKeyWordListL( iKeywordList );
	
	iFilterCapabilities.AppendL( fc );
	CleanupStack::Pop( fc );
	}
// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CSmlDataFieldFromDbCaps::NewLC()
// constructs CSmlDataField from parameters
// ------------------------------------------------------------------------------------------------
CSmlDataField* TNSmlDbCapsSerializer::CNSmlDataFieldFromDbCaps::NewLC(
														const RStringPool& aStringPool,
														const TPtrC8& aDisplayName, 
														const TPtrC8& aPropName,
														const TPtrC8& aDataType,
														const sml_pcdata_list_s* aValEnums )
	{
	CNSmlDataFieldFromDbCaps* self = new ( ELeave ) CNSmlDataFieldFromDbCaps();
	CleanupStack::PushL( self );
	
	//display name
	NSmlUnicodeConverter::HBufC16InUnicodeLC( aDisplayName, self->iDisplayName );
	CleanupStack::Pop( self->iDisplayName ); // self->iDisplayName
	
	//name
	self->iName = aStringPool.OpenStringL( aPropName );
	
	//data type
	self->iDataType = aStringPool.OpenStringL( aDataType );
	
	//value enums
	for ( ; aValEnums; aValEnums = aValEnums->next )
		{
		const SmlPcdataPtr_t val = aValEnums->data;
		if ( val )
			{
			RString tmprstr( aStringPool.OpenStringL( TNSmlDbCapsSerializer::SafePtr( val ) ) );
			CleanupClosePushL( tmprstr );
			self->iEnumValues.AppendL( tmprstr );
			CleanupStack::Pop(); // tmprstr
			}
		}
	
	return self;
	}
														
// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CSmlDataPropertyParamFromDbCaps::NewLC()
// constructs CSmlDataPropertyParam from parameters
// ------------------------------------------------------------------------------------------------
CSmlDataPropertyParam* TNSmlDbCapsSerializer::CNSmlDataPropertyParamFromDbCaps::NewLC(
														const RStringPool& aStringPool,
														const sml_devinf_propparam_s* aDipp )
	{
	CNSmlDataPropertyParamFromDbCaps* self = new ( ELeave ) CNSmlDataPropertyParamFromDbCaps();
	CleanupStack::PushL( self );
	
	//diplay name, property name, datatype, value enums -> iField
	self->iField = CNSmlDataFieldFromDbCaps::NewLC( aStringPool, 
		TNSmlDbCapsSerializer::SafePtr( aDipp->displayname ), 
		TNSmlDbCapsSerializer::SafePtr( aDipp->paramname ),
		TNSmlDbCapsSerializer::SafePtr( aDipp->datatype ), 
		aDipp->valenum );
		
	CleanupStack::Pop( self->iField ); // self->iField

	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CSmlDataPropertyFromDbCaps::NewLC()
// constructs CSmlDataProperty from parameters
// ------------------------------------------------------------------------------------------------
CSmlDataProperty* TNSmlDbCapsSerializer::CNSmlDataPropertyFromDbCaps::NewLC(
														const RStringPool& aStringPool,
														const sml_devinf_property_s* aDip )
	{
	CNSmlDataPropertyFromDbCaps* self = new ( ELeave ) CNSmlDataPropertyFromDbCaps();
	CleanupStack::PushL( self );
	
	//max occur
	if( aDip->maxoccur )
		{
		TLex8 lex( aDip->maxoccur->Data() );
		User::LeaveIfError( lex.Val( self->iMaxOccur, EDecimal ) );
		self->iFlags |= KSmlDataProperty_HasMaxOccur;
		}
	
	//max size
	if ( aDip->maxsize )
		{
		TLex8 lex( aDip->maxsize->Data() );
		User::LeaveIfError( lex.Val( self->iMaxSize, EDecimal ) );
		self->iFlags |= KSmlDataProperty_HasMaxSize;
		}
	
	//options
	if ( aDip->notruncate )
		{
		self->iFlags |= KSmlDataProperty_NoTruncate;
		}
	
	//diplay name, property name, datatype, value enums -> iField
	self->iField = CNSmlDataFieldFromDbCaps::NewLC( aStringPool, 
		TNSmlDbCapsSerializer::SafePtr( aDip->displayname ), 
		TNSmlDbCapsSerializer::SafePtr( aDip->propname ),
		TNSmlDbCapsSerializer::SafePtr( aDip->datatype ), 
		aDip->valenum );
		
	CleanupStack::Pop( self->iField ); // self->iField
		
	SmlDevInfPropParamListPtr_t dippl = aDip->propparam;

	for ( ; dippl; dippl = dippl->next )
		{
		SmlDevInfPropParamPtr_t dipp = dippl->data;
		if ( dipp )
			{
			CSmlDataPropertyParam* temppoint = CNSmlDataPropertyParamFromDbCaps::NewLC( aStringPool, dipp );
			self->iParams.AppendL( temppoint );
			CleanupStack::Pop(); // temppoint
			}
		}
	
	return self;
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CSmlFilterCapabilityFromDbCaps::NewLC()
// constructs CSmlFilterCapability from parameters
// ------------------------------------------------------------------------------------------------
CSmlFilterCapability* TNSmlDbCapsSerializer::CNSmlFilterCapabilityFromDbCaps::NewLC( 
														const RStringPool& aStringPool, 
														const sml_devinf_filtercap_s* aFtCap )
	{
	CNSmlFilterCapabilityFromDbCaps* self = new ( ELeave ) CNSmlFilterCapabilityFromDbCaps();
	CleanupStack::PushL( self );
	
	//mime type
	self->iMimeType = aStringPool.OpenFStringL( TNSmlDbCapsSerializer::SafePtr( aFtCap->cttype ) );
	
	//mime version
	self->iMimeVersion = aStringPool.OpenFStringL( TNSmlDbCapsSerializer::SafePtr( aFtCap->verct ) );
	
	//keywords
	SmlPcdataListPtr_t keys = aFtCap->filterkeyword;
	for( ; keys; keys = keys->next )
		{
		if ( keys->data )
			{
			const TDesC8& key = keys->data->Data();
			RString rkey( aStringPool.OpenStringL( key ) );
			CleanupClosePushL( rkey );
			self->iKeywordList.AppendL( rkey );
			CleanupStack::Pop(); // rkey
			}
		}
	
	//properties
	SmlPcdataListPtr_t props = aFtCap->propname;
	for( ; props; props = props->next )
		{
		if ( props->data )
			{
			const TDesC8& prop = props->data->Data();
			RString rprop( aStringPool.OpenStringL( prop ) );
			CleanupClosePushL( rprop );
			self->iPropertyList.AppendL( rprop );
			CleanupStack::Pop(); // rkey
			}
		}
	
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CSmlMimeFormatFromDbCaps::NewLC()
// constructs CSmlMimeFormat from parameters
// ------------------------------------------------------------------------------------------------
CSmlMimeFormat* TNSmlDbCapsSerializer::CNSmlMimeFormatFromDbCaps::NewLC( 
														const RStringPool& aStringPool, 
														const sml_devinf_ctcap_s& aDic )
	{
	CNSmlMimeFormatFromDbCaps* self = new ( ELeave ) CNSmlMimeFormatFromDbCaps();
	CleanupStack::PushL( self );
	
	//mime type
	self->iMimeType = aStringPool.OpenFStringL( TNSmlDbCapsSerializer::SafePtr( aDic.cttype ) );
	
	//mime version
	self->iMimeVersion = aStringPool.OpenFStringL( TNSmlDbCapsSerializer::SafePtr( aDic.verct ) );
	
	//field level
	self->iFieldLevel = EFalse;
	if ( aDic.fieldlevel )
		{
		self->iFieldLevel = ETrue;
		}
		
	//properties
	SmlDevInfPropertyListPtr_t dipl = aDic.property;
	for ( ; dipl; dipl = dipl->next )
		{
		const SmlDevInfPropertyPtr_t dip = dipl->data;
		if ( dip )
			{
			CSmlDataProperty *temppoint = CNSmlDataPropertyFromDbCaps::NewLC( aStringPool, dip );
			self->iProperties.AppendL( temppoint );
			CleanupStack::Pop(); // temppoint
			}
		}
		
	return self;
	}
	
// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::CSmlDataStoreFormatFromDbCaps::NewLC()
// constructs CSmlDataStoreFormat from parameters
// ------------------------------------------------------------------------------------------------
CSmlDataStoreFormat* TNSmlDbCapsSerializer::CNSmlDataStoreFormatFromDbCaps::NewLC(
														const RStringPool& aStringPool,
														const CNSmlDbCaps& aDbCaps )
	{
	CNSmlDataStoreFormatFromDbCaps* self = new ( ELeave ) CNSmlDataStoreFormatFromDbCaps();
	CleanupStack::PushL( self );
	const sml_devinf_datastore_s* dds = aDbCaps.Datastore();
	
	//display name
	if(dds)
	    {
        NSmlUnicodeConverter::HBufC16InUnicodeLC( 
                TNSmlDbCapsSerializer::SafePtr( dds->displayname ), self->iDisplayName );
        CleanupStack::Pop( self->iDisplayName ); // self->iDisplayName
	    }
	
	//sync types
	TSmlSyncTypeMask& synctype( self->iSyncTypeMask );
			
	if ( aDbCaps.SupportsSyncType( CNSmlDbCaps::ETwoWaySync ) )
		{
		synctype.SetSupported( ESmlTwoWay );
		}
	else
		{
		synctype.SetNotSupported( ESmlTwoWay );
		}
	
	if ( aDbCaps.SupportsSyncType( CNSmlDbCaps::EOneWaySyncFromServer ) )
		{
		synctype.SetSupported( ESmlOneWayFromServer );
		}
	else
		{
		synctype.SetNotSupported( ESmlOneWayFromServer );
		}
	
	if ( aDbCaps.SupportsSyncType( CNSmlDbCaps::EOneWaySyncFromClient ) )
		{
		synctype.SetSupported( ESmlOneWayFromClient );
		}
	else
		{
		synctype.SetNotSupported( ESmlOneWayFromClient );
		}
	
	if ( aDbCaps.SupportsSyncType( CNSmlDbCaps::ESlowTwoWaySync ) ) 
		{
		synctype.SetSupported( ESmlSlowSync );
		}
	else
		{
		synctype.SetNotSupported( ESmlSlowSync );
		}
	
	if ( aDbCaps.SupportsSyncType( CNSmlDbCaps::ERefreshSyncFromServer ) )
		{
		synctype.SetSupported( ESmlRefreshFromServer );	
		}
	else
		{
		synctype.SetNotSupported( ESmlRefreshFromServer );
		}
	
	if ( aDbCaps.SupportsSyncType( CNSmlDbCaps::ERefreshSyncFromClient ) ) 
		{
		synctype.SetSupported( ESmlRefreshFromClient );
		}
	else
		{
		synctype.SetNotSupported( ESmlRefreshFromClient );
		}
	if (dds)
        {
        if ( dds->dsmem )
            {
            //max size
            if ( dds->dsmem->maxmem )
                {
                TLex8 lex( dds->dsmem->maxmem->Data() );
                User::LeaveIfError( lex.Val( self->iMaxSize, EDecimal ) );
                self->iFlags |= KSmlDataStore_HasMaxSize;
                }
            
            //max items
            if ( dds->dsmem->maxid )
                {
                TLex8 lex( dds->dsmem->maxid->Data() );
                User::LeaveIfError( lex.Val( self->iMaxItems, EDecimal ) );
                self->iFlags |= KSmlDataStore_HasMaxItems;
                }
            }
        
        
        //options
        if ( dds->supportHierarchicalSync )
            {
            self->iFlags |= KSmlDataStore_Hierarchical;
            }
        }
	
	//filter caps
	const sml_devinf_filtercaplist_s* filterList = aDbCaps.FilterCapsList();
	
	for ( ; filterList; filterList = filterList->next )
		{
		const SmlDevInfFilterCapPtr_t ft = filterList->data;
		if (ft)
			{
			CSmlFilterCapability* fc = CNSmlFilterCapabilityFromDbCaps::NewLC( aStringPool, ft );
			self->iFilterCapabilities.AppendL( fc );
			CleanupStack::Pop(); // fc
			}
		}
	
	// CSmlMimeFormat
	RPointerArray<CSmlMimeFormat> mimeFormatArray;
	CleanupClosePushL( mimeFormatArray );
	// First search all mime types that server supports at receiving
	// RX-pref
	CSmlMimeFormat* tempFormatRX = CSmlMimeFormat::NewLC();
    if( dds )
        {
        const TPtrC8& ctTypeRX = TNSmlDbCapsSerializer::SafePtr( dds->rxpref->cttype );
        const TPtrC8& verCtRX = TNSmlDbCapsSerializer::SafePtr( dds->rxpref->verct );
                    
        if ( ctTypeRX.Compare( KNullDesC8 ) != 0 && verCtRX.Compare( KNullDesC8 ) != 0 )
            {
            RStringF mimeFormatRX = aStringPool.OpenFStringL( ctTypeRX );
            RStringF mimeVersionRX = aStringPool.OpenFStringL( verCtRX );
            tempFormatRX->SetMimeTypeL( mimeFormatRX );
            tempFormatRX->SetMimeVersionL( mimeVersionRX );
            mimeFormatArray.AppendL( tempFormatRX );
            CleanupStack::Pop(); // tempFormatRX
            }
        else
            {
            CleanupStack::PopAndDestroy(); // tempFormatRX
            }
        // RXs
        
            SmlDevInfXmitListPtr_t rx = dds->rx;
            for ( ; rx ; rx = rx->next )
                {
                CSmlMimeFormat* tempFormat = CSmlMimeFormat::NewLC();
                const TPtrC8& ctType = TNSmlDbCapsSerializer::SafePtr( rx->data->cttype );
                const TPtrC8& verCt = TNSmlDbCapsSerializer::SafePtr( rx->data->verct );
                if ( ctType.Compare( KNullDesC8 ) != 0 && verCt.Compare( KNullDesC8 ) != 0 )
                    {
                    RStringF mimeFormat = aStringPool.OpenFStringL( ctType );
                    RStringF mimeVersion = aStringPool.OpenFStringL( verCt );
                    tempFormat->SetMimeTypeL( mimeFormat );
                    tempFormat->SetMimeVersionL( mimeVersion );
                    mimeFormatArray.AppendL( tempFormat );
                    CleanupStack::Pop(); // tempFormat
                    }
                else
                    {
                    CleanupStack::PopAndDestroy(); // tempFormat
                    }
                }
            
        
        const sml_devinf_ctcaplist_s*  ctCapList = dds->ctcap;
        if ( ctCapList == 0 )
            {
            ctCapList = aDbCaps.CtCaps();
            }
        // Then add CTCaps to correct mime types
        if ( ctCapList != 0 )
            {    
            for (; ctCapList; ctCapList = ctCapList->next )
                {
                const SmlDevInfCtCapPtr_t ctCap = ctCapList->data;
                if ( ctCap->cttype->Data() == KNSmlFolderType )
                    {
                    SmlDevInfPropertyListPtr_t dipl = ctCap->property;
                    for ( ; dipl; dipl = dipl->next )
                        {
                        const SmlDevInfPropertyPtr_t dip = dipl->data;
                        if ( dip )
                            {
                            CSmlDataProperty *temppoint = CNSmlDataPropertyFromDbCaps::NewLC( aStringPool, dip );
                            self->iFolderProperties.AppendL( temppoint );
                            CleanupStack::Pop(); //  temppoint
                            }
                        }
                    }
                else
                    {
                    for ( TInt j(0); j < mimeFormatArray.Count(); j++ )
                        {
                        if ( mimeFormatArray[j]->MimeType().DesC().Compare( ctCap->cttype->Data() ) == 0 )
                            {
                            // Mime version is only in rx-pref or in rx so it must be copied to new mime format
                            CSmlMimeFormat* temppoint = CNSmlMimeFormatFromDbCaps::NewLC( aStringPool, *ctCap );
                            RStringF newMimeVersion = aStringPool.OpenFStringL( mimeFormatArray[j]->MimeVersion().DesC() );
                            temppoint->SetMimeVersionL( newMimeVersion );
                            delete mimeFormatArray[j];
                            mimeFormatArray[j] = NULL;
                            mimeFormatArray[j] = temppoint;
                            CleanupStack::Pop(); //  temppoint
                            }
                        }
                    }
                }
            }
        }
	self->SetMimeFormatsL( mimeFormatArray );
	mimeFormatArray.ResetAndDestroy();
	CleanupStack::PopAndDestroy(); // mimeFormatArray
	return self;
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::TNSmlDbCapsSerializer()
// C++ constructor
// ------------------------------------------------------------------------------------------------	
EXPORT_C TNSmlDbCapsSerializer::TNSmlDbCapsSerializer()
	{
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::InternalizeL( RReadStream& aDataStoreFormatStream )
// Internalizes from data store stream to dbcaps instance.
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDbCaps* TNSmlDbCapsSerializer::InternalizeL( RReadStream& aDataStoreFormatStream ) const
	{
	CNSmlDbCaps* dbcaps = CNSmlDbCaps::NewLC();
	
	RStringPool pool;
	pool.OpenL();
	CleanupClosePushL( pool );
	const CSmlDataStoreFormat* dsf = CSmlDataStoreFormat::NewLC( pool, aDataStoreFormatStream );
	SetFromL( *dsf, *dbcaps );
	
	CleanupStack::PopAndDestroy( 2 ); //dsf, pool
	CleanupStack::Pop(); // dbcaps
	return dbcaps;
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::ExternalizeL()
// externalizes from dbcaps to data store stream
// ------------------------------------------------------------------------------------------------
EXPORT_C void TNSmlDbCapsSerializer::ExternalizeL( const CNSmlDbCaps& aDbCaps, RWriteStream& aDataStoreFormatStream ) const
	{
	RStringPool pool;
	pool.OpenL();
	CleanupClosePushL( pool );
	CSmlDataStoreFormat* dsf = CNSmlDataStoreFormatFromDbCaps::NewLC( pool, aDbCaps );
	dsf->ExternalizeL( aDataStoreFormatStream );
	CleanupStack::PopAndDestroy( 2 ); //dsf, pool
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::ExternalizeL()
// externalizes from dbcaps to data store stream
// ------------------------------------------------------------------------------------------------
EXPORT_C void TNSmlDbCapsSerializer::ExternalizeL( const CNSmlDbCaps& aDbCaps, const CArrayFix<TNSmlFilterCapData>& aFilterInfoArr, RWriteStream& aDataStoreFormatStream ) const
	{
	RStringPool pool;
	pool.OpenL();
	CleanupClosePushL( pool );
	CSmlDataStoreFormat* dsf = CNSmlDataStoreFormatFromDbCaps::NewLC( pool, aDbCaps );
	CNSmlFilterCapabilityData* fcd = CNSmlFilterCapabilityData::NewLC();
	
	fcd->MakeL( pool, aFilterInfoArr );
	dsf->SetFilterCapabilitiesL( fcd->iFilterCapabilities );
	dsf->ExternalizeL( aDataStoreFormatStream );
	
	CleanupStack::PopAndDestroy( 3 ); //fcd, dsf, pool
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::SafePtr()
// checks for null before returning data pointer
// ------------------------------------------------------------------------------------------------	
TPtrC8 TNSmlDbCapsSerializer::SafePtr( sml_pcdata_s* aPcdata )
	{
	if ( aPcdata == NULL )
		{
		return TPtrC8( NULL, 0 );
		}
	else
		{
		return aPcdata->Data();
		}
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::SetFromL( const CSmlDataStoreFormat& aFrom, CNSmlDbCaps& aTo )
// copies and converts data from aFrom to aTo
// ------------------------------------------------------------------------------------------------
void TNSmlDbCapsSerializer::SetFromL( const CSmlDataStoreFormat& aFrom, CNSmlDbCaps& aTo ) const
	{
	const TDesC& dispName( aFrom.DisplayName() );
	HBufC8* dispName8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( dispName, dispName8 );
	aTo.SetDisplayNameL( *dispName8 ); // display name
	CleanupStack::PopAndDestroy(); // dispName8
	
	//max size
	_LIT8( KNSmlMaxGuidSize, "8" );
	aTo.SetMaxGuidSizeL( KNSmlMaxGuidSize );
	
	if ( aFrom.IsSupported( CSmlDataStoreFormat::EOptionHasMaxSize ) )
		{
		TBuf8<KNSml32BitNumLength> maxSizeDes;
		maxSizeDes.Num( aFrom.MaxSize() );
		aTo.SetMaxSizeL( maxSizeDes );
		}
	
	if ( aFrom.IsSupported( CSmlDataStoreFormat::EOptionHasMaxItems ) )
		{
		TBuf8<KNSml32BitNumLength> maxItemsDes;
		maxItemsDes.Num( aFrom.MaxItems() );
		aTo.SetMaxItemsL( maxItemsDes );
		}
	
	//sync types
	TSmlSyncTypeMask synctypes( aFrom.SyncTypes() );
	
	if ( synctypes.IsSupported( ESmlTwoWay ) )
		{
		aTo.SetSyncType( CNSmlDbCaps::ETwoWaySync );
		}
	if ( synctypes.IsSupported( ESmlOneWayFromServer ) )
		{
		aTo.SetSyncType( CNSmlDbCaps::EOneWaySyncFromServer );
		}
	if ( synctypes.IsSupported( ESmlOneWayFromClient ) )
		{
		aTo.SetSyncType( CNSmlDbCaps::EOneWaySyncFromClient );
		}
	if ( synctypes.IsSupported( ESmlSlowSync ) )
		{
		aTo.SetSyncType( CNSmlDbCaps::ESlowTwoWaySync );
		}
	if ( synctypes.IsSupported( ESmlRefreshFromServer ) )
		{
		aTo.SetSyncType( CNSmlDbCaps::ERefreshSyncFromServer );
		}
	if ( synctypes.IsSupported( ESmlRefreshFromClient ) )
		{
		aTo.SetSyncType( CNSmlDbCaps::ERefreshSyncFromClient );
		}
	
	//options
	if ( aFrom.IsSupported( CSmlDataStoreFormat::EOptionHierarchial ) )
		{
		aTo.SetSupportHierarchicalSyncL();
		}		
	
	//CNSmlCtCap / CSmlMimeFormat
	TInt rxPref( aFrom.MimeFormatRxPref() );
	TInt txPref( aFrom.MimeFormatTxPref() );
	
	for( TInt i = 0; i < aFrom.MimeFormatCount(); i++ )
		{
		const CSmlMimeFormat& mf = aFrom.MimeFormat( i );
		CNSmlCtCap* ctcap = aTo.AddCtCapLC();
		SetFromL( mf, *ctcap );
		CleanupStack::PopAndDestroy(); // ctcap
		
		const TDesC8& ctType = mf.MimeType().DesC();
		const TDesC8& verCt = mf.MimeVersion().DesC();
		
		if ( i == rxPref && i == txPref )
			{
			aTo.SetRxPrefL( ctType, verCt );
			aTo.SetTxPrefL( ctType, verCt );
			}
		else if ( i == txPref )
			{
			aTo.SetTxPrefL( ctType, verCt );
			aTo.AddRxL( ctType, verCt );
			}
		else if ( i == rxPref )
			{
			aTo.SetRxPrefL( ctType, verCt );
			aTo.AddTxL( ctType, verCt );
			}
		else
			{
			aTo.AddRxL( ctType, verCt );
			aTo.AddTxL( ctType, verCt );
			}
		}
	
	// CNSmlCtCap / CSmlDataProperty
	// folder properties
	if ( aFrom.FolderPropertyCount() > 0 )
		{
		aTo.AddRxL( KNSmlFolderType, KNSmlFolderVersion );
		aTo.AddTxL( KNSmlFolderType, KNSmlFolderVersion );
			
		CNSmlCtCap* ctcap = aTo.AddCtCapLC();
		ctcap->SetCtTypeL( KNSmlFolderType );
		ctcap->SetVerCtL( KNSmlFolderVersion );
		
		for( TInt i = 0; i < aFrom.FolderPropertyCount(); i++ )
			{
			const CSmlDataProperty& fp = aFrom.FolderProperty( i );
			
			CNSmlDevInfProp* dip = ctcap->AddDevInfPropLC( fp.Field().Name().DesC() );
			SetFromL( fp, *dip );
			CleanupStack::PopAndDestroy(); // dip
			}
		CleanupStack::PopAndDestroy(); // ctcap
		}
	
	// CNSmlFilterCap / CSmlFilterCapability
	for( TInt i = 0; i < aFrom.FilterCapabilityCount(); i++ )
		{
		const CSmlFilterCapability& fc = aFrom.FilterCapability( i );
		const TDesC8& ctType = fc.MimeType().DesC();
		const TDesC8& verCt = fc.MimeVersion().DesC();
		CNSmlFilterCap* fcSet = aTo.AddFilterCapLC( ctType, verCt );
		SetFromL( fc, *fcSet );
		CleanupStack::PopAndDestroy(); // fcSet
		
		aTo.AddFilterRxL( ctType, verCt );
		}	
	}
	
// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::SetFromL( const CSmlDataPropertyParam& aFrom, CNSmlPropParam& aTo )
// copies and converts data from aFrom to aTo
// ------------------------------------------------------------------------------------------------
void TNSmlDbCapsSerializer::SetFromL( const CSmlDataPropertyParam& aFrom, CNSmlPropParam& aTo ) const
	{
	const CSmlDataField& field = aFrom.Field();
	
	const TDesC& dispName( field.DisplayName() );
	HBufC8* dispName8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( dispName, dispName8 );
	aTo.SetDisplayNameL( *dispName8 ); // display name
	CleanupStack::PopAndDestroy(); // dispName8
	
	//data type
	aTo.SetDataTypeL( field.DataType().DesC() );
	
	//enum values
	for ( TInt i(0); i < field.EnumValueCount(); i++ )
		{
		aTo.AddValEnumL( field.EnumValue( i ).DesC() );
		}
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::SetFromL( const CSmlDataProperty& aFrom, CNSmlDevInfProp& aTo )
// copies and converts data from aFrom to aTo
// ------------------------------------------------------------------------------------------------
EXPORT_C void TNSmlDbCapsSerializer::SetFromL( const CSmlDataProperty& aFrom, CNSmlDevInfProp& aTo ) const
	{
	const CSmlDataField& field = aFrom.Field();
	TBuf8<KNSml32BitNumLength> tdes;
	
	//max occur
	if ( aFrom.MaxOccur() != 0 )
		{
		tdes.Num( aFrom.MaxOccur() );
		aTo.SetMaxOccurL( tdes );
		}
	
	//max size
	if ( aFrom.MaxSize() != 0 )
		{
		tdes.Num( aFrom.MaxSize() );
		aTo.SetMaxSizeL( tdes );
		}
	
	//display name
	const TDesC& dispName( field.DisplayName() );
	HBufC8* dispName8 = NULL;
	NSmlUnicodeConverter::HBufC8InUTF8LC( dispName, dispName8 );
	aTo.SetDisplayNameL( *dispName8 ); // display name
	CleanupStack::PopAndDestroy(); // dispName8
	
	//data type
	aTo.SetDataTypeL( field.DataType().DesC() );
	
	//options
	if ( aFrom.IsSupported( CSmlDataProperty::EOptionNoTruncate ) )
		{
		aTo.SetNoTruncateL();		
		}
	
	//param values
	for ( TInt i(0) ; i < aFrom.ParamCount(); i++ )
		{
		const CSmlDataPropertyParam& dpp = aFrom.Param(i);
		CNSmlPropParam* pp = aTo.AddParamLC( dpp.Field().Name().DesC() );
		SetFromL( dpp, *pp );
		CleanupStack::PopAndDestroy(); // pp
		}
		
	//enum values
	for ( TInt i(0) ; i < field.EnumValueCount(); i++ )
		{
		aTo.AddValEnumL( field.EnumValue( i ).DesC() );
		}
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::SetFromL( const CSmlMimeFormat& aFrom, CNSmlCtCap& aTo )
// copies and converts data from aFrom to aTo
// ------------------------------------------------------------------------------------------------
void TNSmlDbCapsSerializer::SetFromL( const CSmlMimeFormat& aFrom, CNSmlCtCap& aTo ) const
	{
	//mime type
	aTo.SetCtTypeL( aFrom.MimeType().DesC() );
	
	//mime version
	aTo.SetVerCtL( aFrom.MimeVersion().DesC() );
	
	//field level
	if ( aFrom.FieldLevel() )
		{
		aTo.SetFieldLevelL();
		}
		
	//properties
	for ( TInt i(0);i < aFrom.PropertyCount(); i++ )
		{
		const CSmlDataProperty& dp = aFrom.Property( i );
		CNSmlDevInfProp* dip = aTo.AddDevInfPropLC( dp.Field().Name().DesC() );
		SetFromL( dp, *dip );
		CleanupStack::PopAndDestroy(); // dip
		}
	}
// ------------------------------------------------------------------------------------------------
// TNSmlDbCapsSerializer::SetFromL( const CSmlFilterCapability& aFrom, CNSmlFilterCap& aTo )
// copies and converts data from aFrom to aTo
// ------------------------------------------------------------------------------------------------
void TNSmlDbCapsSerializer::SetFromL( const CSmlFilterCapability& aFrom, CNSmlFilterCap& aTo ) const
	{
	//keywords
	for ( TInt i = 0; i < aFrom.KeywordCount(); i++ )
		{
		aTo.AddFilterKeywordL( aFrom.Keyword(i).DesC() );
		}
	
	//properties
	for ( TInt i = 0; i < aFrom.PropertyCount(); i++ )
		{
		aTo.AddPropNameL( aFrom.PropertyName(i).DesC() );
		}
	}
	
// End of File
