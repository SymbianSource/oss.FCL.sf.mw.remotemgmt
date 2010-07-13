/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */


#include <hbapplication.h>
#include <hbtransparentwindow.h>
#include <hbstackedlayout.h>
#include <hbmainwindow.h>
#include <hbview.h>
#include <QTranslator>
#include <QLocale>
#include <e32property.h>
#include "FotaServer.h"
#include "FotaSrvDebug.h"
#include "fsview.h"

int main(int argc, char *argv[])
    { 
    FLOG(_L("CFotaServer::Main >>"));

    FLOG(_L("CFotaServer::HbApplication >>"));
    HbApplication a(argc, argv);
    
    FSView* view = new FSView();
    FLOG(_L("CFotaServer::HbView >>"));
    view->hideItems(Hb::AllItems);
    FLOG(_L("CFotaServer::hideItems >>"));
    view->setContentFullScreen();
    FLOG(_L("CFotaServer::setContentFullScreen >>"));

    HbMainWindow mainWindow(0, Hb::WindowFlagTransparent);
    FLOG(_L("CFotaServer::mainWindow >>"));

    HbTransparentWindow *transparentWindow = new HbTransparentWindow;
    FLOG(_L("CFotaServer::transparentWindow >>"));
    HbStackedLayout *stackedLayout = new HbStackedLayout;
    FLOG(_L("CFotaServer::stackedLayout >>"));
    stackedLayout->addItem(transparentWindow);
    FLOG(_L("CFotaServer::addItem >>"));
    view->setLayout(stackedLayout);
    FLOG(_L("CFotaServer::setLayout >>"));

    mainWindow.addView(view);
    FLOG(_L("CFotaServer::addView >>"));
    mainWindow.setCurrentView(view);
    mainWindow.lower();
    FLOG(_L("CFotaServer::setCurrentView >>"));

    // create the server (leave it on the cleanup stack)
    CFotaServer* server = CFotaServer::NewInstance(mainWindow);

    if (server == NULL)
        {
        return 0;
        }
    
    mainWindow.show();

    FLOG(_L("CFotaServer::show <<"));
    //app.installEventFilter(view);
    RSemaphore sem;
    TInt ret = sem.OpenGlobal(KFotaServerScem);
    FLOG(_L("CFotaServer::OpenGlobal >>"));

    FLOG(_L("CFotaServer::showMaximized <<"));

    QTranslator *translator = new QTranslator();
    QString lang = QLocale::system().name();
    QString path = "Z:/resource/qt/translations/";
    bool fine = translator->load("deviceupdates_" + lang, path);
    if (fine)
        qApp->installTranslator(translator);
    
    QTranslator *commontranslator = new QTranslator();
    fine = commontranslator->load("common_" + lang, path);/*finally required once localisation available*/
       if(fine)
            qApp->installTranslator(commontranslator);
       else
            qDebug("fotaserver common translator loading failed");

       a.setApplicationName("FotaServer");
    
    
    view->SetServer(server);
    a.installEventFilter(view);


    // Initialisation complete, now signal the client
    if (ret == KErrNone)
        {
        sem.Signal();
        FLOG(_L("CFotaServer::Signal <<"));
        sem.Close();
        FLOG(_L("CFotaServer::Close <<"));
        }
    FLOG(_L("CFotaServer::KErrNone <<"));
    //This will install and start a active scheduler for this thread.
    ret = a.exec();

    RProperty::Set(TUid::Uid(KOmaDMAppUid), KFotaServerActive, 0);  
    delete server;
    FLOG(_L("CFotaServer::Main <<"));
    return ret;
    }
