/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  This checks for uninstallation (presently only for sisx uninstallation) or 
 *								removal of memory card. Whenever there is uninstallation the database has 
 * 								to be updated for the parameters changes.
 *								This depends Application installer & the SCPDatabase.
 *               
*/

#include "SCPEventHandler.h"
#include "SCPParamDBController.h"
#include "SCPClient.h"
#include "SCPServerInterface.h"
#include "DmEventNotifierDebug.h"

// -----------------------------------------------------------------------------
// CSCPEventHandler::CSCPEventHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSCPEventHandler::CSCPEventHandler()
    {
    }

// -----------------------------------------------------------------------------
// CSCPEventHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSCPEventHandler::ConstructL()
    {
    //No values to assign
    }

// -----------------------------------------------------------------------------
// CSCPEventHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSCPEventHandler* CSCPEventHandler::NewL()
    {

    CSCPEventHandler* self = NewLC();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CSCPEventHandler::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CSCPEventHandler* CSCPEventHandler::NewLC()
    {
    CSCPEventHandler* self = new( ELeave ) CSCPEventHandler();

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CSCPEventHandler::~CSCPEventHandler()
    {
    }


// -----------------------------------------------------------------------------
// NotifyChangesL
// -----------------------------------------------------------------------------
EXPORT_C void CSCPEventHandler::NotifyChangesL(THandlerServiceId aEvent, THandlerOperation aOperation) 
    {
    
    _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> started NotifyChangesL() aOperation=%d"), aOperation);

    if( (EOpnUninstall == aOperation) || (EOpnRemoved == aOperation))
    {
        _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> EUninstallation == aOperation || EOpnRemoved == aOperation"));
        RArray <TUid> uids;
        CleanupClosePushL(uids);

        // Call database api & fill the uids
        CSCPParamDBController* dbObj = CSCPParamDBController::NewLC();
       
        //calling...ListApplicationsL
	   	_DMEVNT_DEBUG(_L("[CSCPEventHandler]-> ListApplicationsL started"));
        dbObj->ListApplicationsL(uids);
        CleanupStack :: PopAndDestroy(); // dbObj
	    _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> ListApplicationsL ended"));
        _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> uids = %d"), uids.Count());

/*        RArray <TUid> sisUids;
        CleanupClosePushL( sisUids );*/
        
        //GetInstalledSisUidsL(sisUids);
        Swi::RSisRegistrySession sisses ;
        CleanupClosePushL( sisses );
        User :: LeaveIfError(sisses.Connect());
        
        //RArray <TUid> javaUids;
        //CleanupClosePushL( javaUids );
        //GetInstalledJavaUidsL(javaUids);

        RArray <TUid> resultUids;
        CleanupClosePushL(resultUids);        
        _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> Entering loop for each application, to check with AI"));
        
        for (int i=0; i < uids.Count(); i++)
        {
            _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> INFO: Checking case for UID %d"), uids[i].iUid);
            
            switch(uids[i].iUid) {
                case KNSmlDMHostServer1ID:
                case KNSmlDMHostServer2ID:
                case KNSmlDMHostServer3ID:
                case KNSmlDMHostServer4ID:
                    _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> INFO: UID is marked as an exception, cleanup request bypassed..."));
                    break;
                default: {
            Swi::RSisRegistryEntry sisEntry;
            //Opens the base package entry by specifying a UID. 
            TInt lErr = sisEntry.Open(sisses, uids[i]);
            CleanupClosePushL(sisEntry);
            
            _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> INFO: lErr = %d"), lErr);

            //if(EFalse == isInstalledSis(uids[i], sisUids))
            
            // If the application does not exist
            if( lErr != KErrNone || EFalse == sisEntry.IsPresentL())
            {
                _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> INFO: UID %d was identified as uninstalled,\
                        performing cleanup on the same"), uids[i].iUid);
                // Does not exist in sis-registry, need to check in java registry
                //if(EFalse == isInstalledJava(uids[i], javaUids))
             		resultUids.Append(uids[i]);
            }
            
            CleanupStack :: PopAndDestroy(); // sisEntry
                }
                break;
            };
        } // for uids

        //CleanupStack::PopAndDestroy( &sisUids ); // sisUids
        //CleanupStack::PopAndDestroy( &javaUids ); // javaUids
        _DMEVNT_DEBUG(_L("[CSCPEventHandler]->NotifyChangesL(): Loop ended"));        
        _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> resultUids = %d"), resultUids.Count());

        if(resultUids.Count())
        {
        	NotifyCleanupL(resultUids);
        }
        CleanupStack :: PopAndDestroy(3); // resultUids, sisses, uids
    }// if (EUninstallation == aOperation)
    
    _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> end of NotifyChangesL()"));
}

/*
void CSCPEventHandler::GetInstalledJavaUidsL(RArray<TUid>& aUids)
{
    JavaRegistry* JavaRegistryEntry = NULL;

    JavaRegistryEntry = CJavaRegistry::NewL();

    JavaRegistryEntry->GetRegistryEntryUidsL( aUids );
    delete JavaRegistryEntry;
    JavaRegistryEntry = NULL;
}
*/

/*
// Check for application existance, return true is exists.
inline TBool CSCPEventHandler::isInstalledJava(const TUid& aUid, const  RArray<TUid>& aUids ) const
{
    TBool found( EFalse );
    TInt uidc( aUids.Count() );
    for( TInt j( 0 ); found == EFalse && j < uidc; j++ )
        {
        if( aUids[j] == aUid )
            {
            found = ETrue;
            break;
            }
        }
    return found;
}
*/

inline void CSCPEventHandler::NotifyCleanupL(RArray<TUid>& aUids)
{
    _DMEVNT_DEBUG(_L("[CSCPEventHandler:NotifyCleanupL]-> RSCPClient creation & connecting..."));
    RSCPClient lClient;
    CleanupClosePushL (lClient);
    User :: LeaveIfError(lClient.Connect());

    _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> Initializing cleanup..."));
    //TRAPD(lErr, lClient.PerformCleanupL(ESCPApplicationUninstalled, aUids));
    User :: LeaveIfError(lClient.PerformCleanupL(aUids));
    CleanupStack :: PopAndDestroy(); // lClient
    _DMEVNT_DEBUG(_L("[CSCPEventHandler]-> cleanup complete..."));
    _DMEVNT_DEBUG(_L("[CSCPEventHandler:NotifyCleanupL]-> RSCPClient operation Completed..."));
}
