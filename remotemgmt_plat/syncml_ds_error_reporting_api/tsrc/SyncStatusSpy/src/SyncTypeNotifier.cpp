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
* SyncTypeNotifier.cpp
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

#include "SyncTypeNotifier.h"
#include "CenrepUtils.h"
 


// ================= MEMBER FUNCTIONS =========================================

CSyncTypeNotifier* CSyncTypeNotifier::NewL()
    {
    CSyncTypeNotifier* self = CSyncTypeNotifier::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CSyncTypeNotifier* CSyncTypeNotifier::NewLC()
    {
    CSyncTypeNotifier* self = new ( ELeave ) CSyncTypeNotifier;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CSyncTypeNotifier::~CSyncTypeNotifier()
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

CSyncTypeNotifier::CSyncTypeNotifier(): 
    CActive( CActive::EPriorityStandard )
	{
    CActiveScheduler::Add( this );  
	}

void CSyncTypeNotifier::ConstructL( )
    {
    iRep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys );
    ListenForKeyNotifications();
	}


// ----------------------------------------------------------------------------
// CSyncTypeNotifier::ListenForKeyNotifications
// ----------------------------------------------------------------------------
void CSyncTypeNotifier::ListenForKeyNotifications()
    {      
	iRep->NotifyRequest( KNsmlOpDsSyncType, iStatus );
    SetActive();
    }

void CSyncTypeNotifier::DoCancel()
	{
	iRep->NotifyCancelAll(); 
	}

void CSyncTypeNotifier::RunL()
	{
	if( iStatus > 0 )
		{
		CCenrepUtils::WriteLogL( KNsmlOpDsSyncType, ESyncType );

		TApaTask task(CEikonEnv::Static()->WsSession( ));
		task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
		task.BringToForeground(); 

		ListenForKeyNotifications();
		}
	}

// End of File
