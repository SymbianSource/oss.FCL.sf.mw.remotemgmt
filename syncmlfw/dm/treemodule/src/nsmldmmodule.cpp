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
* Description:  DM Tree module
*
*/


#include <e32def.h>
#include <nsmldebug.h>
#include <nsmldmmodule.h>
#include "nsmldmddf.h"
#include "nsmldmcommandbuffer.h"
#include "nsmldmtreeconstants.h"
#include "nsmldmuri.h"
#include <featmgr.h>

_LIT8(KNSmlDmUriDDF, "DDF");
_LIT8(KNSmlDmPropetyACL, "ACL");
_LIT8(KNSmlDmPropetyFormat, "Format");
_LIT8(KNSmlDmPropetyName, "Name");
_LIT8(KNSmlDmPropetySize, "Size");
_LIT8(KNSmlDmPropetyType, "Type");
_LIT8(KNSmlDmPropertyCaseSense, "CaseSense");
_LIT8(KNSmlDmDDFDevInfoModDDF, "DevInfo/Ext/ModDDF");

// ===========================================================================
// CSmlDmModule
// ===========================================================================

// ----------------------------------------------------------------------------
// CNSmlDmModule::~CNSmlDmModule()
// ----------------------------------------------------------------------------
EXPORT_C CNSmlDmModule::~CNSmlDmModule()
	{
	delete iDDF;
	delete iCommandBuffer;
	iPropResults.ResetAndDestroy();
	FeatureManager::UnInitializeLib();
	}

