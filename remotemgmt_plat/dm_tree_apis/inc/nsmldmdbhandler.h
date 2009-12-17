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
* Description:  header file for dm tree db handler
*
*/



#ifndef __NSMLDMDBHANDLER_H
#define __NSMLDMDBHANDLER_H

#include <e32std.h>
#include <e32base.h>
#include <d32dbms.h>
#include <f32file.h>
#include <s32file.h>
#include <smldmadapter.h>
#include <nsmldmtreedbclient.h>


_LIT( KSmlDmModuleDbName, "NSmlDmTree.db" );

_LIT( KSmlDmModuleDbPath, "C:\\System\\data\\NSmlDmTree.db" ); 

_LIT(KNSmlDmServerId, "ServerId");
_LIT(KNSmlDmAdapterNames, "AdapterNames");
_LIT(KNSmlDmDDFVersions, "DDFVersions");
_LIT(KNSmlDmDllUids, "DllUids");
_LIT(KNSmlDmImplUids, "ImplUids");
_LIT(KNSmlDmDDFVersionTable, "DDFVersionTable");

_LIT(KNSmlDmLuidMappingTable, "LuidMappingTable");
_LIT(KNSmlDmAdapterId, "AdapterId");
_LIT(KNSmlDmMappingURI, "MappingURI");
_LIT(KNSmlDmMappingLuid, "MappingLuid");

_LIT(KNSmlDmAclTable, "AclTable");
_LIT(KNSmlDmAclURI, "AclURI");
_LIT(KNSmlDmAcl, "Acl");


_LIT8(KNSmlDmAclDefaultRoot, "Add=*&Get=*&Exec=*");
_LIT8(KNSmlDmAclDefault, "Add=*&Replace=*&Get=*&Delete=*&Exec=*");
_LIT8(KNSmlDmAclAddEqual, "Add=");
_LIT8(KNSmlDmAclGetEqual, "Get=");
_LIT8(KNSmlDmAclDeleteEqual, "Delete=");
_LIT8(KNSmlDmAclReplaceEqual, "Replace=");
_LIT8(KNSmlDmAclAddForAll, "Add=*");
_LIT8(KNSmlDmAclAll, "*");
_LIT8(KNSmlDmAclSeparator, "&");
_LIT8(KNSmlDmAclExecEqual, "Exec=");
_LIT8(KNSmlDmAclServerIdSeparator, "+");

_LIT8(KNSmlDmProperty, "?prop=" );
_LIT8(KNSmlDmLitSeparator, "/" );
_LIT(KNSmlDmLitMappingSeparator16, "&" );
_LIT8(KNSmlDmLitAclNull, "null" );

_LIT( KSmlDmTreeDbHandlerPanic, "NSmlDmTreeDBHandler" );

const TInt KNSmlDMMappingSeparator = 0x26; //'&' 
const TInt KNSmlDMAclSeparator = 0x2b; //'+' 
const TInt KNSmlDMAclUriSeparator = 0x1e; //Record Separator
const TInt KNSmlDMAclCommandSeparator = 0x26; //'&' 

// Number of elements inserted in one go
const TInt KGranularity = 8;
const TUint32 KNSmlAclDbId = 1;
const TInt KNSmlDmIdLength = 10;

enum TNSmlDmMapOperation
   	{
   	EMapNoAction,
   	EMapDelete,
   	EMapInsert,
   	EMapUpdate,
   	EMapRename
   	};

enum TNSmlDmSearchMethod
   	{
   	ENoSearch,
   	ENormalSearch,
   	EUsingParents,
   	EAmongChildren,
   	EAmongParents
   	};


class CNSmlDmURIMapping;
class CNSmlDmAdapterElement;

// ===========================================================================
// CNSmlDmDbHandler
// ===========================================================================

