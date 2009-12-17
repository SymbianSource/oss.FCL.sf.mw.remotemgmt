/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Resource data for DS-settings 
*
*/


#ifndef __NSMLDSRESOURCEPROFILE_H
#define __NSMLDSRESOURCEPROFILE_H

// INCLUDES
#include "e32base.h"  // CBase
#include "barsread.h" // TResourceReader @bafl.lib
#include "nsmldssettings.h"
// CONSTANTS

// FORWARD DECLARATIONS
class CNSmlDSResourceProfile;
class CNSmlDSResourceContentType;
class CNSmlDSSettings;
class CNSmlDSProfile;

// CLASS DECLARATION

/**
* CNSmlDSResourceProfiles
* For reading profiles from resources
*/
class CNSmlDSResourceProfiles : public CBase
	{

	public: // New functions
		/**
		* ~CNSmlDSResourceProfiles(); 
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDSResourceProfiles();
		
		/**
		* static CNSmlDSResourceProfiles* NewLC( TResourceReader& aReader, CNSmlDSSettings* aSettings );
		* Construction
		* @param TResourceReader& aReader - Resource to be read
		* @param CNSmlDSSettings* aSettings - the database
		* @return CNSmlDSResourceProfiles* - new CNSmlDSResourceProfiles*
		*/
		static CNSmlDSResourceProfiles* NewLC( TResourceReader& aReader, CNSmlDSSettings* aSettings );

		/**
		* void SaveProfilesL(); 
		* @param -
		* @return -
		*/
		void SaveProfilesL(CNSmlProfileArray* aArray);
		
		
	private: // New functions

		/**
		* void ConstructL( TResourceReader& aReader, CNSmlDSSettings* aSettings );
		* @param TResourceReader& aReader - Resource to be read
		* @param CNSmlDSSettings* aSettings - the database
		* @return -
		*/
		void ConstructL( TResourceReader& aReader, CNSmlDSSettings* aSettings );

		/**
		* void AddDataL( TResourceReader& aReader ) 
		* Gets data from resourcefile
		* @param TResourceReader& aReader - Resource to be read
		* @return -
		*/
		void AddDataL( TResourceReader& aReader );
		

	private: // Data
		CArrayPtrFlat<CNSmlDSResourceProfile>* iProfileArray;
		CNSmlDSSettings* iSettings;
	
	};


/**
* CNSmlDSResourceProfile
* For reading profile from resources
*/
class CNSmlDSResourceProfile : public CBase
	{

	public: // New functions
		/**
		* ~CNSmlDSResourceProfile(); 
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDSResourceProfile();
		
		/**
		* static CNSmlDSResourceProfile* NewLC( TResourceReader& aReader );
		* Construction
		* @param TResourceReader& aReader - Resource to be read
		* @return CNSmlDSResourceProfile* - pointer to new object
		*/
		static CNSmlDSResourceProfile* NewLC( TResourceReader& aReader );

		/**
		* TBool SaveProfileL(); 
		* @param CNSmlDSProfile* iProfile - profile to save data to
		* @return TBool - ETrue if save succesfull
		*/
		TBool SaveProfileL( CNSmlDSProfile* aProfile );
		
		
	
	private: // New functions

		/**
		* void ConstructL( TResourceReader& aReader );
		* @param TResourceReader& aReader - Resource to be read
		* @return -
		*/
		void ConstructL( TResourceReader& aReader );

		/**
		* void AddDataL( TResourceReader& aReader ) 
		* Gets data from resourcefile
		* @param TResourceReader& aReader - Resource to be read
		* @return -
		*/
		void AddDataL( TResourceReader& aReader );
		

	private: // Data
		CArrayPtrFlat<CNSmlDSResourceContentType>* iContentTypeArray;

		HBufC* iProfileDisplayName;
		TInt iIapId;
		TInt iTransportId;
		HBufC* iServerUserName;
		HBufC* iServerPassWord;
		HBufC* iServerURL;
		HBufC* iServerId;
		TInt iServerAlertedAction;
		TBool iDeleteAllowed;
		TBool iProfileHidden;
		HBufC* iVisibilityStr;
		TInt iProtocolVersion;
		TInt iCreatorId;
		HBufC* iHttpAuthUsername;
		HBufC* iHttpAuthPassword;
		TBool iHttpAuthUsed;
		TInt iFilterMatchType;
		
	};

/**
* CNSmlDSResourceContentType
* For reading contenttypes from resources
*/
class CNSmlDSResourceContentType : public CBase
	{

	public: // New functions
		/**
		* ~CNSmlDSResourceContentType(); 
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDSResourceContentType();
		
		/**
		* static CNSmlDSResourceContentType* NewLC( TResourceReader& aReader );
		* Construction
		* @param TResourceReader& aReader - Resource to be read
		* @return CNSmlDSResourceContentType* - pointer to new object
		*/
		static CNSmlDSResourceContentType* NewLC( TResourceReader& aReader );

		/**
		* TBool SaveContentL(); 
		* @param CNSmlDSProfile* iProfile - prfile to save content for
		* @return TBool - ETrue if save ok
		*/
		TBool SaveContentL( CNSmlDSProfile* iProfile );
		
		
	
	private: // New functions

		/**
		* void ConstructL( TResourceReader& aReader );
		* @param TResourceReader& aReader - Resource to be read
		* @return -
		*/
		void ConstructL( TResourceReader& aReader );

		/**
		* void AddDataL( TResourceReader& aReader ) 
		* Gets data from resourcefile
		* @param TResourceReader& aReader - Resource to be read
		* @return -
		*/
		void AddDataL( TResourceReader& aReader );
		

	private: // Data
		TInt iImplementationId;
		HBufC* iClientDataSource;
		HBufC* iServerDataSource;
		TInt iSyncType;
		HBufC* iFilterVisibility;
		HBufC* iSyncTypeVisibility;
		TInt iCreatorId;
		TBool iEnabled;
	};


#endif __NSMLDSRESOURCEPROFILE_H