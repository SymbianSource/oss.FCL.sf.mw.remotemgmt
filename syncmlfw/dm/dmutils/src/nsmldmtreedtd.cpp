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

 
#include "nsmldmtreedtd.h"
#include <nsmldmuri.h>
#include <s32file.h> 
#include <featmgr.h>

// ===========================================================================
// CNSmlDmPcdata
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmPcdata::CNSmlDmPcdata()
// ---------------------------------------------------------------------------
CNSmlDmPcdata::CNSmlDmPcdata()
	{
	}

// ---------------------------------------------------------------------------
// CNSmlDmPcdata::~CNSmlDmPcdata()
// ---------------------------------------------------------------------------
CNSmlDmPcdata::~CNSmlDmPcdata()
	{
	FreeContent();
	}

// ---------------------------------------------------------------------------
// CNSmlDmPcdata::FreeContent()
// Free the content of pcdata element
// ---------------------------------------------------------------------------
void CNSmlDmPcdata::FreeContent()
	{
    User::Free(iContent);
    iContent = 0;
	}

// ---------------------------------------------------------------------------
// CNSmlDmPcdata::DataL()
// fill the pc data element
// ---------------------------------------------------------------------------
void CNSmlDmPcdata::DataL( const TDesC8& aData )
	{
	iLength = aData.Length();
	iContent = User::AllocL(iLength);
	Mem::Copy(iContent, aData.Ptr(), iLength);
	}


// ---------------------------------------------------------------------------
// CNSmlDmPcdata::Compare()
// compare content of pcdata element to aStr 
// ---------------------------------------------------------------------------
TBool CNSmlDmPcdata::Compare(const TUint8* aStr, TInt aLength)
	{
	if(iLength!=aLength)
		{
		return EFalse;
		}

	for(TInt i=0;i<iLength;i++)
		{
		if(((TUint8*)iContent)[i]!=aStr[i])
			{
			return EFalse;
			}
		}
	return ETrue;
	}

// ===========================================================================
// CNSmlDmNode
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmNode::CNSmlDmNode()
// ---------------------------------------------------------------------------
CNSmlDmNode::CNSmlDmNode()
	{
	    iDFProperties.iCaseSense=ETrue;
	}


// ---------------------------------------------------------------------------
// CNSmlDmNode::~CNSmlDmNode()
// ---------------------------------------------------------------------------
CNSmlDmNode::~CNSmlDmNode()
	{
	delete iPath;
	delete iRTProperties;
	delete iValue;
		
	CNSmlDmNodeList* ptr = iNodeListPtr;
	while(iNodeListPtr!=0)
		{
		delete iNodeListPtr->iNode;
		iNodeListPtr = iNodeListPtr->iNext;
		delete ptr;
		ptr = iNodeListPtr; 
		}
	}

// ---------------------------------------------------------------------------
// CNSmlDmNode::SetAccessTypesL( TSmlDmAccessTypes aAccessTypes )
// set accestypes of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetAccessTypesL( TSmlDmAccessTypes aAccessTypes )
	{
	iDFProperties.iAccessTypes = aAccessTypes.GetACL();
	}


// ---------------------------------------------------------------------------
// CNSmlDmNode::SetDefaultValueL( const TDesC8& aDefaultValue )
// Set default value of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetDefaultValueL( const TDesC8& aDefaultValue )
	{
	if(iDFProperties.iDefaultValue!=0)
		{
		delete iDFProperties.iDefaultValue;
		iDFProperties.iDefaultValue=0;
		}
	iDFProperties.iDefaultValue = new (ELeave) CNSmlDmPcdata();
	iDFProperties.iDefaultValue->DataL(aDefaultValue);
	}


// ---------------------------------------------------------------------------
// CNSmlDmNode::SetDescriptionL( const TDesC8& aDescription )
// Set description of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetDescriptionL( const TDesC8& aDescription )
	{
	if(iDFProperties.iDescription!=0)
		{
		delete iDFProperties.iDescription;
		iDFProperties.iDescription=0;
		}
	iDFProperties.iDescription = new (ELeave) CNSmlDmPcdata();
	iDFProperties.iDescription->DataL(aDescription);
	}

// ---------------------------------------------------------------------------
// CNSmlDmNode::SetDFFormatL( TDFFormat aFormat )
// set dfformat of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetDFFormatL( TDFFormat aFormat )
	{
	iDFProperties.iDFFormat = aFormat;
	}

// ---------------------------------------------------------------------------
// CNSmlDmNode::SetOccurenceL( TOccurence aOccurence )
// set occurrence of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetOccurenceL( TOccurence aOccurence )
	{
	iDFProperties.iOccurrence = aOccurence;
	}


void CNSmlDmNode::SetCaseSenseL( TBool aCaseSense )
    {
    iDFProperties.iCaseSense = aCaseSense;
	}

// ---------------------------------------------------------------------------
// CNSmlDmNode::SetScopeL( TScope aScope )
// set scope of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetScopeL( TScope aScope )
	{
	iDFProperties.iScope = aScope;
	}


// ---------------------------------------------------------------------------
// CNSmlDmNode::SetDFTitleL( const TDesC8& aDescription )
// set dftitle of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetDFTitleL( const TDesC8& /*aDescription*/ )
	{
	}


// ---------------------------------------------------------------------------
// CNSmlDmNode::AddDFTypeMimeTypeL( const TDesC8& aMimeType )
// set mimetype of node to ddf
// ---------------------------------------------------------------------------
void CNSmlDmNode::AddDFTypeMimeTypeL( const TDesC8& aMimeType )
	{
	if(iDFProperties.iDFType!=0)
		{
		delete iDFProperties.iDFType;
		iDFProperties.iDFType=0;
		}
	iDFProperties.iDFType = new (ELeave) CNSmlDmPcdata();
	iDFProperties.iDFType->DataL(aMimeType);
	}


// ---------------------------------------------------------------------------
// CNSmlDmNode::SetAsObjectGroup()
// se as object group, adapter level is the last objectgroup level
// ---------------------------------------------------------------------------
void CNSmlDmNode::SetAsObjectGroup()
	{
	}



