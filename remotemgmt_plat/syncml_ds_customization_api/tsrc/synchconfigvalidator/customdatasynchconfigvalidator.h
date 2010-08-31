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

#ifndef CUSTOMDATASYNCHCONFIGVALIDATOR_H
#define CUSTOMDATASYNCHCONFIGVALIDATOR_H

#include <QtGui/QMainWindow>
#include "ui_synchconfigvalidator.h"

// FORWARD DECLARATIONS
class CustomDataSynchConfigValidatorEngineWrapper;

class CustomDataSynchConfigValidator : public QMainWindow
{
    Q_OBJECT
    
public:
    CustomDataSynchConfigValidator(QWidget *parent = 0);
    ~CustomDataSynchConfigValidator();

protected:
    void resizeEvent (QResizeEvent* event);

private:
    Ui::CustomDataSynchConfigValidator ui;
    void Initialize();
    bool matchCheckNameWithTableRowIdAndString(int row, QString matchString);

public Q_SLOTS:
    void activateValidation();
    void handleMoreInfoItemsClicks(QTableWidgetItem *itemClicked);

private:
    CustomDataSynchConfigValidatorEngineWrapper *mEngineWrapper;
    QString mContactsAdapterAvailabilityTestErrorNote;
    QString mCustomSynchProfileServerIdTestErrorNote;
    QString mContactsAdapterSynchProfileTestErrorNote;
    QString mMyprofileAdapterSynchProfileTestErrorNote;
};

#endif // CUSTOMDATASYNCHCONFIGVALIDATOR_H
