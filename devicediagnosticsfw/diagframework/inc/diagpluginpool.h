/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Manages Diagnostics Plug-in
*
*/


#ifndef DIAGPLUGINPOOL_H
#define DIAGPLUGINPOOL_H

// INCLUDES
#include <e32base.h>            // CBase

// FORWARD DECLARATIONS
class MDiagPlugin;
class MDiagPluginPoolObserver;
class CDiagPluginPoolImpl;        


/**
*  Diagnostics Framework Plugin Pool Class
*
*  This class is used to load Diagnostics ECom plug-ins. It works with both
*  Suite Plug-in and Test Plug-in. Once loaded, it will manage the life
*  cycle of the plug-ins.
*
*  All loaded plug-ins will be destroyed when pool is deleted.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CDiagPluginPool ) : public CBase
    {
public:

    /**
    * Two-phased constructors
    *
    * @param aObserver     Plug-in Pool observer.
    * @return New instance of CDiagPluginPool
    */
    IMPORT_C static CDiagPluginPool* NewL( MDiagPluginPoolObserver& aObserver );
    IMPORT_C static CDiagPluginPool* NewLC( MDiagPluginPoolObserver& aObserver );

    /**
    * Destructor
    *
    */
    IMPORT_C virtual ~CDiagPluginPool();

public:     // new API

    /**
    * Loads all plug-ins. Progress is notified via MDiagPluginPoolObserver.
    * 
    * @param aUid - Interface Uid to load.
    */
    IMPORT_C void LoadAsyncL( TUid aUid );
    
    /**
    * Create an instance of a plug-in. It can be a test plug-in or suite plug-in.
    * If this is a suite, it will not have its children associated with it since
    * will require children to be loaded as well.
    *
    * Plug-ins created using this method is not part of plug-in pool. Therefore
    * client must manage the life cycle of the plug-in.
    * 
    * @param aUid - Implementation Uid of plug-in. 
    * @return Newly instantiated plug-in. Ownership is transferred to caller.
    */
    IMPORT_C static MDiagPlugin* CreatePluginL( TUid aUid );

    /**
    * Cancel Plug-in Loading
    * This method cancels plug-in loading. If it was not currently loading
    * plug-ins, it will leave with KErrNotReady
    */
    IMPORT_C void CancelLoadPluginsL();

    /**
    * Find Plug-in that matches given Uid.
    *
    * @param aUid      Uid of plug-in to search.
    * @param aPlugin   Reference to Plugin found. Engine owns this plug-in.
    *                  Client must not deallocate plug-in.
    * @return TInt     KErrNone        - Success.
    *                  KErrNotFound    - Not found.
    *                  KErrNotReady    - Plug-ins are not yet loaded.
    */
    IMPORT_C TInt FindPlugin( TUid aUid, MDiagPlugin*& aPlugin ) const;

    /**
    * Find Plug-in that matches given Uid. This function will leave on error.
    *  Leave codes:    KErrNone        - Success.
    *                  KErrNotFound    - Not found.
    *                  KErrNotReady    - Plug-ins are not yet loaded.
    *
    * @param aUid      Uid of plug-in to search.
    * @return Refernce to the plug-in found.
    */
    IMPORT_C MDiagPlugin& FindPluginL( TUid aUid ) const;

    /**
    * Find Plug-in that matches given service name.
    *
    * @param aServiceName  Service name of the plug-in. Name must match exactly.
    * @param aPlugin       Reference to Plugin found. Engine owns this plug-in.
    *                      Client must not deallocate plug-in.
    * @return TInt         KErrNone        - Success.
    *                      KErrNotFound    - Not found.
    *                      KErrNotReady    - Plug-ins are not yet loaded.
    */
    IMPORT_C TInt FindPlugin( const TDesC& aServiceName,
                              MDiagPlugin*& aPlugin ) const;

    /**
    * Find Plug-in that matches given service name. Will leave on error.
    *  Leave codes:    KErrNone        - Success.
    *                  KErrNotFound    - Not found.
    *                  KErrNotReady    - Plug-ins are not yet loaded.
    *
    * @param aServiceName  Service name of the plug-in. Name must match exactly.
    * @return Reference to Plugin found.
    */
    IMPORT_C MDiagPlugin& FindPluginL( const TDesC& aServiceName ) const;

private:  // private constructors
    
    /**
    * C++ Constructor
    *    
    */    
    CDiagPluginPool();
    
    /**
    * 2nd phase constructor
    *    
    */        
    void ConstructL( MDiagPluginPoolObserver& aObserver );

private:    //  MEMBER DATA
    
    /**
    * iPluginPoolImpl - Acutal implementation of CDiagPluginPool
    * Owns
    */
    CDiagPluginPoolImpl* iPluginPoolImpl;
    };


#endif // DIAGPLUGINPOOL_H

// End of File

