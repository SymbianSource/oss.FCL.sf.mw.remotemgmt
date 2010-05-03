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
* Description:
*
*/
#include "SCPDBDebug.h"
#include "SCPParamDBController.h"
#include <TerminalControl3rdPartyAPI.h>

#include <centralrepository.h>
#include "DmEventNotifierCommon.h"
#include "DmEventNotifierCRKeys.h"
#include <e32property.h>
#include <apgcli.h>
#include <apacmdln.h>

void CSCPParamDBController :: ConstructL() {
    _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL() >>>"));
    
    _SCPDB_LOG(_L("[CSCPParamDBController]->INFO: Creating iParamDB"));
    iParamDB = CSCPParamDB :: NewL();
    _SCPDB_LOG(_L("[CSCPParamDBController]-> iParamDB created successfully..."));

    /*
     * Invocation of sw-handler. This is done only once hence cenrep key is used.
     */
 
    _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL(): Checking for KAppUidDmEventNotifier cenrep "));
    CRepository* centrep = NULL;

    TRAPD(err, centrep = CRepository::NewL( TUid::Uid(KAppUidDmEventNotifier) ));

    if(err == KErrNone) {
        TInt lVal = KErrNone;
    	TInt err_val = centrep->Get(KDmEventNotifierEnabled, lVal);
    	
#ifndef __WINS__
        _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL(): cenrep KDmEventNotifierEnabled value:  %d "), lVal);
        if(err_val == KErrNone && 0 == lVal) {
            _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL(): RApaLsSession connection >>> "));
            RApaLsSession apaLsSession;
            User :: LeaveIfError(apaLsSession.Connect());
            TApaAppInfo appInfo;
            _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL(): RApaLsSession connection successful "));

            _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL(): run dmeventnotifier.exe "));
            //apaLsSession.GetAppInfo(appInfo, aUid); Required only if handle is registered through _reg file.
            #if defined (__WINS__) // ifdef (__WINSCW__)
            _LIT(KExampleTaskHandlerExe, "dmeventnotifier.exe");
            #else
            _LIT(KExampleTaskHandlerExe, "Z:\\Sys\\Bin\\dmeventnotifier.exe");            
            #endif

            appInfo.iFullName = KExampleTaskHandlerExe;
            CApaCommandLine* cmdLine  = CApaCommandLine::NewLC();
            cmdLine->SetExecutableNameL( appInfo.iFullName );
            TBuf<KMaxFileName> temp;
            temp.Copy (appInfo.iFullName);
            TInt err = apaLsSession.StartApp(*cmdLine);

            User :: LeaveIfError(err);
            CleanupStack :: PopAndDestroy(cmdLine);
            apaLsSession.Close();
            _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL(): started dmeventnotifier.exe successfully"));
        }
#endif
    }
    _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL(): Checking KAppUidDmEventNotifier cenrep completed "));

    delete centrep;
    _SCPDB_LOG(_L("[CSCPParamDBController]-> ConstructL() <<<"));
}

EXPORT_C CSCPParamDBController :: ~CSCPParamDBController() {
    if(iParamDB != NULL) {
        delete iParamDB;
    }
}

EXPORT_C CSCPParamDBController* CSCPParamDBController :: NewL() {
    _SCPDB_LOG(_L("[CSCPParamDBController]-> NewL() >>>"));
    CSCPParamDBController* lSelf = new(ELeave) CSCPParamDBController();
    CleanupStack :: PushL(lSelf);
    lSelf->ConstructL();
    CleanupStack :: Pop();
    _SCPDB_LOG(_L("[CSCPParamDBController]-> NewL() <<<"));
    return lSelf;
}

EXPORT_C CSCPParamDBController* CSCPParamDBController :: NewLC() {
    _SCPDB_LOG(_L("[CSCPParamDBController]-> NewLC() >>>"));
    CSCPParamDBController* lSelf = NewL();
    CleanupStack :: PushL(lSelf);
    _SCPDB_LOG(_L("[CSCPParamDBController]-> NewLC() <<<"));
    return lSelf;
}

EXPORT_C TInt CSCPParamDBController :: SetValueL(TInt aParamID, const TInt32 aValue, const TInt32 aApp) {
    switch(aParamID) {
        case RTerminalControl3rdPartySession :: EMaxTimeout:
        case RTerminalControl3rdPartySession :: EPasscodeMinLength:
        case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
        case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
        case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeExpiration:
        case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
        case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
        case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
        case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
        case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
            break;
        default:
            return KErrNotSupported;
    }

    if(aValue < 0) {
        return KErrArgument;
    }

    return iParamDB->SetValueForParameterL(aParamID, aValue, aApp);
}

