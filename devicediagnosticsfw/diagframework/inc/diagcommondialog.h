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
* Description:  List of common dialog types
*
*/


#ifndef DIAGCOMMONDIALOG_H
#define DIAGCOMMONDIALOG_H


/**
*  Diagnostics Common Dialog Types
*
*  This is a list of common dialogs that can be used by plug-ins.
* 
*/
enum TDiagCommonDialog
    {
    /**
    * EDiagCommonDialogConfirmCancelAll
    * Dialog that asks the user if all tests should be cancelled or not.
    *
    * aInitData:       No init data needed.  Always pass NULL.
    * Return:          Standard dialog.  Use CAknDialog.
    * Execution:       Call RunLD().
    * RunLD() returns: - EAknSoftkeyYes, if the user confirmed the request to
    *                    cancel all test execution.  Plugins should not do
    *                    any further execution and should return immediately.
    *                  - EAknSoftkeyNo, if the user dismissed the dialog and
    *                    decided not to cancel all test execution.
    *                  - 0, if the dialog is forced closed.  Generally,
    *                    plugins should not distinguish between EAknSoftkeyNo
    *                    and 0 (i.e., they should *only* check for
    *                    EAknSoftkeyYes), unless there is good reason to.
    *
    * @code
    *
    *    CAknDialog* dlg = ExecutionParam().Engine().CreateCommonDialogLC(
    *        EDiagCommonDialogConfirmCancelAll,
    *        NULL );
    *
    *    TInt result = 0;
    *    if ( !RunWaitingDialogL( dlg, result ) ||
    *         result == EAknSoftkeyYes )
    *        {
    *        // Do not continue executing.  Either the dialog has been deleted
    *        // or the user has cancelled all execution.
    *        return;
    *        }
    *
    *    dlg = NULL;
    *
    *    // Loop and display the plugin query again.
    *    askAgain = ETrue;
    *
    */
    EDiagCommonDialogConfirmCancelAll = 1,
    EDiagCommonDialogConfirmSkipAll =2
    };

#endif // DIAGCOMMONDIALOG_H

// End of File

