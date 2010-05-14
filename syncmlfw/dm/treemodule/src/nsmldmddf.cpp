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
* Description:  DM tree etc.
*
*/

#include <utf.h>
#include <smldmadapter.h>
#include <nsmldebug.h>
#include <nsmldmmodule.h>
//For P&S keys
#include <e32property.h>
#include "nsmldminternalpskeys.h"
#include "nsmldmddf.h"
#include "nsmldmtreeconstants.h"
#include "nsmldmhostclient.h"
#include "nsmldmclientandserver.h"
#include "nsmldmuri.h"
#include <featmgr.h>
// FOTA
#include "nsmldmimpluids.h"
// FOTA end

#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif

const TInt KGranularity = 4;
// FOTA
_LIT8 ( KNSmlDMFotaNode, "FUMO" );
_LIT8 ( KNSmlDMRoot, "." );
// FOTA end

// ===========================================================================
// CNSmlDmDDF
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmDDF::CNSmlDmDDF(CNSmlDmModule& aDmModule)
// ----------------------------------------------------------------------------
CNSmlDmDDF::CNSmlDmDDF(CNSmlDmModule& aDmModule) : iDmModule(aDmModule)
	{
	}

// ----------------------------------------------------------------------------
// CNSmlDmDDF::~CNSmlDmDDF()
// ----------------------------------------------------------------------------
CNSmlDmDDF::~CNSmlDmDDF()
	{
	delete iMgmtTree;

	iSessionArray.Reset();
	iSessionArray.Close();
	
	iResultRefs.ResetAndDestroy();
	iDeleteStatusRefs.ResetAndDestroy();
	iAddNodeStatusRefs.ResetAndDestroy();
	if(iAddRootNodesList)
		{
		delete iAddRootNodesList;
		}
	//P&S key deletion,even fine if the key is not there
	RProperty::Delete( KPSUidNSmlDMSyncAgent,KNSmlDMCmdAddNodeSuccess);
	FeatureManager::UnInitializeLib();
	}

// ----------------------------------------------------------------------------
//CNSmlDmDDF* CNSmlDmDDF::NewL(CNSmlDmDbHandler *aDbHandler)
// ----------------------------------------------------------------------------
CNSmlDmDDF* CNSmlDmDDF::NewL(CNSmlDmModule& aModule)
	{
	CNSmlDmDDF* self = new (ELeave) CNSmlDmDDF(aModule);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------------------------------
// void CNSmlDmDDF::GenerateDDFL(CBufBase& aObject)
// Generates generates xml from the ddf
// ----------------------------------------------------------------------------
void CNSmlDmDDF::GenerateDDFL(CBufBase& aObject)
	{
	_DBG_FILE("CNSmlDmDDF::GenerateDDFL() : begin");
	iMgmtTree->GenerateXMLL(aObject);
	_DBG_FILE("CNSmlDmDDF::GenerateDDFL() : end");
	}

// ----------------------------------------------------------------------------
// void CNSmlDmDDF::SetServerL(const TDesC8& aURI)
// Sets serverid to db
// ----------------------------------------------------------------------------
void CNSmlDmDDF::SetServerL(const TDesC8& aServerId)
	{
	_DBG_FILE("CNSmlDmDDF::SetServerL() : begin");
	iDbSession.SetServerL(aServerId);
	_DBG_FILE("CNSmlDmDDF::SetServerL() : end");
	}

// ----------------------------------------------------------------------------
// TBool CNSmlDmDDF::IsDDFChangedL(/*CNSmlDmDbHandler *aDbHandler,*/
// 	TDesC& aServer)
// Checks if ddf has been changed. Ask the ddf version from all the adapters
// and calculates checksum from adapter implementaion uids and versions
// ----------------------------------------------------------------------------
TInt CNSmlDmDDF::IsDDFChangedL()
	{
	_DBG_FILE("CNSmlDmDDF::IsDDFChangedL() : begin");
	User::LeaveIfError( iDbSession.Connect() );
	TInt checkSum(0);
	for(TInt i=0;i<4;i++)
		{
		switch(i)
			{
			case 0:
				if(iSessionArray[i].ConnectL(KNSmlDmHostServer1Name,this)
					==KErrNone)
					{
					iOkSessions=iOkSessions|(0x01<<i);
					checkSum=checkSum+iSessionArray[i].DDFStructureL(
						*iMgmtTree);
					iMgmtTree->SetServerId(i+1);
					}
				break;
			case 1:
				if(iSessionArray[i].ConnectL(KNSmlDmHostServer2Name,this)
					==KErrNone)
					{
					iOkSessions=iOkSessions|(0x01<<i);
					checkSum=checkSum+iSessionArray[i].DDFStructureL(
						*iMgmtTree);
					iMgmtTree->SetServerId(i+1);
					}
				break;
			case 2:
				if(iSessionArray[i].ConnectL(KNSmlDmHostServer3Name,this)
					==KErrNone)
					{
					iOkSessions=iOkSessions|(0x01<<i);
					checkSum=checkSum+iSessionArray[i].DDFStructureL(
						*iMgmtTree);
					iMgmtTree->SetServerId(i+1);
					}
				break;
			case 3:
				if(iSessionArray[i].ConnectL(KNSmlDmHostServer4Name,this)
					==KErrNone)
					{
					iOkSessions=iOkSessions|(0x01<<i);
					checkSum=checkSum+iSessionArray[i].DDFStructureL(
						*iMgmtTree);
					iMgmtTree->SetServerId(i+1);
					}
				break;
			default:
				User::Panic ( KNSmlTxtPanicDmModule, KErrArgument );
				break;
			}
		}
	return checkSum;
	}


// ----------------------------------------------------------------------------
// void CNSmlDmDDF::AddObjectL(const TDesC& aURI, const TDesC8& aObject,
// 	const TDesC& aType)
// Adds object, if adapter is found by using th uri, calls the adapters add object function.
// Adapters AddNodeObjectL is called if not leaf object
// ----------------------------------------------------------------------------
void CNSmlDmDDF::AddObjectL(const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	const TInt aStatusRef,
	TBool aLargeItem)
	{
	_DBG_FILE("CNSmlDmDDF::AddObjectL() : begin");
	TBool adapterCalled = EFalse;
	TInt status = KNSmlDmStatusNotFound;		
	if(iAdapterId==0||iSessionId==0)
		{
		//update not allowed in object group level (levels before adapter
		//level) 
		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusCommandNotAllowed);
		return;
		}
		
	if(iDbSession.CheckDynamicAclL(aURI,EAclAdd,iMgmtTree))	 //tarm
		{
		//acl rigths ok
		TNSmlDmDDFFormat format = IsLeafL(aURI);
		TPtrC8 parentUri = NSmlDmURI::ParentURI(aURI);
		if(format==ENSmlDmDDFLeaf)
			{
			HBufC8* luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
			TInt len = luid->Length();
			CleanupStack::PushL(luid);

			TInt find = KErrNotFound;
			if( !FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112) )//means DM 1.2
			 {
			 if(iAddRootNodesList)
			     {							
			     HBufC8* puri = HBufC8::NewLC(parentUri.Length()+2);
			     puri->Des().Append(parentUri);
			     puri->Des().Append(',');
			     find = iAddRootNodesList->Find(puri->Des());	
			     CleanupStack::PopAndDestroy();
			     }			
			 if( find <= KErrNotFound )
			     {
			     TBool returnstatus = ETrue; 
			     TPtrC8 Uri1 = NSmlDmURI::URISeg(aURI,1);
			     if( iAddRootNodesList )
			     {
			     TPtrC8 FindUri = NSmlDmURI::URISeg(aURI,2);
			     HBufC8* puri = HBufC8::NewLC(Uri1.Length()+FindUri.Length()+2);
			     puri->Des().Append(aURI.Left(Uri1.Length()+FindUri.Length()+1));
			     puri->Des().Append(',');
			     find = iAddRootNodesList->Find(puri->Des());   
			     CleanupStack::PopAndDestroy();
			     if(find >= 0)
			         {
			         returnstatus = EFalse; 
			         }
			     }
			     if(returnstatus)//not added in this session
			         {			    	 
			         _DBG_FILE("CNSmlDmDDF::AddObjectL()node not added in this session");
                     if(len > 0) //Luid exists,Already existed
                     {
                     iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusAlreadyExists);	 
                     }
                     else
                     {
                     iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusNotFound);
                     }
			         CleanupStack::PopAndDestroy(); //luid
			         return;
			         }
			     }
				}
			iSessionArray[iSessionId-1].UpdateLeafL(iAdapterId,*luid,
				aStatusRef,aURI,aObject,aType,aLargeItem);
			adapterCalled = ETrue;		
			CleanupStack::PopAndDestroy(); //luid
			}
		else
			{
			RProperty::Set(KPSUidNSmlDMSyncAgent, 
						          KNSmlDMCmdAddNodeSuccess, ENotAdded);
			HBufC8* luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
			CleanupStack::PushL(luid);			
			CNSmlCallbackElement *newElement 
				= new (ELeave)CNSmlCallbackElement();
			CleanupStack::PushL(newElement);
			newElement->iRef = aStatusRef;
			newElement->iUri = aURI.AllocL();
			iAddNodeStatusRefs.AppendL(newElement);
			CleanupStack::Pop(); //newElement
			iSessionArray[iSessionId-1].AddNodeL(iAdapterId,*luid,aStatusRef,
				aURI);
			//means DM 1.2
			if( !FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112) )
			  {
			  //Check the status of add node, so that update the add buffer
			  TInt NodeAddingSuccess = KErrNotFound;
			  TInt NodeorLeafexists= KErrNotFound;
			  TInt err = RProperty::Get(KPSUidNSmlDMSyncAgent, 
					  KNSmlDMCmdAddNodeSuccess, NodeAddingSuccess);
			  //This key will be set by DM Host Session's setstatus
			  TInt err1 = RProperty::Get( KPSUidNSmlDMSyncAgent, 
					  KNSmlDMCmdAddOnExistingNodeorLeafKey,
					  NodeorLeafexists );				  
			  if( ( err1 == KErrNone && NodeorLeafexists == EAddCmd ) && 
			          (err == KErrNone && (NodeAddingSuccess == EAdded ||
			                  NodeAddingSuccess == ENotAdded) ) )
			      {
			      UpdateAddNodeListL(aURI);
			      }
			  else if(err == KErrNone && NodeAddingSuccess != EFailed )
			      {
			      //Check luid, 
			      HBufC8* newluid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
			               
			      CleanupStack::PushL(newluid);
			      if(newluid->Length() > 0 && newluid->Compare(*luid)==0 ) 
			          {
					  _DBG_FILE("Already existed dont buffer");
			          }
			      else
			          {
			          UpdateAddNodeListL(aURI);
			          }
			      CleanupStack::PopAndDestroy(); //newluid
			      }
			  
			 
			  
			  }
			adapterCalled = ETrue;
		
			CleanupStack::PopAndDestroy(); //luid
			}
		} //end if(iDbSession.CheckAclL())
	else
		{
		//no acl rights
		status = KNSmlDmStatusPermissionDenied;
		}

	if(!adapterCalled)
		{
		iDmModule.DoSetStatusL(aStatusRef,status);
		}
	_DBG_FILE("CNSmlDmDDF::AddObjectL() : begin");
	}

