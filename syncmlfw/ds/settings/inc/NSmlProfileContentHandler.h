/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: XML parser call back implementation to read profile setting from xml file
*
*/



#ifndef __NSmlProfileContentHandler_H
#define __NSmlProfileContentHandler_H

//  INCLUDES
#include <e32std.h>
#include <f32file.h>
#include <eikenv.h> //JIM is this needed ??
#include "xml/MXMLAttributes.h"
#include "xml/MXMLContentHandler.h"

#include "nsmldssettings.h"
//#include "nsmldsresourceprofiledefs.h"
#include <nsmldsconstants.h>
#include <nsmlconstants.h>

enum TDSparameterState
	{
	//Management operations
	EProfileDisplayName,
	EProfileServerURL,
	EProfileIAPId,
	EProfileTransportId,
	EProfileSyncServerUsername,
	EProfileSyncServerPassword,
	EProfileServerAlertedAction,
	EProfileDeleteAllowed,
	EProfileHidden,
	EProfileHttpAuthUsed,
	EProfileHttpAuthUsername,
	EProfileHttpAuthPassword,
	EProfileAutoChangeIAP,
    EProfileCreatorId,   
	EProfileServerId,
	EProfileProtocolVersion,	
	ESyncType,
	EDefaultProfile,
	EAdapterImplementationId,
	EAdapterServerDataSource,
	EStateHandled
	};

// CLASS DECLARATION

/**
*  Interface for SAX Parser Callback events.
*  If an application (client) needs to be informed of basic parsing events, 
*  it implements this interface and registers an instance with the SAX parser using the SetContentHandler() method.
*  The parser uses the instance to report basic document-related events like the start and end of elements.
*
*  @lib ?library
*  @since ?Series60_version
*/
class CNSmlProfileContentHandler: public CBase, public MXMLContentHandler
{
public:


	 /**
        * Two-phased constructor.
        */
        static CNSmlProfileContentHandler* NewL(CNSmlDSSettings* aSettings ,CNSmlProfileArray* aArray);

		static CNSmlProfileContentHandler* NewLC();
        
        /**
        * Destructor.
        */
        virtual ~CNSmlProfileContentHandler();

/**
* Receive notification of the beginning of a document.
* @since ?Series60_version
* @param none
* @return none
*/		
     TInt StartDocument();

/**
* Receive notification of the end of a document.
* @since ?Series60_version
* @param none
* @return none
*/
	TInt EndDocument();

/**
* Receive notification of the beginning of an element.
* @since ?Series60_version
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
* Receive notification 	void ConstructL( TResourceReader& aReader, CNSmlDSSettings* aSettings );of the end of an element.
* @since ?Series60_version
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
* @since ?Series60_version
* @param none. 
* @return none.
*/
	TInt StartCDATA();

/**
* Receive notification of the end of a CDATA section.  
* @since ?Series60_version
* @param none. 
* @return none.
*/
	TInt EndCDATA();

/**
* Receive notification of character data inside an element.
* @since ?Series60_version
* @param aBuff: The characters.
* @param aStart: The start position in the character buffer.
* @param aLength: The number of characters to use from the character buffer.
* @return none
*/
	TInt Charecters(TDesC& aBuf, TInt aStart, TInt aLength);

/**
* Receive notification of XML comment.
* @since ?Series60_version
* @param aComment: Buffer containing the comment.
* @return none
*/
	TInt Comment(TDesC& aComment); // JIM might need start and end index as params.

/**
* Receive notification of a processing instruction
* @since ?Series60_version
* @param aTarget: The processing instruction target.
* @param aData: The processing instruction data, or null if none is supplied.
* @return none
*/
   TInt ProcessingInstructions(TDesC& aTarget, TDesC& aData);

/**
* Receive notification of ignorable whitespace in element content.
* @since ?Series60_version
* @param aString: The whitespace characters.
* @return none
*/
	TInt IgnoreWhiteSpace(TDesC& aString); // JIM might need start and end index as params.

/**
* Receive notification of beginning of some internal and external XML entities.
* @since ?Series60_version
* @param aName: The name of the entity.
* @return none
*/
	TInt StartEntity(TDesC& aName);

/**
* Receive notification of end of XML entities.
* @since ?Series60_version
* @param aName: The name of the entity.
* @return none
*/
	TInt EndEntity(TDesC& aName);

/**
* Receive notification of a skipped entity.
* @since ?Series60_version
* @param aName: The name of the skipped entity.
* @return none
*/
	TInt SkippedEntity(TDesC& aName);

/**
* Receive notification of error situation during parsing.
* @since ?Series60_version
* @param aErrorCode: Error status code.
* @param aSeverity: Error Severity.
* @return none.
*/
	TInt Error(TInt aErrorCode, TInt aSeverity);
    
/**
* Create a content type
* @param none
* @return none.
*/
	void CreateContentTypeL();
	
/**
* Check whether a content type is already created for the same profile
* @param adapter ID
* @return ETrue/EFalse.
*/
	TBool IsContentAdded(TInt aAdapter);
	
/**
* Convert XML value to a valid server alert type 
* @param TInt xml value
* @return TSmlServerAlertedAction type
*/
	TSmlServerAlertedAction ServerAlertType(TInt aType);

/**
* Convert XML value to a valid sync type 
* @param xml value
* @return TSmlSyncType type
*/
	TSmlSyncType SyncType(TDesC& aBuf);
	
/**
* Get the resolved access point ID for a string entered in XML
* @param xml value
* @return TInt access point ID
*/
	TInt AccessPointIdL(TDesC& aBuf);
	
/**
* Check mandatory settings for content defention
* @param none
* @return ETrue/EFalse
*/
	TBool CheckXMLContentSetting();
	
/**
* Configure visibility based on value entered in xml
* @param value
* @return none
*/
	void SetVisibility(TInt aValue);
	
/**
* Configure visibility based on value entered in xml
* @param value
* @return none
*/
	
	void SetServerURL(TDesC &aBuf);

protected:

		CNSmlProfileContentHandler();
		

	private:
		void ConstructL(CNSmlDSSettings* aSettings ,CNSmlProfileArray* aArray);

private:

	// List of attributes
//	CXMLAttributes *iAttr;
	RFile iFile;
	RFs iSession;
	TBuf<255> ifilename;
	TLex16 iLex;

	TDSparameterState iState;
	TInt iCounter;
	TInt iContentCounter;
	
	TBool iDSSettingInProgress;
	TBool iDSContentInProgress;
	
	TBool iInvalidContent;
	
	CNSmlDSSettings* iSettings;
	CArrayFixFlat<TInt>* iSyncTypeArray;
	CArrayPtrFlat<CNSmlDSProfile>* iCustomProfileArray;
	CArrayPtrFlat<CNSmlDSContentSettingType>* iContentArray;

};


#endif      // __NSmlProfileContentHandler_H   
            
// End of File
