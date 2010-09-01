/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provisioning context list
*
*/


//  INCLUDE FILES
#include <eikstart.h>

#include "CWPCxApp.h"
#include "CWPCxDocument.h"
#include "ProvisioningUIDs.h"


LOCAL_C CApaApplication* NewApplication()
    {
    return new CWPCxApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWPCxApp::AppDllUid()
// ---------------------------------------------------------
//
TUid CWPCxApp::AppDllUid() const
    {
    return TUid::Uid( KProvisioningCxUID3 );
    }

// ---------------------------------------------------------
// CWPCxApp::CreateDocumentL()
// ---------------------------------------------------------
//
CApaDocument* CWPCxApp::CreateDocumentL()
    {
    return CWPCxDocument::NewL( *this );
    }

// End of File  

