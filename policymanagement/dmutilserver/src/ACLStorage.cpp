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


// INCLUDES

#ifdef __TARM_SYMBIAN_CONVERGENCY
 #include <devman.h>
 #include <dmtreenode.h>
#else
 #include <nsmldmdbhandler.h>
#endif

#include "ACLStorage.h"
#include "debug.h"


// CONSTANTS

_LIT( KStorageFile, "C:\\private\\10207843\\aclnodes.ini");
_LIT( KStorageTempFile, "C:\\private\\10207843\\aclnodes.tmp");
_LIT( KStorageBackupFile, "C:\\private\\10207843\\aclnodes.bak");



#ifdef __TARM_SYMBIAN_CONVERGENCY
_LIT8(KNSmlDmAclAddEqual, "Add=");
_LIT8(KNSmlDmAclGetEqual, "Get=");
_LIT8(KNSmlDmAclDeleteEqual, "Delete=");
_LIT8(KNSmlDmAclReplaceEqual, "Replace=");
_LIT8(KNSmlDmAclSeparator, "&");
_LIT8(KNSmlDmAclExecEqual, "Exec=");
#else
 const TInt KStatusRef = 0;
 const TInt KResultRef = 1;
#endif



namespace
{
	TBool CompareCertInfos( const TCertInfo& aCertInfo1, const TCertInfo& aCertInfo2)
	{
	
		TBool retVal = ( aCertInfo1.iSerialNo == aCertInfo2.iSerialNo ) &&
					   ( aCertInfo1.iFingerprint == aCertInfo2.iFingerprint ) &&	
					   ( aCertInfo1.iIssuerDNInfo.iCommonName == aCertInfo2.iIssuerDNInfo.iCommonName ) &&	
					   ( aCertInfo1.iIssuerDNInfo.iCountry == aCertInfo2.iIssuerDNInfo.iCountry ) &&	
					   ( aCertInfo1.iIssuerDNInfo.iLocality == aCertInfo2.iIssuerDNInfo.iLocality ) &&	
					   ( aCertInfo1.iIssuerDNInfo.iOrganization == aCertInfo2.iIssuerDNInfo.iOrganization ) &&	
					   ( aCertInfo1.iIssuerDNInfo.iOrganizationUnit == aCertInfo2.iIssuerDNInfo.iOrganizationUnit );
		
		return retVal;
	}
	
	TBool CompareCommands( const CCommand& aCommand1, const CCommand& aCommand2)
	{
		TBool match = EFalse;
	
		if ( aCommand1.iCommand == aCommand2.iCommand)
		{
			if ( aCommand1.iServerId && *aCommand1.iServerId == *aCommand2.iServerId)
			{
				match = ETrue;	
			}
			else
			{
				//Compare certficates
				match = CompareCertInfos( aCommand1.iCertInfo, aCommand2.iCertInfo); 
			}
		}
		
		return match;
	}	
}



// ----------------------------------------------------------------------------------------
// CCommand::NewL()
// ----------------------------------------------------------------------------------------
CCommand* CCommand::NewL( const TCertInfo& aCertInfo, const TAclCommands& aCommand)
{
	CCommand * self = new (ELeave) CCommand();
	self->iCertInfo = aCertInfo;
	self->iCommand = aCommand;
	
	return self;
}


// ----------------------------------------------------------------------------------------
// CCommand::NewL()
// ----------------------------------------------------------------------------------------
CCommand* CCommand::NewL( const TDesC8& aServerUid, const TAclCommands& aCommand)
{
	CCommand * self = new (ELeave) CCommand();
	
	CleanupStack::PushL( self);	
	self->iServerId = aServerUid.AllocL();
	self->iCommand = aCommand;
	CleanupStack::Pop( self);	
	
	return self;	
}


// ----------------------------------------------------------------------------------------
// CCommand::NewL()
// ----------------------------------------------------------------------------------------

void CCommand::CommandString( TDes8& aStr)
{
	switch ( iCommand)
	{
		case EACLAdd:
			aStr.Append( KNSmlDmAclAddEqual);
		break;
		case EACLDelete:
			aStr.Append( KNSmlDmAclDeleteEqual);
		break;
		case EACLExec:
			aStr.Append( KNSmlDmAclExecEqual);
		break;
		case EACLGet:
			aStr.Append( KNSmlDmAclGetEqual);
		break;
		case EACLReplace:
			aStr.Append( KNSmlDmAclReplaceEqual);
		break;
		default:
		break;
	}
	
	aStr.Append( *iServerId);
}

// ----------------------------------------------------------------------------------------
// CCommand::Length()
// ----------------------------------------------------------------------------------------

TInt CCommand::Length()
{
	//Calculate serialized command length 
	TPckg<TCertInfo> certPck( iCertInfo);
	TPckg<TAclCommands> cmdPck( iCommand);
	
	return 	certPck.Length() + cmdPck.Length();
}

