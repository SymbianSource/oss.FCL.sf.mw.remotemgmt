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


#ifndef __DMUtilCLIENT_H__
#define __DMUtilCLIENT_H__

// INCLUDES
#include <e32std.h>
#include <ssl.h>
#include "DMUtilClientServer.h"

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  RDMUtil class to establish DMUtil server connection
*/

class RDMUtil : public RSessionBase
{
	public:
	    /**
		* Connect Connects client side to policy engine server
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Connect();

	    /**
		* Close Close server connection
        */
		IMPORT_C void Close();

	    /**
		* Connect Remove ACL configuration from specific ACL node in DMUtilServer
		* @param aURI Node address
		* @param aRestoreDefaults If true, default values are restored to ACL node (in DM client), 
		* otherwise existing values will stay
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt RemoveACL( const TDesC8& aURI, TBool aRestoreDefaults);	
		
	    /**
		* Connect Add ACL to node, this is incremental operation
		* @param aURI Node address
		* @param aCommandType Scecific commanf type (add, get, remove...)
	    * @return KErrNone Symbian error code
        */
        
		IMPORT_C TInt AddACLForNode( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType);	
		
	    /**
		* Connect Add ACL to node, this is not incremental operation, old values are deleted
		* @param aURI Node address
		* @param aCommandType Scecific commanf type (add, get, remove...)
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt SetACLForNode( const TDesC8& aURI, const TACLDestination& aDestination, const TAclCommands& aCommandType);	
	
	    /**
		* Connect Add certificate that is used during session
		* @param aCertInfo 
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt SetMngSessionCertificate( const TCertInfo& aCertInfo);	


	    /**
		* Connect Connects client side to policy engine server
		* @param aCertInfo
		* @param aServerID
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt NewDMSession( const TCertInfo& aCertInfo, const TDesC8& aServerID);

	    /**
		* Connect Flush changes to database
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Flush();

		/**
		* GetDMSessionCertInfo Gets certificate info of the current _active_ DM Session
		* @return KErrNone Symbian error code
		*/
		IMPORT_C TInt GetDMSessionCertInfo( TCertInfo &aCertInfo );

		/**
		* GetDMSessionSessionId Gets server id of the current _active_ DM Session
		* @return KErrNone Symbian error code
		*/
		IMPORT_C TInt GetDMSessionServerId( TDes8& aServerId );
		
		/**
		* UpdatePolicyMngStatusFlags Update policy management status flags
		* @param aPolicyMngStatusFlags
		* @return KErrNone Symbian error code
		*/
		IMPORT_C TInt UpdatePolicyMngStatusFlags( KPolicyMngStatusFlags aPolicyMngStatusFlags);		
		
		/**
		* PerformRFS Perform RFS operation Policy Management subsystem
		* @return KErrNone Symbian error code
		*/
		IMPORT_C TInt PerformRFS();		
		
		/**
		* Use this method to set a flag to perform MMC format along deep reset.
		* This happens when the system is called for deep reset -> system boot ->
		* PMRfsPlugin is started -> reads the flag and performs also MMC wipe.
		*/
		IMPORT_C TInt MarkMMCWipe();
		
};

#endif //__DMUtilCLIENT_H__
