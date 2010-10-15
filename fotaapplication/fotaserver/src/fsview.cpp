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
 * Description: Implementation of Fotaapplication components
 *
 */

#include "fsview.h"
#include "fotaserver.h"

// ------------------------------------------------------------------------------------------------
// HbFSView::HbFSView()
// ------------------------------------------------------------------------------------------------
HbFSView::HbFSView()
    {
    
    }
// ------------------------------------------------------------------------------------------------
// HbFSView::~HbFSView()
// ------------------------------------------------------------------------------------------------
HbFSView::~HbFSView()
    {
    
    }
// ------------------------------------------------------------------------------------------------
// HbFSView::eventFilter()
// ------------------------------------------------------------------------------------------------
bool HbFSView::eventFilter(QObject *object, QEvent *event)
{   
    switch (event->type())
        {
        case QEvent::ApplicationActivate:
            {
            m_Server->SetVisible(ETrue);
            break;
            }
        default:
            break;
        }
return HbView::eventFilter(object, event); 
}



void HbFSView::SetServer(CFotaServer * aServer)
{
    m_Server = aServer;
}
