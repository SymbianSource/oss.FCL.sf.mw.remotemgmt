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

#include "customlistview.h"
#include <hbmenu.h>
#include <hbaction.h>
#include <hblistwidgetitem.h>
#include <hblistwidget.h>
#include <QAbstractItemModel>
#include <xqsettingsmanager.h>
#include <nsmloperatordatacrkeys.h>
#include <hbinputdialog.h>
#include <syncmlclient.h>
#include <syncmlclientds.h>
#include <hbmessagebox.h>
#include <centralrepository.h>

CustomListView::CustomListView( QGraphicsItem *parent )
    : HbView( parent )
    {
    setTitle(tr("SyncFwCustomizer"));
    populateModel();
    createMenu();
    }

void CustomListView::editItem()
    {
    HbListWidgetItem* item( mWidget->currentItem() );
    QString text = item->text();
    QString content = item->secondaryText();

	HbInputDialog *dialog = new HbInputDialog();
	dialog->setAttribute(Qt::WA_DeleteOnClose, true); 
	dialog->setPromptText(text);
	dialog->setPrimaryAction(new HbAction("Ok"));
	dialog->setSecondaryAction(new HbAction("Cancel"));
	dialog->setInputMode(HbInputDialog::TextInput);
	dialog->setValue(content);
	dialog->open(this, SLOT(handleEditItem(HbAction*)));
	}

void CustomListView::handleEditItem(HbAction* action)
    {
    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());
    
    if (dialog && action == dialog->actions().first())
        {
        QString text = dialog->value().toString();
        HbListWidgetItem* item(mWidget->currentItem());
        item->setSecondaryText(text);
        mWidget->setCurrentItem(item);
        saveKeyItems();
        }
    }

void CustomListView::saveKeyItems()
    {
    QString notDefined = tr("Not defined");
    XQSettingsManager* manager = new XQSettingsManager( this );
    XQSettingsKey key1( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsOperatorSyncServerURL );
    QVariant url = mWidget->item( 0 )->secondaryText();
    bool err = false;
    if( url.toString().compare( notDefined ) )
        {
        err = manager->writeItemValue( key1, url );
        }
    XQSettingsKey key2( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsOperatorAdapterUid );
    QVariant adapter = mWidget->item( 1 )->secondaryText();
    if( adapter.toString().compare( notDefined ) )
        {
        bool ok;
        int value = adapter.toString().toInt( &ok, 10 );
        if( !ok )
            {
            value = adapter.toString().toInt( &ok, 16 );
            }
        if( ok )
            {
            QVariant var( value );
            err = manager->writeItemValue( key2, var );
            }
        }
    XQSettingsKey key3( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsProfileAdapterUid );
    QVariant profile = mWidget->item( 2 )->secondaryText();
    if( profile.toString().compare( notDefined ) )
        {
        bool ok;
        int value = profile.toString().toInt( &ok, 10 );
        if( !ok )
            {
            value = profile.toString().toInt( &ok, 16 );
            }
        if( ok )
            {
            QVariant var( value );
            err = manager->writeItemValue( key3, var );
            }
        }
    XQSettingsKey key4( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsDevInfoSwVValue );
    QVariant swv = mWidget->item( 3 )->secondaryText();
    if( swv.toString().compare( notDefined ) )
        {
        err = manager->writeItemValue( key4, swv.toByteArray() );
        }
    XQSettingsKey key5( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsDevInfoModValue );
    QVariant mod = mWidget->item( 4 )->secondaryText();
    if( mod.toString().compare(notDefined) )
        {
        err = manager->writeItemValue( key5, mod.toByteArray() );
        }
    XQSettingsKey key6( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsSyncProfileVisibility );
    QVariant visibility = mWidget->item( 5 )->secondaryText();
    if( visibility.toString().compare( notDefined ) )
        {
        bool ok;
        int value = visibility.toString().toInt( &ok, 10 );
        if( !ok )
            {
            value = visibility.toString().toInt( &ok, 16 );
            }
        if( ok )
            {
            QVariant var( value );
            err = manager->writeItemValue( key6, var );
            }
        }
    XQSettingsKey key7( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsOperatorSyncServerId );
    QVariant serverId = mWidget->item( 6 )->secondaryText();
    if( serverId.toString().compare( notDefined ) )
        {
        err = manager->writeItemValue( key7, serverId.toByteArray() );
        }
    XQSettingsKey key8( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsSyncMLStatusCodeList );
    QVariant syncmlStatus = mWidget->item( 7 )->secondaryText();
    if( syncmlStatus.toString().compare( notDefined ) )
        {
        err = manager->writeItemValue( key8, syncmlStatus );
        }
    XQSettingsKey key9( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsDevInfoManValue );
    QVariant man = mWidget->item( 8 )->secondaryText();
    if( man.toString().compare(notDefined) )
        {
        err = manager->writeItemValue( key9, man.toByteArray() );
        }
    XQSettingsKey key10( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsHttpErrorReporting );
    QVariant httpErrorReporting = mWidget->item( 9 )->secondaryText();
    if( httpErrorReporting.toString().compare( notDefined ) )
        {
        bool ok;
        int value = httpErrorReporting.toString().toInt( &ok, 10 );
        if( !ok )
            {
            value = httpErrorReporting.toString().toInt( &ok, 16 );
            }
        if( ok )
            {
            QVariant var( value );
            err = manager->writeItemValue( key10, var );
            }
        } 
    XQSettingsKey key11( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsSyncMLErrorReporting );
    QVariant syncMLErrorReporting = mWidget->item( 10 )->secondaryText();
    if( syncMLErrorReporting.toString().compare( notDefined ) )
        {
        bool ok;
        int value = syncMLErrorReporting.toString().toInt( &ok, 10 );
        if( !ok )
            {
            value = syncMLErrorReporting.toString().toInt( &ok, 16 );
            }
        if( ok )
            {
            QVariant var( value );
            err = manager->writeItemValue( key11, var );
            }
        }
    delete manager;
    }

