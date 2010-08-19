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

#include <QModelIndex>
#include <QDateTime>
#include <QDir>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbmainwindow.h>
//#include <hbstyleoptioncommonnote.h> //<hbcommonnote.h> this new header compensate oldone ??
#include <HbDataFormModelItem.h>
#include <hbdataformviewitem.h>
#include <hblineedit.h>
#include <HbComboBox.h>
#include <HbPushButton.h>
#include <HbAbstractButton.h>
#include <HbLineEdit.h>
#include <xqsettingsmanager.h>
#include <xqsettingskey.h>
#include <XQServiceUtil.h>

#include <nsmloperatorerrorcrkeys.h>
#include <DataSyncInternalPSKeys.h>
#include "qtsyncstatusmodel.h"
#include "qtsyncstatuslog.h"

QtSyncStatusModel::QtSyncStatusModel(QtSyncStatusLog& log, HbDataForm *form, QObject *parent)
:QObject(parent),
 mSettingsForm(form),
 mSettingsModel(0),
 mSyncLog(log)
{
    connect(mSettingsForm, SIGNAL(activated(const QModelIndex)),
            this, SLOT(itemDisplayed(const QModelIndex)));
    
    mSettingsManager = new XQSettingsManager(this);
    mNsmlOpDsSyncErrorCode = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
            KCRUidOperatorDatasyncErrorKeys.iUid, KNsmlOpDsSyncErrorCode);
    mSettingsManager->startMonitoring(*mNsmlOpDsSyncErrorCode);
    mNsmlOpDsSyncProfUid = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
            KCRUidOperatorDatasyncErrorKeys.iUid, KNsmlOpDsSyncProfId);
    mSettingsManager->startMonitoring(*mNsmlOpDsSyncProfUid);
    mNsmlOpDsSyncType = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
            KCRUidOperatorDatasyncErrorKeys.iUid, KNsmlOpDsSyncType);
    mSettingsManager->startMonitoring(*mNsmlOpDsSyncType);
    mNsmlOpDsSyncInitiation = new XQSettingsKey(XQSettingsKey::TargetCentralRepository,
            KCRUidOperatorDatasyncErrorKeys.iUid, KNsmlOpDsSyncInitiation);
    mSettingsManager->startMonitoring(*mNsmlOpDsSyncInitiation);
    
    mDataSyncStatusKey = new XQSettingsKey(XQSettingsKey::TargetPublishAndSubscribe,
            KPSUidDataSynchronizationInternalKeys.iUid, KDataSyncStatus);
    
    connect(mSettingsManager, SIGNAL(valueChanged(const XQSettingsKey&, const QVariant&)),
            this, SLOT(valueChanged(const XQSettingsKey&, const QVariant&)));
    
    // Read the initial values from the cenrep
    QVariant value = mSettingsManager->readItemValue(*mNsmlOpDsSyncErrorCode);
    mSyncErrorCode = value.toUInt();
    value = mSettingsManager->readItemValue(*mNsmlOpDsSyncProfUid);
    mSyncProfileUid = value.toUInt();
    value = mSettingsManager->readItemValue(*mNsmlOpDsSyncType);
    mSyncType = value.toUInt();
    value = mSettingsManager->readItemValue(*mNsmlOpDsSyncInitiation);
    mSyncInitiation = value.toUInt();
}

QtSyncStatusModel::~QtSyncStatusModel()
{
    delete mSettingsModel;
    delete mSettingsManager;
    delete mNsmlOpDsSyncErrorCode;
    delete mNsmlOpDsSyncProfUid;
    delete mNsmlOpDsSyncType;
    delete mNsmlOpDsSyncInitiation;
    delete mDataSyncStatusKey;
}

