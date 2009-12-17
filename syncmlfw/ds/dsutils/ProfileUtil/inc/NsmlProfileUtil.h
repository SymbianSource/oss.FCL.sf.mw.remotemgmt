/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Used in Auto-Restart to read/write profile to cenrep, read from
*                settings database etc.
*
*/


#ifndef NSML_PROFILE_UTIL_H
#define NSML_PROFILE_UTIL_H

// SYSTEM INCLUDES
#include <e32cons.h>
#include <s32mem.h>
#include <e32uid.h>
#include <centralrepository.h>

// USER INCLUDES
#include "nsmldssettings.h"

// FORWARD DECLARATIONS
class CNsmlContentData;

/**
 * Utility class. Provides features to read a profile form settings database,
 * cache the profile to cenrep in a binary key, read the cached profile from
 * cenrep.
 */
class CNsmlProfileUtil : public CBase
{
    public:

        /**
         * Two phase construction
         */
		IMPORT_C static CNsmlProfileUtil* NewLC();
		
		/**
         * Two phase construction
         */
		IMPORT_C static CNsmlProfileUtil* NewL();
		
		/**
         * Destructor
         */
		virtual ~CNsmlProfileUtil();
		
		/**
         * Initialize this object by reading profile details from settings database
         */
		IMPORT_C void InternalizeFromSettingsDBL(TInt aProfileId);
		
		/**
         * Initialize this object by reading profile details from central repository
         */
		IMPORT_C void InternalizeFromCenrepL();
		
		/**
         * Write this object details to central repository
         */
		IMPORT_C void ExternalizeToCenrepL();
		
		/**
		 * Returns the profile id
		 */
		IMPORT_C TSmlProfileId ProfileId();
		
		/**
		 * Compare this profile against a new profile. Return ETrue if same
		 */
		IMPORT_C TBool IsSame(CNsmlProfileUtil& aNewProfile);
		/**
		 * Check Whether a Valid Interrupt has occurred or not
		 */
		IMPORT_C void IsValidResumeL(TBool& aValue);
		
		/**
		 * Write the value of Interrupt Flag 
		 */
		IMPORT_C void WriteInterruptFlagL(TInt aFlag);
		
		/**
		 * Write the value of IAPID
		 */
		IMPORT_C void WriteAccessPointIdL(TInt aIAPID);
		
		/**
		 * Read the value of IAPID
		 */
		IMPORT_C void AccessPointIdL(TInt& aIAPID);
		
		/**
		 * Write the TaskIds' in to the Cenrep
		 */
		IMPORT_C void ExternalizeTaskIdL(RArray<TSmlTaskId> aTaskIdArray);
		
		/**
		 * Read the TaskIds' in to the Cenrep
		 */
		IMPORT_C void InternalizeTaskIdL(RArray<TSmlTaskId>& aTaskIdArray);
			    
    private:
		
		/**
		 * Second Phase constructor
		 */
		void ConstructL(); 
		
		/**
		 * Utility to write a size of string and its contents to a stream
		 */
		void WriteToStreamL(RDesWriteStream& aStream, const TDesC& aString);
		
		
		/**
		 * Compares the Int values of the Profile details
		 */
		 TBool CompareIntValues(TInt aCenrepValue,TInt aCurrentValue);
		 
		 /**
		  * Compares the String values of the Profile details
		  */
		  		  
		 TBool CompareStringValues(TPtr16 aNewValue, TPtr16 aCurrentValue);
		
    private:
	    
	    /**
	     * Internal profile id
	     */
		TSmlProfileId iProfileId; 

        /**
         * Profile name
         */
		HBufC* iDisplayName;
 
        /**
         * Protocol version selected for this profile
         */
		TSmlProtocolVersion iProtocolVer;
		
		
		/**
		 * Bearer Type selected for this profile
		 */
		 TSmlTransportId   iTransportId;
		 		

        /**
         * Access Point selected for this profile
         *
         */
         TInt iIapId;        
        
        /**
         * Server id set for this profile
         */
		HBufC* iServerId;

        /**
         * Server URL defined for this profile
         */
		HBufC* iServerURI;

        /**
         * Server port set for this profile
         */
		TInt  iServerPort;

        /**
         * Username defined for this profile
         */
		HBufC* iSyncMLUserName;		
		
        /**
         * Password defined for this profile
         */
		HBufC* iSyncMLPassword;
		
		/**
		 * SAN request settings for this profile
		 */
		 TSmlServerAlertedAction iSanRequest; 

        /**
         * HTTP username defined for this profile
         */
		HBufC* iSyncHTTPAuthUserName;	

        /**
         * HTTP passwd defined for this profile
         */
		HBufC* iSyncHTTPAuthPassword;

        /**
         * Sync Type set for this profile
         */
		TSmlSyncType iSyncType;
		

        /**
         * Content details defined for this profile
         */	    
		RPointerArray<CNsmlContentData> iContents; 
		
};


/**
 * Support utility for CNsmlProfileUtil. This class holds the content details
 * selected for a profile
 */
class CNsmlContentData : public CBase 
{
    public:
    
       /**
        * Two phase construction
        */
       IMPORT_C static CNsmlContentData* NewLC();
       
       /**
        * Two phase construction
        */
       IMPORT_C static CNsmlContentData* NewL();

       /**
        * Virtual destructor
        */
       virtual ~CNsmlContentData();       
       
       /**
        * Returns the server data source for this profile
        */
       IMPORT_C HBufC* ServerDataSource();
       
       /**
        * Set the server data source defined for this profile
        */      
       IMPORT_C void SetServerDataSourceL(const TDesC& aServerDataSource);
       
       /**
        * Returns the implementation id for this profile
        */      
       IMPORT_C TInt ImplementationId();
       
       /**
        * Set the implementation id for this profile
        */            
       IMPORT_C void SetImplementationId(const TInt aImplementationId)  ;
       
       /**
		* Returns the content enabled status
		*/  
       IMPORT_C TInt ContentEnabled();
                 
	   /**
		* Set the content enabled status
		*/  
       IMPORT_C void SetContentEnabled(const TInt aContentEnabled);
   
    private:
    
    	/**
		 * Second Phase constructor
	 	 */
        void ConstructL();     
 
    private:    
     
        /**
         * Content is enabled or not
         */
        TInt iEnabled;
    
       
        /**
         * Implementation id for this content type
         */
		TInt iImplementationId;
		
        /**
         * Server data source defined for this content type
         */
		HBufC* iServerDataSource;
};

#endif // NSML_PROFILE_UTIL_H





