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


// INCLUDE FILES
#include <nsmldebug.h>
#include "nsmldsluidbuffer.h"
#include "nsmlcliagconstants.h"
#include "nsmldshostclient.h"
#include "nsmldscontent.h"
#include "NsmlDSContentItem.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDSLUIDBuffer::CNSmlDSLUIDBuffer
// C++ constructor.
// -----------------------------------------------------------------------------
//
CNSmlDSLUIDBuffer::CNSmlDSLUIDBuffer( 
    CNSmlDSHostClient& aHostClient, 
    CNSmlDSContentItem& aContentItem ) : iHostClient( aHostClient ), 
                                         iContentItem( aContentItem ),
                                         iWrittenItems( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CNSmlDSUIDBuffer::~CNSmlDSUIDBuffer
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlDSLUIDBuffer::~CNSmlDSLUIDBuffer()
    {
    iModificationSet.Close();
    }
	
// -----------------------------------------------------------------------------
// CNSmlDSLUIDBuffer::NextUIDL
// Gets the first UID and its modification type from the buffer. The aCmdName
// parameter is determined by the modification type.
// -----------------------------------------------------------------------------
//
TInt CNSmlDSLUIDBuffer::NextUIDL( TSmlDbItemUid& aLocalUID, TDes8& aCmdName )
    {
    TInt ret( KErrNone );
	
    switch ( iCurrState )
        {
        case EModifications:
            if ( iModificationSet.ItemCount() > 0 )
                {
                aLocalUID = iModificationSet.ItemAt( 0 ).ItemId();
				
                switch ( iModificationSet.ItemAt( 0 ).ModificationType() )
                    {
                    case TNSmlDbItemModification::ENSmlDbItemAdd:
                        aCmdName = KNSmlAgentAdd;
                        break;
					
                    case TNSmlDbItemModification::ENSmlDbItemModify:
                        aCmdName = KNSmlAgentReplace;
                        break;
					
                    case TNSmlDbItemModification::ENSmlDbItemDelete:
                        aCmdName = KNSmlAgentDelete;
                        break;
					
                    case TNSmlDbItemModification::ENSmlDbItemMove:
                        aCmdName = KNSmlAgentMove;
                        break;
					
                    case TNSmlDbItemModification::ENSmlDbItemSoftDelete:
                        aCmdName = KNSmlAgentSoftDelete;
                        break;
                        
                    default:
                    	User::Leave( KErrGeneral );
                    }
                }
            else
                {
                iModificationSet.Reset();
                iCurrState = EEnd;
                ret = KErrEof;
                }
            break;

        case EEnd:
            return KErrEof;
            
        default:
        	User::Leave( KErrGeneral );
        }
		
    return ret;
    }

// -----------------------------------------------------------------------------
// CNSmlDSLUIDBuffer::CurrentUID
// Returns the first UID in the buffer. 
// -----------------------------------------------------------------------------
//
void CNSmlDSLUIDBuffer::CurrentUID( TSmlDbItemUid& aLocalUID ) const
    {
    if ( iModificationSet.ItemCount() > 0 )
        {
        aLocalUID = iModificationSet.ItemAt( 0 ).ItemId();
        }
    }
	
// -----------------------------------------------------------------------------
// CNSmlDSLUIDBuffer::ClientItemCount
// Returns the total number of UIDs (both those still in the buffer and those 
// already written).
// -----------------------------------------------------------------------------
//
TInt CNSmlDSLUIDBuffer::ClientItemCount() const
    {
    return iModificationSet.ItemCount() + iWrittenItems;
    }

// -----------------------------------------------------------------------------
// CNSmlDSLUIDBuffer::SetCurrentItemWritten
// Removes the current UID from the buffer. 
// -----------------------------------------------------------------------------
//
void CNSmlDSLUIDBuffer::SetCurrentItemWritten()
    {
    iModificationSet.RemoveItem( 0 );
    ++iWrittenItems;
    }

// -----------------------------------------------------------------------------
// CNSmlDSLUIDBuffer::FetchModificationsL
// Gets UIDs of all modified items from datastore to the buffer asynchronously.
// -----------------------------------------------------------------------------
//
void CNSmlDSLUIDBuffer::FetchModificationsL( TInt& aResultCode )
    {
    DBG_FILE(_S8("CNSmlDSLUIDBuffer::FetchModificationsL begins"));

    iModificationSet.Reset();
	
    iHostClient.AllItemsL( iModificationSet, iContentItem.iImplementationUID, 
                           *iContentItem.iStoreName, aResultCode );
	
    iCurrState = EModifications;
	
    DBG_FILE(_S8("CNSmlDSLUIDBuffer::FetchModificationsL ends"));
    }

// End of File
