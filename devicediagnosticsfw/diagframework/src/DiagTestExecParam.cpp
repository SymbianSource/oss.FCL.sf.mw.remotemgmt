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
* Description:  Class definition of TDiagTestExecParam
*
*/


// CLASS DECLARATION
#include <DiagTestExecParam.h>

// SYSTEM INCLUDE FILES

// USER INCLUDE FILES

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TDiagTestExecParam::TDiagTestExecParam()
// ---------------------------------------------------------------------------
//
EXPORT_C TDiagTestExecParam::TDiagTestExecParam( MDiagTestObserver& aObserver,
                                                  MDiagEngineCommon& aEngine )
    :   iObserver( aObserver ),
        iEngine( aEngine )
    {
    }


// ---------------------------------------------------------------------------
// TDiagTestExecParam::Observer()
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagTestObserver& TDiagTestExecParam::Observer() const
    {
    return iObserver;
    }


// ---------------------------------------------------------------------------
// TDiagTestExecParam::Engine()
// ---------------------------------------------------------------------------
//
EXPORT_C MDiagEngineCommon& TDiagTestExecParam::Engine() const
    {
    return iEngine;
    }

// End of File

