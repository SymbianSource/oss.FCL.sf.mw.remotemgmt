/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Info class for BT connection
*
*/



// INCLUDE FILES
#include    "Btconninfo.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TBTConnInfo::TBTConnInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

EXPORT_C TBTConnInfo::TBTConnInfo() :
	iDevAddr( TBTDevAddr()), iServiceClass( TUUID( 0x00000001, 0x00001000, 0x80000002, 0xEE000002 ) )
    {
    }

// Destructor
EXPORT_C TBTConnInfo::~TBTConnInfo()
    {
    }

//  End of File  
