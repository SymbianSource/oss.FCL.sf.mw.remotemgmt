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
* Description:  Base class for obex searchers
*
*/



// INCLUDE FILES
#include "ObexSearcherBase.h"
#include "Obexsearcherobserver.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CObexSearcherBase::CObexSearcherBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CObexSearcherBase::CObexSearcherBase() :
	CActive( CActive::EPriorityStandard ),
	iState( EIdle )
    {
    }

    
// Destructor
CObexSearcherBase::~CObexSearcherBase()
    {
    }

void CObexSearcherBase::BaseConstructL()
	{
	CActiveScheduler::Add( this );
	}

void CObexSearcherBase::SetObserver( MObexSearcherObserver* aObserver )
	{
	iObserver = aObserver;
	}

void CObexSearcherBase::SetExtObserver( MExtBTSearcherObserver* aExtObserver )
	{
	iExtObserver = aExtObserver;
	}

void CObexSearcherBase::NotifyDeviceFoundL()
	{
	iObserver->HandleDeviceFoundL();
	}

void CObexSearcherBase::NotifyDeviceErrorL( TInt aErr )
	{
	iObserver->HandleDeviceErrorL( aErr );
	}

void CObexSearcherBase::NotifyServiceFoundL()
	{
	iObserver->HandleServiceFoundL();
	}

void CObexSearcherBase::NotifyServiceErrorL( TInt aErr )
	{
	iObserver->HandleServiceErrorL( aErr );
	}

//  End of File  
