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


#ifndef DIAGPLUGINBASE_PAN
#define DIAGPLUGINBASE_PAN

/**
* Panic Codes for Diagnostics PluginBase
* 
* @since 5.0
*/
enum TDiagPluginBasePanics
    {
    EDiagPluginBasePanicConstruction     = 1,   // Failed to construct
    EDiagPluginBasePanicBadArgument      = 2,   // Invalid input parameter
    EDiagPluginBasePanicDialogAlreadyUp  = 3,   // Another dialog is already being displayed.
    EDiagPluginBasePanicInternal         = 4,   // Base class internal error
    EDiagPluginBasePanicInvalidState     = 5    // Function called in invalid state.
    };


_LIT( KDiagPluginBaseCategory, "Diag Plugin Base" );

inline void Panic( TDiagPluginBasePanics aReason )
    {
    User::Panic( KDiagPluginBaseCategory, aReason );
    }

#endif // DIAGPLUGINBASE_PAN

// End of File

