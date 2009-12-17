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
* Description:  DCMO Interface 
*
*/

/**
 * C++ Destructor
 */
inline CDCMOInterface::~CDCMOInterface()
	{
	// Destroy any instance variables and then
	// inform the framework that this specific 
	// instance of the interface has been destroyed.
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
	
/**
 * Create instance of CDCMOInterface
 * @param aUid , implementation Uid
 * @return Instance of CDCMOInterface
 */
inline CDCMOInterface* CDCMOInterface::NewL(TUid aUid)
	{		
		TAny* ptr = REComSession::CreateImplementationL( aUid, _FOFF(CDCMOInterface, iDtor_ID_Key));
    CDCMOInterface* self = reinterpret_cast<CDCMOInterface*>( ptr );
    return self;
	}

/**
 * Create instance of CDCMOInterface
 * @param aInitParms of TDCMOInterfaceInitParams type.
 * @return Instance of CDCMOInterface
 */
inline CDCMOInterface* CDCMOInterface::NewL(TDCMOInterfaceInitParams aInitParms)
	{		
		TAny* ptr = REComSession::CreateImplementationL( aInitParms.uid, _FOFF(CDCMOInterface, iDtor_ID_Key), &aInitParms);
    CDCMOInterface* self = reinterpret_cast<CDCMOInterface*>( ptr );
    return self;
	}
