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
*   An abstract class being representative of the
*   concrete class which the client wishes to use.
*   It acts as a base, for a real class to provide all the
*   functionality that a client requires.
*   It supplies instantiation & destruction by using
*   the ECom framework, and functional services
*   by using the methods of the actual class.
*/
#ifndef TERMINALCONTROL3RDPARTYPARAMSPLUGIN_H_
#define TERMINALCONTROL3RDPARTYPARAMSPLUGIN_H_

#include <e32base.h>
#include <ecom/ecom.h>
#include "TerminalControl3rdPartyParamsPluginDef.h"

struct TParamChange {
	const TInt iParamID;
	const TDesC8 iParamValue;

	TParamChange(TInt aParamID, TDesC8 aParamValue) : iParamID(aParamID), iParamValue(aParamValue) {

	}
};

class CTC3rdPartyParamsEcomIF : public CBase
    {
public:
    // Instantiates an object of this type
    static CTC3rdPartyParamsEcomIF* NewL(const TUid& aImplementationUid);

    // Destructor
    virtual ~CTC3rdPartyParamsEcomIF();

    // Requests a list of all available implementations which follow this
    // given interface.
    static void ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray);

    // pure interface method which the derived class must implement
    /**
     DeviceLockParamChanged

     @param aParamTypeArray The list of device lock param ID(s) for which value was chnaged
                  see TTerminalControl3rdPartyMessages (documentation) for supported param ID values

     @param aParamValueArray The array that contains values for the parameter ID's specified in aParamTypeArray.

     */
    virtual void DeviceLockParamChangedL(const RArray<const TParamChange>& aChange) = 0;

protected:
    inline CTC3rdPartyParamsEcomIF();

protected:
    // Unique instance identifier key
    TUid iDtor_ID_Key;

    };

#include "TerminalControl3rdPartyParamsPlugin.inl"
#endif /*TERMINALCONTROL3RDPARTYPARAMSPLUGIN_H_*/
