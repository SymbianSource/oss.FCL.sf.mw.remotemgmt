/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  XML file handler for DM-settings
*
*/


#ifndef __DMPROFILECONTENTHANDLER_H
#define __DMPROFILECONTENTHANDLER_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <xml/MXMLAttributes.h>
#include <xml/MXMLContentHandler.h>
#include "NSmlDMResourceProfile.h"

const TInt KNSmlDmFileNameLength=255;
const TInt KNSmlDmProfileNameMaxLength = 100;
const TInt KNSmlDmSettingsSuccess = 1;
const TInt KNSmlDmSettingsFail = 0; 
const TInt KNSmlDmInternetType = 0x101F99F0;
const TInt KNSmlDmBluetoothType = 0x101F99F1;
const TInt KNSmlDmMaxPortLength = 5;
const TUint8 KNSmlDMColon = ':';
const TUint8 KNSmlDMUriSeparator = '/';

_LIT( KNSmlDMXmlFactorySettings, 				"DMFactorySettings" );
_LIT( KNSmlDMXmlServerId,						"ServerID" );
_LIT( KNSmlDMXmlProfileName,					"ProfileName" );
_LIT( KNSmlDMXmlProfileAP,						"AccessPoint" );
_LIT( KNSmlDMXmlProfileTransportId,				"TransportID" );
_LIT( KNSmlDMXmlProfileUsername,				"UserName" );
_LIT( KNSmlDMXmlProfileServerPassword,			"ServerPassword" );
_LIT( KNSmlDMXmlProfileClientPassword, 			"ClientPassword" );
_LIT( KNSmlDMXmlProfileServerUrl,				"ServerURL" );
_LIT( KNSmlDMXmlProfileServerAlertedAction,		"ServerAlertedAction" );
_LIT( KNSmlDMXmlProfileAuthentication,	 		"Authentication" );
_LIT( KNSmlDMXmlProfileDeleteAllowed,			"DeleteAllowed" );
_LIT( KNSmlDMXmlProfileView,					"ProfileView" );
_LIT( KNSmlDMXmlProfileNetworkAuthentication,	"NetworkAuthentication" );
_LIT( KNSmlDMXmlProfileHttpAuthUsername,	 	"HTTPUserName" );
_LIT( KNSmlDMXmlProfileHttpAuthPassword,	 	"HTTPPassword" );
_LIT( KNSmlDMInternetTypeString,	 			"0x101F99F0" );
_LIT( KNSmlDMBluetoothTypeString,	 			"0x101F99F1" );
_LIT( KNSmlDMDefaultProfile,					"Unnamed server" );
_LIT( KNSmlDMXmlProfileLock,					"ProfileLock" );

_LIT( KNSmlDMSettingsHTTP,  "http://" );
_LIT( KNSmlDMSettingsHTTPS, "https://" );

enum TDMparameterState
	{
	//Management operations
	EStateHandled = 0,
	EProfileName,
	EServerID,
	EServerPassword,
	EUserName,
	EUserPwd,
	EClientPwd,
	EServerURL,
	ETransportID,
	EAccessPoint,
	EServerAlertedAction,
	EAuthentication,
	EDeleteAllowed,
	EProfileView,
	ENetworkAuthentication,
	EHTTPUserName,
	EHTTPPwd,
	EProfileLock	
	};

// CLASS DECLARATION

// FORWARD DECLARATIONS
class CNSmlDMSettings;

/**
*  Interface for SAX Parser Callback events.
*  If an application (client) needs to be informed of basic parsing events, 
*  it implements this interface and registers an instance with the SAX parser using the SetContentHandler() method.
*  The parser uses the instance to report basic document-related events like the start and end of elements.
*
*  @lib ?library
*  @since ?Series60_version
*/
class CDMProfileContentHandler: public CBase, public MXMLContentHandler
{
public:


	/**
    * Two-phased constructor.
    */
    static CDMProfileContentHandler* NewL(CNSmlDMSettings* aSettings);

	static CDMProfileContentHandler* NewLC();
        
    /**
    * Destructor.
    */
    virtual ~CDMProfileContentHandler();

	/**
	* Appends the new profile
	* @param -
	* @return -
	*/
	void AddProfileL();
	
	/** 
	* Assign the profile element value
	* @param -
	* @return -
	*/
	void AddProfileElementL(TDesC& aBuf);
	
	/**
	* Removes duplicate profiles from array
	* @param -
	* @return -
	*/
	void RemoveDuplicateEntry();
	
	/**
	* Removes RSC duplicate profiles from array
	* @param -
	* @return -
	*/
	void RemoveRSCEntryL();

