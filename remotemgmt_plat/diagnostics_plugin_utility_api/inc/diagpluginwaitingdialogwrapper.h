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
* Description:  Provides wrapper for waiting dialogs.
*
*/


#ifndef DIAGPLUGINWAITINGDIALOGWRAPPER_H
#define DIAGPLUGINWAITINGDIALOGWRAPPER_H

// INCLUDES
#include <e32base.h>                // CBase

// FORWARD DECLARATIONS
class CAknDialog;

/**
*  Diagnostics Plugin Waiting Dialog Wrapper.
*
*   This class provides a wrapper for executing dialogs. The only functionality
*   it adds is that it provides a way for the caller to find out whether
*   the dialog was dismissed by the user response, or by application deletion.
*
*  @since S60 v5.0
*/
class CDiagPluginWaitingDialogWrapper : public CBase
    {
public: // new API
    /**
    * Two-phased constructor.
    *
    *   @param - aWaitingDialog.  Pointer to dialog to display. Note that 
    *       it must be a dialog with EEikDialogFlagWait set. Otherwise, it 
    *       will panic with EDiagPluginBasePanicBadArgument
    *       Ownership is transferred to the wrapper.
    *   @return - New instance of CDiagPluginWaitingDialogWrapper
    */
    IMPORT_C static CDiagPluginWaitingDialogWrapper* NewL( CAknDialog* aWaitingDialog );
    
    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDiagPluginWaitingDialogWrapper();

    /**
    * Run a dialog that waits for response. 
    *
    * To dismiss the dialog, simply delete CDiagPluginWaitingDialogWrapper instance.
    *
    * The difference from normal dialog RunLD is that this function returns 
    * ETrue if dialog exited due to user response and 
    * EFalse if the object was deleted.
    * 
    *   !!!! NOTE THAT PLUG-IN MUST RETURN IMMEDIATELY WITHOUT ACCESSING  !!!!
    *   !!!! LOCAL VARIABLE OR LOCAL FUNCITONS WHEN THIS FUNCTION RETURNS !!!!
    *   !!!! EFalse VALUE BECAUSE "THIS" POINTER MAY BE FREED ALREADY.    !!!!
    * 
    * The normal dialog response is returned via aDialogResponse parameter.
    *
    * @param aDialogResponse - Response from the dialog.  
    *   For detailed values @see avkon.hrh "CBA constants". E.g. EAknSoftkeyYes
    *   The only exception is that if cancel is pressed, it will be 0.
    * @return ETrue if dialog is exiting due to user response.
    *   Efalse if dialog is dismissed without user input. 
    *   If EFalse is returned, plug-in MUST NOT act on the response.
    */
    IMPORT_C TBool RunLD( TInt& aDialogResponse );

private:    // private interface
    /**
    * C++ Constructor
    *   
    *   @param - aWaitingDialog.  Pointer to dialog to display. Note that 
    *       it must be a dialog with EEikDialogFlagWait set. Otherwise, it 
    *       will panic with EDiagPluginBasePanicBadArgument
    *       Ownership is transferred to the wrapper.
    */
    CDiagPluginWaitingDialogWrapper( CAknDialog* aWaitingDialog );

private:    // Private Data
    /**
    * Pointer to the dialog being displayed.
    * Ownership: Shared. Normally, dialog will dismiss itself. However,
    * if plug-in is being deleted, it can be deleted by the plug-in as well.
    */
    CAknDialog* iWaitingDialog;

    /**
    * Indicates whether this class is deleted by caller.
    * Ownership: Not owned. Normally, it will point to a stack entry.
    */
    TBool* iIsObjectDeletedPtr;
    };

#endif // DIAGTESTPLUGINBASE_H

// End of File

