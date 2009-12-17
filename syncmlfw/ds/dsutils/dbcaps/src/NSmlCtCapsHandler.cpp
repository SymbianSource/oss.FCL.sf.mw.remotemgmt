/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Source for CNSmlCtCapsHandler class
*
*/


// INCLUDE FILES
#include <badesca.h>

#include <nsmldebug.h>
#include "nsmldbcaps.h"
#include "smldevinfdtd.h"
#include "smlmetinfdtd.h"
#include "smldevinftags.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlCtCapsHandler::CNSmlCtCapsHandler
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlCtCapsHandler::CNSmlCtCapsHandler( sml_devinf_ctcaplist_s* aCtCaps )
 : iCtCaps(aCtCaps)
	{
	//_DBG_FILE("CNSmlCtCapsHandler::CNSmlCtCapsHandler(): begin");
	//_DBG_FILE("CNSmlCtCapsHandler::CNSmlCtCapsHandler(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlCtCapsHandler::GetCtCapsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlCtCapsHandler::GetCtCapsL( CArrayFix<TNSmlCtCapData>& aCtCaps, const TDesC8& aCtType )
	{
	aCtCaps.Reset();
	if( iCtCaps )
		{
		if( aCtType.Length() == 0 )
			{
			// add all to array
			for( SmlDevInfCtCapListPtr_t l = iCtCaps; l; l = l->next )
				{
				AppendCtCapL(aCtCaps, l->data);
				}
			}
		else
			{
			// append only defined cttype into array
			for( SmlDevInfCtCapListPtr_t l = iCtCaps; l; l = l->next )
				{
				if( l->data->cttype->Data() ==  aCtType )
					{
					AppendCtCapL(aCtCaps, l->data);
					break;
					}
				}
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlCtCapsHandler::AppendCtCapL
// -----------------------------------------------------------------------------
//
void CNSmlCtCapsHandler::AppendCtCapL( CArrayFix<TNSmlCtCapData>& aCtCaps, SmlDevInfCtCapPtr_t aCtCap )
	{
    if ( aCtCap->cttype )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::ECtType, aCtCap->cttype->Data()));
        }
    if ( aCtCap->verct )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EVerCt, aCtCap->verct->Data()));
        }
	if ( aCtCap->fieldlevel)
		{
		aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EFieldLevel, aCtCap->fieldlevel->Data()));
		}
    for( SmlDevInfPropertyListPtr_t l = aCtCap->property; l; l = l->next )
    	{
    	AppendDevInfPropL(aCtCaps, l->data);
    	}
    }

// -----------------------------------------------------------------------------
// CNSmlCtCapsHandler::AppendDevInfPropL
// -----------------------------------------------------------------------------
//
void CNSmlCtCapsHandler::AppendDevInfPropL( CArrayFix<TNSmlCtCapData>& aCtCaps, SmlDevInfPropertyPtr_t aCtCap )
	{
    if ( aCtCap->propname )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EPropName, aCtCap->propname->Data()));
        }
    if ( aCtCap->datatype )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EPropDataType, aCtCap->datatype->Data()));
        }
    if ( aCtCap->maxoccur )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EPropMaxOccur, aCtCap->maxoccur->Data()));
        }
    if ( aCtCap->maxsize )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EPropMaxSize, aCtCap->maxsize->Data()));
        }
    if ( aCtCap->notruncate )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EPropNoTruncate, aCtCap->notruncate->Data()));
        }
    if ( aCtCap->valenum )
        {
        for ( SmlPcdataListPtr_t l = aCtCap->valenum; l; l = l->next )
            {
            aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EPropValEnum, l->data->Data()));
            }
        }
    if ( aCtCap->displayname )
        {
        aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EPropDisplayName, aCtCap->displayname->Data()));
        }
	for( SmlDevInfPropParamListPtr_t l = aCtCap->propparam; l; l = l->next )
		{
		AppendPropParamL(aCtCaps, l->data, TNSmlCtCapData::EParamName);
		}
	}

// -----------------------------------------------------------------------------
// CNSmlCtCapsHandler::AppendPropParamL
// -----------------------------------------------------------------------------
//
void CNSmlCtCapsHandler::AppendPropParamL( CArrayFix<TNSmlCtCapData>& aCtCaps, SmlDevInfPropParamPtr_t aCtCap, TNSmlCtCapData::TNSmlCtCapTag aTag )
	{
	aCtCaps.AppendL(TNSmlCtCapData(aTag, aCtCap->paramname->Data()));
	if( aCtCap->valenum )
		{
		for( SmlPcdataListPtr_t l = aCtCap->valenum; l; l = l->next )
			{
			aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EParamValEnum, l->data->Data()));
			}
		}
	if( aCtCap->datatype )    
		{
		aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EParamDataType, aCtCap->datatype->Data()));
		}

	if( aCtCap->displayname )
		{
		aCtCaps.AppendL(TNSmlCtCapData(TNSmlCtCapData::EParamDisplayName, aCtCap->displayname->Data()));
		}
	}

//  End of File
