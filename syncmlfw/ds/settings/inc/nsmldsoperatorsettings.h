/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  DS Operator Settings
*
*/

#ifndef __NSMLDSOPERATORSETTINGS_H
#define __NSMLDSOPERATORSETTINGS_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CRepository;

// ----------------------------------------------------------------------------
//  CNSmlDSOperatorSettings provides an interface to 
//  DS Operator Settings repository
//
//*  @lib nsmldssettings.lib
// ----------------------------------------------------------------------------
class CNSmlDSOperatorSettings : public CBase
    {
    public:
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CNSmlDSOperatorSettings* NewL();

        /**
        * Two-phased constructor.
        */
        IMPORT_C static CNSmlDSOperatorSettings* NewLC();

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CNSmlDSOperatorSettings();

        /**
        * Is current profile Operator specific profile.
        * @param aServerId Server Id to compare in Repository value.
        * @return Boolean ETrue if profile is Operator specific profile.
        */
        IMPORT_C TBool IsOperatorProfileL( const TDesC& aServerId  );

        /**
        * SW version of Operator specific profile.
        * @return Software version.
        */
        IMPORT_C HBufC8* CustomSwvValueLC();

        /**
        * Model value of Operator specific profile.
        * @return Mod value.
        */
        IMPORT_C HBufC8* CustomModValueLC();

        /**
        * Manufacturer value of Operator specific profile.
        * @return Manufacturer value.
        */
        IMPORT_C HBufC8* CustomManValueLC();

        /**
        * Populates server status code list
        * @param RArray to contains status codes
        */
        IMPORT_C void PopulateStatusCodeListL( RArray< TInt >& aArray );

        /**
        * SyncML Error Reporting
        * @return Boolean ETrue if SyncML Error Reporting is enabled.
        */
        IMPORT_C TBool SyncErrorReportingEnabled(); 
                
        /**
        * HTTP Error Reporting
        * @return Boolean ETrue if HTTP Error Reporting is enabled.
        */
        IMPORT_C TBool HttpErrorReportingEnabled(); 
                
    private:

        /**
         * Reads list of status codes from cenrep
        * @param RArray to contains status codes
        */
        void InitializeServerStatusCodeListL( RArray< TInt >& aArray );

        /**
        * C++ constructor.
        */
        CNSmlDSOperatorSettings();
        
        /**
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Get Repository key value of Operator specific profile.
        * @param aKey Repository key which value method returns.
        * @return Repository key value.
        */
        HBufC8* GetValueLC( const TUint32 aKey );

    private: // data
        CRepository* iRepository;

    };

#endif // __NSMLDSOPERATORSETTINGS_H

// End of File  
