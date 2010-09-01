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
*     Provisioning dumpper application
*
*/


#ifndef CWPCXAPP_H
#define CWPCXAPP_H

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CProvisioningCxApp application class.
* Provides factory to create concrete document object.
* 
*/
class CWPCxApp : public CAknApplication
    {
    
    private:

        /**
        * From CApaApplication, creates CProvisioningCxDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
        
        /**
        * From CApaApplication, returns application's UID (KUidProvisioningCx).
        * @return The value of KUidProvisioningDump.
        */
        TUid AppDllUid() const;
    };

#endif

// End of File

