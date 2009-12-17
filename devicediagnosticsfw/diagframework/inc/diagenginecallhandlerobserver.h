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
* Description:  Class declaration for MDiagEngineCallHandlerObserver
*
*/


#ifndef DIAGENGINECALLHANDLEROBSERVER_H
#define DIAGENGINECALLHANDLEROBSERVER_H

#include "diagenginecallhandlerstates.h"    // TDiagEngineCallHandlerState

class MDiagEngineCallHandlerObserver
    {
public:
    /**
    * Notify call state change
    * @param aState - EDiagEngineCallHandlerStateIdle if not in call.
    *                 EDiagEngineCallHandlerStateBusy if in call.
    */
    virtual void CallHandlerStateChangedL( TDiagEngineCallHandlerState aState ) = 0;
    };

#endif // DIAGENGINECALLHANDLEROBSERVER_H

// End of File

