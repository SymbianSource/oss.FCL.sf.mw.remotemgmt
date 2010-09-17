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
* Description:  DM callback server sewssion source file
*
*/

#include <s32mem.h>
#include <smldmadapter.h>

#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldmdbhandler.h>
#include "nsmldmcallbackserver.h"
#include "nsmldmcallbackserverdefs.h"


// ===========================================================================
// CNSmlDmCallbackSession
// ===========================================================================


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::NewL()
// ---------------------------------------------------------------------------
CNSmlDmCallbackSession* CNSmlDmCallbackSession::NewL(
	CNSmlDmCallbackServer& aServer)
	{
	CNSmlDmCallbackSession* self=
		new (ELeave) CNSmlDmCallbackSession( aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::CNSmlDmCallbackSession()
// ---------------------------------------------------------------------------
CNSmlDmCallbackSession::CNSmlDmCallbackSession(
	CNSmlDmCallbackServer& aServer ) : iServer(aServer)
	{
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::ConstructL()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::ConstructL()
	{
	iUriSegList =
		new (ELeave) CArrayFixFlat <TSmlDmMappingInfo> (KGranularity);
	iServer.Increase();
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::~CNSmlDmCallbackSession()
// ---------------------------------------------------------------------------
CNSmlDmCallbackSession::~CNSmlDmCallbackSession()
	{
	iServer.Decrease();
	if ( iUriSegList )
		{
		iUriSegList->Reset();
		delete iUriSegList;
		}
	delete iLuid;
	delete iSegList;
	delete iACL;
	delete iACLUri;
	iAtomicAclURIs.ResetAndDestroy();
	iAtomicAcls.ResetAndDestroy();
 	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::ServiceL( )
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::ServiceL( const RMessage2& aMessage )
	{
	TRAPD(err,DispatchMessageL(aMessage));
	if(err!=KErrNone)
	    {
	    aMessage.Complete(err);
	    }
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::DispatchMessageL( )
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::DispatchMessageL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::DispatchMessageL(): begin");
	switch( aMessage.Function() )
        {
        case ECmdDMCallbackSetServer:
        	SetServerL(aMessage);
        	return;
     // requests
        case ECmdDMCallbackGetLuid:
			GetLuidL( aMessage );
			return;
        case ECmdDMCallbackGetSegmentList:
			GetUriSegListL(aMessage );
            return;
        case ECmdDMCallbackGetLuidLength:
			GenerateLuidL( aMessage );
			return;
        case ECmdDMCallbackGetSegmentListLength:
			GenerateUriSegListL(aMessage );
            return;
        case ECmdDMCallbackAddMappingInfo:
        	AddMappingInfoL(aMessage);
        	return;
        case ECmdDMCallbackRemoveMappingInfo:
        	RemoveMappingInfoL(aMessage);
        	return;
        case ECmdDMCallbackRenameMapping:
        	RenameMappingInfoL(aMessage);
        	return;
        case ECmdDMCallbackUpdateMapping:
        	UpdateMappingInfoL(aMessage);
        	return;
        case ECmdDMCallbackDeleteAcl:
        	DeleteAclL(aMessage);
        	return;
        case ECmdDMCallbackUpdateAcl:
        	UpdateAclL(aMessage);
        	return;
        case ECmdDMCallbackGetAclLength:
        	GetAclLengthL(aMessage);
        	return;
        case ECmdDMCallbackGetAcl:
        	GetAclL(aMessage);
        	return;
        case ECmdDMCallbackCheckAcl:
        	CheckAclL(aMessage);
        	return;
        case ECmdDMCallbackDefaultAclsToServer:
        	DefaultAclsToServerL(aMessage);
        	return;
        case ECmdDMCallbackDefaultAclsToChildren:
        	DefaultAclsToChildrenL(aMessage);
        	return;
        case ECmdDMCallbackEraseServerId:
        	EraseServerIdL(aMessage);
        	return;
        case ECmdDMCallbackStartAtomic:
        	StartAtomic(aMessage);
        	return;
        case ECmdDMCallbackCommitAtomic:
        	CommitAtomicL(aMessage);
        	return;
        case ECmdDMCallbackRollbackAtomic:
        	RollbackAtomic(aMessage);
        	return;
        case ECmdDMCallbackCloseSession:
            aMessage.Complete(KErrNone);
            return;
        case ECmdDMCallbackResourceCount:
		    aMessage.Complete(KErrNone);
            return;
        default:
            aMessage.Panic( KNSmlDmCallbackServerName, KErrArgument );
            return;
        }
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::SetServerL()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::SetServerL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::SetServerL(): begin");
    HBufC8* server = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 serverPtr = server->Des();
    aMessage.ReadL( 1, serverPtr);
    
	iServer.DbHandler()->SetServerL(serverPtr);

	CleanupStack::PopAndDestroy(); //server

    aMessage.Complete(KErrNone);
		
	_DBG_FILE("CNSmlDmCallbackSession::SetServerL(): end");
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::GetLuidL()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::GetLuidL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::GetLuidL(): begin");
	if(!iLuid)
		{
		iLuid = KNullDesC8().AllocL();
		}
		
	TPtr8 bufPtr=iLuid->Des();
	
    aMessage.WriteL( 3, bufPtr);
    
    aMessage.Complete(KErrNone);
    
    delete iLuid;
    iLuid = 0;
	
	_DBG_FILE("CNSmlDmCallbackSession::GetLuidL(): end");
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::GetUriSegListL()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::GetUriSegListL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::GetUriSegListL(): begin");
	if(!iSegList)
		{
		iSegList = CBufFlat::NewL(1);
		}
		
	TPtr8 bufPtr=iSegList->Ptr(0);
	
    aMessage.WriteL( 3, bufPtr);
    
    aMessage.Complete(KErrNone);

	delete iSegList;
    iSegList = 0;

	_DBG_FILE("CNSmlDmCallbackSession::GetUriSegListL(): end");
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::GenerateLuidL()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::GenerateLuidL( const RMessage2& aMessage )
	{
	TInt ret(KErrNone);
	if(iLuid)
		{
		delete iLuid;
		iLuid =0;
		}
	HBufC8* luid = NULL;

	TUint32 adapterId = aMessage.Int0();
	
    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);
    
	ret = iServer.DbHandler()->GetMappingInfoLC(adapterId,uriPtr,luid);
	iLuid = luid->AllocL();
	CleanupStack::PopAndDestroy(2); //luid, uri

	TPckgBuf<TInt> len;
	len()=iLuid->Length();

    aMessage.WriteL(3, len, 0);
    aMessage.Complete(ret);
	}



// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::GenerateUriSegListL()
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::GenerateUriSegListL( const RMessage2& aMessage )
	{
	TInt ret(KErrNone);
	if(iSegList)
		{
		delete iSegList;
		iSegList =0;
		}
	iSegList = CBufFlat::NewL(16);

	TUint32 adapterId = aMessage.Int0();
	
    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);
    
 	TRAPD(err,ret = iServer.DbHandler()->
 		GetURISegmentListL(adapterId,uriPtr,*iUriSegList));

	CleanupStack::PopAndDestroy(); //uri
	if(err!=KErrNone)
		{
		ret = err;
		}
	
 	
 	RBufWriteStream writeStream(*iSegList,0);
	CleanupClosePushL(writeStream);
	TUint16 count=iUriSegList->Count();
	writeStream.WriteUint16L(count);
	for(TUint16 i=0;i<count;i++)
		{
		writeStream.WriteUint8L(iUriSegList->At(i).iURISeg.Length());
		writeStream.WriteL(iUriSegList->At(i).iURISeg);
		writeStream.WriteUint8L(iUriSegList->At(i).iURISegLUID.Length());
		writeStream.WriteL(iUriSegList->At(i).iURISegLUID);
		}

	iUriSegList->Reset();
	
	CleanupStack::PopAndDestroy(); //writeStream
	
    TPckgBuf<TInt> len(iSegList->Size());

    aMessage.WriteL(3, len, 0);
    aMessage.Complete(ret);
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::AddMappingInfoL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::AddMappingInfoL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::AddMappingInfoL(): begin");

	TUint32 adapterId = aMessage.Int0();
	
    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

    HBufC8* luid = HBufC8::NewLC( aMessage.GetDesLengthL(2) );
    TPtr8 luidPtr = luid->Des();
    aMessage.ReadL( 2, luidPtr);
    
	TInt ret = iServer.DbHandler()->AddMappingInfoL(adapterId,uriPtr,luidPtr);

	CleanupStack::PopAndDestroy(2); //uri, luid
	iServer.DbHandler()->WriteMappingInfoToDbL();	

    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::AddMappingInfoL(): end");
	}



// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::RemoveMappingInfoL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::RemoveMappingInfoL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::RemoveMappingInfoL(): begin");

	TUint32 adapterId = aMessage.Int0();
	
    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

	TBool childAlso = aMessage.Int2();
    
	TInt ret = iServer.DbHandler()->
		RemoveMappingInfoL(adapterId,uriPtr,childAlso);

	CleanupStack::PopAndDestroy(); //uri
	iServer.DbHandler()->WriteMappingInfoToDbL();	

    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::RemoveMappingInfoL(): end");
	}


    
// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::RenameMappingInfoL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::RenameMappingInfoL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::RenameMappingInfoL(): begin");

	TUint32 adapterId = aMessage.Int0();
	
    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

    HBufC8* newName = HBufC8::NewLC( aMessage.GetDesLengthL(2) );
    TPtr8 newNamePtr = newName->Des();
    aMessage.ReadL( 2, newNamePtr);
    
 	TInt ret = iServer.DbHandler()->
 		RenameMappingL(adapterId,uriPtr,newNamePtr);

	CleanupStack::PopAndDestroy(2); //uri, newName
	iServer.DbHandler()->WriteMappingInfoToDbL();	

    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::RenameMappingInfoL(): end");
	}


   
// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::UpdateMappingInfoL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::UpdateMappingInfoL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::UpdateMappingInfoL(): begin");

	TUint32 adapterId = aMessage.Int0();
	
    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

    HBufC8* luidList = HBufC8::NewLC( aMessage.GetDesLengthL(2) );
    TPtr8 luidListPtr = luidList->Des();
    aMessage.ReadL( 2, luidListPtr);
    
    CBufBase *childList = CBufFlat::NewL(16);
    CleanupStack::PushL(childList);
    childList->InsertL(0,luidListPtr);
    
 	iServer.DbHandler()->UpdateMappingInfoL(adapterId,uriPtr,*childList);

	CleanupStack::PopAndDestroy(3); //uri, luid, childList
	iServer.DbHandler()->WriteMappingInfoToDbL();	

    aMessage.Complete(KErrNone);
		
	_DBG_FILE("CNSmlDmCallbackSession::UpdateMappingInfoL(): end");
	}



// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::DeleteAclL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::DeleteAclL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::DeleteAclL(): begin");

	
    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

	TInt ret = iServer.DbHandler()->DeleteAclL(uriPtr);

	CleanupStack::PopAndDestroy(); //uri
	iServer.DbHandler()->WriteAclInfoToDbL();	

    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::DeleteAclL(): end");
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::UpdateAclL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::UpdateAclL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::UpdateAclL(): begin");

    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);
    
    HBufC8* acl = HBufC8::NewLC( aMessage.GetDesLengthL(2) );
    TPtr8 aclPtr = acl->Des();
    aMessage.ReadL( 2, aclPtr);
    
    TInt ret(KErrNone);

    if(iAtomic)
    	{
    	iAtomicAcls.AppendL(acl);
		CleanupStack::Pop(); //acl
    	
    	iAtomicAclURIs.AppendL(uri);
		CleanupStack::Pop(); //uri
    	}
    else
    	{
		ret = iServer.DbHandler()->UpdateAclL(uriPtr,aclPtr);
		CleanupStack::PopAndDestroy(2); //uri, acl
		iServer.DbHandler()->WriteAclInfoToDbL();	
    	}

    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::UpdateAclL(): end");
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::GetAclLengthL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::GetAclLengthL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::GetAclLengthL(): begin");
	
	delete iACLUri;
	iACLUri = 0;

	delete iACL;
	iACL = 0;

    iACLUri = HBufC8::NewL( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = iACLUri->Des();
    aMessage.ReadL( 1, uriPtr);
    
    iACL = CBufFlat::NewL(16);
    
	TInt ret = iServer.DbHandler()->GetAclL(uriPtr,*iACL,EFalse);
	
    TPckgBuf<TInt> len(iACL->Size());
    aMessage.WriteL(3, len, 0);
    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::GetAclLengthL(): end");
	}



// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::GetAclL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::GetAclL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::GetAclL(): begin");
	
	if(iACLUri)
		{
	    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
	    TPtr8 uriPtr = uri->Des();
	    aMessage.ReadL( 1, uriPtr);
    		
		if(iACLUri->Compare(uriPtr)==0&&iACL)
			{
			TPtr8 bufPtr=iACL->Ptr(0);
		    aMessage.WriteL( 3, bufPtr);
		    aMessage.Complete(KErrNone);
			}
		else
			{
		    aMessage.Complete(KErrNotFound);
			}
		
		CleanupStack::PopAndDestroy(); //uri
		
		}
	else
		{
	    aMessage.Complete(KErrNotFound);
		}

	delete iACLUri;
	iACLUri = 0;

	delete iACL;
	iACL = 0;
		
	_DBG_FILE("CNSmlDmCallbackSession::GetAclL(): end");
	}



// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::CheckAclL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::CheckAclL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::CheckAclL(): begin");

   TInt count =0;
    TRAPD(err, count = aMessage.GetDesLengthL(1));
    if(err == KErrNone)
    {
    HBufC8* uri = HBufC8::NewLC(count);
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

	TNSmlDmCmdType cmdType = (TNSmlDmCmdType)aMessage.Int2();

	TBool ret = iServer.DbHandler()->CheckAclL(uriPtr,cmdType);

	CleanupStack::PopAndDestroy(); //uri

    TPckgBuf<TBool> retVal(ret);
    aMessage.WriteL(3, retVal, 0);

    aMessage.Complete(KErrNone);
    }
    else
    {
    	User::Leave(err);
    }
		
	_DBG_FILE("CNSmlDmCallbackSession::CheckAclL(): end");
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::DefaultAclsToServerL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::DefaultAclsToServerL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::DefaultAclsToServerL(): begin");
    TInt count =0;  
     TRAPD(err, count = aMessage.GetDesLengthL(1));
   if(err== KErrNone&& count >0)
   {   
    HBufC8* uri = HBufC8::NewLC( count );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

	TInt ret = iServer.DbHandler()->DefaultACLsToServerL(uriPtr);

	CleanupStack::PopAndDestroy(); //uri
	iServer.DbHandler()->WriteAclInfoToDbL();	

    aMessage.Complete(ret);
 }
  else
    {
    	User::Leave(err);
    }
	_DBG_FILE("CNSmlDmCallbackSession::DefaultAclsToServerL(): end");
	}

// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::DefaultAclsToChildrenL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::DefaultAclsToChildrenL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::DefaultAclsToChildrenL(): begin");
    
    TInt ret(KErrNone);

    HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 uriPtr = uri->Des();
    aMessage.ReadL( 1, uriPtr);

    HBufC8* childList = HBufC8::NewLC( aMessage.GetDesLengthL(2) );
    TPtr8 childListPtr = childList->Des();
    aMessage.ReadL( 2, childListPtr);
    
 	RPointerArray<HBufC8> currentUris;
	CleanupStack::PushL ( PtrArrCleanupItemRArr ( HBufC8, &currentUris ) );
	
	TInt startPos(0);
	TInt segEnds(0);

  	while ( startPos < ( childListPtr.Length()  ) )
		{
		TPtrC8 tmpDesc = childListPtr.Right(childListPtr.Length()-startPos);
		segEnds = tmpDesc.Find ( KNSmlDmLitSeparator );
		if ( segEnds == KErrNotFound )
		    {
			segEnds = tmpDesc.Length();
		    }
		
		HBufC8* childUri = HBufC8::NewL ( uriPtr.Length() +
			KNSmlDmLitSeparator().Length() + segEnds );
			
		TPtr8 childUriPtr = childUri->Des();
		childUriPtr.Append ( uriPtr );
		childUriPtr.Append ( KNSmlDmLitSeparator );
		childUriPtr.Append ( tmpDesc.Left ( segEnds ) );	
		TRAPD(err, currentUris.AppendL ( childUri ));
		if(err != KErrNone)
		    {
		    delete childUri;
		    ret = err;
		    CleanupStack::PopAndDestroy(3);
		    aMessage.Complete(ret);
		    return;
		    }
		startPos += segEnds + 1;
		}
	TBool aclUpdated(EFalse);
	for(TInt i = 0;i<currentUris.Count();i++)
	    {
	    CBufBase* acl= CBufFlat::NewL(8);
	    CleanupStack::PushL(acl);
	    iServer.DbHandler()->GetAclL(*currentUris[i],*acl,EFalse);
	    if(acl->Ptr(0).Length()==0)
	        {
	        acl->Reset();
       	    iServer.DbHandler()->GetAclL(*currentUris[i],*acl);
       	    if(acl->Ptr(0).Compare(KNSmlDmAclDefaultRoot)==0)
       	        {
    	        iServer.DbHandler()->UpdateAclL(*currentUris[i],KNSmlDmAclDefault);
    	        aclUpdated=ETrue;
       	        }
	        }
    	CleanupStack::PopAndDestroy(acl);
	    }

	CleanupStack::PopAndDestroy(3); //currentUris,childList,uri 
    if(aclUpdated)
        {
    	iServer.DbHandler()->WriteAclInfoToDbL();	
        }

    aMessage.Complete(ret);
	_DBG_FILE("CNSmlDmCallbackSession::DefaultAclsToChildrenL(): end");
	}




// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::EraseServerIdL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::EraseServerIdL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::EraseServerId(): begin");

	HBufC8* serverId = HBufC8::NewLC( aMessage.GetDesLengthL(1) );
    TPtr8 serverPtr = serverId->Des();
    aMessage.ReadL( 1, serverPtr);

	TInt ret = iServer.DbHandler()->EraseServerIdL(serverPtr);

	CleanupStack::PopAndDestroy(); //serverId
	iServer.DbHandler()->WriteAclInfoToDbL();	

    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::EraseServerIdL(): end");
	}


// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::StartAtomic
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::StartAtomic( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::StartAtomic(): begin");
	
	iAtomic = ETrue;
    aMessage.Complete(KErrNone);
		
	_DBG_FILE("CNSmlDmCallbackSession::StartAtomic(): end");
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::CommitAtomicL
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::CommitAtomicL( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::CommitAtomicL(): begin");
	TInt ret(KErrNone);
	if (iAtomicAclURIs.Count()!=iAtomicAcls.Count())
		{
		ret = KErrGeneral;
		}
	else
		{
		for(TInt i = 0;i<iAtomicAclURIs.Count();i++)
			{
			TInt err=iServer.DbHandler()->UpdateAclL(*iAtomicAclURIs[i],*iAtomicAcls[i]);
			if(err!=KErrNone)
				{
				ret = err;
				}
			}
		}
		
	iAtomic = EFalse;
	iAtomicAclURIs.ResetAndDestroy();
	iAtomicAcls.ResetAndDestroy();
    aMessage.Complete(ret);
		
	_DBG_FILE("CNSmlDmCallbackSession::CommitAtomicL(): end");
	}
	
// ---------------------------------------------------------------------------
// CNSmlDmCallbackSession::RollbackAtomic
// ---------------------------------------------------------------------------
void CNSmlDmCallbackSession::RollbackAtomic( const RMessage2& aMessage )
	{
	_DBG_FILE("CNSmlDmCallbackSession::RollbackAtomic(): begin");
	
	iAtomic = EFalse;
	iAtomicAclURIs.ResetAndDestroy();
	iAtomicAcls.ResetAndDestroy();
    aMessage.Complete(KErrNone);
		
	_DBG_FILE("CNSmlDmCallbackSession::RollbackAtomic(): end");
	}
	

