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
* Description:  Main class for handling provisioning message
*
*/



// INCLUDE FILES
#include "CWPMessage.h"
#include "CWPHandler.h"
#include "WPHandlerUtil.h"
#include "WPHandlerDebug.h"
#include "ProvisioningDebug.h"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPHandler::CWPHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPHandler::CWPHandler()
    : CContentHandlerBase()
    {
    }

// -----------------------------------------------------------------------------
// CWPHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPHandler::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CWPHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPHandler* CWPHandler::NewL()
    {
    CWPHandler* self = new(ELeave) CWPHandler; 
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CWPHandler::~CWPHandler()
    {
    }

// -----------------------------------------------------------------------------
// CWPHandler::HandleMessageL
// -----------------------------------------------------------------------------
//
void CWPHandler::HandleMessageL(CPushMessage* aPushMsg, TRequestStatus& aStatus)
    {
    FLOG( _L( "CWPHandler::HandleMessageL: Received message" ) );
    
    iMessage = aPushMsg;
    iAcknowledge = ETrue;
    SetConfirmationStatus(aStatus);

    iState = EProcessing;
    IdleComplete();
    }

// -----------------------------------------------------------------------------
// CWPHandler::HandleMessageL
// -----------------------------------------------------------------------------
//
void CWPHandler::HandleMessageL(CPushMessage* aPushMsg)
    {
    FLOG( _L( "CWPHandler::HandleMessageL: Received message" ) );

    iAcknowledge = EFalse;
    iMessage = aPushMsg;
    
    iState = EProcessing;
    IdleComplete();
    }

// -----------------------------------------------------------------------------
// CWPHandler::CancelHandleMessage
// -----------------------------------------------------------------------------
//
void CWPHandler::CancelHandleMessage()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CWPHandler::CPushHandlerBase_Reserved1
// -----------------------------------------------------------------------------
//
void CWPHandler::CPushHandlerBase_Reserved1()
    {
    }

// -----------------------------------------------------------------------------
// CWPHandler::CPushHandlerBase_Reserved2
// -----------------------------------------------------------------------------
//
void CWPHandler::CPushHandlerBase_Reserved2()
    {
    }

// -----------------------------------------------------------------------------
// CWPHandler::DoCancel
// -----------------------------------------------------------------------------
//
void CWPHandler::DoCancel()
    {
    Complete(KErrCancel);
    }

// -----------------------------------------------------------------------------
// CWPHandler::RunL
// -----------------------------------------------------------------------------
//
void CWPHandler::RunL()
    {
    // use active state machine routine to manage activites:
    switch(iState)
        {
        case EProcessing:
            {
            ProcessingPushMsgEntryL();
            break;
            }
        case EDone:
            {
            Complete(KErrNone);
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CWPHandler::RunError
// -----------------------------------------------------------------------------
//
TInt CWPHandler::RunError(TInt aError)
    {
    FLOG( _L( "CWPHandler::RunError" ) );
    FTRACE(RDebug::Print(_L("[Provisioning] CWPHandler::RunError (%d)"), aError));

    iState = EDone;
    Complete(aError);
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWPHandler::ProcessingPushMsgEntryL
// -----------------------------------------------------------------------------
//
void CWPHandler::ProcessingPushMsgEntryL()
    {
    FLOG( _L( "CWPHandler::ProcessingPushMsgEntryL: Processing message" ) );

    CWPMessage* message = CWPMessage::NewL( *iMessage );
    CleanupStack::PushL( message );
    message->ProcessL();
    CleanupStack::PopAndDestroy();

    FLOG( _L( "CWPHandler::ProcessingPushMsgEntryL: Processed message" ) );
        
    iState = EDone;
    IdleComplete();
    }

//  End of File  
