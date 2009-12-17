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



// INCLUDE FILES
#include "nsmldbcaps.h"
#include "nsmlfilter.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::CNSmlFilterHandler
// Constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlFilterHandler::CNSmlFilterHandler( sml_filter_s* aFilter )
    : iFilter(aFilter)
    {
    }

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::FilterMetaType
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlFilterHandler::FilterMetaType() const
    {
    if ( iFilter && iFilter->meta )
        {
        SmlMetInfMetInf_t* metinf;
        metinf = (SmlMetInfMetInf_t*) iFilter->meta->content;
        return metinf->type->Data();
        }
    return TPtrC8();
    }

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::FilterType
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlFilterHandler::FilterType() const
    {
    if ( iFilter && iFilter->filterType )
        {
        return iFilter->filterType->Data();
        }
    return TPtrC8();
    }

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::RecordMetaType
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlFilterHandler::RecordMetaType() const
    {
    if ( iFilter && iFilter->record )
        {
        SmlItem_t* item;
        item = iFilter->record->item;

        if ( item->meta )
            {
            SmlMetInfMetInf_t* metinf;
            metinf = (SmlMetInfMetInf_t*) item->meta->content;
            
            if ( metinf->type )
                {
                return metinf->type->Data();
                }
            }
        }

    return TPtrC8();
    }

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::RecordData
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlFilterHandler::RecordData() const
    {
    if ( iFilter && iFilter->record )
        {
        SmlItem_t* item;
        item = iFilter->record->item;

        if ( item->data )
            {
            return item->data->Data();
            }
        }
    
    return TPtrC8();
    }

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::FieldMetaType
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC8 CNSmlFilterHandler::FieldMetaType() const
    {
    if ( iFilter && iFilter->field )
        {
        SmlItem_t* item;
        item = iFilter->field->item;

        if ( item->meta )
            {
            SmlMetInfMetInf_t* metinf;
            metinf = (SmlMetInfMetInf_t*) item->meta->content;

            return metinf->type->Data();
            }
        }
    return TPtrC8();
    }

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::FieldDataPropsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlFilterHandler::FieldDataPropsL( CArrayFix<TNSmlFieldPropData>& aFieldProps, const TDesC8& aPropName )
    {
    if ( iFilter && iFilter->field )
        {
        SmlItem_t* item;
        item = iFilter->field->item;
        
        if ( item->data )
            {
            SmlDevInfPropertyList_t* proplist;
            proplist = (SmlDevInfPropertyList_t*) item->data->content;

            aFieldProps.Reset();
            if ( proplist )
                {
                if( aPropName.Length() == 0 )
			        {
			        // Add all properties into array.
			        for( SmlDevInfPropertyListPtr_t l = proplist; l; l = l->next )
				        {
			            AppendFieldPropL(aFieldProps, l->data);
                        }
			        }
		        else
			        {
			        // Append only properties of defined property name into array.
			        for( SmlDevInfPropertyListPtr_t l = proplist; l; l = l->next )
				        {
				        if( l->data->propname->Data() ==  aPropName )
					        {
					        AppendFieldPropL(aFieldProps, l->data);
					        break;
					        }
				        }
			        }
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::AppendFieldPropL
// -----------------------------------------------------------------------------
//
void CNSmlFilterHandler::AppendFieldPropL( CArrayFix<TNSmlFieldPropData>& aFieldProps, SmlDevInfPropertyPtr_t aProp )
	{
    if ( aProp->propname )
        {
        aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EPropName, aProp->propname->Data()));
        }
    if ( aProp->datatype )
        {
        aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EPropDataType, aProp->datatype->Data()));
        }
    if ( aProp->maxoccur )
        {
        aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EPropMaxOccur, aProp->maxoccur->Data()));
        }
    if ( aProp->maxsize )
        {
        aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EPropMaxSize, aProp->maxsize->Data()));
        }
    if ( aProp->notruncate )
        {
        aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EPropNoTruncate, aProp->notruncate->Data()));
        }
    if ( aProp->valenum )
        {
        for ( SmlPcdataListPtr_t l = aProp->valenum; l; l = l->next )
            {
            aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EPropValEnum, l->data->Data()));
            }
        }
    if ( aProp->displayname )
        {
        aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EPropDisplayName, aProp->displayname->Data()));
        }
	for( SmlDevInfPropParamListPtr_t l = aProp->propparam; l; l = l->next )
		{
		AppendPropParamL(aFieldProps, l->data, TNSmlFieldPropData::EParamName);
		}
	}

// -----------------------------------------------------------------------------
// CNSmlFilterHandler::AppendPropParamL
// -----------------------------------------------------------------------------
//
void CNSmlFilterHandler::AppendPropParamL( CArrayFix<TNSmlFieldPropData>& aFieldProps, SmlDevInfPropParamPtr_t aProp, TNSmlFieldPropData::TNSmlFieldPropTag aTag )
	{
	aFieldProps.AppendL(TNSmlFieldPropData(aTag, aProp->paramname->Data()));
	if( aProp->valenum )
		{
		for( SmlPcdataListPtr_t l = aProp->valenum; l; l = l->next )
			{
			aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EParamValEnum, l->data->Data()));
			}
		}
	if( aProp->datatype )    
		{
		aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EParamDataType, aProp->datatype->Data()));
		}

	if( aProp->displayname )
		{
		aFieldProps.AppendL(TNSmlFieldPropData(TNSmlFieldPropData::EParamDisplayName, aProp->displayname->Data()));
		}
	}

//  End of File
