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
* Description:  Client's Status command buffering
*
*/



//#include "nsmlcliagdefines.h"
#include "NSmlStatusContainer.h"
#include "nsmlcliagconstants.h"
#include "nsmlerror.h"
#include "smlmetinfdtd.h"

// ---------------------------------------------------------
// CNSmlStatusContainer::CNSmlStatusContainer
// Constructor, nothing special in here.
// ---------------------------------------------------------
//
CNSmlStatusContainer::CNSmlStatusContainer()
	{
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::ConstructL()
// Two-way construction. Constructor may leave in EPOC.
// ---------------------------------------------------------
//
void CNSmlStatusContainer::ConstructL(TBool aClearText)
	{
	iClearText = aClearText;
	iStatusArray = new( ELeave ) CArrayFixFlat<TStatusData>(KNSmlStatusArrayGranularity);
	iArrayInd = -1;
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::~CNSmlStatusContainer()
// Destructor
// ---------------------------------------------------------
//

CNSmlStatusContainer::~CNSmlStatusContainer()
	{
	delete iMsgRef;
	if ( !iStatusArray )
		{
		return;
		}
	TInt arrayCount = iStatusArray->Count();
	for ( TInt i = 0; i < arrayCount ; i++ )
		{
		FreeResources( i );
		}
	delete iStatusArray; 
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::NewL()
// Creates new instance of CNSmlStatusContainer. 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlStatusContainer* CNSmlStatusContainer::NewL( TBool aClearText)
	{
	CNSmlStatusContainer* self = CNSmlStatusContainer::NewLC(aClearText);
	CleanupStack::Pop();
	return( self );
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::NewLC()
// Creates new instance of CNSmlStatusContainer 
// Leaves instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlStatusContainer* CNSmlStatusContainer::NewLC(TBool aClearText)
	{
	CNSmlStatusContainer* self = new (ELeave) CNSmlStatusContainer();
	CleanupStack::PushL( self );
	self->ConstructL(aClearText);
	return( self );
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::InitMsgRefL
// Creates new empty Status element and adds it to the array
// ---------------------------------------------------------
//
void CNSmlStatusContainer::InitMsgRefL( const SmlPcdata_t* aMsgRef )
	{
	delete iMsgRef;
	iMsgRef = NULL;
	CopyPcdataL( aMsgRef, iMsgRef );
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::CreateNewStatusElementL()
// Creates new empty Status element and adds it to the array
// ---------------------------------------------------------
//
EXPORT_C TInt CNSmlStatusContainer::CreateNewStatusElementL()
	{
	TStatusData statusData;
	statusData.status = NULL;
	statusData.noResponse = EFalse;
	statusData.wasWritten = EFalse;
	statusData.atomicOrSequenceID = 0;
	statusData.performedInAtomic = EFalse;
	statusData.statusIsFixed = EFalse;
	iStatusArray->AppendL( statusData );
	TInt i = iStatusArray->Count() - 1;
	(*iStatusArray)[i].status = new( ELeave ) SmlStatus_t;
	(*iStatusArray)[i].status->elementType = SML_PE_STATUS;
	CopyPcdataL( iMsgRef, (*iStatusArray)[i].status->msgRef );
	return iStatusArray->Count();
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::SetCmdIDL()
// Sets CmdID element
// ---------------------------------------------------------
//
void CNSmlStatusContainer::SetCmdIDL( TInt aEntryID, const SmlPcdata_t* aCmdID )
	{
	delete (*iStatusArray)[aEntryID-1].status->cmdID;
	(*iStatusArray)[aEntryID-1].status->cmdID = NULL;
	CopyPcdataL( aCmdID, (*iStatusArray)[aEntryID-1].status->cmdID );
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::SetMsgRefL()
// Sets MsgRef element
// ---------------------------------------------------------
//
void CNSmlStatusContainer::SetMsgRefL( TInt aEntryID, const SmlPcdata_t* aMsgRef )
	{
	delete (*iStatusArray)[aEntryID-1].status->msgRef;
	(*iStatusArray)[aEntryID-1].status->msgRef = NULL;
	CopyPcdataL( aMsgRef, (*iStatusArray)[aEntryID-1].status->msgRef );
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::SetCmdRefL()
// Sets CmdRef element
// ---------------------------------------------------------
//
EXPORT_C void CNSmlStatusContainer::SetCmdRefL( TInt aEntryID, const SmlPcdata_t* aCmdRef )
	{
	delete (*iStatusArray)[aEntryID-1].status->cmdRef;
	(*iStatusArray)[aEntryID-1].status->cmdRef = NULL;
	CopyPcdataL( aCmdRef, (*iStatusArray)[aEntryID-1].status->cmdRef );
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::SetCmdL()
// Sets CmdRef element
// ---------------------------------------------------------
//
EXPORT_C void CNSmlStatusContainer::SetCmdL( TInt aEntryID, const TDesC8& aCmd )
	{
	delete (*iStatusArray)[aEntryID-1].status->cmd;
	(*iStatusArray)[aEntryID-1].status->cmd = NULL;
	CreatePcdataL( (*iStatusArray)[aEntryID-1].status->cmd, aCmd );
	}
	
// ---------------------------------------------------------
// CNSmlStatusContainer::SetStatusCodeL()
// Sets Status code to Data element 
// Created element is not pushed to Cleanup stack
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::SetStatusCodeL( TInt aEntryID, TInt aStatusCode, TBool aFixStatus )
	{
	//status code in data element
	if ( !(*iStatusArray)[aEntryID-1].statusIsFixed )
		{
		HBufC8* statusCode = HBufC8::NewLC( KNSmlAgentStatusCodeLength );	
		statusCode->Des().Num( aStatusCode );
		if ( !(*iStatusArray)[aEntryID-1].status->data )
			{
			CreatePcdataL( (*iStatusArray)[aEntryID-1].status->data, *statusCode );
			}
		else
			{
			// Don't overwrite status 213 with 200, otherwise replace 
			// existing status
			TInt existingStatus( TNSmlError::ESmlStatusOK );
			TBool ok = StatusCodeVal ( (*iStatusArray)[aEntryID-1].status->data, existingStatus );
			if ( !( ok &&  existingStatus == TNSmlError::ESmlStatusItemAccepted &&
			        aStatusCode == TNSmlError::ESmlStatusOK ) )
		        {
    			delete (*iStatusArray)[aEntryID-1].status->data;
	    		(*iStatusArray)[aEntryID-1].status->data = NULL;
		    	CreatePcdataL( (*iStatusArray)[aEntryID-1].status->data, *statusCode ); 			        
			    }
			}
		if ( aFixStatus)
			{
			(*iStatusArray)[aEntryID-1].statusIsFixed = ETrue;
			}
		}
	CleanupStack::PopAndDestroy(); //statusCode
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::SetChalL()
// Creates Chal element 
// ---------------------------------------------------------
void CNSmlStatusContainer::SetChalL( TInt aEntryID, const TDesC8& aNonce )
	{
	(*iStatusArray)[aEntryID-1].status->chal = new( ELeave ) SmlChal_t;
	SmlMetInfMetInf_t* metInf = new( ELeave ) SmlMetInfMetInf_t;
	CleanupStack::PushL( metInf );
	CreatePcdataL( metInf->type, KNSmlAgentAuthMD5 );
	CreatePcdataL( metInf->format, KNSmlAgentBase64Format );
	CreatePcdataL( metInf->nextnonce, aNonce );
	(*iStatusArray)[aEntryID-1].status->chal->meta = new( ELeave ) SmlPcdata_t; 
	(*iStatusArray)[aEntryID-1].status->chal->meta->length = 0;
	(*iStatusArray)[aEntryID-1].status->chal->meta->content = ( TAny* ) metInf;
	CleanupStack::Pop(); //metInf
	(*iStatusArray)[aEntryID-1].status->chal->meta->contentType = SML_PCDATA_EXTENSION;
	(*iStatusArray)[aEntryID-1].status->chal->meta->extension = SML_EXT_METINF;
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::SetNoResponse
// Sets NoResponse flag 
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::SetNoResponse( TInt aEntryID, TBool aNoResponse )
	{
	(*iStatusArray)[aEntryID-1].noResponse = aNoResponse;
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::SetAtomicOrSequenceId
// Sets Atomic or Sequence Id  
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::SetAtomicOrSequenceId( TInt aEntryID, TInt aAtomicOrSequenceID )
	{
	(*iStatusArray)[aEntryID-1].atomicOrSequenceID = aAtomicOrSequenceID;
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::SetPerformedInAtomic
// Mark an item be performed inside Atomic 
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::SetPerformedInAtomic( TInt aEntryID )
	{
	(*iStatusArray)[aEntryID-1].performedInAtomic = ETrue;
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::SetStatusCodeToAtomicOrSequenceCmdL
// Change status code for Atomic command 
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::SetStatusCodeToAtomicOrSequenceCmdL( TInt aAtomicOrSequenceID, TInt aStatusCode, const TDesC8& aCmd )
	{
	TInt arrayCount = iStatusArray->Count();
	for ( TInt i = 0; i < arrayCount; i++ )
		{
		if ( (*iStatusArray)[i].atomicOrSequenceID == aAtomicOrSequenceID )
			{
			if ( (*iStatusArray)[i].status->cmd )
				{
				if ( (*iStatusArray)[i].status->cmd->content )
					{
					TPtrC8 cmd( (TUint8*) (*iStatusArray)[i].status->cmd->content, (*iStatusArray)[i].status->cmd->length );
					if ( cmd == aCmd )
						{
						SetStatusCodeL( i + 1, aStatusCode );
						break;
						}
					}
				}
			}
		}
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::SetStatusCodesInAtomicL
// Change status code to all items in Atomic or to items which 
// have been already performed
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::SetStatusCodesInAtomicL( TInt aAtomicID, TInt aStatusCode, TBool aOnlyForPerformed )
	{
	TInt arrayCount = iStatusArray->Count();
	for ( TInt i = 0; i < arrayCount; i++ )
		{
		if ( !(*iStatusArray)[i].atomicOrSequenceID == aAtomicID )
			{
			continue;
			}
		if ( (!aOnlyForPerformed) || ( aOnlyForPerformed && (*iStatusArray)[i].performedInAtomic ) )
			{
			SetStatusCodeL( i + 1, aStatusCode );
			}
		}
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::AddTargetRefL
// Adds TargetRef element
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::AddTargetRefL( TInt aEntryID, const SmlTarget_t* aTarget )
	{
	if ( !aTarget )
		{
		return;
		}
	if ( !aTarget->locURI )
		{
		return;
		}
	if ( !aTarget->locURI->content )
		{
		return;
		}
	SmlTargetRefList_t** targetRefList;
	targetRefList = &(*iStatusArray)[aEntryID-1].status->targetRefList;
	while( *targetRefList )
		{
		targetRefList = &(*targetRefList)->next;
		} 
	*targetRefList = new( ELeave ) SmlTargetRefList_t;
	CreateTargetRefL( aTarget, (*targetRefList)->targetRef ); 
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::AddSourceRefL
// Adds SourceRef element
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::AddSourceRefL( TInt aEntryID, const SmlSource_t* aSource )
	{
	if ( !aSource )
		{
		return;
		}
	if ( !aSource->locURI )
		{
		return;
		}
	if ( !aSource->locURI->content )
		{
		return;
		}
	SmlSourceRefList_t** sourceRefList;
	sourceRefList = &(*iStatusArray)[aEntryID-1].status->sourceRefList;
	while ( *sourceRefList )
		{
		sourceRefList = &(*sourceRefList)->next;
		} 
	*sourceRefList = new( ELeave ) SmlSourceRefList_t;
	CreateSourceRefL( aSource, (*sourceRefList)->sourceRef ); 
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::AddItemDataL
// Adds Item/Data element 
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::AddItemDataL (TInt aEntryID, const SmlPcdata_t* aData )
	{
	SmlItemList_t* newItemList = new( ELeave ) SmlItemList_t;
	CleanupStack::PushL( newItemList );
	newItemList->item = new( ELeave ) SmlItem_t;
	CopyPcdataL( aData, newItemList->item->data );
	SmlItemList_t** itemList;
	itemList = &(*iStatusArray)[aEntryID-1].status->itemList;
	while ( *itemList )
		{
		itemList = &(*itemList)->next;
		}
	*itemList = newItemList;
	CleanupStack::Pop(); //newItemList
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::RemoveWritten()
// Marks Status command written in outgoing XML document  
// ---------------------------------------------------------
void CNSmlStatusContainer::RemoveWritten( TInt aEntryID )
	{
	(*iStatusArray)[aEntryID-1].wasWritten = ETrue;
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::Begin
// Sets index for reading to starting value 
// ---------------------------------------------------------
EXPORT_C void CNSmlStatusContainer::Begin()
	{
	iArrayInd = -1;
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::NextStatusElement()
// Returns pointer to the next Status element 
// ---------------------------------------------------------
EXPORT_C TBool CNSmlStatusContainer::NextStatusElement( SmlStatus_t*& aStatus, TBool aOnlyIfResponse )
	{
	TBool moreElements( ETrue );
	while ( moreElements )
		{
		iArrayInd++;
		if ( iArrayInd > iStatusArray->Count() - 1 )
			{
			RemoveAllWrittenOnes();
			iArrayInd = -1;
			moreElements = EFalse;
			}
		else
			{
			if ( aOnlyIfResponse && (*iStatusArray)[iArrayInd].noResponse )
				{
				}
			else 
			if ( !(*iStatusArray)[iArrayInd].wasWritten )
				{
				aStatus = (*iStatusArray)[iArrayInd].status;
				return ETrue;
				}
			}
		}
	return moreElements;
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::CurrentEntryID()
// Returns current status ID 
// ---------------------------------------------------------
TInt CNSmlStatusContainer::CurrentEntryID() const
	{
	return( iArrayInd + 1);
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::LastEntryID()
// Returns last status ID 
// ---------------------------------------------------------
EXPORT_C TInt CNSmlStatusContainer::LastEntryID() const
	{
	return iStatusArray->Count();
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::AnyOtherThanSyncHdrStatus()
// 
// ---------------------------------------------------------
TBool CNSmlStatusContainer::AnyOtherThanOkSyncHdrStatus() const
	{
	TBool statusPresents = EFalse;
	TInt arrayCount = iStatusArray->Count();
	for ( TInt i = 0; i < arrayCount; i++ )
		{
		if ( !(*iStatusArray)[i].noResponse )
			{
			if ( (*iStatusArray)[i].status->cmd )
				{
				if ( (*iStatusArray)[i].status->cmd->content )
					{
					TPtrC8 cmd( (TUint8*) (*iStatusArray)[i].status->cmd->content, (*iStatusArray)[i].status->cmd->length );
					if ( cmd == KNSmlAgentSyncHdr )
						{
						if ( (*iStatusArray)[i].status->data )
							{
							if ( (*iStatusArray)[i].status->data->content )
								{
								TPtrC8 statusCode( (TUint8*) (*iStatusArray)[i].status->data->content, (*iStatusArray)[i].status->data->length );
								TLex8 lexicalValue( statusCode );
								TInt statusCodeNum;
								if ( lexicalValue.Val( statusCodeNum ) != KErrNone )
									{
									if ( statusCodeNum != TNSmlError::ESmlStatusOK &&
										 statusCodeNum != TNSmlError::ESmlStatusAuthenticationAccepted )
										{
										statusPresents = ETrue;
										break;
										}
									}
								}
							}
						}
					else
						{
						statusPresents = ETrue;
						break;
						}
					}
				}
			}
		}	
	return statusPresents;
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::CreatePcdataL()
// Creates Pcdata 
// Created element is not pushed to Cleanup stack
// ---------------------------------------------------------
void CNSmlStatusContainer::CreatePcdataL( SmlPcdata_t*& aPcdata, const TDesC8& aContent ) const
	{
	aPcdata = new( ELeave ) SmlPcdata_t;
	aPcdata->SetDataL( aContent );
	aPcdata->contentType = SML_PCDATA_OPAQUE;   
	aPcdata->extension = SML_EXT_UNDEFINED; 
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::CopyPcdataL()
// Deep copy to Pcdata element
// Copied element is not pushed to Cleanup stack
// ---------------------------------------------------------
//
void CNSmlStatusContainer::CopyPcdataL( const SmlPcdata_t* aFromPcdata, SmlPcdata_t*& aToPcdata ) const
	{
	aToPcdata = NULL;
	if ( aFromPcdata )
		{
		SmlPcdata_t* newPcdata = new( ELeave ) SmlPcdata_t;
		CleanupStack::PushL( newPcdata );
		newPcdata->length = aFromPcdata->length;
		if (iClearText)
			{
			newPcdata->contentType = SML_PCDATA_STRING;
			}
		else
			{
			newPcdata->contentType = SML_PCDATA_OPAQUE;
			}   
		newPcdata->extension = SML_EXT_UNDEFINED; 
		if ( aFromPcdata->content )
			{
			if ( aFromPcdata->contentType == SML_PCDATA_EXTENSION && aFromPcdata->extension == SML_EXT_METINF )
				{
				newPcdata->contentType = SML_PCDATA_EXTENSION;
				newPcdata->extension = aFromPcdata->extension;
				newPcdata->length = 0;
				CopyMetInfL( aFromPcdata->content, newPcdata->content );
				}
			else
				{
				newPcdata->content = User::AllocL( 	newPcdata->length );
				TPtr8 fromPtr ( (TUint8*) aFromPcdata->content, aFromPcdata->length );
				fromPtr.SetLength( fromPtr.MaxLength() );
				TPtr8 toPtr ( (TUint8*) newPcdata->content, newPcdata->length );
				toPtr.SetLength( toPtr.MaxLength() );
				toPtr = fromPtr;
				}
			}	
		CleanupStack::Pop();  //newPcdata 
		aToPcdata = newPcdata;
		}
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::CopyMetInfL()
// Deep copy to Meta Info
// ---------------------------------------------------------
void CNSmlStatusContainer::CopyMetInfL ( const void* aFromMetInf, void*& aToMetInf ) const
	{
	if ( aFromMetInf )
		{
		SmlMetInfMetInf_t* fromMetInf = (SmlMetInfMetInf_t*) aFromMetInf;
		SmlMetInfMetInf_t* toMetInf = new( ELeave ) SmlMetInfMetInf_t;
		if ( fromMetInf->anchor )
			{
			toMetInf->anchor = new( ELeave ) SmlMetInfAnchor_t;
			CopyPcdataL( fromMetInf->anchor->next, toMetInf->anchor->next );
			}
		aToMetInf = toMetInf;
		}
	}
// ---------------------------------------------------------
// CNSmlStatusContainer::CreateTargetOrSourceRefL()
// Creates TargetRef or SourceRef element
// ---------------------------------------------------------
void CNSmlStatusContainer::CreateSourceRefL( const SmlSource_t* aSource, SmlPcdata_t*& aSourceRef ) const
	{
	CopyPcdataL( aSource->locURI, aSourceRef );
	}


// ---------------------------------------------------------
// CNSmlStatusContainer::CreateTargetOrSourceRefL()
// Creates TargetRef or SourceRef element
// ---------------------------------------------------------
void CNSmlStatusContainer::CreateTargetRefL( const SmlTarget_t* aTarget, SmlPcdata_t*& aTargetRef ) const
	{
	CopyPcdataL( aTarget->locURI, aTargetRef );
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::RemoveAllWrittenOnes()
// Removes all written Status elements 
// ---------------------------------------------------------
void CNSmlStatusContainer::RemoveAllWrittenOnes()
	{
	for ( TInt i = iStatusArray->Count() - 1; i >= 0; i--)
		{
		if ( (*iStatusArray)[i].wasWritten )
			{
			FreeResources( i );
			iStatusArray->Delete( i );
			}
		}
	iStatusArray->Compress();
	iArrayInd = -1;
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::FreeResources()
// Frees resources of an item
// ---------------------------------------------------------
void CNSmlStatusContainer::FreeResources( TInt aI )
	{
	delete (*iStatusArray)[aI].status;
	}

// ---------------------------------------------------------
// CNSmlStatusContainer::StatusCodeVal()
// Converts status code to integer value
// ---------------------------------------------------------
TBool CNSmlStatusContainer::StatusCodeVal ( SmlPcdata_t*& aStatusCodeData, TInt& aVal ) const
    {
    TBool ret(EFalse);
	if ( aStatusCodeData->content )
	    {
	    TPtrC8 statusCode( (TUint8*) aStatusCodeData->content, aStatusCodeData->length );
	    TLex8 lexicalValue( statusCode );
	    TInt statusCodeNum( TNSmlError::ESmlStatusOK );
        if ( lexicalValue.Val( statusCodeNum ) == KErrNone )
		    {
		    ret = ETrue;
		    aVal = statusCodeNum;
			}
		else
		    {
		    aVal = 0;
		    }
	    }
	return ret;
    }
    

