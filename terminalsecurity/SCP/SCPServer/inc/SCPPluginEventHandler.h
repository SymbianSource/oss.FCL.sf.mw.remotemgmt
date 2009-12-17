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


#ifndef SCPPLUGINEVENTHANDLER_H
#define SCPPLUGINEVENTHANDLER_H

//  INCLUDES
#include <bldvariant.hrh>
#include <e32base.h>
#include <e32property.h>
#include <f32file.h>

#include "SCPPlugin.h"


// LOCAL CONSTANTS

const TInt KSCPNotifierTimeout( 45000000 ); // 45 seconds

_LIT( KSCPPluginConfigFilename, "plugincfg.bin" );

//  FORWARD DECLARATIONS
class CSCPParamObject;


/**
*  A base class for the active objects responsible for the lock phone functionality
*/
class CSCPPluginEventHandler :  public CBase, 
                                public MSCPPluginEventHandler

	{		
	public:  // Methods

        // Constructors and destructor                
        
        /**
        * Static constructor.
        */
   	    static CSCPPluginEventHandler* NewL( RFs* aRfs );
        
        /**
        * Static constructor, that leaves the pointer to the cleanup stack.
        */
        static CSCPPluginEventHandler* NewLC( RFs* aRfs );         

        /**
        * Destructor.
        */
        virtual ~CSCPPluginEventHandler();
                
        /**
        * Write the plugin configuration into persistent storage.
        * @return A system status code.
        */
        TInt WritePluginConfigurationL();
        
        /**
        * Reset the common storage for the plugins.
        */
        void ResetConfigurationL();               
        
        // Methods from base classes 
        
        // From MSCPPluginEventHandler
        
        /**
        * Retrieve the stored parameter object pointer, which can be used to
        * read/write the configuration for the plugin.
        */
        CSCPParamObject& GetParameters();
        
        /**
        * Fetch the storage path of the server to aPath
        */
        TInt GetStoragePathL( TDes& aPath );
                           
        /**
        * Output the MD5 hash digest of the given buffer to aOutput
        */
        TInt HashInput( TDes& aInput, TDes& aOutput ); 
        
        /**
        * Return a reference to the Event handler's File Server session
        */        
        virtual RFs& GetFsSession();
                
    protected:  // Methods
                                     
        
        
    private: //Methods 
        
        // New Methods   
                           
        /**
        * C++ default constructor.
        */
        CSCPPluginEventHandler( RFs* aRfs );
        
        /**
        * Symbian second-phase constructor
        */
        void ConstructL();
          
    private:   // Data 
        /** A pointer to the object, which contains the configuration for the plugins, owned */
        CSCPParamObject*            iPluginConfiguration;
        /** The configuration path */
        TFileName iConfigurationPath;
        /** A pointer to a connected File Server session, not owned */
        RFs* iRfs;
    };

#endif      // SCPPLUGINEVENTHANDLER_H   
            
// End of File

