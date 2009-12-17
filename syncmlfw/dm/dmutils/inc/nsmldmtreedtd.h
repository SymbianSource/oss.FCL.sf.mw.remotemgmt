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
* Description:  DM Utilities
*
*/

 

#ifndef __NSMLDMTREEDTD_H
#define __NSMLDMTREEDTD_H

#include <e32std.h>
#include <e32base.h>
#include <s32file.h>
#include <smldmadapter.h>


_LIT8( KNSmlDmTreeDTDVersion, "1.1" ); 

//XML generator flags
_LIT8( KNSmlDDFMgmtTree, "<MgmtTree>" ); 
_LIT8( KNSmlDDFMgmtTreeEnd, "</MgmtTree>" ); 
_LIT8( KNSmlDDFNode, "<Node>" ); 
_LIT8( KNSmlDDFNodeEnd, "</Node>" ); 
_LIT8( KNSmlDDFNodeName, "<NodeName>" ); 
_LIT8( KNSmlDDFNodeNameEnd, "</NodeName>" ); 
_LIT8( KNSmlDDFDFProperties, "<DFProperties>" ); 
_LIT8( KNSmlDDFDFPropertiesEnd, "</DFProperties>" ); 
_LIT8( KNSmlDDFPath, "<Path>" ); 
_LIT8( KNSmlDDFPathEnd, "</Path>" ); 
_LIT8( KNSmlDDFValue, "<Value>" ); 
_LIT8( KNSmlDDFValueEnd, "</Value>" ); 
_LIT8( KNSmlDDFVerDTD, "<VerDTD>" );
_LIT8( KNSmlDDFVerDTDEnd, "</VerDTD>" ); 
_LIT8( KNSmlDDFMan, "<Man>" );
_LIT8( KNSmlDDFManEnd, "</Man>" );
_LIT8( KNSmlDDFMod, "<Mod>" );
_LIT8( KNSmlDDFModEnd, "</Mod>" );
_LIT8( KNSmlDDFAccessType, "<AccessType>" );
_LIT8( KNSmlDDFAccessTypeEnd, "</AccessType>" );
_LIT8( KNSmlDDFDefaultValue, "<DefaultValue>" );
_LIT8( KNSmlDDFDefaultValueEnd, "</DefaultValue>" );
_LIT8( KNSmlDDFDescription, "<Description>" );
_LIT8( KNSmlDDFDescriptionEnd, "</Description>" );
_LIT8( KNSmlDDFDFTitle, "<DFTitle>" );
_LIT8( KNSmlDDFDFTitleEnd, "</DFTitle>" );
_LIT8( KNSmlDDFDFType, "<DFType>" );
_LIT8( KNSmlDDFDFTypeEnd, "</DFType>" );
_LIT8( KNSmlDDFDFFormat, "<DFFormat>" );
_LIT8( KNSmlDDFDFFormatEnd, "</DFFormat>" );
_LIT8( KNSmlDDFFormatB64, "<b64/>" );
_LIT8( KNSmlDDFFormatDate, "<date/>" );
_LIT8( KNSmlDDFFormatTime, "<time/>" );
_LIT8( KNSmlDDFFormatFloat, "<float/>" );
_LIT8( KNSmlDDFFormatBool, "<bool/>" );
_LIT8( KNSmlDDFFormatChr, "<chr/>" );
_LIT8( KNSmlDDFFormatInt, "<int/>" );
_LIT8( KNSmlDDFFormatNode, "<node/>" );
_LIT8( KNSmlDDFFormatNull, "<null/>" );
_LIT8( KNSmlDDFFormatXml, "<xml/>" );
_LIT8( KNSmlDDFFormatBin, "<bin/>" );
_LIT8( KNSmlDDFAccessTypeAdd, "<Add/>" );
_LIT8( KNSmlDDFAccessTypeDelete, "<Delete/>" );
_LIT8( KNSmlDDFAccessTypeGet, "<Get/>" );
_LIT8( KNSmlDDFAccessTypeReplace, "<Replace/>" );
_LIT8( KNSmlDDFAccessTypeExec, "<Exec/>" );
_LIT8( KNSmlDDFAccessTypeCopy, "<Copy/>" );
_LIT8( KNSmlDDFOccurrence, "<Occurrence>" );
_LIT8( KNSmlDDFOccurrenceEnd, "</Occurrence>" ); 
_LIT8( KNSmlDDFOccurrenceOne, "<One/>" );
_LIT8( KNSmlDDFOccurrenceZeroOrOne, "<ZeroOrOne/>" );
_LIT8( KNSmlDDFOccurrenceZeroOrMore, "<ZeroOrMore/>" );
_LIT8( KNSmlDDFOccurrenceOneOrMore, "<OneOrMore/>" );
_LIT8( KNSmlDDFOccurrenceZeroOrN, "<ZeroOrN/>" );
_LIT8( KNSmlDDFOccurrenceOneOrN, "<OneOrN/>" );
_LIT8( KNSmlDDFScope, "<Scope>" );
_LIT8( KNSmlDDFScopeEnd, "</Scope>" );
_LIT8( KNSmlDDFScopePermanent, "<Permanent/>" );
_LIT8( KNSmlDDFScopeDynamic, "<Dynamic/>" );
_LIT8( KNSmlDDFMIME, "<MIME>" );
_LIT8( KNSmlDDFMIMEEnd, "</MIME>" );

