/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
*     Provisioning context list application
*
*/


#ifndef CWPCXAPPUI_H
#define CWPCXAPPUI_H

// INCLUDES
#include <aknviewappui.h>

// FORWARD DECLARATIONS
class CWPCxContainer;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* - view architecture
* - status pane
* 
*/
class CWPCxAppUi : public CAknViewAppUi
    {
    public: // // Constructors and destructor

        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CWPCxAppUi();
        
    private: // from CEikAppUi

        void HandleCommandL(TInt aCommand);

    };

#endif

// End of File
