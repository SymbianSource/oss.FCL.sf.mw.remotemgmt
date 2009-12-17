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
* Implementation of CFotaCustCmdFirstBoot class.
*
*/

#include "fotacustcmdfirstboot.h"
#include "fotastartupDebug.h"
//#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CFotaCustCmdFirstBoot::NewL
// ---------------------------------------------------------------------------
//
CFotaCustCmdFirstBoot* CFotaCustCmdFirstBoot::NewL()
	{
   FLOG( _L( "[CFotaCustCmdFirstBoot::NewL()  " ) );
	return new ( ELeave ) CFotaCustCmdFirstBoot;
	}


// ---------------------------------------------------------------------------
// CFotaCustCmdFirstBoot::~CFotaCustCmdFirstBoot
// ---------------------------------------------------------------------------
//
CFotaCustCmdFirstBoot::~CFotaCustCmdFirstBoot()
    {
      FLOG( _L( "[CFotaCustCmdFirstBoot::~CFotaCustCmdFirstBoot()  " ) );
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirstBoot::Initialize
// ---------------------------------------------------------------------------
//
TInt CFotaCustCmdFirstBoot::Initialize( CSsmCustomCommandEnv* /*aCmdEnv*/ )
    {
  
  FLOG( _L( "[CFotaCustCmdFirstBoot::Initialize()  " ) );

    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirstBoot::Execute
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirstBoot::Execute(
    const TDesC8& /*aParams*/,
    TRequestStatus& aRequest )
    {
    	     aRequest = KRequestPending;
   FLOG( _L( "[CFotaCustCmdFirstBoot::Execute()  " ) );
    #ifdef __PLUG_AND_PLAY_MOBILE_SERVICES 
      FLOG( _L( "NOTHING TO DO   " ) );
    #endif
      TRequestStatus* request = &aRequest;
      User::RequestComplete( request, KErrNone );

    
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirstBoot::ExecuteCancel
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirstBoot::ExecuteCancel()
    {
      FLOG( _L( "[CFotaCustCmdFirstBoot::ExecuteCancel()  " ) );

    
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirstBoot::Close
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirstBoot::Close()
    {
     FLOG( _L( "[CFotaCustCmdFirstBoot::Close()  " ) );
    }


// ---------------------------------------------------------------------------
// CFotaCustCmdFirstBoot::Release
// ---------------------------------------------------------------------------
//
void CFotaCustCmdFirstBoot::Release()
    {
     FLOG( _L( "[CFotaCustCmdFirstBoot::Release()  " ) );

	delete this;
    }
