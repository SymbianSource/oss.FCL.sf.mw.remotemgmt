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
* Description:  Declares SyncML notifiers server application class.
*
*/


// INCLUDE FILES
#include "SyncMLNotifierServerApplication.h"
#include "SyncMLNotifDebug.h"

// CONSTANTS
_LIT( KSmlNotifierLibraryFileName,  "SyncMLNotifier.dll" );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// NewApplication Creates a new notifier server application.
// -----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
	{
	FLOG(_L("[SmlNotif]\t NewApplication()"));
	return new CSyncMLNotifierServerApplication();
	}	
	
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSyncMLNotifierServerApplication::CSyncMLNotifierServerApplication
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLNotifierServerApplication::CSyncMLNotifierServerApplication()
	{
	FLOG(_L("[SmlNotif]\t CSyncMLNotifierServerApplication::CreateDocumentL()"));
	}

// -----------------------------------------------------------------------------
// CSyncMLNotifierServerApplication::NewAppServerL
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierServerApplication::NewAppServerL( CApaAppServer*& aAppServer )
    {
    FLOG(_L("[SmlNotif]\t CSyncMLNotifierServerApplication::NewAppServerL()"));
    CAknNotifierAppServer* server = new (ELeave) CAknNotifierAppServer();
    CleanupStack::PushL(server);

    server->AppendNotifierLibNameL( KSmlNotifierLibraryFileName );
    server->LoadNotifiersL();
    CleanupStack::Pop( server );
    aAppServer = &*server;
    FLOG(_L("[SmlNotif]\t CSyncMLNotifierServerApplication::NewAppServerL() completed"));
    }
        
// -----------------------------------------------------------------------------
// CSyncMLNotifierServerApplication::CreateDocumentL
// -----------------------------------------------------------------------------
//
CApaDocument* CSyncMLNotifierServerApplication::CreateDocumentL()
	{
	FLOG(_L("[SmlNotif]\t CSyncMLNotifierServerApplication::CreateDocumentL()"));
	return new (ELeave) CSyncMLNotifierAppServerDocument( *this );
	}

// -----------------------------------------------------------------------------
// CSyncMLNotifierAppServerDocument::CreateAppUiL
// -----------------------------------------------------------------------------
//
CEikAppUi* CSyncMLNotifierAppServerDocument::CreateAppUiL()
	{
	FLOG(_L("[SmlNotif]\t CSyncMLNotifierAppServerDocument::CreateAppUiL()"));
	return new (ELeave) CSyncMLNotifierAppServerAppUi();
	}

// -----------------------------------------------------------------------------
// CSyncMLNotifierAppServerAppUi::CSyncMLNotifierAppServerAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSyncMLNotifierAppServerAppUi::CSyncMLNotifierAppServerAppUi()
	{
	FLOG(_L("[SmlNotif]\t CSyncMLNotifierAppServerAppUi::CSyncMLNotifierAppServerAppUi()"));
	}

// Destructor
CSyncMLNotifierAppServerAppUi::~CSyncMLNotifierAppServerAppUi()
	{
	FLOG(_L("[SmlNotif]\t CSyncMLNotifierAppServerAppUi::~CSyncMLNotifierAppServerAppUi()"));
	}

// -----------------------------------------------------------------------------
// CSyncMLNotifierAppServerAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSyncMLNotifierAppServerAppUi::ConstructL()
	{
	FLOG(_L("[SmlNotif]\t CSyncMLNotifierAppServerAppUi::ConstructL()"));
	// base call
	CAknNotifierAppServerAppUi::ConstructL();
	FLOG(_L("[SmlNotif]\t CSyncMLNotifierAppServerAppUi::ConstructL() completed"));
	}
	
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// E32Main
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

//  End of File  
