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
* Description:  Device information command struct implementations.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------

#include "smldtd.h"
#include "smldevinfdtd.h"
#include "smldevinftags.h"
#include "WBXMLDevInfParser.h"
#include "WBXMLParserError.h"

// ------------------------------------------------------------------------------------------------
// External functions
// ------------------------------------------------------------------------------------------------
extern CXMLElement* AddPCDataL( SmlPcdataListPtr_t* aList );

// ------------------------------------------------------------------------------------------------
// Macros
// ------------------------------------------------------------------------------------------------
#define CHECK_NO_DUPLICATE(a) if( a ) { User::Leave(KErrParserErrorInvalidDocument); }

// ------------------------------------------------------------------------------------------------
// Helper functions
// ------------------------------------------------------------------------------------------------
CXMLElement* AddCTParamL(  SmlDevInfCTDataListPtr_t* aList )
	{
	SmlDevInfCTDataListPtr_t itemL = new (ELeave) SmlDevInfCTDataList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfCTData_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddCTPropL( SmlDevInfCTDataPropListPtr_t* aList )
	{
	SmlDevInfCTDataPropListPtr_t itemL = new (ELeave) SmlDevInfCTDataPropList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfCTDataProp_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddPropertyL( SmlDevInfPropertyListPtr_t* aList )
	{
	SmlDevInfPropertyListPtr_t itemL = new (ELeave) SmlDevInfPropertyList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfProperty_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// 1.2 CHANGES: new element PropParam
// ------------------------------------------------------------------------------------------------
CXMLElement* AddPropParamL( SmlDevInfPropParamListPtr_t* aList )
	{
	SmlDevInfPropParamListPtr_t itemL = new (ELeave) SmlDevInfPropParamList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfPropParam_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}
// Changes end

// ------------------------------------------------------------------------------------------------
CXMLElement* AddXmitL( SmlDevInfXmitListPtr_t* aList )
	{
	SmlDevInfXmitListPtr_t itemL = new (ELeave) SmlDevInfXmitList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfXmit_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// 1.2 CHANGES: new element FilterCap
// ------------------------------------------------------------------------------------------------
CXMLElement* AddFilterCapL( SmlDevInfFilterCapListPtr_t* aList)
	{
	SmlDevInfFilterCapListPtr_t itemL = new (ELeave) SmlDevInfFilterCapList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfFilterCap_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); //itemL
	return itemL->data;
	}
// Changes end

// ------------------------------------------------------------------------------------------------
CXMLElement* AddCtCapL( SmlDevInfCtCapListPtr_t* aList, CXMLElement* aParent = 0 )
	{
	SmlDevInfCtCapListPtr_t itemL = new (ELeave) SmlDevInfCtCapList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfCtCap_t(aParent);
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddDataStoreL( SmlDevInfDatastoreListPtr_t* aList )
	{
	SmlDevInfDatastoreListPtr_t itemL = new (ELeave) SmlDevInfDatastoreList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfDatastore_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* AddExtL( SmlDevInfExtListPtr_t* aList )
	{
	SmlDevInfExtListPtr_t itemL = new (ELeave) SmlDevInfExtList_t();
	CleanupStack::PushL(itemL);
	itemL->data = new (ELeave) SmlDevInfExt_t();
	GenericListAddL(aList, itemL);
	CleanupStack::Pop(); // itemL
	return itemL->data;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_xmit_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_xmit_s::sml_devinf_xmit_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_xmit_s::~sml_devinf_xmit_s()
	{
	delete cttype;
	delete verct;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_xmit_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
	{ 
		case EDevCTType:
			CHECK_NO_DUPLICATE(cttype);
			cttype = new (ELeave) SmlPcdata_t();
			return cttype;
		
		case EDevVerCT:
			CHECK_NO_DUPLICATE(verct);
			verct = new (ELeave) SmlPcdata_t();
			return verct;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_xmitlist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_xmitlist_s::sml_devinf_xmitlist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_xmitlist_s::~sml_devinf_xmitlist_s()
	{
	delete data;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_ext_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ext_s::sml_devinf_ext_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ext_s::~sml_devinf_ext_s()
	{
    delete xnam;     
    GenericListDelete( xval );
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_ext_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevXNam:
			CHECK_NO_DUPLICATE(xnam);
			xnam = new (ELeave) SmlPcdata_t();
			return xnam;

		case EDevXVal:
			return AddPCDataL(&xval);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_extlist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_extlist_s::sml_devinf_extlist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_extlist_s::~sml_devinf_extlist_s()
	{
	delete data;	
	}

// 1.2 CHANGES: New element FilterCap
// ------------------------------------------------------------------------------------------------
// sml_devinf_filtercap_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_filtercap_s::sml_devinf_filtercap_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_filtercap_s::~sml_devinf_filtercap_s()
	{
    delete cttype;
    delete verct;
	
	GenericListDelete( filterkeyword ); /* optional */
	GenericListDelete( propname ); /* optional */
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_filtercap_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
{

	switch( aTag )
		{
		case EDevCTType:
			CHECK_NO_DUPLICATE(cttype);
			cttype = new (ELeave) SmlPcdata_t();
			return cttype;

		case EDevVerCT:
			CHECK_NO_DUPLICATE(verct);
			verct = new (ELeave) SmlPcdata_t();
			return verct;

		case EDevFilterKeyword:
			return AddPCDataL(&filterkeyword);

		case EDevPropName:
			return AddPCDataL(&propname);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
}

// ------------------------------------------------------------------------------------------------
// sml_devinf_filtercaplist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_filtercaplist_s::sml_devinf_filtercaplist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_filtercaplist_s::~sml_devinf_filtercaplist_s()
	{
	delete data;	
	}
// Changes end


// ------------------------------------------------------------------------------------------------
// sml_devinf_synccap_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_synccap_s::sml_devinf_synccap_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_synccap_s::~sml_devinf_synccap_s()
	{    
    GenericListDelete( synctype );
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_synccap_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevSyncType:
			return AddPCDataL(&synctype);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_dsmem_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_dsmem_s::sml_devinf_dsmem_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_dsmem_s::~sml_devinf_dsmem_s()
	{
    delete maxmem; /* optional */
    delete maxid;  /* optional */
	delete shared; /* optional */
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_dsmem_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevSharedMem:
			CHECK_NO_DUPLICATE(shared);
			shared = new (ELeave) SmlPcdata_t();
			return shared;
		
		case EDevMaxMem:
			CHECK_NO_DUPLICATE(maxmem);
			maxmem = new (ELeave) SmlPcdata_t();
			return maxmem;

		case EDevMaxID:
			CHECK_NO_DUPLICATE(maxid);
			maxid = new (ELeave) SmlPcdata_t();
			return maxid;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// 1.2 CHANGES: new elements PropParam and Property
// ------------------------------------------------------------------------------------------------
// sml_devinf_propparam_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_propparam_s::sml_devinf_propparam_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_propparam_s::~sml_devinf_propparam_s()
	{
	delete datatype;
	delete displayname;
	delete paramname;	
	GenericListDelete( valenum );
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_propparam_s::BeginElementL(TUint8 aTag, const TXMLElementParams& /* aParams*/ )
	{
	switch (aTag)
		{
		case EDevParamName:
			CHECK_NO_DUPLICATE(paramname);
			paramname = new (ELeave) SmlPcdata_t();
			return paramname;

		case EDevDataType:
			CHECK_NO_DUPLICATE(datatype);
			datatype = new (ELeave) SmlPcdata_t();
			return datatype;

		case EDevValEnum:
			return AddPCDataL(&valenum);

		case EDevDisplayName:
			CHECK_NO_DUPLICATE(displayname);
			displayname = new (ELeave) SmlPcdata_t();
			return displayname;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_propparamlist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_propparamlist_s::sml_devinf_propparamlist_s()
	{
	};

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_propparamlist_s::~sml_devinf_propparamlist_s()
	{	
	delete data;	
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_property_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_property_s::sml_devinf_property_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_property_s::~sml_devinf_property_s()
	{
    delete datatype;
	delete displayname;
	delete maxoccur;
	delete maxsize;
	delete propname;	
	GenericListDelete( propparam );	
	GenericListDelete( valenum );	
	delete notruncate;	
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_property_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevPropName:
			CHECK_NO_DUPLICATE(propname);
			propname = new (ELeave) SmlPcdata_t();
			return propname;

		case EDevDataType:
			CHECK_NO_DUPLICATE(datatype);
			datatype = new (ELeave) SmlPcdata_t();
			return datatype;
			
		case EDevMaxOccur:
			CHECK_NO_DUPLICATE(maxoccur);
			maxoccur = new (ELeave) SmlPcdata_t();
			return maxoccur;

		case EDevMaxSize:
			CHECK_NO_DUPLICATE(maxsize);
			maxsize = new (ELeave) SmlPcdata_t();
			return maxsize;

		case EDevNoTruncate:
			CHECK_NO_DUPLICATE(notruncate);
			notruncate = new(ELeave) SmlPcdata_t();
			return notruncate;
						
		case EDevValEnum:
			return AddPCDataL(&valenum);
			
		case EDevDisplayName:
			CHECK_NO_DUPLICATE(displayname);
			displayname = new (ELeave) SmlPcdata_t();
			return displayname;

		case EDevPropParam:
			return AddPropParamL(&propparam);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_propertylist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_propertylist_s::sml_devinf_propertylist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_propertylist_s::~sml_devinf_propertylist_s()
	{
	delete data;	
	}
//Changes end

// ------------------------------------------------------------------------------------------------
// sml_devinf_ctdata_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdata_s::sml_devinf_ctdata_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdata_s::~sml_devinf_ctdata_s()
	{
    delete name; /* param OR prop name */
    delete dname; /* optional, display name */    
    GenericListDelete( valenum );
    delete datatype;
    delete size;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_ctdata_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevParamName:
			CHECK_NO_DUPLICATE(name);
			name = new (ELeave) SmlPcdata_t();
			return name;

		case EDevDisplayName:
			CHECK_NO_DUPLICATE(dname);
			dname = new (ELeave) SmlPcdata_t();
			return dname;

		case EDevValEnum:
			return AddPCDataL(&valenum);

		case EDevDataType:
			CHECK_NO_DUPLICATE(datatype);
			datatype = new (ELeave) SmlPcdata_t();
			return datatype;

		case EDevMaxSize:
			CHECK_NO_DUPLICATE(size);
			size = new (ELeave) SmlPcdata_t();
			return size;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
void sml_devinf_ctdata_s::SetDataL( const TDesC8& aData )
	{
	if( !name )
		{
		name = new (ELeave) SmlPcdata_t();
		name->SetDataL(aData);
		}
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_ctdatalist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdatalist_s::sml_devinf_ctdatalist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdatalist_s::~sml_devinf_ctdatalist_s()
	{
	delete data;	
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_ctdataprop_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdataprop_s::sml_devinf_ctdataprop_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdataprop_s::~sml_devinf_ctdataprop_s()
	{
    delete prop;    
    GenericListDelete( param );    
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_ctdataprop_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevDisplayName:
			CHECK_NO_DUPLICATE(prop->dname);
			prop->dname = new (ELeave) SmlPcdata_t();
			return prop->dname;

		case EDevValEnum:
			return AddPCDataL(&prop->valenum);

		case EDevDataType:
			CHECK_NO_DUPLICATE(prop->datatype);
			prop->datatype = new (ELeave) SmlPcdata_t();
			return prop->datatype;

		case EDevMaxSize:
			CHECK_NO_DUPLICATE(prop->size);
			prop->size = new (ELeave) SmlPcdata_t();
			return prop->size;

		case EDevParamName:
			return AddCTParamL(&param);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
void sml_devinf_ctdataprop_s::SetDataL( const TDesC8& aData )
	{
	if( !prop )
		{
		prop = new (ELeave) SmlDevInfCTData_t();
		}
	if( !prop->name )
		{
		prop->name = new (ELeave) SmlPcdata_t();
		prop->name->SetDataL(aData);
		}
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_ctdataproplist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdataproplist_s::sml_devinf_ctdataproplist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctdataproplist_s::~sml_devinf_ctdataproplist_s()
	{
	delete data;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_ctcap_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctcap_s::sml_devinf_ctcap_s()
	{
	}
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctcap_s::sml_devinf_ctcap_s( CXMLElement* aParent ) : iParent(aParent)
	{
	}
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctcap_s::~sml_devinf_ctcap_s()
	{
    delete cttype;	
	GenericListDelete( property );
	delete verct;
	delete fieldlevel;
	GenericListDelete( prop );
	//delete iParent;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_ctcap_s::GetPreviousElem() const
	{
	CXMLElement* e = 0;
	if( prevelem == SML_PROPNAME )
		{
		SmlDevInfCTDataPropListPtr_t l;
		for( l = prop; l && l->next; l = l->next )
			;
		if( l )
			{
			e = l->data;
			}
		}
	else
		{
		SmlDevInfCTDataPropListPtr_t l;
		for( l = prop; l && l->next; l = l->next )
			;
		if( l )
			{
			SmlDevInfCTDataListPtr_t l2;
			for( l2 = l->data->param; l2 && l2->next; l2 = l2->next )
				;
			if( l2 )
				{
				e = l2->data;
				}
			}
		}
	return e;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_ctcap_s::BeginElementL( TUint8 aTag, const TXMLElementParams& aParams )
	{
	switch( aTag )
		{
		case EDevCTType:
			if( !cttype )
				{
				CHECK_NO_DUPLICATE(cttype);
				cttype = new (ELeave) SmlPcdata_t();
				return cttype;
				}
			else if( iParent )
				{
				// Support for many CTTypes inside one CTCap - adds new CTCap element in the case of
				// multiple CTTypes within one CTCap. I.e. all following CTTypes will be added into new CTCap elements.
				CXMLElement::TAction action = this->EndElementL(aParams.Callbacks(), EDevCTCap);
				if( action != CXMLElement::ENone )
					{
					CXMLElement* e = aParams.CmdStack()->Pop();
					if( aParams.CleanupStack()->Top() == e )
						{
						aParams.CleanupStack()->Pop();
						}
					if( action == CXMLElement::EPopAndDestroy )
						{
						delete e;
						}
					}
				CXMLElement* e = iParent->BeginElementL(EDevCTCap, aParams.Callbacks());
				if( e )
					{
					aParams.CmdStack()->Push(e);
					return e->BeginElementL(aTag, aParams.Callbacks());
					}
				}				
				User::Leave(KErrGeneral);

// 1.2 CHANGES - version 1.1 and 1.2 support 
		case EDevVerCT:
			CHECK_NO_DUPLICATE(verct);
			verct = new (ELeave) SmlPcdata_t();
			return verct;

		case EDevFieldLevel:
			CHECK_NO_DUPLICATE(fieldlevel);
			fieldlevel = new (ELeave) SmlPcdata_t();
			return fieldlevel;
			
		case EDevProperty:
			return AddPropertyL(&property);
// changes end
			
		case EDevPropName:
			prevelem = SML_PROPNAME;
			return AddCTPropL(&prop);

		case EDevParamName:
		case EDevDisplayName:
		case EDevValEnum:
		case EDevDataType:
		case EDevMaxSize:
			{
			// paramname cannot be added into param -> switch prevelem
			if( (aTag == EDevParamName) && (prevelem == SML_PARAMNAME) )
				{
				prevelem = SML_PROPNAME;
				}

			CXMLElement* e = GetPreviousElem();
			
			if( aTag == EDevParamName )
				{
				prevelem = SML_PARAMNAME;
				}

			return e->BeginElementL(aTag, aParams.Callbacks());
			}

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_ctcaplist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctcaplist_s::sml_devinf_ctcaplist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_ctcaplist_s::~sml_devinf_ctcaplist_s()
	{
	delete data;	
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_datastore_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_datastore_s::sml_devinf_datastore_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_datastore_s::~sml_devinf_datastore_s()
	{
	delete sourceref;    
    delete displayname;   /* optional */	
    delete maxguidsize;   /* optional */
    delete rxpref;        
    GenericListDelete( rx ); /* optional */    
    delete txpref;            
    GenericListDelete( tx ); /* optional */    
    delete dsmem;         /* optional */
    delete synccap;
	// 1.2 CHANGES : new / moved elements
	delete supportHierarchicalSync;		
    GenericListDelete( ctcap );		    	
	GenericListDelete( filterrx ); /*optional*/ 	
	GenericListDelete( filtercap ); /*optional*/	
	// changes end	
	}

// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_datastore_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevSourceRef:
			CHECK_NO_DUPLICATE(sourceref);
			sourceref = new (ELeave) SmlPcdata_t();
			return sourceref;
		
		case EDevDisplayName:
			CHECK_NO_DUPLICATE(displayname);
			displayname = new (ELeave) SmlPcdata_t();
			return displayname;
		
		case EDevMaxGUIDSize:
			CHECK_NO_DUPLICATE(maxguidsize);
			maxguidsize = new (ELeave) SmlPcdata_t();
			return maxguidsize;
		
		case EDevRxPref:
			CHECK_NO_DUPLICATE(rxpref);
			rxpref = new (ELeave) SmlDevInfXmit_t();
			return rxpref; 

		case EDevRx:
			return AddXmitL(&rx);

		case EDevTxPref:
			CHECK_NO_DUPLICATE(txpref);
			txpref = new (ELeave) SmlDevInfXmit_t();
			return txpref;
		
		case EDevTx:
			return AddXmitL(&tx);

//1.2 CHANGES: in version 1.2 CtCap inside DataStore -element 
		case EDevCTCap:
			return AddCtCapL(&ctcap);
// Changes end

		case EDevDSMem:
			CHECK_NO_DUPLICATE(dsmem);
			dsmem = new (ELeave) SmlDevInfDSMem_t();
			return dsmem;

		case EDevSupportHierarchicalSync:
			CHECK_NO_DUPLICATE(supportHierarchicalSync);
			supportHierarchicalSync = new (ELeave) SmlPcdata_t();
			return supportHierarchicalSync;

		case EDevSyncCap:
			CHECK_NO_DUPLICATE(synccap);
			synccap = new (ELeave) SmlDevInfSyncCap_t();
			return synccap;

//1.2 CHANGES: New elements added FilterRx and FilterCap
		case EDevFilterRx:
			return AddXmitL(&filterrx);

		case EDevFilterCap:
			return AddFilterCapL(&filtercap);
// Changes end
							
		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_datastorelist_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_datastorelist_s::sml_devinf_datastorelist_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_datastorelist_s::~sml_devinf_datastorelist_s()
	{
	delete data;	
	}

// ------------------------------------------------------------------------------------------------
// sml_devinf_devinf_s
// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_devinf_s::sml_devinf_devinf_s()
	{
	}

// ------------------------------------------------------------------------------------------------
EXPORT_C sml_devinf_devinf_s::~sml_devinf_devinf_s()
	{
    delete verdtd;
    delete man; /* optional */
    delete mod; /* optional */
    delete oem; /* optional */
    delete fwv; /* optional */
    delete swv; /* optional */
    delete hwv; /* optional */
    delete devid;
    delete devtyp;    
    GenericListDelete( datastore );	
	GenericListDelete( ctcap );    
    GenericListDelete( ext );
	}


// ------------------------------------------------------------------------------------------------
CXMLElement* sml_devinf_devinf_s::BeginElementL( TUint8 aTag, const TXMLElementParams& /*aParams*/ )
	{
	switch( aTag )
		{
		case EDevVerDTD:
			CHECK_NO_DUPLICATE(verdtd);
			verdtd = new (ELeave) SmlPcdata_t();
			return verdtd;
		
		case EDevMan:
			CHECK_NO_DUPLICATE(man);
			man = new (ELeave) SmlPcdata_t();
			return man;
		
		case EDevMod:
			CHECK_NO_DUPLICATE(mod);
			mod = new (ELeave) SmlPcdata_t();
			return mod;

		case EDevOEM:
			CHECK_NO_DUPLICATE(oem);
			oem = new (ELeave) SmlPcdata_t();
			return oem;

		case EDevFwV:
			CHECK_NO_DUPLICATE(fwv);
			fwv = new (ELeave) SmlPcdata_t();
			return fwv;
		
		case EDevSwV:
			CHECK_NO_DUPLICATE(swv);
			swv = new (ELeave) SmlPcdata_t();
			return swv;

		case EDevHwV:
			CHECK_NO_DUPLICATE(hwv);
			hwv = new (ELeave) SmlPcdata_t();
			return hwv;

		case EDevDevID:
			CHECK_NO_DUPLICATE(devid);
			devid = new (ELeave) SmlPcdata_t();
			return devid;

		case EDevDevTyp:
			CHECK_NO_DUPLICATE(devtyp);
			devtyp = new (ELeave) SmlPcdata_t();
			return devtyp;

		case EDevUTC:
			CHECK_NO_DUPLICATE(flags & SmlUTC_f);
			flags |= SmlUTC_f;
			break;

		case EDevSupportNumberOfChanges:
			CHECK_NO_DUPLICATE(flags & SmlSupportNumberOfChanges_f);
			flags |= SmlSupportNumberOfChanges_f;
			break;
		
		case EDevSupportLargeObjs:
			CHECK_NO_DUPLICATE(flags & SmlSupportLargeObjects_f);
			flags |= SmlSupportLargeObjects_f;
			break;
		
		case EDevDataStore:
			return AddDataStoreL(&datastore);

		case EDevCTCap:
			return AddCtCapL(&ctcap, this);
				
		case EDevExt:
			return AddExtL(&ext);

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}
	return 0;
	}

// ------------------------------------------------------------------------------------------------
CXMLElement::TAction sml_devinf_devinf_s::EndElementL( TAny* aCallbacks, TUint8 aTag )
	{
	switch( aTag )
		{
		case EDevDevInf:
			((MWBXMLDevInfCallbacks*)aCallbacks)->smlDeviceInfoL(this);
			return EPopAndDestroy;

		case EDevUTC:
		case EDevSupportNumberOfChanges:
		case EDevSupportLargeObjs:
			return ENone;

		default:
			User::Leave(KWBXMLParserErrorInvalidTag);
		}

	return EPop;
	}

// ------------------------------------------------------------------------------------------------
TBool sml_devinf_devinf_s::NeedsCleanup() const
	{
	return ETrue;
	}
