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
*     Provisioning context list document
*
*/


#ifndef CWPCXDOCUMENT_H
#define CWPCXDOCUMENT_H

// INCLUDES
#include <akndoc.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;

// CLASS DECLARATION

/**
*  CWPCxDocument document class.
*/
class CWPCxDocument : public CAknDocument
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CWPCxDocument* NewL(CEikApplication& aApp);

        /**
        * Destructor.
        */
        ~CWPCxDocument();

    private:

        /**
        * C++ default constructor.
        */
        CWPCxDocument(CEikApplication& aApp);

        /**
        * Symbian second phase constructor.
        */
        void ConstructL();

    private: // from CEikDocument

        CEikAppUi* CreateAppUiL();
    };

#endif

// End of File

