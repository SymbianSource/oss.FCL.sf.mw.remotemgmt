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
* Description:  Source for CNSmlFilterCapsHandler class
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
// CNSmlFilterCapsHandler::CNSmlFilterCapsHandler
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlFilterCapsHandler::CNSmlFilterCapsHandler( sml_devinf_filtercaplist_s* aFilterCaps) : iFilterCaps(aFilterCaps)
	{
	}


// -----------------------------------------------------------------------------
// CNSmlFilterCapsHandler::AppendFilterCapL
// -----------------------------------------------------------------------------
//
void CNSmlFilterCapsHandler::AppendFilterCapL(  CArrayFix<TNSmlFilterCapData>& aFilterCaps, SmlDevInfFilterCapPtr_t aFilterCap )
	{
	if ( aFilterCap->cttype)
		{
		aFilterCaps.AppendL(TNSmlFilterCapData(TNSmlFilterCapData::ECtType, aFilterCap->cttype->Data()));
		}
	if ( aFilterCap->verct)
		{
		aFilterCaps.AppendL(TNSmlFilterCapData(TNSmlFilterCapData::EVerCt, aFilterCap->verct->Data()));
		}

	if ( aFilterCap->filterkeyword )
		{
		for ( SmlPcdataListPtr_t l = aFilterCap->filterkeyword; l; l = l->next )
			{
			aFilterCaps.AppendL(TNSmlFilterCapData(TNSmlFilterCapData::EFilterKeyword, l->data->Data()));
			}
		}
	
	if ( aFilterCap->propname )
		{
		for ( SmlPcdataListPtr_t l = aFilterCap->propname; l; l = l->next)
			{
			aFilterCaps.AppendL(TNSmlFilterCapData(TNSmlFilterCapData::EPropName, l->data->Data()));
			}
		}
	}

// -----------------------------------------------------------------------------
// CNSmlFilterCapsHandler::GetFilterCapsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlFilterCapsHandler::GetFilterCapsL( CArrayFix<TNSmlFilterCapData>& aFilterCaps, const TDesC8& aCtType )
	{
	aFilterCaps.Reset();
	if( iFilterCaps )
		{
		if( aCtType.Length() == 0 )
			{
			// add all to array
			for( SmlDevInfFilterCapListPtr_t l = iFilterCaps; l; l = l->next )
				{
				AppendFilterCapL(aFilterCaps, l->data);
				}
			}
		else
			{
			// append only defined cttype into array
			for( SmlDevInfFilterCapListPtr_t l = iFilterCaps; l; l = l->next )
				{
				if( l->data->cttype->Data() ==  aCtType )
					{
					AppendFilterCapL(aFilterCaps, l->data);
					break;
					}
				}
			}
		}
	}

//  End of File
