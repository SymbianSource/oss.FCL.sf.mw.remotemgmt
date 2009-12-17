/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/


#ifndef ROOT_ELEMENT_HEADER_
#define ROOT_ELEMENT_HEADER_


// INCLUDES


#include "ElementBase.h"

#include <e32base.h>
#include <ContentHandler.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION


class CRootElement : public CElementBase
{
	public:
		void EncodeXACML( TDesC& aXACML);
		void DecodeXACML( TDes& aXACML);
		
		void Load();
		void Save();
	
	private:
		HBufC * iVersion;
		HBufC * encoding;
	
};



#endif