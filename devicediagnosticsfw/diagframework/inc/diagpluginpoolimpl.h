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


#ifndef DIAGPLUGINPOOLIMPL_H
#define DIAGPLUGINPOOLIMPL_H

// INCLUDES
#include <e32base.h>                    // CActive
#include <implementationinformation.h>  // RImplInfoPtrArray

// FORWARD DECLARATIONS
class MDiagPlugin;
class MDiagPluginPoolObserver;
class CDiagPluginConstructionParam;
class MDiagSuitePlugin;   

/**
*  Diagnostics Framework Plugin Pool Implementation Class
*
*  This class is used to load Diagnostics ECom plug-ins. It works with both
*  Suite Plug-in and Test Plug-in. Once loaded, it will manage the life
*  cycle of the plug-ins.
*
*  All loaded plug-ins will be destroyed when pool is deleted.
*
*  @since S60 v5.0
*/
NONSHARABLE_CLASS( CDiagPluginPoolImpl ) : public CActive
    {
public:

    /**
    * Two-phased constructors
    *
    * @param aObserver     Plug-in Pool observer.
    * @return New instance of CDiagPluginPool
    */
    static CDiagPluginPoolImpl* NewL( MDiagPluginPoolObserver& aObserver );
    static CDiagPluginPoolImpl* NewLC( MDiagPluginPoolObserver& aObserver );

    /**
    * Destructor
    *
    */
    virtual ~CDiagPluginPoolImpl();

public:     // new API

    /**
    * Loads all plug-ins. Progress is notified via MDiagPluginPoolObserver.
    * Leave codes:     KErrNone        - Success.
    *                  KErrNotFound    - No plugins
    *                  KErrAlreadyExists    - Plug-ins have already been loaded.
    */
    void LoadAsyncL();
    
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
    static MDiagPlugin* CreatePluginL( const TUid aUid );

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
    TInt FindPlugin( TUid aUid, MDiagPlugin*& aPlugin ) const;

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
    TInt FindPlugin( const TDesC& aServiceName, 
                             MDiagPlugin*& aPlugin ) const;      


private:    // internal types                                   
    typedef RPointerArray<CDiagPluginConstructionParam> RConstructionParamArray;        
                             
private:    // private constructors
    
    /**
    * Construct a CDiagPluginPoolImpl object
    *
    * @param aObserver  The observer which listens to plug-in pool events
    */
    CDiagPluginPoolImpl( MDiagPluginPoolObserver& aObserver );
    
    /**
    * 2nd phase constructor
    *
    */    
    void ConstructL();  


private: // internal API

    /**
    * Obtain the list construction paramaters to construct a plug-in
    *    
    * @return Pointer to construction paramaters pointer array
    */    
    static RConstructionParamArray* GeneratePluginListL();
    
    /**
    * Generate parmaters to construct a plug-in given the ECOM implementation
    * info structure
    *
    * @param aInfo  Pointer to the implementation info structure
    * @return Pointer to newly generated construction params
    */
    
    static CDiagPluginConstructionParam* GenerateConstructionParamsLC(
            const CImplementationInformation* aInfo );
            
    /**
    * Create a plug-in from given construction data
    *
    * @param aInfo  Pointer to the implementation info structure
    * @return Pointer to the newly created plug-in
    */
    static MDiagPlugin* CreatePluginFromConstructionParamsL(
            const CDiagPluginConstructionParam* aParams );                       

    /**
    * Setup active object for next iteration of plug-in loading
    *    
    */
    void SetNextIteration();
    
    /**
    * Load the next plug-in in the list per built in iterator
    *    
    */
    void LoadNextPluginL();
    
    /**
    * Add plug-in to plug-in pool by inserting into the plug-in tree.
    * Plug-in ownership is transferred to the Plug-in Pool if it is
    * supported.  If the plug-in is not supported, it is deleted.
    *
    * @param aPlugin pointer to the new plug-in to be added
    * @return TUid UID of the plug-in to report back to the application.
    * Set to TUid::Null() if the plug-in was not added to the pool.
    */
    TUid AddPluginToPoolLD( MDiagPlugin* aPlugin );
    
    /**
    * Destroy implementation info regarding ECOM plug-ins
    *    
    */
    void DestroyConstructionParams();            

    /**
    * Call when a plug-in is loaded to add a newly loaded plug-in to a currently 
    * loaded suite (if available)
    * no ownership transfer
    *
    * @param aPluginSuite pointer to the new plug-in to be added
    */
    void AddNewPluginToSuiteL(MDiagPlugin* aPlugin);
    
    /**
    * Call when a suite is loaded to add any existing parentless plug-ins with
    * matching parent UID to suite
    * no ownership transfer
    *
    * @param aPluginSuite pointer to the new suite to be added
    */
    void AddPluginsToNewSuiteL(MDiagSuitePlugin* aPluginSuite);
    
    /**
    * Resets all member data and notifies observer that load is copmlete
    *
    * @param aErrorCode Plug-In load error
    * @return LoadCompletedL error, if any
    */
    TInt ResetAndNotify(TInt aErrorCode);


private: // From CActive

    /**
    * @see CActive::RunL
    */
    virtual void RunL();

    /**
    * @see CActive::DoCancel
    */
    virtual void DoCancel();

    /**
    * @see CActive::RunError
    */
    virtual TInt RunError( TInt aError );

private:    //  member data

    MDiagPluginPoolObserver& iObserver;
            
    // List of plug-ins that have been loaded
    RPointerArray<MDiagPlugin> iPlugins;        
    
    // Array of plug-in construction params        
    // owned
    RConstructionParamArray* iConstructionParamArray;
    
    // Indicates that all plug-ins have been loaded
    TBool iPluginsLoaded;
    
    // Indicates that plug-ins are currently loading
    TBool iPluginsLoading;
            
    // Index of current plug-in for plug-in load iteration
    TInt iCurrentPluginIndex;            
    
    // Holds error code for first error found in loading process
    TInt iErrorCode;
    
    // Total number of plug-ins found
    TInt iTotalPluginsFound;
    };
    
#endif // DIAGPLUGINPOOLIMPL_H

// End of File