void CustomListView::syncNow()
    {
    HbInputDialog *dialog = new HbInputDialog();
    dialog->setAttribute(Qt::WA_DeleteOnClose, true); 
    dialog->setPromptText(tr("Profile ID"));
    dialog->setPrimaryAction(new HbAction("Ok"));
    dialog->setSecondaryAction(new HbAction("Cancel"));
    dialog->setInputMode(HbInputDialog::TextInput);
    dialog->open(this, SLOT(startSync(HbAction*)));
    }

void CustomListView::startSync(HbAction* action)
    {
    HbInputDialog *dialog = static_cast<HbInputDialog*>(sender());
        
    if (dialog && action == dialog->actions().first())
        {
        QString text = dialog->value().toString();
        bool ok = false;
        int profileId = text.toInt(&ok, 10);
        if (ok)
            {
            // Open syncML session
            RSyncMLSession syncMLSession;
            RSyncMLDataSyncJob job;
            CleanupClosePushL( syncMLSession );
            CleanupClosePushL( job );
            syncMLSession.OpenL();
            
            // Need instance of data sync class
            // For creating a data sync job, need to specify id of the profile used for synchronization
            // If profile selected is PCSuite, it searches for the bluetooth device to sync with
            
            job.CreateL( syncMLSession, profileId );
            // close the job
            job.Close();
            // close the syncML session
            CleanupStack::PopAndDestroy( &job );
            CleanupStack::PopAndDestroy( &syncMLSession );
            }
        }
    }

