/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
* SyncInitiationNotifier.cpp
*/

// INCLUDE FILES
#include <eikenv.h>
#include <eikappui.h>
#include <centralrepository.h>
#include <NsmlOperatorErrorCRKeys.h>
#include <bautils.h>				// BaflUtils
#include <utf.h>
#include <APGWGNAM.H> //CApaWindowGroupName
#include <APGTASK.H>

#include "SyncInitiationNotifier.h"
#include "CenrepUtils.h"
 


// ================= MEMBER FUNCTIONS =========================================

CSyncInitiationNotifier* CSyncInitiationNotifier::NewL()
    {
    CSyncInitiationNotifier* self = CSyncInitiationNotifier::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CSyncInitiationNotifier* CSyncInitiationNotifier::NewLC()
    {
    CSyncInitiationNotifier* self = new ( ELeave ) CSyncInitiationNotifier;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CSyncInitiationNotifier::~CSyncInitiationNotifier()
	{
	if( IsActive() )
		{
        Cancel();
        if( iRep )
            {
            iRep->NotifyCancelAll();
            }
		}
	delete iRep;
	iFs.Close();
	}

CSyncInitiationNotifier::CSyncInitiationNotifier(): 
    CActive( CActive::EPriorityStandard )
	{
    CActiveScheduler::Add( this );  
	}

void CSyncInitiationNotifier::ConstructL( )
    {
    iRep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys );
    ListenForKeyNotifications();
	}


// ----------------------------------------------------------------------------
// CSyncInitiationNotifier::ListenForKeyNotifications
// ----------------------------------------------------------------------------
void CSyncInitiationNotifier::ListenForKeyNotifications()
    {      
	iRep->NotifyRequest( KNsmlOpDsSyncInitiation, iStatus );
    SetActive();
    }

void CSyncInitiationNotifier::DoCancel()
	{
	iRep->NotifyCancelAll(); 
	}

void CSyncInitiationNotifier::RunL()
	{
	if( iStatus > 0 )
		{
		CCenrepUtils::WriteLogL( KNsmlOpDsSyncInitiation, ESyncInitiation );

		TApaTask task(CEikonEnv::Static()->WsSession( ));
		task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
		task.BringToForeground(); 

		ListenForKeyNotifications();
		}
	}

// End of File