// ---------------------------------------------------------------------------
// CNSmlDmNode::AddChildObjectL( const TDesC8& aNodeName )
// Add child object for the node
// ---------------------------------------------------------------------------
MSmlDmDDFObject& CNSmlDmNode::AddChildObjectL( const TDesC8& aNodeName )
	{
	CNSmlDmNode* newNode;

	if ( iNodeListPtr == 0 )
		{
		newNode = new (ELeave) CNSmlDmNode();
		CleanupStack::PushL(newNode);
		newNode->iParent = this;
		iNodeListPtr = new (ELeave) CNSmlDmNodeList();
		CleanupStack::Pop(); // newNode
		iNodeListPtr->iNext = 0;
		iNodeListPtr->iNode = newNode;
		if(iStaticLevel)
			{
			newNode->iStaticLevel=ETrue;
			}
		newNode->iNodeName.DataL(aNodeName);
		}
	else
		{
		CNSmlDmNodeList* tmpNode = iNodeListPtr;
		CNSmlDmNodeList* tmpNodePrev = iNodeListPtr;
		TBool found=EFalse;

		//check if same nodename already exists
		while(tmpNode!=0)
			{
			if(tmpNode->iNode->iNodeName.Compare( aNodeName.Ptr(),
				aNodeName.Length() ) )
				{
				found = ETrue;
				break;
				}
			tmpNodePrev = tmpNode;
			tmpNode = tmpNode->iNext;
			}

		if(found)
			{
			//nodename already exist, new node is not created and pointer
			//to existing is returned
			newNode = tmpNode->iNode;
			}
		else
			{
			tmpNodePrev->iNext = new (ELeave) CNSmlDmNodeList();
			newNode = new (ELeave) CNSmlDmNode();
			newNode->iParent = this;
			tmpNode = tmpNodePrev->iNext;
			tmpNode->iNext = 0;
			tmpNode->iNode = newNode;
			if(iStaticLevel)
				{
				newNode->iStaticLevel=ETrue;
				}
			newNode->iNodeName.DataL( aNodeName );
			}
		}
	return *newNode;	
	}

// ---------------------------------------------------------------------------
// CNSmlDmNode::AddChildObjectGroupL()
// Adds nameless child to node
// ---------------------------------------------------------------------------
MSmlDmDDFObject& CNSmlDmNode::AddChildObjectGroupL()
	{
	CNSmlDmNode* newNode = 0;

	if(iNodeListPtr==0)
		{
		newNode = new (ELeave) CNSmlDmNode();
		CleanupStack::PushL(newNode);
		newNode->iParent = this;
		iNodeListPtr = new (ELeave) CNSmlDmNodeList();
		iNodeListPtr->iNext = 0;
		iNodeListPtr->iNode = newNode;
		CleanupStack::Pop(); //newNode

		newNode->iNodeName.DataL(KNullDesC8);
		}
	else
		{
		User::Leave(KErrAlreadyExists);
		}
		
	return *newNode;
	}

// ===========================================================================
// CNSmlDmNodeList
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmNodeList::CNSmlDmNodeList()
// ---------------------------------------------------------------------------
CNSmlDmNodeList::CNSmlDmNodeList()
	{
	}

// ---------------------------------------------------------------------------
// CNSmlDmNodeList::GetImplUid()
// return implementation uid of adapter
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmNodeList::GetImplUid(TUint32& aImplUid,
	TUint8& aSessionId)
	{
	CNSmlDmNode* nodePtr = this->iNode;
	while(nodePtr!=0)
		{
		if(nodePtr->iImplUid!=0)
			{
			aImplUid=nodePtr->iImplUid;
			aSessionId=nodePtr->iHostServerId;
			return;
			}
		nodePtr=nodePtr->iParent;
		}
	aImplUid=0;
	aSessionId=0;
	}



// ---------------------------------------------------------------------------
// CNSmlDmNodeList::GenerateNodeListL(CBufBase& aObject)
// generate xml
// ---------------------------------------------------------------------------
void CNSmlDmNodeList::GenerateNodeListL(CBufBase& aObject)
	{
	CNSmlDmNodeList* listPtr = this;
	while(listPtr!=0)
		{

		CNSmlDmNode* nodePtr = listPtr->iNode;
		aObject.InsertL(aObject.Size(),KNSmlDDFNode);
		aObject.InsertL(aObject.Size(),KNSmlDDFNodeName);
		aObject.InsertL(aObject.Size(),nodePtr->iNodeName.iContent,
			nodePtr->iNodeName.iLength);
			
		aObject.InsertL(aObject.Size(),KNSmlDDFNodeNameEnd);

		if(nodePtr->iPath!=0)
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFPath);
			aObject.InsertL(aObject.Size(),nodePtr->iPath->iContent,
				nodePtr->iPath->iLength);
			aObject.InsertL(aObject.Size(),KNSmlDDFPathEnd);
			}
		nodePtr->iDFProperties.GenerateDFPropertiesL(aObject);
		
		if(nodePtr->iNodeListPtr!=0)
			{
			nodePtr->iNodeListPtr->GenerateNodeListL(aObject);
			}
		else
			{
			if(nodePtr->iValue!=0)
				{
				aObject.InsertL(aObject.Size(),KNSmlDDFValue);
				aObject.InsertL(aObject.Size(),nodePtr->iValue->iContent,
					nodePtr->iValue->iLength);
				aObject.InsertL(aObject.Size(),KNSmlDDFValueEnd);
				}

			}
		aObject.InsertL(aObject.Size(),KNSmlDDFNodeEnd); 
		listPtr = listPtr->iNext;
		}
	}