_LIT8( KNSmlDDFRTProperties, "<RTProperties>" ); 
_LIT8( KNSmlDDFRTPropertiesEnd, "</RTProperties>" ); 
_LIT8( KNSmlDDFACL, "<ACL>" ); 
_LIT8( KNSmlDDFACLEnd, "</ACL>" ); 
_LIT8( KNSmlDDFFormat, "<Format>" ); 
_LIT8( KNSmlDDFFormatEnd, "</Format>" ); 
_LIT8( KNSmlDDFName, "<Name>" ); 
_LIT8( KNSmlDDFNameEnd, "</Name>" ); 
_LIT8( KNSmlDDFSize, "<Size>" ); 
_LIT8( KNSmlDDFSizeEnd, "</Size>" );
_LIT8( KNSmlDDFTitle, "<Title>" ); 
_LIT8( KNSmlDDFTitleEnd, "</Title>" );
_LIT8( KNSmlDDFTStamp, "<TStamp>" ); 
_LIT8( KNSmlDDFTStampEnd, "</TStamp>" );
_LIT8( KNSmlDDFType, "<Type>" ); 
_LIT8( KNSmlDDFTypeEnd, "</Type>" );
_LIT8( KNSmlDDFVerNo, "<VerNo>" ); 
_LIT8( KNSmlDDFVerNoEnd, "</VerNo>" );

_LIT8( KNSmlDevInfoMan, "DevInfo/Man" );
_LIT8( KNSmlDevInfoMod, "DevInfo/Mod" );

_LIT( KNSmlTreeDTDPanic, "NSmlDMTreeDTD" );

