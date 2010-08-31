/*
 * Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:   Fota server update and download functionality 
 *
 */

#ifndef FOTADEVICEDIALOGOBSERVER_H
#define FOTADEVICEDIALOGOBSERVER_H

class MfotadevicedialogObserver
    {
public:
    /**
    * Notify the input of the device dialog to the respective caller.
    */
    virtual void HandleDialogResponse( int response, TInt aDialogid ) = 0;
    };

#endif // FOTADEVICEDIALOGOBSERVER_H

// End of File

