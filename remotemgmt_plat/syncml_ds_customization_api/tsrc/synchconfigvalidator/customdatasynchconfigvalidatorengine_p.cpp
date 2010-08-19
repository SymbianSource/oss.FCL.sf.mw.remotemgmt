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

// CLASS HEADER
#include "customdatasynchconfigvalidatorengine_p.h"

// INTERNAL INCLUDES
#include "customdatasynchconfigvalidatorenginewrapper.h"
#include "customdatasynchdeviceinfo.h"

// EXTERNAL INCLUDES
#include <etel3rdparty.h>
#include <sysutil.h>
#include <centralrepository.h>
#include <ecom/ecom.h>
#include <e32def.h>
#include <utf.h>
#include <nsmloperatordatacrkeys.h>
#include <nsmloperatorerrorcrkeys.h>
#include <syncmlclientds.h>

#include <d32dbms.h>
#include <f32file.h>

const TUid KCRUidDatasyncKeys = { 0x2000CF7E };
const TUint32 KDataSynchCustomProfileKey = 0x0;
const QString SERVERID_DB_COLUMN_NAME = "ServerId";
const QString PROFILEID_DB_COLUMN_NAME = "Id";

// Utility clean up function
void CleanupEComArray(TAny* aArray);

// ----------------------------------------------------------------------------
CustomDataSynchConfigValidatorEngine::CustomDataSynchConfigValidatorEngine(
    CustomDataSynchConfigValidatorEngineWrapper *parent) :
    q(parent)
{
}

