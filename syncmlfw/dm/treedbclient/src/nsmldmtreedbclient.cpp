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
* Description:  source file for dm tree db client
*
*/


// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include <eikdll.h>
#include <apgcli.h>
#include <s32mem.h>

#include <nsmldmtreedbclient.h>
#include <nsmldmuri.h> //tarm
#include <SettingEnforcementInfo.h>
#include <nsmldebug.h>
#include "nsmldmcallbackserver.h"
#include "nsmldmcallbackserverdefs.h"
#include "nsmldmtreedtd.h" //tarm

//tarm start
_LIT8( KNSmlDmDefaultACLForDataWithoutServer, "Add=*&Get=*&Replace=*&Delete=*&Exec=*");
//tarm end

// -----------------------------------------------------------------------------
// LaunchServerL()
// Launches the server.
// -----------------------------------------------------------------------------
//
TInt LaunchServerL()
	{	
	TInt result = KErrNone;
	
	RFs fs;
	User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
	TFindFile findFile( fs );
		
	RProcess server;
	
	User::LeaveIfError( server.Create( KNSmlDMCallbackExeName, KNullDesC ) );
	
	CleanupStack::PopAndDestroy(); // fs
	
	TRequestStatus status;
	server.Rendezvous( status );

	if ( status != KRequestPending )
		{
		server.Kill( 0 );	// abort startup
		server.Close();
		return KErrGeneral;	// status can be KErrNone: don't return status.Int()
		}
	else
		{
		server.Resume();	// logon OK - start the server
		}
		
	User::WaitForRequest( status );
	
	result = status.Int();
	
	if ( status == KErrNone )
		{
		server.Close();
		}

	return result;
	}
	
// ---------------------------------------------------------------------------
// RNSmlDMCallbackSession::RNSmlDMCallbackSession()
// ---------------------------------------------------------------------------
EXPORT_C RNSmlDMCallbackSession::RNSmlDMCallbackSession()
	{
	iEnforceCheckdone = 0;
	}

// ---------------------------------------------------------------------------
// RNSmlDMCallbackSession::~RNSmlDMCallbackSession()
// ---------------------------------------------------------------------------
EXPORT_C RNSmlDMCallbackSession::~RNSmlDMCallbackSession()
	{
	iLuids.ResetAndDestroy();
	iLuids.Close();
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::Connect()
// Connect to the  server
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::Connect()
	{
	TInt result(KErrNone);
	
	TVersion ver ( KNSmlDmCallbackServerVersionMajor,
		KNSmlDmCallbackServerVersionMinor, 0  );
		
	result = CreateSession( KNSmlDmCallbackServerName, ver );
	if(result!=KErrNone)
		{
		TRAPD(error, result = LaunchServerL());
		if ( (error == KErrNone) && ( result == KErrNone ) )
			{
			result = CreateSession( KNSmlDmCallbackServerName, ver );
			}
		}
	return result;
	}
	

// ---------------------------------------------------------------------------
// TVersion RNSmlDMCallbackSession::Version()
// ---------------------------------------------------------------------------
TVersion RNSmlDMCallbackSession::Version(void) const
	{
	return(TVersion(KNSmlDmCallbackServerVersionMajor,
		KNSmlDmCallbackServerVersionMinor,0));
	}


// ---------------------------------------------------------------------------
// void RNSmlDMCallbackSession::Close()
// ---------------------------------------------------------------------------
EXPORT_C void RNSmlDMCallbackSession::Close()
	{
	SendReceive(ECmdDMCallbackCloseSession,TIpcArgs());
	RHandleBase::Close();
	}


// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::ResourceCount()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::ResourceCount()
	{
	//previously not initialised
	TInt count=0;
	TPckgBuf<TInt> pckg(count);
	SendReceive(ECmdDMCallbackResourceCount,TIpcArgs(&pckg));
	return pckg();
	}

// ---------------------------------------------------------------------------
// void RNSmlDMCallbackSession::SetServerL()
// ---------------------------------------------------------------------------
EXPORT_C void RNSmlDMCallbackSession::SetServerL(const TDesC8& aServer)
	{
	HBufC8* serverId = aServer.AllocLC();
	TPtr8 serverPtr = serverId->Des();
	TIpcArgs args(0,&serverPtr);
	
	User::LeaveIfError(SendReceive(ECmdDMCallbackSetServer,args));
	CleanupStack::PopAndDestroy(); //serverId
	}


// ---------------------------------------------------------------------------
// HBufC8* RNSmlDMCallbackSession::GetLuidAllocL()
// ---------------------------------------------------------------------------
EXPORT_C HBufC8* RNSmlDMCallbackSession::GetLuidAllocL(
	TUint32 aAdapterId,const TDesC8& aURI)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	TPckgBuf<TUint32> len;
	HBufC8* data=NULL;
	TIpcArgs args(aAdapterId,&uriPtr,0,&len);
	
	if(SendReceive(ECmdDMCallbackGetLuidLength,args)==KErrNone)
		{
		if(len())
			{
			data = HBufC8::NewLC(len());		
			TPtr8 dataPtr = data->Des();
			TIpcArgs args2(0,0,0,&dataPtr);
			
			SendReceive(ECmdDMCallbackGetLuid,args2);
			CleanupStack::Pop(); //data
			}
		else
			{
			data=KNullDesC8().AllocL();
			}
		}
	else
		{
		data = KNullDesC8().AllocL();
		}
	CleanupStack::PopAndDestroy(); //uri
	
	return data;
	}


