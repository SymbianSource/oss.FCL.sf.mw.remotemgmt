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
* Description:  Class definition of DiagFwInternal::CEventTestProgress
*
*/


// CLASS DECLARATION
#include "diagengineeventtestprogress.h"   

// SYSTEM INCLUDE FILES

// USER INCLUDE FILES

using namespace DiagFwInternal;

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CEventTestProgress::CEventTestProgress()
// ---------------------------------------------------------------------------
//
CEventTestProgress::CEventTestProgress(
        const MDiagPlugin& aSender, TUint aCurrStep, TUint aTotalSteps ) 
    :   CEventBasic( EEventTestProgress ), 
        iSender( aSender ),
        iCurrStep( aCurrStep ),
        iTotalSteps( aTotalSteps )
    {
    }

// ---------------------------------------------------------------------------
// CEventTestProgress::Sender()
// ---------------------------------------------------------------------------
//
const MDiagPlugin& CEventTestProgress::Sender() const
    {
    return iSender;
    }

// ---------------------------------------------------------------------------
// CEventTestProgress::CurrStep()
// ---------------------------------------------------------------------------
//
TUint CEventTestProgress::CurrStep() const
    {
    return iCurrStep;
    }

// ---------------------------------------------------------------------------
// CEventTestProgress::TotalSteps()
// ---------------------------------------------------------------------------
//
TUint CEventTestProgress::TotalSteps() const
    {
    return iTotalSteps;
    }

// End of File

