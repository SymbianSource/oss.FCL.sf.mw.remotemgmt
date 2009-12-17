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
* Description:  DS Utils Data Store format
*
*/


#ifndef __NSMLDSSTOREFORMATUTILS_H__
#define __NSMLDSSTOREFORMATUTILS_H__

// INCLUDES
#include <e32base.h>
#include <SmlDataFormat.h>
#include <SmlDataFormat.hrh>

// CLASS DECLARATION

/**
* Utility class for CSmlDataStoreFormat.
* 
* @lib smlstoreformat.lib
*/
class TSmlDataStoreFormatUtils
	{
	public:

	    /**
		* Sets the received resource type to the sync type mask.
		* @param aSyncTypeMask. the Mask where the supported sync types are stored. 
		* @param aSyncTypeFromResource. single sync type. 
		*/
		static void SetSyncTypeMaskFromResource( TSmlSyncTypeMask& aSyncTypeMask, TUint16 aSyncTypeFromResource );
		
		};

#endif // __NSMLDSSTOREFORMATUTILS_H__

// End of File
