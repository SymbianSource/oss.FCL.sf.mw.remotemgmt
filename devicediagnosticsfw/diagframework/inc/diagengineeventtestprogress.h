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
* Description:  Class declaration for DiagFwInternal::CEventTestProgress
*
*/


#ifndef DIAGENGINEEVENTTESTPROGRESS_H
#define DIAGENGINEEVENTTESTPROGRESS_H

// SYSTEM INCLUDES
#include <e32base.h>            // CBase

// USER INCLUDES
#include "diagengineevents.h"       // DiagFwInternal::TEvent
#include "diagengineeventbasic.h"   // DiagFwInternal::CEventBasic

namespace DiagFwInternal
    {
/**
* CEventTestProgress
*
* Event for test progress
*/
NONSHARABLE_CLASS( CEventTestProgress ) : public CEventBasic
    {
public:
    /** 
    * C++ Constructor
    *
    * @param aSender - Sender of the progress event
    * @param aCurrentStep - Current progress step
    * @param aTotalSteps - Total number of steps to complete
    */
    CEventTestProgress( const MDiagPlugin& aSender, 
                        TUint aCurrentStep, 
                        TUint aTotalSteps );

    /** 
    * Get Sender
    *
    * @return Reference to the plugin
    */
    const MDiagPlugin& Sender() const;

    /** 
    * Get progress
    *
    * @return progress
    */
    TUint CurrStep() const;

    /** 
    * Get total number of steps to complete
    *
    * @return total number of steps
    */
    TUint TotalSteps() const;

private:    // private data
    /**
    * iSender - Plug-in that sent in the progress
    */
    const MDiagPlugin& iSender;

    /**
    * iCurrStep - Current step.
    */
    TUint iCurrStep;

    /**
    * iTotalSteps - Total steps
    */
    TUint iTotalSteps;
    };
    } // namespace DiagFwInternal

#endif // ifndef DIAGENGINEEVENTTESTPROGRESS_H

// End of File