// ---------------------------------------------------------------------------
// void RNSmlDMCallbackSession::GetUriSegListL()
// ---------------------------------------------------------------------------
EXPORT_C void RNSmlDMCallbackSession::GetUriSegListL(TUint32 aAdapterId,
	const TDesC8& aURI, CArrayFixFlat<TSmlDmMappingInfo>& aURISegList)
	{
	iLuids.ResetAndDestroy();
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	TPckgBuf<TUint32> len;
	HBufC8* data=NULL;
	TIpcArgs args(aAdapterId,&uriPtr,0,&len);
	
	if(SendReceive(ECmdDMCallbackGetSegmentListLength,args)==KErrNone)
		{
		if(len())
			{
			data = HBufC8::NewLC(len());
			TPtr8 dataPtr = data->Des();
			TIpcArgs args2(0,0,0,&dataPtr);
			
			SendReceive(ECmdDMCallbackGetSegmentList,args2);
			RDesReadStream readStream(dataPtr);
			CleanupClosePushL(readStream);

			TUint16 count = readStream.ReadUint16L();
			TUint8 length8;
			
			for(TInt i=0;i<count;i++)
				{
				length8 = readStream.ReadUint8L();
				HBufC8 *uriSeg =HBufC8::NewLC(length8);
				TPtr8 uriSegPtr = uriSeg->Des();
				readStream.ReadL(uriSegPtr,length8);

				length8 = readStream.ReadUint8L();
				HBufC8 *luid =HBufC8::NewLC(length8);
				TPtr8 luidPtr = luid->Des();
				readStream.ReadL(luidPtr,length8);
				iLuids.AppendL(luid);
				CleanupStack::Pop(); //luid

				TSmlDmMappingInfo mapInfo;
				mapInfo.iURISeg = TBufC8<KSmlMaxURISegLen>(uriSegPtr);
				mapInfo.iURISegLUID.Set(*luid);
				aURISegList.AppendL(mapInfo);
				CleanupStack::PopAndDestroy(); //uriSeg
				}
			CleanupStack::PopAndDestroy(2); //readStream,data
			}
		}
	CleanupStack::PopAndDestroy(); //uri
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::AddMappingInfoL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::AddMappingInfoL(
	TUint32 aAdapterId, const TDesC8& aURI, const TDesC8& aLuid )
	{

	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	HBufC8* luid = aLuid.AllocLC();
	TPtr8 luidPtr = luid->Des();

	TIpcArgs args(aAdapterId,&uriPtr,&luidPtr);
	
	TInt ret = SendReceive(ECmdDMCallbackAddMappingInfo,args);
	CleanupStack::PopAndDestroy(2); //uri, luid
    User::LeaveIfError(ret);
	return ret;
	}
	
// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::RemoveMappingInfoL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::RemoveMappingInfoL(
	TUint32 aAdapterId, const TDesC8& aURI, TBool aChildAlso)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();

	TIpcArgs args(aAdapterId,&uriPtr,aChildAlso);
	
	TInt ret = SendReceive(ECmdDMCallbackRemoveMappingInfo,args);
	CleanupStack::PopAndDestroy(); //uri
    User::LeaveIfError(ret);
	return ret;
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::RenameMappingL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::RenameMappingL(
	TUint32 aAdapterId, const TDesC8& aURI, const TDesC8& aObject )
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	HBufC8* newName = aObject.AllocLC();
	TPtr8 newNamePtr = newName->Des();

	TIpcArgs args(aAdapterId,&uriPtr,&newNamePtr);
	
	TInt ret = SendReceive(ECmdDMCallbackRenameMapping,args);
	CleanupStack::PopAndDestroy(2); //uri,newName
    User::LeaveIfError(ret);
	return ret;
	}
	

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::UpdateMappingInfoL()
// ---------------------------------------------------------------------------
EXPORT_C void RNSmlDMCallbackSession::UpdateMappingInfoL(
	TUint32 aAdapterId, const TDesC8& aURI, CBufBase& aCurrentList)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	TPtr8 currentListPtr = aCurrentList.Ptr(0);

	TIpcArgs args(aAdapterId,&uriPtr,&currentListPtr);
	
	TInt ret = SendReceive(ECmdDMCallbackUpdateMapping,args);
    User::LeaveIfError(ret);
	CleanupStack::PopAndDestroy(); //uri
	}
	
	

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::DeleteAclL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::DeleteAclL(const TDesC8& aURI)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();

	TIpcArgs args(0,&uriPtr);
	
	TInt ret = SendReceive(ECmdDMCallbackDeleteAcl,args);
	CleanupStack::PopAndDestroy(); //uri
    User::LeaveIfError(ret);
	return ret;
	}
	

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::UpdateAclL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::UpdateAclL(
	const TDesC8& aURI,const TDesC8& aACL)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	HBufC8* acl = aACL.AllocLC();
	TPtr8 aclPtr = acl->Des();

	TIpcArgs args(0,&uriPtr,&aclPtr);
	
	TInt ret = SendReceive(ECmdDMCallbackUpdateAcl,args);
	CleanupStack::PopAndDestroy(2); //uri,acl
    User::LeaveIfError(ret);
	return ret;
	}
	

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::GetAclL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::GetAclL(const TDesC8& aURI,
	CBufBase& aACL,TBool /*aInherited*/)
    {    
    //If enforced pass parent uri to server and get that ACL    
    TInt enforced=KErrNotFound;    
    //connect to policy engine
    CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
    CleanupStack::PushL(info);        
    info->EnforcementActiveL(aURI, enforced);
    DBG_FILE_CODE(enforced,_S8("GetAclL tarm enforcement is"));    
    CleanupStack::PopAndDestroy(info);

    //In enforced case this method will be visited by trusted server only
    if( EEnforcedServer == enforced )
        {
        TPtrC8 parent;
        TInt foundpos = aURI.Locate('/');
        if(foundpos > 0)
            {
            parent.Set(aURI.Left(foundpos));
            }
        else
            {
            parent.Set(aURI);
            }
        HBufC8* uri = parent.AllocLC();
        TPtr8 uriPtr = uri->Des();
        TPckgBuf<TUint32> len;
        TIpcArgs args(0,&uriPtr,0,&len);
        TInt ret(KErrNotFound);
        if(SendReceive(ECmdDMCallbackGetAclLength,args)==KErrNone)
            {
            HBufC8* acl = HBufC8::NewLC(len());
            TPtr8 aclPtr = acl->Des();
            TIpcArgs args2(0,&uriPtr,0,&aclPtr);
            ret = SendReceive(ECmdDMCallbackGetAcl,args2);
            aACL.InsertL(0,aclPtr);
            CleanupStack::PopAndDestroy(); //acl
            }
        CleanupStack::PopAndDestroy(); //uri
        return ret;	
        }
    else
        {
        HBufC8* uri = aURI.AllocLC();
        TPtr8 uriPtr = uri->Des();
        TPckgBuf<TUint32> len;
        TIpcArgs args(0,&uriPtr,0,&len);
        TInt ret(KErrNotFound);
        if(SendReceive(ECmdDMCallbackGetAclLength,args)==KErrNone)
            {
            HBufC8* acl = HBufC8::NewLC(len());
            TPtr8 aclPtr = acl->Des();
            TIpcArgs args2(0,&uriPtr,0,&aclPtr);
            ret = SendReceive(ECmdDMCallbackGetAcl,args2);
            aACL.InsertL(0,aclPtr);
            CleanupStack::PopAndDestroy(); //acl
            }
        CleanupStack::PopAndDestroy(); //uri
        return ret;
        }
    }

	
