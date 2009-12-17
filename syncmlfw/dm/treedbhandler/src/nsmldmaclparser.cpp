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
* Description:  source file for dm acl parser
*
*/

#include "nsmldmaclparser.h"


// ===============================================================================================
// class CNSmlDmACLParser
// ===============================================================================================


// ---------------------------------------------------------------------------
// TInt CNSmlDmACLParser::NewLC()
// Returns pointer to newly created CNSmlDmACLParser instance
// ---------------------------------------------------------------------------
CNSmlDmACLParser* CNSmlDmACLParser::NewLC()
	{
	CNSmlDmACLParser* self = new (ELeave) CNSmlDmACLParser();
	CleanupStack::PushL( self );
	return self;
	}


// ---------------------------------------------------------------------------
// CNSmlDmACLParser::~CNSmlDmACLParser()
// Destructor
// ---------------------------------------------------------------------------
CNSmlDmACLParser::~CNSmlDmACLParser()
	{
	Reset();
	}

// ---------------------------------------------------------------------------
// CNSmlDmACLParser::ParseL()
// Parses ACL data and keeps data until Reset() is called or instance
// is destructed
// ---------------------------------------------------------------------------
TInt CNSmlDmACLParser::ParseL(const TDesC8& aACL)
	{
	Reset();
	for(TInt i=EAclExecute;i>=EAclAdd;i--)
		{
		TInt aclStart = 0;
		TBool found=EFalse;
		switch(i)
			{
			case EAclAdd:
				aclStart = aACL.Find(KNSmlDmAclAddEqual);
				found = aclStart>=0;
				aclStart=aclStart+KNSmlDmAclAddEqual().Length();
				break;
			case EAclReplace:
				aclStart = aACL.Find(KNSmlDmAclReplaceEqual);
				found = aclStart>=0;
				aclStart=aclStart+KNSmlDmAclReplaceEqual().Length();
				break;
			case EAclDelete:
				aclStart = aACL.Find(KNSmlDmAclDeleteEqual);
				found = aclStart>=0;
				aclStart=aclStart+KNSmlDmAclDeleteEqual().Length();
				break;
			case EAclGet:
				aclStart = aACL.Find(KNSmlDmAclGetEqual);
				found = aclStart>=0;
				aclStart=aclStart+KNSmlDmAclGetEqual().Length();
				break;
			case EAclExecute:
				aclStart = aACL.Find(KNSmlDmAclExecEqual);
				found = aclStart>=0;
				aclStart=aclStart+KNSmlDmAclExecEqual().Length();
				break;
			default:
				User::Panic(KSmlDmTreeDbHandlerPanic,KErrArgument);
				break;

			}
		if(found)
			{
			TInt aclStop = aACL.Right(aACL.Length()-aclStart).
				Locate(KNSmlDMAclCommandSeparator);

			if(aclStop<0)
				{
				aclStop = aACL.Length()-aclStart;
				}

			TPtrC8 commandAcl = aACL.Mid(aclStart,aclStop);

			CNSmlAclElement* aclElement = new(ELeave) CNSmlAclElement();

			aclElement->iCommandType = (TNSmlDmCmdType)i;
			aclElement->iNext = iCommandAcls;
			iCommandAcls=aclElement;

			if(commandAcl.Compare(KNSmlDmAclAll)==0)
				{
				aclElement->iAllServers=ETrue;
				}
			else
				{
				TBool end = EFalse;

				TInt serverIdStart=0;
				while(!end)
					{
					TPtrC8 serverIdPtr =
						commandAcl.Right(commandAcl.Length()-serverIdStart);
						
					TInt serverIdStop =
						serverIdPtr.Locate(KNSmlDMAclSeparator);
						
					if(serverIdStop == KErrNotFound)
						{
						serverIdStop=commandAcl.Length();
						end=ETrue;
						}
					HBufC8* serverId =
						serverIdPtr.Left(serverIdStop).AllocL();
						
					aclElement->iServerIds.AppendL(serverId);
					serverIdStart=serverIdStart+serverIdStop+1;
					}
				}
			}
		}
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CNSmlDmACLParser::GenerateL()
// Generates ACL data string from the kept data.
// ---------------------------------------------------------------------------
HBufC8* CNSmlDmACLParser::GenerateL()
	{
	CBufBase *acl = CBufFlat::NewL(32);
	CleanupStack::PushL(acl);

	CNSmlAclElement* aclElement=iCommandAcls;

	while(aclElement)
		{
		TBool anyServerIds=EFalse;
		if(aclElement->iAllServers||aclElement->iServerIds.Count())
			{
			anyServerIds = ETrue;
			switch(aclElement->iCommandType)
				{
				case EAclAdd:
					acl->InsertL(acl->Size(),KNSmlDmAclAddEqual);
					break;
				case EAclReplace:
					acl->InsertL(acl->Size(),KNSmlDmAclReplaceEqual);
					break;
				case EAclDelete:
					acl->InsertL(acl->Size(),KNSmlDmAclDeleteEqual);
					break;
				case EAclGet:
					acl->InsertL(acl->Size(),KNSmlDmAclGetEqual);
					break;
				case EAclExecute:
					acl->InsertL(acl->Size(),KNSmlDmAclExecEqual);
					break;
				default:
					User::Panic(KSmlDmTreeDbHandlerPanic,KErrArgument);
					break;
				}
			if(aclElement->iAllServers)
				{
				acl->InsertL(acl->Size(),KNSmlDmAclAll);
				}
			else
				{
				for(TInt i=0;i<aclElement->iServerIds.Count();i++)
					{
					acl->InsertL(acl->Size(),*aclElement->iServerIds[i]);
					if(i<aclElement->iServerIds.Count()-1)
						{
						acl->InsertL(acl->Size(),KNSmlDmAclServerIdSeparator);
						}
					}
				}
			}

		aclElement=aclElement->iNext;
		if(aclElement&&anyServerIds)
			{
			acl->InsertL(acl->Size(),KNSmlDmAclSeparator);
			}
		}
	
	HBufC8 *aclBuf = HBufC8::NewL(acl->Size());
	TPtr8 bufPtr = aclBuf->Des();
	acl->Read(0,bufPtr,acl->Size());
	CleanupStack::PopAndDestroy(); //acl
	return aclBuf;
	}

// ---------------------------------------------------------------------------
// CNSmlDmACLParser::RemoveAllReferences()
// Removes all references to aServerId
// ---------------------------------------------------------------------------
void CNSmlDmACLParser::RemoveAllReferences(const TDesC8& aServerId)
	{
	CNSmlAclElement* aclElement=iCommandAcls;


	while(aclElement)
		{
		for(TInt i=0;i<aclElement->iServerIds.Count();i++)
			{
			if(aclElement->iServerIds[i]->Compare(aServerId)==0)
				{
				delete aclElement->iServerIds[i];
				aclElement->iServerIds.Remove(i);
				break;
				}
			}
		aclElement=aclElement->iNext;
		}
	}

// ---------------------------------------------------------------------------
// CNSmlDmACLParser::ShouldDelete()
// Check if the instance contains any data to generate acl string.
// If not acl can be deleted
// ---------------------------------------------------------------------------
TBool CNSmlDmACLParser::ShouldDelete()
	{
	CNSmlAclElement* aclElement=iCommandAcls;

	while(aclElement)
		{
		if(aclElement->iAllServers||aclElement->iServerIds.Count())
			{
			return EFalse;
			}
		aclElement=aclElement->iNext;
		}
	return ETrue;	
	}

// ---------------------------------------------------------------------------
// CNSmlDmACLParser::HasRights()
// Check if the aServerId server has rights for command aCommand type
// ---------------------------------------------------------------------------
TBool CNSmlDmACLParser::HasRights(const TDesC8& aServerId,
	TNSmlDmCmdType aCommandType)
	{
	CNSmlAclElement* aclElement=iCommandAcls;
	TBool ret = EFalse;

	while(aclElement)
		{
		if(aclElement->iCommandType==aCommandType)
			{
			if(aclElement->iAllServers)
				{
				ret = ETrue;
				}
			else
				{
				for(TInt i=0;i<aclElement->iServerIds.Count();i++)
					{
					if(aclElement->iServerIds[i]->Compare(aServerId)==0)
						{
						ret = ETrue;
						break;
						}
					}
				}
			if(ret)
				{
				break;
				}
			}
		aclElement=aclElement->iNext;
		}
	return ret;	
	}

// ---------------------------------------------------------------------------
// CNSmlDmACLParser::Reset()
// Resets the instance data
// ---------------------------------------------------------------------------
 void CNSmlDmACLParser::Reset()
	{
	CNSmlAclElement* aclElement=iCommandAcls;

	while(iCommandAcls)
		{
		aclElement = iCommandAcls->iNext;
		delete iCommandAcls;
		iCommandAcls=aclElement;
		}
	}


// ===============================================================================================
// class CNSmlAclElement
// ===============================================================================================

// ---------------------------------------------------------------------------
// CNSmlAclElement::CNSmlAclElement()
// constructor
// ---------------------------------------------------------------------------
CNSmlAclElement::CNSmlAclElement()
	{
	}
	
// ---------------------------------------------------------------------------
// CNSmlAclElement::~CNSmlAclElement()
// destructor
// ---------------------------------------------------------------------------
CNSmlAclElement::~CNSmlAclElement()
	{
	iServerIds.ResetAndDestroy();
	}
