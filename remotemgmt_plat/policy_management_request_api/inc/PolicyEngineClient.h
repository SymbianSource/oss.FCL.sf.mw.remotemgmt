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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/


#ifndef __POLICYENGINECLIENT_H__
#define __POLICYENGINECLIENT_H__

// INCLUDES

#include <PolicyEngineClientServer.h>
#include <RequestContext.h>
#include <ManagementContext.h>

#include <e32std.h>
#include <ssl.h>
#include <x509cert.h>

// CONSTANTS
// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

/**
*  RPolicyEngine class to establish PolicyEngine server connection
*/

class RPolicyEngine : public RSessionBase
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
		* PerformRFS Performs reset factory settings for policy mangement components.
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt PerformRFS();	
};

/**
*  RPolicyManagement class to handle policy management sessions
*/
	
class RPolicyManagement : public RSubSessionBase
{
	public:
		//Session management		
	    /**
		* Connect Open client side session
		* @param aServer Reference to connected RPolicyEngine
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt Open(RPolicyEngine &aServer);
		
		/**
		* Close Close session
        */
		IMPORT_C void Close();

		//Element handling
	    /**
		* AddElement Add policy element to policy storage. 
		* @param aOperationDescription Operation descriped with XML 
		* @param aResponse Parameter returns operation status
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt ExecuteOperation( const TDesC8& aOperationDescription, 
		          					  	TParserResponse &aResponse);

		//Element handling
	    /**
		* AddElement Add policy element to policy storage. 
		* @param aOperationDescription Operation descriped with XML 
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt ExecuteOperation( const TDesC8& aOperationDescription );

	    /**
		* GetElementListL Fetch element list fron policy engine
		* @param aElementType Defines type of elements which are fetched
	    * @param aElementIdArray List of fetched elements
	    * @return KErrNone Symbian error code
        */
        IMPORT_C TInt GetElementListL( const TElementType& aElementType, RElementIdArray& aElementIdArray);

	    /**
		* GetElement Fetch element info from policy engine
		* @param aElementInfo Prefilled with element id value, operation get data from policy 
		* engine and fill missing fields in structure except description
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt GetElementL( TElementInfo& aElementInfo);

	    /**
		* GetElement Fetch element description from policy engine
		* @param aElementInfo Prefilled with element id value, operation get data from policy 
		* engine and fill XACML description field in structure
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt GetXACMLDescriptionL( TElementInfo& aElementInfo);	

	    /**
		* AddSessionTrust Authorize session to use predetermined Subject name (Use AuthorizeSubject to authorize subject). 
		* @param aCertInfo Certificate info for subject
	    * @return KErrNone Symbian error code
        */
		IMPORT_C TInt AddSessionTrust( TCertInfo& aCertInfo);	
		
	    /**
		* IsAllowedServerId Check is the serverId given in parameter allowed to establish silent DM operation
		* @param aServerID Server ID
	    * @return KErrNone is the server id is ok, otherwise Symbian error code
        */
		IMPORT_C TInt IsAllowedServerId( const TDesC& aServerID);		

	    /**
		* CertificateRole Check which role is given for the specific certificate
		* @param aCertInfo Certificate info 
		* @param aRole Role for certificate
	    * @return Symbian error code
        */
		IMPORT_C TInt CertificateRole( TCertInfo& aCertInfo, TRole& aRole);			
		
        /**
        * AddServerCert sets the certificate received from 
        * server and makes it as trusted.
        * @param aCert certificate recieved for storage
        * @return KErrNone Symbian error code
        */		
		IMPORT_C TInt AddServerCert(const CX509Certificate& aCert, TDes& aLabel);
        
		
		/**
        * RemoveServerCert removes the certificate stored 
        * in the cert store.
        * @param aLabel label of certificate to be removed
        * @return KErrNone Symbian error code
        */
        IMPORT_C TInt RemoveServerCert(const TDesC& aLabel = KNullDesC);
};

/**
*  RPolicyRequest class to handle policy request sessions
*/

class RPolicyRequest : public RSubSessionBase
{
	public:
		//session management
		IMPORT_C TInt Open(RPolicyEngine &aServer);
		IMPORT_C void Close();
	
		//request 
		IMPORT_C TInt MakeRequest( TRequestContext& aRequestContext, TResponse& aResponse);
};


#endif //__POLICYENGINECLIENT_H__


