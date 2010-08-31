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

#ifndef CUSTOMDATASYNCHCONFIGVALIDATORENGINE_H
#define CUSTOMDATASYNCHCONFIGVALIDATORENGINE_H

// INCLUDES
#include "customdatasynchconfigvalidatorenginewrapper.h"
#include <e32base.h>

// CLASS DECLARATION
class CustomDataSynchConfigValidatorEngine: public QObject, public CBase
{
    Q_OBJECT
    
public:
    CustomDataSynchConfigValidatorEngine(CustomDataSynchConfigValidatorEngineWrapper* parent);
    ~CustomDataSynchConfigValidatorEngine();
    
    //Tests
    bool dataSynchCenrepExists();
    bool operatordataSynchCenrepExists();
    bool operatordataSynchErrorCenrepExists();
    bool customSynchProfilesAllowed();
    bool configuredContactsAdapterExists(QString & errorMsg);
    bool configuredCustomSynchProfileExists(QString & errorMsg);
    bool contactAdapterCenrepValueToProfileMatches(QString & errorMsg);
    bool myprofileAdapterCenrepValueToProfileMatches(QString & errorMsg);

    /**
     * Tests whether a device info extension plugin is available
     * @return True, if plugin is found, false otherwise.
     */
    bool devInfoExtensionPluginAvailable();
    
private:
    bool getStringValueFromSyncProfileSettingsDb(QString & value, QString columnName, QString whereTypeConditionForSql="");
    /*@returns profileId -1 if problems acquiring the op.specific profile Id*/
    int operatorProfileIdFromProfilesDB();
    bool adapterCenrepValueToProfileMatches(QString & errorMsg, TUid adapterUid);
    
    QString imsi() const;
    QString imei() const;
    QString softwareVersion() const;
    QString model() const;
    QString manufacturer() const;

private:
    CustomDataSynchConfigValidatorEngineWrapper* q;//for emitting signals in q
    TUid iEComDtorKey;
};

#endif /*CUSTOMDATASYNCHCONFIGVALIDATORENGINE_H*/

// End of file
