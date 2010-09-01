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
* Description:  Class declaration for DiagFwInternal::TState
*
*/



#ifndef DIAGENGINESTATES_H
#define DIAGENGINESTATES_H

namespace DiagFwInternal
    {
enum TState
    {
    EStateAny               = -1,
    EStateNotReady,
    EStateCreatingPlan,
    EStateRunning,
    EStateCancelAll,
    EStateSuspended,
    EStateFinalizing,
    EStateStopped
    };
    } // end of namespace DiagFwInternal

#endif // DIAGENGINESTATES_H

// End of File

