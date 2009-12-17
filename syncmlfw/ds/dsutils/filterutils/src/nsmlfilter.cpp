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
* Description:  Filter sources.
*
*/


// ------------------------------------------------------------------------------------------------
// Includes
// ------------------------------------------------------------------------------------------------
#include "nsmldbcaps.h"
#include "nsmlfilter.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"


// ------------------------------------------------------------------------------------------------
// CNSmlFilter
// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------
EXPORT_C CNSmlFilter* CNSmlFilter::NewL()
    {
	CNSmlFilter* self = CNSmlFilter::NewLC();
	CleanupStack::Pop();
	return self;
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C CNSmlFilter* CNSmlFilter::NewLC()
    {
	CNSmlFilter* self = new (ELeave) CNSmlFilter();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C CNSmlFilter::~CNSmlFilter()
    {
    delete iFilter;    
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

void CNSmlFilter::ConstructL()
    {
    iFilter = new (ELeave) sml_filter_s();
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

CNSmlFilter::CNSmlFilter()
    {    
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C CNSmlFilterHandler* CNSmlFilter::CreateHandlerL() const
    {
    return new (ELeave) CNSmlFilterHandler(iFilter);
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C SmlFilterPtr_t CNSmlFilter::FilterL() const
    {
    SmlFilterPtr_t filter = iFilter;
    iFilter = new (ELeave) sml_filter_s();
    return filter;
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C SmlDevInfPropertyListPtr_t CNSmlFilter::Properties() const
    {
    return iPropList;
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlFilter::SetFilterMetaTypeL( const TDesC8& aMetaType )
    {
    if ( iFilter->meta )
        {
        delete iFilter->meta;
        iFilter->meta = NULL;
        }

    SmlMetInfMetInfPtr_t metinf = new (ELeave) SmlMetInfMetInf_t();
    
    CleanupStack::PushL(metinf);

    PcdataNewL( metinf->type, aMetaType );
    DoMetaL( iFilter->meta, metinf );
    CleanupStack::Pop(); // metinf
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlFilter::SetFilterTypeL( const TDesC8& aFilterType )
    {
    if ( !iFilter->filterType )
        {
        iFilter->filterType = new (ELeave) SmlPcdata_t();
        iFilter->filterType->SetDataL(aFilterType);
        }
    else
        {
        iFilter->filterType->SetDataL(aFilterType);
        }
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlFilter::SetRecordL( const TDesC8& aMetaType, const TDesC8& aData )
    {
    if ( iFilter->record && iFilter->record->item->meta )
        {
        delete iFilter->record->item->meta;
        iFilter->record->item->meta = 0;
        }
    if ( iFilter->record && iFilter->record->item->data )
        {
        delete iFilter->record->item->data;
        iFilter->record->item->data = 0;
        }

    SmlMetInfMetInfPtr_t metinf = new (ELeave) SmlMetInfMetInf_t();
    CleanupStack::PushL(metinf);

    PcdataNewL( metinf->type, aMetaType );

    if ( !iFilter->record )
        {
        iFilter->record = new (ELeave) SmlRecord_t;
        iFilter->record->item = new (ELeave) SmlItem_t;
        }

    DoMetaL( iFilter->record->item->meta, metinf );    
    CleanupStack::Pop(); // metinf

    PcdataNewL( iFilter->record->item->data, aData );
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C void CNSmlFilter::SetFieldMetaTypeL( const TDesC8& aMetaType )
    {
    if ( iFilter->field && iFilter->field->item->meta )
        {
        delete iFilter->field->item->meta;
        iFilter->field->item->meta = 0;
        }
    SmlMetInfMetInfPtr_t metinf = new (ELeave) SmlMetInfMetInf_t();
    CleanupStack::PushL(metinf);

    if ( !iFilter->field )
        {
        iFilter->field = new (ELeave) SmlField_t;
        iFilter->field->item = new (ELeave) SmlItem_t;
        }
    CleanupStack::Pop(); // metinf
        
    PcdataNewL( metinf->type, aMetaType );
    DoMetaL( iFilter->field->item->meta, metinf );
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

EXPORT_C CNSmlDevInfProp* CNSmlFilter::AddFieldDataPropLC( const TDesC8& aPropName )
    {     
    SmlDevInfPropertyListPtr_t proplist = new (ELeave) SmlDevInfPropertyList_t();
	CleanupStack::PushL(proplist);
 
    GenericListAddL( &iPropList, proplist );

    if ( !iFilter->field )
        {
        iFilter->field = new (ELeave) SmlField_t;
        iFilter->field->item = new (ELeave) SmlItem_t;
        }
    CleanupStack::Pop(); // proplist

    proplist->data = new (ELeave) SmlDevInfProperty_t();
    proplist->data->propname = new (ELeave) SmlPcdata_t();
    proplist->data->propname->SetDataL(aPropName);

    DoItemDataL( iFilter->field->item->data, iPropList );    

    CNSmlDevInfProp* prop = new (ELeave) CNSmlDevInfProp(proplist->data);
    CleanupStack::PushL(prop);    
    
    return prop;
    }

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

void CNSmlFilter::PcdataNewL( SmlPcdata_t*& aPcdata, const TDesC8& aContent ) const
	{
    aPcdata = new( ELeave ) SmlPcdata_t;
	aPcdata->SetDataL( aContent );
	aPcdata->contentType = SML_PCDATA_OPAQUE;   
	aPcdata->extension = SML_EXT_UNDEFINED; 
	}

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

void CNSmlFilter::DoMetaL( SmlPcdata_t*& aMeta, const SmlMetInfMetInf_t* aMetaData ) const
	{
    aMeta = new( ELeave ) SmlPcdata_t; 
	aMeta->length = 0;
	aMeta->content = ( TAny* ) aMetaData;
	aMeta->contentType = SML_PCDATA_EXTENSION;
	aMeta->extension = SML_EXT_METINF;
	}

// ------------------------------------------------------------------------------------------------
// 
// ------------------------------------------------------------------------------------------------

void CNSmlFilter::DoItemDataL( SmlPcdata_t*& aData, const SmlDevInfPropertyList_t* aProp ) const
	{
    if ( !aData)
        {
        aData = new( ELeave ) SmlPcdata_t; 
        }
	aData->length = 0;
	aData->content = ( TAny* ) aProp;
	aData->contentType = SML_PCDATA_EXTENSION;
	aData->extension = SML_EXT_DEVINFPROP; 
	}

//End of File
