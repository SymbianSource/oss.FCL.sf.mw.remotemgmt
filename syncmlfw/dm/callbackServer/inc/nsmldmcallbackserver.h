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
* Description:  DM callback server header file
*
*/



#ifndef __NSMLDMCALLBACKSERVER_H__
#define __NSMLDMCALLBACKSERVER_H__

// ---------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------
#include <e32base.h>
#include <badesca.h>
#include <e32property.h>

#include "nsmldmcallbackserverdefs.h"

// ---------------------------------------------------------------------------
// Panic reasons
// ---------------------------------------------------------------------------

enum TNSmlDmCallbackServerPanic
    {
    ECleanupCreateError,
    EMainSchedulerError
    };

// ---------------------------------------------------------------------------
// Class forwards
// ---------------------------------------------------------------------------
class CNSmlDmDbHandler;
struct TSmlDmMappingInfo;

// ---------------------------------------------------------------------------
// class CNSmlDmCallbackServer
// ---------------------------------------------------------------------------
/**
* CNSmlDmCallbackServer is a Symbian OS server which owns the tree db handler
* i.e. the tree db can be accessed only via IPC through this server.
*
*  @since
*/
class CNSmlDmCallbackServer : public CServer2
	{
public:

	/**
	* Panics the server
	* @param	aPanic			Panic code
	*/
    static void PanicServer( TNSmlDmCallbackServerPanic aPanic );

	/**
	* Launches server thread
	* @param	aStarted		Semaphore which is signallede when server
	*							is started
	*/
    static TInt ThreadFunction( TAny* aStarted );

	/**
	* Increase session count
	*/
    void Increase();

	/**
	* Decrease session count
	*/
    void Decrease();

	/**
	* Return session count
	* @return			Session count
	*/
	TInt Count();
	
	/**
	* Return pointer to db handler class
	* @return			Pointer to db handler
	*/
	CNSmlDmDbHandler* DbHandler();
	
	/**
	* Two-phased constructor
	* @return						Pointer to newly created module instance
	*/
    static CNSmlDmCallbackServer* NewL();

	/**
	* Destructor
	*/
	virtual ~CNSmlDmCallbackServer();
   
public:
 	/**
	* Creates new session
	* @param	aVersion		Version for checking
	* @param	aMessage		IPC message
	*/
    CSession2* NewSessionL( const TVersion& aVersion,
    	const RMessage2& aMessage ) const;


private:    
	/**
	* Constructor
	*/
	CNSmlDmCallbackServer();

	/**
	* Second phase constructor
	*/
    void ConstructL();
    

private:
	/**
	* Pointer to Dbhandler class
	*/
    CNSmlDmDbHandler* iDbHandler;

	/**
	* Session count
	*/
	TInt iCount;
	};

