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
* ErrorCodeNotifier.cpp
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

#include "ErrorCodeNotifier.h"
#include "CenrepUtils.h"



// ================= MEMBER FUNCTIONS =========================================

CErrorCodeNotifier* CErrorCodeNotifier::NewL()
    {
    CErrorCodeNotifier* self = CErrorCodeNotifier::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

CErrorCodeNotifier* CErrorCodeNotifier::NewLC()
    {
    CErrorCodeNotifier* self = new ( ELeave ) CErrorCodeNotifier;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CErrorCodeNotifier::~CErrorCodeNotifier()
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

CErrorCodeNotifier::CErrorCodeNotifier(): 
    CActive( CActive::EPriorityStandard )
	{
    CActiveScheduler::Add( this );
	}

void CErrorCodeNotifier::ConstructL( )
    {
    iRep = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys );
    ListenForKeyNotifications();
	}


// ----------------------------------------------------------------------------
// CErrorCodeNotifier::ListenForKeyNotifications
// ----------------------------------------------------------------------------
void CErrorCodeNotifier::ListenForKeyNotifications()
    {
	iRep->NotifyRequest( KNsmlOpDsErrorCode, iStatus );
    SetActive();
    }

void CErrorCodeNotifier::DoCancel()
	{
	iRep->NotifyCancelAll();
	}

void CErrorCodeNotifier::RunL()
	{
	if( iStatus > 0 )
		{
		CCenrepUtils::WriteLogL( KNsmlOpDsErrorCode, EErrorCode );

		TApaTask task(CEikonEnv::Static()->WsSession( ));
		task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
		task.BringToForeground();

		ListenForKeyNotifications();
		}
	}

// End of File