// ----------------------------------------------------------------------------------------
// CCommand::SaveStringL()
// ----------------------------------------------------------------------------------------
HBufC8 * CCommand::SaveStringL()
{
	//Create buffer for string
	HBufC8 * buf = HBufC8::NewL( Length());
	TPtr8 ptr = buf->Des();

	//create descriptor packs
	TPckg<TCertInfo> certPck( iCertInfo);
	TPckg<TAclCommands> cmdPck( iCommand);

	//add serialized data to buffer
	ptr.Append( certPck);
	ptr.Append( cmdPck);
	
	return buf;
}

// ----------------------------------------------------------------------------------------
// CCommand::LoadFromStringL()
// ----------------------------------------------------------------------------------------
CCommand * CCommand::LoadFromStringL( TPtrC8& aString)
{
	CCommand * command = new (ELeave) CCommand();
	TPtrC8 ptr( aString);

	//Certificate 
	TPckg<TCertInfo> certPck( command->iCertInfo);
	certPck.Copy( ptr.Ptr(), certPck.Length());
	ptr.Set( ptr.Mid( certPck.Length()));	

	//Commands
	TPckg<TAclCommands> cmdPck( command->iCommand);
	cmdPck.Copy( ptr.Ptr(), cmdPck.Length());
	ptr.Set( ptr.Mid( cmdPck.Length()));	
	
	return command;	
}




// ----------------------------------------------------------------------------------------
// CACLNode::CACLNode()
// ----------------------------------------------------------------------------------------
CACLNode::CACLNode()
	: iURI(0)
{	
}

// ----------------------------------------------------------------------------------------
// CACLNode::CACLNode()
// ----------------------------------------------------------------------------------------
CACLNode::~CACLNode()
{
	delete iURI;
	iCommands.Close();	
}


// ----------------------------------------------------------------------------------------
// CACLNode::CompareElements()
// ----------------------------------------------------------------------------------------

TInt CACLNode::CompareElements( CACLNode const& aNode1, CACLNode const& aNode2)
{
	RDEBUG("CACLNode::CompareElements()");
	RDEBUG8_2("	-> Node1 URI = %S", &aNode1.iURIPtr );
	RDEBUG8_2("	-> Node2 URI = %S", &aNode2.iURIPtr );
	TInt result = (aNode1.iURIPtr).Compare( aNode2.iURIPtr);
	RDEBUG_2("	-> comparison result = %d", result);
	return result;
}

// ----------------------------------------------------------------------------------------
// CACLNode::FindCorrespondingCommand()
// ----------------------------------------------------------------------------------------

CCommand* CACLNode::FindCorrespondingCommand( const CCommand* aCommand)
{
	CCommand* retVal = 0;

	//find corresponding command from command list
	for ( TInt i(0); i < iCommands.Count(); i++)
	{
		CCommand* command = iCommands[i];
		
		if ( CompareCommands( *command, *aCommand))
		{
			retVal = command;
			break;
		}
	}
	
	return retVal;
}


// ----------------------------------------------------------------------------------------
// CACLNode::GetNodeACLStringL()
// ----------------------------------------------------------------------------------------

HBufC8 * CACLNode::GetNodeACLStringL()
{
	RDEBUG("CACLNode::GetNodeACLStringL()");
	HBufC8 * buf = HBufC8::NewL( iCommands.Count() * 21);
	TPtr8 ptr = buf->Des();
	
	for ( TInt i(0); i < iCommands.Count();)
	{
		TBuf8<30> command;
	
		iCommands[i++]->CommandString( command);
		ptr.Append( command);
		
		if ( i != iCommands.Count() )
		{
			ptr.Append(KNSmlDmAclSeparator);
		}
	}
	
	
	return buf;
}


// ----------------------------------------------------------------------------------------
// CACLNode::SetURIL()
// ----------------------------------------------------------------------------------------

void CACLNode::SetURIL( const TDesC8& aURI )
{
	delete iURI;
	iURI = NULL;
	iURIPtr.Set( KNullDesC8);
	iURI = aURI.AllocL();
	iURIPtr.Set( *iURI);
}
		
// ----------------------------------------------------------------------------------------
// CACLNode::ClearACL()
// ----------------------------------------------------------------------------------------

void CACLNode::ClearACL()
{
	//find all command which concern specific command remove them from list
	for ( TInt i(0); i < iCommands.Count(); i++)
	{
#ifdef _DEBUG
			TBuf8<30> command;
			iCommands[i]->CommandString( command);
			RDEBUG8_2("DMUtilServer: Remove command %S", &command);
#endif //_DEBUG

		delete iCommands[i];
		iCommands.Remove( i--);
	}
}



// ----------------------------------------------------------------------------------------
// CACLNode::SetServerIdToACLL()
// ----------------------------------------------------------------------------------------


void CACLNode::SetServerIdToACLL( const TAclCommands& aCommandType, const TDesC8& aServerId)
{
	RDEBUG("CACLStorage::SetServerIdToACLL()");
	//check first that command isn't already exist
	for ( TInt i(0); i < iCommands.Count(); i++)
	{
		CCommand * command = iCommands[i];
		
		if ( (command->iCommand == aCommandType || aCommandType == EACLAll )
			 && *command->iServerId == aServerId)
		{
			return;
		}
	}
	
	//and if command is new, create new CCommand for it
	CCommand * command = CCommand::NewL( aServerId, aCommandType);

	CleanupStack::PushL( command);
	iCommands.AppendL( command);
	CleanupStack::Pop( command);
	
#ifdef _DEBUG
	TBuf8<30> commandStr;
	command->CommandString( commandStr);
	RDEBUG8_2("DMUtilServer: New command %S", &commandStr);
#endif //_DEBUG	
	
}