_LIT8( KNSmlDDFCaseSense, "CS" );
_LIT8( KNSmlDDFCaseInSense, "CIS" );
enum TNSmlDdfWBXMLTags 
	{
EDDFMgmtTree = 5,
EDDFMgmtTreeEnd,
EDDFNode,
EDDFNodeEnd,
EDDFNodeName,
EDDFNodeNameEnd,        //10
EDDFDFProperties,
EDDFDFPropertiesEnd,
EDDFPath,
EDDFPathEnd,
EDDFValue,              //15
EDDFValueEnd,
EDDFVerDTD,
EDDFVerDTDEnd,
EDDFMan,
EDDFManEnd,             //20
EDDFMod,
EDDFModEnd,
EDDFAccessType,
EDDFAccessTypeEnd,
EDDFDefaultValue,       //25
EDDFDefaultValueEnd,
EDDFDescription,
EDDFDescriptionEnd,
EDDFDFTitle,
EDDFDFTitleEnd,         //30
EDDFDFType,
EDDFDFTypeEnd,
EDDFDFFormat,
EDDFDFFormatEnd,
EDDFFormatB64,          //35
EDDFFormatBool,
EDDFFormatChr,
EDDFFormatInt,
EDDFFormatNode,
EDDFFormatNull,         //40
EDDFFormatXml,
EDDFFormatBin,
EDDFFormatDate,
EDDFFormatTime,
EDDFFormatFloat,
EDDFAccessTypeAdd,
EDDFAccessTypeDelete,
EDDFAccessTypeGet,      //45
EDDFAccessTypeReplace,
EDDFAccessTypeExec,
EDDFAccessTypeCopy,
EDDFOccurrence,
EDDFOccurrenceEnd,      //50
EDDFOccurrenceOne,
EDDFOccurrenceZeroOrOne,
EDDFOccurrenceZeroOrMore,
EDDFOccurrenceOneOrMore,
EDDFOccurrenceZeroOrN,  //55
EDDFOccurrenceOneOrN,
EDDFScope,
EDDFScopeEnd,
EDDFScopePermanent,
EDDFScopeDynamic,       //60
EDDFMIME,
EDDFMIMEEnd,
EDDFRTProperties,
EDDFRTPropertiesEnd,
EDDFACL,                //65
EDDFACLEnd,
EDDFFormat,
EDDFFormatEnd,
EDDFName,
EDDFNameEnd,            //70
EDDFSize,
EDDFSizeEnd,
EDDFTitle,
EDDFTitleEnd,
EDDFTStamp,             //75
EDDFTStampEnd,
EDDFType,
EDDFTypeEnd,
EDDFVerNo,
EDDFVerNoEnd,            //80
EDDFStaticLevel,
EDDFStaticLevelEnd,
EDDFAdapterUid,
EDDFAdapterUidEnd,
EDDFHostServerId,
EDDFHostServerIdEnd
	};

enum TNSmlDmDDFFormat 
	{
	ENSmlDmDDFLeaf,
	ENSmlDmDDFNode,
	ENSmlDmDDFNotFound
	};


//class forwards
class CNSmlDmNodeList;

// ===========================================================================
//PCData element
// ===========================================================================
/**
* CNSmlDmPcdata is data element in DDF tree structure
*
*  @since
*/
class CNSmlDmPcdata : public CBase
	{
public:

	/**
	* Length of the data in this PCDATA structure
	*/
	TInt	iLength;
	
	/**
	* Pointer to the data
	*/
	TAny*	iContent;       

public:

	/**
	* Constructor
	*/
	CNSmlDmPcdata();

	/**
	* Destructor
	*/
	~CNSmlDmPcdata();
	
	/**
	* Frees content
	*/
	void FreeContent();
	
	/**
	* Sets data
	* @param	aData				Data to be set
	*/
	void DataL( const TDesC8& aData );


	/**
	* Compare data
	* @param	aStr				Data start  pointer
	* @param	aLength				Data length
	* @return						ETrue if data is similar to iContent
	*/
	TBool Compare(const TUint8* aStr, TInt aLength);

private:
	};


// ===========================================================================
// RTProperties 
// ===========================================================================
/**
* CNSmlDmRTProperties contains runtime properties element in DDF tree
* structure
*
*  @since
*/
class CNSmlDmRTProperties : public CBase
	{
public:

	/**
	* ACL Property
	*/
	CNSmlDmPcdata iACL;

	/**
	* Name property
	*/
	CNSmlDmPcdata iName;				

	/**
	* Format property
	*/
	MSmlDmDDFObject::TDFFormat	iFormat;

	/**
	* Size property, optional
	*/
	CNSmlDmPcdata* iSize;		

	/**
	* Title property, optional
	*/
	CNSmlDmPcdata* iTitle;		

	/**
	* TimeStamp property, optional
	*/
	CNSmlDmPcdata* iTStamp;		
	
	/**
	* Type property, optional
	*/
	CNSmlDmPcdata* iType;		

	/**
	* Version number property, optional
	*/
	CNSmlDmPcdata* iVerNo;		

public:
	/**
	* Destructor
	*/
	virtual ~CNSmlDmRTProperties();

	/**
	* Generates xml of elelment
	* @param	aObject			Buffer to write xml
	*/
	void GenerateRTPropertiesL(CBufBase& aObject);

private:
	/**
	* Constructor
	*/
	CNSmlDmRTProperties();
	};



