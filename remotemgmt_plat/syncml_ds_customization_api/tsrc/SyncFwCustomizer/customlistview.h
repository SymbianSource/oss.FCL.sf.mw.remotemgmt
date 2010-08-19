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

#ifndef CUSTOMLISTVIEW_H_
#define CUSTOMLISTVIEW_H_

#include <QStringListModel>
#include <hbview.h>

class HbListWidget;

class CustomListView : public HbView
{
    Q_OBJECT

public:
    CustomListView( QGraphicsItem *parent=0 );  
    virtual ~CustomListView() {};

public slots:
    void editItem();
    void saveKeyItems();
    void listSyncProfiles();
    void syncNow();
    void resetItem();
    void resetAll();
    void handleEditItem(HbAction* action);
    void startSync(HbAction* action);

protected:
    void populateModel();
    void createMenu();

protected:
    HbListWidget* mWidget;

};

#endif /* CUSTOMLISTVIEW_H_*/
