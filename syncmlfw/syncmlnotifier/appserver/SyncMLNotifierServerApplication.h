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
* Description:  Declares SyncML notifiers server application class.
*
*/



#ifndef SYNCMLNOTIFIERSERVERAPPLICATION_H
#define SYNCMLNOTIFIERSERVERAPPLICATION_H

//  INCLUDES
#include <AknNotiferAppServerApplication.h> 
#include <AknNotifierAppServer.h>
#include <eikstart.h>

// CONSTANTS
const TUid KSyncMLNotifierAppServerUid = { 0x102072bf };

// CLASS DECLARATION

/**
*  Server application, in which the notifiers are executed.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
class CSyncMLNotifierServerApplication: public CAknNotifierAppServerApplication
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CSyncMLNotifierServerApplication();
 
    public: // New functions
        
        /**
        * Returns the resource file name of the object. Returns
        * an empty string, since component has no resource file.
        * @since Series 60 3.0
        * @param None
        * @return Resource file name.
        */
        TFileName ResourceFileName() const { TBuf<1> buf; return buf; }

    public: // Functions from base classes
 
        /**
        * From CAknApplication
        * @since Series 60 3.0
        * @param aAppaServer 
        * @return None.
        */
        void NewAppServerL( CApaAppServer*& aAppServer );
 
        /**
        * From CAknApplication Creates the application server document
        * instance.
        * @since Series 60 3.0
        * @param None
        * @return Pointer to the created document instance.
        */
        CApaDocument* CreateDocumentL();
 
        /**
        * From CApaApplication Returns the application server uid.
        * @since Series 60 3.0
        * @param None
        * @return Application server uid.
        */
        TUid AppDllUid() const { return KSyncMLNotifierAppServerUid; } 
    };

/**
*  Document instance for notifier server application.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
class CSyncMLNotifierAppServerDocument: public CAknDocument
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CSyncMLNotifierAppServerDocument( CEikApplication& aApp )
            : CAknDocument( aApp ) {};
        
    protected:  // Functions from base classes
 
        /**
        * From CAknDocument Creates the application server UI
        * instance.
        * @since Series 60 3.0
        * @param None
        * @return Pointer to the created UI instance.
        */
        CEikAppUi* CreateAppUiL();
    };

/**
*  Application UI instance of the notifier server application.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
 class CSyncMLNotifierAppServerAppUi: public CAknNotifierAppServerAppUi
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CSyncMLNotifierAppServerAppUi();
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLNotifierAppServerAppUi();
 
        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();
    };

#endif      // SYNCMLNOTIFIERSERVERAPPLICATION_H
            
// End of File