// ===========================================================================
// DFProperties 
// ===========================================================================
/**
* CNSmlDmDFProperties contains description framwork properties element in
* DDF tree structure
*
*  @since
*/
class CNSmlDmDFProperties : public CBase
	{
public:

	/**
	* Accestype property of the node
	*/
	TUint8 iAccessTypes;

	/**
	* Default value property of the node, optional
	*/
	CNSmlDmPcdata* iDefaultValue;					

	/**
	* Description property of the node, optional
	*/
	CNSmlDmPcdata* iDescription;	

	/**
	* Format property of the node
	*/
	MSmlDmDDFObject::TDFFormat iDFFormat;		

	/**
	* Occurrence property of the node, optional
	*/
	MSmlDmDDFObject::TOccurence iOccurrence;

	/**
	* Scope property of the node, optional
	*/
	MSmlDmDDFObject::TScope iScope;	

	/**
	* Title property of the node, optional
	*/
	CNSmlDmPcdata* iDFTitle;
			
	/**
	* Type property of the node, optional
	*/
	CNSmlDmPcdata* iDFType;

    TBool iCaseSense;
public:

	/**
	* Desstructor
	*/
	~CNSmlDmDFProperties();

	/**
	* Constructor
	*/
	CNSmlDmDFProperties();

	/**
	* Generates xml of elelment
	* @param	aObject			Buffer to write xml
	*/
	void GenerateDFPropertiesL(CBufBase& aObject);

	/**
	* Generates wbxml of elelment
	* @param	aObject			Buffer to write xml
	*/
	void GenerateDFPropertiesWBXMLL(RWriteStream& aStream);

private:
	/**
	* Generates xml of elelment
	* @param	aObject			Buffer to write xml
	*/
	void GenerateDFPropsSubmethodL(CBufBase& aObject);
	};


// ===========================================================================
// CNSmlDmNode 
// ===========================================================================
/**
* CNSmlDmNode is node object in DDF structure
*
*  @since
*/
class CNSmlDmNode : public CBase, public MSmlDmDDFObject
	{
public:
	/**
	* Path of the node, optional
	*/
	CNSmlDmPcdata* iPath;

	/**
	* Run time properties of the node, optional
	*/
	CNSmlDmRTProperties* iRTProperties;
	
	/**
	* Description framework properties of the node
	*/
	CNSmlDmDFProperties iDFProperties;

	/**
	* Children of the node, optional
	* (if null -> leaf otherwise interior node)
	*/
	CNSmlDmNodeList* iNodeListPtr;
	
	/**
	* Default value of the node, optional
	*/
	CNSmlDmPcdata* iValue;

	/**
	* Name of the node
	*/
	CNSmlDmPcdata iNodeName;
	
	/**
	* True if node is static level, i.e. before firs nameless level
	*/
	TBool iStaticLevel;

	/**
	* Implementation uid of adapter which handles the current node
	*/
	TUint32 iImplUid;

	/**
	* Host server id which handles adapter
	*/
	TUint8 iHostServerId;

public:

	/**
	* Constructor
	*/
	CNSmlDmNode();

	/**
	* Desstructor
	*/
	virtual ~CNSmlDmNode();
	
	/**
	* Sets access types to node
	* @param	aAccessTypes		Access types
	*/
	void SetAccessTypesL( TSmlDmAccessTypes aAccessTypes );

	/**
	* Sets default value to node
	* @param	aDefaultValue		Default value
	*/
	void SetDefaultValueL( const TDesC8& aDefaultValue ); 

	/**
	* Sets description to node
	* @param	aDescription		Description
	*/
	void SetDescriptionL( const TDesC8& aDescription );

	/**
	* Sets format of the node
	* @param	aFormat				Format
	*/
	void SetDFFormatL( TDFFormat aFormat );
	
	/**
	* Sets occurrence of the node
	* @param	aOccurence			Occurrence
	*/
	void SetOccurenceL( TOccurence aOccurence );


    /**
	* Sets case sense of the node
	* @param	aOccurence			Occurrence
	*/
	void SetCaseSenseL( TBool aCaseSense );

	/**
	* Sets scope of the node
	* @param	aScope			Scope
	*/
	void SetScopeL( TScope aScope );

	/**
	* Sets Title of the node
	* @param	aTitle			Title
	*/
	void SetDFTitleL( const TDesC8& aTitle );

	/**
	* Sets mime type of the node
	* @param	aMimeType 		Mime type
	*/
	void AddDFTypeMimeTypeL( const TDesC8& aMimeType );
	
	/**
	* Sets the object as object group. Inherited from MSmlDmDDFObject
	* but not used anymore
	*/
	void SetAsObjectGroup();

	/**
	* Compare data
	* @param	aNodeName			Name of the child node
	* @return						Reference to created child object
	*/
	MSmlDmDDFObject& AddChildObjectL(const TDesC8& aNodeName);

	/**
	* Add nameless child to node
	* @return						Reference to created child object
	*/
	MSmlDmDDFObject& AddChildObjectGroupL();
	

	/**
	* Pointer to parent node
	*/
	CNSmlDmNode* iParent;


private:
	};