/**
* CNSmlDmDbHandler is interface to physical tree db database
*
*  @since
*/
class CNSmlDmDbHandler : public CBase
	{
	public:

	/**
	* Two-phased constructor, leaves pointer to cleanup stack
	* @return						Pointer to newly created module instance
	*/
	IMPORT_C static CNSmlDmDbHandler* NewLC(); 

	/**
	* Two-phased constructor
	* @return						Pointer to newly created module instance
	*/
	IMPORT_C static CNSmlDmDbHandler* NewL(); 

	/**
	* Destructor
	*/
	IMPORT_C ~CNSmlDmDbHandler();

	/**
	* Sets server id. This is not stored, this is set at the beginning
	* of session fro checkin acls
	* @param	aServerId	Server identifier
	*/
	IMPORT_C void SetServerL ( const TDesC8& aServerId );

	//Luid mapping functions

	/**
	* Get mapping info from db
	* @param	aAdapterId	Adapter id 
	* @param	aURI		URI for returning the mapped luid
	* @param	aLuid		Buffer for writing the mapped luid
	* @return				Success of operation
	*/
	IMPORT_C TInt GetMappingInfoLC ( TUint32 aAdapterId,
		const TDesC8& aURI,
		HBufC8*& aLuid );
	
	/**
	* Adds new mapping info to db
	* @param	aAdapterId	Adapter id 
	* @param	aURI		URI which is mapped with luid
	* @param	aLuid		Luid which is mapped with URI
	* @return				Success of operation
	*/
	IMPORT_C TInt AddMappingInfoL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aLuid );
		
	/**
	* Removes mapping info
	* @param	aAdapterId	Adapter id 
	* @param	aURI		URI which should be deleted from mappings
	* @param	aChildAlso	If true, also child uries are removed
	* @return				Success of operation
	*/
	IMPORT_C TInt RemoveMappingInfoL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		TBool aChildAlso = ETrue );
		
	/**
	* Renames mapping info
	* @param	aAdapterId	Adapter id 
	* @param	aURI		URI which should be renamed
	* @param	aObject		New name, i.e. last uri segment
	* @return				Success of operation
	*/
	IMPORT_C TInt RenameMappingL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aObject );
		
	/**
	* Writes mappings from memory to database
	* @return				Success of operation
	*/
	IMPORT_C TInt WriteMappingInfoToDbL();
	
	/**
	* Get list of mappings in certain level
	* @param	aAdapterId	Adapter id 
	* @param	aURI		URI which should be renamed
	* @param	aURISegList	Reference to list where to write list of mappings
	* @return				Success of operation
	*/
	IMPORT_C TInt GetURISegmentListL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		CArrayFix<TSmlDmMappingInfo>& aURISegList );
		
	/**
	* Clears the tree when child list is got from plug-in adapter
	* @param	aAdapterId	Adapter id 
	* @param	aURI		URI which should be renamed
	* @param	aCurrentList 	List of children. The mappings which are not
	*							in list are removed	
	*/
	IMPORT_C void UpdateMappingInfoL ( TUint32 aAdapterId,
		const TDesC8& aURI,
		CBufBase& aCurrentList );


	//acl functions

	/**
	* Delete ACL which is set to URI
	* @param	aURI		URI from which the acl info must be deleted
	* @return				Success of operation
	*/
	IMPORT_C TInt DeleteAclL(const TDesC8& aURI);

	/**
	* Update ACL for URI
	* @param	aURI		URI for updating the ACL info
	* @return				Success of operation
	*/
	IMPORT_C TInt UpdateAclL(const TDesC8& aURI,const TDesC8& aACL);

	/**
	* Get ACL which is set for URI
	* @param	aURI		URI for returning the ACL info
	* @param	aACL		Buffer for writing the found acl
	* @param	aInherited	If true, the inherited value is given if the
	*						ACL is not set to given URI
	* @return				Success of operation
	*/
	IMPORT_C TInt GetAclL(const TDesC8& aURI,
		CBufBase& aACL,TBool aInherited = ETrue);
		
	/**
	* Write ACLs from memory to db
	*/
	IMPORT_C TInt WriteAclInfoToDbL();

	/**
	* Check if current server has aacl rights for URI
	* @param	aURI		URI for checking ACL
	* @param	aCmdType	Type of command (add,..)
	* @return				ETrue if access ok
	*/
	IMPORT_C TBool CheckAclL(const TDesC8& aURI, TNSmlDmCmdType aCmdType);

	/**
	* Set default acls to current server for certain URI
	* @param	aURI		URI which acl is set to default for curren server
	* @return				Success of operation
	*/
	IMPORT_C TInt DefaultACLsToServerL(const TDesC8& aURI);

	/**
	* Erase server references from all the acls
	* @param	aServerId	
	* @return				Success of operation
	*/
	IMPORT_C TInt EraseServerIdL(const TDesC8& aServerId);


	private:  // functions
	
	void CreateDatabaseL( const TDesC& aFullName );
	void ConstructL();
	TInt GetAclDataFromDbL();
	TInt RenameAclL(const TDesC8& aURI, const TDesC8& aNewURI);
	void ReadAllURIMappingsFromDbL();
	void ReadOneLineFromDbL();
	
	void UpdateInternalArrayL( TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aLUID,
		TNSmlDmMapOperation aOperation,
		TNSmlDmSearchMethod aMethod,
		const TDesC8& aNwUri = KNullDesC8 );
		
	TInt SearchUriL( TUint32 aAdapterId,
		const TDesC8& aURI,
		TNSmlDmSearchMethod aSearchMethod,
		TInt& aAdIndex,
		RArray<TInt>& aUriIndex );
		
	TInt SearchRowInDbL( TUint32 aAdapterId, const TDesC8& aURI );
	
	void UpdateRowInDbL( TUint32 aAdapterId,
		const TDesC8& aURI,
		const TDesC8& aLUID,
		TNSmlDmMapOperation aOperation );
		

	private:  // data
	
		
	RFs iFsSession;
	HBufC8* iServer;
	TBool iAclUpToDate;

	RPointerArray<HBufC8> iAclURIs;
	RPointerArray<HBufC8> iAcls;

	RDbStoreDatabase iDatabase;
    CFileStore* iFileStore;
	RDbView iView;
	
	RPointerArray<CNSmlDmAdapterElement> iAdapters;
	TBool iURIMappingsReadFromDb;
	TBool iPrepareView;
	};


