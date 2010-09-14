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
* Description:   CDevEncController definitions Part of ES System Application
*
*/

//System Include
#include <apgcli.h> // for RApaLsSession
#include <apacmdln.h> // for CApaCommandLine
#include <centralrepository.h>
#include <featmgr.h> // for checking DE feature
#include <DevEncEngineConstants.h>
#include <DevEncSessionBase.h> //for device encryption
#include <DevEncProtectedPSKey.h>  //for device encryption
#include <fotaserver.rsg>

//User Include
#include "DevEncController.h"
#include "FotaServer.h"
#include "DevEncProgressObserver.h"
#include "FotaSrvDebug.h"

#define __LEAVE_IF_ERROR(x) if(KErrNone!=x) {FLOG(_L("LEAVE in %s: %d"), __FILE__, __LINE__); User::Leave(x); }

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CDevEncController::NewL
// Instancies CDevEncController object
// ----------------------------------------------------------
//
CDevEncController* CDevEncController::NewL(CFotaServer* aCallback )
    {
    FLOG(_L("CDevEncController::NewL >>"));

    CDevEncController* self = CDevEncController::NewLC(aCallback);
    CleanupStack::Pop();

    FLOG(_L("CDevEncController::NewL <<"));
    return self;
    }


// ----------------------------------------------------------
// CDevEncController::NewL
// Instancies CDevEncController object
// ----------------------------------------------------------
//
CDevEncController* CDevEncController::NewLC(CFotaServer* aCallback )
    {
    FLOG(_L("CDevEncController::NewLC >>"));

    CDevEncController* self = new ( ELeave ) CDevEncController(aCallback);
    CleanupStack::PushL ( self );
    self->ConstructL();

    FLOG(_L("CDevEncController::NewC <<"));
    return self;
    }

// ----------------------------------------------------------
// CDevEncController::ConstructL()
// Initializes data objects
// ----------------------------------------------------------
//
void CDevEncController::ConstructL()
    {
    FLOG(_L("CDevEncController::ConstructL >>"));

    if (IsDeviceEncryptionSupportedL())
        {
				LoadDevEncSessionL();
        }
    else
    	{
				FLOG(_L("Device doesn't support encryption!!"));
        User::Leave(KErrNotSupported);
    	}
        	
    FLOG(_L("CDevEncController::ConstructL <<"));
    }


TBool CDevEncController::IsDeviceEncryptionSupportedL()
    {
    FLOG(_L("CDevEncController::IsDeviceEncryptionSupportedL >>"));

    TBool ret(EFalse);

    FeatureManager::InitializeLibL();
    ret = FeatureManager::FeatureSupported( KFeatureIdFfDeviceEncryptionFeature);
    FeatureManager::UnInitializeLib();

    FLOG(_L("CDevEncController::IsDeviceEncryptionSupportedL, ret = %d <<"), ret);
    return ret;
    }
// ----------------------------------------------------------
// CDevEncController::CDevEncController()
// Constructor
// ----------------------------------------------------------
//
CDevEncController::CDevEncController(CFotaServer* aCallback): iCallback (aCallback),
iEncMemorySession(NULL),
iDevEncObserver (NULL),
iDevEncOperation(EIdle)
        {

        }

// ----------------------------------------------------------
// CDevEncController::CDevEncController()
// Destructor
// ----------------------------------------------------------
//
CDevEncController::~CDevEncController()
    {
    FLOG(_L("CDevEncController::~CDevEncController >>"));

		UnloadDevEncSession();

    if (iDevEncObserver)
        {
        delete iDevEncObserver;
        iDevEncObserver = NULL;
        }

    FLOG(_L("CDevEncController::~CDevEncController <<"));
    }

