/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*  Defines Terminal Control Devicelock Param ECom interface
*/


inline CTC3rdPartyParamsEcomIF::CTC3rdPartyParamsEcomIF()
    {
    }

inline CTC3rdPartyParamsEcomIF::~CTC3rdPartyParamsEcomIF()
    {
    // Destroy any instance variables and then
    // inform the framework that this specific
    // instance of the interface has been destroyed.
    REComSession::DestroyedImplementation(iDtor_ID_Key);
    }

inline CTC3rdPartyParamsEcomIF* CTC3rdPartyParamsEcomIF::NewL( const TUid& aImplementationUid)
    {
    
    CTC3rdPartyParamsEcomIF* plugin =
        reinterpret_cast<CTC3rdPartyParamsEcomIF*> (
            REComSession::CreateImplementationL(aImplementationUid,
            _FOFF(CTC3rdPartyParamsEcomIF, iDtor_ID_Key)));

    return plugin;    
    }

inline void CTC3rdPartyParamsEcomIF::ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray)
    {
    REComSession::ListImplementationsL( KTC3rdPartyParamsPluginIfUid, aImplInfoArray );    
    }
