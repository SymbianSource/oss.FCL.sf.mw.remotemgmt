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
* Description:   Methods for CFotaSrvUi
*
*/



// INCLUDES 

#include <apgwgnam.h>
#include <aknnotewrappers.h>
#include <StringLoader.h>
#include <textresolver.h>
#include "FotaSrvUI.h"
#include "FotaSrvApp.h"
#include "FotaSrvDebug.h"

// ============================ MEMBER FUNCTIONS =============================
//

// ---------------------------------------------------------------------------
// CFotaSrvUi::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CFotaSrvUi::ConstructL()
    {
    FLOG( _L( "[FotaServer] CFotaSrvUi::ConstructL:" ) );
    TInt        flags = EStandardApp|EAknEnableSkin|EAknEnableMSK;
    CAknAppUi::BaseConstructL( flags );
    CFotaSrvApp* app = (CFotaSrvApp*) Application();
    app->SetUIVisibleL(EFalse);
    iNeedToClose = EFalse;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CFotaSrvUi::~CFotaSrvUi()
    {
	}

// ---------------------------------------------------------------------------
// CFotaSrvUi::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CFotaSrvUi::DynInitMenuPaneL( TInt /*aResourceId*/,
                                         CEikMenuPane* /*aMenuPane*/ )
    {
    FLOG( _L( "[FotaServer] CFotaSrvUi::DynInitMenuPaneL:" ) );
    }

// ---------------------------------------------------------------------------
// CFotaSrvUi::HandleKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CFotaSrvUi::HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                            	TEventCode aType )
    {
    FLOG(_L( "[FotaServer] CFotaSrvUi::HandleKeyEventL: %d  %d" ), aType
            , aKeyEvent.iCode ); 
    if (aType == EEventKey && aKeyEvent.iCode == EKeyEscape )
    	{
    	FLOG(_L("Application requested to shutdown..."));
        CFotaSrvApp* app = (CFotaSrvApp*) Application();
        CFotaServer* server = app->Server();

    	server->iNeedToClose = ETrue;
    	}
    return EKeyWasNotConsumed;
	}

// ---------------------------------------------------------------------------
// CFotaSrvUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CFotaSrvUi::HandleCommandL( TInt aCommand )
    {
    FLOG(_L( "[FotaServer] CFotaSrvUi::HandleCommandL >> %d" ),aCommand  );
    switch ( aCommand )
        {
	    case EAknCmdExit:
        case EEikCmdExit:		// quit application
		    {
            
	        CFotaSrvApp* app = (CFotaSrvApp*) Application();
	        CFotaServer* server = app->Server();
	        if (!server->GetDEOperation())
	            {
	            FLOG(_L("Exitting FotaServer!!"));
	            server->iNeedToClose = ETrue;
	            Exit();
	            }
	        else
	            {
	            FLOG(_L("Defering exit!"));
	            }
		    }
		    break;

        default:
            break;      
        }
    FLOG(_L( "[FotaServer] CFotaSrvUi::HandleCommandL << %d" ),aCommand  );
    }

// ---------------------------------------------------------------------------
// CIVAppUi::OpenFileL
// This is called by framework when application is already open in background
// and user open other file in eg. FileBrowse.
// New file to been shown is passed via aFileName.
// ---------------------------------------------------------------------------
//
void CFotaSrvUi::OpenFileL(const TDesC& aFileName)
    {
    FLOG(_L("CFotaSrvUi::OpenFileL(%S)"),&aFileName);
    }


// ---------------------------------------------------------------------------
// CFotaAppUi::PrepareToExit
// This is called by framework when application is about to exit
// and server can prepare to stop any active downloads
// ---------------------------------------------------------------------------
//
void CFotaSrvUi::PrepareToExit()
    {
    FLOG(_L("CFotaSrvUi::PrepareToExit >>"));
	
    CFotaSrvApp* app = (CFotaSrvApp*) Application();
    CFotaServer* server = app->Server();
    server->iNeedToClose = ETrue;

    FLOG(_L("CFotaSrvUi::PrepareToExit <<"));
    }
