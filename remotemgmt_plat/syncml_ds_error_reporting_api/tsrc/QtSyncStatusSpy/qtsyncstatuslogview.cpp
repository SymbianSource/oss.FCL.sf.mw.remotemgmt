/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source file for the class QtSyncStatusLogView
*
*/
#include <QGraphicsItem>
#include <QtGui>
#include <QTimer>
#include <QWidget>
#include <QDebug>
#include <hbdataform.h>
#include <hblabel.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbmenu.h>
#include <hbtoolbar.h>
#include <hbscrollarea.h>
#include <hbscrollbar.h>
#include <hbtextitem.h>
#include <hblistwidget.h>
#include <qgraphicslinearlayout.h>
#include <hbpushbutton.h>
#include <qlistiterator>

#include "qtsyncstatuslogview.h"
#include "qtsyncstatuslog.h"


QtSyncStatusLogView::QtSyncStatusLogView(QtSyncStatusLog& log, QGraphicsItem *parent)
    : HbView(parent),
      mSyncLog(log)
{
    setTitle("QtSyncStatusSpy");
    createMenu();
    
    QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Vertical);
    HbScrollArea* scrollArea = new HbScrollArea(this);
    scrollArea->setScrollDirections(Qt::Vertical);
    QGraphicsLinearLayout* layout2 = new QGraphicsLinearLayout(Qt::Vertical);
    QGraphicsWidget* content = new QGraphicsWidget(this);
    
    mTextItem = new HbTextItem();
    layout2->addItem(mTextItem);
    layout2->setContentsMargins(5, 5, 5, 5);
    content->setLayout(layout2);
    scrollArea->setContentWidget(content);

    layout->addItem(scrollArea);
    layout->setStretchFactor(scrollArea, 1);
    setLayout(layout);
}

QtSyncStatusLogView::~QtSyncStatusLogView()
{
    
}

void QtSyncStatusLogView::updateView()
    {
    readLog();
    }

void QtSyncStatusLogView::deleteLog()
    {
    mSyncLog.clear();
    readLog();
    }

void QtSyncStatusLogView::goToMainView()
    {
    emit comingBack();
    }

void QtSyncStatusLogView::activateView()
{
    mainWindow()->setCurrentView(this);
}

void QtSyncStatusLogView::createMenu()
{
    HbMenu* myMenu = menu();
    connect( myMenu->addAction( tr("Update")), SIGNAL( triggered() ), SLOT( updateView() ) );
    connect( myMenu->addAction( tr("Delete log")), SIGNAL( triggered() ), SLOT( deleteLog() ) );
    connect( myMenu->addAction( tr("Back")), SIGNAL( triggered() ), SLOT( goToMainView() ) );
    
    HbToolBar* myToolBar = toolBar();
    connect( myToolBar->addAction( tr("Update")), SIGNAL( triggered() ), SLOT( updateView() ) );
    connect( myToolBar->addAction( tr("Delete log")), SIGNAL( triggered() ), SLOT( deleteLog() ) );
    connect( myToolBar->addAction( tr("Back")), SIGNAL( triggered() ), SLOT( goToMainView() ) );
}

void QtSyncStatusLogView::showEvent(QShowEvent* event)
    {
    readLog();
    HbView::showEvent(event);
    }

void QtSyncStatusLogView::readLog()
    {
    QString logData;
    QString line;
    QStringList list;
    QStringList lines;
    QString prevDate;
    
    lines = mSyncLog.lines();
    QListIterator<QString> iter(lines);

    while (iter.hasNext()) {
        line = iter.next();
        line.remove(QChar('['));
        line.remove(QChar(']'));
        list = line.split(" ");
        if (list.at(0).compare(prevDate)) {
         logData.append(list.at(0) + "\n");
         prevDate = list.at(0); 
        }
        logData.append(list.at(1) + " " + list.at(2) + list.at(3) + "\n");
    }
    mTextItem->setText(logData);
}