// ---------------------------------------------------------------------------
// CNSmlDmNodeList::GenerateNodeListWBXMLL(TPtr8& aObject)
// generate wbxml
// ---------------------------------------------------------------------------
void CNSmlDmNodeList::GenerateNodeListWBXMLL(RWriteStream& aStream)
	{
	CNSmlDmNodeList* listPtr = this;
	while(listPtr!=0)
		{
		CNSmlDmNode* nodePtr = listPtr->iNode;
		aStream.WriteUint8L(EDDFNode);
		// Add the iObjectGroup field
		aStream.WriteUint8L(EDDFStaticLevel);
		aStream.WriteUint8L(nodePtr->iStaticLevel);
		aStream.WriteUint8L(EDDFStaticLevelEnd);
		aStream.WriteUint8L(EDDFAdapterUid);
		aStream.WriteUint32L(nodePtr->iImplUid);
		aStream.WriteUint8L(EDDFAdapterUidEnd);
		aStream.WriteUint8L(EDDFHostServerId);
		aStream.WriteUint8L(nodePtr->iHostServerId);
		aStream.WriteUint8L(EDDFHostServerIdEnd);
		aStream.WriteUint8L(EDDFNodeName);
		aStream.WriteUint8L(nodePtr->iNodeName.iLength);
		
		aStream.WriteL(static_cast<TUint8*>(nodePtr->iNodeName.iContent),
			nodePtr->iNodeName.iLength);
			
		aStream.WriteUint8L(EDDFNodeNameEnd);
		//Path is not added
		nodePtr->iDFProperties.GenerateDFPropertiesWBXMLL(aStream);
		
		if(nodePtr->iNodeListPtr!=0)
			{
			nodePtr->iNodeListPtr->GenerateNodeListWBXMLL(aStream);
			}
		else
			{
			if(nodePtr->iValue!=0)
				{
				aStream.WriteUint8L(EDDFValue);
				aStream.WriteUint8L(nodePtr->iValue->iLength);
				aStream.WriteL(static_cast<TUint8*>(nodePtr->iValue->iContent),
					nodePtr->iValue->iLength);
				aStream.WriteUint8L(TChar(EDDFValueEnd));
				}
			}
		aStream.WriteUint8L(EDDFNodeEnd); 
		listPtr = listPtr->iNext;
		}
	}



// ---------------------------------------------------------------------------
// CNSmlDmNodeList::ParseNodeListWBXMLL(TPtr8& aObject)
// Parse nodelist from wbxml to structure
// ---------------------------------------------------------------------------
TUint CNSmlDmNodeList::ParseNodeListWBXMLL(RReadStream& aReader,
	CNSmlDmNodeList*& aNodeListPtr, CNSmlDmNode* aParent)
	{
	CNSmlDmNode* newNode = NULL;
	TUint tag = 0;
	CNSmlDmNodeList* lastInList = NULL;

	// read node tag
	tag = aReader.ReadUint8L();
	if (tag == EDDFNode )
	    {
		newNode = new (ELeave) CNSmlDmNode();
       	newNode->iParent= aParent;
		aNodeListPtr->iNode = newNode;
		lastInList=aNodeListPtr;
    
		ParseNodeWBXMLL( aReader, newNode );
		newNode = NULL;
		}
	else
		{
		delete aNodeListPtr;
		aNodeListPtr = NULL;
		return tag;
		}
		
	// read next tag ( EDDFNode | EDDFNodeEnd  )
	tag = aReader.ReadUint8L();
   	__ASSERT_DEBUG(((tag == EDDFNode) || (tag == EDDFNodeEnd)||
   		(tag == EDDFMgmtTreeEnd)),
   		User::Panic(_L("ParseNodeListWBXMLL: unknown tag: "),tag));
		            
	while ( tag == EDDFNode)
		{
   		lastInList->iNext = new (ELeave) CNSmlDmNodeList();
   		lastInList=lastInList->iNext;
		   
		newNode = new (ELeave) CNSmlDmNode();
	   	newNode->iParent= aParent;
	   	lastInList->iNode = newNode;
	        
       	lastInList->ParseNodeWBXMLL( aReader, newNode );
            
      	// read next tag
    	tag = aReader.ReadUint8L();
		}
	return tag;
	}
	

// ---------------------------------------------------------------------------
// CNSmlDmNodeList::ParseNodeWBXMLL(TPtr8& aObject)
// Parse node from wbxml to structure
// ---------------------------------------------------------------------------
void CNSmlDmNodeList::ParseNodeWBXMLL(RReadStream& aReader,CNSmlDmNode* aNode )
	{
	HBufC8* data;
	TUint tag = 0;

   	//EDDFStaticLevel tag
  	tag = aReader.ReadUint8L();
    aNode->iStaticLevel = aReader.ReadUint8L();
   	tag = aReader.ReadUint8L();

   	//EDDFAdapterUid tag
  	tag = aReader.ReadUint8L();
    aNode->iImplUid = aReader.ReadUint32L();
   	tag = aReader.ReadUint8L();

   	//EDDFHostServerId tag
  	tag = aReader.ReadUint8L();
    aNode->iHostServerId = aReader.ReadUint8L();
   	tag = aReader.ReadUint8L();

	// nodename tag
	tag = aReader.ReadUint8L();

	// read nodename length
	TUint8 len = aReader.ReadUint8L();
	if ( len != 0 )
		{
		data = HBufC8::NewLC( len );
		TPtr8 dataPtr = data->Des();
		// ..and nodename
		aReader.ReadL(dataPtr, len);
	   	aNode->iNodeName.DataL(dataPtr);
	   	CleanupStack::PopAndDestroy();	
	   	}
	
	// end nodename
	tag = aReader.ReadUint8L();
				
	// read next tag
	tag = aReader.ReadUint8L();
	__ASSERT_DEBUG( tag == EDDFDFProperties,
		User::Panic(_L("ParseNodeWBXMLL: wrong tag: "),tag));
		
   	// Path is not added to data    
	ParseDFPropertiesWBXMLL( aReader, aNode );
		

	if ( aNode->iNodeListPtr == NULL )
		{
		aNode->iNodeListPtr = new (ELeave) CNSmlDmNodeList();
       	tag = ParseNodeListWBXMLL(aReader, aNode->iNodeListPtr, aNode);
		}

   	if (tag == EDDFValue )
		{
		// value length
		len = aReader.ReadUint8L();
		// value data
		data = HBufC8::NewLC( len );
		TPtr8 dataPtr = data->Des();
		aReader.ReadL(dataPtr, len);
		// store the data
		aNode->iValue->DataL(dataPtr);
		// end value tag
		tag = aReader.ReadUint8L();
		CleanupStack::PopAndDestroy(); // data	
		}
	}