// ----------------------------------------------------------------------------------------
// CACLNode::Length()
// ----------------------------------------------------------------------------------------

TInt CACLNode::Length()
{
	TInt testvalue;
	TPckgC<TInt> valueLength( testvalue);
	TPckgC<TACLDestination> destLength( iDestination);
	
	TInt size = valueLength.Length() * 2 + destLength.Length() + iURI->Length();
	
	for (TInt i(0); i < iCommands.Count(); i++)
	{
		size += iCommands[i]->Length();
	}
	RDEBUG_2("DMUtilServer: CACLNode::Length - End (returned size: %d)", size);

	return size;
}

// ----------------------------------------------------------------------------------------
// CACLNode::SaveStringL()
// ----------------------------------------------------------------------------------------

HBufC8 * CACLNode::SaveStringL()
{
	RDEBUG("DMUtilServer: CACLNode::SaveStringL - Start");

	//create buffer for string
	HBufC8* buf = HBufC8::NewL( Length());
	TPtr8 ptr = buf->Des();

	//clearflag
	TPckgC<TInt> flagPck( iClearFlag);
	ptr.Append( flagPck);

	//destPack
	TPckgC<TACLDestination> destPck( iDestination);
	ptr.Append( destPck);
 
	//uri
	TInt uriLength( iURI->Length());
	TPckgC<TInt> uriPck( uriLength);
	ptr.Append( uriPck);
	ptr.Append( *iURI);

	//append commands to string
	for ( TInt i(0); i < iCommands.Count(); i++)
	{
		//get command save string
		HBufC8* buf = iCommands[i]->SaveStringL();
		ptr.Append( *buf);
		
		delete buf;
	}
	
	RDEBUG("DMUtilServer: CACLNode::SaveStringL - End");
	return buf;
}

// ----------------------------------------------------------------------------------------
// CACLNode::LoadFromStringL()
// ----------------------------------------------------------------------------------------

CACLNode * CACLNode::LoadFromStringL( TDes8& aString)
{
	RDEBUG("DMUtilServer: CACLNode::LoadFromStringL - Start");

	CACLNode * node = new (ELeave) CACLNode();
	TPtrC8 ptr( aString);

	//Clear flag
	TPckg<TInt> flagPck( node->iClearFlag);
	flagPck.Copy( ptr.Ptr(), flagPck.Length());
	ptr.Set( ptr.Mid( flagPck.Length()));	


	//destPack
	TPckg<TACLDestination> destPck( node->iDestination);
	destPck.Copy( ptr.Ptr(), destPck.Length());
	ptr.Set( ptr.Mid( destPck.Length()));	

	//get uri length
	TInt uriLength;
	TPckg<TInt> intPck( uriLength);
	intPck.Copy( ptr.Ptr(), intPck.Length());
	ptr.Set( ptr.Mid( intPck.Length()));	
	
	//get URI and save it to node
	node->iURI = HBufC8::NewL( uriLength);
	node->iURI->Des().Copy( ptr.Ptr(), uriLength);
	node->iURIPtr.Set( *(node->iURI));
	ptr.Set( ptr.Mid( uriLength));	

	TInt len( ptr.Length() );
	while ( len )
	{
		//Read buffer
		CCommand * command = CCommand::LoadFromStringL( ptr);
		
		//Set Pointer	
		ptr.Set( ptr.Mid( command->Length()));	
		len = ptr.Length();
		//append readen command to command list
		CleanupStack::PushL( command);
		node->iCommands.AppendL( command);
		CleanupStack::Pop( command);
	}	
	
	RDEBUG("DMUtilServer: CACLNode::LoadFromStringL - End");

	return node;
}


// ----------------------------------------------------------------------------------------
// CACLNode::SetCertificateToACLL()
// ----------------------------------------------------------------------------------------

void CACLNode::SetCertificateToACLL( const TACLDestination& aDestination, const TAclCommands& aCommandType, const TCertInfo& aCertInfo)
{
	RDEBUG("DMUtilServer: CACLNode::SetCertificateToACLL - Start");

	//check first that command isn't already exist
	for ( TInt i(0); i < iCommands.Count(); i++)
	{
		CCommand * command = iCommands[i];
		
		if ( (command->iCommand == aCommandType || aCommandType == EACLAll )
			 && CompareCertInfos( command->iCertInfo, aCertInfo ))
		{
			return;
		}
	}
	
	//and if command is new, create new CCommand for it
	CCommand * command = CCommand::NewL( aCertInfo, aCommandType);

	CleanupStack::PushL( command);
	iCommands.AppendL( command);
	CleanupStack::Pop( command);
	
	iDestination = aDestination;


	RDEBUG("DMUtilServer: CACLNode::SetCertificateToACLL - End");
}

