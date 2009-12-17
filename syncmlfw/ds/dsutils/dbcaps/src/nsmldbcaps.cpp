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
* Description:  Sources
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include <badesca.h>

#include <nsmldebug.h>
#include "nsmldbcaps.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"
#include "smldevinftags.h"


// ------------------------------------------------------------------------------------------------
// CNSmlDbCaps
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDbCaps* CNSmlDbCaps::NewL()
	{
	//_DBG_FILE("CNSmlDbCaps::NewL(): begin");
	CNSmlDbCaps* self = CNSmlDbCaps::NewLC();
	CleanupStack::Pop(); // self
	//_DBG_FILE("CNSmlDbCaps::NewL(): end");
	return self;
	}

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDbCaps* CNSmlDbCaps::NewLC()
	{
	//_DBG_FILE("CNSmlDbCaps::NewLC(): begin");
	CNSmlDbCaps* self = new (ELeave) CNSmlDbCaps();
	CleanupStack::PushL(self);
	self->ConstructL();
	//_DBG_FILE("CNSmlDbCaps::NewLC(): end");
	return self;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
CNSmlDbCaps::CNSmlDbCaps()
	{
	//_DBG_FILE("CNSmlDbCaps::CNSmlDbCaps(): begin");
	//_DBG_FILE("CNSmlDbCaps::CNSmlDbCaps(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
void CNSmlDbCaps::ConstructL()
	{
	//_DBG_FILE("CNSmlDbCaps::ConstructL(): begin");
	iDatastore = new (ELeave) sml_devinf_datastore_s();
	//_DBG_FILE("CNSmlDbCaps::ConstructL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlDbCaps::~CNSmlDbCaps()
	{
	//_DBG_FILE("CNSmlDbCaps::~CNSmlDbCaps(): begin");
	delete iDatastore;
//	delete iCtCaps;
    delete iFilterCaps;
	//_DBG_FILE("CNSmlDbCaps::~CNSmlDbCaps(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C SmlDevInfDatastorePtr_t CNSmlDbCaps::DatastoreL() const
	{
	//_DBG_FILE("CNSmlDbCaps::Datastore(): begin");
	//_DBG_FILE("CNSmlDbCaps::Datastore(): end");
	SmlDevInfDatastorePtr_t temp = iDatastore;
	iDatastore = new (ELeave) sml_devinf_datastore_s();
	return temp;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C SmlDevInfCtCapListPtr_t CNSmlDbCaps::CtCaps() const
	{
	//_DBG_FILE("CNSmlDbCaps::CtCaps(): begin");
	//_DBG_FILE("CNSmlDbCaps::CtCaps(): end");
	SmlDevInfCtCapListPtr_t temp = iDatastore->ctcap;
	iDatastore->ctcap = 0;
	return temp;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C SmlDevInfFilterCapListPtr_t CNSmlDbCaps::FilterCaps() const
	{
	SmlDevInfFilterCapListPtr_t temp = iFilterCaps;
	iFilterCaps = 0;
	return temp;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDbCaps::SetRxPrefL( const TDesC8& aCtType, const TDesC8& aVerCt )
	{
	//_DBG_FILE("CNSmlDbCaps::SetRxPrefL(): begin");
	if( !iDatastore->rxpref )
		{
		iDatastore->rxpref = CreateXmitL(aCtType, aVerCt);
		}
	else
		{
		iDatastore->rxpref->cttype->SetDataL(aCtType);
		iDatastore->rxpref->verct->SetDataL(aVerCt);
		}
	//_DBG_FILE("CNSmlDbCaps::SetRxPrefL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDbCaps::AddRxL( const TDesC8& aCtType, const TDesC8& aVerCt )
	{
	//_DBG_FILE("CNSmlDbCaps::AddRxL(): begin");
	SmlDevInfXmitListPtr_t node = new (ELeave) SmlDevInfXmitList_t();
	CleanupStack::PushL(node);
	node->data = CreateXmitL(aCtType, aVerCt);
	GenericListAddL(&iDatastore->rx, node);
	CleanupStack::Pop(); // node
	//_DBG_FILE("CNSmlDbCaps::AddRxL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDbCaps::SetTxPrefL( const TDesC8& aCtType, const TDesC8& aVerCt )
	{
	//_DBG_FILE("CNSmlDbCaps::SetTxPrefL(): begin");
	if( !iDatastore->txpref )
		{
		iDatastore->txpref = CreateXmitL(aCtType, aVerCt);
		}
	else
		{
		iDatastore->txpref->cttype->SetDataL(aCtType);
		iDatastore->txpref->verct->SetDataL(aVerCt);
		}
	//_DBG_FILE("CNSmlDbCaps::SetTxPrefL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDbCaps::AddTxL( const TDesC8& aCtType, const TDesC8& aVerCt )
	{
	//_DBG_FILE("CNSmlDbCaps::AddTxL(): begin");
	SmlDevInfXmitListPtr_t node = new (ELeave) SmlDevInfXmitList_t();
	CleanupStack::PushL(node);
	node->data = CreateXmitL(aCtType, aVerCt);
	GenericListAddL(&iDatastore->tx, node);
	CleanupStack::Pop(); // node
	//_DBG_FILE("CNSmlDbCaps::AddTxL(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C void CNSmlDbCaps::SetSyncType( ENSmlSyncTypes aType )
	{
	//_DBG_FILE("CNSmlDbCaps::SetSyncType(): begin");
	if( aType == EAllTypes )
		{
		iSyncTypes = 0xffffffff;
		}
	else
		{
		iSyncTypes |= (1<<aType);
		}
	//_DBG_FILE("CNSmlDbCaps::SetSyncType(): end");
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C TBool CNSmlDbCaps::SupportsSyncType( ENSmlSyncTypes aType ) const
	{
	//_DBG_FILE("CNSmlDbCaps::SupportsSyncType(): begin");
	//_DBG_FILE("CNSmlDbCaps::SupportsSyncType(): end");
	return (((1<<aType)&iSyncTypes) != 0);
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlCtCapsHandler* CNSmlDbCaps::CreateHandlerL() const
	{
	//_DBG_FILE("CNSmlDbCaps::CreateHandlerL(): begin");
	//_DBG_FILE("CNSmlDbCaps::CreateHandlerL(): end");
	return new (ELeave) CNSmlCtCapsHandler(iDatastore->ctcap);
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlDbCaps::AddFilterRxL( const TDesC8& aCtType, const TDesC8& aVerCt )
    {
	SmlDevInfXmitListPtr_t filter = new (ELeave) SmlDevInfXmitList_t();
	CleanupStack::PushL(filter);
	filter->data = CreateXmitL(aCtType, aVerCt);
	GenericListAddL(&iDatastore->filterrx, filter);    
	CleanupStack::Pop(); // filter
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------

EXPORT_C CNSmlFilterCap* CNSmlDbCaps::AddFilterCapLC( const TDesC8& aCtType, const TDesC8& aVerCt )                                        
    {
    SmlDevInfFilterCapListPtr_t filtercap = new (ELeave) SmlDevInfFilterCapList_t();
    CleanupStack::PushL(filtercap);
    GenericListAddL(&iFilterCaps, filtercap);
    CleanupStack::Pop(); // filtercap

    filtercap->data = new (ELeave) SmlDevInfFilterCap_t();
    filtercap->data->cttype = new (ELeave) SmlPcdata_t();
    filtercap->data->cttype->SetDataL(aCtType);
    filtercap->data->verct = new (ELeave) SmlPcdata_t();
    filtercap->data->verct->SetDataL(aVerCt);

    CNSmlFilterCap* f = new (ELeave) CNSmlFilterCap(filtercap->data);
    CleanupStack::PushL(f);
    return f;
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlDbCaps::SetSupportHierarchicalSyncL()
    {
    if ( !iDatastore->supportHierarchicalSync )
        {
        iDatastore->supportHierarchicalSync = new (ELeave) SmlPcdata_t();
        }
    iDatastore->supportHierarchicalSync->SetDataL(TPtrC8());
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlDbCaps::SetSourceRefL( const TDesC8& aSourceRef )
    {
    if ( !iDatastore->sourceref )
        {
        iDatastore->sourceref = new (ELeave) SmlPcdata_t();
        }
	iDatastore->sourceref->SetDataL(aSourceRef);
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlDbCaps::SetDisplayNameL( const TDesC8& aDispName )
    {
    if ( !iDatastore->displayname )
        {
        iDatastore->displayname = new (ELeave) SmlPcdata_t();
        }
    iDatastore->displayname->SetDataL(aDispName);
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlDbCaps::SetMaxGuidSizeL( const TDesC8& aMaxGuid )
    {
    if ( !iDatastore->maxguidsize )
        {
        iDatastore->maxguidsize = new (ELeave) SmlPcdata_t();
        }
	iDatastore->maxguidsize->SetDataL(aMaxGuid);
    }

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlCtCap* CNSmlDbCaps::AddCtCapLC()
	{
	//_DBG_FILE("CNSmlDbCaps::AddCtCapL(): begin");
	SmlDevInfCtCapListPtr_t cpcapptr = new (ELeave) SmlDevInfCtCapList_t();
	CleanupStack::PushL(cpcapptr);
	GenericListAddL(&iDatastore->ctcap, cpcapptr);
	CleanupStack::Pop(); // cpcapptr
	cpcapptr->data = new (ELeave) SmlDevInfCtCap_t();
	CNSmlCtCap* c = new (ELeave) CNSmlCtCap(cpcapptr->data);
	CleanupStack::PushL(c);
	//_DBG_FILE("CNSmlDbCaps::AddCtCapL(): end");
	return c;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
SmlPcdataPtr_t CNSmlDbCaps::CreatePcdataL( const TDesC8& aData )
	{
	//_DBG_FILE("CNSmlDbCaps::CreatePcdataL(): begin");
	SmlPcdataPtr_t temp = new (ELeave) SmlPcdata_t();	
	CleanupStack::PushL(temp);
	temp->SetDataL(aData);
	CleanupStack::Pop(); // temp
	//_DBG_FILE("CNSmlDbCaps::CreatePcdataL(): end");
	return temp;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
SmlDevInfXmitPtr_t CNSmlDbCaps::CreateXmitL( const TDesC8& aCtType, const TDesC8& aVerCt )
	{
	//_DBG_FILE("CNSmlDbCaps::CreateXmitL(): begin");
	SmlDevInfXmitPtr_t temp = new (ELeave) SmlDevInfXmit_t();
	CleanupStack::PushL(temp);
	temp->cttype = CreatePcdataL(aCtType);
	temp->verct = CreatePcdataL(aVerCt);
	CleanupStack::Pop(); // temp
	//_DBG_FILE("CNSmlDbCaps::CreateXmitL(): end");
	return temp;
	}

// ------------------------------------------------------------------------------------------------
//
// ------------------------------------------------------------------------------------------------
EXPORT_C TInt CNSmlDbCaps::ConvertDeviceInfoL( sml_devinf_devinf_s* aDeviceInfo)
	{

	SmlDevInfCtCapListPtr_t target = new (ELeave) SmlDevInfCtCapList_t();
	CleanupStack::PushL(target);
	target =0;

	TPtrC8 ver;
	ver.Set(aDeviceInfo->verdtd->Data());

	if ( ver.FindC(_L8("1.2")) != KErrNone ) 
		{
		CleanupStack::PopAndDestroy(); // target
		return KErrNotSupported;
		} 

	aDeviceInfo->verdtd->SetDataL( _L8("1.1"));

	for ( SmlDevInfDatastoreListPtr_t datastorePtr = aDeviceInfo->datastore; datastorePtr && datastorePtr->data; datastorePtr = datastorePtr->next )
		{	
	for ( SmlDevInfCtCapListPtr_t ctcapPtr = datastorePtr->data->ctcap; ctcapPtr && ctcapPtr->data; ctcapPtr = ctcapPtr->next )
			{
			SmlDevInfCtCapListPtr_t ctcaps = new (ELeave) SmlDevInfCtCapList_t();
			CleanupStack::PushL( ctcaps);
			GenericListAddL(&target, ctcaps);
			CleanupStack::Pop(); // ctcaps
			ctcaps->data = new (ELeave) SmlDevInfCtCap_t();
			ctcaps->data->cttype = new (ELeave) SmlPcdata_t();
			ctcaps->data->cttype->SetDataL ( ctcapPtr->data->cttype->Data() );
			for ( SmlDevInfPropertyListPtr_t propPtr = ctcapPtr->data->property; propPtr && propPtr->data; propPtr = propPtr->next)
				{
				SmlDevInfCTDataPropListPtr_t props = new (ELeave) SmlDevInfCTDataPropList_t();
				CleanupStack::PushL( props );
				GenericListAddL(&ctcaps->data->prop, props);
				CleanupStack::Pop(); // props
				props->data = new (ELeave) SmlDevInfCTDataProp_t();
				props->data->prop = new (ELeave) SmlDevInfCTData_t();
				props->data->prop->name = new (ELeave) SmlPcdata_t();
				props->data->prop->name->SetDataL ( propPtr->data->propname->Data() );
				if ( propPtr->data->datatype )
					{
					props->data->prop->datatype = new (ELeave) SmlPcdata_t();
					props->data->prop->datatype->SetDataL ( propPtr->data->datatype->Data() );
					}
				if ( propPtr->data->displayname)
					{
					props->data->prop->dname = new (ELeave) SmlPcdata_t();
					props->data->prop->dname->SetDataL ( propPtr->data->displayname->Data() );
					}
				if ( propPtr->data->maxsize)
					{
					props->data->prop->size = new (ELeave) SmlPcdata_t();
					props->data->prop->size->SetDataL ( propPtr->data->maxsize->Data() );
					}
		for ( SmlPcdataListPtr_t valenumPtr = propPtr->data->valenum; valenumPtr && valenumPtr->data; valenumPtr = valenumPtr->next)
					{
					SmlPcdataListPtr_t valenum = new (ELeave) SmlPcdataList_t();
					CleanupStack::PushL(valenum);
					GenericListAddL(&props->data->prop->valenum, valenum);
					CleanupStack::Pop(); // valenum
					valenum->data=new (ELeave) SmlPcdata_t();
					valenum->data->SetDataL ( valenumPtr->data->Data());
					}
			for ( SmlDevInfPropParamListPtr_t paramPtr = propPtr->data->propparam; paramPtr && paramPtr->data; paramPtr = paramPtr->next )
					{
					SmlDevInfCTDataListPtr_t param = new (ELeave) SmlDevInfCTDataList_t();
					CleanupStack::PushL(param);
					GenericListAddL(&props->data->param, param);
					CleanupStack::Pop(); // param
					param->data = new (ELeave) SmlDevInfCTData_t();
					param->data->name = new (ELeave) SmlPcdata_t();
					param->data->name->SetDataL (paramPtr->data->paramname->Data() );
					if ( paramPtr->data->displayname )
						{
						param->data->dname = new (ELeave) SmlPcdata_t();
						param->data->dname->SetDataL (paramPtr->data->displayname->Data() );
						}
					if ( paramPtr->data->datatype)
						{
						param->data->datatype = new (ELeave) SmlPcdata_t();
						param->data->datatype->SetDataL ( paramPtr->data->datatype->Data() );
						}
					for (SmlPcdataListPtr_t valenumPtr = paramPtr->data->valenum; valenumPtr && valenumPtr->data; valenumPtr = valenumPtr->next )
						{
						SmlPcdataListPtr_t valenum = new (ELeave) SmlPcdataList_t();
						CleanupStack::PushL(valenum);
						GenericListAddL(&param->data->valenum, valenum);
						CleanupStack::Pop(); // valenum
						valenum->data = new (ELeave) SmlPcdata_t();
						valenum->data->SetDataL ( valenumPtr->data->Data() );
						}
					}
				}
			}
		}
	aDeviceInfo->ctcap = target;

	CleanupStack::PopAndDestroy(); // target

	return KErrNone;
	};
	
// ------------------------------------------------------------------------------------------------
// CNSmlDbCaps::SetMaxItemsL
// Sets maxitems element. Same as: iDatastore->dsmem->maxid
// ------------------------------------------------------------------------------------------------
void CNSmlDbCaps::SetMaxItemsL( const TDesC8& aMaxItems )
	{
	if ( !iDatastore->dsmem )
		{
		iDatastore->dsmem = new (ELeave) SmlDevInfDSMem_t();
		}
	if ( !iDatastore->dsmem->maxid )
        {
        iDatastore->dsmem->maxid = new (ELeave) SmlPcdata_t();
        }
	iDatastore->dsmem->maxid->SetDataL( aMaxItems );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDbCaps::SetMaxSizeL
// sets maxsize element. Same as: iDatastore->dsmem->maxmem
// ------------------------------------------------------------------------------------------------
void CNSmlDbCaps::SetMaxSizeL( const TDesC8& aMaxSize )
	{
	if ( !iDatastore->dsmem )
		{
		iDatastore->dsmem = new (ELeave) SmlDevInfDSMem_t();
		}
	if ( !iDatastore->dsmem->maxmem )
        {
        iDatastore->dsmem->maxmem = new (ELeave) SmlPcdata_t();
        }
	iDatastore->dsmem->maxmem->SetDataL( aMaxSize );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDbCaps::Datastore
// Returns datastore structure.
// ------------------------------------------------------------------------------------------------
const sml_devinf_datastore_s* CNSmlDbCaps::Datastore() const
	{
	return iDatastore;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDbCaps::FilterCapsList
// Returns pointer to filtercap structure.
// ------------------------------------------------------------------------------------------------
const sml_devinf_filtercaplist_s* CNSmlDbCaps::FilterCapsList() const
	{
	return iFilterCaps;
	}

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlFilterCapsHandler* CNSmlDbCaps::CreateFilterCapsHandlerL() const
	{
	//_DBG_FILE("CNSmlDbCaps::CreateHandlerL(): begin");
	//_DBG_FILE("CNSmlDbCaps::CreateHandlerL(): end");
	return new (ELeave) CNSmlFilterCapsHandler(iFilterCaps);
	}

// End of File
