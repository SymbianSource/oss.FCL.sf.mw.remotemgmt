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
* Description: CSCPUserInf declaration
*
*/

#ifndef CSCPUSERINF_H
#define CSCPUSERINF_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib
class CSCPUserInf : public CActive
    {
public:
    // Cancel and destroy
    ~CSCPUserInf();

    // Two-phased constructor.
    static CSCPUserInf* NewL();

    // Two-phased constructor.
    static CSCPUserInf* NewLC();

public:
    // New functions
    // Function for making the initial request
    void StartL(TDesC& aString);
    void DoRfsL();
private:
    // C++ constructor
    CSCPUserInf();

    // Second-phase constructor
    void ConstructL();

private:
    // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError(TInt aError);

private:
    enum TCSCPUserInfState
        {
        EGlobalConf, // Uninitialized
        ERfs, // Initalized
        EError
        // Error condition
        };

private:
    TInt iState; // State of the active object
    //RTimer iTimer; // Provides async timing service
    HBufC16* idispText;
    };

#endif // CSCPUSERINF_H