// ----------------------------------------------------------------------------------------
// CACLNode::CreateACLL()
// ----------------------------------------------------------------------------------------

CACLNode * CACLNode::CreateACLL( const TDesC8& aACLString)
{
	RDEBUG("DMUtilServer: CACLNode::CreateACLL - Start");


	//Create ACL node
	CACLNode * node = new (ELeave) CACLNode();

	TPtrC8 ptr( aACLString);
	TInt err( KErrNone);
	
	//decode acl string
	while ( ptr.Length() && err == KErrNone)
	{
		//find equal mark...
		TInt index = ptr.Locate('=');
		
		//...acl is corrupted if mark not found
		if ( index == KErrNotFound)
		{
			err = KErrCorrupt;
			break;
		}
		
		//create command ptr, which includes only command string		
		TPtrC8 command( ptr.Left( index + 1));
		TAclCommands aclCommand;

		//remove encoded part
		ptr.Set( ptr.Mid( index + 1));
		
		//resolve command type
		if ( command.CompareF( KNSmlDmAclAddEqual) == 0)
		{
			aclCommand = EACLAdd;	
		}
		else
		if ( command.CompareF( KNSmlDmAclGetEqual)  == 0)
		{
			aclCommand = EACLGet;	
		}
		else
		if ( command.CompareF( KNSmlDmAclDeleteEqual)  == 0)
		{
			aclCommand = EACLDelete;	
		}
		else
		if ( command.CompareF( KNSmlDmAclReplaceEqual)  == 0)
		{
			aclCommand = EACLReplace;	
		}
		else
		if ( command.CompareF( KNSmlDmAclExecEqual)  == 0)
		{
			aclCommand = EACLExec;	
		}
		
		//find &-mark
		index = ptr.Locate('&');
		
		//server ptr contains server id 	
		TPtrC8 server(ptr);
		if ( index != KErrNotFound)
		{
			//set server text
			server.Set( ptr.Left( index));

			//remove encoded part
			ptr.Set( ptr.Mid( index + 1));
		}
		else
		{
			ptr.Set( KNullDesC8);
		}
		
		
		//create command add it to CNode
		if ( err == KErrNone)
		{
			CCommand * command = CCommand::NewL( server, aclCommand);
			CleanupStack::PushL( command);
			node->iCommands.AppendL( command);
			CleanupStack::Pop( command);	
		}
	}
	
	RDEBUG("DMUtilServer: CACLNode::CreateACLL - End");
	return node;	
}



// ----------------------------------------------------------------------------------------
// CACLStorage::CACLStorage()
// ----------------------------------------------------------------------------------------

CACLStorage::CACLStorage()
{
}

// ----------------------------------------------------------------------------------------
// CACLStorage::~CACLStorage()
// ----------------------------------------------------------------------------------------

CACLStorage::~CACLStorage()
{
	delete iCurrentServerId;
	iNodes.ResetAndDestroy();
#ifdef __TARM_SYMBIAN_CONVERGENCY
	// nothing
#else
	//iDbSession.Close();
#endif
}

// ----------------------------------------------------------------------------------------
// CACLStorage::NewL()()
// ----------------------------------------------------------------------------------------

CACLStorage* CACLStorage::NewL()
{
	CACLStorage * self =  new (ELeave) CACLStorage();
	
	CleanupStack::PushL( self);
	self->ConstructL();
	CleanupStack::Pop( self);
	
	return self;
}


// ----------------------------------------------------------------------------------------
// CACLStorage::ConstructL()()
// ----------------------------------------------------------------------------------------

void CACLStorage::ConstructL()
	{
	RDEBUG("CACLStorage::ConstructL()");
#ifdef __TARM_SYMBIAN_CONVERGENCY
	// nothing
#else

//	User::LeaveIfError( iDbSession.Connect() );
#endif
    iCertificateReceived = EFalse;
	LoadACLL();
	}


// ----------------------------------------------------------------------------------------
// CACLStorage::NewSession()
// ----------------------------------------------------------------------------------------
void CACLStorage::NewSessionL( const TCertInfo& aCertInfo, const TDesC8& aServerID)
{
	RDEBUG("=== CACLStorage::NewSessionL() ===");
	delete iCurrentServerId;
	iCurrentServerId = 0;
	iCurrentServerId = aServerID.AllocL();

	//set current session values
	SetCertInfo( aCertInfo );

	//update ACLs
	RDEBUG("==== Starting to update ACLs ==== ");
	UpdateACLsL();
	RDEBUG("==== Starting to update ACLs DONE ! ==== ");
}

// ----------------------------------------------------------------------------------------
// CACLStorage::CloseSession()
// ----------------------------------------------------------------------------------------
void CACLStorage::CloseSession( )
{
	delete iCurrentServerId;
	iCurrentServerId = 0;
	iCertificateReceived = EFalse;
}

// ----------------------------------------------------------------------------------------
// CACLStorage::CertInfo()
// ----------------------------------------------------------------------------------------
TInt CACLStorage::CertInfo( TCertInfo &aCertInfo )
{
    if( ! iCertificateReceived )
        {
        return KErrNotFound;
        }

    aCertInfo = iCurrentCertificate;
    return KErrNone;
}