// ----------------------------------------------------------------------------
// void CNSmlDmDDF::UpdateObjectL(const TDesC& aURI, const TDesC8& aObject,
//	const TDesC& aType)
// Calls the adapters UpdateObjectL function if the adapter is recognised by
// using URI 
// ----------------------------------------------------------------------------
void CNSmlDmDDF::UpdateObjectL(const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	const TInt aStatusRef,
	TBool aLargeItem,
	// FOTA
	TInt aTotSizeOfLarge
	// FOTA end
	)
	{
	_DBG_FILE("CNSmlDmDDF::UpdateObjectL() : begin");

	TBool adapterCalled = EFalse;
	TInt status = KNSmlDmStatusNotFound;
	
	
	if(iAdapterId==0||iSessionId==0)
		{
		//update not allowed in object group level
		//(levels before adapter level) 
		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusCommandNotAllowed);
		return;
		}
		
	if(iDbSession.CheckDynamicAclL(aURI,EAclReplace,iMgmtTree)) //tarm
		{
		//acl rigths ok
		TNSmlDmDDFFormat format = IsLeafL(aURI);
		
		if(format==ENSmlDmDDFLeaf)
			{
			HBufC8* luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
			CleanupStack::PushL(luid);

			// FOTA
			iSessionArray[iSessionId-1].UpdateLeafL(iAdapterId,*luid,
				aStatusRef,aURI,aObject,aType,aLargeItem,aTotSizeOfLarge);
			// FOTA end
			adapterCalled = ETrue;
		
			CleanupStack::PopAndDestroy(); //luid
			}
		else
			{
			status=KNSmlDmStatusCommandNotAllowed;
			}
			
			
		} //end if(iDbSession.CheckAclL())
	else
		{
		//no acl rights
		status = KNSmlDmStatusPermissionDenied;
		}

	if(!adapterCalled)
		{
		iDmModule.DoSetStatusL(aStatusRef,status);
		}

	_DBG_FILE("CNSmlDmDDF::UpdateObjectL() : end");
	}

