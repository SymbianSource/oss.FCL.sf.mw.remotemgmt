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
* Description:   Methods for CFotaSrvApp
*
*/



// INCLUDE FILES
#include <apgwgnam.h>
#include <apgcli.h>
#include "FotaSrvApp.h"
#include "FotaSrvDocument.h"
#include "FotaSrvDebug.h"
#include "FotaServer.h"
#include "fotaserverPrivatePSKeys.h"
#include <apgtask.h>

// ---------------------------------------------------------------------------
// CFotaSrvApp::CFotaSrvApp
// ---------------------------------------------------------------------------
CFotaSrvApp::CFotaSrvApp() : iFotaServer(0), iDocument(0)
    {
    }

// ---------------------------------------------------------------------------
// CFotaSrvApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------------------------
//
TUid CFotaSrvApp::AppDllUid() const
    {
    TUid fotauid;
    fotauid.iUid =  KFotaServerUid;
    return fotauid;
    }

// --------------------------------------------------------------------------
// CFotaSrvApp::SetUIVisibleL
// Turns UI visible or hidden
// --------------------------------------------------------------------------
//
void CFotaSrvApp::SetUIVisibleL( const TBool aVisible, const TBool aAlsoFSW )
    {
    CFotaSrvUi*  ui = (CFotaSrvUi*)iDocument->AppUi(); //CFotaSrvUi

    FLOG(_L("[CFotaSrvApp]  CFotaSrvApp::SetUIVisibleL %d alsoFSW:%d")
    	,aVisible?1:0,aAlsoFSW?1:0);
    CCoeEnv*    coe = CCoeEnv::Static();

    TApaTask                task = TApaTask(coe->WsSession());

    task.SetWgId(coe->RootWin().Identifier());
    
    if ( aVisible )
        {
        //Launch DM UI Application
        TApaTaskList taskList(coe->WsSession());
        TApaTask task1(taskList.FindApp( TUid::Uid(KOmaDMAppUid)));
        FLOG(_L("Checking whether DM UI is running..."));
        TInt value (EFotaUpdateDM);
        if (task1.Exists())
	        {
   		    FLOG(_L("Bring DM UI into FG")); 
	        task1.BringToForeground(); //DM UI
	        }
        else
        	{
TInt err = RProperty::Get( KPSUidNSmlDMSyncApp, KNSmlCurrentFotaUpdateAppName, value );

    			if (value != EFotaUpdateNSC && value != EFotaUpdateNSCBg )
					{
						FLOG(_L("Launching DM UI into FG"));
        		RApaLsSession apaLsSession;
      			User::LeaveIfError(apaLsSession.Connect());
      			TThreadId thread;
           	User::LeaveIfError( apaLsSession.StartDocument(KNullDesC, TUid::Uid( KOmaDMAppUid), thread) );
           	apaLsSession.Close();
					}				     
        	}
        if(value == EFotaUpdateDM)
        {
        	if ( aAlsoFSW ) ui->HideApplicationFromFSW( EFalse );
				}
        else if (value == EFotaUpdateNSC || value == EFotaUpdateNSCBg )
        {
        	if ( aAlsoFSW ) ui->HideApplicationFromFSW( ETrue );
        }
				FLOG(_L("Bring FOTA into FG")); 
      	task.BringToForeground();
      }  
    else
        {
        if ( aAlsoFSW ) ui->HideApplicationFromFSW( ETrue );

        task.SendToBackground();      // the task whose wingroup is at next 
                                      // ordinal position is brought to 
                                      // foreground

        }
    }

// ---------------------------------------------------------------------------
// CFotaSrvApp::~CFotaSrvApp()
// ---------------------------------------------------------------------------
//
CFotaSrvApp::~CFotaSrvApp()
    {
    }

// ---------------------------------------------------------------------------
// CFotaSrvApp::CreateDocumentL()
// Creates FotaSrvDocument object
// ---------------------------------------------------------------------------
//
CApaDocument* CFotaSrvApp::CreateDocumentL()
    {
    #ifdef _FOTA_DEBUG
    RDebug::Print( _L( "[FotaServer] CFotaSrvApp::CreateDocumentL:" ) );
    #endif 
    iDocument = CFotaSrvDocument::NewL( *this );
    return iDocument;
    }

#include <eikstart.h>

// ---------------------------------------------------------------------------
// NewApplication() 
// Constructs CFotaSrvApp
// Returns: CApaDocument*: created application object
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    #ifdef _FOTA_DEBUG
    RDebug::Print( _L( "[FotaServer] CApaApplication* NewApplication:" ) );
    #endif 
    return new CFotaSrvApp;
    }

// ---------------------------------------------------------------------------
// ServerL() 
// Getter for server
// ---------------------------------------------------------------------------
//
CFotaServer* CFotaSrvApp::Server()
    {
    return iFotaServer;
    }

// ---------------------------------------------------------------------------
// NewAppServerL() 
// Called by framwork
// ---------------------------------------------------------------------------
//
void CFotaSrvApp::NewAppServerL( CApaAppServer*& aAppServer )
	{
    #ifdef _FOTA_DEBUG
	RDebug::Print( _L( "[FotaServer] CFotaSrvApp::NewAppServerL:" ) );
    #endif 
    iFotaServer =  new (ELeave) CFotaServer();
    iFotaServer->iParentApp = this;
    aAppServer  = iFotaServer;
	}


// ---------------------------------------------------------------------------
// E32Main
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    TInt err;
    RProcess pr; TFullName fn = pr.FullName(); 
    #ifdef _FOTA_DEBUG
    RDebug::Print(_L("[FotaServer] E32Main:    >> called by %S"), &fn);
    #endif
    err = EikStart::RunApplication( NewApplication );
    #ifdef _FOTA_DEBUG
    RDebug::Print(_L("[FotaServer] E32Main:    <<"));
    #endif
    return err;
    }

// End of File