// ----------------------------------------------------------------------------------------
// CACLStorage::SetCertInfo()
// ----------------------------------------------------------------------------------------
void CACLStorage::SetCertInfo( const TCertInfo &aCertInfo )
{   
    TPckg<TCertInfo> certa( aCertInfo );
    TPckg<TCertInfo> certb( iCurrentCertificate );
    certb.Copy( certa );

    iCertificateReceived = ETrue;
}

// ----------------------------------------------------------------------------------------
// CACLStorage::ServerIDL()
// ----------------------------------------------------------------------------------------
HBufC8* CACLStorage::ServerIDL()
    {
    if( iCurrentServerId != 0)
        {
        return iCurrentServerId->AllocL();
        }
    return 0;
    }

// ----------------------------------------------------------------------------------------
// CACLStorage::NewSession()
// ----------------------------------------------------------------------------------------

CACLNode * CACLStorage::FindNodeL( const TDesC8& aURI, TBool aCreateNewIfDoesntExist)
{
	RDEBUG8_2("CACLStorage::FindNodeL() (%S)", &aURI);
	//Create reference element with URI
	TLinearOrder<CACLNode> linearOrder( &CACLNode::CompareElements);
	CACLNode referenceNode;
	referenceNode.iURIPtr.Set( aURI);
	
	//find element index
	TInt index = iNodes.FindInOrder( &referenceNode, linearOrder);
	
	if ( index != KErrNotFound )
	{
		return iNodes[ index];
	}
	
	if ( aCreateNewIfDoesntExist )
	{
		CACLNode * newNode = new (ELeave) CACLNode();
		CleanupStack::PushL( newNode);
		newNode->SetURIL( aURI);
		iNodes.InsertInOrderL( newNode, linearOrder);
		CleanupStack::Pop( newNode);
		
		RDEBUG8_2("CACLStorage::FindNodeL() Node not found, created (%S)", &aURI);
		
		return newNode; 	
	}

	return 0;
}





// ----------------------------------------------------------------------------------------
// CACLStorage::NewSession()
// ----------------------------------------------------------------------------------------

TInt CACLStorage::RemoveNode( const TDesC8& aURI)
{
	RDEBUG8_2("CACLStorage::RemoveNode() (%S)", &aURI);
	//Create reference element with URI
	TLinearOrder<CACLNode> linearOrder( &CACLNode::CompareElements);
	CACLNode referenceNode;
	referenceNode.iURIPtr.Set( aURI);
	
	//find element index
	TInt index = iNodes.FindInOrder( &referenceNode, linearOrder);
	
	if ( index != KErrNotFound)
	{
		RDEBUG8_2("CACLStorage::Removed (%S)", &aURI);
		delete iNodes[ index];
		iNodes.Remove( index);
		
		index = KErrNone;
	}
	
	return index;	
}

// ----------------------------------------------------------------------------------------
// CACLStorage::MngSessionCertificate()
// ----------------------------------------------------------------------------------------

TCertInfo& CACLStorage::MngSessionCertificate()
{
	return iMngSessionCertificate;
}

// ----------------------------------------------------------------------------------------
// CACLStorage::RemoveACL()
// ----------------------------------------------------------------------------------------
TInt CACLStorage::RemoveACL( const TDesC8& aURI, TBool aRestoreDefaults)
{
	RDEBUG8_3("CACLStorage::RemoveACL() (%S)->(%d)", &aURI, aRestoreDefaults);
	TInt err = KErrNone;

	if( aRestoreDefaults )
		{
#ifdef __TARM_SYMBIAN_CONVERGENCY
TRAPD( trapErr, {
			// Symbian framework used
			RDmTree dmTreeSession;
			dmTreeSession.ConnectL();
			CleanupClosePushL( dmTreeSession );
			
			RDmTreeNode node;
			User::LeaveIfError( dmTreeSession.OpenNodeL( aURI, node, RDmTree::EReadWrite ) );
			CleanupClosePushL( node );

			// remove the node
			node.RemoveDmPropertyL( DevMan::EACL );
			node.Close();
			dmTreeSession.CommitL();
			
			CleanupStack::PopAndDestroy( &node );
			CleanupStack::PopAndDestroy( &dmTreeSession );
			} );
	err = trapErr;
			
#else
User::LeaveIfError( iDbSession.Connect() );
	iDbSession.UpdateAclL( aURI, KNullDesC8 );
	iDbSession.Close();
#endif
		
		
		}	


	if( err == KErrNone )
		{
		RemoveNode( aURI );
		}

	return err;
}

// ----------------------------------------------------------------------------------------
// CACLStorage::AddACLForNode()
// ----------------------------------------------------------------------------------------
void CACLStorage::AddACLForNodeL( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType)
{
	//find node from storage
	CACLNode * node = FindNodeL( aURI, ETrue);

	CleanupStack::PushL( node);
	node->SetCertificateToACLL( aDestination, aCommandType, iMngSessionCertificate);	
	CleanupStack::Pop( node);

	//incremental operation
	node->iClearFlag = EFalse;	
}