// ===========================================================================
// CNSmlDmNodeList 
// ===========================================================================
/**
* CNSmlDmNodeList is node list object in DDF structure and it contains pointer
* to node and pointer to next CNSmlDmNodeList object
*
*  @since
*/
class CNSmlDmNodeList : public CBase
	{
public:
	/**
	* Constructor
	*/
	CNSmlDmNodeList();
	
	/**
	* Pointer to node
	*/
	CNSmlDmNode* iNode;

	/**
	* Pointer to next list item
	*/
	CNSmlDmNodeList* iNext;

	/**
	* Generates nodelist xml to aObject
	* @param	aObject			Buffer to write xml
	*/
	void GenerateNodeListL(CBufBase& aObject);

	/**
	* Generates nodelist wbxml to aObject
	* @param	aStream			Stream to write wbxml
	*/
	void GenerateNodeListWBXMLL(RWriteStream& aStream);
	
	/**
	* Parse nodelist wbxml to structure
	* @param	aReader			Stream which contains wbxml
	* @param	aNodeListPtr	Reference to nodelist ptr where to build
	*							ddf structure
	* @param	aParent			Parent node pointer
	*/
	TUint ParseNodeListWBXMLL(RReadStream& aReader,
		CNSmlDmNodeList*& aNodeListPtr, CNSmlDmNode* aParent);
		
	/**
	* Parse node wbxml to structure
	* @param	aReader			Stream which contains wbxml
	* @param	aNode			Reference to node where to build
	*							ddf structure
	*/
	void ParseNodeWBXMLL(RReadStream& aReader,CNSmlDmNode* aNode );

	/**
	* Parse df properties wbxml to structure
	* @param	aReader			Stream which contains wbxml
	* @param	aNodePtr		Pointer to node where to build
	*							ddf structure
	*/
   	void ParseDFPropertiesWBXMLL(RReadStream& aReader,
   		MSmlDmDDFObject* aNodePtr );
   		
	/**
	* Get adapter id and host session id
	* @param	aImplUid		Reference where to write adapter id
	* @param	aSessionId		Reference where to write session id
	*/
	IMPORT_C void GetImplUid(TUint32& aImplUid,TUint8& aSessionId);

private:
	};