// ---------------------------------------------------------------------------
// class CNSmlDmCallbackSession 
// ---------------------------------------------------------------------------
/**
* CNSmlDmCallbackSession is a Symbian OS server side session class which
* handles the messages from one client
*
*  @since
*/
class CNSmlDmCallbackSession : public CSession2
	{
public:
	/**
	* Two-phased constructor
	* @param	aServer			Pointer to server (owner)
	* @return					Pointer to newly created module instance
	*/
	static CNSmlDmCallbackSession* NewL( CNSmlDmCallbackServer& aServer);

    /**
    Handles the servicing of a client request that has been passed
    to the server.

	@param aMessage The message containing the details of the client request.
    */
	void ServiceL( const RMessage2 &aMessage );

private:
	
	/**
	* Called and TRAPped in ServiceL function. Directed to correct opertaion
	* by aMessage.Function() information

	@param aMessage		The message containing the details of the client
	*					request.
    */
    void DispatchMessageL( const RMessage2 &aMessage );
	    
	/**
	* SetServer IPC called. Sets the current server id into to db handler

	@param aMessage		The message containing the details of the client
	*					request.
    */
    void SetServerL( const RMessage2& aMessage );

	/**
	* Get Luid IPC called.Gets luid from Db handler

	@param aMessage		The message containing the details of the client
	*					request.
    */
    void GetLuidL( const RMessage2& aMessage );

	/**
	* Get Uri segment list IPC called. Gives list to the client

	@param aMessage		The message containing the details of the client
	*					request.
    */
    void GetUriSegListL( const RMessage2& aMessage );

	/**
	* Close connection IPC called. Closes connection and decreases session
	* count. If Count is 0, server terminates itself.

	@param aMessage		The message containing the details of the client
	*					request.
    */
    void CloseConnectionL( const RMessage2& aMessage );

	/**
	* Generate luid IPC called. Gets the luid from db handler and return
	* the length to client

	@param aMessage		The message containing the details of the client
	*request.
    */
    void GenerateLuidL(const RMessage2& aMessage );

	/**
	* Generate luidlist IPC called. Gets the list from db handler and makes
	* it flat adn returns the length to client

	@param aMessage		The message containing the details of the client
	*request.
    */
    void GenerateUriSegListL(const RMessage2& aMessage );

	/**
	* Add mapping IPC called. Adds mappping by calling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void AddMappingInfoL( const RMessage2& aMessage );

	/**
	* Remove mapping IPC called. Removes mapping by alling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void RemoveMappingInfoL( const RMessage2& aMessage );

	/**
	* Rename mapping IPC called. Renames mapping by calling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void RenameMappingInfoL( const RMessage2& aMessage );

	/**
	* Update mapping IPC called. Updates mapping by calling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void UpdateMappingInfoL(const RMessage2& aMessage );

	/**
	* Delete ACL IPC called. Deletes correct ACL info by calling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void DeleteAclL(const RMessage2& aMessage );

	/**
	* Update ACL IPC called. Updates correct ACL info by calling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void UpdateAclL(const RMessage2& aMessage );

	/**
	* Get ACL length IPC called. Gets correct ACL info by calling db handler
	* and return the length to client

	@param aMessage		The message containing the details of the client
	*request.
    */
    void GetAclLengthL(const RMessage2& aMessage );

	/**
	* Get ACL IPC called. Returns the correct ACL info to client

	@param aMessage		The message containing the details of the client
	*request.
    */
    void GetAclL(const RMessage2& aMessage );

	/**
	* Check ACL IPC called. Checks the asked ACL rights

	@param aMessage		The message containing the details of the client
	*request.
    */
    void CheckAclL(const RMessage2& aMessage );

	/**
	* Default ACLs to server IPC called. Sets the deafult acls to server
	* by calling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void DefaultAclsToServerL(const RMessage2& aMessage );


	/**
	* Default ACLs to children IPC called. Sets the deafult acls to children
	* which does not contain the earlier set acls.

	@param aMessage		The message containing the details of the client
	*request.
    */
  	void DefaultAclsToChildrenL(const RMessage2& aMessage);

	/**
	* Erase server id IPC called. Erases references to current server id
	* by calling db handler

	@param aMessage		The message containing the details of the client
	*request.
    */
    void EraseServerIdL(const RMessage2& aMessage );

	/**
	* Atomic start indication
	@param aMessage		The message containing the details of the client
	*request.
    */
    void StartAtomic(const RMessage2& aMessage );
    
	/**
	* Commands inside the atomic committed
	@param aMessage		The message containing the details of the client
	*request.
    */
    void CommitAtomicL(const RMessage2& aMessage );

	/**
	* Commads inside atomic rolled back
	@param aMessage		The message containing the details of the client
	*request.
    */
    void RollbackAtomic(const RMessage2& aMessage );

private:

	/**
	* Second phase constructor
    */
	void ConstructL();

	/**
	* Constructor
    */
	CNSmlDmCallbackSession( CNSmlDmCallbackServer& aServer);

	/**
	* Destructor
    */
	~CNSmlDmCallbackSession();
	
private:
	/**
	* Pointer to server
    */
    CNSmlDmCallbackServer& iServer;

	/**
	* Flat segment list
    */
    CBufBase *iSegList;

	/**
	* Luid in case of getting luid
    */
    HBufC8* iLuid;

	/**
	* ACL info in case of getting ACL
    */
    CBufBase* iACL;

	/**
	* ACL URI in case of getting ACL
    */
    HBufC8* iACLUri;

	/**
	* Uri segment list in list format
    */
    CArrayFixFlat <TSmlDmMappingInfo>* iUriSegList;
    
	/**
	* List of acl uris in case of atomic
    */
   	RPointerArray<HBufC8> iAtomicAclURIs;

	/**
	* List of acl values in case of atomic
    */
	RPointerArray<HBufC8> iAtomicAcls;
	
	/**
	* True when atomic operation is going on
    */
	TBool iAtomic;
	};


TInt LaunchServer();

#endif // __NSMLDMCALLBACKSERVER_H__
