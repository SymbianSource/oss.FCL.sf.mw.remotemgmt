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
* Description:  Dumper application
*
*/


//  INCLUDE FILES
#include "CWPCxDocument.h"
#include "CWPCxAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// C++ constructor
CWPCxDocument::CWPCxDocument( CEikApplication& aApp )
: CAknDocument(aApp)
    {
    }

// destructor
CWPCxDocument::~CWPCxDocument()
    {
    }

// EPOC default constructor can leave.
void CWPCxDocument::ConstructL()
    {
    }

// Two-phased constructor.
CWPCxDocument* CWPCxDocument::NewL( CEikApplication& aApp )
    {
    CWPCxDocument* self = new (ELeave) CWPCxDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CWPCxDocument::CreateAppUiL
// ----------------------------------------------------
//
CEikAppUi* CWPCxDocument::CreateAppUiL()
    {
    return new (ELeave) CWPCxAppUi;
    }

// End of File  