// ----------------------------------------------------------
// CDevEncController::LoadDevEncSessionL()
// Loads the devenc library
// ----------------------------------------------------------
//
void CDevEncController::LoadDevEncSessionL()
    {
    FLOG(_L("CDevEncController::LoadDevEncSessionL >> "));
    
    if (!iEncMemorySession)
        {
	      TInt err = iLibrary.Load(KDevEncCommonUtils);	 
        if (err != KErrNone)
            {
            FLOG(_L("Error in finding the library... %d"), err);
            }
        else
        	{
		       TLibraryFunction entry = iLibrary.Lookup(1);
         
	        if (!entry)
	            {
	            FLOG(_L("Error in loading the library..."));
	            User::Leave(KErrBadLibraryEntryPoint);
	            }
	        iEncMemorySession = (CDevEncSessionBase*) entry();
	        FLOG(_L("Library is found and loaded successfully..."));
	        }
	      }
    FLOG(_L("CDevEncController::LoadDevEncSessionL << "));
    }

// ----------------------------------------------------------
// CDevEncController::UnloadDevEncSessionL()
// Unloads the devenc library
// ----------------------------------------------------------
//
void CDevEncController::UnloadDevEncSession()
    {
    FLOG(_L("CDevEncController::UnloadDevEncSession >> "));
    
    if (iEncMemorySession)
        {
        delete iEncMemorySession;
        iEncMemorySession = NULL;
        }

		if (iLibrary.Handle())
      	{
       	iLibrary.Close();    
        }
    FLOG(_L("CDevEncController::UnloadDevEncSession << "));
    }
    
TBool CDevEncController::NeedToDecryptL(const TDriveNumber &aDrive)
    {
    FLOG(_L("CDevEncController::NeedToDecryptL, drive = %d >>"), (TInt) aDrive);

    TBool ret (EFalse);
    TInt err (KErrNone);
    TInt status (KErrNone);

    CheckIfDeviceMemoryBusyL();
    
		iEncMemorySession->SetDrive( aDrive);

    err = iEncMemorySession->Connect();
    if (err != KErrNone)
        {
        FLOG(_L("Error in connecting to devencdisk session = %d"), err);
        User::Leave(err);
        }

    err = iEncMemorySession->DiskStatus(status);

    if (!err && (status == EEncrypted || status == EEncrypting))
        {
        FLOG(_L("Drive %d is encrypted"), (TInt) aDrive);
        ret = ETrue;
        }
    else
        {
        CRepository *centrep = CRepository::NewL( KCRUidFotaServer );
        err = centrep->Set(  KDriveToEncrypt, KErrNotFound );
        delete centrep; centrep = NULL;
        }

#if defined(__WINS__)
    ret = ETrue;
#endif

    iEncMemorySession->Close();
    
    FLOG(_L("CDevEncController::NeedToDecrypt, ret = %d <<"), ret);
    return ret;
    }

void CDevEncController::DoStartDecryptionL(const TDriveNumber &aDrive)
    {
    FLOG(_L("CDevEncController::DoStartDecryptionL >>"));

    TInt deoperation (EOpIdle);  

    RProperty::Get(KDevEncProtectedUid, KDevEncOperationKey, deoperation );

    if (deoperation != EOpIdle)
        {
        FLOG(_L("Some disk operation is ongoing. Hence Fota is not possible."));
        User::Leave(KErrNotReady);
        }

    iDevEncOperation = EDecryption;
    iStorageDrive = aDrive;


    StartDecryptionL();

    FLOG(_L("CDevEncController::DoStartDecryptionL <<"));
    }


