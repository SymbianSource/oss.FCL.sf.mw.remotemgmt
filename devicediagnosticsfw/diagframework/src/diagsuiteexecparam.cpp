/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition of TDiagSuiteExecParam
*
*/


// CLASS DECLARATION
#include <DiagSuiteExecParam.h>

// SYSTEM INCLUDE FILES

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TDiagSuiteExecParam::TDiagSuiteExecParam()
// ---------------------------------------------------------------------------
//
EXPORT_C TDiagSuiteExecParam::TDiagSuiteExecParam( MDiagSuiteObserver& aObserver,
                                                    MDiagEngineCommon& aEngine )
    :   iObserver( aObserver ),
        iEngine( aEngine )
    {
    }

// ---------------------------------------------------------------------------
// TDiagSuiteExecParam::Observer()
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagSuiteObserver& TDiagSuiteExecParam::Observer() const
    {
    return iObserver;
    }


// ---------------------------------------------------------------------------
// TDiagSuiteExecParam::Engine()
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagEngineCommon& TDiagSuiteExecParam::Engine() const
    {
    return iEngine;
    }

// End of File

