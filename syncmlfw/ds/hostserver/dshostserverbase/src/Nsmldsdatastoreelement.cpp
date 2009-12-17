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
* Description:  array for storing dataproviders and datastores.
*
*/


// INCLUDE FILES
#include <s32std.h>
#include <nsmldebug.h>

#include "nsmldshostconstants.h"
#include "Nsmldsdataproviderarray.h"

#ifdef __HOST_SERVER_MTEST__
#include "../../stif/DSHostServerTest/inc/fakedataprovider.h"
#else
#include <SmlDataProvider.h>
#endif

// ======================================= MEMBER FUNCTIONS =======================================

// ------------------------------------------------------------------------------------------------
// TNSmlDSDataStoreElement::FreeBatchItemArray
// ------------------------------------------------------------------------------------------------
void TNSmlDSDataStoreElement::FreeBatchItemArray()
    {
    if ( iBatchItemUids )
    	{
    	iBatchItemUids->ResetAndDestroy();
    	delete iBatchItemUids;
    	iBatchItemUids = NULL;
    	}
    }

// ------------------------------------------------------------------------------------------------
// TNSmlDSDataStoreElement::IsOpen
// ------------------------------------------------------------------------------------------------
TBool TNSmlDSDataStoreElement::IsOpen() const
    {
    return ( iStoreName == NULL );
    }

// ------------------------------------------------------------------------------------------------
// TNSmlDSDataStoreElement::SetOpened
// ------------------------------------------------------------------------------------------------    
void TNSmlDSDataStoreElement::SetOpened()
	{
	delete iStoreName;
	iStoreName = NULL;
	}
	
// ------------------------------------------------------------------------------------------------
// TNSmlDSDataStoreElement::StoreName
// ------------------------------------------------------------------------------------------------
const TDesC& TNSmlDSDataStoreElement::StoreName() const
	{
	if ( iStoreName )
		{
		return *iStoreName;
		}
	return iDataStore->StoreName();
    }

// ------------------------------------------------------------------------------------------------
// TNSmlDSDataStoreElement::setStoreFormat
// ------------------------------------------------------------------------------------------------
void TNSmlDSDataStoreElement::setStoreFormat( CSmlDataStoreFormat* aDSFormat )
	{
	delete iDSFormat;
	iDSFormat = aDSFormat;
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDSDataStoreElement::TNSmlDSDataStoreElement
// ------------------------------------------------------------------------------------------------
TNSmlDSDataStoreElement::TNSmlDSDataStoreElement( const HBufC* aStoreName ) : iDSAO( NULL ), 
    iDataStore( NULL ), iBatchItemUids( NULL ), iAdpLog( NULL ), iCreatedUid( 0 ), 
    iHostMode( ENSmlNormalMode ), iStoreName( aStoreName ), iDSFormat( NULL )
    {
    }

// End of File
