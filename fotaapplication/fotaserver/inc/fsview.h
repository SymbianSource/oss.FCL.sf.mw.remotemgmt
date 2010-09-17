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

#ifndef FSVIEW_H_
#define FSVIEW_H_

#include <hbview.h>
#include <QObject>

class CFotaServer;

class FSView : public HbView
    {
    Q_OBJECT

public:
    FSView();
    ~FSView();
signals :
	/**
	 * applicationReady() - To emit the application ready signal for matti tool.
   */
   void applicationReady();
public:   
    void SetServer(CFotaServer * aServer);
    
    //Handles the incoming events
    bool eventFilter(QObject *object, QEvent *event);

private:
    CFotaServer *iServer;
    TBool Initilized;
    };
#endif /* FSVIEW_H_ */