// ----------------------------------------------------------------------------
// void CNSmlDmDDF::FetchObjectL(const TDesC& aURI, const TDesC& aType,
//	CBufBase& aObject )
// Calls the adapters FetchObjectL function via IPC if the adapter and 
// correct host server are recognised by using URI.
// Adapters ChildURIListL is called, if not leaf object
// ----------------------------------------------------------------------------
void CNSmlDmDDF::FetchObjectL(const TDesC8& aURI,
	const TDesC8& aType,
	const TInt aResultsRef,
	const TInt aStatusRef,
	TBool aAclPass )
	{
	_DBG_FILE("CNSmlDmDDF::FetchObjectL() : begin");
	TBool adapterCalled = EFalse;
	TInt status = KNSmlDmStatusNotFound;

	if(aAclPass||iDbSession.CheckDynamicAclL(aURI,EAclGet,iMgmtTree)) //tarm
		{
		//acl rights ok or internal query when aclchecking is passed
		HBufC8* luid = NULL;

		if(iAdapterId==0||iSessionId==0)
			{
			//this will response to gets before adapter level
			TInt status = KNSmlDmStatusOK;
			CBufBase* object = CBufFlat::NewL(128);
			CleanupStack::PushL(object);
			CNSmlDmNodeList* final = NULL;
			if(aURI.Length()==0||aURI.Compare(KNSmlDmRootUri)==0)
				{
				//fetch for the root level
				CNSmlDmNodeList* nodeListPtr = iMgmtTree->iNodeListPtr;
				//loop through the fisrt level after root
				while(nodeListPtr!=0)
					{
					object->InsertL(object->Size(),
						nodeListPtr->iNode->iNodeName.iContent,
						nodeListPtr->iNode->iNodeName.iLength);
						
					if(nodeListPtr->iNext!=0)
						{
						object->InsertL(object->Size(),KNSmlDmLitSeparator8);
						}
					nodeListPtr = nodeListPtr->iNext;
					}
				}
			else
				{
				iMgmtTree->FindNameFromNodeListL(aURI,final);

				if(!final)
					{
					//the node was not found from ddf
					status = KNSmlDmStatusNotFound;
					}
				else
					{
					if(final->iNode)
						{
						CNSmlDmNodeList* nodeListPtr =
							final->iNode->iNodeListPtr;
					
						//loop the childs of "final" node
						while(nodeListPtr)
							{
							object->InsertL(object->Size(),
								nodeListPtr->iNode->iNodeName.iContent,
								nodeListPtr->iNode->iNodeName.iLength);
								
							if(nodeListPtr->iNext)
								{
								object->InsertL(object->Size(),
									KNSmlDmLitSeparator8);
								}
							nodeListPtr = nodeListPtr->iNext;
							}
						}
					else
						{
						object->InsertL(0,KNullDesC8);
						}
					}
				}
			iDmModule.DoSetStatusL(aStatusRef,status);
			if(status == KNSmlDmStatusOK)
				{
				CNSmlCallbackElement *newElement =
					new (ELeave)CNSmlCallbackElement();
					
				CleanupStack::PushL(newElement);
				newElement->iRef = aResultsRef;
				newElement->iUri = aURI.AllocL();
				newElement->iNode = ETrue;
				iResultRefs.AppendL(newElement);
				CleanupStack::Pop(); //newElement
				SetResultsL(aResultsRef,*object,aType,object->Size());
				}
			CleanupStack::PopAndDestroy(); //object
			return;
			} //end if(iAdapterId==0||iSessionId==0)

		luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
		CleanupStack::PushL(luid);

		CNSmlCallbackElement *newElement = new (ELeave)CNSmlCallbackElement();
		CleanupStack::PushL(newElement);
		newElement->iRef = aResultsRef;
		newElement->iUri = 0;

		//mapping info found for the uri
		TNSmlDmDDFFormat format = IsLeafL(aURI);
		if(format==ENSmlDmDDFLeaf)
			{
			newElement->iUri = aURI.AllocL();
			newElement->iNode = EFalse;
			iResultRefs.AppendL(newElement);
			CleanupStack::Pop(); //newElement
			
			iSessionArray[iSessionId-1].FetchLeafObjectL(iAdapterId,
				aURI,*luid,aType,aResultsRef,aStatusRef);

			adapterCalled = ETrue;
			}
		else if(format==ENSmlDmDDFNode)
			{
			//interior node, ask the childurilist from the adapter
			CArrayFixFlat<TSmlDmMappingInfo> *previousList
				=new (ELeave) CArrayFixFlat <TSmlDmMappingInfo> (KGranularity);
				
			CleanupStack::PushL(previousList);
			iDbSession.GetUriSegListL(iAdapterId,aURI,*previousList);
			newElement->iUri = aURI.AllocL();
			newElement->iNode = ETrue;
			iResultRefs.AppendL(newElement);
			CleanupStack::Pop(2); //previousList,newElement
			CleanupStack::PushL(previousList);
			iSessionArray[iSessionId-1].ChildURIListL(iAdapterId,aURI,*luid,
				*previousList,aResultsRef,aStatusRef);
				
			adapterCalled = ETrue;
			CleanupStack::PopAndDestroy(); //previousList
			} //end else IsLeaf
		CleanupStack::PopAndDestroy(); //luid
		} //end if(iDbSession.CheckAclL())
	else
		{
		status = KNSmlDmStatusPermissionDenied;
		}

	if(!adapterCalled)
		{
		iDmModule.DoSetStatusL(aStatusRef,status);
		}

	_DBG_FILE("CNSmlDmDDF::FetchObjectL() : end");
	}


// ----------------------------------------------------------------------------
// void CNSmlDmDDF::FetchObjectSizeL(const TDesC8& aURI,
//	const TDesC8& aType,
//	const TInt aResultsRef,
//	const TInt aStatusRef)
// Calls the adapters FetchObjectSizeL function via IPC if the adapter and 
// correct host server are recognised by using URI.
// ----------------------------------------------------------------------------
void CNSmlDmDDF::FetchObjectSizeL(const TDesC8& aURI,
	const TDesC8& aType,
	const TInt aResultsRef,
	const TInt aStatusRef)
	{
	_DBG_FILE("CNSmlDmDDF::FetchObjectSizeL() : begin");

	if(iAdapterId&&iSessionId)
		{
		HBufC8* luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
		CleanupStack::PushL(luid);

		CNSmlCallbackElement *newElement = new (ELeave)CNSmlCallbackElement();
		CleanupStack::PushL(newElement);
		newElement->iRef = aResultsRef;
		newElement->iUri = aURI.AllocL();
		newElement->iNode = EFalse;
		iResultRefs.AppendL(newElement);
		CleanupStack::Pop(); //newElement
		iSessionArray[iSessionId-1].FetchLeafObjectSizeL(iAdapterId,aURI,*luid,aType,aResultsRef,aStatusRef);
		CleanupStack::PopAndDestroy(); //luid
		}
	else
		{
		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusNotFound);
		}


	_DBG_FILE("CNSmlDmDDF::FetchObjectSizeL() : end");
	}


// ----------------------------------------------------------------------------
// void CNSmlDmDDF::ExecuteObjectL(const TDesC& aURI, const TDesC8& aObject,
// const TDesC& aType)
// Calls the adapters ExecuteObjectL function if the adapter is recognised
// by using URI 
// ----------------------------------------------------------------------------
void CNSmlDmDDF::ExecuteObjectL(const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	const TInt aStatusRef,
	// FOTA
	const TDesC8& aCorrelator,
	// FOTA end	
	TBool aLargeItem)
	{
	_DBG_FILE("CNSmlDmDDF::ExecuteObjectL() : begin");

	TBool adapterCalled = EFalse;
	TInt status = KNSmlDmStatusNotFound;
	
	if(iAdapterId==0||iSessionId==0)
		{
		//update not allowed in object group level
		//(levels before adapter level) 
		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusCommandNotAllowed);
		return;
		}
		
	if(iDbSession.CheckDynamicAclL(aURI,EAclExecute,iMgmtTree)) //tarm
		{
		//acl rigths ok
		HBufC8* luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
		CleanupStack::PushL(luid);
		// FOTA 
		// For the FOTA adapter put the correlator to aObject instead of
		// data.
		if ( iAdapterId == KNSmlDMFotaAdapterImplUid || iAdapterId == KNSmlDMAMAdapterImplUid)
			{
			iSessionArray[iSessionId-1].ExecuteCommandL(iAdapterId,*luid,
				aStatusRef,aURI,aCorrelator,aType,aLargeItem);
			}
		else
			{
			iSessionArray[iSessionId-1].ExecuteCommandL(iAdapterId,*luid,
				aStatusRef,aURI,aObject,aType,aLargeItem);		
			}
		// FOTA end			
		adapterCalled = ETrue;
		
		CleanupStack::PopAndDestroy(); //luid
		} //end if(iDbSession.CheckAclL())
	else
		{
		//no acl rights
		status = KNSmlDmStatusPermissionDenied;
		}

	if(!adapterCalled)
		{
		iDmModule.DoSetStatusL(aStatusRef,status);
		}

	_DBG_FILE("CNSmlDmDDF::ExecuteObjectL() : end");

	}

