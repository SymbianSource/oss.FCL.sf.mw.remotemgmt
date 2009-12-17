/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/


#ifndef __ACL_STORAGE_H__
#define __ACL_STORAGE_H__

// INCLUDES


#include <e32base.h>
#include <ssl.h>
#include "DMUtilClientServer.h"

#ifdef __TARM_SYMBIAN_CONVERGENCY
 #include <dmtree.h>
#else
 #include <nsmldmmodule.h>		//Private SyncML header
 #include <nsmldmtreedbclient.h>	//Private SyncML header
#endif


// FORWARD DECLARATIONS
class CNSmlDmDbHandler;



#ifdef __TARM_SYMBIAN_CONVERGENCY
class CChildInfo : public CBase
#else
class CChildInfo : public CBase, public MNSmlDmModuleCallBack
#endif
	{
	public:

		typedef RArray<TPtrC8> RChildNodes;
		
		CChildInfo( RChildNodes& aNodes);
		~CChildInfo();

		TInt GetChildsL( const TDesC8& aURI);		

#ifdef __TARM_SYMBIAN_CONVERGENCY
 // nothing
#else
		/**
		* Function for setting result in callback interface
		* @param	aResultsRef			Reference to result element
		* @param	aObject				Data
		* @param	aType				Mime type
		* @param	aFormat				Format of the object
		* @param	aTotalSize			Total size
		*/
		virtual void SetResultsL( TInt aResultsRef,
			const CBufBase& aObject,
			const TDesC8& aType,
			const TDesC8& aFormat,
			TInt aTotalSize );
		
		/**
		* Function for setting status in callback interface
		* @param	aStatusRef			Reference to result element
		* @param	aStatusCode			Status of command
		*/
		virtual void SetStatusL( TInt aStatusRef, TInt aStatusCode );
#endif
	private:
		
		RChildNodes& iChilds;
#ifdef __TARM_SYMBIAN_CONVERGENCY
 // nothing
#else
		HBufC8* iBuffer;
#endif
		TInt iErr;
	};

class CCommand : public CBase
{
	public:
		static CCommand* NewL( const TCertInfo& aCertInfo, const TAclCommands& aCommand); 
		static CCommand* NewL( const TDesC8& aServerUid, const TAclCommands& aCommand); 
	
		void CommandString( TDes8& aStr);
		
		//save and load
		TInt Length();
		HBufC8 * SaveStringL();
		static CCommand * LoadFromStringL( TPtrC8& aString);
	public:
		TCertInfo iCertInfo;
		HBufC8* iServerId;
		TAclCommands iCommand;
};

class CACLNode : public CBase
{
	public:
		CACLNode();
		~CACLNode();
		
		//URI handling	
		void SetURIL( const TDesC8& aURI );
		
		//ACL content handling
		void ClearACL();
		void SetServerIdToACLL( const TAclCommands& aCommandType, const TDesC8& aServerId);
		void SetCertificateToACLL( const TACLDestination& aDestination, const TAclCommands& aCommandType, const TCertInfo& aCertInfo);
		
		//save and load
		TInt Length();
		HBufC8 * SaveStringL();
		static CACLNode * LoadFromStringL( TDes8& aString);
		
		//list functions
		static TInt CompareElements( CACLNode const& aNode1, CACLNode const& aNode2);
		CCommand* FindCorrespondingCommand( const CCommand* aCommand);
		
		//ACL factory, creates ACL from string
		static CACLNode * CreateACLL( const TDesC8& aACLString);
		HBufC8 * GetNodeACLStringL();
	private:
		friend class CACLStorage;
		
		RPointerArray<CCommand> iCommands;
		
		TACLDestination iDestination;
		TBool iClearFlag;
		HBufC8* iURI;
		TPtrC8 iURIPtr;
};

class CACLStorage : public CBase
{
	public:
		//constructors
		CACLStorage();
		~CACLStorage();
		
		static CACLStorage* NewL();
		void ConstructL();
		
		//save and load
		void SaveACLL();
		void LoadACLL();

		//API calls
		TCertInfo& MngSessionCertificate();
		TInt RemoveACL( const TDesC8& aURI, TBool aRestoreDefaults);	
		void AddACLForNodeL( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType);	
		void SetACLForNodeL( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType);	
		
		//Certificate handling
		void NewSessionL( const TCertInfo& aCertInfo, const TDesC8& aServerID);
		void CloseSession( );
		TInt CertInfo( TCertInfo &aCertInfo );
		HBufC8* ServerIDL();
	private:
	    void SetCertInfo( const TCertInfo &aCertInfo );
		CACLNode * FindNodeL( const TDesC8& aURI, TBool aCreateNewIfDoesntExist);
		TInt RemoveNode( const TDesC8& aURI);
	
		void UpdateACLsL();
		void UpdateACLL( const CACLNode *aACLNode);
	private:
		void UpdateACLL( const CACLNode *aACLNode, const TDesC8& aURI);

		RPointerArray<CACLNode> iNodes;

        TBool iCertificateReceived;
        TCertInfo iMngSessionCertificate;
		TCertInfo iCurrentCertificate;
		HBufC8* iCurrentServerId;
		
		#ifdef __TARM_SYMBIAN_CONVERGENCY
		// nothing
		#else
		RNSmlDMCallbackSession iDbSession;
		#endif
};

#endif	//__ACL_STORAGE_H__