// ----------------------------------------------------------------------------------------
// CACLStorage::SetACLForNode()
// ----------------------------------------------------------------------------------------
void CACLStorage::SetACLForNodeL( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType)
{

	//find node from storage
	CACLNode * node = FindNodeL( aURI, ETrue);

	CleanupStack::PushL( node);
	node->SetCertificateToACLL( aDestination, aCommandType, iMngSessionCertificate);	
	CleanupStack::Pop( node);

	//non-incremental operation
	node->iClearFlag = ETrue;	
}




// ----------------------------------------------------------------------------------------
// CACLStorage::UpdateACLs()
// ----------------------------------------------------------------------------------------

void CACLStorage::UpdateACLsL()
{
	RDEBUG_2("	... starting to update ACL nodes, count: %d", iNodes.Count() );
	for ( TInt i(0); i < iNodes.Count(); i++)
	{
		UpdateACLL( iNodes[i]);
	}
}

// ----------------------------------------------------------------------------------------
// CACLStorage::UpdateACLs()
// ----------------------------------------------------------------------------------------

void CACLStorage::UpdateACLL( const CACLNode *aACLNode)
{
	RDEBUG_2("DMUtilServer: CACLStorage::UpdateACLL - Start (%d)", aACLNode->iDestination);

	if ( aACLNode->iDestination == EForNode )
	{
		UpdateACLL( aACLNode, aACLNode->iURIPtr);
	}
	else
	{	
		//Create child info object
		CChildInfo::RChildNodes childs;

		//childinfo owns "CChildInfo::RChildNodes childs"
		CChildInfo * childinfo = new (ELeave) CChildInfo( childs);
		CleanupStack::PushL( childinfo);
		
		//get childs
		childinfo->GetChildsL( aACLNode->iURIPtr);
		
		//update all childs nodes
		for ( TInt i(0); i < childs.Count(); i++)
		{
			UpdateACLL( aACLNode, childs[i]);
		}
		

		//update parent also if iDestination is EForBoth
		if ( aACLNode->iDestination == EForBoth)
		{
			UpdateACLL( aACLNode, aACLNode->iURIPtr);
		}

		//destroy child info
		CleanupStack::PopAndDestroy( childinfo);
	}

	RDEBUG("DMUtilServer: CACLStorage::UpdateACL - End");
}

// ----------------------------------------------------------------------------------------
// CACLStorage::UpdateACL()
// ----------------------------------------------------------------------------------------
void CACLStorage::UpdateACLL( const CACLNode *aACLNode, const TDesC8& aURI)
	{
	RDEBUG8_2("DMUtilServer: CACLStorage::UpdateACL - Start: %S", &aURI );
#ifdef __TARM_SYMBIAN_CONVERGENCY
	RDmTree dmTreeSession;
	dmTreeSession.ConnectL();
	CleanupClosePushL( dmTreeSession );

	RDmTreeNode node;
	User::LeaveIfError( dmTreeSession.OpenNodeL( aURI, node, RDmTree::EReadWrite ) );
	CleanupClosePushL( node );

	RBuf8 buf;
	//get old ACL values from dmdbtree
	node.DmPropertyL( DevMan::EACL, buf );
	
	RDEBUG8_3("DMUtilServer: Original ACL from DM tree node %S: %S", &aURI, &buf);
#else
	//get old ACL values from dmdbhandler
	CBufFlat* bufBase = CBufFlat::NewL( 1);
	User::LeaveIfError( iDbSession.Connect() );
	iDbSession.GetAclL( aURI, *bufBase, EFalse);
iDbSession.Close();

	TPtrC8 aclPtr = bufBase->Ptr( 0 );
	
	RDEBUG8_3("DMUtilServer: Original ACL from DM tree node %S: %S", &aURI, &aclPtr);
#endif

	

	//Create CACLNode from acl string
#ifdef __TARM_SYMBIAN_CONVERGENCY
	CACLNode * acl = CACLNode::CreateACLL( buf );	
#else
	CACLNode * acl = CACLNode::CreateACLL( aclPtr );	
#endif
	CleanupStack::PushL( acl);

	//clear original nodes
	if ( aACLNode->iClearFlag )
	{
		RDEBUG("DMUtilServer: Remove original ACL items");
		acl->ClearACL();
	}

	//copy commands to original ACL
	for ( TInt i(0); i < aACLNode->iCommands.Count(); i++)
	{
		CCommand * command = aACLNode->iCommands[i];

		CleanupStack::PushL( command);
		delete command->iServerId;
		command->iServerId = 0;
		command->iServerId = iCurrentServerId->AllocL();
		CleanupStack::Pop( command);

		//find corresponding command from original acl string
		CCommand * origCommand = acl->FindCorrespondingCommand( command);

		//if no correspondinding command exist and current session certificate is same
		//as in command, add command to node
		if ( !origCommand )
		{
			if ( CompareCertInfos( iCurrentCertificate, command->iCertInfo))
			{
				RDEBUG("DMUtilServer: Add new ACL item");
				acl->SetServerIdToACLL( command->iCommand, *iCurrentServerId);
			}
		}
		else
		{
			//remove all commands which have same server id than new session (except origCommand)
			for ( TInt i(0); i < acl->iCommands.Count(); i++)
			{
				if ( *iCurrentServerId == *acl->iCommands[i]->iServerId && acl->iCommands[i] != origCommand)
				{
					acl->iCommands.Remove(i--);
				}
			}
		}
	}

	//update ACL to CM tree
#ifdef __TARM_SYMBIAN_CONVERGENCY
	HBufC8* aclStr = acl->GetNodeACLStringL();
	RDEBUG_2("DMUtilServer: New ACL string for node: %S", aclStr);
	CleanupStack::PushL( aclStr );
	
	node.SetDmPropertyL( DevMan::EACL, *aclStr );

	node.Close();
	dmTreeSession.CommitL();
	
	CleanupStack::PopAndDestroy( aclStr);
	CleanupStack::PopAndDestroy( acl);
	
	CleanupStack::PopAndDestroy( &node );
	CleanupStack::PopAndDestroy( &dmTreeSession );
#else
	HBufC8 * aclStr = acl->GetNodeACLStringL();
	CleanupStack::PushL( aclStr);
	RDEBUG8_2("DMUtilServer: New ACL string for node: %S", aclStr);
	User::LeaveIfError( iDbSession.Connect() );
	iDbSession.UpdateAclL( aURI, *aclStr);
	iDbSession.Close();
	CleanupStack::PopAndDestroy( aclStr);

	CleanupStack::PopAndDestroy( acl);
#endif		
	RDEBUG("DMUtilServer: CACLStorage::UpdateACL - End");
	}