// ===========================================================================
// CNSmlDmMgmtTree
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::NewL()
// ---------------------------------------------------------------------------
EXPORT_C CNSmlDmMgmtTree* CNSmlDmMgmtTree::NewL()
	{
	CNSmlDmMgmtTree* self = new (ELeave) CNSmlDmMgmtTree();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::ConstructL()
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::ConstructL()
	{
	iNodeListPtr = 0;
	iVerDTD.DataL ( KNSmlDmTreeDTDVersion );
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::CNSmlDmMgmtTree()
// ---------------------------------------------------------------------------
CNSmlDmMgmtTree::CNSmlDmMgmtTree()
	{
	}


// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::~CNSmlDmMgmtTree()
// ---------------------------------------------------------------------------
CNSmlDmMgmtTree::~CNSmlDmMgmtTree()
	{
	CNSmlDmNodeList* ptr = iNodeListPtr;
	while(iNodeListPtr!=0)
		{
		delete iNodeListPtr->iNode;
		iNodeListPtr = iNodeListPtr->iNext;
		delete ptr;
		ptr = iNodeListPtr; 
		}
	}


// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetAccessTypesL( TSmlDmAccessTypes aAccessTypes )
// set accestypes for root node
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetAccessTypesL( TSmlDmAccessTypes /*aAccessTypes*/ )
	{
	//root node cannot be changed
	}



// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetDefaultValueL( const TDesC8& aDefaultValue )
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetDefaultValueL( const TDesC8& /*aDefaultValue*/ )
	{
	//root node cannot be changed
	}


// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetDescriptionL( const TDesC8& aDescription )
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetDescriptionL( const TDesC8& /*aDescription*/ )
	{
	//root node cannot be changed
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetDFFormatL( TDFFormat /*aFormat*/ )
	{
	//root node cannot be changed
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetOccurenceL( TOccurence aOccurence )
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetOccurenceL( TOccurence /*aOccurence*/ )
	{
	//root node cannot be changed
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetScopeL( TScope aScope )
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetScopeL( TScope /*aScope*/ )
	{
	//root node cannot be changed
	}


// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetDFTitleL( const TDesC8& aTitle )
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetDFTitleL( const TDesC8& /*aTitle*/ )
	{
	//root node cannot be changed
	}


// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::AddDFTypeMimeTypeL( const TDesC8& aMimeType )
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::AddDFTypeMimeTypeL( const TDesC8& /*aMimeType*/ )
	{
	//root node cannot be changed
	}


// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetAsObjectGroup()
// ---------------------------------------------------------------------------
void CNSmlDmMgmtTree::SetAsObjectGroup()
	{
	//root node cannot be changed
	}



// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::AddChildObjectL(const TDesC8& aNodeName)
// add child under root node
// ---------------------------------------------------------------------------
MSmlDmDDFObject& CNSmlDmMgmtTree::AddChildObjectL(const TDesC8& aNodeName)
	{
	CNSmlDmNode* newNode;

	if(iNodeListPtr==0)
		{
		newNode = new (ELeave) CNSmlDmNode();
		CleanupStack::PushL(newNode);
		iNodeListPtr = new (ELeave) CNSmlDmNodeList();
		CleanupStack::Pop(); // newNode
		iNodeListPtr->iNext = 0;
		iNodeListPtr->iNode = newNode;
		newNode->iStaticLevel=ETrue;
		newNode->iNodeName.DataL(aNodeName);
		}
	else
		{
		CNSmlDmNodeList* tmpNode = iNodeListPtr;
		CNSmlDmNodeList* tmpNodePrev = iNodeListPtr;
		TBool found=EFalse;

		//check if nodename already exists
		while(tmpNode!=0)
			{
			if(tmpNode->iNode->iNodeName.Compare ( aNodeName.Ptr(),
				aNodeName.Length() ) )
				{
				found = ETrue;
				break;
				}
			tmpNodePrev = tmpNode;
			tmpNode = tmpNode->iNext;
			}

		if(found)
			{
			//nodename already exist, new node is not created and pointer
			//to existing is returned
			newNode = tmpNode->iNode;
			}
		else
			{
			//create new node
			tmpNodePrev->iNext = new (ELeave) CNSmlDmNodeList();
			newNode = new (ELeave) CNSmlDmNode();
			tmpNode = tmpNodePrev->iNext;
			tmpNode->iNext = 0;
			tmpNode->iNode = newNode;
			newNode->iStaticLevel=ETrue;
			newNode->iNodeName.DataL(aNodeName);
			}
		}
	return *newNode;	
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::AddChildObjectGroupL()
// add child under root node
// ---------------------------------------------------------------------------
	
MSmlDmDDFObject& CNSmlDmMgmtTree::AddChildObjectGroupL()
	{
	User::Leave(KErrAccessDenied);
	CNSmlDmNode* newNode=0;
	return *newNode;	
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::GenerateXMLL(CBufBase& aObject)
// Generate xml from ddf structure
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmMgmtTree::GenerateXMLL(CBufBase& aObject)
	{
	CBufBase* man = CBufFlat::NewL(8);
	CleanupStack::PushL(man);
	CBufBase* mod = CBufFlat::NewL(8);
	CleanupStack::PushL(mod);
	
	aObject.InsertL(aObject.Size(),KNSmlDDFMgmtTree);
	aObject.InsertL(aObject.Size(),KNSmlDDFVerDTD);
	aObject.InsertL(aObject.Size(),iVerDTD.iContent, iVerDTD.iLength );
	aObject.InsertL(aObject.Size(),KNSmlDDFVerDTDEnd);
		
	//man
	aObject.InsertL(aObject.Size(),KNSmlDDFMan);
	aObject.InsertL(aObject.Size(),man->Ptr(0));
	aObject.InsertL(aObject.Size(),KNSmlDDFManEnd);

	//mod
	aObject.InsertL(aObject.Size(),KNSmlDDFMod);
	aObject.InsertL(aObject.Size(),mod->Ptr(0));
	aObject.InsertL(aObject.Size(),KNSmlDDFModEnd);
	CleanupStack::PopAndDestroy(2); //man,mod

	if(iNodeListPtr!=0)
		{
		iNodeListPtr->GenerateNodeListL(aObject);
		}
	aObject.InsertL(aObject.Size(),KNSmlDDFMgmtTreeEnd);
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::GenerateWBXMLL(CBufBase& aObject)
// Generate wbxml from ddf structure
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmMgmtTree::GenerateWBXMLL(RWriteStream& aStream)
	{
	aStream.WriteUint8L(EDDFMgmtTree);
	aStream.WriteUint8L(EDDFVerDTD);
	aStream.WriteUint8L(iVerDTD.iLength);
	aStream.WriteL(static_cast<TUint8*>(iVerDTD.iContent), iVerDTD.iLength);
	aStream.WriteUint8L(EDDFVerDTDEnd);

	if(iNodeListPtr!=0)
		{
		iNodeListPtr->GenerateNodeListWBXMLL(aStream);
		}

	aStream.WriteUint8L(EDDFMgmtTreeEnd);
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::ParseWBXMLL(CBufBase& aObject)
// Parse wbxml to ddf structure
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmMgmtTree::ParseWBXMLL(RReadStream& aReader)
	{
	//Mgmttree; ignore
	aReader.ReadUint8L();
	//VerDTD; ignore
	aReader.ReadUint8L();
	// verDTD length
	TInt len = aReader.ReadUint8L();
	// ..and value
	//TDes8& aDes
	HBufC8* verData = HBufC8::NewLC( len );
	TPtr8 verDataPtr = verData->Des();
	aReader.ReadL(verDataPtr, len);
	//Append the verDTD to new DDF
	CleanupStack::PopAndDestroy(); //verData
	// end verDTD, ignore
	aReader.ReadUint8L();	
	
	
	CNSmlDmNodeList* nodeListPtr;
	TBool first=EFalse;

	if(iNodeListPtr==0)
		{
		iNodeListPtr = new (ELeave) CNSmlDmNodeList();
		nodeListPtr=iNodeListPtr;
		first=ETrue;
		}
	else
		{
		nodeListPtr=iNodeListPtr;
		while(nodeListPtr->iNext)
			{
			nodeListPtr=nodeListPtr->iNext;					
			}
		nodeListPtr->iNext = new (ELeave) CNSmlDmNodeList();
		}
	if(first)
		{
	  	iNodeListPtr->ParseNodeListWBXMLL(aReader,iNodeListPtr, NULL );
		}
	else
		{
	  	iNodeListPtr->ParseNodeListWBXMLL(aReader,nodeListPtr->iNext, NULL );
		}
	}



// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetUid(TUint32 aUid)
// Sets adapter uid to tree
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmMgmtTree::SetAdapterUid(TUint32 aUid)
	{
	SetUidToNewItems(iNodeListPtr,aUid);
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetServerId(TUint8 aServerId)
// Sets host server id (host session id) to tree
// ---------------------------------------------------------------------------
EXPORT_C void CNSmlDmMgmtTree::SetServerId(TUint8 aServerId)
	{
	SetServerIdToNewItems(iNodeListPtr,aServerId);
	}



// ---------------------------------------------------------------------------
// TNSmlDmDDFFormatt CNSmlDmDDF::FindNameFromNodeListL()
// Recursive function for searching certain uri segment from ddf structure. 
// ---------------------------------------------------------------------------
EXPORT_C TNSmlDmDDFFormat CNSmlDmMgmtTree::FindNameFromNodeListL(
	const TDesC8& aURI,CNSmlDmNodeList*& aFinalPtr)
	{
	TNSmlDmDDFFormat ret = ENSmlDmDDFNotFound;

	CNSmlDmNodeList* nodeListPtr = iNodeListPtr;
	TInt numOfUriSegs=NSmlDmURI::NumOfURISegs(aURI);
	
	TInt level=1;


	while(nodeListPtr!=0)
		{
		TPtrC8 nodeNamePtr = 
			TPtrC8((TUint8*)nodeListPtr->iNode->iNodeName.iContent,
			nodeListPtr->iNode->iNodeName.iLength);
		

		//check if we are in correct level
		if(level>=numOfUriSegs)
			{
			if( nodeNamePtr.Compare(NSmlDmURI::URISeg(aURI,level)) == 0 ||
				nodeNamePtr.Length()==0)
				{
				if(nodeListPtr->iNode->iNodeListPtr!=0)
					{
					ret = ENSmlDmDDFNode;
					}
				else
					{
					ret = ENSmlDmDDFLeaf;
					}
				aFinalPtr = nodeListPtr;
				break;
				}
			else
				{
				nodeListPtr = nodeListPtr->iNext;
				}
			}
		else
			{
			//not yet in final level, if correct nodename found, this function is called recursively
			if( nodeNamePtr.Compare(NSmlDmURI::URISeg(aURI,level)) == 0 ||
				nodeNamePtr.Length()==0)
				{
				nodeListPtr=nodeListPtr->iNode->iNodeListPtr;
				level++;
				}
			else
				{
				nodeListPtr = nodeListPtr->iNext;
				}
			}
		} // end while

	return ret;
	}


// ---------------------------------------------------------------------------
// TBool CNSmlDmDDF::FindLoadedAdaptersL()
// Finds the uids of allready loaded adapters
// ---------------------------------------------------------------------------
EXPORT_C TBool CNSmlDmMgmtTree::FindLoadedAdaptersL(
	RArray<TUint32>& aLoadedAdapters,
	CNSmlDmNodeList* aNodeListPtr)
	{
	CNSmlDmNodeList* nodeListPtr;
	if(aNodeListPtr)
		{
		nodeListPtr = aNodeListPtr;
		}
	else
		{
		nodeListPtr = iNodeListPtr;
		}
	while(nodeListPtr!=0)
		{
		if(nodeListPtr->iNode)
			{
			if(nodeListPtr->iNode->iImplUid)
				{
				TBool found(EFalse);
				for(TInt i = 0;i<aLoadedAdapters.Count();i++)
					{
					if(aLoadedAdapters[i]==nodeListPtr->iNode->iImplUid)
						{
						found = ETrue;
						break;
						}
					}
				if(!found)
					{
					aLoadedAdapters.AppendL(nodeListPtr->iNode->iImplUid);
					}
				}
			if(nodeListPtr->iNode->iNodeListPtr)
				{
				FindLoadedAdaptersL(aLoadedAdapters,
					nodeListPtr->iNode->iNodeListPtr);
				}
			}
		nodeListPtr=nodeListPtr->iNext;
		} // end while
	return EFalse;
	}




// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetUidToNewItems
// Set adapter id to new items in tree
// ---------------------------------------------------------------------------
TBool CNSmlDmMgmtTree::SetUidToNewItems ( CNSmlDmNodeList* aNodeListPtr,
	TUint32 aUid)
	{
	CNSmlDmNodeList* nodeListPtr = aNodeListPtr;
	while ( nodeListPtr )
		{
		if ( nodeListPtr->iNode->iStaticLevel)
			{
			if(nodeListPtr->iNode->iParent)
				{
				nodeListPtr->iNode->iParent->iImplUid=0;
				}
			if(nodeListPtr->iNode->iImplUid==0 )
				{
				nodeListPtr->iNode->iImplUid = aUid;
				SetUidToNewItems ( nodeListPtr->iNode->iNodeListPtr, aUid );
				}
			}
		nodeListPtr =nodeListPtr->iNext;
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::SetServerIdToNewItems
// Set session id to new items in tree
// ---------------------------------------------------------------------------
TBool CNSmlDmMgmtTree::SetServerIdToNewItems ( CNSmlDmNodeList* aNodeListPtr,
	TUint8 aServerId)
	{
	CNSmlDmNodeList* nodeListPtr = aNodeListPtr;
	while ( nodeListPtr )
		{
		if ( nodeListPtr->iNode->iStaticLevel )
			{
			if(nodeListPtr->iNode->iParent)
				{
				nodeListPtr->iNode->iParent->iHostServerId=0;
				}
			if(nodeListPtr->iNode->iHostServerId==0 )
				{
				nodeListPtr->iNode->iHostServerId = aServerId;
				SetServerIdToNewItems ( nodeListPtr->iNode->iNodeListPtr, aServerId );
				}
			}
		nodeListPtr = nodeListPtr->iNext;
		}
	return EFalse;
	}

// ---------------------------------------------------------------------------
// CNSmlDmMgmtTree::NodeListPtr()
// ---------------------------------------------------------------------------
EXPORT_C CNSmlDmNodeList* CNSmlDmMgmtTree::NodeListPtr()
	{
	return iNodeListPtr;
	}


// ===========================================================================
// CNSmlDmDFProperties
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmDFProperties::CNSmlDmDFProperties()
// ---------------------------------------------------------------------------
CNSmlDmDFProperties::CNSmlDmDFProperties()
	{
	iOccurrence = MSmlDmDDFObject::EOne;
	iScope = MSmlDmDDFObject::EDynamic;
	iDFFormat = MSmlDmDDFObject::ENode;
	iAccessTypes = 0;	
	}


// ---------------------------------------------------------------------------
// CNSmlDmDFProperties::~CNSmlDmDFProperties()
// ---------------------------------------------------------------------------
CNSmlDmDFProperties::~CNSmlDmDFProperties()
	{
	delete iDefaultValue;
	delete iDescription;
	delete iDFTitle;
	delete iDFType;
	}


// ---------------------------------------------------------------------------
// CNSmlDmDFProperties::GenerateDFPropertiesL(CBufBase& aObject)
// generate xml from df properties
// ---------------------------------------------------------------------------
void CNSmlDmDFProperties::GenerateDFPropertiesL(CBufBase& aObject)
	{
	aObject.InsertL(aObject.Size(),KNSmlDDFDFProperties);
	aObject.InsertL(aObject.Size(),KNSmlDDFAccessType);
	for(TUint8 i = 0; i<6;i++)
		{
		TUint8 acl = TUint8(0x1<<i);
		if(acl&iAccessTypes)
			{
			switch (acl)
				{
				case TSmlDmAccessTypes::EAccessType_Add:
					{
					aObject.InsertL(aObject.Size(),KNSmlDDFAccessTypeAdd);
					break;
					}
				case TSmlDmAccessTypes::EAccessType_Copy:
					{
					aObject.InsertL(aObject.Size(),KNSmlDDFAccessTypeCopy);
					break;
					}
				case TSmlDmAccessTypes::EAccessType_Delete:
					{
					aObject.InsertL(aObject.Size(),KNSmlDDFAccessTypeDelete);
					break;
					}
				case TSmlDmAccessTypes::EAccessType_Exec:
					{
					aObject.InsertL(aObject.Size(),KNSmlDDFAccessTypeExec);
					break;
					}
				case TSmlDmAccessTypes::EAccessType_Get:
					{
					aObject.InsertL(aObject.Size(),KNSmlDDFAccessTypeGet);
					break;
					}
				case TSmlDmAccessTypes::EAccessType_Replace:
					{
					aObject.InsertL(aObject.Size(),KNSmlDDFAccessTypeReplace);
					break;
				default:
					User::Panic(KNSmlTreeDTDPanic,KErrArgument);
					break;
					}
				} //end switch
			} //end if(acl&AccessTypes)
		}
	aObject.InsertL(aObject.Size(),KNSmlDDFAccessTypeEnd);
	FeatureManager::InitializeLibL();				
	GenerateDFPropsSubmethodL(aObject); // To reduce cyclomatic complexity
	aObject.InsertL(aObject.Size(),KNSmlDDFOccurrence);
	switch( iOccurrence )
		{
		case MSmlDmDDFObject::EOne:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFOccurrenceOne);
			break;
			}
		case MSmlDmDDFObject::EZeroOrOne:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFOccurrenceZeroOrOne);
			break;
			}
		case MSmlDmDDFObject::EZeroOrMore:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFOccurrenceZeroOrMore);
			break;
			}
		case MSmlDmDDFObject::EOneOrMore:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFOccurrenceOneOrMore);
			break;
			}
		case MSmlDmDDFObject::EZeroOrN:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFOccurrenceZeroOrN);
			break;
			}
		case MSmlDmDDFObject::EOneOrN:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFOccurrenceOneOrN);
			break;
		default:
			User::Panic(KNSmlTreeDTDPanic,KErrArgument);
			break;
			}
		}
	aObject.InsertL(aObject.Size(),KNSmlDDFOccurrenceEnd);
	aObject.InsertL(aObject.Size(),KNSmlDDFScope);
	switch( iScope )
		{
		case MSmlDmDDFObject::EPermanent:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFScopePermanent);
			break;
			}
		case MSmlDmDDFObject::EDynamic:
			{
			aObject.InsertL(aObject.Size(),KNSmlDDFScopeDynamic);
			break;
			}
		default:
			User::Panic(KNSmlTreeDTDPanic,KErrArgument);
			break;
		}
	aObject.InsertL(aObject.Size(),KNSmlDDFScopeEnd);
	if(iDFTitle!=0)
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFDFTitle);
		aObject.InsertL(aObject.Size(),iDFTitle->iContent,iDFTitle->iLength);
		aObject.InsertL(aObject.Size(),KNSmlDDFDFTitleEnd);
		}
	if(iDFType!=0)
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFDFType);
		aObject.InsertL(aObject.Size(),KNSmlDDFMIME);
		aObject.InsertL(aObject.Size(),iDFType->iContent,iDFType->iLength);
		aObject.InsertL(aObject.Size(),KNSmlDDFMIMEEnd);
		aObject.InsertL(aObject.Size(),KNSmlDDFDFTypeEnd);
		}

	aObject.InsertL(aObject.Size(),KNSmlDDFDFPropertiesEnd);
	
	FeatureManager::UnInitializeLib();
	}