// ----------------------------------------------------------------------------
// void CNSmlDmDDF::CopyObjectL(const TDesC& aURI, const TDesC8& aObject,
//	const TDesC& aType)
// Calls the adapters CopyObjectL function if the adapter is recognised
// by using URI 
// ----------------------------------------------------------------------------
void CNSmlDmDDF::CopyObjectL(const TDesC8& aTargetURI,
	const TDesC8& aSourceURI,
	const TDesC8& aType,
	const TInt aStatusRef)
	{
	_DBG_FILE("CNSmlDmDDF::CopyObjectL() : begin");

	TBool adapterCalled = EFalse;
	TInt status = KNSmlDmStatusNotFound;
	
	
//	if(format==ENode)
	if(iAdapterId==0||iSessionId==0)
		{
		//update not allowed in object group level
		//(levels before adapter level) 
		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusCommandNotAllowed);
		return;
		}
		
	if(iDbSession.CheckDynamicAclL(aSourceURI,EAclGet,iMgmtTree)&& //tarm
		iDbSession.CheckDynamicAclL(aTargetURI,EAclAdd,iMgmtTree))
		{
		//acl rigths ok
		HBufC8* targetLuid = iDbSession.GetLuidAllocL(iAdapterId,aTargetURI);
		CleanupStack::PushL(targetLuid);
		HBufC8* sourceLuid = iDbSession.GetLuidAllocL(iAdapterId,aSourceURI);
		CleanupStack::PushL(sourceLuid);

		iSessionArray[iSessionId-1].CopyCommandL(iAdapterId,
			*targetLuid,aTargetURI,*sourceLuid,aSourceURI,aStatusRef,aType);

		adapterCalled = ETrue;
		
		CleanupStack::PopAndDestroy(2); //targetLuid,sourceLuid
		} //end if(iDbSession.CheckAclL())
	else
		{
		//no acl rights
		status = KNSmlDmStatusPermissionDenied;
		}

	if(!adapterCalled)
		{
		iDmModule.DoSetStatusL(aStatusRef,status);
		}

	_DBG_FILE("CNSmlDmDDF::CopyObjectL() : end");
	}




// ----------------------------------------------------------------------------
// void CNSmlDmDDF::DeleteObjectL(const TDesC& aURI)
// Calls the adapters DeleteObjectL function if the adapter is
// recognised by using URI.
// ----------------------------------------------------------------------------
void CNSmlDmDDF::DeleteObjectL(const TDesC8& aURI,const TInt aStatusRef)
	{
	_DBG_FILE("CNSmlDmDDF::DeleteObjectL(): begin");
	TInt status = KNSmlDmStatusNotFound;
	TBool adapterCalled = EFalse;

	if(iAdapterId==0||iSessionId==0)
		{
		//delete not allowed in object group level
		//(levels before adapter level) 
		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusCommandNotAllowed);
		return;
		}

	if(iDbSession.CheckDynamicAclL(aURI,EAclDelete,iMgmtTree)) //tarm
		{
		//acl rights ok
		HBufC8* luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
		CleanupStack::PushL(luid);

		if(luid->Length())
			{
			CNSmlCallbackElement *newElement 
				= new (ELeave)CNSmlCallbackElement();
			CleanupStack::PushL(newElement);
			newElement->iRef = aStatusRef;
			newElement->iUri = aURI.AllocL();
			iDeleteStatusRefs.AppendL(newElement);
			CleanupStack::Pop(); //newElement
			
			iSessionArray[iSessionId-1].DeleteObjectL(iAdapterId,*luid,
				aStatusRef,aURI);
				
			adapterCalled = ETrue;
			}
		else
			{
			iSessionArray[iSessionId-1].DeleteObjectL(iAdapterId,*luid,
				aStatusRef,aURI);
				
			adapterCalled = ETrue;
			}	
		CleanupStack::PopAndDestroy(); //luid
		} //end if(iDbSession.CheckAclL())
	else
		{
		status = KNSmlDmStatusPermissionDenied;
		}

	if(!adapterCalled)
		{
		iDmModule.DoSetStatusL(aStatusRef,status);
		}

	_DBG_FILE("CNSmlDmDDF::DeleteObjectL() : end");
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::CheckURIL(const TDesC& aURI)
// Compares URI to ddf and check that URI is valid
// ----------------------------------------------------------------------------
CNSmlDmDDF::TAccess CNSmlDmDDF::CheckURIL(const TDesC8& aURI,
	TNSmlDmCmdType aCmd)
	{
	_DBG_FILE("CNSmlDmDDF::CheckURIL() : begin");
	TAccess ret(ENotExist);
	TUint8 access(0);
	TUriCheck found = CheckAdapterL(aURI,access);
	if(aURI.Length() > 0 && found!=EUriNotFound)
		{
		//uri OK, now check the access which is set in ddf
		ret = ENotAccess;
		switch(aCmd)
			{
			case EAclAdd:
			if(access&TSmlDmAccessTypes::EAccessType_Add)
				{
				ret = EOk;
				}
			break;
		
			case EAclReplace:
			if(access&TSmlDmAccessTypes::EAccessType_Replace)
				{
				ret = EOk;
				}
			break;
		
			case EAclGet:
			if(access&TSmlDmAccessTypes::EAccessType_Get)
				{
				ret = EOk;
				}
			break;
	
			case EAclDelete:
			if(access&TSmlDmAccessTypes::EAccessType_Delete)
				{
				ret = EOk;
				}
			break;

			case EAclExecute:
			if(access&TSmlDmAccessTypes::EAccessType_Exec)
				{
				ret = EOk;
				}
			break;

			case EAclCopy:
			if(access&TSmlDmAccessTypes::EAccessType_Copy)
				{
				ret = EOk;
				}
			break;
			default:
			User::Panic ( KNSmlTxtPanicDmModule, KErrArgument );
			break;
			}
		}

	_DBG_FILE("CNSmlDmDDF::CheckURIL() : end");
	return ret;
	}




// ----------------------------------------------------------------------------
// void CNSmlDmDDF::ConstructL()
// Second phase constructor
// ----------------------------------------------------------------------------
void CNSmlDmDDF::ConstructL()
	{
	_DBG_FILE("CNSmlDmDDF::ConstructL() : begin");
	iMgmtTree = CNSmlDmMgmtTree::NewL();
	iResultSessionId=KErrNotFound;
	for(TInt i=0;i<4;i++)
		{
		RNSmlDMDataSession session;
		iSessionArray.AppendL(session);
		}
    FeatureManager::InitializeLibL(); 		
    RProperty::Define(KPSUidNSmlDMSyncAgent,
    		KNSmlDMCmdAddNodeSuccess, RProperty::EInt,
			KReadPolicy, KWritePolicy );
	_DBG_FILE("CNSmlDmDDF::ConstructL() : end");
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::CheckAclL( const TDesC& aURI,
// CNSmlDmDbHandler::TCmdType aCmdType)
// Does the ACL checking
// ----------------------------------------------------------------------------
TBool CNSmlDmDDF::CheckAclL( const TDesC8& aURI, TNSmlDmCmdType aCmdType)
	{
	TUint8 access(0);
	if(aURI.Length()==0||aURI.Compare(KNSmlDmRootUri)==0
		||CheckAdapterL(aURI,access)!=EUriNotFound)
		{
		return iDbSession.CheckDynamicAclL(aURI,aCmdType,iMgmtTree); //tarm
		}
	return EFalse;
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::UpdateAclL( const TDesC& aURI,const TDesC8 &aACL)
// Updates ACL to tree db
// ----------------------------------------------------------------------------
void CNSmlDmDDF::UpdateAclL( const TDesC8& aURI,
	const TDesC8 &aACL, const TInt aStatusRef)
	{
	if(aURI.Length()==0||aURI.Compare(KNSmlDmRootUri)==0)
	    {
		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusForbidden);
	    }
	else
	    {
    	TNSmlDmDDFFormat format = IsLeafL(aURI);
    	TBool access(0);
    	TPtrC8 parentUri = NSmlDmURI::ParentURI(aURI);
    	if(format==ENSmlDmDDFLeaf)
    		{
    		access = CheckAclL(parentUri,EAclReplace);
    		}
    	else
    		{
    		access = CheckAclL(parentUri,EAclReplace)
    			||CheckAclL(aURI,EAclReplace);
    		}

    	if(access)
    		{
    		TInt ret = iDbSession.UpdateAclL(aURI,aACL);
    		if( ret == KErrNone)
    			{
    			iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusOK);
    			}
    		else if( ret == KErrAccessDenied)
    			{
    			iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusForbidden);
    			}
    		else
    			{
    			iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusCommandFailed);
    			}
    		}
    	else
    		{
    		iDmModule.DoSetStatusL(aStatusRef,KNSmlDmStatusPermissionDenied);
    		}
	    }
	}