// ---------------------------------------------------------------------------
// TBool RNSmlDMCallbackSession::CheckAclL()
// ---------------------------------------------------------------------------
EXPORT_C TBool RNSmlDMCallbackSession::CheckAclL(
	const TDesC8& aURI, TNSmlDmCmdType aCmdType)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	TPckgBuf<TBool> retVal;
	TBool ret(EFalse);


	TIpcArgs args(0,&uriPtr,TInt(aCmdType),&retVal);
	
	if(SendReceive(ECmdDMCallbackCheckAcl,args)==KErrNone)
		{
		ret = retVal();
		}
	CleanupStack::PopAndDestroy(); //uri
	return ret;
	}

//tarm start
// ----------------------------------------------------------------------------
// RNSmlDMCallbackSession::CheckDynamicAclL( const TDesC& aURI, TNSmlDmCmdType aCmdType)
// Does the ACL checking
// ----------------------------------------------------------------------------
EXPORT_C TBool RNSmlDMCallbackSession::CheckDynamicAclL(
        const TDesC8& aURI,
        TNSmlDmCmdType aCmdType,
        CNSmlDmMgmtTree *aMgmtTree
        )
    {
    //Check aURI enforced
    if(iEnforceCheckdone == 0)
        {
        TInt enforced=KErrNotFound;        
        //connect to policy engine
        CSettingEnforcementInfo* info = CSettingEnforcementInfo::NewL();
        CleanupStack::PushL(info);        
        info->EnforcementActiveL(aURI, enforced);
        DBG_FILE_CODE(enforced,_S8("CheckDynamicAclL tarm enforcement is"));        
        CleanupStack::PopAndDestroy(info);
        if(EEnforcedServer == enforced)
            {
            return ETrue;
            }
        else if(ENonEnforcedServer == enforced)
            {
            return EFalse;
            }
        else 
            {
            //behave as no enforcement case
            }
        }
    CBufFlat *acl = CBufFlat::NewL( 128 );
    CleanupStack::PushL( acl );
    //
    // NOTE: In here we make assumption that if acl is not defined,
    // function will return empty acl and NOT derived one.
    GetAclL( aURI, *acl, EFalse );

    TBool emptyACL = (acl->Size() == 0);
    CleanupStack::PopAndDestroy( acl );

    //
    // If we don't have ACL defined for this URI, check if default is needed
    //
    if( emptyACL )
        {
        //
        // Conditions:
        // 1. We have either dynamic node or static leaf
        // 2. All the parent nodes are static (non-dynamic)
        // 3. All the parent nodes except root node must not have ACL (?)
        //
        TBool uriExists = ETrue;
        TBool checkParents = EFalse;
        TBool nonStaticParents = EFalse;
        TBool parentHasACL = EFalse;
        CNSmlDmNodeList* nodelist = NULL;

        TNSmlDmDDFFormat format =
            aMgmtTree->FindNameFromNodeListL(aURI,nodelist);

        if( ENSmlDmDDFNotFound == format )
            {
            uriExists = EFalse;
            }
        else if( NULL == nodelist )
            {
            uriExists = EFalse;
            }
        else if( NULL == nodelist->iNode )
            {
            uriExists = EFalse;            
            }

        if( uriExists )
            {

            CNSmlDmNode *node = nodelist->iNode;

            //
            // Check that parents are static
            //
            if( ENSmlDmDDFLeaf == format )
                {
                checkParents = ETrue;
                }

            if( (ENSmlDmDDFNode == format) &&
                (node->iDFProperties.iScope == MSmlDmDDFObject::EDynamic)
                )
                {
                checkParents = ETrue;
                }

            if( checkParents )
                {
                CNSmlDmNode *parentNode = node;
                TInt ascend = 0; // Keeps track of levels we check
                TInt highestDynamic = 0;
                while( parentNode->iParent )
                    {
                    parentNode = parentNode->iParent;
                    ascend ++;

                    if( parentNode->iDFProperties.iScope == MSmlDmDDFObject::EDynamic )
                        {
                        highestDynamic = ascend;
                        nonStaticParents = ETrue;
                        // break if you use ascend in this scope
                        // instead of highestDynamic
                        //break;
                        }
                    }

                if( nonStaticParents && ascend )
                    {
                    //
                    // Continue checking... finally check the dynamic node nearest to root
                    // If it has no acl, set it.
                    // ACL will then be inherited to the URI in question.
                    //
                    TPtrC8 upperURI = aURI;
                    while( highestDynamic )
                        {
                        upperURI.Set( NSmlDmURI::RemoveLastSeg( upperURI ) );
                        highestDynamic --;
                        }

                    // NOTE: If following recursive call will result in ACL change,
                    // it will affect the return value of the current call also.
                    // NOTE: To prevent infinite loop, do not allow same URI to be used again.
                    if( upperURI.Compare(aURI) != 0 )
                        {
                        iEnforceCheckdone++;
                        if(CheckDynamicAclL( upperURI, aCmdType, aMgmtTree ))
                        	{                        
                        		_DBG_FILE("RNSmlDMCallbackSession::CheckDynamicAclL() : returns true");
                        	}
                        }
                    }
                else
                    {
                    //
                    // Check that parents have no ACL
                    //
                    TPtrC8 parentURI = NSmlDmURI::RemoveLastSeg( aURI );
                    CBufFlat *aclParent = CBufFlat::NewL( 128 );
                    CleanupStack::PushL( aclParent );
                    while( parentURI.Length() && (parentURI.Compare( _L8(".") ) != 0) )
                        {
                        // NOTE: In here we make assumption that if acl is not defined,
                        // function will return empty acl and NOT derived one.
                        GetAclL( parentURI, *aclParent, EFalse );

                        if( aclParent->Size() != 0 )
                            {
                            parentHasACL = ETrue;
                            break;                    
                            }

                        parentURI.Set( NSmlDmURI::RemoveLastSeg( parentURI ) );
                        }
                    CleanupStack::PopAndDestroy( aclParent );

                    if( !parentHasACL )
                        {
                        //
                        // Set ACL
                        //
                        UpdateAclL( aURI, KNSmlDmDefaultACLForDataWithoutServer() );
                        }
                    }
                }
            }
        }

    // NOTE: This must be only function to use aCbSession.CheckAclL
    // If dynamically adjusted ACLs are supported
    //
    if(iEnforceCheckdone > 0)
        iEnforceCheckdone--;
    return CheckAclL( aURI, aCmdType );
    }