void CustomListView::listSyncProfiles()
    {
    // Open syncML session
    RSyncMLSession syncMLSession;
    syncMLSession.OpenL();
    CleanupClosePushL( syncMLSession );

    RArray<TSmlProfileId> profiles;
    // Get the list of available profiles into an array
    syncMLSession.ListProfilesL( profiles, ESmlDataSync );
    QString text = NULL;
    for( TInt i = 0; i < profiles.Count(); i++ )
        {
        RSyncMLDataSyncProfile profile;         
        profile.OpenL( syncMLSession,profiles[i], ESmlOpenReadWrite );
        CleanupClosePushL( profile );
        text.append( QString::number( profiles[i] ) );
        text.append( tr(" ") );
        QString qname( (QChar*)profile.DisplayName().Ptr(), profile.DisplayName().Length() );
        text.append( qname );
        if( profiles.Count() != ( i + 1 ) )
            {
            text.append( tr("; ") );
            }
        CleanupStack::PopAndDestroy(& profile );      
        }
    profiles.Close();
    HbMessageBox::information(text);
    CleanupStack::PopAndDestroy( &syncMLSession );    
    }

void CustomListView::resetItem()
    {
    //HbListWidgetItem* item( mWidget->currentItem() );
    CRepository* rep = CRepository::NewLC( KCRUidOperatorDatasyncInternalKeys );
    rep->Reset( mWidget->currentRow() + 1 );
    CleanupStack::PopAndDestroy( rep );
    populateModel();
    }

void CustomListView::resetAll()
    {
    CRepository* rep = CRepository::NewLC( KCRUidOperatorDatasyncInternalKeys );
    rep->Reset();
    CleanupStack::PopAndDestroy( rep );  
    populateModel();
    }

