/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Methods for SyncML notifier Ecom Plugins entry point
*
*/


// INCLUDE FILES
#include <implementationproxy.h>
#include <AknNotifierWrapper.h> // link against aknnotifierwrapper.lib

#include "SyncMLNotifierBase.h"
#include "SyncMLNotifDebug.h"


// CONSTANTS
const TInt KMaxSynchReplyBufLength = 256;

const TInt KSmlNAppLaunchPriority = MEikSrvNotifierBase2::ENotifierPriorityVHigh;
const TInt KSmlNDlgPriority       = MEikSrvNotifierBase2::ENotifierPriorityHigh;
const TInt KSmlNFwUpdPriority     = MEikSrvNotifierBase2::ENotifierPriorityHigh;

#ifdef __SERIES60_30__
    const TUid KSmlNAppServerUid = { 0x102072bf }; 
#else // __SERIES60_30__
    _LIT( KSmlSyncMLNotifierDll, "syncmlnotifier.dll" );
#endif // __SERIES60_30__


// Provides pointer to the notifier creation function.
const TImplementationProxy ImplementationTable[] =
	{
#ifdef __EABI__
	{{0x101F876A},(TFuncPtr)NotifierArray}
#else
	{{0x101F876A},NotifierArray}
#endif
	};

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CleanupArray
// Deletes notifiers
// -----------------------------------------------------------------------------
void CleanupArray( TAny* aArray )
    {
    FLOG(_L("[SmlNotif]\t CleanupArray()"));
    
    CArrayPtrFlat<MEikSrvNotifierBase2>*     
        subjects = static_cast<CArrayPtrFlat<MEikSrvNotifierBase2>*>(aArray);
    TInt lastInd = subjects->Count() - 1;
    for ( TInt i = lastInd; i >= 0; i-- )
        {
        subjects->At(i)->Release();	    	    
        }
    delete subjects;
    
    FLOG(_L("[SmlNotif]\t CleanupArray() completed"));
    }

// -----------------------------------------------------------------------------
// DoCreateNotifierArrayL
// -----------------------------------------------------------------------------
CArrayPtr<MEikSrvNotifierBase2>* DoCreateNotifierArrayL()
    {
    FLOG(_L("[SmlNotif]\t DoCreateNotifierArrayL()"));
    CArrayPtrFlat<MEikSrvNotifierBase2>* subjects =
        new ( ELeave ) CArrayPtrFlat<MEikSrvNotifierBase2>( KSmlNotifierArrayIncrement );
    
    CleanupStack::PushL( TCleanupItem( CleanupArray, subjects ) );

    // Create Wrappers

    // Session owning notifier
#ifdef __SERIES60_30__


    CAknNotifierWrapper* master = 
        CAknNotifierWrapper::NewL( KSyncMLAppLaunchNotifierUid,
                                   KSmlAppLaunchChannel,
                                   KSmlNAppLaunchPriority,
                                   KSmlNAppServerUid,
                                   KMaxSynchReplyBufLength);

#else // __SERIES60_30__

    CAknCommonNotifierWrapper* master = 
        CAknCommonNotifierWrapper::NewL( KSyncMLAppLaunchNotifierUid,
                                         KSmlAppLaunchChannel,
                                         KSmlNAppLaunchPriority,
                                         KSmlSyncMLNotifierDll(),
                                         KMaxSynchReplyBufLength);

#endif // __SERIES60_30__
		
    subjects->AppendL( master );
    
   // Notifiers using masters session 
    subjects->AppendL(new (ELeave) CAknNotifierWrapperLight(*master, 
                                                            KSyncMLDlgNotifierUid,
                                   			 				KSmlDlgChannel,
                                   			 				KSmlNDlgPriority));
                                   			 				
   // Notifiers using masters session 
    subjects->AppendL(new (ELeave) CAknNotifierWrapperLight(*master, 
                                                            KSyncMLFwUpdNotifierUid,
                                   			 				KSmlFwUpdChannel,
                                   			 				KSmlNFwUpdPriority));
                                   			 				
    CleanupStack::Pop();	// array cleanup
    FLOG(_L("[SmlNotif]\t DoCreateNotifierArrayL() completed"));
    return( subjects );
    }


// -----------------------------------------------------------------------------
// NotifierArray
// Lib main entry point: Creates a notifiers array.
// -----------------------------------------------------------------------------
EXPORT_C CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    FLOG(_L("[SmlNotif]\t NotifierArray()"));
    CArrayPtr<MEikSrvNotifierBase2>* array = 0;
    TRAPD( ignore, array = DoCreateNotifierArrayL() );
    if (ignore != KErrNone) 
    	{
    	ignore = KErrNone;
    	}
    FLOG(_L("[SmlNotif]\t NotifierArray() completed"));
    return array;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// DLL entry point
// -----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	FLOG(_L("[SmlNotif]\t ImplementationGroupProxy()"));
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	FTRACE( FPrint(
	    _L("[SmlNotif]\t ImplementationGroupProxy completed, Table count = %d"),
	    aTableCount ) );
	return ImplementationTable;
	}

