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
* Description:  header file for dm acl parser
*
*/

#ifndef __NSMLDMACLPARSER_H
#define __NSMLDMACLPARSER_H

#include <nsmldmdbhandler.h>


// ===========================================================================
// CNSmlDmACLParser
// ===========================================================================
class CNSmlAclElement;

/**
* CNSmlDmACLParser is used for parsing and generating acl strings from
* structures and vice versa
*
*  @since
*/
class CNSmlDmACLParser : public CBase
	{
	public:

	/**
	* Two-phased constructor
	* @return						Pointer to newly created module instance
	*/
	static CNSmlDmACLParser* NewLC();

	/**
	* Destructor
	*/
	~CNSmlDmACLParser();
	
	/**
	* Parses acl to structure
	* @param	aACL		Acl in descriptor
	* @return				KErrNone if succesfully parsed
	*/
	TInt ParseL(const TDesC8& aACL);

	/**
	* Generates ACL from structure
	* @return				Generated acl
	*/
	HBufC8* GenerateL();

	/**
	* Removes all the references to server from acls
	* @param	aServerId	Server id to be removed from acls
	*/
	void RemoveAllReferences(const TDesC8& aServerId);
	
	/**
	* Check if current acl can be deleted. The Acl can be deleted if it is
	* empty, i.e. no references to any server in any command
	* @return				ETrue if can be deleted
	*/
	TBool ShouldDelete();

	/**
	* Check if server has access rights for certain command 
	* @param	aServerId		Server id to be checked
	* @param	aCommandType	Type of command (add,...)
	* @return					ETrue if access OK
	*/
	TBool HasRights(const TDesC8& aServerId,TNSmlDmCmdType aCommandType);

	private:
	void Reset();
	CNSmlAclElement* iCommandAcls;
	};

/**
* CNSmlAclElement is container class for one command in acls
*
*  @since
*/
class CNSmlAclElement : public CBase
	{
	public:
	/**
	* Constructor
	*/
	CNSmlAclElement();
	
	/**
	* Destructor
	*/
	virtual ~CNSmlAclElement();

	/**
	* Type of command
	*/
	TNSmlDmCmdType iCommandType;

	/**
	* Array of server ids to one command
	*/
	RPointerArray<HBufC8> iServerIds;

	/**
	* True if all servers has access rigths, i.e acl value is *
	*/
	TBool iAllServers;

	/**
	* Pointer to next element
	*/
	CNSmlAclElement* iNext;
	};



#endif // __NSMLDMACLPARSER_H