// ===========================================================================
// CNSmlDmURIMapping
// ===========================================================================
/**
* CNSmlDmURIMapping is container element for mappings when keeping them memory
*
*  @since
*/
class CNSmlDmURIMapping : public CBase
	{	
	public:
		/**
		* Two-phased constructor. Leaves instance to cleanup stack
		* @return				Pointer to newly created module instance
		*/
		static CNSmlDmURIMapping* NewLC ();
		
		/**
		* Two-phased constructor. Leaves instance to cleanup stack
		* @param	aURI		Mapped URI
		* @param	aLUID		Mapped luid
		* @param	aOperation	Operation which is made to element
		* @return				Pointer to newly created module instance
		*/
		static CNSmlDmURIMapping* NewLC ( const TDesC8& aURI, 
			const TDesC8& aLUID,
			TNSmlDmMapOperation aOperation );
			
		/**
		* Destructor
		*/
		~CNSmlDmURIMapping();
	public:
		/**
		* URI
		*/
		HBufC8* iURI;

		/**
		* Luid
		*/
		HBufC8* iLUID;

		/**
		* Operation information
		*/
		TNSmlDmMapOperation iOperation;
	private:
		void ConstructL();
		
		void ConstructL ( const TDesC8& aURI,
			const TDesC8& aLUID,
			TNSmlDmMapOperation aOperation );
	};


// ===========================================================================
// CNSmlDmAdapterElement
// ===========================================================================

/**
* CNSmlDmAdapterElement is container element for arraying mapping elements
* for each adapter
*
*  @since
*/
class CNSmlDmAdapterElement : public CBase
{	
	public:
		/**
		* Two-phased constructor. Leaves instance to cleanup stack
		* @param	aAdapterId	Adapter id
		* @return				Pointer to newly created module instance
		*/
		static CNSmlDmAdapterElement* NewLC ( TUint32 aAdapterId );

		/**
		* Destructor
		*/
		~CNSmlDmAdapterElement();		
	public:

		/**
		* Adapter id
		*/
		TUint32 iAdapterId;

		/**
		* Array of mappings belongin to current adapter
		*/
		RPointerArray<CNSmlDmURIMapping> iMapArray;
	private:
		CNSmlDmAdapterElement(TUint32 aAdapterId);		
};



#endif // __NSMLDMDBHANDLER_H