void QtSyncStatusModel::initModel()
{
    // Remove the model.
    if (mSettingsForm->model()) {
        delete mSettingsForm->model();
        mSettingsForm->setModel(0);
        mSettingsModel = 0;
    }
        
    mSettingsModel = new HbDataFormModel();
        
    mNsmlOpDsSyncErrorCodeItem = new HbDataFormModelItem(
            HbDataFormModelItem::TextItem, QString( tr("Sync error code") ), 0);
    QString errorCode;
    errorCode.setNum(mSyncErrorCode);
    mNsmlOpDsSyncErrorCodeItem->setContentWidgetData( "text", errorCode );
    mNsmlOpDsSyncErrorCodeItem->setContentWidgetData( QString("readOnly"), QString("false") );
    mSettingsModel->appendDataFormItem(mNsmlOpDsSyncErrorCodeItem);
    
    mNsmlOpDsSyncProfUidItem = new HbDataFormModelItem(
            HbDataFormModelItem::TextItem, QString(tr("Sync profile ID")), 0);
            //HbDataFormModelItem::TextItem, QString(tr("Sync profile ID")), mSettingsModel->invisibleRootItem());
    QString profileUId;
    profileUId.setNum(mSyncProfileUid);
    mNsmlOpDsSyncProfUidItem->setContentWidgetData( "text", profileUId );
    mNsmlOpDsSyncProfUidItem->setContentWidgetData( QString("readOnly"), QString("false") );
    mSettingsModel->appendDataFormItem(mNsmlOpDsSyncProfUidItem);
    
    QStringList syncTypes;
    syncTypes << tr("ESmlTwoWay")
               << tr("ESmlOneWayFromServer")
               << tr("ESmlOneWayFromClient")
               << tr("ESmlSlowSync")
               << tr("ESmlRefreshFromServer")
               << tr("ESmlRefreshFromClient");
    
    mNsmlOpDsSyncTypeItem = new HbDataFormModelItem();
    mNsmlOpDsSyncTypeItem->setType(HbDataFormModelItem::ComboBoxItem);
    mNsmlOpDsSyncTypeItem->setData(HbDataFormModelItem::LabelRole, QString(tr("Sync type")));
    mNsmlOpDsSyncTypeItem->setContentWidgetData(QString("items"), syncTypes);
    mNsmlOpDsSyncTypeItem->setContentWidgetData(QString("currentIndex"), mSyncType);
    mSettingsModel->appendDataFormItem(mNsmlOpDsSyncTypeItem);

    QStringList syncInitiation;
    syncInitiation << tr("No sync ongoing")
               << tr("Client initiated")
               << tr("Server alerted");
    
    mNsmlOpDsSyncInitiationItem = new HbDataFormModelItem();
    mNsmlOpDsSyncInitiationItem->setType(HbDataFormModelItem::ComboBoxItem);
    mNsmlOpDsSyncInitiationItem->setData(HbDataFormModelItem::LabelRole, QString(tr("Sync initiation")));
    mNsmlOpDsSyncInitiationItem->setContentWidgetData(QString("items"), syncInitiation);
    mNsmlOpDsSyncInitiationItem->setContentWidgetData(QString("currentIndex"), mSyncInitiation);
    mSettingsModel->appendDataFormItem(mNsmlOpDsSyncInitiationItem);
    
    mSettingsForm->setModel(mSettingsModel);
}

void QtSyncStatusModel::itemDisplayed(const QModelIndex &index)
{   
    //HbDataFormModelItem *data = static_cast<HbDataFormModelItem*>(index.internalPointer());
    HbDataFormViewItem *viewItem = static_cast<HbDataFormViewItem*>( mSettingsForm->itemByIndex(index) );
       
    switch (index.row())
        {
        case 0:
            {
            HbWidget* contentWidget = viewItem->dataItemContentWidget();
            mSyncErrorCodeEdit = static_cast<HbLineEdit *>(contentWidget);
            mSyncErrorCodeEdit->setInputMethodHints(Qt::ImhPreferNumbers);
            connect(mSyncErrorCodeEdit, SIGNAL(editingFinished()),
                    this, SLOT(handleSyncMLErrorCodeChangeFinished()));
            break;
            }
            
        case 1:
            {
            HbWidget* contentWidget = viewItem->dataItemContentWidget();
            mSyncProfileUidEdit = static_cast<HbLineEdit *>(contentWidget);
            mSyncProfileUidEdit->setInputMethodHints(Qt::ImhPreferNumbers);
            connect(mSyncProfileUidEdit, SIGNAL(editingFinished()),
                    this, SLOT(handleProfileUidChangeFinished()));            
            break;
            }
           
        case 2:
            {
            HbWidget* contentWidget = viewItem->dataItemContentWidget();
            mmSyncTypeComboBox = static_cast<HbComboBox *>(contentWidget);
            connect(mmSyncTypeComboBox, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(handleSyncTypeChange(int)));                
            break;
            }
                
        case 3:
            {
            HbWidget* contentWidget = viewItem->dataItemContentWidget();
            mSyncInitiationComboBox = static_cast<HbComboBox *>(contentWidget);
            connect(mSyncInitiationComboBox, SIGNAL(currentIndexChanged(int)),
                    this, SLOT(handleSyncInitiationChange(int)));                
            break;
            }
        
        default:
        break;
        }
}

