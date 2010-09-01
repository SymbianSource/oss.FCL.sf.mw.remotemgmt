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
* ProfileIdNotifier.cpp
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

#include "ProfileIdNotifier.h"
#include "CenrepUtils.h"
 


// ================= MEMBER FUNCTIONS =========================================

CProfileIdNotifier* CProfileIdNotifier::NewL()
    {
    CProfileIdNotifier* self = CProfileIdNotifier::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CProfileIdNotifier* CProfileIdNotifier::NewLC()
    {
    CProfileIdNotifier* self = new ( ELeave ) CProfileIdNotifier;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CProfileIdNotifier::~CProfileIdNotifier()
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

CProfileIdNotifier::CProfileIdNotifier(): 
    CActive( CActive::EPriorityStandard )
	{
    CActiveScheduler::Add( this );  
	}

void CProfileIdNotifier::ConstructL( )
    {
    iRep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys );
    ListenForKeyNotifications();
	}


// ----------------------------------------------------------------------------
// CProfileIdNotifier::ListenForKeyNotifications
// ----------------------------------------------------------------------------
void CProfileIdNotifier::ListenForKeyNotifications()
    {      
	iRep->NotifyRequest( KNsmlOpDsSyncProfUid, iStatus );
    SetActive();
    }

void CProfileIdNotifier::DoCancel()
	{
	iRep->NotifyCancelAll(); 
	}

void CProfileIdNotifier::RunL()
	{
	if( iStatus > 0 )
		{
		CCenrepUtils::WriteLogL( KNsmlOpDsSyncProfUid, EProfileId );

		TApaTask task(CEikonEnv::Static()->WsSession( ));
		task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
		task.BringToForeground(); 

		ListenForKeyNotifications();
		}
	}

// End of File
