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


#ifndef SCPPLUGINMANAGER_H
#define SCPPLUGINMANAGER_H

//  INCLUDES
#include <bldvariant.hrh>
#include <e32base.h>
#include <e32cmn.h>
#include <ecom/implementationinformation.h>

//  FORWARD DECLARATIONS
class CSCPPlugin;
class CSCPParamObject;
class CSCPServer;

/**
*  A class responsible for managing the plugins for SCP server
*/
class CSCPPluginManager : public CBase
	{		
	public:  // Methods

        // Constructors and destructor                
        
        /**
        * Static constructor.
        */
   	    static CSCPPluginManager* NewL( CSCPServer* aServer );
        
        /**
        * Static constructor, that leaves the pointer to the cleanup stack.
        */
        static CSCPPluginManager* NewLC( CSCPServer* aServer );

        /**
        * Destructor.
        */
        virtual ~CSCPPluginManager();
        
        /**
        * Post events to the plugins
        * @param aID The event ID
        * @param aParam The event dependent parameter set
        * @returns 
        */
        CSCPParamObject* PostEvent( TInt aID, CSCPParamObject& aParam );
                
    protected:  // Methods
        
        
    private: //Methods 
        
        // New Methods   
                           
        /**
        * C++ default constructor.
        */
        CSCPPluginManager( CSCPServer* aServer );
        
        /**
        * Symbian second-phase constructor
        */
        void ConstructL();
        
        /**
        * Load the plugins
        */        
        void LoadPluginsL();          
          
    private:   // Data 
        /** Contains the authenticated implementation pointers */
        RImplInfoPtrArray           iImplementations;
        /** Contains the authenticated plugin pointers */
        RPointerArray<CSCPPlugin>   iPlugins; 
        /** A pointer to the SCP server, not owned */
        CSCPServer*                 iServer;
        /** Are the plugins loaded. They will only be loaded when needed. */
        TBool                       iPluginsLoaded;
    };

#endif      // SCPPLUGINMANAGER_H   
            
// End of File