void CDevEncController::StartDecryptionL()
    {
    FLOG(_L("CDevEncController::StartDecryptionL >>"));

    TInt status (KErrNone);

  	iEncMemorySession->SetDrive ( iStorageDrive );
			
    __LEAVE_IF_ERROR(iEncMemorySession->Connect());

    __LEAVE_IF_ERROR(iEncMemorySession->DiskStatus(status));

    FLOG(_L("Status = %d"), status);

    if (status == EEncrypted)
        {
        if (CheckBatteryL())
            {
            FLOG(_L("Started decryption of drive %d..."), iStorageDrive);

            if (!iDevEncObserver)
                iDevEncObserver = CDevEncProgressObserver::NewL(this, R_FOTASERVER_DECRYPTION_PROGRESS_DIALOG);

            __LEAVE_IF_ERROR(iEncMemorySession->StartDiskDecrypt());

            FLOG(_L("Monitor for completion of the decryption operation..."));

            iDevEncObserver->StartMonitoringL(iEncMemorySession);
            }
        else
            {
            FLOG(_L("Battery low for performing decryption!"));

            iDevEncOperation = EIdle;
            iEncMemorySession->Close();

            iCallback->HandleDecryptionCompleteL(KErrBadPower, EBatteryLevelLevel4);
            }
        }
    else if(status == EDecrypted)
        {
        FLOG(_L("Device is already decrypted"));

        iDevEncOperation = EIdle;
        iEncMemorySession->Close();

        iCallback->HandleDecryptionCompleteL(KErrNone);
        }
    else
        {
        // status is either unmounted, encrypting, decrypting, wiping or corrupted
        FLOG(_L("Can't proceed as disk status is %d"), status);

        iDevEncOperation = EIdle;
        iEncMemorySession->Close();

        iCallback->HandleDecryptionCompleteL(KErrNotReady);
        }

    FLOG(_L("CDevEncController::StartDecryptionL <<"));
    }

void CDevEncController::ReportDevEncOpnCompleteL(TInt aResult)
    {
    FLOG(_L("CDevEncController::ReportDevEncOpnCompleteL, result = %d >>"), aResult);

    TInt err (KErrNone);

    if (iEncMemorySession)
        {
        iEncMemorySession->Close();
        }

    CRepository *centrep = CRepository::NewL( KCRUidFotaServer );

    if (iDevEncOperation == EDecryption)
        {
        err = centrep->Set(  KDriveToEncrypt, iStorageDrive );
        if (err != KErrNone)
            {
            FLOG(_L("Setting drive to encrypt as %d after firmware update, error = %d"), (TInt) iStorageDrive, err);
            }
        }
    else if (iDevEncOperation == EEncryption)
        {
        err = centrep->Set(  KDriveToEncrypt, KErrNotFound );
        if (err != KErrNone)
            {
            FLOG(_L("Setting no drive, error = %d"), err);
            }
        }
    delete centrep; centrep = NULL;

    if (iDevEncOperation == EDecryption)
        {
        FLOG(_L("Starting update..."));
        iCallback->HandleDecryptionCompleteL(KErrNone);
        iDevEncOperation = EIdle;
        }
    else if (iDevEncOperation == EEncryption)
        {
        FLOG(_L("Encryption ended"));
        iCallback->HandleEncryptionCompleteL(KErrNone);
        iDevEncOperation = EIdle;
        }
    else
        {
        //should not land here!
        }

    FLOG(_L("CDevEncController::ReportDevEncOpnCompleteL <<"));
    }

TBool CDevEncController::NeedToEncryptL(TDriveNumber &aDrive)
    {
    FLOG(_L("CDevEncController::NeedToEncryptL >> "));
    TBool ret (EFalse);

    CRepository *centrep = CRepository::NewL( KCRUidFotaServer );
    TInt drive (KErrNotFound);
    TInt err = centrep->Get(  KDriveToEncrypt, drive );
    if (drive != KErrNotFound)
        {
        aDrive = (TDriveNumber) drive;
        ret = ETrue;
        }

    delete centrep; centrep = NULL;

    FLOG(_L("CDevEncController::NeedToEncryptL, ret = %d, err = %d << "), ret, err);
    return ret;
    }

void CDevEncController::DoStartEncryptionL(const TDriveNumber &aDrive)
    {
    FLOG(_L("CDevEncController::DoStartEncryptionL, drive = %d >>"), (TInt) aDrive);

    iDevEncOperation = EEncryption;
    iStorageDrive = aDrive;
    StartEncryptionL();

    FLOG(_L("CDevEncController::DoStartEncryptionL <<"));
    }

