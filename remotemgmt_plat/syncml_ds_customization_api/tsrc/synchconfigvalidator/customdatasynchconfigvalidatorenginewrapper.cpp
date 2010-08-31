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
#include "customdatasynchconfigvalidatorenginewrapper.h"

// INTERNAL INCLUDES
#include "customdatasynchconfigvalidatorengine_p.h"

// ----------------------------------------------------------------------------
CustomDataSynchConfigValidatorEngineWrapper::CustomDataSynchConfigValidatorEngineWrapper (QObject* parent):
QObject(parent),engine(new CustomDataSynchConfigValidatorEngine(this))
{
}

// ----------------------------------------------------------------------------
CustomDataSynchConfigValidatorEngineWrapper::~CustomDataSynchConfigValidatorEngineWrapper()
{
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::dataSynchCenrepExists()
{
    return engine->dataSynchCenrepExists();
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::operatordataSynchCenrepExists()
{
    return engine->operatordataSynchCenrepExists();
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::operatordataSynchErrorCenrepExists()
{
    return engine->operatordataSynchErrorCenrepExists();
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::customSynchProfilesAllowed()
{
    return engine->customSynchProfilesAllowed();
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::configuredContactsAdapterExists(QString & errorMsg)
{
    return engine->configuredContactsAdapterExists(errorMsg);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::configuredCustomSynchProfileExists(QString & errorMsg)
{
    return engine->configuredCustomSynchProfileExists(errorMsg);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::contactAdapterCenrepValueToProfileMatches(QString & errorMsg)
{
    return engine->contactAdapterCenrepValueToProfileMatches(errorMsg);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::myprofileAdapterCenrepValueToProfileMatches(QString & errorMsg)
{
    return engine->myprofileAdapterCenrepValueToProfileMatches(errorMsg);
}

// ----------------------------------------------------------------------------
bool CustomDataSynchConfigValidatorEngineWrapper::devInfoExtensionPluginAvailable()
{
    return engine->devInfoExtensionPluginAvailable();
}

// End of file