	/**
	* Returns true value for other than numerical character.
	* @param - aChar
	* @return - true if aChar is other than numerical character.
	*/
	TBool IsChar( const TUint8 aChar );
	
	/**
	* Gets IAP id from the db
	* @param - TInt count, TInt &iapId
	* @return - TBool - ETrue if server id was found
	*/
	TBool GetAccessPointIdL(TInt count, TInt &iapId);
	
	/**
	* Save profiles to db
	* @param - 
	* @return -
	*/
	void SaveProfilesL();
	
	/**
	* Receive notification of the beginning of a document.
	* @param none
	* @return none
	*/		
    TInt StartDocument();

	/**
	* Receive notification of the end of a document.
	* @param none
	* @return none
	*/
	TInt EndDocument();

	/**
	* Receive notification of the beginning of an element.
	* @param aURI: The Namespace URI, or the empty string if the element has no Namespace URI or if Namespace
	*              processing is not being performed.
	* @param aLocalName: The local name (without prefix), or the empty string if Namespace processing is not being
	*                    performed.
	* @param aName: The qualified name, or the empty string if qualified names are not available.
	* @param aAttributeList: The attributes attached to the element. If there are no attributes, it shall be an 
	*                        empty Attributes object. 
	*        
	* @return none
	*/
	TInt StartElement(TDesC& aURI, TDesC& aLocalName, TDesC& aName, MXMLAttributes *aAttributeList);

	/**
	* Receive notification of the end of an element.
	* @param aURI: The Namespace URI, or the empty string if the element has no Namespace URI or if Namespace
	*              processing is not being performed.
	* @param aLocalName: The local name (without prefix), or the empty string if Namespace processing is not being
	*                    performed.
	* @param aName: The qualified name, or the empty string if qualified names are not available.
	* @return none
	*/
	TInt EndElement(TDesC& aURI, TDesC& aLocalName, TDesC& aName);

	/**
	* Receive notification of the start of a CDATA section. 
	* The contents of the CDATA section will be reported through the Characters() event. 
	* @param none. 
	* @return none.
	*/
	TInt StartCDATA();

	/**
	* Receive notification of the end of a CDATA section.  
	* @param none. 
	* @return none.
	*/
	TInt EndCDATA();

	/**
	* Receive notification of character data inside an element.
	* @param aBuff: The characters.
	* @param aStart: The start position in the character buffer.
	* @param aLength: The number of characters to use from the character buffer.
	* @return none
	*/
	TInt Charecters(TDesC& aBuf, TInt aStart, TInt aLength);

	/**
	* Receive notification of XML comment.
	* @param aComment: Buffer containing the comment.
	* @return none
	*/
	TInt Comment(TDesC& aComment); // JIM might need start and end index as params.

	/**
	* Receive notification of a processing instruction
	* @param aTarget: The processing instruction target.
	* @param aData: The processing instruction data, or null if none is supplied.
	* @return none
	*/
   TInt ProcessingInstructions(TDesC& aTarget, TDesC& aData);

	/**
	* Receive notification of ignorable whitespace in element content.
	* @param aString: The whitespace characters.
	* @return none
	*/
	TInt IgnoreWhiteSpace(TDesC& aString); // JIM might need start and end index as params.

	/**
	* Receive notification of beginning of some internal and external XML entities.
	* @param aName: The name of the entity.
	* @return none
	*/
	TInt StartEntity(TDesC& aName);

	/**
	* Receive notification of end of XML entities.
	* @param aName: The name of the entity.
	* @return none
	*/
	TInt EndEntity(TDesC& aName);

	/**
	* Receive notification of a skipped entity.
	* @param aName: The name of the skipped entity.
	* @return none
	*/
	TInt SkippedEntity(TDesC& aName);

	/**
	* Receive notification of error situation during parsing.
	* @param aErrorCode: Error status code.
	* @param aSeverity: Error Severity.
	* @return none.
	*/
	TInt Error(TInt aErrorCode, TInt aSeverity);

protected:
	CDMProfileContentHandler();
		

private:
	void ConstructL(CNSmlDMSettings* aSettings);
	
	/** 
	* Assign the profile element value
	* @param aBuf : element value
	* @return -
	*/
	void AddRemainingElementsL(TDesC& aBuf);

private:

// List of attributes
	RFile iFile;
	RFs iSession;
	TBuf<KNSmlDmFileNameLength> iFilename;

	TDMparameterState iState;
	TInt iCounter;
	TBool iDMAccountInProgress;
	CArrayPtrFlat<CNSmlDMResourceProfile>* iProfileArray;
	CNSmlDMSettings* iSettings;
};
            
#endif	// End of File