// ----------------------------------------------------------------------------------------
// CACLStorage::SaveACLL()
// ----------------------------------------------------------------------------------------

void CACLStorage::SaveACLL()
{
	RDEBUG("DMUtilServer: Save ACL configuration!");

	//Open rfs
	RFs rfs;
	CleanupClosePushL( rfs);
	User::LeaveIfError( rfs.Connect());
	
	//Open rfile
	RFile file;
	CleanupClosePushL( file);
	
	TInt err = file.Open( rfs, KStorageTempFile, EFileWrite);
	
	if ( err != KErrNone)
	{
		if ( err == KErrPathNotFound)
		{
			rfs.CreatePrivatePath( EDriveC);
		}
		
		err = file.Create( rfs, KStorageTempFile, EFileWrite);
	}
	
	User::LeaveIfError( err);

	TInt size = 0;

	//write all nodes
	for ( TInt i(0); i < iNodes.Count(); i++)
	{
		RDEBUG_2("		-> node %d", i );
		//write node length to file
		TInt nodeLength( iNodes[i]->Length());
		TPckgC<TInt> pck( nodeLength);
		User::LeaveIfError( file.Write( pck));
		size += pck.Length();
		size += nodeLength;
		
		HBufC8 * saveString = iNodes[i]->SaveStringL();
		CleanupStack::PushL( saveString); 
		User::LeaveIfError( file.Write( *saveString));
		CleanupStack::PopAndDestroy( saveString); 
	}
	
	//Set file size
	file.SetSize( size);

	CleanupStack::PopAndDestroy( &file);
	
	//Create storage file
	CFileMan * fileMan = CFileMan::NewL( rfs);
	CleanupStack::PushL( fileMan);
	
	err = file.Open( rfs, KStorageFile, EFileRead );
	
	if ( err == KErrNone )
	{
		file.Close();	
		User::LeaveIfError( fileMan->Rename( KStorageFile, KStorageBackupFile, CFileMan::EOverWrite));
	}
	
	User::LeaveIfError( fileMan->Rename( KStorageTempFile, KStorageFile, CFileMan::EOverWrite));
	
	if ( err == KErrNone )
	{
		User::LeaveIfError( fileMan->Delete( KStorageBackupFile));
	}
	

	CleanupStack::PopAndDestroy( fileMan);
	CleanupStack::PopAndDestroy( &rfs);
}

// ----------------------------------------------------------------------------------------
// CACLStorage::LoadACLL()
// ----------------------------------------------------------------------------------------

void CACLStorage::LoadACLL()
{
	RDEBUG("DMUtilServer: Load ACL configuration!");

	//Open rfs
	RFs rfs;
	User::LeaveIfError( rfs.Connect());
	CleanupClosePushL( rfs);
	
	//Open rfile
	RFile file;
	CleanupClosePushL( file);
	
	TInt err = file.Open( rfs, KStorageBackupFile, EFileRead);
	file.Close();
	
	//backup file exist, error in previois save operation, restore backup
	if ( err == KErrNone)
	{
		CFileMan * fileMan = CFileMan::NewL( rfs);
		CleanupStack::PushL( fileMan);
		
		User::LeaveIfError( fileMan->Rename( KStorageBackupFile, KStorageFile, CFileMan::EOverWrite));
		
		CleanupStack::PopAndDestroy( fileMan);
	}
	
	//open proper storage file
	err = file.Open( rfs, KStorageFile, EFileRead);	
	
	if ( err == KErrNone)
	{
		TInt size(0);
		User::LeaveIfError( file.Size( size));
	
		while ( size > 0)
		{
			//read next node size
			TInt nodeSize(0);
			TPckg<TInt> pckg(nodeSize);
			User::LeaveIfError( file.Read( pckg));
		
			//decrase file remaining index
			size -= nodeSize;
			size -= pckg.Length();
		
			//create buffer for next node string and read string
			HBufC8 * nodeStr = HBufC8::NewLC( nodeSize);
			TPtr8 ptr = nodeStr->Des();
			User::LeaveIfError( file.Read( ptr, nodeSize));
			
			//create node from string and append it string list
			CACLNode * node = CACLNode::LoadFromStringL( ptr);
			CleanupStack::PushL( node);
			iNodes.AppendL( node);
		
			CleanupStack::Pop( node);
			CleanupStack::PopAndDestroy( nodeStr);
		}
	}		
		
	CleanupStack::PopAndDestroy( 2, &rfs);
}




