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
* Description:  Client's Results command buffering
*
*/


// INCLUDE FILES
#include "NSmlResultsContainer.h"
#include "NSmlCmdsBase.h"
#include "nsmlcliagconstants.h"

// ---------------------------------------------------------
// CNSmlResultsContainer::CNSmlResultsContainer
// Constructor, nothing special in here.
// ---------------------------------------------------------
//
CNSmlResultsContainer::CNSmlResultsContainer()
	{
	}

// ---------------------------------------------------------
// CNSmlResultsContainer::~CNSmlResultsContainer()
// Destructor
// ---------------------------------------------------------
//
CNSmlResultsContainer::~CNSmlResultsContainer()
	{
	if ( !iResultsArray )
		{
		return;
		}
	TInt arrayCount = iResultsArray->Count();
	for ( TInt i = 0; i < arrayCount ; i++ )
		{
		FreeResources( i );
		}
	delete iResultsArray; 
	}
	
// ---------------------------------------------------------
// CNSmlResultsContainer::NewL()
// Creates new instance of CNSmlResultsContainer. 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlResultsContainer* CNSmlResultsContainer::NewL( CNSmlCmdsBase* aSyncMLCmds)
	{
	CNSmlResultsContainer* self = new (ELeave) CNSmlResultsContainer();
	CleanupStack::PushL( self );
	self->ConstructL( aSyncMLCmds );
	CleanupStack::Pop(); //self
	return( self );
	}

// ---------------------------------------------------------
// CNSmlResultsContainer::CreateNewResultsL()
// Creates new empty Results element if needed.
// ---------------------------------------------------------
//
EXPORT_C TInt CNSmlResultsContainer::CreateNewResultsL( const TDesC8& aMsgRef, const TDesC8& aCmdRef, const SmlTarget_t* aTargetRef, const SmlSource_t* aSourceRef )
	{
	TResultsData resultsData;
	resultsData.iMsgRef = NULL;
	resultsData.iCmdRef = NULL; 
	resultsData.iResults = NULL;
	resultsData.iWasWritten = EFalse;
	resultsData.iStatusRef = -1;
	iResultsArray->AppendL( resultsData );
	TInt i = iResultsArray->Count() - 1;
	(*iResultsArray)[i].iMsgRef = aMsgRef.AllocL();
	(*iResultsArray)[i].iCmdRef = aCmdRef.AllocL();
	(*iResultsArray)[i].iResults = new( ELeave ) SmlResults_t;
	(*iResultsArray)[i].iResults->elementType = SML_PE_RESULTS;
	iSyncMLCmds->PcdataNewL( (*iResultsArray)[i].iResults->msgRef, aMsgRef );
	iSyncMLCmds->PcdataNewL( (*iResultsArray)[i].iResults->cmdRef, aCmdRef );
	if ( aTargetRef )
		{
		if ( aTargetRef->locURI )
			{
			CopyPcdataL( aTargetRef->locURI, (*iResultsArray)[i].iResults->targetRef );
			}
		}
	if ( aSourceRef )
		{
		if ( aSourceRef->locURI )
			{
			CopyPcdataL( aSourceRef->locURI, (*iResultsArray)[i].iResults->sourceRef );
			}
		}
	return iResultsArray->Count();
	}
// ---------------------------------------------------------
// CNSmlResultsContainer::SetCmdIDL()
// Sets CmdID element
// ---------------------------------------------------------
//
EXPORT_C void CNSmlResultsContainer::SetCmdIDL( TInt aEntryID, const SmlPcdata_t* aCmdID )
	{
	delete (*iResultsArray)[aEntryID-1].iResults->cmdID;
	(*iResultsArray)[aEntryID-1].iResults->cmdID = NULL; 
	CopyPcdataL( aCmdID, (*iResultsArray)[aEntryID-1].iResults->cmdID );
	}

// ---------------------------------------------------------
// CNSmlResultsContainer::AddItemL()
// Adds Item/Data element 
// ---------------------------------------------------------
EXPORT_C void CNSmlResultsContainer::AddItemL( TInt aEntryID, const CBufBase& aObject, const TDesC8& aType, const TDesC8& aFormat )
	{
	(*iResultsArray)[aEntryID-1].iResults->itemList = new( ELeave ) SmlItemList_t;
	(*iResultsArray)[aEntryID-1].iResults->itemList->item = new( ELeave ) SmlItem_t;
	iSyncMLCmds->PcdataNewL( (*iResultsArray)[aEntryID-1].iResults->itemList->item->data, CONST_CAST( CBufBase&, aObject ).Ptr( 0 ) );
	if ( (*iResultsArray)[aEntryID-1].iResults->targetRef )
		{
		HBufC* source;
		iSyncMLCmds->PCDATAToUnicodeLC( *(*iResultsArray)[aEntryID-1].iResults->targetRef, source );
		iSyncMLCmds->DoSourceL( (*iResultsArray)[aEntryID-1].iResults->itemList->item->source, *source );
		CleanupStack::PopAndDestroy(); // source
		}
	if ( aType.Length() > 0 || aFormat.Length() > 0 )
		{
		SmlMetInfMetInf_t* metInf;
		iSyncMLCmds->DoMetInfLC( metInf ); 
		if ( aType.Length() > 0 )
			{
			iSyncMLCmds->PcdataNewL( metInf->type, aType );
			}
		if ( aFormat.Length() > 0 )
			{
			iSyncMLCmds->PcdataNewL( metInf->format, aFormat );
			}
		iSyncMLCmds->DoMetaL( (*iResultsArray)[aEntryID-1].iResults->itemList->item->meta, metInf );
		CleanupStack::Pop(); //metInf
		}
	}
