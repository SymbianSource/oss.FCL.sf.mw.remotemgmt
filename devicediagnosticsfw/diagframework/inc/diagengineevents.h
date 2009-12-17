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
* Description:  Class declaration for DiagFwInternal::TEvent
*
*/


#ifndef DIAGENGINEEVENTS_H
#define DIAGENGINEEVENTS_H

// SYSTEM INCLUDES
#include <DiagResultsDatabaseItem.h>    // CDiagResultsDatabaseItem
#include <DiagEngineCommon.h>           // MDiagEngineCommon

class MDiagPlugin;
class MDiagTestPlugin;
class MDiagSuitePlugin;

namespace DiagFwInternal
    {
typedef enum
    {
    EEventExecute,              // ::ExecuteTestL() is called.
    EEventPlanCreated,          // Execution plan is created.
    EEventExecuteNext,          // Execute next item in plan.
    EEventTestProgress,         // ::TestProgressL is called.
    EEventAllPluginsCompleted,  // All plug-ins are completed.
    EEventResumeToRunning,      // Resume to Runinng state from suspend
    EEventResumeToCreatingPlan, // Resume to Creating plan from suspend
    EEventSkip,                 // ::ExecutionStopL w/ Skip called
    EEventCancelAll,            // ::ExecutionStopL w/ CancelAll called
    EEventSuspend,              // ::SuspendL called.
    EEventVoiceCallActive,      // MO / MT call
    EEventFinalized             // db record and plug-ins are finalized.
    } TEvent;
    } // end of namespace DiagFwInternal

#endif // DIAGENGINEEVENTS_H

// End of File