// ----------------------------------------------------------------------------------------
// CChildInfo::CChildInfo()
// ----------------------------------------------------------------------------------------

CChildInfo::CChildInfo( RChildNodes& aNodes)
	: iChilds( aNodes)
{
	
}

// ----------------------------------------------------------------------------------------
// CChildInfo::~CChildInfo()
// ----------------------------------------------------------------------------------------

CChildInfo::~CChildInfo()
	{
	iChilds.Close();
#ifdef __TARM_SYMBIAN_CONVERGENCY
 // nothing
#else
 delete iBuffer;
#endif
	}

// ----------------------------------------------------------------------------------------
// CChildInfo::GetChildsL()
// ----------------------------------------------------------------------------------------


TInt CChildInfo::GetChildsL( const TDesC8& aURI )
	{
	RDEBUG8_2("DMUtilServer: CChildInfo::GetChildsL - Start (%S)", &aURI);
#ifdef __TARM_SYMBIAN_CONVERGENCY
	RDmTree dmTreeSession;
	dmTreeSession.ConnectL();
	CleanupClosePushL( dmTreeSession );

	CDesC8ArrayFlat* buf = new( ELeave ) CDesC8ArrayFlat( 4 );
	CleanupStack::PushL( buf );

	dmTreeSession.ChildrenL( aURI, *buf );
	
	// copy, should be fixed so that this step is not needed
	for( TInt i = 0; i < buf->Count(); i++ )
		{
		iChilds.AppendL( ( *buf)[ i ] );
		}
	
	CleanupStack::PopAndDestroy( buf );
	CleanupStack::PopAndDestroy( &dmTreeSession );
		
	
	RDEBUG("DMUtilServer: CChildInfo::GetChildsL - End");
	return KErrNone;
#else
	//create dmmodule
	CNSmlDmModule* dmmodule = CNSmlDmModule::NewL( this); 
	CleanupStack::PushL( dmmodule);
	
	//fetch objects...
	dmmodule->FetchObjectL(  aURI, KNullDesC8, KResultRef, KStatusRef, ETrue);
	
	CleanupStack::PopAndDestroy( dmmodule);
	
	//Check errors
	TInt error( KErrNone);
	if ( iErr )
	{
		error = KErrGeneral;
	}
	
	
	RDEBUG("DMUtilServer: CChildInfo::GetChildsL - End");
	return error;
#endif
	}






#ifdef __TARM_SYMBIAN_CONVERGENCY
 // nothing
#else



// ----------------------------------------------------------------------------------------
// CChildInfo::SetResultsL()
// ----------------------------------------------------------------------------------------

void CChildInfo::SetResultsL( TInt aResultsRef,
		const CBufBase& aObject,
		const TDesC8& /*aType*/,
		const TDesC8& /*aFormat*/,
		TInt /*aTotalSize*/ )
{	
	RDEBUG("DMUtilServer: CChildInfo::SetResultsL - Start");

	if ( !aObject.Size())
	{
		//no child nodes
		return;
	}
	
	if ( aResultsRef != KResultRef || iErr)
	{
		iErr = ETrue;
		return;
	}
	
	
	//create pointer for childlist and read it from package buffer
	delete iBuffer;
	iBuffer = 0;
	
	iBuffer = HBufC8::NewL( aObject.Size());
	
	TPtr8 bufptr = iBuffer->Des();
	aObject.Read( 0, bufptr, aObject.Size());
	TPtrC8 ptr = *iBuffer;
	
	//separate childrens
	while ( ptr.Length())
	{
		//find separator
		TInt index = ptr.Locate('/');
		
		if ( index == KErrNotFound)
		{
			iChilds.AppendL( ptr);
		}
		else
		{
			ptr.Mid( index + 1);
			iChilds.AppendL( ptr.Right( index));
		}
	}
	
	RDEBUG("DMUtilServer: CChildInfo::SetResultsL - End");
}
		
// ----------------------------------------------------------------------------------------
// CChildInfo::SetStatusL()
// ----------------------------------------------------------------------------------------

void CChildInfo::SetStatusL( TInt aStatusRef, TInt aStatusCode )
{
	iErr = !( aStatusRef == KStatusRef && aStatusCode == 200/*KNSmlDmStatusOK*/);
}



#endif