void CustomListView::populateModel()
    {
    HbListWidget* widget = new HbListWidget;
    XQSettingsManager* manager = new XQSettingsManager( this );
    XQSettingsKey key1( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsOperatorSyncServerURL );
    QString url = manager->readItemValue( key1, XQSettingsManager::TypeString ).toString();
    XQSettingsKey key2( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsOperatorAdapterUid );
    QString adapter = manager->readItemValue( key2, XQSettingsManager::TypeInt ).toString();
    XQSettingsKey key3( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsProfileAdapterUid );
    QString profile = manager->readItemValue( key3, XQSettingsManager::TypeInt ).toString();
    XQSettingsKey key4( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsDevInfoSwVValue );
    QString swv = manager->readItemValue( key4 ).toString();
    XQSettingsKey key5( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsDevInfoModValue );
    QString mod = manager->readItemValue( key5 ).toString();
    XQSettingsKey key6( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsSyncProfileVisibility );
    QString visibility = manager->readItemValue( key6, XQSettingsManager::TypeInt ).toString();
    XQSettingsKey key7( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsOperatorSyncServerId );
    QString serverId = manager->readItemValue( key7 ).toString();
    XQSettingsKey key8( XQSettingsKey::TargetCentralRepository, 
        KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsSyncMLStatusCodeList );
    QString status = manager->readItemValue( key8, XQSettingsManager::TypeString ).toString();
    XQSettingsKey key9( XQSettingsKey::TargetCentralRepository, 
          KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsDevInfoManValue );
    QString man = manager->readItemValue( key9 ).toString();
    XQSettingsKey key10( XQSettingsKey::TargetCentralRepository, 
          KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsHttpErrorReporting );
    QString httpErrorReporting = manager->readItemValue( key10 ).toString();
    XQSettingsKey key11( XQSettingsKey::TargetCentralRepository, 
              KCRUidOperatorDatasyncInternalKeys.iUid, KNsmlOpDsSyncMLErrorReporting );
    QString syncMLErrorReporting = manager->readItemValue( key11 ).toString();
    
    delete manager;
    HbListWidgetItem* result1 = new HbListWidgetItem();
    result1->setText( tr("Operator server URL") );
    if( !url.isEmpty() )
        {
        result1->setSecondaryText( url );
        }
    else
        {
        result1->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result2 = new HbListWidgetItem();
    result2->setText( tr("Sync adapter UID") );
    if( !adapter.isEmpty() )
        {
        result2->setSecondaryText( adapter );
        }
    else
        {
        result2->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result3 = new HbListWidgetItem();
    result3->setText( tr("Profile adapter UID") );
    if( !profile.isEmpty() )
        {
        result3->setSecondaryText( profile );
        }
    else
        {
        result3->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result4 = new HbListWidgetItem();
    result4->setText( tr("Customized SwV") );
    if( !swv.isEmpty() )
        {
        result4->setSecondaryText( swv );
        }
    else
        {
        result4->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result5 = new HbListWidgetItem;
    result5->setText( tr("Customized Mod") );
    if( !mod.isEmpty() )
        {
        result5->setSecondaryText( mod );
        }
    else
        {
        result5->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result6 = new HbListWidgetItem;
    result6->setText( tr("Sync profile visibility") );
    if( !visibility.isEmpty() )
        {
        result6->setSecondaryText( visibility );
        }
    else
        {
        result6->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result7 = new HbListWidgetItem;
    result7->setText( tr("Sync server ID") );
    if( !serverId.isEmpty() )
        {
        result7->setSecondaryText( serverId );
        }
    else
        {
        result7->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result8 = new HbListWidgetItem;
    result8->setText( tr("SyncML status codes") );
    if( !status.isEmpty() )
        {
        result8->setSecondaryText( status );
        }
    else
        {
        result8->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result9 = new HbListWidgetItem;
    result9->setText( tr("Customized Man") );
    if( !man.isEmpty() )
        {
        result9->setSecondaryText( man );
        }
    else
        {
        result9->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result10 = new HbListWidgetItem;
    result10->setText( tr("Http error reporting") );
    if( !httpErrorReporting.isEmpty() )
        {
        result10->setSecondaryText( httpErrorReporting );
        }
    else
        {
        result10->setSecondaryText( tr("Not defined") );
        }
    HbListWidgetItem* result11 = new HbListWidgetItem;
    result11->setText( tr("SyncML error reporting") );
    if( !syncMLErrorReporting.isEmpty() )
        {
        result11->setSecondaryText( syncMLErrorReporting );
        }
    else
        {
        result11->setSecondaryText( tr("Not defined") );
        }
    widget->addItem( result1 );
    widget->addItem( result2 );
    widget->addItem( result3 );
    widget->addItem( result4 );
    widget->addItem( result5 );
    widget->addItem( result6 );
    widget->addItem( result7 );
    widget->addItem( result8 );
    widget->addItem( result9 );
    widget->addItem( result10 );
    widget->addItem( result11 );
    
    connect( widget,
    SIGNAL( activated(HbListWidgetItem*) ),
    SLOT( handleItem(HbListWidgetItem*) ) );
    connect(widget,
    SIGNAL( longPressed(HbListWidgetItem*, const QPointF&) ),
    SLOT( editItem()) );
    mWidget = widget;
    setWidget( widget );  
    }

void CustomListView::createMenu()
    {
    HbMenu* theMenu = menu();
    connect( theMenu->addAction( tr("Edit")), SIGNAL( triggered() ), SLOT( editItem() ) );
    //connect( theMenu->addAction( tr("Save")), SIGNAL( triggered() ), SLOT( saveKeyItems() ) );
    connect( theMenu->addAction( tr("List profiles")), SIGNAL( triggered() ), SLOT( listSyncProfiles() ) );
    connect( theMenu->addAction( tr("Sync now")), SIGNAL( triggered() ), SLOT( syncNow() ) );
    HbMenu* subMenu = theMenu->addMenu(tr("Reset"));
    connect( subMenu->addAction( tr("Reset item")), SIGNAL( triggered() ), SLOT( resetItem() ) );
    connect( subMenu->addAction( tr("Reset All")), SIGNAL( triggered() ), SLOT( resetAll() ) );
    }

