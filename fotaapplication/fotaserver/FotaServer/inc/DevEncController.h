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
* Description:   CDevEncController class declaration
*
*/

#ifndef __DEVENCCONTROLLER_H__
#define __DEVENCCONTROLLER_H__

// INCLUDES
#include <e32base.h>

#include <DevEncConstants.h>
#include "DevEncProgressDlg.h"
#define KDevEncUIUid 0x2000259A

//Forward declaration
class CFotaServer;
class CDevEncSessionBase;
class CDevEncStarterStarter;
class CDevEncProgressObserver;

class CDevEncController : public CBase
    {
    public:
        static CDevEncController* NewL( CFotaServer* aCallback );
        static CDevEncController* NewLC( CFotaServer* aCallback );
        ~CDevEncController();

    public:
        TBool NeedToDecryptL(const TDriveNumber &aDrive);
    
        void DoStartDecryptionL(const TDriveNumber &aDrive);
        
        TBool NeedToEncryptL(TDriveNumber &aDrive);
        
        void DoStartEncryptionL(const TDriveNumber &aDrive );
        
        void ReportDevEncStartCompleteL(TInt aResult);
        
        void ReportDevEncOpnCompleteL(TInt aResult);
        
        TInt GetDEOperation();
        void CheckIfDeviceMemoryBusyL();
        
    private:
    // Functions
        CDevEncController(CFotaServer* aCallback);

        void ConstructL();
        
        TBool IsDeviceEncryptionSupportedL();
        
        void LoadDevEncSessionL();
        
        void UnloadDevEncSession();
        
        void StartDecryptionL();
        
        void StartEncryptionL();
        
        TBool CheckBatteryL();
        
    // Data
        CFotaServer* iCallback; // parent
        
		RLibrary iLibrary;
                
        CDevEncSessionBase*     iEncMemorySession; // Device Encryption engine session

        CDevEncProgressObserver* iDevEncObserver; //Observer for the encryption/decryption operation
        
        TDriveNumber iStorageDrive; //Holds the storage drive id
        
        TInt iDevEncOperation; //TOperations values
        
    };

#endif //__DEVENCCONTROLLER_H__

// End of file
