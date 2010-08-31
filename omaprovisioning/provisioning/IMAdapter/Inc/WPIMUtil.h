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
* Description: 
*     Utility class for reading resource strings.
*
*/


#ifndef WPIMUTIL_H
#define WPIMUTIL_H

// INCLUDES
#include <e32base.h>
#include <impssapsettings.h>

// CONSTANTS
#if ( defined (__WINS__) || defined (__WINSCW) ) // this different on hw
    _LIT( KWVAdapterName, "WPWVAdapterResource" );
#else
    _LIT( KWVAdapterName, "WPWVAdapter" );
#endif
_LIT( KIMAppID1, "wa" );
_LIT( KIMAppID2, "wA" );
_LIT( KServicesName, "SERVICES");
_LIT( KIMName, "IM");
_LIT( KPresenceName, "PR");

const TUint32 KNotSet = 0xffffffff;

// FORWARD DECLARATIONS
class RResourceFile;
class CWPCharacteristic;
// CLASS DECLARATION

/**
 * TData stores settings.
 */
class TData
	{
	public:
	    
	    /**
        * C++ default constructor.
        */
		TData();
		
		/**
        * Destructor.
        */
		virtual ~TData();
		
	public: // data, nothing owned
		TPtrC                               iName;
		TPtrC                               iURL;
		TPtrC                               iUserID;
		TPtrC                               iPassword;
		RPointerArray<CWPCharacteristic>    iLinks;
		TPckgBuf<TUint32>                   iSAPId;
		TIMPSAccessGroup                    iProvLocation;
	};

/**
 * WPIMUtil is a utility class for reading resource strings.
 *
 * @lib WPIMAdapter
 * @since 2.0
 */ 
class WPIMUtil
	{
	public: // New functions

        /**
        * Check does the given descriptor contain any illegal character
        * from 0x00 to 0x1F.
        * @param aDes Descritor to be checked
        * @return TBool Did the descriptor contain any illegal chars
        */
        static TBool HasIllegalChars( const TDesC& aDes );
			
        static TUint32 FindGPRSL( RPointerArray<CWPCharacteristic>& aLinks );
        
        /**
        * Checks the validity of current data
        * @return TBool was the settings data valid or not.
        */
        static TBool IsValid( TData* aCurrentData );

	};

#endif	// WPWAPUTIL_H
            
// End of File