// ---------------------------------------------------------------------------
// CNSmlDmDFProperties::GenerateDFPropertiesWBXMLL(TPtr8& aObject)
// generate wbxml from df properties
// ---------------------------------------------------------------------------
void CNSmlDmDFProperties::GenerateDFPropertiesWBXMLL(RWriteStream& aStream)
	{
	aStream.WriteUint8L(EDDFDFProperties);
	aStream.WriteUint8L(EDDFAccessType);
	aStream.WriteUint8L(iAccessTypes);
	aStream.WriteUint8L(EDDFAccessTypeEnd);
	if(iDefaultValue != 0)
		{
		aStream.WriteUint8L(EDDFDefaultValue);
		aStream.WriteUint16L(iDefaultValue->iLength);
		aStream.WriteL(static_cast<TUint8*>(iDefaultValue->iContent),
			iDefaultValue->iLength);
		aStream.WriteUint8L(EDDFDefaultValueEnd);
		}
	if(iDescription != 0)
		{
		aStream.WriteUint8L(EDDFDescription);
		aStream.WriteUint16L(iDescription->iLength);
		aStream.WriteL(static_cast<TUint8*>(iDescription->iContent),
			iDescription->iLength);
		aStream.WriteUint8L(EDDFDescriptionEnd);
		}

	aStream.WriteUint8L(EDDFDFFormat);
	aStream.WriteUint8L(iDFFormat);

	aStream.WriteUint8L(EDDFDFFormatEnd);
	aStream.WriteUint8L(EDDFOccurrence);
	aStream.WriteUint8L( iOccurrence );
	
	aStream.WriteUint8L(EDDFOccurrenceEnd);
	aStream.WriteUint8L(EDDFScope);
	aStream.WriteUint8L( iScope );
	
	aStream.WriteUint8L(EDDFScopeEnd);
	
	if(iDFTitle!=0)
		{
		aStream.WriteUint8L(EDDFDFTitle);
		aStream.WriteUint8L(iDFTitle->iLength);
		aStream.WriteL(static_cast<TUint8*>(iDFTitle->iContent),
			iDFTitle->iLength);
			
		aStream.WriteUint8L(EDDFDFTitleEnd);
		}
	if(iDFType!=0)
		{
		aStream.WriteUint8L(EDDFDFType);
		aStream.WriteUint8L(EDDFMIME);
		aStream.WriteUint8L(iDFType->iLength);
		aStream.WriteL(static_cast<TUint8*>(iDFType->iContent),
			iDFType->iLength);
		aStream.WriteUint8L(EDDFMIMEEnd);
		aStream.WriteUint8L(EDDFDFTypeEnd);
		}
	aStream.WriteUint8L(EDDFDFPropertiesEnd);
	}


