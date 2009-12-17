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
#include <f32file.h>
#include <e32math.h>
#include <SCPParamObject.h>
#include <hash.h>

#include "SCPServer.h"
#include "SCPPluginEventHandler.h"
#include "SCP_IDs.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// void CSCPPluginEventHandler::CSCPPluginEventHandler()
// C++ default constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginEventHandler::CSCPPluginEventHandler( RFs* aRfs ):
    iPluginConfiguration( NULL ),
    iRfs( aRfs )
	{
	Dprint( (_L("--> CSCPPluginEventHandler::CSCPPluginEventHandler()") ));
 
    // No implementation required    
	
	Dprint( (_L("<-- CSCPPluginEventHandler::CSCPPluginEventHandler()") ));
	}


// ---------------------------------------------------------
// void CSCPPluginEventHandler::NewL()
// Static constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginEventHandler* CSCPPluginEventHandler::NewL( RFs* aRfs )
	{
	CSCPPluginEventHandler* self = CSCPPluginEventHandler::NewLC( aRfs );
	CleanupStack::Pop( self ); 
	
	return self;	
	}
	
// ---------------------------------------------------------
// void CSCPPluginEventHandler::NewLC()
// Static constructor, leaves object pointer to the cleanup stack.
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginEventHandler* CSCPPluginEventHandler::NewLC( RFs* aRfs )
	{
	CSCPPluginEventHandler* self = new (ELeave) CSCPPluginEventHandler( aRfs );

    CleanupStack::PushL( self );
    self->ConstructL();	
        
    return self;	
    }

// ---------------------------------------------------------
// void CSCPPluginEventHandler::ConstructL()
// Symbian 2nd phase constructor
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPPluginEventHandler::ConstructL()
    {    
    // Read the configuration path            
    iConfigurationPath.Zero();
    User::LeaveIfError( iRfs->PrivatePath( iConfigurationPath ) );
    
    // Fetch the configuration for the plugins from disk
    
    iPluginConfiguration = CSCPParamObject::NewL();
        
    // Build the configuration file name
    TFileName cfgFileName = iConfigurationPath;                    
    cfgFileName.Append( KSCPPluginConfigFilename );
        
    TRAPD( err, iPluginConfiguration->ReadFromFileL( cfgFileName, iRfs ) );
    
    if ( ( err != KErrNotFound ) && ( err != KErrNone ) )
        {
        Dprint( (_L("CSCPPluginEventHandler::ConstructL(): WARNING:\
            failed to read plugin configuration: %d"), err )); 
        }            
    }
    

    
// ---------------------------------------------------------
// void CSCPPluginEventHandler::GetParameters()
// Return the stored object pointer.
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPParamObject& CSCPPluginEventHandler::GetParameters()
    {
    return *iPluginConfiguration;
    }
    


// ---------------------------------------------------------
// void CSCPPluginEventHandler::GetRfsSession()
// Return the stored RFs reference
// 
// Status : Approved
// ---------------------------------------------------------
//
RFs& CSCPPluginEventHandler::GetFsSession()
    {
    return *iRfs;  
    }
 
 
 
// ---------------------------------------------------------
// void CSCPPluginEventHandler::WritePluginConfigurationL()
// Get the param buffer from the object and write it to disk
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt CSCPPluginEventHandler::WritePluginConfigurationL()
    {   
    // Build the configuration file name
    TFileName cfgFileName;
    TInt ret = GetStoragePathL( cfgFileName );
    if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPPluginEventHandler::WritePluginConfigurationL():\
            ERROR: Failed to get storage path: %d"), ret ));             
        User::Leave( ret );
        }
        
    cfgFileName.Append( KSCPPluginConfigFilename );   
        
    TRAP( ret, iPluginConfiguration->WriteToFileL( cfgFileName, iRfs ) );
        
    return ret;
    }


// ---------------------------------------------------------
// void CSCPPluginEventHandler::GetStoragePath()
// Retrieve the private path and fill it in aPath
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt CSCPPluginEventHandler::GetStoragePathL( TDes& aPath )
    {    
    aPath.Copy( iConfigurationPath );
                
    return KErrNone;
    }    
    
    
    
// ---------------------------------------------------------
// CSCPPluginEventHandler::HashInput()
// Use the symbian Crypto API to hash the input buffer.
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt CSCPPluginEventHandler::HashInput( TDes& aInput, TDes& aOutput )
    {
    TInt ret = KErrNone;
    
    CMD5* hashObject = NULL;
    TRAP( ret, hashObject = CMD5::NewL() );    
    
#ifdef UNICODE
    TUint16* inputPtr = const_cast<TUint16*>( aInput.Ptr() ); 
    TPtrC8 inputData( reinterpret_cast<TUint8*>(inputPtr), aInput.Length()*2 );
#else // !UNICODE
    TPtrC8 inputData = aInput;
#endif
    
    if ( hashObject != NULL )
        {
        TPtrC8 hash = hashObject->Final( inputData );
        aOutput.Copy( hash );
        delete hashObject;
        }
    
    return ret;
    }


// ---------------------------------------------------------
// CSCPPluginEventHandler::ResetConfigurationL()
// Reset the common parameter storage by resetting the ParamObject.
// 
// Status : Approved
// ---------------------------------------------------------
//
void CSCPPluginEventHandler::ResetConfigurationL()
    {
    iPluginConfiguration->Reset();
        
    WritePluginConfigurationL();
    }

// ---------------------------------------------------------
// void CSCPPluginEventHandler::~CSCPPluginEventHandler()
// Destructor
// 
// Status : Approved
// ---------------------------------------------------------
//
CSCPPluginEventHandler::~CSCPPluginEventHandler()
	{
	Dprint( (_L("--> CSCPPluginEventHandler::~CSCPPluginEventHandler()") )); 
			
	TRAPD( err, WritePluginConfigurationL() );
    if ( err != KErrNone )
        {
        Dprint( (_L("CSCPPluginEventHandler::~CSCPPluginEventHandler():\
            WARNING: Failed to store plugin configuration: %d"), err ));        
        }	
	delete iPluginConfiguration;  
                  
	Dprint( (_L("<-- CSCPPluginEventHandler::~CSCPPluginEventHandler()") ));
	}


//  End of File  