EXPORT_C TInt CSCPParamDBController :: SetValuesL(TInt aParamID, const RPointerArray <HBufC>& aParamValues, const TInt32 aApp) {
    switch(aParamID) {
    case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific:
        break;
    default:
        return KErrNotSupported;
    }

    return iParamDB->SetValuesForParameterL(aParamID, aParamValues, aApp);
}

EXPORT_C TInt CSCPParamDBController :: GetValueL(TInt aParamID, TInt32& aValue, TInt32& aApp) {
    switch(aParamID) {
        case RTerminalControl3rdPartySession :: EMaxTimeout:
        case RTerminalControl3rdPartySession :: EPasscodeMinLength:
        case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
        case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
        case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeExpiration:
        case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
        case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
        case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
        case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
        case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
        case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
        case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
        case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
            break;
        default:
            return KErrNotSupported;
    }

    return iParamDB->GetValueForParameterL(aParamID, aValue, aApp);
}

EXPORT_C TInt CSCPParamDBController :: GetValuesL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp) {
    switch(aParamID) {
    case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific:
        break;
    default:
        return KErrNotSupported;
    }

    return iParamDB->GetValuesForParameterL(aParamID, aParamValues, aApp);
}

EXPORT_C TInt CSCPParamDBController :: ListApplicationsL(RArray <TUid>& aIDArray) {
    return iParamDB->GetApplicationIDListL(aIDArray);
}

EXPORT_C TBool CSCPParamDBController :: IsParamValueSharedL(HBufC* aParamValue, const TInt32 aApp) {
    return iParamDB->IsParamValueSharedL(aParamValue, aApp);
}

EXPORT_C TInt CSCPParamDBController :: DropValuesL(TInt aParamID, const TInt32 aApp) {
    switch(aParamID) {
    case RTerminalControl3rdPartySession :: EMaxTimeout:
    case RTerminalControl3rdPartySession :: EPasscodeMinLength:
    case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
    case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
    case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
    case RTerminalControl3rdPartySession :: EPasscodeExpiration:
    case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
    case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
    case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
    case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
    case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
    case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
    case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
    case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
    case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
    case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific:
        break;
    default:
        return KErrNotSupported;
    }

    return iParamDB->DropValuesL(aParamID, aApp);
}

EXPORT_C TInt CSCPParamDBController :: DropValuesL(TInt aParamID, RPointerArray <HBufC>& aParamValues, const TInt32 aApp) {
    switch(aParamID) {
    case RTerminalControl3rdPartySession :: EMaxTimeout:
    case RTerminalControl3rdPartySession :: EPasscodeMinLength:
    case RTerminalControl3rdPartySession :: EPasscodeMaxLength:
    case RTerminalControl3rdPartySession :: EPasscodeRequireUpperAndLower:
    case RTerminalControl3rdPartySession :: EPasscodeRequireCharsAndNumbers:
    case RTerminalControl3rdPartySession :: EPasscodeExpiration:
    case RTerminalControl3rdPartySession :: EPasscodeMaxRepeatedCharacters:
    case RTerminalControl3rdPartySession :: EPasscodeMinChangeTolerance:
    case RTerminalControl3rdPartySession :: EPasscodeMinChangeInterval:
    case RTerminalControl3rdPartySession :: EPasscodeCheckSpecificStrings:
    case RTerminalControl3rdPartySession :: EPasscodeMaxAttempts:
    case RTerminalControl3rdPartySession :: EPasscodeConsecutiveNumbers:
    case RTerminalControl3rdPartySession :: EPasscodeHistoryBuffer:
    case RTerminalControl3rdPartySession :: EPasscodeMinSpecialCharacters:
    case RTerminalControl3rdPartySession :: EPasscodeDisallowSimple:
    case RTerminalControl3rdPartySession :: EPasscodeDisallowSpecific:
        break;
    default:
        return KErrNotSupported;
    }

    return iParamDB->DropValuesL(aParamID, aParamValues, aApp);
}

EXPORT_C TInt CSCPParamDBController :: ListParamsUsedByAppL(RArray <TInt>& aParamIds, const TInt32 aApp) {
    return iParamDB->ListParamsUsedByAppL(aParamIds, aApp);
}

EXPORT_C TInt CSCPParamDBController :: ListEntriesL(RArray <TInt32>& aNumCols, RPointerArray <HBufC>& aDesCols, const TInt32 aApp) {
    return iParamDB->ListEntriesL(aNumCols, aDesCols, aApp);
}