void CDevEncController::StartEncryptionL()
    {
    FLOG(_L("CDevEncController::StartEncryptionL >>"));

    TInt status (KErrNone);

    iEncMemorySession->SetDrive( iStorageDrive );

    __LEAVE_IF_ERROR(iEncMemorySession->Connect());

    __LEAVE_IF_ERROR(iEncMemorySession->DiskStatus(status));

    FLOG(_L("Status = %d"), status);

    if (status == EDecrypted)
        {
        FLOG(_L("Started encryption of drive %d..."), iStorageDrive);

        if (CheckBatteryL())
            {
            if (!iDevEncObserver)
                iDevEncObserver = CDevEncProgressObserver::NewL(this, R_FOTASERVER_ENCRYPTION_PROGRESS_DIALOG);

            __LEAVE_IF_ERROR(iEncMemorySession->StartDiskEncrypt());

            FLOG(_L("Monitor for completion of the decryption operation..."));

            iDevEncObserver->StartMonitoringL(iEncMemorySession);
            }
        else
            {
            FLOG(_L("Battery low for performing encryption!"));

            iDevEncOperation = EIdle;
            iEncMemorySession->Close();

            iCallback->HandleEncryptionCompleteL(KErrBadPower);
            }
        }
    else if (status == EEncrypted)
        {
        FLOG(_L("Memory is already encrypted"));

        iDevEncOperation = EIdle;
        iEncMemorySession->Close();

        iCallback->HandleEncryptionCompleteL(KErrNone);
        }
    else
        {
        //status is either encrypting, decrypting, wiping, corrupted
        FLOG(_L("Can't proceed as disk status is %d"), status);

        iDevEncOperation = EIdle;
        iEncMemorySession->Close();

        iCallback->HandleEncryptionCompleteL(KErrNotReady);
        }
    FLOG(_L("CDevEncController::StartEncryptionL <<"));
    }

TBool CDevEncController::CheckBatteryL()
    {
    FLOG(_L("CDevEncController::CheckBatteryL >>"));
#ifdef __WINS__

    // In the emulator, the battery level is always 0 and the charger is never
    // connected, so just return ETrue.
    return ETrue;

#else // __WINS__

    // Running on target. Check the real battery and charger status

    TInt chargingstatus( EChargingStatusError );
    TInt batterylevel( 1 );
    TBool enoughPower( EFalse );

    // Read battery
    FLOG( _L("CDevEncUiEncryptionOperator::CheckBatteryL" ));
    RProperty pw;
    User::LeaveIfError( pw.Attach( KPSUidHWRMPowerState, KHWRMBatteryLevel ) );
    User::LeaveIfError( pw.Get( batterylevel ) );
    pw.Close();

    User::LeaveIfError( pw.Attach( KPSUidHWRMPowerState, KHWRMChargingStatus ) );
    User::LeaveIfError( pw.Get( chargingstatus ));
    pw.Close();

    // Too low battery, power insufficient
    if ( batterylevel >= EBatteryLevelLevel4 )
        {
        enoughPower = ETrue;
        }
    // But charger is connected, power sufficient
    if ( ( chargingstatus != EChargingStatusError ) &&
            ( chargingstatus != EChargingStatusNotConnected ) )
        {
        enoughPower = ETrue;
        }

    FLOG( _L("Battery level: %d  (0..7), chargingstatus %d"),
            batterylevel, chargingstatus );
    FLOG( _L("CDevEncController::CheckBatteryL, ret=%d <<"), ( enoughPower ? 1 : 0 ) );
    return enoughPower;

#endif // __WINS__
    }

TInt CDevEncController::GetDEOperation()
    {
    return iDevEncOperation;
    }

void CDevEncController::CheckIfDeviceMemoryBusyL()
    {
    TInt deoperation (EOpIdle);  

    RProperty::Get(KDevEncProtectedUid, KDevEncOperationKey, deoperation );

    if (deoperation != EOpIdle)
        {
        FLOG(_L("Some disk operation is ongoing. Hence Fota is not possible."));
        User::Leave(KErrNotReady);
        }
	
	  }

// End of file

