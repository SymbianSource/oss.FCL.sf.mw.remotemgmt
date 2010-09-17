/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


// INCLUDE FILES
#include <e32debug.h>
#include <ecom/ecom.h>
#include <SCPParamObject.h>
#include "SCP_IDs.h"
#include "SCPServer.h"
#include "SCPPluginManager.h"
#include "SCPPlugin.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// void CSCPPluginManager::CSCPPluginManager()
// Destructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginManager::CSCPPluginManager( CSCPServer* aServer )
    :iServer( aServer ),
    iPluginsLoaded( EFalse )
	{ 
    // No implementation required    	
	}


// ---------------------------------------------------------
// void CSCPPluginManager::NewL()
// Static constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginManager* CSCPPluginManager::NewL( CSCPServer* aServer )
	{	
	CSCPPluginManager* self = CSCPPluginManager::NewLC( aServer );
	CleanupStack::Pop( self );
		
	return self;	
	}
	

// ---------------------------------------------------------
// void CSCPPluginManager::NewLC()
// Static constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginManager* CSCPPluginManager::NewLC( CSCPServer* aServer )
	{	
	CSCPPluginManager* self = new (ELeave) CSCPPluginManager( aServer );

    CleanupStack::PushL( self );
    self->ConstructL();	
        
    return self;	
    }


// ---------------------------------------------------------
// void CSCPPluginManager::ConstructL()
// Symbian 2nd phase constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPPluginManager::ConstructL()
    {   
    TEComResolverParams resParams;
    
    TRAPD( err, REComSession::ListImplementationsL( 
        KSCPPluginInterfaceUID,
        resParams,
        KRomOnlyResolverUid, 
        iImplementations ) );
    
    if ( err != KErrNone )
        {
        // Fatal, we can't get the plugin list
        Dprint( (_L("CSCPPluginManager::ConstructL(): Failed to get \
            the list of plugins: %d"), err ));        
        User::Leave( err );
        }                  
    }


// ---------------------------------------------------------
// void CSCPPluginManager::LoadPlugins()
// Loop through the availble ROM-based plugins and create the
// implementations.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPPluginManager::LoadPluginsL()
    {
    CSCPPlugin* thisPlugin = NULL;
    // Loop through the array and create the implementations
    for ( TInt i = 0; i < iImplementations.Count(); i++ )
        {
        // We use the implementation UID as the default data
        // to create each allowed implementation
        
        // The buffer should be the UID of the implementation
        TBuf8<KSCPMaxIntLength> curData = iImplementations[i]->DataType();
                
        TRAPD( err, thisPlugin = CSCPPlugin::NewL( curData ) );
        if ( err != KErrNone )
            {
            Dprint( (_L("CSCPPluginManager::LoadPluginsL(): Failed to create\
                plugin, UID: %x"), iImplementations[i]->ImplementationUid().iUid ));            
            }
        else
            {
            iPlugins.AppendL( thisPlugin );            
            thisPlugin->SetEventHandler( iServer->GetEventHandlerL() );
            
            Dprint( (_L("CSCPPluginManager::LoadPluginsL(): Plugin active,\
                UID: %x"), iImplementations[i]->ImplementationUid().iUid ));
            }
        } // for
        
    // Plugins loaded
    iPluginsLoaded = ETrue;
    }
                            

// ---------------------------------------------------------
// void CSCPPluginManager::PostEvent()
// Post an event to the available plugins.
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPParamObject* CSCPPluginManager :: PostEvent(TInt aID, CSCPParamObject& aParam) {       
    Dprint(_L("[CSCPPluginManager]-> PostEvent() >>>"));
    // If the plugins aren't loaded, load them here
    TBool okToPost = ETrue;
    TInt lErr = KErrNone;
    
    if(!iPluginsLoaded) {
        TRAP(lErr, LoadPluginsL());
        
        if(lErr != KErrNone) {
            Dprint((_L("CSCPPluginManager::PostEvent(): ERROR loading plugins: %d"), lErr));
            okToPost = EFalse;
        }
    }
    
    lErr = KErrNone;
    CSCPParamObject* reply = NULL;
    
    TRAP(lErr, reply = CSCPParamObject :: NewL());
    
    if(lErr != KErrNone) {
        return NULL;
    }
    
    lErr = KErrNone;
    TInt lPolicyRunStatus = KErrNone;
    
    if(okToPost) {
        // Send the incoming event to all plugins
        for(TInt i = 0; i < iPlugins.Count(); i++) {
            // The method shouldn't leave, but make sure
            TRAP(lErr, iPlugins[i]->HandleEventL(aID, aParam, *reply));
            
            if(reply->Get(KSCPParamStatus, lPolicyRunStatus) == KErrNone) {
                if(lPolicyRunStatus != KErrNone) {
                    lErr = lPolicyRunStatus;
                }
            }
        }
        
        reply->Set(KSCPParamStatus, lErr);
    }
        
    Dprint(_L("[CSCPPluginManager]-> PostEvent() okToPost=%d<<<"), okToPost);
    return reply;
}
    
// ---------------------------------------------------------
// void CSCPPluginManager::~CSCPPluginManager()
// Destructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginManager::~CSCPPluginManager()
	{
	Dprint( (_L("--> CSCPPluginManager::~CSCPPluginManager()") ));    

    // Delete the plugins and infos, and reset the arrays
    iPlugins.ResetAndDestroy();
    iImplementations.ResetAndDestroy();
    
    REComSession::FinalClose();
                        
	Dprint( (_L("<-- CSCPPluginManager::~CSCPPluginManager()") ));
	}


//  End of File  