// ---------------------------------------------------------------------------
// CNSmlDmDFProperties::ParseDFPropertiesWBXMLL(RFileReadStream& reader)
// Parses DF properties element to structure
// ---------------------------------------------------------------------------
void CNSmlDmNodeList::ParseDFPropertiesWBXMLL(RReadStream& aReader,
	MSmlDmDDFObject* aNodePtr )
	{
	TInt len;	
	//Accesstypes
	TUint tag = aReader.ReadUint8L();
	// get the Accesstypes value
	TSmlDmAccessTypes act;
	tag = aReader.ReadUint8L();
	Mem::Copy(&act, &tag, 1);
	aNodePtr->SetAccessTypesL( act );
	// Accesstypes End
	tag = aReader.ReadUint8L();

	// next tag
	do
		{
		tag = aReader.ReadUint8L();
		
		if(tag==EDDFDefaultValue)
			{
			len = aReader.ReadUint16L();
			//value
			HBufC8* data = HBufC8::NewLC( len );
			TPtr8 dataPtr = data->Des();
			aReader.ReadL(dataPtr, len);
			aNodePtr->SetDefaultValueL(dataPtr);
			CleanupStack::PopAndDestroy(1); //data
			// end defaultvalue
			tag = aReader.ReadUint8L();
			}
		else if(tag==EDDFDescription)
			{
			len = aReader.ReadUint16L();
			//value
			HBufC8* desData = HBufC8::NewLC( len );
			TPtr8 desDataPtr = desData->Des();
			aReader.ReadL(desDataPtr, len);
			aNodePtr->SetDescriptionL(desDataPtr);
			CleanupStack::PopAndDestroy(); //desData
			// end description
			tag = aReader.ReadUint8L();
			}
		else if(tag==EDDFDFFormat)
			{
		    aNodePtr->SetDFFormatL(MSmlDmDDFObject::TDFFormat(aReader.ReadUint8L()));
			// end format
			tag = aReader.ReadUint8L();
			}
		}while ((tag == EDDFDefaultValueEnd) || (tag == EDDFDescriptionEnd));
	
	//Occurrence
	tag = aReader.ReadUint8L();
	__ASSERT_DEBUG(tag == EDDFOccurrence,
		User::Panic(_L("Tag not EDDFOccurrence, tag:"),tag));
		
    aNodePtr->SetOccurenceL(MSmlDmDDFObject::TOccurence(aReader.ReadUint8L()));
	// end occurrence
	tag = aReader.ReadUint8L();
	
	//Scope
	tag = aReader.ReadUint8L();
    aNodePtr->SetScopeL(MSmlDmDDFObject::TScope(aReader.ReadUint8L()));
	// end occurrence
	tag = aReader.ReadUint8L();

	do
		{
		tag = aReader.ReadUint8L();
		
		if(tag==EDDFDFTitle)
			{
			//length
			len = aReader.ReadUint8L();
			//value
			HBufC8*	titleData = HBufC8::NewLC( len );
			TPtr8 titleDataPtr = titleData->Des();
			aReader.ReadL(titleDataPtr, len);
			
			aNodePtr->SetDFTitleL(titleDataPtr);
			CleanupStack::PopAndDestroy(); //titleData, titleData16
			// end description
			tag = aReader.ReadUint8L();
			}
		else if(tag==EDDFDFType)
			{
			tag = aReader.ReadUint8L();
			if ( tag == EDDFMIME )
				{	
				//length
				len = aReader.ReadUint8L();
				//value
				HBufC8* typeData = HBufC8::NewLC( len );
				TPtr8 typeDataPtr = typeData->Des();
				aReader.ReadL(typeDataPtr, len);
				
				aNodePtr->AddDFTypeMimeTypeL(typeDataPtr);
				CleanupStack::PopAndDestroy(); //data
				//end MIMETYPE
				tag = aReader.ReadUint8L();
				}
			// end DFType
			tag = aReader.ReadUint8L();
			
			}
		else if(tag==EDDFDFPropertiesEnd)
			{
			
			}
		}while ( tag != EDDFDFPropertiesEnd );
	}


