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
* Description:  Resource data for DM-settings 
*
*/


#ifndef __NSMLDMRESOURCEPROFILE_H
#define __NSMLDMRESOURCEPROFILE_H

// INCLUDES
#include "e32base.h"  // CBase
#include "barsread.h" // TResourceReader @bafl.lib

// CONSTANTS

// FORWARD DECLARATIONS
class CNSmlDMResourceProfile;
class CNSmlDMSettings;
class CNSmlDMProfile;

// CLASS DECLARATION

/**
* CNSmlDMResourceProfiles
* For reading profiles from resources
*/
class CNSmlDMResourceProfiles : public CBase
	{

	public: // New functions
		/**
		* ~CNSmlDMResourceProfiles(); 
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDMResourceProfiles();
		
		/**
		* static CNSmlDMResourceProfiles* NewLC( TResourceReader& aReader, CNSmlDMSettings* aSettings );
		* Construction
		* @param TResourceReader& aReader - Resource to be read
		* @param CNSmlDMSettings* aSettings - database to save settings into
		* @return CNSmlDMResourceProfiles* - pointer to new CNSmlDMResourceProfiles
		*/
		static CNSmlDMResourceProfiles* NewLC( TResourceReader& aReader, CNSmlDMSettings* aSettings );

		/**
		* TInt RemoveDuplicateEntry() - marks duplicate profiles
		* @param -
		* @return -
		*/
		void RemoveDuplicateEntry();
		
		/**
		 * Removes OBEX profiles when DM OBEX is not supported
		 * @param -
		 * @return -
		 */
		void RemoveObexEntryL();
		
		/**
		* TInt SaveProfilesL() - saves profiles
		* @param -
		* @return -
		*/
		void SaveProfilesL();
		
		
	private: // New functions

		/**
		* void ConstructL( TResourceReader& aReader, CNSmlDMSettings* aSettings );
		* @param TResourceReader& aReader - Resource to be read
		* @param CNSmlDMSettings* aSettings - database to save settings into
		* @return -
		*/
		void ConstructL( TResourceReader& aReader, CNSmlDMSettings* aSettings );

		/**
		* void AddDataL( TResourceReader& aReader ) 
		* Gets data from resourcefile
		* @param TResourceReader& aReader - Resource to be read
		* @return -
		*/
		void AddDataL( TResourceReader& aReader );
		

	private: // Data
		CArrayPtrFlat<CNSmlDMResourceProfile>* iProfileArray;
		CNSmlDMSettings* iSettings;
	
	};


/**
* CNSmlDMResourceProfile
* For reading profile from resources
*/
class CNSmlDMResourceProfile : public CBase
	{

	public: // New functions
		/**
		* ~CNSmlDMResourceProfile(); 
		* Destructor
		* @param -
		* @return -
		*/
		~CNSmlDMResourceProfile();
		
		/**
		* static CNSmlDMResourceProfile* NewLC();
		* Construction
		* @param  - 
		* @return CNSmlDMResourceProfile* - pointer to new object
		*/
		static CNSmlDMResourceProfile* NewLC();
		
		/**
		* static CNSmlDMResourceProfile* NewLC( TResourceReader& aReader );
		* Construction
		* @param TResourceReader& aReader - Resource to be read
		* @return CNSmlDMResourceProfile* - pointer to new object
		*/
		static CNSmlDMResourceProfile* NewLC( TResourceReader& aReader );

		/**
		* TBool SaveProfileL(); 
		* @param CNSmlDMProfile* iProfile - profile to save data to
		* @return TBool - ETrue if save succesfull
		*/
		TBool SaveProfileL( CNSmlDMProfile* aProfile );
		
		/**
		* HBufC* GetServerId(); 
		* @param 
		* @return HBufC* - ServerId
		*/
		
		HBufC* GetServerId();
		
	friend class CDMProfileContentHandler;
	friend class CNSmlDMResourceProfiles;

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
		HBufC* iProfileDisplayName;
		TInt iIapId;
		TInt iTransportId;
		TInt iDMSessionId;
		HBufC* iServerId;
		HBufC* iUserName;
		HBufC* iPassWord;
		HBufC* iServerURL;
		HBufC* iClientPassword;
		HBufC* iAccessPoint;
		TBool iServerAlertAction;
		TInt8 iAuthenticationRequired;
		TBool iSyncAccepted;
		TInt iCreatorId;
		TBool iDeleteAllowed;
		TBool iProfileHidden;
		
		TBool iHttpAuthUsed;
		HBufC* iHttpAuthUsername;
		HBufC* iHttpAuthPassword;
		TBool iProfileLock;
	};

#endif //__NSMLDMRESOURCEPROFILE_H