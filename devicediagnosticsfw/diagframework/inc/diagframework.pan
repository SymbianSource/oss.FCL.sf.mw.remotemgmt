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
* Description:  Enum definition of Framdwork Panic Codes
*
*/


#ifndef DIAGFRAMEWORK_PAN
#define DIAGFRAMEWORK_PAN

/**
* Panic Codes for Diagnostics Framework
* 
* @since 5.0
*/
enum TDiagFrameworkPanics
    {
    EDiagFrameworkBadArgument           = 1,    // Invalid input parameter
    EDiagFrameworkInvalidState          = 2,    // Function called in incorrect state
    EDiagFrameworkInvalidEvent          = 3,    // Even not valid in current state.
    EDiagFrameworkCorruptStateMachine   = 4,    // State machine may be corrupted.
    EDiagFrameworkArrayBounds           = 5,    // Array out of bounds
    EDiagFrameworkCorruptPlugin         = 6,    // Plug-in information is corrupted.
    EDiagFrameworkNullTestResult        = 7,    // Test result is NULL
    EDiagFrameworkInternal              = 8,    // Other framework internal error
    EDiagFrameworkDb                    = 9,    // Database related errors.
    EDiagFrameworkPlugin                = 10    // General plug-in behavior error
    };

_LIT(KDiagFwCategory,"Diag Fw");

inline void Panic( TDiagFrameworkPanics aReason )
    {
    User::Panic( KDiagFwCategory, aReason );
    }

#endif // DIAGFRAMEWORK_PAN