// ===========================================================================
// CNSmlDmRTProperties
// ===========================================================================

// ---------------------------------------------------------------------------
// CNSmlDmRTProperties::CNSmlDmRTProperties()
// ---------------------------------------------------------------------------
CNSmlDmRTProperties::CNSmlDmRTProperties()
	{
	}

// ---------------------------------------------------------------------------
// CNSmlDmRTProperties::~CNSmlDmRTProperties()
// ---------------------------------------------------------------------------
CNSmlDmRTProperties::~CNSmlDmRTProperties()
	{
	delete iSize;
	delete iTitle;
	delete iTStamp;
	delete iType;
	delete iVerNo;
	}

// ---------------------------------------------------------------------------
// CNSmlDmRTProperties::GenerateDFPropsSubmethodL(CBufBase& aObject)
// ---------------------------------------------------------------------------
void CNSmlDmDFProperties::GenerateDFPropsSubmethodL(CBufBase& aObject)
	{
	if(iDefaultValue != 0)
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFDefaultValue);
		aObject.InsertL(aObject.Size(),iDefaultValue->iContent,
			iDefaultValue->iLength);
			
		aObject.InsertL(aObject.Size(),KNSmlDDFDefaultValueEnd);
		}
	if(iDescription != 0)
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFDescription);
		aObject.InsertL(aObject.Size(),iDescription->iContent,
			iDescription->iLength);
			
		aObject.InsertL(aObject.Size(),KNSmlDDFDescriptionEnd);
		}

	aObject.InsertL(aObject.Size(),KNSmlDDFDFFormat);
	switch( iDFFormat )
	{
	case MSmlDmDDFObject::EB64:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatB64);
		break;
		}
	case MSmlDmDDFObject::EBool:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatBool);
		break;
		}
	case MSmlDmDDFObject::EChr:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatChr);
		break;
		}
	case MSmlDmDDFObject::EInt:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatInt);
		break;
		}
	case MSmlDmDDFObject::ENode:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatNode);
		break;
		}
	case MSmlDmDDFObject::ENull:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatNull);
		break;
		}
	case MSmlDmDDFObject::EXml:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatXml);
		break;
		}
	case MSmlDmDDFObject::EBin:
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatBin);
		break;
		}
	
	case MSmlDmDDFObject::EDate:
		{
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatDate);	
		}
		else
		{
		User::Panic(KNSmlTreeDTDPanic,KErrArgument);
		}
		break;
		}
	case MSmlDmDDFObject::ETime:
		{
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatTime);
		}
		else
		{
		User::Panic(KNSmlTreeDTDPanic,KErrArgument);
		}
		break;
		}
	case MSmlDmDDFObject::EFloat:
		{
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
		aObject.InsertL(aObject.Size(),KNSmlDDFFormatFloat);
		}
		else
		{
		User::Panic(KNSmlTreeDTDPanic,KErrArgument);	
		}
		break;
		}
	default:
		User::Panic(KNSmlTreeDTDPanic,KErrArgument);
		break;
	}
	aObject.InsertL(aObject.Size(),KNSmlDDFDFFormatEnd);
	}
// End of file

