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

#include "nsmldmcommandbuffer.h"
#include "nsmldmtreeconstants.h"
#include "nsmldmddf.h"
#include "nsmldmuri.h"


// ===========================================================================
// CNSmlDmCommandBuffer
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::~CNSmlDmCommandBuffer()
// ---------------------------------------------------------------------------
CNSmlDmCommandBuffer::~CNSmlDmCommandBuffer()
	{
	if(iCommandBuffer !=0 )
	    {
	    CNSmlDmCommands* tmp=iCommandBuffer;
    	    while(tmp!=0)
    		{
        		tmp=iCommandBuffer->iNext;
        		delete iCommandBuffer;
        		iCommandBuffer = tmp;
    		}
	    }
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::CNSmlDmCommandBuffer()
// ---------------------------------------------------------------------------
CNSmlDmCommandBuffer::CNSmlDmCommandBuffer(CNSmlDmModule& aDmModule)
	:iDmModule(aDmModule)
	{
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer* CNSmlDmCommandBuffer::NewL()
// ---------------------------------------------------------------------------
CNSmlDmCommandBuffer* CNSmlDmCommandBuffer::NewL(CNSmlDmModule& aDmModule)
	{
	CNSmlDmCommandBuffer* self = new (ELeave) CNSmlDmCommandBuffer(aDmModule);
	return self;
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::AddObjectL()
// Adds an Add command to the command buffer. The command is executed
// in Commit
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::AddObjectL(const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	TInt aStatusRef )
	{
	CNSmlDmCommands* cmd = new (ELeave) CNSmlDmCommands;
	CleanupStack::PushL(cmd);
	
	cmd->iURI = aURI.AllocL();
	cmd->iObject = aObject.AllocL();
	cmd->iType = aType.AllocL();
	cmd->iStatusRef = aStatusRef;
	cmd->iCmdType = EAclAdd;

	if(iCommandBuffer==0)
		{
		iCommandBuffer = cmd;
		}
	else
		{
		CNSmlDmCommands* tmp=iCommandBuffer;
		while(tmp->iNext!=0)
			{
			tmp=tmp->iNext;
			}
		tmp->iNext = cmd;
		}
	CleanupStack::Pop(); //cmd
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::UpdateObjectL()
// Adds an Update command to the command buffer. The command is executed
// in Commit
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::UpdateObjectL( const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	TInt aStatusRef)
	{
	CNSmlDmCommands* cmd = new (ELeave) CNSmlDmCommands;
	CleanupStack::PushL(cmd);
	
	cmd->iURI = aURI.AllocL();
	cmd->iObject = aObject.AllocL();
	cmd->iType = aType.AllocL();
	cmd->iStatusRef = aStatusRef;
	cmd->iCmdType = EAclReplace;

	if(iCommandBuffer==0)
		{
		iCommandBuffer = cmd;
		}
	else
		{
		CNSmlDmCommands* tmp=iCommandBuffer;
		while(tmp->iNext!=0)
			{
			tmp=tmp->iNext;
			}
		tmp->iNext = cmd;
		}
	CleanupStack::Pop();
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::FetchObjectL()
// Adds an Fetch command to the command buffer. The command is executed in Commit
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::FetchObjectL(const TDesC8& aURI,
	const TDesC8& aType,
	TInt aResultsRef,
	TInt aStatusRef )
	{
	CNSmlDmCommands* cmd = new (ELeave) CNSmlDmCommands;
	CleanupStack::PushL(cmd);
	
	cmd->iURI = aURI.AllocL();
	cmd->iType = aType.AllocL();
	cmd->iStatusRef = aStatusRef;
	cmd->iCmdType = EAclGet;
	cmd->iResultsRef = aResultsRef;

	if(iCommandBuffer==0)
		{
		iCommandBuffer = cmd;
		}
	else
		{
		CNSmlDmCommands* tmp=iCommandBuffer;
		while(tmp->iNext!=0)
			{
			tmp=tmp->iNext;
			}
		tmp->iNext = cmd;
		}
	CleanupStack::Pop(); //cmd
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::DeleteObjectL()
// Adds an Delete command to the command buffer. The command is executed
// in Commit
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::DeleteObjectL( const TDesC8& aURI, TInt aStatusRef)
	{
	CNSmlDmCommands* cmd = new (ELeave) CNSmlDmCommands;
	CleanupStack::PushL(cmd);
	
	cmd->iURI = aURI.AllocL();
	cmd->iStatusRef = aStatusRef;
	cmd->iCmdType = EAclDelete;

	if(iCommandBuffer==0)
		{
		iCommandBuffer = cmd;
		}
	else
		{
		CNSmlDmCommands* tmp=iCommandBuffer;
		while(tmp->iNext!=0)
			{
			tmp=tmp->iNext;
			}
		tmp->iNext = cmd;
		}
	CleanupStack::Pop(); //cmd
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::ExecuteObjectL()
// Adds an Execute command to the command buffer. The command is executed
// in Commit
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::ExecuteObjectL( const TDesC8& aURI,
	const TDesC8& aObject,
	const TDesC8& aType,
	TInt aStatusRef)
	{
	CNSmlDmCommands* cmd = new (ELeave) CNSmlDmCommands;
	CleanupStack::PushL(cmd);
	
	cmd->iURI = aURI.AllocL();
	cmd->iObject = aObject.AllocL();
	cmd->iType = aType.AllocL();
	cmd->iStatusRef = aStatusRef;
	cmd->iCmdType = EAclExecute;

	if(iCommandBuffer==0)
		{
		iCommandBuffer = cmd;
		}
	else
		{
		CNSmlDmCommands* tmp=iCommandBuffer;
		while(tmp->iNext!=0)
			{
			tmp=tmp->iNext;
			}
		tmp->iNext = cmd;
		}
	CleanupStack::Pop(); //cmd
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::CopyObjectL()
// Adds an Copy command to the command buffer. The command is executed
// in Commit
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::CopyObjectL( const TDesC8& aTargetURI,
	const TDesC8& aSourceURI,
	const TDesC8& aType,
	TInt aStatusRef)
	{
	CNSmlDmCommands* cmd = new (ELeave) CNSmlDmCommands;
	CleanupStack::PushL(cmd);
	
	cmd->iURI = aSourceURI.AllocL();
	cmd->iTargetURI = aTargetURI.AllocL();
	cmd->iType = aType.AllocL();
	cmd->iStatusRef = aStatusRef;
	cmd->iCmdType = EAclCopy;

	if(iCommandBuffer==0)
		{
		iCommandBuffer = cmd;
		}
	else
		{
		CNSmlDmCommands* tmp=iCommandBuffer;
		while(tmp->iNext!=0)
			{
			tmp=tmp->iNext;
			}
		tmp->iNext = cmd;
		}
	CleanupStack::Pop(); //cmd
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::IsGetWithAtomic()
// Checks whether nested Get command is there inside atomic command
// ---------------------------------------------------------------------------
TBool CNSmlDmCommandBuffer::IsGetWithAtomic()
{ TBool isget(EFalse);
	TInt  isnested = -1;
 CNSmlDmCommands* cmd=iCommandBuffer;
 
 while(cmd !=0)
 {
 	if(cmd->iCmdType == EAclGet)
    {  
    	
    	isget =ETrue;
    }
  isnested++;
  
  cmd=cmd->iNext;
 }
if(isnested > 0 && isget)
    {
    	  return ETrue;
    } 
    
  
 return EFalse;   
   
 }
// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::CommitL()
// Executes the commands in commandbuffer. If some of commands fails,
// the rest of commands are deleted without execution
// (NotExecuted status is returned)
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::CommitL(  CNSmlDmDDF& aDDF )
	{
	aDDF.StartAtomicL();
	CNSmlDmCommands* cmd=iCommandBuffer;
	while(cmd!=0)
		{
		if(!cmd->iGotStatus)
			{
			//check the access from the ddf (not acl)
			CNSmlDmDDF::TAccess access = aDDF.CheckURIL(
				NSmlDmURI::RemoveProp(cmd->iURI->Des()),cmd->iCmdType);
	
			if(access == CNSmlDmDDF::EOk ) //access ok
				{
				TInt offset = cmd->iURI->Find(KNSmlDmProperty);
				if(offset==KErrNotFound) //not property asked
					{
					switch(cmd->iCmdType)
						{
						case EAclAdd:
						aDDF.AddObjectL(*cmd->iURI,*cmd->iObject,
							*cmd->iType,cmd->iStatusRef);
						break;
			
						case EAclReplace:
						aDDF.UpdateObjectL(*cmd->iURI,*cmd->iObject,
							*cmd->iType,cmd->iStatusRef);
						break;
			
						case EAclGet:
						if(IsGetWithAtomic())
						{
							cmd->iStatusCode = KNSmlDmStatusCommandFailed;
				        cmd->iGotStatus = ETrue;
						}
						else
						aDDF.FetchObjectL(*cmd->iURI,*cmd->iType,
							cmd->iResultsRef,cmd->iStatusRef);
						break;
			
						case EAclDelete:
						aDDF.DeleteObjectL(*cmd->iURI,cmd->iStatusRef);
						break;

						case EAclExecute:
						// FOTA
						aDDF.ExecuteObjectL(*cmd->iURI,*cmd->iObject,
							*cmd->iType,cmd->iStatusRef, KNullDesC8);
						// FOTA end							
						break;

						case EAclCopy:
						aDDF.CopyObjectL(*cmd->iTargetURI,*cmd->iURI,
							*cmd->iType,cmd->iStatusRef);
						break;
						default:
						User::Panic ( KNSmlTxtPanicDmModule, KErrArgument );
						break;
						}
					}
				else //property
					{
					switch(cmd->iCmdType)
						{
						case EAclReplace:
						iDmModule.UpdatePropertyL(*cmd->iURI,*cmd->iObject,
							*cmd->iType,offset+KNSmlDmProperty().Length(),
							cmd->iStatusRef);
						break;
			
						case EAclGet:
						iDmModule.GetPropertyL(*cmd->iURI,*cmd->iType,
							KNSmlDmProperty().Length()+offset,
							cmd->iResultsRef,cmd->iStatusRef);
						break;

						default:
						cmd->iStatusCode = KNSmlDmStatusCommandNotAllowed;
						cmd->iGotStatus = ETrue;
						break;

						}

					} //end else
				} //end if(CheckURIL())
			else if(access == CNSmlDmDDF::ENotAccess )
				{
				cmd->iStatusCode = KNSmlDmStatusCommandNotAllowed;
				cmd->iGotStatus = ETrue;
				}
			else
				{
				SetStatus(cmd->iStatusRef,KNSmlDmStatusNotFound);
				}
			}
		cmd=cmd->iNext;
		} //end while
	aDDF.EndMessageL();
	if(AtomicFailed())
		{
		aDDF.RollbackAtomicL();
		RollBackL(aDDF);
		}
	else
		{
		aDDF.CommitAtomicL();		
		}
	ChangeAtomicStatuses();
	SendStatusAndResultCodesL();
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::RollBackL()
// Returns status NotExecuted to the commands in commandbuffer
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::RollBackL(CNSmlDmDDF& aDDF)
	{
	CNSmlDmCommands* cmd=iCommandBuffer;
	TBool loopAgain = EFalse;
	while(cmd!=0)
		{
		if(cmd->iGotStatus &&
			cmd->iStatusCode==KNSmlDmStatusOK &&
			cmd->iURI->Find(KNSmlDmProperty)==KErrNotFound)
			{
			if(cmd->iCmdType==EAclAdd)
				{
				loopAgain=ETrue;
				delete cmd->iCallback;
				cmd->iCallback=0;
				cmd->iCallback = CNSmlDmLinkCallback::NewL();
				iDmModule.DeleteInTransactionL(*cmd->iURI,cmd->iCallback);
				}
			if(cmd->iCmdType!=EAclGet)
				{
				cmd->iStatusCode = KNSmlDmStatusAtomicRollBackFailed;
				}
			}
			else if(cmd->iStatusCode != KNSmlDmStatusOK)
			{	
				cmd->iStatusCode = KNSmlDmStatusCommandFailed;
			}
		cmd=cmd->iNext;
		}

	if(loopAgain)
		{
		cmd=iCommandBuffer;
		while(cmd!=0)
			{
			if(cmd->iCmdType==EAclAdd&&cmd->iCallback)
				{
				TInt status = cmd->iCallback->GetStatusL();
				if(status==KNSmlDmStatusOK || ParentNodeDeletedL(*cmd->iURI) ||
					(status==KNSmlDmStatusNotFound&&
					aDDF.CheckURIL(*cmd->iURI,EAclAdd)==CNSmlDmDDF::EOk) )
					{
					cmd->iStatusCode = KNSmlDmStatusAtomicRollBackOK;
					}
				}
			else if(cmd->iCmdType==EAclReplace)
				{
				if(ParentNodeDeletedL(*cmd->iURI))
					{
					cmd->iStatusCode = KNSmlDmStatusAtomicRollBackOK;
					}
				}
			cmd=cmd->iNext;
			}
		}
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::ChangeAtomicStatuses()
// Changes all statuses so that they matches to atomic statuses
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::ChangeAtomicStatuses()
	{
	CNSmlDmCommands* cmd=iCommandBuffer;
	
	if(iAtomicFailed)
		{
		while(cmd!=0)
			{
			if(cmd->iCmdType==EAclReplace &&
				cmd->iURI->Find(KNSmlDmProperty) &&
				cmd->iStatusCode == KNSmlDmStatusOK)
				{
				cmd->iStatusCode = KNSmlDmStatusAtomicRollBackOK;
				}
			if( cmd->iStatusCode<KNSmlDmStatusLargestOK)
				{
				cmd->iStatusCode = KNSmlDmStatusAtomicRollBackFailed;
				}

			cmd=cmd->iNext;
			}		
		}
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::CheckResultsRef()
// Check if this fetch has came inside atomc
// ---------------------------------------------------------------------------
TBool CNSmlDmCommandBuffer::CheckResultsRef(TInt aResultsRef)
	{
	CNSmlDmCommands* cmd=iCommandBuffer;
	while(cmd!=0)
		{
		if(aResultsRef==cmd->iResultsRef)
			{
			return ETrue;
			}
		cmd=cmd->iNext;
		}
	return EFalse;
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::CheckStatusRef()
// Check if this command has came inside atomc
// ---------------------------------------------------------------------------
TBool CNSmlDmCommandBuffer::CheckStatusRef(TInt aStatusRef)
	{
	CNSmlDmCommands* cmd=iCommandBuffer;
	while(cmd!=0)
		{
		if(aStatusRef==cmd->iStatusRef)
			{
			return ETrue;
			}
		cmd=cmd->iNext;
		}
	return EFalse;
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::SetResultsL( TInt aResultsRef,
// 	const CBufBase& aObject,
// 	const TDesC8& aType,
// 	const TDesC8& aFormat )
// Set Results to buffer for later returning
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::SetResultsL( TInt aResultsRef,
	const CBufBase& aObject,
	const TDesC8& /*aType*/,
	const TDesC8& aFormat )
	{
	CNSmlDmCommands* cmd=iCommandBuffer;
	while(cmd!=0)
		{
		if(aResultsRef==cmd->iResultsRef)
			{
			cmd->iFormat = aFormat.AllocL();
			if(cmd->iResult)
				{
				cmd->iResult->Reset();
				}
			else
				{
				cmd->iResult = CBufFlat::NewL(32);
				}
			cmd->iResult->InsertL(0,CONST_CAST(CBufBase&, aObject).Ptr(0),
				aObject.Size());
			return;
			}
		cmd=cmd->iNext;
		}
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::SetStatusL(TInt aStatusRef, TInt aStatusCode)
// Set Status to buffer for later returning
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::SetStatus(TInt aStatusRef, TInt aStatusCode)
	{
	CNSmlDmCommands* cmd=iCommandBuffer;
	if(aStatusCode>KNSmlDmStatusLargestOK)
		{
		iAtomicFailed = ETrue;
		}
	while(cmd!=0)
		{
		if(iAtomicFailed&&!cmd->iGotStatus)
			{
			cmd->iGotStatus = ETrue;
			cmd->iStatusCode = KNSmlDmStatusNotExecuted;
			}
		if(aStatusRef==cmd->iStatusRef)
			{
			cmd->iGotStatus = ETrue;
			cmd->iStatusCode = aStatusCode;
			}
		cmd=cmd->iNext;
		}
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::SendStatusAndResultCodesL()
// Send the results and status codes from the buffer
// ---------------------------------------------------------------------------
void CNSmlDmCommandBuffer::SendStatusAndResultCodesL()
	{
	CNSmlDmCommands* cmd=iCommandBuffer;

	while(cmd!=0)
		{
		if(cmd->iCmdType==EAclGet && cmd->iStatusCode>0 &&
			cmd->iStatusCode<KNSmlDmStatusLargestOK)
			{
			iDmModule.DoSetResultsL(cmd->iResultsRef,*cmd->iResult,
				*cmd->iType,*cmd->iFormat,cmd->iResult->Size(),ETrue);
			}
		iDmModule.DoSetStatusL(cmd->iStatusRef,cmd->iStatusCode,ETrue);
		cmd=cmd->iNext;
		}
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::ParentNodeDeletedL()
// Checks if parent node has been deleted
// ---------------------------------------------------------------------------
TBool CNSmlDmCommandBuffer::ParentNodeDeletedL(const TDesC8& aURI)
	{
	
	HBufC8* uri = NSmlDmURI::RemoveProp(aURI).AllocLC();
	TPtr8 uriPtr = uri->Des();
	
	while(uriPtr.Length()!=0)
		{

		CNSmlDmCommands* cmd=iCommandBuffer;

		while(cmd!=0)
			{
			if(cmd->iCallback&&cmd->iCallback->GetStatusL()==KNSmlDmStatusOK)
				{
				if(cmd->iURI->Compare(uriPtr)==0)
					{
					CleanupStack::PopAndDestroy(); //uri
					return ETrue;
					}
				}
			cmd=cmd->iNext;
			}
		uriPtr = NSmlDmURI::RemoveLastSeg(uriPtr);
		}
	CleanupStack::PopAndDestroy(); //uri
	return EFalse;
	}


// ---------------------------------------------------------------------------
// CNSmlDmCommandBuffer::AtomicFailed()
// Checks if some command has failed or status missing inside the atomic
// ---------------------------------------------------------------------------
TBool CNSmlDmCommandBuffer::AtomicFailed()
	{
		
	TBool isnestedget =	IsGetWithAtomic();
	if(isnestedget)
	  return ETrue;
	if(iAtomicFailed)
		{
		return ETrue;
		}
	else
		{
		CNSmlDmCommands* cmd=iCommandBuffer;
		while(cmd!=0)
			{
			if(!cmd->iGotStatus)
				{
				return ETrue;
				}
			cmd=cmd->iNext;
			}
		}
	return EFalse;
	}
	

// ---------------------------------------------------------------------------
// CNSmlDmCommands::CNSmlDmCommands
// Constructor
// ---------------------------------------------------------------------------
CNSmlDmCommands::CNSmlDmCommands()
	{
	iStatusCode=KNSmlDmStatusNotExecuted;
	}

// ---------------------------------------------------------------------------
// CNSmlDmCommands::~CNSmlDmCommands
// Destructor
// ---------------------------------------------------------------------------
CNSmlDmCommands::~CNSmlDmCommands()
	{
	delete iURI;
	delete iTargetURI;
	delete iResult;
	delete iObject;
	delete iType;
	delete iFormat;
	delete iCallback;
	}

