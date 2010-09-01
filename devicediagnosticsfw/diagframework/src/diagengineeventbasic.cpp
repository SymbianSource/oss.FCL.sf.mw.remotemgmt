/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition of DiagFwInternal::CEventBasic
*
*/


// CLASS DECLARATION
#include "diagengineeventbasic.h"       

// SYSTEM INCLUDE FILES
#include <DiagFrameworkDebug.h>

// USER INCLUDE FILES
#include "diagframework.pan"            // Panic Codes.

using namespace DiagFwInternal;

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEventBasic::CEventBasic()
// ---------------------------------------------------------------------------
//
CEventBasic::CEventBasic( TEvent aType )
    :   iType( aType )
    {
    }

// ---------------------------------------------------------------------------
// CEventBasic::GetType()
// ---------------------------------------------------------------------------
//
TEvent CEventBasic::GetType() const
    {
    return iType;
    }

// ---------------------------------------------------------------------------
// CEventBasic::ToString()
// ---------------------------------------------------------------------------
//
const TDesC& CEventBasic::ToString() const
    {
    #if _DEBUG
        _LIT( KEventExecute,             "EEventExecute" );
        _LIT( KEventPlanCreated,         "EEventPlanCreated" );
        _LIT( KEventExecuteNext,         "EEventExecuteNext" );
        _LIT( KEventTestProgress,        "EEventTestProgress" );
        _LIT( KEventResumeToRunning,     "EEventResumeToRunning" );
        _LIT( KEventResumeToCreatingPlan,"EEventResumeToCreatingPlan" );
        _LIT( KEventSkip,                "EEventSkip" );
        _LIT( KEventCancelAll,           "EEventCancelAll" );
        _LIT( KEventSuspend,             "EEventSuspend" );
        _LIT( KEventVoiceCallActive,     "EEventVoiceCallActive" );
        _LIT( KEventAllPluginsCompleted, "EEventAllPluginsCompleted" );
        _LIT( KEventFinalized,           "EEventFinalized" );

        switch ( iType )
            {
            case EEventExecute:
                return KEventExecute();
            case EEventPlanCreated:
                return KEventPlanCreated();
            case EEventExecuteNext:
                return KEventExecuteNext();
            case EEventTestProgress:
                return KEventTestProgress();
            case EEventResumeToRunning:
                return KEventResumeToRunning();
            case EEventResumeToCreatingPlan:
                return KEventResumeToCreatingPlan();
            case EEventSkip:
                return KEventSkip();
            case EEventCancelAll:
                return KEventCancelAll();
            case EEventSuspend:
                return KEventSuspend();
            case EEventVoiceCallActive:
                return KEventVoiceCallActive();
            case EEventAllPluginsCompleted:
                return KEventAllPluginsCompleted();
            case EEventFinalized:
                return KEventFinalized();
            default:
                Panic( EDiagFrameworkInternal );
                break;
            }


    #endif // if _DEBUG
    
    _LIT( KEventDefaultName, "?" );
    return KEventDefaultName();
    }

// End of File