// ----------------------------------------------------------------------------
// CNSmlDmModule* CNSmlDmModule::NewL( MNSmlDmModuleCallBack* aCallBack)
// ----------------------------------------------------------------------------
EXPORT_C CNSmlDmModule* CNSmlDmModule::NewL( MNSmlDmModuleCallBack* aCallBack)
	{
	CNSmlDmModule* self = new (ELeave) CNSmlDmModule();
	CleanupStack::PushL(self);
	self->ConstructL();
	self->iCallBack = aCallBack;
	CleanupStack::Pop();
	return self;
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::SetServerL(const TDesC& aServer)
// Sets the server
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::SetServerL(const TDesC8& aServer)
	{
	_DBG_FILE("CNSmlDmModule::SetServer() : begin");
	iDDF->SetServerL(aServer);
	_DBG_FILE("CNSmlDmModule::SetServer() : end");
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::AddObjectL(const TDesC8& aURI, const TDesC8& aObject,
//		const TDesC8& aType, TInt aStatusRef )
// Add operation in DM protocoll
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::AddObjectL(const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	TInt aStatusRef,
	TBool aLargeItem )
	{
	_DBG_FILE("CNSmlDmModule::AddObjectL() : begin");

	TPtrC8 uri = NSmlDmURI::RemoveDotSlash(aURI);

	if(iInTransaction)
		{
		if(aLargeItem)
			{
			DoSetStatusL(aStatusRef,KNSmlDmStatusCommandFailed);
			return;
			}
		//command buffering
		iCommandBuffer->AddObjectL(uri,aObject,aType,aStatusRef);
		}
	else
		{
		TInt status = KNSmlDmNoStatus;
		CNSmlDmDDF::TAccess access = iDDF->CheckURIL(NSmlDmURI::RemoveProp(uri),EAclAdd);

		if(access == CNSmlDmDDF::EOk )
			{
			//check if property asked
			TInt offset = uri.Find(KNSmlDmProperty);
			if(offset==KErrNotFound)
				{
				//property is not asked, put the request to adapter
				iDDF->AddObjectL(uri,aObject,aType,aStatusRef,aLargeItem);
				}
			else
				{
				status = KNSmlDmStatusCommandNotAllowed;
				}
			}
		else if(access==CNSmlDmDDF::ENotAccess)
			{
			status = KNSmlDmStatusCommandNotAllowed;
			}
		else
			{
			status = KNSmlDmStatusNotFound;
			}
		if(status!=KNSmlDmNoStatus)
			{
			DoSetStatusL(aStatusRef,status);
			}
		}
	_DBG_FILE("CNSmlDmModule::AddObjectL() : end");
	}

// ----------------------------------------------------------------------------
// void CNSmlDmModule::UpdateObjectL(const TDesC8& aURI, const TDesC8& aObject,
// const TDesC8& aType, TInt aStatusRef )
// Replace operation in DM protocoll
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::UpdateObjectL(const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	TInt aStatusRef,
	TBool aLargeItem,
	// FOTA
	TInt aTotSizeOfLarge
	// FOTA end
	 )
	{
	_DBG_FILE("CNSmlDmModule::UpdateObjectL() : begin");

	TPtrC8 uri = NSmlDmURI::RemoveDotSlash(aURI);

	if(iInTransaction)
		{
		if(aLargeItem)
			{
			DoSetStatusL(aStatusRef,KNSmlDmStatusCommandFailed);
			return;
			}
		//command buffering
		iCommandBuffer->UpdateObjectL(uri,aObject,aType,aStatusRef);
		}
	else
		{
		TInt status = KNSmlDmNoStatus;
		TInt offset = uri.Find(KNSmlDmProperty);
		CNSmlDmDDF::TAccess access = iDDF->CheckURIL(
			NSmlDmURI::RemoveProp(uri),EAclReplace);

		if(access == CNSmlDmDDF::EOk)
			{
			//check if property asked
			if(offset==KErrNotFound)
				{
				//property is not asked, put the request to adapter
				// FOTA
				iDDF->UpdateObjectL(uri,aObject,aType,aStatusRef,aLargeItem,aTotSizeOfLarge);
				// FOTA end
				}
			else
				{
				UpdatePropertyL(uri,aObject,aType,offset+
					KNSmlDmProperty().Length(),aStatusRef);
				}
			}
		else if(access == CNSmlDmDDF::ENotAccess)
			{
			status = KNSmlDmStatusCommandNotAllowed;
			}
		else
			{
			status = KNSmlDmStatusNotFound;
			}

		if(status!=KNSmlDmNoStatus)
			{
			DoSetStatusL(aStatusRef,status);
			}
		} //end else(iInTransAction)
	_DBG_FILE("CNSmlDmModule::UpdateObjectL() : end");
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::FetchObjectL(const TDesC8& aURI, const TDesC8& aType,
// const TDesC8& aCmdRef, TInt aStatusRef,TBool aAclPass )
// Get operation in DM protocoll
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::FetchObjectL(const TDesC8& aURI,
	const TDesC8& aType,
	const TInt aResultsRef,
	TInt aStatusRef,
	TBool aAclPass)
	{
	_DBG_FILE("CNSmlDmModule::FetchObjectL() : begin");
	CBufBase *object=NULL;
	TPtrC8 uri = NSmlDmURI::RemoveDotSlash(aURI);

	if (uri.Compare(KNSmlDmUriDDF)==0)
		{
		object = CBufFlat::NewL(1024);
		CleanupStack::PushL(object);
		iDDF->GenerateDDFL(*object/*,iDbHandler*/);
		iCallBack->SetResultsL(aResultsRef,*object,aType,KNSmlDmFormatChr,
			object->Size());
		iCallBack->SetStatusL(aStatusRef,KNSmlDmStatusOK);
		CleanupStack::PopAndDestroy(); //object
		}
	else if(uri.Compare(KNSmlDmDDFDevInfoModDDF)==0)
		{
		object = CBufFlat::NewL(10);
		CleanupStack::PushL(object);
		TBuf8<10> ddfCRC;
		ddfCRC.Num(iDDFCrc);
		object->InsertL(0,ddfCRC);
		
		iCallBack->SetResultsL(aResultsRef,*object,aType,KNSmlDmFormatChr,
			object->Size());
		iCallBack->SetStatusL(aStatusRef,KNSmlDmStatusOK);
		CleanupStack::PopAndDestroy(); //object
		}
	else
		{
		if(iInTransaction)
			{
			//command buffering
			iCommandBuffer->FetchObjectL(uri,aType,aResultsRef,aStatusRef);
			}
		else
			{
			TInt status = KNSmlDmNoStatus;
			CNSmlDmDDF::TAccess access = iDDF->CheckURIL(
				NSmlDmURI::RemoveProp(uri),EAclGet);
	
			if(access == CNSmlDmDDF::EOk)
				{
				//check if property asked
				TInt offset = uri.Find(KNSmlDmProperty);
				if(offset==KErrNotFound)
					{
					if(uri.Find(KNSmlDmQuestionMark)>=0)
						{
						status = KNSmlDmStatusOptionalFeatureNotSupported;
						}
					else
						{
						//property is not asked, put the request to adapter
						iDDF->FetchObjectL(uri,aType,aResultsRef,aStatusRef,
							aAclPass);
						}
					}
				else
					{
					GetPropertyL(uri,aType,KNSmlDmProperty().Length()+
						offset,aResultsRef,aStatusRef);
					}
				}
			else if(access == CNSmlDmDDF::ENotAccess)
				{
				status = KNSmlDmStatusCommandNotAllowed;
				//check if property asked

				}
			else
				{
				status = KNSmlDmStatusNotFound;
				}
	
			if(status!=KNSmlDmNoStatus)
				{
				DoSetStatusL(aStatusRef,status);
				}
			} //end else iInTransAction
		} //end else DDF
	_DBG_FILE("CNSmlDmModule::FetchObjectL() : end");
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::DeleteObjectL( const TDesC8& aURI, TInt aStatusRef )
// Delete operation in DM protocoll
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::DeleteObjectL( const TDesC8& aURI,
TInt aStatusRef )
	{
	_DBG_FILE("CNSmlDmModule::DeleteObjectL() : begin");
	TPtrC8 uri = NSmlDmURI::RemoveDotSlash(aURI);

	if(iInTransaction)
		{
		//command buffering
		iCommandBuffer->DeleteObjectL(uri,aStatusRef);
		}
	else
		{
		TInt status = KNSmlDmNoStatus;
		CNSmlDmDDF::TAccess access = iDDF->CheckURIL(
			NSmlDmURI::RemoveProp(uri),EAclDelete);
		if(access==CNSmlDmDDF::EOk)
			{
			//check if property asked
			TInt offset = uri.Find(KNSmlDmProperty);
			if(offset==KErrNotFound)
				{
				//property is not asked, put the request to adapter
				iDDF->DeleteObjectL(uri,aStatusRef);
				}
			else
				{
				status = KNSmlDmStatusCommandNotAllowed;
				}
			}
		else if(access==CNSmlDmDDF::ENotAccess)
			{
			status = KNSmlDmStatusCommandNotAllowed;
			}
		else
			{
			status = KNSmlDmStatusNotFound;
			}

		if(status!=KNSmlDmNoStatus)
			{
			DoSetStatusL(aStatusRef,status);
			}
		}
	_DBG_FILE("CNSmlDmModule::DeleteObjectL() : end");
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::ExecuteObjectL( const TDesC8& aURI,
// const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef)
// Execute operation in DM protocoll
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::ExecuteObjectL( const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	TInt aStatusRef,
	// FOTA
	const TDesC8& aCorrelator,
	// FOTA end	
	TBool aLargeItem )
	{
	_DBG_FILE("CNSmlDmModule::ExecuteObjectL() : begin");

	TPtrC8 uri = NSmlDmURI::RemoveDotSlash(aURI);

	if(iInTransaction)
		{
		if(aLargeItem)
			{
			DoSetStatusL(aStatusRef,KNSmlDmStatusCommandFailed);
			return;
			}
		//command buffering
		// FOTA
		// It is not expected that the FOTA adapter will get atomic commands.
		// That's why correlator is not forwarded to the command buffer.
		// FOTA end		
		iCommandBuffer->ExecuteObjectL(uri,aObject,aType,aStatusRef);
		}
	else
		{
		TInt status = KNSmlDmNoStatus;
		TInt offset = uri.Find(KNSmlDmProperty);
		CNSmlDmDDF::TAccess access = iDDF->CheckURIL(
			NSmlDmURI::RemoveProp(uri),EAclExecute);

		if(access == CNSmlDmDDF::EOk)
			{
			//check if property asked
			if(offset==KErrNotFound)
				{
				//property is not asked, put the request to adapter
				// FOTA
				iDDF->ExecuteObjectL(uri,aObject,aType,aStatusRef,aCorrelator,aLargeItem);
				// FOTA end
				}
			else
				{
				status = KNSmlDmStatusCommandNotAllowed;
				}
			}
		else if(access == CNSmlDmDDF::ENotAccess)
			{
			//check if property asked
			status = KNSmlDmStatusCommandNotAllowed;
			}
		else
			{
			status = KNSmlDmStatusNotFound;
			}

		if(status!=KNSmlDmNoStatus)
			{
			DoSetStatusL(aStatusRef,status);
			}
		} //end else(iInTransAction)
	_DBG_FILE("CNSmlDmModule::ExecuteObjectL() : end");
	}

// ----------------------------------------------------------------------------
// void CNSmlDmModule::CopyObjectL( const TDesC8& aURI,
//	const TDesC8& aObject, const TDesC8& aType, TInt aStatusRef)
// Copy operation in DM protocoll.
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::CopyObjectL( const TDesC8& aTargetURI,
	const TDesC8& aSourceURI,
	const TDesC8& aType,
	TInt aStatusRef)
	{
	_DBG_FILE("CNSmlDmModule::CopyObjectL() : begin");

	TPtrC8 sourceUri = NSmlDmURI::RemoveDotSlash(aSourceURI);
	TPtrC8 targetUri = NSmlDmURI::RemoveDotSlash(aTargetURI);

	if(iInTransaction)
		{
		//command buffering
		iCommandBuffer->CopyObjectL(targetUri,sourceUri,aType,aStatusRef);
		}
	else
		{
		TInt status = KNSmlDmNoStatus;
		TInt offset = sourceUri.Find(KNSmlDmProperty);
		CNSmlDmDDF::TAccess access = iDDF->CheckURIL(
			NSmlDmURI::RemoveProp(sourceUri),EAclCopy);

		if(access == CNSmlDmDDF::EOk)
			{
			//check if property asked
			if(offset==KErrNotFound)
				{
				//property is not asked, put the request to adapter
				iDDF->CopyObjectL(targetUri,sourceUri,aType,aStatusRef);
				}
			else
				{
				status = KNSmlDmStatusCommandNotAllowed;
				}
			}
		else if(access == CNSmlDmDDF::ENotAccess)
			{
			//check if property asked
			status = KNSmlDmStatusCommandNotAllowed;
			}
		else
			{
			status = KNSmlDmStatusNotFound;
			}

		if(status!=KNSmlDmNoStatus)
			{
			DoSetStatusL(aStatusRef,status);
			}
		} //end else(iInTransAction)
	_DBG_FILE("CNSmlDmModule::CopyObjectL() : end");
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::StartTransactionL( )
// Start Atomic operation
// Tells that the following commands must be buffered
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::StartTransactionL( )
	{
	iInTransaction = ETrue;
	if(iCommandBuffer==0)
		{
		iCommandBuffer = CNSmlDmCommandBuffer::NewL(*this);
		}
	}

// ----------------------------------------------------------------------------
// void CNSmlDmModule::CommitTransactionL( )
// Commit Atomic operation
// The buffered commands can be executed
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::CommitTransactionL( )
	{
	iInTransaction = EFalse;
	if(iCommandBuffer)
		{
		iCommandBuffer->CommitL(*iDDF);
		delete iCommandBuffer;
		iCommandBuffer = 0;
		}
	}

// ----------------------------------------------------------------------------
// void CNSmlDmModule::RollBackL( )
// Rollback Atomic operation
// The buffered commands are removed from buffer and they are not executed
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::RollBackL( )
	{
	iInTransaction = EFalse;
	if(iCommandBuffer)
		{
		delete iCommandBuffer;
		iCommandBuffer = 0;
		}
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::EndMessageL( )
// Indicates message ending
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::EndMessageL( )
	{
	_DBG_FILE("CNSmlDmModule::EndMessageL() : begin");
	iDDF->EndMessageL();
	iPropResults.ResetAndDestroy();
	iInternalStatusRef = 0;
	_DBG_FILE("CNSmlDmModule::EndMessageL() : end");
	}

// ----------------------------------------------------------------------------
// TInt CNSmlDmModule::IsDDFChangedL( )
// Returns a checksum of ddf versions. Checksum changes if the ddf changes
// ----------------------------------------------------------------------------
EXPORT_C TInt CNSmlDmModule::IsDDFChangedL( )
	{
	return iDDFCrc;
	}

// ----------------------------------------------------------------------------
// void CNSmlDmModule::MoreDataL(CBufBase*& adata)
// Gets more data in case of largeobject
// ----------------------------------------------------------------------------
EXPORT_C void CNSmlDmModule::MoreDataL(CBufBase*& aData)
	{
	iDDF->MoreDataL(aData);
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::UpdatePropertyL(const TDesC8& aURI,
//	const TDesC8& aObject, const TDesC8& aType, TInt aOffset)
// Replace property operation in DM protocoll
// ----------------------------------------------------------------------------
void CNSmlDmModule::UpdatePropertyL(const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& /*aType*/,
	TInt aOffset,
	const TInt aStatusRef )
	{

	if(aURI.Mid(aOffset).Compare(KNSmlDmPropetyACL)==0)
		{
		iDDF->UpdateAclL(NSmlDmURI::RemoveProp(aURI),aObject,aStatusRef);
		}
	else
		{
		DoSetStatusL(aStatusRef,KNSmlDmStatusCommandNotAllowed);
		}
	}



// ----------------------------------------------------------------------------
// void CNSmlDmModule::GetPropertyL(const TDesC8& aURI, const TDesC8& aType,
//	const TDesC8& aCmdRef, TInt aOffset )
// Get property operation in DM protocoll
// ----------------------------------------------------------------------------
void CNSmlDmModule::GetPropertyL(const TDesC8& aURI,
	const TDesC8& aType,
	TInt aOffset,
	TInt aResultsRef,
	TInt aStatusRef)
	{
	TPtrC8 uri = NSmlDmURI::RemoveProp(aURI);
	if(iDDF->CheckAclL(uri,EAclGet))
		{
		CNSmlGetPropertyElement* propGet =
			 new (ELeave)CNSmlGetPropertyElement();
		CleanupStack::PushL(propGet);
		propGet->iResultsRef = aResultsRef;
		propGet->iStatusRef = aStatusRef;
		propGet->iStatusCode = KNSmlDmNoStatus;
		propGet->iUri = uri.AllocL();

		if(aURI.Mid(aOffset).Compare(KNSmlDmPropetyACL)==0)
			{
			propGet->iProp = EPropACL;
			iPropResults.AppendL(propGet);
			CleanupStack::Pop(); // propGet
			if(uri.Compare(KNSmlDmDDFDevInfoModDDF)==0)
				{
				DoSetStatusL(aStatusRef,KNSmlDmStatusOK);
				CBufBase* object = CBufFlat::NewL(128);
				CleanupStack::PushL(object);
				DoSetResultsL(aResultsRef,*object,aType,KNullDesC8,0,ETrue);
				CleanupStack::PopAndDestroy(); //object
				}
			else
				{
				iDDF->FetchObjectL(uri,aType,aResultsRef,aStatusRef);
				}
			}
		else if(aURI.Mid(aOffset).Compare(KNSmlDmPropetyFormat)==0)
			{
			propGet->iProp = EPropFormat;
			iPropResults.AppendL(propGet);
			CleanupStack::Pop(); // propGet
			iDDF->FetchObjectL(uri,aType,aResultsRef,aStatusRef);
			}
		else if(aURI.Mid(aOffset).Compare(KNSmlDmPropetyName)==0)
			{
			propGet->iProp = EPropName;
			iPropResults.AppendL(propGet);
			CleanupStack::Pop(); // propGet
			iDDF->FetchObjectL(uri,aType,aResultsRef,aStatusRef);
			}
		else if(aURI.Mid(aOffset).Compare(KNSmlDmPropetySize)==0)
			{
//			if(iDDF->IsLeafL(uri)==CNSmlDmDDF::ELeaf)
			if(iDDF->IsLeafL(uri)==ENSmlDmDDFLeaf)
				{
				propGet->iProp = EPropSize;
				iPropResults.AppendL(propGet);
				CleanupStack::Pop(); // propGet
				iDDF->FetchObjectSizeL(uri,aType,aResultsRef,aStatusRef);
				}
			else
				{
				delete propGet->iUri;
				propGet->iUri = 0;
				CleanupStack::PopAndDestroy(); // propGet
				DoSetStatusL(aStatusRef,KNSmlDmStatusOptionalFeatureNotSupported);
				}
			}
		else if(aURI.Mid(aOffset).Compare(KNSmlDmPropetyType)==0)
			{
			propGet->iProp = EPropType;
			iPropResults.AppendL(propGet);
			CleanupStack::Pop(); // propGet
			iDDF->FetchObjectL(uri,aType,aResultsRef,aStatusRef);
			}
		else
			{
			if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
			{
				if(aURI.Mid(aOffset).Compare(KNSmlDmPropertyCaseSense)==0) 
			    {
			    propGet->iProp = EPropCaseSense;
				iPropResults.AppendL(propGet);
				CleanupStack::Pop(); // propGet
				iDDF->FetchObjectL(uri,aType,aResultsRef,aStatusRef);
			    }
			    else
			    {
			    delete propGet->iUri;
				propGet->iUri =0;
				CleanupStack::PopAndDestroy(); // propGet
				DoSetStatusL(aStatusRef,KNSmlDmStatusOptionalFeatureNotSupported);
			    }
			    
			}
			else
			{
				delete propGet->iUri;
				propGet->iUri =0;
				CleanupStack::PopAndDestroy(); // propGet
				DoSetStatusL(aStatusRef,KNSmlDmStatusOptionalFeatureNotSupported);
			}
			}
		}
	else
		{
		DoSetStatusL(aStatusRef,KNSmlDmStatusPermissionDenied);
		}
	}


// ----------------------------------------------------------------------------
// TBool CNSmlDmModule::DeleteInTransactionL( )
// Deletes te items added inside the atomic, in case that atomic fails
// ----------------------------------------------------------------------------
void CNSmlDmModule::DeleteInTransactionL(const TDesC8& aURI,
	MNSmlDmModuleCallBack *aCallBack)
	{
	MNSmlDmModuleCallBack *tmpCallBack = iCallBack;
	TBool tmpInTransaction = iInTransaction;
	iInTransaction = EFalse;
	iCallBack = aCallBack;
	iInternalCommand = ETrue;
	DeleteObjectL(aURI, iInternalStatusRef--);
	iInternalCommand = EFalse;
	iCallBack = tmpCallBack;
	iInTransaction = tmpInTransaction;
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::DoSetStatusL( )
// The status is provided throug this function to caller
// ----------------------------------------------------------------------------
void CNSmlDmModule::DoSetStatusL(TInt aStatusRef, TInt aStatusCode,
	TBool aSkipCmdBuf)
	{
	if(!iInternalCommand&&aStatusRef==-1)
		{
		return;
		}
	TInt status = aStatusCode;
	
	//loop through if the command is for getting property
	for(TInt i=0;i<iPropResults.Count();i++)
		{
		if(aStatusRef==iPropResults[i]->iStatusRef)
			{
			if(status>KNSmlDmStatusLargestOK)
				{
				//the property is asked, but the fetch for command is failed for
				//some reason -> return status not allowed 
				status = KNSmlDmStatusCommandNotAllowed;
				if(iPropResults[i]->iStatusCode==0) //checks if status is allready handled
					{
					//status was not handled before
					iPropResults[i]->iStatusCode=status;
					}
				else
					{
					//status was handled before in DoSetResultsL()
					return;
					}
				}
			else
				{
				return;
				}
			break;
			}
		}
	if(iCommandBuffer!=0 && !aSkipCmdBuf &&
		iCommandBuffer->CheckStatusRef(aStatusRef))
		{
		//command has came inside atomic
		iCommandBuffer->SetStatus(aStatusRef,status);
		}
	else
		{
		iCallBack->SetStatusL(aStatusRef,status);
		}
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::DoSetResultsL( )
// Sets the result to caller by using callback interface
// ----------------------------------------------------------------------------
void CNSmlDmModule::DoSetResultsL(TInt aResultsRef, CBufBase& aObject,
	const TDesC8& aType,
	const TDesC8& aFormat,
	TInt aTotalSize,
	TBool aSkipCmdBuf)
	{
	if(!iInternalCommand&&aResultsRef==-1)
		{
		return;
		}

	TInt index = -1;
	//loop throug the prop result buffer, the buffe contains element for
	//each fetch to any property
	for(TInt i=0;i<iPropResults.Count();i++)
		{
		if(aResultsRef==iPropResults[i]->iResultsRef &&
			iPropResults[i]->iStatusCode<KNSmlDmStatusLargestOK)
			{
			//fetch to property found and status to fetch is
			//OK status -> it is OK to return the asked property
			index = i;
			//the HandlePropertyResultsL gets the property to aObject
			iPropResults[i]->iStatusCode = HandlePropertyResultsL(i,aObject);
			break;
			}
		}

	if(iCommandBuffer!=0 && !aSkipCmdBuf &&
		iCommandBuffer->CheckResultsRef(aResultsRef))
		{
		//command has come inside atomic
		if(index>=0)
			{
			//property is asked inside the atomic, the status and result are
			//given back to command buffer
			iCommandBuffer->SetStatus(iPropResults[index]->iStatusRef,
				iPropResults[index]->iStatusCode);
			}
		iCommandBuffer->SetResultsL(aResultsRef,aObject,aType,aFormat);
		}
	else
		{
		//command has not come niside the atomic
		if(index>=0)
			{
			//fetch for property found, the result and status are returned
			//from property result buffer
			iCallBack->SetStatusL(iPropResults[index]->iStatusRef,
				iPropResults[index]->iStatusCode);
			if(iPropResults[index]->iStatusCode<KNSmlDmStatusLargestOK)
				{
				iCallBack->SetResultsL(aResultsRef,aObject,aType,aFormat,
					aTotalSize);
				}
			}
		else
			{
			//gives the result to agent 
			//this is the normal case, i.e. fetch to item (not property)
			//and not inside the atomic
			//However, if atomic has come inside the atomic, the
			//command buffer finally callst this function with
			//aSkipCmdBuf==ERrue -> execution comes to this branch
			iCallBack->SetResultsL(aResultsRef,aObject,aType,aFormat,
				aTotalSize);
			}
		}
	}

// ----------------------------------------------------------------------------
// CNSmlDmModule::DisconnectFromOtherServers()
//	Closes the connections to callback server and host servers.
//	Separate function is needed, since the disconnecting cannot 
//	be made after the active scheduler of the thread is stopped.
// ----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDmModule::DisconnectFromOtherServers()
	{
	iDDF->DisconnectFromOtherServers();
	}

// FOTA
// ----------------------------------------------------------------------------
// CNSmlDmModule::MarkGenAlertsSentL()
//	When the generic alerts are successfully sent to the remote 
//	server, the FOTA adapter needs to be informed about this.
//	This command is chained through the DM engine.
// ----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDmModule::MarkGenAlertsSentL()
	{
	iDDF->MarkGenAlertsSentL();
	}
// FOTA end

// ----------------------------------------------------------------------------
// CNSmlDmModule::MarkGenAlertsSentL(const TDesC8& aURI)
//	When the generic alerts are successfully sent to the remote 
//	server, the repective adapter needs to be informed about this.
//	This command is chained through the DM engine.
//  This function is used if DM 1.2 version is enabled
// ----------------------------------------------------------------------------
//
EXPORT_C void CNSmlDmModule::MarkGenAlertsSentL(const TDesC8& aURI)
	{
	_DBG_FILE(" CNSmlDmModule::MarkGenAlertsSentL (TDesC8& aURI )calling iDDF->MarkgenAlert  ");
   	iDDF->MarkGenAlertsSentL(aURI);
	}


// ----------------------------------------------------------------------------
// void CNSmlDmModule::HandlePropertyResultsL(TInt aIndex,
// CBufBase& aObject)
// Sets the property result in case that property fetched
// ----------------------------------------------------------------------------
TInt CNSmlDmModule::HandlePropertyResultsL(TInt aIndex, CBufBase& aObject)
	{
	TInt status = KNSmlDmStatusOK;
	if(iPropResults[aIndex]->iProp==EPropACL)
		{
		aObject.Reset();
		iDDF->GetAclL(*iPropResults[aIndex]->iUri,aObject);
		}
	else if(iPropResults[aIndex]->iProp==EPropFormat)
		{
		aObject.Reset();
		HBufC8* format8 = iDDF->GetFormatAllocL(*iPropResults[aIndex]->iUri);
		CleanupStack::PushL(format8); //format8
		aObject.InsertL(0,*format8);
		CleanupStack::PopAndDestroy(); //format8
		}
	else if(iPropResults[aIndex]->iProp==EPropName)
		{
		aObject.Reset();
		aObject.InsertL(0,NSmlDmURI::LastURISeg(*iPropResults[aIndex]->iUri));
		}
	else if(iPropResults[aIndex]->iProp==EPropSize)
		{
		}
	else if(iPropResults[aIndex]->iProp==EPropType)
		{
		aObject.Reset();
		iDDF->GetMimeTypeL(*iPropResults[aIndex]->iUri,aObject);
		}
	
	else if(iPropResults[aIndex]->iProp==EPropCaseSense)
		{
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
		aObject.Reset();
		iDDF->GetCaseSenseL(*iPropResults[aIndex]->iUri,aObject);
		}
		else
		{
		status = KNSmlDmStatusOptionalFeatureNotSupported;
		}
		
		}
	else
		{
		status = KNSmlDmStatusOptionalFeatureNotSupported;
		}
	return status;
	}

// ----------------------------------------------------------------------------
// void CNSmlDmModule::ConstructL( )
// Second phase constructor
// ----------------------------------------------------------------------------
void CNSmlDmModule::ConstructL()
	{
	_DBG_FILE("CNSmlDmModule::ConstructL( ) : begin");
	iDDF = CNSmlDmDDF::NewL(*this);
	iDDFCrc = iDDF->IsDDFChangedL();
	FeatureManager::InitializeLibL();
	_DBG_FILE("CNSmlDmModule::ConstructL( ) : end");
	}


// ===========================================================================
// CNSmlGetPropertyElement
// ===========================================================================
// ----------------------------------------------------------------------------
// CNSmlGetPropertyElement::CNSmlGetPropertyElement( )
// ----------------------------------------------------------------------------
CNSmlGetPropertyElement::~CNSmlGetPropertyElement()
	{
	delete iUri;
	}

// End of file

