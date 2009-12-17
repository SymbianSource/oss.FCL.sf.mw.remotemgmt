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
* Description:  header file for dm tree db client
*
*/


#ifndef __NSMLDMTREEDBCLIENT_H__
#define __NSMLDMTREEDBCLIENT_H__

// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include <e32base.h>
#include <smldmadapter.h>

enum TNSmlDmCmdType 
	{
	EAclAdd = 0,
	EAclReplace,	
	EAclDelete,
	EAclGet,
	EAclExecute,
	EAclCopy
	};

enum TEnforcedServer
    {
    ENonEnforcedServer = 0,
    EEnforcedServer    
    };
// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

// FORWARD DECLARATIONS
class CNSmlDmMgmtTree; //tarm

// ===========================================================================
// RNSmlDMCallbackSession
// ===========================================================================
/**
* RNSmlDMCallbackSession is Symbian OS client session for connecting to
* DM callback server which handles the tree database. The tree database can be
* used via this client interface.
*
*  @since
*/
class RNSmlDMCallbackSession : public RSessionBase
	{
public:

	/**
	* Constructor
	*/
	IMPORT_C RNSmlDMCallbackSession();

	/**
	* Desstructor
	*/
	IMPORT_C ~RNSmlDMCallbackSession();

	/**
	* Connects to server

	* @return	Success of opertaion, system wide error codes
	*/
	IMPORT_C TInt Connect();

	/**
	* Not used at the moment
	*/
	IMPORT_C TInt ResourceCount();
	
	/**
	* Close the session
	*/
	IMPORT_C void Close();

	/**
	* Returns version
	* @return	Version
	*/
	TVersion Version(void) const;
	
	/**
	* Gets luid from server. Two phase operation, asks first the
	* length, then data 
	* @param	aAdapterId	Id of plug-in adapter
	* @param	aURI		URI
	* @return				Mapped luid
	*/
	IMPORT_C HBufC8* GetLuidAllocL(TUint32 aAdapterId,const TDesC8& aURI);
	
	/**
	* Gets uri seg list from server. Two phase operation, asks first the
	* data length, then data itself 
	* @param	aAdapterId	Id of plug-in adapter
	* @param	aURI		URI
	* @param	aURISegList	Reference to list where to fill data
	*/
	IMPORT_C void GetUriSegListL(TUint32 aAdapterId,
		const TDesC8& aURI,
		CArrayFixFlat<TSmlDmMappingInfo>& aURISegList);

	/**
	* Sets server id of current session
	* @param	aServerId	Server identifier		
	*/
	IMPORT_C void SetServerL ( const TDesC8& aServerId );
	
	/**
	* Adds new mapping
	* @param	aAdapterId	Id of plug-in adapter
	* @param	aURI		URI
	* @param	aLuid		Luid to be mapped to URI
	* @return				Success of operation
	*/
	IMPORT_C TInt AddMappingInfoL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aLuid );
		
	/**
	* Remove mapping
	* @param	aAdapterId	Id of plug-in adapter
	* @param	aURI		URI
	* @param	aChildAlso	If true, removes also children
	* @return				Success of operation
	*/
	IMPORT_C TInt RemoveMappingInfoL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		TBool aChildAlso = ETrue );
		
	/**
	* Rename mapping
	* @param	aAdapterId	Id of plug-in adapter
	* @param	aURI		URI
	* @param	aObject		New name (last uri segment)
	* @return				Success of operation
	*/
	IMPORT_C TInt RenameMappingL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aObject );
	

	/**
	* Update mappings (clear database in case of getting childlist)
	* @param	aAdapterId	Id of plug-in adapter
	* @param	aURI		URI
	* @param	aCurrentList Child list from adapter
	*/
	IMPORT_C void UpdateMappingInfoL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		CBufBase& aCurrentList );

	/**
	* Delete acl pointed by URI
	* @param	aURI		URI
	* @return				Success of operation
	*/
	IMPORT_C TInt DeleteAclL(const TDesC8& aURI);

	/**
	* Update acl pointed by URI
	* @param	aURI		URI
	* @param	aACL		New ACL info
	* @return				Success of operation
	*/
	IMPORT_C TInt UpdateAclL(const TDesC8& aURI,const TDesC8& aACL);
	
	/**
	* Get acl pointed by URI. Two phase operation, the length is get first,
	* then the acl data itself
	* @param	aURI		URI
	* @param	aACL		Buffer for writing the result ACL 
	* @param	aInherited	If true, the inherited value is searhed
	* @return				Success of operation
	*/
	IMPORT_C TInt GetAclL(const TDesC8& aURI,
		CBufBase& aACL,
		TBool aInherited = ETrue);
		
	/**
	* Check acl to current command.
	* @param	aURI		URI
	* @param	aCmdType	Type of command to be checked
	* @return				ETrue if access ok
	*/
	IMPORT_C TBool CheckAclL(const TDesC8& aURI, TNSmlDmCmdType aCmdType);

//tarm start
	/** 
	* Check ACL rights, default value is used if acl is not available
	* @note This function (as well as RNSmlDMCallbackSession::CheckAclL)
    * will not specify when the URI does not point into an _existing_ node.
    * Then it returns the same code when access to URI is denied.
    *
	* @param	aURI			Target URI in command
	* @param	aCmdType		Command type
	*							or not
	* @param    aMgmtTree       Management tree, a structure that holds
	*                           current and valid ddf information
	* @return					True if command is allowed by acls
	*/
	IMPORT_C TBool CheckDynamicAclL(
    	const TDesC8& aURI,
        TNSmlDmCmdType aCmdType,
        CNSmlDmMgmtTree *aMgmtTree);
//tarm end

	/**
	* Set default acls to current server
	* @param	aURI		URI
	* @return				Success of operation
	*/
	IMPORT_C TInt DefaultACLsToServerL(const TDesC8& aURI);

	/**
	* Set default acls to children which does not have earlier set ACLs
	* @param	aURI		URI
	* @param	aCurrentList	List of children
	* @return				Success of operation
	*/
	IMPORT_C TInt DefaultACLsToChildrenL(const TDesC8& aURI,
		CBufBase& aCurrentList );

	/**
	* Erases references to server from acls
	* @param	aServerId	Server name to be erased
	* @return				Success of operation
	*/
	IMPORT_C TInt EraseServerIdL(const TDesC8& aServerId);

	/**
	* Indicates atomic start
	* @return				Success of operation
	*/
	IMPORT_C TInt StartAtomic();

	/**
	* Commits atomic
	* @return				Success of operation
	*/
	IMPORT_C TInt CommitAtomic();

	/**
	* Rolls back the commansd inside atomic
	* @return				Success of operation
	*/
	IMPORT_C TInt RollbackAtomic();
	
	
private:
	/**
	* Luid list when gettiing the luid list
	*/
	RPointerArray<HBufC8> iLuids;
	
	/**
	 * Memeber variable to know the enforcement check of a particular setting
	 * is happened or not	 
	 */
	TInt iEnforceCheckdone;

	};

#endif // __NSMLDMTREEDBCLIENT_H__
