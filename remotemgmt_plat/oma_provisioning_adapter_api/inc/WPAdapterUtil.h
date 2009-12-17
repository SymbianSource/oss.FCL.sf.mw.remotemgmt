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
* Description:  Utility class for reading resource strings.
*
*/


#ifndef WPADAPTERUTIL_H
#define WPADAPTERUTIL_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class RResourceFile;
class CWPCharacteristic;
class RFs;

// CLASS DECLARATION

/**
 * WPAdapterUtil is a utility class for reading resource strings.
 *
 * @lib ProvisioningEngine
 * @since 2.0
 */ 
class WPAdapterUtil
    {
    public: // New functions
        /**
        * Get a pointer to the access point pointed by a logical proxy.
        * @param aCharacteristic The logical proxy
        * @return The characteristic containing the access point
        */
        IMPORT_C static CWPCharacteristic* AccesspointL( CWPCharacteristic& aCharacteristic );

        /**
        * Loads a specified resource string from resource file.
        * @param aResourceId is of a resource string.
        * @param aDllPath Path for the calling DLL from Dll::FileName
        * @param aAdapterName Name of the adapter. Resource file name is derived from this.
        * @return pointer to loaded descriptor.
        */
        IMPORT_C static HBufC* ReadHBufCL( const TDesC& aDllPath, 
            const TDesC& aAdapterName, TInt aResourceId );


        /**
        * Checks if a URL is valid.
        * @param aURL the URL to check
        * @return ETrue if the URL is valid
        */
        IMPORT_C static TBool CheckURI( const TDesC& aUrl );

        /**
        * Checks if an IPv4 address is valid.
        * @param aP The IP address
        * @return ETrue if the address is valid
        */
        IMPORT_C static TBool CheckIPv4( const TDesC& aIP );

        /**
        * Checks if an IPv6 address is valid.
        * @param aP The IP address
        * @return ETrue if the address is valid
        */
        IMPORT_C static TBool CheckIPv6( const TDesC& aIP );

	    //INTERNET Parameter functions
	    /**
	     * Saves accesspoint attributes of APID, BearerType 
	     * and Originator to DataBase and saves the 
	     * Access Point in SNAP folder
	     * @param aAPId The AccessPoint ID
	     * @return void
	     */
	    IMPORT_C static void SetAPDetailsL(TUint aAPId);

	    /**
	     * Gets the accesspoint ID from DB file or from SNAP 
	     * @param void
	     * @return TUint Accesspoint ID
	     */
	    IMPORT_C static TUint GetAPIDL();
    protected:
        /**
        * Searches for the resource file with the correct language extension 
        * for the language of the current locale, or failing this, the best 
        * matching file.
        * @param aFs reference to file server.
        * @param aDllPath Path for the calling DLL from Dll::FileName
        * @param aAdapterName Name of the adapter. Resource file name is derived from this.
        * @param aResFile Reference to resource file.
        */
        static void FindAndOpenResourceFileLC(RFs& aFs, 
            const TDesC& aDllPath, 
            const TDesC& aAdapterName, 
            RResourceFile& aResFile);
    };

#endif  // WPADAPTERUTIL_H
            
// End of File
