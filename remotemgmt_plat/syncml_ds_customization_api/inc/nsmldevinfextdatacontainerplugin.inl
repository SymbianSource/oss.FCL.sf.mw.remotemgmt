/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline methods for ECom plugin use.
*
*/

inline CNSmlDevInfExtDataContainerPlugin::~CNSmlDevInfExtDataContainerPlugin()
    {
    // Inform the ECOM framework that this specific instance of the
    // interface has been destroyed.
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
    
inline CNSmlDevInfExtDataContainerPlugin* CNSmlDevInfExtDataContainerPlugin::NewL()
    {
    const TUid KExtDataContainerInterfaceUid = { 0x2002DC7C };
    TEComResolverParams resolutionParameters;
    resolutionParameters.SetGenericMatch( ETrue );
	TAny* interface = REComSession::CreateImplementationL (
		KExtDataContainerInterfaceUid, 
        _FOFF( CNSmlDevInfExtDataContainerPlugin, iDtor_ID_Key ),
        resolutionParameters );

	return reinterpret_cast < CNSmlDevInfExtDataContainerPlugin* > ( interface );
    }