void QtSyncStatusModel::handleSyncErrorCodeChangeFinished()
{
    mSettingsManager->writeItemValue(*mNsmlOpDsSyncErrorCode, mSyncErrorCodeEdit->text().toInt()); 
}

void QtSyncStatusModel::handleProfileUidChangeFinished()
{
    mSettingsManager->writeItemValue(*mNsmlOpDsSyncProfUid, mSyncProfileUidEdit->text().toInt());
}

void QtSyncStatusModel::handleSyncTypeChange(int index)
{
    mSettingsManager->writeItemValue(*mNsmlOpDsSyncType, index);
}

void QtSyncStatusModel::handleSyncInitiationChange(int index)
{
    mSettingsManager->writeItemValue(*mNsmlOpDsSyncInitiation, index);
}

void QtSyncStatusModel::valueChanged(const XQSettingsKey& key, const QVariant& value)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString logLine;
    
    unsigned long int crKey = key.key();
    if (crKey == mNsmlOpDsSyncErrorCode->key()) {
        mSyncErrorCode = value.toUInt();
        QString errorCode;
        errorCode.setNum(mSyncErrorCode);
        mNsmlOpDsSyncErrorCodeItem->setContentWidgetData( "text", errorCode );
        logLine.append("[" + dt.toString("dd.MM.yyyy hh:mm:ss") + "] [SyncErrCode: " + errorCode + "]\r\n");
    }
    else if (crKey == mNsmlOpDsSyncProfUid->key()) {
        mSyncProfileUid = value.toUInt();
        QString profileUId;
        profileUId.setNum(mSyncProfileUid);
        mNsmlOpDsSyncProfUidItem->setContentWidgetData( "text", profileUId );
        logLine.append("[" + dt.toString("dd.MM.yyyy hh:mm:ss") + "] [SyncProfileId: " + profileUId + "]\r\n");
    }
    else if (crKey == mNsmlOpDsSyncType->key()) {
        mSyncType = value.toUInt();
        QString syncType;
        syncType.setNum(mSyncType);
        mNsmlOpDsSyncTypeItem->setContentWidgetData(QString("currentIndex"), mSyncType);
        logLine.append("[" + dt.toString("dd.MM.yyyy hh:mm:ss") + "] [SyncType: " + syncType + "]\r\n");
    }
    else if (crKey == mNsmlOpDsSyncInitiation->key()) {
        mSyncInitiation = value.toUInt();
        QString syncInitiation;
        syncInitiation.setNum(mSyncInitiation);
        mNsmlOpDsSyncInitiationItem->setContentWidgetData(QString("currentIndex"), mSyncInitiation);
        logLine.append("[" + dt.toString("dd.MM.yyyy hh:mm:ss") + "] [SyncInitiation: " + syncInitiation + "]\r\n");        
    }
    mSyncLog.write(logLine);
    XQServiceUtil::toBackground( false );
}

void QtSyncStatusModel::clearSyncLog()
{
    mSyncLog.clear();
}


void QtSyncStatusModel::publishSyncStatusKey()
{
    mSettingsManager->writeItemValue(*mDataSyncStatusKey,EDataSyncRunning12);
    mSettingsManager->writeItemValue(*mDataSyncStatusKey,EDataSyncNotRunning);
}

// End of file