// ----------------------------------------------------------------------------
// CNSmlDmDDF::GetAclL( const TDesC& aURI, CBufBase &aACL)
// ----------------------------------------------------------------------------
TInt CNSmlDmDDF::GetAclL( const TDesC8& aURI, CBufBase &aACL)
	{
	TInt ret = KNSmlDmStatusPermissionDenied;
	if(CheckAclL(aURI,EAclGet))
		{
		iDbSession.GetAclL(aURI,aACL,EFalse);
		ret = KNSmlDmStatusOK;
		}
	return ret;
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::GetLuidAllocL(const TDesC& aURI)
// gets and allocates the mapped luid from tree db
// ----------------------------------------------------------------------------
HBufC8* CNSmlDmDDF::GetLuidAllocL(const TDesC8& aURI)
	{
	HBufC8* luid=0;
	if(CheckURIL(aURI,EAclGet)!=CNSmlDmDDF::ENotExist)
		{
		if(iAdapterId)
			{
			luid = iDbSession.GetLuidAllocL(iAdapterId,aURI);
			}
		}
	if(!luid)
		{
		luid = KNullDesC8().AllocL();
		}
	return luid;
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::StartAtomicL()
// Start atomic indication
// ----------------------------------------------------------------------------
void CNSmlDmDDF::StartAtomicL()
	{
	for(TInt i=0;i<4;i++)
		{
		if(iOkSessions&(0x01<<i))
			{
			iSessionArray[i].StartAtomicL();
			}
		}
	iDbSession.StartAtomic();
	}
	
// ----------------------------------------------------------------------------
// CNSmlDmDDF::CommitAtomicL()
// Commit atomic indication
// ----------------------------------------------------------------------------
void CNSmlDmDDF::CommitAtomicL()
	{
	for(TInt i=0;i<4;i++)
		{
		if(iOkSessions&(0x01<<i))
			{
			iSessionArray[i].CommitAtomicL();
			}
		}
	iDbSession.CommitAtomic();
	}
	
// ----------------------------------------------------------------------------
// CNSmlDmDDF::RollbackAtomicL()
// Rollback atomic indication
// ----------------------------------------------------------------------------
void CNSmlDmDDF::RollbackAtomicL()
	{
	for(TInt i=0;i<4;i++)
		{
		if(iOkSessions&(0x01<<i))
			{
			iSessionArray[i].RollbackAtomicL();
			}
		}
	iDbSession.RollbackAtomic();
	}

// ----------------------------------------------------------------------------
// CNSmlDmDDF::DisconnectFromOtherServers()
//	Closes the connections to callback server and host servers.
//	Separate function is needed, since the disconnecting cannot 
//	be made after the active scheduler of the thread is stopped.
// ----------------------------------------------------------------------------
//
void CNSmlDmDDF::DisconnectFromOtherServers()
	{
	iDbSession.Close();
	for ( TInt i(0); i<iSessionArray.Count(); i++ )
		{
		if(iOkSessions&(0x01<<i))
			{
			iSessionArray[i].Close();
			}
		}
	}
// FOTA
// ----------------------------------------------------------------------------
// CNSmlDmDDF::MarkGenAlertsSentL()
//	When the generic alerts are successfully sent to the remote 
//	server, the FOTA adapter needs to be informed about this.
//	This command is chained through the DM engine.
// ----------------------------------------------------------------------------
//
void CNSmlDmDDF::MarkGenAlertsSentL()
	{
	CNSmlDmNodeList* final = NULL;
	iMgmtTree->FindNameFromNodeListL ( KNSmlDMFotaNode, final );
	if ( final )
		{
		TUint32 adapterId(0);
		TUint8 sessionId(0);
		final->GetImplUid ( adapterId, sessionId );
		iSessionArray[sessionId-1].MarkGenAlertsSent();
		}
	}
// FOTA end


// ----------------------------------------------------------------------------
// CNSmlDmDDF::MarkGenAlertsSentL(const TDesC8& aURI)
//	When the generic alerts are successfully sent to the remote 
//	server, the FOTA adapter needs to be informed about this.
//	This command is chained through the DM engine.
//  For 1.2 Any adapter can send the Generic adapter,
//  Hence URI of adapter is passed 
// ----------------------------------------------------------------------------
//   
 
void CNSmlDmDDF::MarkGenAlertsSentL(const TDesC8& aURI)

    {
    _DBG_FILE("CNSmlDmDDF::MarkGenAlertsSentL (TDesC8& aURI) : begin");
    CNSmlDmNodeList* final = NULL;
    //Get the first root node from the aURI
     HBufC8* nodeURI = HBufC8::NewLC(aURI.Length());
    TInt i= 0;
    TBool dotPresentInURI = EFalse;
    for( i=0; i < aURI.Length(); i++ )
        {
        if((aURI[i] == '.') && (i==0))
        {
        	dotPresentInURI = ETrue;
        	continue;
        }
        
        if((aURI[i] == '/') && (i==1))
        continue;
        if(aURI[i]=='/')
            {
            break;
            }
        }

    if(i>0)
        {
        if(dotPresentInURI)
        {
        	 nodeURI->Des().Format(aURI.Mid(2,i-2)); // removing ./
        }
        else
        {
        	 nodeURI->Des().Format(aURI.Left(i));
        }
        }
       iMgmtTree->FindNameFromNodeListL (nodeURI->Des(),final);
       
       if(final)
        {
        _DBG_FILE("CNSmlDmDDF::MarkGenAlertsSentL (TDesC8& aURI) Adapter found  ");  
         
        TUint32 adapterId(0);
		TUint8 sessionId(0);
		final->GetImplUid ( adapterId, sessionId );
		if(sessionId == 0)
		{
		  sessionId = 1;
		}
		 
	    iSessionArray[sessionId-1].MarkGenAlertsSent(nodeURI->Des());
        }
       else
        {
         _DBG_FILE("CNSmlDmDDF::MarkGenAlertsSentL (TDesC8& aURI) Adapter NOT found  ");  
        }
        CleanupStack::PopAndDestroy(); //nodeURI
        _DBG_FILE("CNSmlDmDDF::MarkGenAlertsSentL (TDesC8& aURI) : END");
    }
// ----------------------------------------------------------------------------
// CNSmlDmDDF::CheckAdapterL(const TDesC& aURI)
// Finds the correct adapter from correct host server by using DDF.
// EUriNotFound returned if uri not according to DDF
// EBeforeAdapter returned if URI ok, but points to node bfore
// adapterlevel
// EPassAdapter returned if command should pass to adapeter
// In pass adapter case, iAdapterId is set to correct adapters uid and
// iSessionId is set to point to session to correct host server.
// ----------------------------------------------------------------------------
CNSmlDmDDF::TUriCheck CNSmlDmDDF::CheckAdapterL(const TDesC8& aURI,
	TUint8& aAccess)
	{
	_DBG_FILE("CNSmlDmDDF::CheckAdapterL() : begin");

	aAccess = 0;

	//check if root level
	if(aURI.Length()==0||aURI.Compare(KNSmlDmRootUri)==0)
		{
		iAdapterId = 0;
		iSessionId = 0;
		aAccess |= TSmlDmAccessTypes::EAccessType_Get;
		aAccess |= TSmlDmAccessTypes::EAccessType_Replace;
		return EBeforeAdapter;
		}

	TUriCheck found = EUriNotFound;


	CNSmlDmNodeList* final=0;
	iMgmtTree->FindNameFromNodeListL(aURI,final);

	if(!final)
		{
		iAdapterId = 0;
		iSessionId = 0;
		return EUriNotFound;
		}
	else
		{
		final->GetImplUid(iAdapterId,iSessionId);
		aAccess |= final->iNode->iDFProperties.iAccessTypes;
		if(iAdapterId==0||iSessionId==0)
			{
			//before adapter level, do not pass adapter
			found = EBeforeAdapter;
			iAdapterId=0;
			iSessionId=0;
			}
		else
			{
			//adapter level, adapter is loaded and EPassAdapter returned
			found = EPassAdapter;
			}
		}

	_DBG_FILE("CNSmlDmDDF::CheckAdapterL() : end");
	return found;
	}


// ----------------------------------------------------------------------------
// TFormat CNSmlDmDDF::IsLeafL(const TDesC& aURI)
// Function checks if the object pointed by aURI is leaf object of
// the adapter returns ELeaf, ENode or ENotFound
// ----------------------------------------------------------------------------
TNSmlDmDDFFormat CNSmlDmDDF::IsLeafL(const TDesC8& aURI)
	{
	_DBG_FILE("CNSmlDmDDF::IsLeafL() : begin");

	_DBG_FILE("CNSmlDmDDF::IsLeafL : before FindNameFromNodelist()");
	CNSmlDmNodeList* final;
	return iMgmtTree->FindNameFromNodeListL(aURI,final);
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::GetFormatAllocL()
// Get format from ddf
// ----------------------------------------------------------------------------
HBufC8* CNSmlDmDDF::GetFormatAllocL(const TDesC8& aURI)
	{
	_DBG_FILE("CNSmlDmDDF::GetFormatAllocL() : begin");

	CNSmlDmNodeList* final=0;
	iMgmtTree->FindNameFromNodeListL(aURI,final);

	HBufC8* format = 0;
	if(!final)
		{
		format = HBufC8::NewL(KNSmlDmFormatNode().Length());
		format->Des().Format(KNSmlDmFormatNode);
		return format;
		}
	else
		{
		switch(final->iNode->iDFProperties.iDFFormat)
			{
			case MSmlDmDDFObject::EChr:
				format = KNSmlDmFormatChr().AllocL();
				break;

			case MSmlDmDDFObject::EB64:
				format = KNSmlDmFormatB64().AllocL();
				break;

			case MSmlDmDDFObject::EBool:
				format = KNSmlDmFormatBool().AllocL();
				break;

			case MSmlDmDDFObject::EInt:
				format = KNSmlDmFormatInt().AllocL();
				break;

			case MSmlDmDDFObject::ENode:
				format = KNSmlDmFormatNode().AllocL();
				break;

			case MSmlDmDDFObject::ENull:
				format = KNSmlDmFormatNull().AllocL();
				break;

			case MSmlDmDDFObject::EXml:
				format = KNSmlDmFormatXml().AllocL();
				break;

			case MSmlDmDDFObject::EBin:
				format = KNSmlDmFormatBin().AllocL();
				break;
            
    		case MSmlDmDDFObject::EDate:
    			{
    			if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
    			format = KNSmlDDFFormatDate().AllocL();
    			else
				format = KNSmlDmFormatChr().AllocL();
    		    break;
    			}
    		case MSmlDmDDFObject::ETime:
    			{
    			if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
    		    format = KNSmlDDFFormatTime().AllocL();
    			else
    			format = KNSmlDmFormatChr().AllocL();
    		    break;
    			}
    		case MSmlDmDDFObject::EFloat:
    			{
    			if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
    		    format = KNSmlDDFFormatFloat().AllocL();
    			else
    			format = KNSmlDmFormatChr().AllocL();
    			break;
    			}
	
			default:
				format = KNSmlDmFormatChr().AllocL();
				break;
			}
		}
	return format;
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::GetMimeTypeL()
// Get mimetype from ddf
// ----------------------------------------------------------------------------
TInt CNSmlDmDDF::GetMimeTypeL(const TDesC8& aURI,CBufBase& aObject)
	{
	_DBG_FILE("CNSmlDmDDF::GetMimeTypeL() : begin");

	CNSmlDmNodeList* final=0;
	iMgmtTree->FindNameFromNodeListL(aURI,final);

	TInt ret(KErrNotFound);
	if(final)
		{
		ret = KErrNone;
		if(final->iNode->iDFProperties.iDFType)
			{
			aObject.InsertL(0,final->iNode->iDFProperties.iDFType->iContent,
				final->iNode->iDFProperties.iDFType->iLength);
			}
		else
			{
			aObject.InsertL(0,KNullDesC8);
			}
		}
	return ret;
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::GetCaseSenseL()
// Get mimetype from ddf
// ----------------------------------------------------------------------------
TInt CNSmlDmDDF::GetCaseSenseL(const TDesC8& aURI,CBufBase& aObject)
	{
	_DBG_FILE("CNSmlDmDDF::GetCaseSenseL() : begin");

	CNSmlDmNodeList* final=0;
	TInt ret(KErrNotFound);
	   
    if(aURI.Compare(KNSmlDMRoot)==0)
    {
    	ret = KErrNone;
    	aObject.InsertL(0,KNSmlDDFCaseSense,
				KNSmlDDFCaseSense.iTypeLength);
    }
    else
    {
    iMgmtTree->FindNameFromNodeListL(aURI,final);	
    }
	if(final)
		{
		ret = KErrNone;
		if(final->iNode->iDFProperties.iCaseSense)
			{
			aObject.InsertL(0,KNSmlDDFCaseSense,
				KNSmlDDFCaseSense.iTypeLength);
			}
		else
			{
			aObject.InsertL(0,KNSmlDDFCaseSense,
				KNSmlDDFCaseInSense.iTypeLength);
				
		//	aObject.InsertL(0,KNullDesC8);
			}
		}
	return ret;
	}


// ----------------------------------------------------------------------------
// CNSmlDmDDF::GetNameL()
// Function which gets the name from ddf by using URI
// ----------------------------------------------------------------------------
TInt CNSmlDmDDF::GetNameL(const TDesC8& aURI,CBufBase& aObject)
	{
	_DBG_FILE("CNSmlDmDDF::GetMimeTypeL() : begin");

	CNSmlDmNodeList* final=NULL;
	iMgmtTree->FindNameFromNodeListL(aURI,final);


	if(!final)
		{
		return KErrNotFound;
		}
	else
		{
		aObject.InsertL(0,final->iNode->iNodeName.iContent,
			final->iNode->iNodeName.iLength);
		}
	return KErrNone;
	}



// ----------------------------------------------------------------------------
// void CNSmlDmDDF::RemoveEndAndBeginSlashes(CBufBase &aObject)
// Removes "/" marks from the end and begin of the aObject 
// ----------------------------------------------------------------------------
void CNSmlDmDDF::RemoveEndAndBeginSlashes(CBufBase &aObject) const
	{
	if(aObject.Size()!=0)
		{
		if(aObject.Ptr(0)[0]==KNSmlDMUriSeparator)
			{
			aObject.Delete(0,1);
			}
		if(aObject.Ptr(aObject.Size()-1)[0]==KNSmlDMUriSeparator)
			{
			aObject.Delete(aObject.Size()-1,1);
			aObject.Compress();
			}
		}
	}

// ----------------------------------------------------------------------------
// void CNSmlDmDDF::MoreDataL(CBufBase*& adata)
// Gets more data in case of largeobject
// ----------------------------------------------------------------------------
void CNSmlDmDDF::MoreDataL(CBufBase*& aData)
	{
	if(iResultSessionId>=0)
		{
		iSessionArray[iResultSessionId].MoreDataL(aData);
		}
	}


// ----------------------------------------------------------------------------
// void CNSmlDmDDF::EndMessageL( )
// Message ends
// ----------------------------------------------------------------------------
void CNSmlDmDDF::EndMessageL( )
	{
	_DBG_FILE("CNSmlDmDDF::EndMessageL() : begin");
	TInt i;
	if(iResultSessionId==KErrNotFound)
		{
    	_DBG_FILE("CNSmlDmDDF::EndMessageL() :iResultSessionId==KErrNotFound ");
		for(i=0;i<iSessionArray.Count();i++)
			{
			if(iOkSessions&(0x01<<i))
				{
            	_DBG_FILE("CNSmlDmDDF::EndMessageL():before CompleteOuts...");
				iSessionArray[i].CompleteOutstandingCmdsL();
				}
			}
		}

	iResultRefs.ResetAndDestroy();
	iDeleteStatusRefs.ResetAndDestroy();
	iAddNodeStatusRefs.ResetAndDestroy();
	_DBG_FILE("CNSmlDmDDF::EndMessageL() : end");
	}



// ----------------------------------------------------------------------------
// void CNSmlDmDDF::SetMappingL(const TDesC& aURI, const TDesC& aLUID )
// Callback function which is called by adapter for setting the mapping info
// ----------------------------------------------------------------------------
void CNSmlDmDDF::SetMappingL(const TDesC8& aURI, const TDesC8& aLUID )
	{
	iDbSession.AddMappingInfoL(iAdapterId,aURI,aLUID);
	}



// ----------------------------------------------------------------------------
// void CNSmlDmDDF::SetStatusL( TInt aStatusRef, 
//	CNSmlDmAdapter::TError aErrorCode )
// Callback function which is called by adapter for returning correct
//	statuscode
// ----------------------------------------------------------------------------
void CNSmlDmDDF::SetStatusL( TInt aStatusRef,
	MSmlDmAdapter::TError aErrorCode )
	{

	TInt status = KNSmlDmStatusCommandFailed;
	//map the adapter return code to statuscode
	switch(aErrorCode)
		{
		case MSmlDmAdapter::EOk:
		status=KNSmlDmStatusOK;
		break;

		case MSmlDmAdapter::ENotFound:
		status=KNSmlDmStatusNotFound;
		break;
							
		case MSmlDmAdapter::EInvalidObject:
		break;
							
		case MSmlDmAdapter::EAlreadyExists:
		status=KNSmlDmStatusAlreadyExists;
		break;
							
		case MSmlDmAdapter::ETooLargeObject:
		status=KNSmlDmStatusRequestEntityTooLarge;
		break;
		
		case MSmlDmAdapter::EDiskFull:
		status = KNSmlDmStatusDeviceFull;
		break;

		case MSmlDmAdapter::EError:
		status = KNSmlDmStatusCommandFailed;
		break;

		case MSmlDmAdapter::ERollbackFailed:
		status = KNSmlDmStatusAtomicRollBackFailed;
		break;

		case MSmlDmAdapter::EObjectInUse:
		break;

		case MSmlDmAdapter::ENoMemory:
		status = KNSmlDmStatusDeviceFull;
		break;

		case MSmlDmAdapter::ECommitOK:
		status = KNSmlDmStatusOK;
		break;

		case MSmlDmAdapter::ERollbackOK:
		status = KNSmlDmStatusAtomicRollBackOK;
		break;

		case MSmlDmAdapter::ECommitFailed:
		status=KNSmlDmStatusOK;
		break;
        
		case MSmlDmAdapter::ENotAllowed:
		status = KNSmlDmStatusCommandNotAllowed;
		break;

		case MSmlDmAdapter::EAcceptedForProcessing:
		status =	KNSmlDmStatusAcceptedForProcessing;
		break;
		
		case MSmlDmAdapter::EExecSuccess:
		status =	KNSmlDmStatusSuccess;
		break;
		
case MSmlDmAdapter::EExecClientError:
status =	KNSmlDmStatusClientError;
		break;
case MSmlDmAdapter::		EExecUserCancelled:
status =	KNSmlDmStatusUserCancelled;
		break;
case MSmlDmAdapter::		EExecDownloadFailed:
status =	KNSmlDmStatusDownloadFailed;
		break;
case MSmlDmAdapter::		EExecAltDwnldAuthFail:
status =	KNSmlDmStatusAltDwnldAuthFail;
		break;
case MSmlDmAdapter::		EExecDownFailOOM:
status =	KNSmlDmStatusDownFailOOM;
		break;
case MSmlDmAdapter::		EExecInstallFailed:
status =	KNSmlDmStatusInstallFailed;
		break;
case MSmlDmAdapter::		EExecInstallOOM:
status =	KNSmlDmStatusInstallOOM;
		break;
case MSmlDmAdapter::		EExecPkgValidationFail:
status =	KNSmlDmStatusPkgValidationFail;
		break;
case MSmlDmAdapter::		EExecRemoveFailed:
status =	KNSmlDmStatusRemoveFailed;
		break;
case MSmlDmAdapter::		EExecActivateFailed:
status =	KNSmlDmStatusActivateFailed;
		break;
case MSmlDmAdapter::		EExecDeactivateFailed:
status =	KNSmlDmStatusDeactivateFailed;
		break;
case MSmlDmAdapter::		EExecNotImplemented:
status =	KNSmlDmStatusNotImplemented;
		break;
case MSmlDmAdapter::		EExecUndefError:
status =	KNSmlDmStatusUndefError;
		break;
case MSmlDmAdapter::		EExecOperationReject:
status =	KNSmlDmStatusOperationReject;
		break;
case MSmlDmAdapter::		EExecAltDwnldSrvError:
status =	KNSmlDmStatusAltDwnldSrvError;
		break;
case MSmlDmAdapter::		EExecAltDwnldSrvUnavailable:
status =	KNSmlDmStatusAltDwnldSrvUnavailable;
		break;
		
		default:
		status = KNSmlDmStatusCommandFailed;
		break;
		}
	DeleteandAddStatusRefsL(aStatusRef, aErrorCode);

	iDmModule.DoSetStatusL(aStatusRef,status);
	}


// ----------------------------------------------------------------------------
// void CNSmlDmDDF::SetResultsL( TInt aResultsRef, CBufBase& aObject,
//	const TDesC& aType )
// Callback function which is called by adapter for returning correct
// statuscode
// ----------------------------------------------------------------------------
void CNSmlDmDDF::SetResultsL( TInt aResultsRef,
	CBufBase& aObject,
	const TDesC8& aType,
	TInt aTotalSize,
	TInt8 aSessionId )
	{
	_DBG_FILE("CNSmlDmDDF::SetResultsL() : begin");
	if(aObject.Size()<aTotalSize)
		{
		DBG_ARGS8(_S8("CNSmlDmDDF::SetResultsL,Size= %d, TotalSize= %d"),
		    aObject.Size(),aTotalSize  );
		iResultSessionId=aSessionId;
		}
	else
		{
		DBG_ARGS8(
		    _S8("CNSmlDmDDF::SetResultsL not large,Size= %d,TotalSize= %d"),
		    aObject.Size(),aTotalSize  );
		iResultSessionId=KErrNotFound;
		}
	TInt index = -1;
	for(TInt i=0;i<iResultRefs.Count();i++)
		{
		if(iResultRefs[i]->iRef == aResultsRef)
			{
			if(iResultRefs[i]->iNode)
				{
				RemoveEndAndBeginSlashes(aObject);
				
				iDbSession.UpdateMappingInfoL(iAdapterId,
					*iResultRefs[i]->iUri,aObject);
					
#ifdef NOT_DEFINED //tarm start (removed)
				CNSmlDmNodeList* final=NULL;
            	iMgmtTree->FindNameFromNodeListL(*iResultRefs[i]->iUri,final);

            	if(final&&final->iNode)
            	    {
            	    final = final->iNode->iNodeListPtr;
            	    if(final&&final->iNode->iNodeName.iLength==0)
            	        {
                		iDbSession.DefaultACLsToChildrenL(*iResultRefs[i]->iUri,aObject);
            	        }
            	    }
#endif //tarm end
				}
			index = i;
			break;
			}
		}

	if(index>=0)
		{
		HBufC8* format = GetFormatAllocL(*iResultRefs[index]->iUri);
		CleanupStack::PushL(format);
		delete iResultRefs[index];
		iResultRefs.Remove(index);
		iDmModule.DoSetResultsL(aResultsRef,aObject,aType,
			format->Des(),aTotalSize);
		CleanupStack::PopAndDestroy(); //format
		}
	}



// ===========================================================================
// CNSmlDmLinkCallback
// ===========================================================================

// ----------------------------------------------------------------------------
// CNSmlDmLinkCallback::CNSmlDmLinkCallback()
// ----------------------------------------------------------------------------
CNSmlDmLinkCallback::CNSmlDmLinkCallback()
	{
	}

// ----------------------------------------------------------------------------
// CNSmlDmLinkCallback::~CNSmlDmLinkCallback()
// ----------------------------------------------------------------------------
CNSmlDmLinkCallback::~CNSmlDmLinkCallback()
	{
	delete iResults;
	}

// ----------------------------------------------------------------------------
// CNSmlDmLinkCallback::NewL()
// ----------------------------------------------------------------------------
CNSmlDmLinkCallback* CNSmlDmLinkCallback::NewL()
	{
	CNSmlDmLinkCallback* self = new (ELeave) CNSmlDmLinkCallback();
	return self;
	}

// ----------------------------------------------------------------------------
// CNSmlDmLinkCallback::SetResultsL()
// ----------------------------------------------------------------------------
void CNSmlDmLinkCallback::SetResultsL( TInt /*aResultsRef*/,
	const CBufBase& aObject,
	const TDesC8& /*aType*/,
	const TDesC8& /*aFormat*/,
	TInt /*aTotalSize*/)
	{
	if(!iResults)
		{
		iResults = CBufFlat::NewL(16);
		}
	else
		{
		iResults->Reset();
		}
	iResults->InsertL(0,CONST_CAST(CBufBase&,aObject).Ptr(0),aObject.Size());
	}

// ----------------------------------------------------------------------------
// CNSmlDmLinkCallback::SetStatusL()
// ----------------------------------------------------------------------------
void CNSmlDmLinkCallback::SetStatusL( TInt /*aStatusRef*/, TInt aStatusCode )
	{
	iStatus = aStatusCode;
	}

// ----------------------------------------------------------------------------
// CNSmlDmLinkCallback::GetResultsL()
// ----------------------------------------------------------------------------
void CNSmlDmLinkCallback::GetResultsL(CBufBase*& aData,
	CSmlDmAdapter::TError& aStatus )
	{
	if(iStatus==KNSmlDmStatusOK)
		{
		aStatus = CSmlDmAdapter::EOk;
		}
	else if(iStatus==KNSmlDmStatusNotFound)
		{
		aStatus = CSmlDmAdapter::ENotFound;
		}
	else
		{
		aStatus = CSmlDmAdapter::EError;
		}
	aData = iResults;
	}


// ----------------------------------------------------------------------------
// CNSmlDmLinkCallback::GetStatusL()
// ----------------------------------------------------------------------------
TInt CNSmlDmLinkCallback::GetStatusL()
	{
	return iStatus;
	}

// ===========================================================================
// CNSmlCallbackElement
// ===========================================================================
CNSmlCallbackElement::~CNSmlCallbackElement()
	{
	delete iUri;
	}


// ===========================================================================
// CNSmlDmDDF::DeleteandAddStatusRefsL
// ===========================================================================
void CNSmlDmDDF::DeleteandAddStatusRefsL( TInt aStatusRef,
		MSmlDmAdapter::TError aErrorCode )
	{
	TInt i=0;
	for(i=0;i<iDeleteStatusRefs.Count();i++)
		{
		if(aStatusRef==iDeleteStatusRefs[i]->iRef)
			{
			if(aErrorCode==CSmlDmAdapter::EOk||
					aErrorCode==CSmlDmAdapter::ENotFound)
				{
				//remove mapping info from db
				iDbSession.RemoveMappingInfoL(iAdapterId,
						*iDeleteStatusRefs[i]->iUri);

				iDbSession.DeleteAclL(*iDeleteStatusRefs[i]->iUri);
				}
			delete iDeleteStatusRefs[i];	
			iDeleteStatusRefs.Remove(i);
			break;
			}
		}

	for(i=0;i<iAddNodeStatusRefs.Count();i++)
		{
		if(aStatusRef==iAddNodeStatusRefs[i]->iRef)
			{
			if(aErrorCode==CSmlDmAdapter::EOk)
				{
				//check replace rights of parent
				if(!iDbSession.CheckAclL(NSmlDmURI::ParentURI(
						*iAddNodeStatusRefs[i]->iUri),EAclReplace))
					{
					iDbSession.DefaultACLsToServerL(
							*iAddNodeStatusRefs[i]->iUri);
					}
				}
			delete iAddNodeStatusRefs[i];
			iAddNodeStatusRefs.Remove(i);
			break;
			}
		}
	}
	
// ===========================================================================
// CNSmlDmDDF::UpdateAddNodeListL
// ===========================================================================
void CNSmlDmDDF::UpdateAddNodeListL(const TDesC8& aURI)
    {
    if(iAddRootNodesList) //already created
        {                                                                                     
        //Do check if there is any node already added                 
        if( iAddRootNodesList->Match(aURI) <= KErrNotFound ) 
            {
            iAddRootNodesList = iAddRootNodesList->ReAllocL
            (iAddRootNodesList->Length() + aURI.Length()+ 10);
            iAddRootNodesList->Des().Append(aURI);
            }                 
        }
    else //getting created
        {                 
        iAddRootNodesList = HBufC8::NewL(aURI.Length()+ 10);                  
        iAddRootNodesList->Des().Append(aURI);
        }    
    iAddRootNodesList->Des().Append(',');     
    }
	