//tarm end

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::DefaultACLsToServerL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::DefaultACLsToServerL(
	const TDesC8& aURI)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	TIpcArgs args(0,&uriPtr);
	
	TInt ret = SendReceive(ECmdDMCallbackDefaultAclsToServer,args);
	CleanupStack::PopAndDestroy(); //uri
	return ret;
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::DefaultACLsToChildrenL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::DefaultACLsToChildrenL(
	const TDesC8& aURI,CBufBase& aChildList)
	{
	HBufC8* uri = aURI.AllocLC();
	TPtr8 uriPtr = uri->Des();
	TPtr8 childListPtr = aChildList.Ptr(0);

	TIpcArgs args(0,&uriPtr,&childListPtr);
	
	TInt ret = SendReceive(ECmdDMCallbackDefaultAclsToChildren,args);
	CleanupStack::PopAndDestroy(); //uri
    User::LeaveIfError(ret);
	return ret;
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::EraseServerIdL()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::EraseServerIdL(const TDesC8& aServerId)
	{
	HBufC8* serverId = aServerId.AllocLC();
	TPtr8 serverPtr = serverId->Des();
	TIpcArgs args(0,&serverPtr);
	
	TInt ret = SendReceive(ECmdDMCallbackEraseServerId,args);
	CleanupStack::PopAndDestroy(); //serverId
    User::LeaveIfError(ret);
	return ret;
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::StartAtomic()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::StartAtomic()
	{
	TIpcArgs args;
	
	TInt ret = SendReceive(ECmdDMCallbackStartAtomic,args);
	return ret;
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::CommitAtomic()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::CommitAtomic()
	{
	TIpcArgs args;
	
	TInt ret = SendReceive(ECmdDMCallbackCommitAtomic,args);
	return ret;
	}

// ---------------------------------------------------------------------------
// TInt RNSmlDMCallbackSession::RollbackAtomic()
// ---------------------------------------------------------------------------
EXPORT_C TInt RNSmlDMCallbackSession::RollbackAtomic()
	{
	TIpcArgs args;
	
	TInt ret = SendReceive(ECmdDMCallbackRollbackAtomic,args);
	return ret;
	}
