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
* Description:  Context manager interface.
*
*/


#ifndef MWPCONTEXTMANAGER_H
#define MWPCONTEXTMANAGER_H

// FORWARD DECLARATIONS
class HBufC16;
class MDesC16Array;
class CDesC16Array;
class MWPContextExtension;
class MWPContextExtensionArray;
class MWPContextObserver;

// CONSTANTS
const TUint32 KWPMgrUidNoContext = 0xffffffff;

// CLASS DECLARATION

/**
*  MWPContextManager declares an interface to be implemented by
*  context managers.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class MWPContextManager
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        virtual ~MWPContextManager() {};

        /**
        * Called after saving one setting.
        * @param aExtension Extension interface for the saving adapter
        * @param aItem Setting number
        */
        virtual void SaveL( MWPContextExtension& aExtension, TInt aItem ) = 0;

        /**
        * Creates a context. Leaves with KErrOverflow if the max. number of contexts
        * has been reached.
        * @param aName Name of the context
        * @param aTPS TPS of the context
        * @param aProxies Proxies that can be used in the context
        * @return UID of the new context
        */
        virtual TUint32 CreateContextL( const TDesC& aName, const TDesC& aTPS, 
            const MDesC16Array& aProxies ) = 0;

        /**
        * Deletes a context.
        * @param aArray The context extension array
        * @param aUID UID of the context
        */
        virtual void DeleteContextL( MWPContextExtensionArray& aArray, 
            TUint32 aUid ) = 0;

        /**
        * Number of contexts available.
        * @return Array of context uids. Ownership is transferred.
        */
        virtual CArrayFix<TUint32>* ContextUidsL() = 0;

        /**
        * Name of a context.
        * @param aUid UID of the context.
        * @return Context name. Ownership is transferred.
        */
        virtual HBufC16* ContextNameL( TUint32 aUid ) = 0;

        /**
        * TPS of a context.
        * @param aUid UID of the context.
        * @return TPS. Ownership is transferred.
        */
        virtual HBufC16* ContextTPSL( TUint32 aUid ) = 0;

        /**
        * Trusted proxies of a context.
        * @param aUid UID of the context.
        * @return Array of proxies. Ownership is transferred.
        */
        virtual CDesC16Array* ContextProxiesL( TUint32 aUid ) = 0;

        /**
        * Deletes a context data item.
        * @param aArray The context extension array
        * @param aUid UID of the context.
        * @return ETrue if there is more data in the context
        */
        virtual TBool DeleteContextDataL( MWPContextExtensionArray& aArray, TUint32 aUid ) = 0;

        /**
        * Returns the number of context data items. Note that this
        * count is an approximate and should not be relied on except
        * for giving user progress feedback. 
        * @param aUid UID of the context
        * @return The number of context data items
        */
        virtual TInt ContextDataCountL( TUint32 aUid ) = 0;

        /**
        * Returns the current context.
        * @return Current context UID
        */
        virtual TUint32 CurrentContextL() = 0;

        /**
        * Sets the current context.
        * @param aUID The UID of the context
        */
        virtual void SetCurrentContextL( TUint32 aUid ) = 0;

        /**
        * Returns a specific context.
        * @param aTPS TPS whose context should be looked for
        * @return The UID of the context
        */
        virtual TUint32 ContextL( const TDesC& aTPS ) = 0;

        /**
        * Checks if a context exists.
        * @param aUid The UID of the context
        * @return ETrue if context exists
        */
        virtual TBool ContextExistsL( TUint32 aUid ) = 0;

        /**
        * Checks if a context exists.
        * @param aTPS The TPS of the context
        * @return ETrue if context exists
        */
        virtual TBool ContextExistsL( const TDesC& aTPS ) = 0;

        /**
        * Register an observer to context information.
        * @param aObserver The new observer
        */
        virtual void RegisterContextObserverL( 
            MWPContextObserver* aObserver ) = 0;

        /**
        * Unregister an observer to context information.
        * @param aObserver The observer to unregister
        * @return KErrNone, or an error code
        */
        virtual TInt UnregisterContextObserver( 
            MWPContextObserver* aObserver ) = 0;
    };

#endif /* MWPCONTEXTMANAGER_H */
