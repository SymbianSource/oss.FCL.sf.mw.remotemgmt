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


#ifndef SCPPLUGIN_H
#define SCPPLUGIN_H

#include "SCPDebug.h"
/*#ifdef _DEBUG
#define __SCP_DEBUG
#endif // _DEBUG

// Define this so the precompiler in CW 3.1 won't complain about token pasting,
// the warnings are not valid
#pragma warn_illtokenpasting off

#ifdef __SCP_DEBUG
#define Dprint(a) RDebug::Print ## a
#else
#define Dprint(a)
#endif // _DEBUG*/

//  INCLUDES
#include <bldvariant.hrh>
#include <e32base.h>
#include <f32file.h>

//  FORWARD DECLARATIONS
class CSCPParamObject;


/**
*  An interface class that specifies the services available to the plugins
*/
class MSCPPluginEventHandler
    {	    
	public:
	
	/**
    * Used by plugins to retrieve a reference to the configuration object.
    */  
	virtual CSCPParamObject& GetParameters() = 0;
	
	/**
    * Used by plugins to retrieve a reference to the configuration object.
    */  	
	virtual TInt GetStoragePathL( TDes& aPath ) = 0;
	
	/**
    * Used to hash the given input buffer using MD5 to the output buffer.
    */  	
	virtual TInt HashInput( TDes& aInput, TDes& aOutput ) = 0;
	
	/**
    * Retrieves a reference to a connected File Server session
    */
    virtual RFs& GetFsSession() = 0;
		
	};
	
/**
*  The base class for SCP plugins
*/
class CSCPPlugin: public CBase
	{		
	public:  // Methods

        // Constructors and destructor                
        
        /**
        * Static constructor.
        */
   	    inline static CSCPPlugin* NewL( TDesC8& aResData );       

        /**
        * Destructor.
        */
        inline virtual ~CSCPPlugin();
        
        /**
        * Handles plugin events
        * @param aID The event ID
        * @param aParam The event parameters
        * @returns The parameter object identifying the actions taken.
        */
        virtual void HandleEventL( TInt aID, CSCPParamObject& aInParam, CSCPParamObject& aOutParam ) = 0;
        
        virtual void SetEventHandler( MSCPPluginEventHandler* aHandler ) = 0;
                
    protected:  // Methods
                        
        // New Methods   
                           
        /**
        * C++ default constructor.
        */
        inline CSCPPlugin();        
                        
        // Methods from base classes       
                
        
    private: //Methods          
          
    private: // Data         
    
       /** ECOM instance identifier key */
       TUid iDtor_ID_Key; 
    };
    
#include "SCPPlugin.inl"

#endif      // SCPPLUGIN_H   
            
// End of File

