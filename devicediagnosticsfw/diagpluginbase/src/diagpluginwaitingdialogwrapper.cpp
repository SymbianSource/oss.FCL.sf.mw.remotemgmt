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
* Description:  Class definition of CDiagPluginWaitingDialogWrapper
*
*/


// CLASS DECLARATION
#include "diagpluginwaitingdialogwrapper.h"

// SYSTEM INCLUDE FILES
#include <AknDialog.h>                      // CAknDialog

// USER INCLUDE FILES
#include "diagpluginbase.pan"               // Panic Codes

// LOCAL TYPES

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagPluginWaitingDialogWrapper::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginWaitingDialogWrapper* CDiagPluginWaitingDialogWrapper::NewL( 
        CAknDialog* aWaitingDialog )
    {
    CleanupStack::PushL( aWaitingDialog );
    CDiagPluginWaitingDialogWrapper* self = new ( ELeave ) 
        CDiagPluginWaitingDialogWrapper( aWaitingDialog );
    CleanupStack::Pop( aWaitingDialog );

    return self;
    }

// ---------------------------------------------------------------------------
// CDiagPluginWaitingDialogWrapper::CDiagPluginWaitingDialogWrapper
// ---------------------------------------------------------------------------
//
CDiagPluginWaitingDialogWrapper::CDiagPluginWaitingDialogWrapper( 
        CAknDialog* aWaitingDialog )
    :   iWaitingDialog( aWaitingDialog ),
        iIsObjectDeletedPtr( NULL )
    {
    // Dialog must not be NULL
    __ASSERT_ALWAYS( iWaitingDialog, Panic( EDiagPluginBasePanicBadArgument ) );

    // Coverity Change .....  Dialog must have EEikDialogFlagWait flag set
    __ASSERT_ALWAYS( (iWaitingDialog->DialogFlags() & EEikDialogFlagWait),
                     Panic( EDiagPluginBasePanicBadArgument ) );
    }

// ---------------------------------------------------------------------------
// CDiagPluginWaitingDialogWrapper::~CDiagPluginWaitingDialogWrapper
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginWaitingDialogWrapper::~CDiagPluginWaitingDialogWrapper()
    {
    if( iIsObjectDeletedPtr )
        {
        // If this pointer is set, we are still in RunLD().
        __ASSERT_DEBUG( iWaitingDialog, Panic( EDiagPluginBasePanicInternal ) );
        
        *iIsObjectDeletedPtr = ETrue;
        iIsObjectDeletedPtr = NULL;

        delete iWaitingDialog;
        iWaitingDialog = NULL;
        }
    }

// ---------------------------------------------------------------------------
// CDiagPluginWaitingDialogWrapper::RunLD
// ---------------------------------------------------------------------------
EXPORT_C TBool CDiagPluginWaitingDialogWrapper::RunLD( TInt& aDialogResponse )
    {
    // Make sure that this function is not called twice.
    __ASSERT_ALWAYS( iIsObjectDeletedPtr == NULL,
        Panic( EDiagPluginBasePanicInvalidState ) );

    TBool isObjectDeleted = EFalse;
    
    iIsObjectDeletedPtr = &isObjectDeleted;

    aDialogResponse = iWaitingDialog->RunLD();
    // Do not access local variable until isObjectDeleted
    // value is checked first. "this" pointer may be invalid at this pointer.

    if ( isObjectDeleted )
        {
        // this class is already deleted. Exit immediately.
        return EFalse;
        }
    else
        {
        // iWaitingDialog->RunLD() returned with user response.
        iIsObjectDeletedPtr = NULL;
        iWaitingDialog = NULL;

        // Self-destruct as it is a "D" function.
        delete this;
        return ETrue;
        }
    }

// End of File

