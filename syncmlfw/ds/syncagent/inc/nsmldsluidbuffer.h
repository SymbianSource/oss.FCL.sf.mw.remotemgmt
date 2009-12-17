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
* Description:  LUID buffering
*
*/


#ifndef __NSMLDSLUIDBUFFER_H__
#define __NSMLDSLUIDBUFFER_H__

// INCLUDES
#include <e32base.h>
#include <SmlDataSyncDefs.h>
#include "nsmldshostclient.h"

// FORWARD DECLARATIONS
class CNSmlDSHostClient;
class CNSmlDSContentItem;

// CLASS DECLARATION

/**
* Class for LUID (Local UID) buffering.
*
* @lib nsmldsagent.lib
*/
class CNSmlDSLUIDBuffer : public CBase 
    {
    public: // constructors and destructor
        /**
        * C++ constructor.
        * @param aHostClient Host Client used for communication with the Host Session.
        * @param aContentItem Sync content whose LUIDs are buffered.
        */
        CNSmlDSLUIDBuffer( CNSmlDSHostClient& aHostClient, CNSmlDSContentItem& aContentItem );

        /**
        * Destructor.
        */
        ~CNSmlDSLUIDBuffer();

    public: // new functions
        /**
        * Gets the current UID.
        * @param aLocalUID On return contains the current UID.
        */
        void CurrentUID( TSmlDbItemUid& aLocalUID ) const;

        /**
        * Gets the first UID and the command associated to its modification type from buffer.
        * @param aLocalUID On return contains the next UID.
        * @param aCmdName On return contains the command associated with the returned UID's
        *        modification type.
        * @return KErrNone if completed successfully or KErrEof if the buffer is empty
        */
        TInt NextUIDL( TSmlDbItemUid& aLocalUID, TDes8& aCmdName );

        /**
        * Gets the total number of UIDs, both in the buffer and written.
        * @return TInt representing the number of UIDs in the buffer.
        */
        TInt ClientItemCount() const;
		
        /**
        * Sets the current UID as written and removes it from the buffer.
        */
        void SetCurrentItemWritten();
		
        /**
        * Gets UIDs of all modified items from datastore to the buffer asynchronously.
        * @param aResultCode On return contains the result code of the operation.
        */
        void FetchModificationsL( TInt& aResultCode );
		
    private: // constructors & operators
        /**
        * Prohibit copy constructor.
        */
        CNSmlDSLUIDBuffer( const CNSmlDSLUIDBuffer& aOther );
		
        /**
        * Prohibit assignment operator.
        */
        CNSmlDSLUIDBuffer& operator=( const CNSmlDSLUIDBuffer& aOther );

    private: // data
        // internal states
        enum TState    
            {		
            EModifications = 1,
            EEnd
            };
        // current state
        TState iCurrState;		
        // array of buffered modification items
        RNSmlDbItemModificationSet iModificationSet;		
        // reference to the ds hostclient
        CNSmlDSHostClient& iHostClient;
        // reference to the current content (datastore) data
        CNSmlDSContentItem& iContentItem;
        // the number of written items
        TInt iWrittenItems;
    };

#endif // __NSMLDSLUIDBUFFER_H__

// End of File
