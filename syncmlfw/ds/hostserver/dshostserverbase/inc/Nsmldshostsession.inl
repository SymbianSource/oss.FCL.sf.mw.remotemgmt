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
* Description:  inline methods for DS host server.
*
*/


#ifndef __NSMLDSHOSTSESSION_INL__
#define __NSMLDSHOSTSESSION_INL__
 
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::TMemPtr()
// ------------------------------------------------------------------------------------------------
inline CNSmlDSHostSession::TMemPtr::TMemPtr()
	{
#ifdef __HOST_SERVER_MTEST__	
	iChunkSize = KNSmlDSHostChunkMinSize;
#endif
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::Des
// ------------------------------------------------------------------------------------------------
inline TPtr8 CNSmlDSHostSession::TMemPtr::Des()
    {
    return TPtr8( Ptr8(), Size() );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::Mid
// ------------------------------------------------------------------------------------------------
inline TPtr8 CNSmlDSHostSession::TMemPtr::Mid( TInt aPos, TInt aLength )
    {
    TInt maxLength = Size() - aPos;
    return TPtr8( Ptr8() + aPos, Min<TInt>( aLength, maxLength ) );
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::CopyTo
// ------------------------------------------------------------------------------------------------
inline void CNSmlDSHostSession::TMemPtr::CopyTo(TDes8& dest) const
    {
    dest.Copy( Ptr8(), dest.MaxLength() );
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::Size
// ------------------------------------------------------------------------------------------------    
inline TInt CNSmlDSHostSession::TMemPtr::Size() const
    {
#ifdef __HOST_SERVER_MTEST__	
	return iChunkSize;
#else
	return iChunk.Size();
#endif
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::Adjust
// ------------------------------------------------------------------------------------------------        
inline TInt CNSmlDSHostSession::TMemPtr::Adjust( TInt aNewSize )
	{
#ifdef __HOST_SERVER_MTEST__
	iChunkSize = aNewSize;
#endif
	return iChunk.Adjust( aNewSize );
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::Ptr8
// ------------------------------------------------------------------------------------------------
inline TUint8* CNSmlDSHostSession::TMemPtr::Ptr8()
    {
    return iChunk.Base();
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::Ptr8
// ------------------------------------------------------------------------------------------------    
inline const TUint8* CNSmlDSHostSession::TMemPtr::Ptr8() const
    {
    return iChunk.Base();
    }
    
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::TMemPtr::Ptr
// ------------------------------------------------------------------------------------------------
inline TAny* CNSmlDSHostSession::TMemPtr::Ptr() const
    {
    return iChunk.Base();
    }

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CNSmlServerDSHostItem::Uid
// ------------------------------------------------------------------------------------------------    
inline TSmlDbItemUid& CNSmlDSHostSession::CNSmlServerDSHostItem::Uid()
	{
	return iUid;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CNSmlServerDSHostItem::FieldChange
// ------------------------------------------------------------------------------------------------		
inline TBool& CNSmlDSHostSession::CNSmlServerDSHostItem::FieldChange()
	{
	return iFieldChange;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CNSmlServerDSHostItem::Size
// ------------------------------------------------------------------------------------------------
inline TInt& CNSmlDSHostSession::CNSmlServerDSHostItem::Size()
	{
	return iSize;
	}
	
// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CNSmlServerDSHostItem::ParentUid
// ------------------------------------------------------------------------------------------------	
inline TSmlDbItemUid& CNSmlDSHostSession::CNSmlServerDSHostItem::ParentUid()
	{
	return iParentUid;
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDSHostSession::CNSmlDSChangedItemsFetcher::ChangedItems
// ------------------------------------------------------------------------------------------------
inline const RNSmlDbItemModificationSet* CNSmlDSChangedItemsFetcher::ChangedItems() const
    {
    return iChangedItems;
    }
    
#endif

// End of File