// ----------------------------------------------------------------------------
CustomDataSynchConfigValidatorEngine::~CustomDataSynchConfigValidatorEngine()
{
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::dataSynchCenrepExists()
{
    int initError = KErrGeneral;
    TRAP(initError,
        CRepository* dataSynchCenrep = CRepository::NewL(KCRUidDatasyncKeys);
        delete dataSynchCenrep;
    )
    return (initError == KErrNone ? true : false);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::operatordataSynchCenrepExists()
{
    int initError = KErrGeneral;
    TRAP(initError,
        CRepository* operatordataSynchCenrep = CRepository::NewL(KCRUidOperatorDatasyncInternalKeys);
        delete operatordataSynchCenrep;
    )
    return (initError == KErrNone ? true : false);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::operatordataSynchErrorCenrepExists()
{
    int initError = KErrGeneral;
    TRAP(initError,
        CRepository* operatordataSynchErrorCenrep = CRepository::NewL(KCRUidOperatorDatasyncErrorKeys);
        delete operatordataSynchErrorCenrep;
    )
    return (initError == KErrNone ? true : false);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::customSynchProfilesAllowed()
{
    int initError = KErrGeneral;//CRepository::Get or CRepository::NewL
    int customProfileKeyValue = 0;//value 1 means allowed
    TRAP(initError,
        CRepository* dataSynchCenrep = CRepository::NewL(KCRUidDatasyncKeys);
        initError = dataSynchCenrep->Get(KDataSynchCustomProfileKey,customProfileKeyValue);
        delete dataSynchCenrep;
    )
    return (initError == KErrNone && customProfileKeyValue == 1 ? true : false);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::configuredContactsAdapterExists(
    QString & errorMsg)
{
    int initError = KErrGeneral;//CRepository::Get or CRepository::NewL
    int pluginloadError = KErrGeneral;
    
    TUid adapterUid = {0x101F6DDD};//platformcontactadapter Uid
    
    TRAP(initError,
        CRepository* operatordataSynchCenrep = CRepository::NewL(KCRUidOperatorDatasyncInternalKeys);
    int adapterUidInt = 0;
    int errorCode = operatordataSynchCenrep->Get(KNsmlOpDsOperatorAdapterUid,adapterUidInt);
    delete operatordataSynchCenrep;
    if( adapterUidInt > 0 )
    {//if greater than 0 custom contact adapter is 'likely' to be used
        adapterUid = TUid::Uid(adapterUidInt);
    }
    )
    TRAP(pluginloadError,
        TAny* any = REComSession::CreateImplementationL(adapterUid, _FOFF(CustomDataSynchConfigValidatorEngine, iEComDtorKey));
        REComSession::DestroyedImplementation(iEComDtorKey);
    )
    bool success = (initError == KErrNone && pluginloadError == KErrNone) ? true : false;
    if(!success)
    {
        errorMsg = "Problems loading adapter UID: 0x";
        errorMsg = errorMsg+QString::number(adapterUid.iUid,16).toUpper(); 
    }
    
    return success;
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::configuredCustomSynchProfileExists(QString & errorMsg)
{
    int cenrepError = KErrGeneral;//CRepository::Get or CRepository::NewL
    bool serverIdmatchFound = false;
    bool dbOk = false;
    
    QString cenrepServerIdString;
    QString profileServerId;
    
    TRAP(cenrepError,      
        CRepository* operatordataSynchCenrep = CRepository::NewL(KCRUidOperatorDatasyncInternalKeys);
        TBuf8<50> cenrepServerIdBuf;
        cenrepError = operatordataSynchCenrep->Get(KNsmlOpDsOperatorSyncServerId,cenrepServerIdBuf);
        delete operatordataSynchCenrep;
        HBufC* cenrepServerIdHBufC;
        cenrepServerIdHBufC = CnvUtfConverter::ConvertToUnicodeFromUtf8L(cenrepServerIdBuf);
        cenrepServerIdString = QString::fromUtf16(cenrepServerIdHBufC->Ptr(), cenrepServerIdHBufC->Length());
        delete cenrepServerIdHBufC;
    )
    dbOk = getStringValueFromSyncProfileSettingsDb(profileServerId,SERVERID_DB_COLUMN_NAME);
    serverIdmatchFound = cenrepServerIdString.compare(profileServerId)==0 ? true : false;

    bool success = (cenrepError == KErrNone && dbOk) ? serverIdmatchFound : false;
    if(!success){
        errorMsg = "Problems with Cenrep defined ServerId:["+cenrepServerIdString+"]"; 
    }
    return success;
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::contactAdapterCenrepValueToProfileMatches(QString & errorMsg)
{
    int cenrepError = KErrGeneral;//CRepository::Get or CRepository::NewL
    
    TUid cenrepAdapterUid = { 0x101F6DDD };//platformcontactadapter Uid
    int adapterUidInt = 0;
    TRAP(cenrepError, 
        CRepository* operatordataSynchCenrep = CRepository::NewL(KCRUidOperatorDatasyncInternalKeys);
        cenrepError = operatordataSynchCenrep->Get(KNsmlOpDsOperatorAdapterUid,adapterUidInt);
        delete operatordataSynchCenrep;
        )
    if( adapterUidInt > 0 )
    {//if greater than 0 custom contact adapter is 'likely' to be used
        cenrepAdapterUid = TUid::Uid(adapterUidInt);
    }
    return adapterCenrepValueToProfileMatches(errorMsg, cenrepAdapterUid);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::myprofileAdapterCenrepValueToProfileMatches(QString & errorMsg)
{
    int cenrepError = KErrGeneral;//CRepository::Get or CRepository::NewL  
    TUid cenrepAdapterUid = { 0x0 };
    int adapterUidInt = 0;
    TRAP(cenrepError,
        CRepository* operatordataSynchCenrep = CRepository::NewL(KCRUidOperatorDatasyncInternalKeys);
        cenrepError = operatordataSynchCenrep->Get(KNsmlOpDsProfileAdapterUid,adapterUidInt);
        delete operatordataSynchCenrep;
    )
    if( adapterUidInt > 0 )
    {//if greater than 0 custom contact adapter is 'likely' to be used
        cenrepAdapterUid = TUid::Uid(adapterUidInt);
    }    
    return adapterCenrepValueToProfileMatches(errorMsg, cenrepAdapterUid);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::devInfoExtensionPluginAvailable()
{
    TUid devInfoExtensionPluginUid = { 0x2002DC7C };
    int err = KErrGeneral;
    bool pluginExists = false;
    RImplInfoPtrArray implInfoArray;
    TCleanupItem cleanup( CleanupEComArray, &implInfoArray );
    TRAP(err,
        CleanupStack::PushL( cleanup );
        REComSession::ListImplementationsL(
            devInfoExtensionPluginUid, implInfoArray );
    if( err == KErrNone && implInfoArray.Count() > 0 )
    {
        pluginExists = true;
    }

    CleanupStack::PopAndDestroy(); // implInfoArray
    )
    return pluginExists;
}

//
// Helper functions
//
//
// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::adapterCenrepValueToProfileMatches(QString & errorMsg, TUid adapterUid)
{
    int profileOpenError = KErrGeneral;
    bool adapterMatchFound = false;
    
    int dbProfileId = operatorProfileIdFromProfilesDB();
    if(dbProfileId<0)
    {//No profile Id or cenrep server Id could not found
        errorMsg = "Test cannot proceed. No profile id was acquired or cenrepServerId was not configured.";
        return false;//RSyncMLDataSyncProfile.OpenL panics
    }
    //Get Contact Adapter UID from synch profile 
    TRAP(profileOpenError,
        RSyncMLSession syncMLSession;
        RArray<TSmlProfileId> profiles;
        syncMLSession.OpenL();
        CleanupClosePushL(syncMLSession);
        // Get the list of available profiles into an array
        syncMLSession.ListProfilesL(profiles, ESmlDataSync);
        QString dataProviderString = NULL;
        QString serverDataSourceName;
    
        for(int i=0; i < profiles.Count(); i++)
        {
            RSyncMLDataSyncProfile profile;
            profile.OpenL(syncMLSession,profiles[i],ESmlOpenRead);
            if (!profileOpenError) {//-42 PermissionDenied -> Capabilities problem
                CleanupClosePushL(profile);
                RArray<TSmlTaskId> profileTasks;
                CleanupClosePushL(profileTasks);
                profile.ListTasksL(profileTasks);
                for (int j = 0; j < profileTasks.Count(); j++) {
                    RSyncMLTask task;
                    task.OpenL(profile, profileTasks[j]);
                    CleanupClosePushL(task);
                    if(task.Profile() == dbProfileId)
                    {
                        int taskProviderInt = task.DataProvider();
                        int cenrepProviderInt = adapterUid.iUid;
                        if(task.DataProvider()==adapterUid.iUid)
                        {
                            serverDataSourceName = QString((QChar*) task.ServerDataSource().Ptr(),
                                task.ServerDataSource().Length());
                            adapterMatchFound = true;
                        }
                    }
                    CleanupStack::PopAndDestroy(&task);
                }
                CleanupStack::PopAndDestroy(&profileTasks);
                CleanupStack::PopAndDestroy(&profile);
            }
        }
        if(profileOpenError==KErrNone)
        {
            profiles.Close();
        }
        CleanupStack::PopAndDestroy(&syncMLSession);
    )
    bool success = (profileOpenError == KErrNone) ? adapterMatchFound : false;
    if(!success)
    {
        errorMsg = "No match found.";
        errorMsg = errorMsg + "\n\n [dbProfileId]=("+QString::number(dbProfileId)+")";
        errorMsg = errorMsg + "\n [cenrepAdapterUid]=\n("+"0x"+QString::number(adapterUid.iUid,16).toUpper()+")";
        errorMsg = errorMsg + "\n\n [profileOpenError]=("+QString::number(profileOpenError)+")";
    }
    return success;
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngine::getStringValueFromSyncProfileSettingsDb(QString& value,QString columnName,QString whereTypeConditionForSql)
{
    //profileDbOpenError can be caused by RFs,RDbs,RDbNamedDatabase,RDbTable or RDbView startup sequences
    int profileDbOpenError = KErrGeneral;
    bool resultsFromDbWithQuery = false;
    
    TPtrC columnNamePtr (static_cast<const TUint16*>(columnName.utf16()), columnName.length());
       
    QString syncSettingsDbName = "\\private\\100012a5\\DBS_101F99FB_nsmldssettings.db";
    QString syncSettingsDbTableProfiles = "Profiles";
    TPtrC syncSettingsDbNamePtr (static_cast<const TUint16*>(syncSettingsDbName.utf16()), syncSettingsDbName.length());
    TPtrC syncSettingsDbTableProfilesPtr (static_cast<const TUint16*>(syncSettingsDbTableProfiles.utf16()), syncSettingsDbTableProfiles.length());
    
    //Query & where condition handling
    QString sqlGetProfileQuery = "SELECT * FROM Profiles";
    if(whereTypeConditionForSql.length()>0)
    {
        sqlGetProfileQuery = sqlGetProfileQuery+" WHERE "+whereTypeConditionForSql;
    }
    TPtrC sqlQueryPtr (static_cast<const TUint16*>(sqlGetProfileQuery.utf16()), sqlGetProfileQuery.length());
    
    //DB INIT
    RDbNamedDatabase database;
    RFs fsSession;
    RDbs rdbSession;
    RDbTable dbTableProfiles;
    CDbColSet* dbColSet;
    RDbView dbView;   
    TRAP(profileDbOpenError,
        //DB CONNECT
        profileDbOpenError = fsSession.Connect();
        CleanupClosePushL(fsSession);
        profileDbOpenError = rdbSession.Connect();
        CleanupClosePushL(rdbSession);
        int i = 0;
        profileDbOpenError = KErrGeneral;//else DBMS 2 panic is rised in dbTableProfiles.Open
        while (i < 10 && profileDbOpenError != KErrNone)
        {//100ms synchronous waits max. 1sec
            profileDbOpenError = database.Open(fsSession,syncSettingsDbNamePtr);
            User::After(100000);
            i++;
        }
        CleanupClosePushL(database);
        profileDbOpenError = dbTableProfiles.Open( database, syncSettingsDbTableProfilesPtr );
        CleanupClosePushL(dbTableProfiles);
    
        //DB QUERY
        profileDbOpenError = dbView.Prepare( database, TDbQuery( sqlQueryPtr ), dbView.EReadOnly );
        CleanupClosePushL(dbView);
        dbColSet = dbTableProfiles.ColSetL();
        CleanupStack::PushL(dbColSet);
    
        while(profileDbOpenError!=KErrArgument&&dbView.NextL())
        {//if results and no db error
            resultsFromDbWithQuery = true;
            dbView.GetL();
            if(columnName.compare(PROFILEID_DB_COLUMN_NAME)==0)
            {//Uint 
                TUint valueFromDb = dbView.ColUint( dbColSet->ColNo( columnNamePtr ));
                value.setNum(valueFromDb);
            }
            else if(columnName.compare(SERVERID_DB_COLUMN_NAME)==0)
            {//String types
                QString valueFromDb ((QChar*)dbView.ColDes( dbColSet->ColNo( columnNamePtr )).Ptr(),
                    dbView.ColDes(dbColSet->ColNo( columnNamePtr )).Length());
                value=valueFromDb;
            }
        }
        //DB CLOSE
        CleanupStack::PopAndDestroy(dbColSet);
        CleanupStack::PopAndDestroy(5,&fsSession);
    )
    
    return profileDbOpenError == KErrNone ? resultsFromDbWithQuery : false;
}

// ----------------------------------------------------------------------------
int CustomDataSynchConfigValidatorEngine::operatorProfileIdFromProfilesDB()
{
    int profileId = -1;//Return this value if problems during fetch
    int cenrepError = KErrGeneral; //CRepository::Get or CRepository::NewL
    bool dbOk = false;
    
    //Find right Synch profile based on trusted cenrep configured Server Id
    QString profileIdString;
    QString cenrepServerIdString;
    
    TRAP(cenrepError,
        CRepository* operatordataSynchCenrep = CRepository::NewL(KCRUidOperatorDatasyncInternalKeys);
        TBuf8<50> cenrepServerIdBuf;
        cenrepError = operatordataSynchCenrep->Get(KNsmlOpDsOperatorSyncServerId,cenrepServerIdBuf);
        delete operatordataSynchCenrep;
        HBufC* cenrepServerIdHBufC;
        cenrepServerIdHBufC = CnvUtfConverter::ConvertToUnicodeFromUtf8L(cenrepServerIdBuf);
        cenrepServerIdString = QString::fromUtf16(cenrepServerIdHBufC->Ptr(), cenrepServerIdHBufC->Length());
        delete cenrepServerIdHBufC;
    )
    if(cenrepServerIdString.compare("")==0)
    {//protect against DBMS panic 2
        return profileId;//-1
    }
    QString whereCondition = SERVERID_DB_COLUMN_NAME+"='"+cenrepServerIdString+"'";
    dbOk = getStringValueFromSyncProfileSettingsDb(profileIdString,PROFILEID_DB_COLUMN_NAME,whereCondition);
    if(dbOk)
    {
        profileId = profileIdString.toInt();
    }
    return profileId;//error handling needs to be done in calling code
}

// ----------------------------------------------------------------------------
QString CustomDataSynchConfigValidatorEngine::imei() const
{
    QScopedPointer<CDeviceInfo> deviceInfo(CDeviceInfo::NewL());
    TBuf<CTelephony::KPhoneSerialNumberSize> imei(deviceInfo->imei());
    return QString::fromUtf16(imei.Ptr(), imei.Length());
}

// ----------------------------------------------------------------------------
QString CustomDataSynchConfigValidatorEngine::model() const
{
    QScopedPointer<CDeviceInfo> deviceInfo(CDeviceInfo::NewL());
    TBuf<CTelephony::KPhoneModelIdSize> model(deviceInfo->model());
    return QString::fromUtf16(model.Ptr(), model.Length());
}

// ----------------------------------------------------------------------------
QString CustomDataSynchConfigValidatorEngine::manufacturer() const
{
    QScopedPointer<CDeviceInfo> deviceInfo(CDeviceInfo::NewL());
    TBuf<CTelephony::KPhoneModelIdSize> manufacturer(deviceInfo->manufacturer());
    return QString::fromUtf16(manufacturer.Ptr(), manufacturer.Length());
}

// ----------------------------------------------------------------------------
QString CustomDataSynchConfigValidatorEngine::imsi() const
{
    QScopedPointer<CDeviceInfo> deviceInfo(CDeviceInfo::NewL());
    TBuf<CTelephony::KIMSISize> imsi(deviceInfo->imsi());
    return QString::fromUtf16(imsi.Ptr(), imsi.Length());
}

// ----------------------------------------------------------------------------
QString CustomDataSynchConfigValidatorEngine::softwareVersion() const
{
    QString version;
    TBuf<KSysUtilVersionTextLength> versionBuf;
    if (SysUtil::GetSWVersion(versionBuf) == KErrNone) {
        version = QString::fromUtf16(versionBuf.Ptr(), versionBuf.Length());
    }
    return version;
}

// ----------------------------------------------------------------------------
// CleanupEComArray function is used for cleanup support of locally declared arrays
void CleanupEComArray(TAny* aArray)
{
    (static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
    (static_cast<RImplInfoPtrArray*> (aArray))->Close();
}

// End of file
