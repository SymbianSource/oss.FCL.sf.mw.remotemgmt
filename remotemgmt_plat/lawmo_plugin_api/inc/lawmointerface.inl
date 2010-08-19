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
* Description:  LAWMO Interface 
*
*/

/**
 * C++ Destructor
 */
inline CLAWMOPluginInterface::~CLAWMOPluginInterface()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
	
/**
 * Create instance of CLAWMOPluginInterface
 * @param aUid , implementation Uid
 * @return Instance of CLAWMOPluginInterface
 */
inline CLAWMOPluginInterface* CLAWMOPluginInterface::NewL(TUid aUid, MLawmoPluginWipeObserver* aObserver)
	{		
		TAny* ptr = REComSession::CreateImplementationL( aUid, _FOFF(CLAWMOPluginInterface, iDtor_ID_Key), aObserver);
    CLAWMOPluginInterface* self = reinterpret_cast<CLAWMOPluginInterface*>( ptr );
    return self;
	}
//