// ---------------------------------------------------------
// CNSmlResultsContainer::SetStatusID()
// Set Id of status buffer. The Id is an entry to CNSmlStatusContainer buffer   
// ---------------------------------------------------------
EXPORT_C void CNSmlResultsContainer::SetStatusID( TInt aEntryID, TInt aStatusEntryID )
	{
	(*iResultsArray)[aEntryID-1].iStatusRef = aStatusEntryID;
	}
		
// ---------------------------------------------------------
// CNSmlResultsContainer::RemoveWritten()
// Marks Results command written in outgoing XML document  
// ---------------------------------------------------------
EXPORT_C void CNSmlResultsContainer::RemoveWritten( TInt aEntryID )
	{
	(*iResultsArray)[aEntryID-1].iWasWritten = ETrue;
	}

// ---------------------------------------------------------
// CNSmlResultsContainer::Begin
// Sets index for reading to starting value 
// ---------------------------------------------------------
EXPORT_C void CNSmlResultsContainer::Begin()
	{
	iArrayInd = -1;
	}
// ---------------------------------------------------------
// CNSmlResultsContainer::NextResultsElement()
// Returns pointer to the next Results element 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlResultsContainer::NextResultsElement( SmlResults_t*& aResults )
	{
	TBool moreElements( ETrue );
	while ( moreElements )
		{
		iArrayInd++;
		if ( iArrayInd > iResultsArray->Count() - 1 )
			{
			RemoveAllWrittenOnes();
			iArrayInd = -1;
			moreElements = EFalse;
			}
		else
			{
			if ( ( !(*iResultsArray)[iArrayInd].iWasWritten ) && ( (*iResultsArray)[iArrayInd].iResults->itemList) )
				{
				aResults = (*iResultsArray)[iArrayInd].iResults;
				return ETrue;
				}
			} 
		}
	return moreElements; 
	}
// ---------------------------------------------------------
// CNSmlResultsContainer::CurrentEntryID()
// Returns current results ID 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlResultsContainer::CurrentEntryID() const
	{
	return( iArrayInd + 1);
	}

// ---------------------------------------------------------
// CNSmlResultsContainer::AnyResults()
// 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlResultsContainer::AnyResults() const
	{
	TInt arrayCount( iResultsArray->Count() );

	for ( TInt i = 0; i < arrayCount ; i++ )
		{
		if ( ( !(*iResultsArray)[i].iWasWritten )  &&  ( (*iResultsArray)[i].iResults->itemList) )
			{
			return ETrue;
			}
		}
		
	return EFalse;
	}
// ---------------------------------------------------------
// CNSmlResultsContainer::StatusID()
// 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlResultsContainer::StatusID( TInt aEntryID ) const
	{
	return (*iResultsArray)[aEntryID-1].iStatusRef;
	}
// 
// private methods
//

// ---------------------------------------------------------
// CNSmlResultsContainer::ConstructL()
// Two-way construction. Constructor may leave in EPOC.
// ---------------------------------------------------------
//
void CNSmlResultsContainer::ConstructL( CNSmlCmdsBase* aSyncMLCmds )
	{
	iSyncMLCmds = aSyncMLCmds;
	iResultsArray = new( ELeave ) CArrayFixFlat<TResultsData>(KNSmlResultsArrayGranularity);
	iArrayInd = -1;
	}
// ---------------------------------------------------------
// CNSmlResultsContainer::CopyPcdataL()
// Deep copy to Pcdata element
// Copied element is not pushed to Cleanup stack
// ---------------------------------------------------------
//
void CNSmlResultsContainer::CopyPcdataL( const SmlPcdata_t* aFromPcdata, SmlPcdata_t*& aToPcdata ) const
	{
	aToPcdata = NULL;
	if ( aFromPcdata )
		{
		SmlPcdata_t* newPcdata = new( ELeave ) SmlPcdata_t;
		CleanupStack::PushL( newPcdata );
		newPcdata->length = aFromPcdata->length;
		newPcdata->contentType = SML_PCDATA_OPAQUE;
		newPcdata->extension = SML_EXT_UNDEFINED; 
		if ( aFromPcdata->content )
			{
			newPcdata->content = User::AllocL( 	newPcdata->length );
			TPtr8 fromPtr ( (TUint8*) aFromPcdata->content, aFromPcdata->length );
			fromPtr.SetLength( fromPtr.MaxLength() );
			TPtr8 toPtr ( (TUint8*) newPcdata->content, newPcdata->length );
			toPtr.SetLength( toPtr.MaxLength() );
			toPtr = fromPtr;
			}	
		CleanupStack::Pop();  //newPcdata 
		aToPcdata = newPcdata;
		}
	}

// ---------------------------------------------------------
// CNSmlResultsContainer::RemoveAllWrittenOnes()
// Removes all written Results elements 
// ---------------------------------------------------------
void CNSmlResultsContainer::RemoveAllWrittenOnes()
	{
	for ( TInt i = iResultsArray->Count() - 1; i >= 0; i--)
		{
		if ( (*iResultsArray)[i].iWasWritten )
			{
			FreeResources( i );
			iResultsArray->Delete( i );
			}
		}
	iResultsArray->Compress();
	iArrayInd = -1;
	}

// ---------------------------------------------------------
// CNSmlResultsContainer::FreeResources()
// Frees resources of an item
// ---------------------------------------------------------
void CNSmlResultsContainer::FreeResources( TInt aI )
	{
	delete (*iResultsArray)[aI].iMsgRef;
	delete (*iResultsArray)[aI].iCmdRef;
	delete (*iResultsArray)[aI].iResults;
	}
