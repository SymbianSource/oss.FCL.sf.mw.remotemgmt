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
* Description:  Utilities for DS Loader Framework
*
*/


// INCLUDE FILES
#include "nsmldsitemmodificationset.h"

// ================================= MEMBER FUNCTIONS =============================================

// ------------------------------------------------------------------------------------------------
// TNSmlDbItemModification::TNSmlDbItemModification
// C++ constructor.
// ------------------------------------------------------------------------------------------------
EXPORT_C TNSmlDbItemModification::TNSmlDbItemModification( TSmlDbItemUid aItemId, 
	TNSmlDbItemModificationType aModType ) : iItemId(aItemId), iModType(aModType)
	{
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbItemModification::ItemId
// Retuns the item UID.
// ------------------------------------------------------------------------------------------------
EXPORT_C TSmlDbItemUid TNSmlDbItemModification::ItemId() const
	{
	return iItemId;
	}
	
// ------------------------------------------------------------------------------------------------
// TNSmlDbItemModification::ModificationType
// Retuns the modification type.
// ------------------------------------------------------------------------------------------------
EXPORT_C TNSmlDbItemModification::TNSmlDbItemModificationType TNSmlDbItemModification::ModificationType() const
	{
	return iModType;
	}

// ------------------------------------------------------------------------------------------------
// TNSmlDbItemModification::TNSmlDbItemModification
// Copy constructor
// ------------------------------------------------------------------------------------------------
EXPORT_C TNSmlDbItemModification::TNSmlDbItemModification( const TNSmlDbItemModification& aDim )
	{
	iItemId = aDim.ItemId();
	iModType = aDim.ModificationType();
	}
	
// ------------------------------------------------------------------------------------------------
// TNSmlDbItemModification::operator=
// Assignment operator
// ------------------------------------------------------------------------------------------------
EXPORT_C TNSmlDbItemModification& TNSmlDbItemModification::operator=( const TNSmlDbItemModification& aDim )
	{
	if ( &aDim != this ) 
		{
		iItemId = aDim.ItemId();
		iModType = aDim.ModificationType();
		}
	
	return *this;
	}

// End of File
