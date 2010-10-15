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

class HbFSView : public HbView
    {
    Q_OBJECT

public:
    /**
     * Used to disable the RSK of the dialog when resume dialogs
     * @param   aVal - to enable or disable the key
     * @return   void
     */
    HbFSView();
    
    /**
     * Used to disable the RSK of the dialog when resume dialogs
     * @param   aVal - to enable or disable the key
     * @return   void
     */
    ~HbFSView();

    /**
     * Sets the instance of the fotaserver to the view to access server related variables.
     * @param   aServer - Instance of the fotaserver which is to be set.
     * @return   void
     */
    void SetServer(CFotaServer * aServer);
    
    /**
     * Implements the eventFilter function of HbView and sets the fullscreen dialog to be visible
     * @param   object - The object on which the event is activated. 
     * @param   event - the event ID of the event which is received
     * @return   void
     */
    bool eventFilter(QObject *object, QEvent *event);

private:
    CFotaServer *m_Server;
    };
#endif /* FSVIEW_H_ */