// ===========================================================================
// CNSmlDmMgmtTree 
// ===========================================================================
/**
* CNSmlDmMgmtTree is the root node in DDF structure
*
*  @since
*/
class CNSmlDmMgmtTree : public CBase, public MSmlDmDDFObject
	{
public:
	/**
	* Two-phased construction
	* @return				Pointer to newly created instance
	*/
	IMPORT_C static CNSmlDmMgmtTree* NewL();

	/**
	* Destructor
	*/
	virtual ~CNSmlDmMgmtTree();

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aAccessTypes		Access type
	*/
	void SetAccessTypesL( TSmlDmAccessTypes aAccessTypes );

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aDefaultValue		Default value
	*/
	void SetDefaultValueL( const TDesC8& aDefaultValue ); 

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aDescription		Description
	*/
	void SetDescriptionL( const TDesC8& aDescription );

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aFormat			Format
	*/
	void SetDFFormatL( TDFFormat aFormat );

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aOccurence		Occurence
	*/
	void SetOccurenceL( TOccurence aOccurence );

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aScope			Scope
	*/
	void SetScopeL( TScope aScope );

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aTitle			Title
	*/
	void SetDFTitleL( const TDesC8& aTitle );

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	* @param	aMimeType			Mime type
	*/
	void AddDFTypeMimeTypeL( const TDesC8& aMimeType );

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	*/
	void SetAsObjectGroup();

	/**
	* Adds child object with name under th root node.
	* This is inherited from MSmlDmDDFObject.
	* @param	aNodeName		Node name
	*/
	MSmlDmDDFObject& AddChildObjectL(const TDesC8& aNodeName);

	/**
	* Inherited from MSmlDmDDFObject, no use in this class
	*/
	MSmlDmDDFObject& AddChildObjectGroupL();

	/**
	* Generates xml of the whole DDF and writes it to aObject
	* @param	aObject			Reference to buffer where to write
	*							generated xml
	*/
	IMPORT_C void GenerateXMLL(CBufBase& aObject);

	/**
	* Looks through the ddf tree and sets the adapter id to new static nodes
	* @param	aUid			Adapter id
	*/
	IMPORT_C void SetAdapterUid(TUint32 aUid);

	/**
	* Looks through the ddf tree and sets the session id to new static nodes
	* @param	aServerId		Id of the session to correct host server
	*/
	IMPORT_C void SetServerId(TUint8 aServerId);

	/**
	* Returns pointer to actual root nodelist pointer. The whole tree can be
	* accessed through this pointer
	* @reutrn				Root nodelist pointer
	*/
	IMPORT_C CNSmlDmNodeList* NodeListPtr();

	/**
	* Generates wbxml of the whole DDF and writes it to aStream
	* @param	aStream			Reference to stream where to write
	*							generated wbxml
	*/
	IMPORT_C void GenerateWBXMLL(RWriteStream& aStream);

	/**
	* Parses wbxml from stream to ddf structure. This is used when
	* ddf subtree is got from hostserver. The ddf is parsed on the existing
	* tree, i.e the whole tree is got when all subtrees are parsed.
	* @param	aReader			Reference to stream where to read wbxml
	*							
	*/
	IMPORT_C void ParseWBXMLL(RReadStream& aReader);

	/**
	* Finds the node pointed by aURI from tree 
	* @param	aURI			Uri
	* @param	aFinalPtr		Found pointer is written here
	* @return					ENSmlDmDDFLeaf,ENSmlDmDDFNode or
	*							ENSmlDmDDFNotFound
	*/
	IMPORT_C TNSmlDmDDFFormat FindNameFromNodeListL(const TDesC8& aURI,
		CNSmlDmNodeList*& aFinalPtr);
		
	/**
	* Finds the allrady loaded adapters 
	* @param	aLoadedAdapters Arrayf of found adapter ids			
	* @param	aNodeListPtr	Algorythm is recursive and next level nodelist
	*							pointer is provided here
	* @return					Not used
	*/
	IMPORT_C TBool FindLoadedAdaptersL(RArray<TUint32>& aLoadedAdapters,
		CNSmlDmNodeList* aNodeListPtr=NULL);

	
private:
	/**
	* Constructor
	*/
	CNSmlDmMgmtTree();

	/**
	* Second phase constructor
	*/
	void ConstructL();

	/**
	* Looks through the ddf tree and sets the adapter id to new static nodes
	* @param	aNodeListPtr	Algorythm is recursive and next level nodelist
	*							pointer is provided here
	* @param	aUid			Adapter id
	*/
	TBool SetUidToNewItems ( CNSmlDmNodeList* aNodeListPtr, TUint32 aUid);

	/**
	* Looks through the ddf tree and sets the session id to new static nodes
	* @param	aNodeListPtr	Algorythm is recursive and next level nodelist
	*							pointer is provided here
	* @param	aServerId		Id of the session to correct host server
	*/
	TBool SetServerIdToNewItems ( CNSmlDmNodeList* aNodeListPtr,
		TUint8 aServerId );

public:
	
	/**
	* Pointer to root node list pointer
	*/
	CNSmlDmNodeList* iNodeListPtr;


private:
	/**
	* dtd version
	*/
	CNSmlDmPcdata iVerDTD;
	};




#endif // __NSMLDMTREEDTD_H
