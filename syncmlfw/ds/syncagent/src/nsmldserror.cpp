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
* Description:  Error code conversion
*
*/


// INCLUDE FILES
#include "nsmldserror.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TNSmlDSError::TNSmlDSError
// C++ constructor.
// -----------------------------------------------------------------------------
//
TNSmlDSError::TNSmlDSError() : TNSmlError()
	{
	}

// -----------------------------------------------------------------------------
// TNSmlDSError::SyncLogMessage
// Converts native error code to message code for UI. Currently does nothing.
// -----------------------------------------------------------------------------
//
TInt TNSmlDSError::SyncLogMessageCode()
	{
	return TNSmlError::SyncLogErrorCode();
	}

// -----------------------------------------------------------------------------
// TNSmlDSError::ErrorCodeConversion
// Converts some error codes to Symbian error codes.
// -----------------------------------------------------------------------------
//
void TNSmlDSError::ErrorCodeConversion()
	{
	TNSmlError::ErrorCodeConversion();
	}

// End of File
