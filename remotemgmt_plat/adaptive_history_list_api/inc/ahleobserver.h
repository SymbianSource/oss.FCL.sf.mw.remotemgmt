/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface for AHLE engine observer class.
*
*/

#ifndef MAHLEOBSERVER_H
#define MAHLEOBSERVER_H

#include "e32def.h"

/**
*  Interface for AHLE engine observer class.
*
*  @lib AHLE2CLIENT.dll
*  @since Series 60 3.1
*/
class MAHLEObserver
    {
    public:

        /**
        * Observer method invoked when primary storage is changed.
        *
        * @param anError KErrNone if ok, otherwise an other error code.
        */
        virtual void AdaptiveListChanged(const TInt aError) = 0;
    };

#endif      // MAHLEOBSERVER_H

// End of File
