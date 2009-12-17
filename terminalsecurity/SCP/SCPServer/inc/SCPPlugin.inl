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


#ifndef __SCPPLUGIN_INL
#define __SCPPLUGIN_INL

// INCLUDE FILES
#include <e32debug.h>
#include <ecom/ecom.h>

#include <SCPServerInterface.h>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSCPPlugin::CSCPPlugin()	
	{
    // No implementation required    
	}


// Static constructor.
CSCPPlugin* CSCPPlugin::NewL( TDesC8& aResData )
	{	
   	// Set the resolver parameters to seek out the
    // specific object with the given ID data            
    TEComResolverParams resolverParams;
	resolverParams.SetDataType( aResData );
	resolverParams.SetWildcardMatch( ETrue );
		
	// Create the implementation	
    TAny* newPlugin = REComSession::CreateImplementationL( 
        KSCPPluginInterfaceUID,
	    _FOFF( CSCPPlugin, iDtor_ID_Key ),
	    resolverParams
	    );
		
	return static_cast<CSCPPlugin*>( newPlugin );	
	}
	      
// ---------------------------------------------------------
// void CSCPPlugin::~CSCPPlugin()
// 
// 
// Status : Draft
// ---------------------------------------------------------
// 
CSCPPlugin::~CSCPPlugin()
	{
	Dprint( (_L("--> CSCPPlugin::~CSCPPlugin()")) );    
        
    REComSession::DestroyedImplementation( iDtor_ID_Key );
                  
	Dprint( (_L("<-- CSCPPlugin::~CSCPPlugin()")) );
	}

#endif // __SCPPLUGIN_INL

//  End of File  

