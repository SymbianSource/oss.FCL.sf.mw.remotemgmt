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
* Description: Implementation of terminalsecurity components
*
*/


#ifndef __CSCPSPECIFICSTRINGSPLUGIN_H
#define __CSCPSPECIFICSTRINGSPLUGIN_H (0x1)

// INCLUDES
#include <e32base.h>
#include "SCPPlugin.h"
#include <TerminalControl3rdPartyAPI.h>
#include <badesca.h>		// for CDesCArrayFlat

// For the Localization
#include <eikenv.h>
#include <bautils.h>
#include <f32file.h>


// CONSTANTS
const TInt KSpecificStringsCounterParamID = 	999;
const TInt KSpecificStringsParamBase = 			1000;

_LIT( KConfigFileSpecific,"SCPSpecificStringsPlugin.ini" );
_LIT( SCPSpecificStringsPluginSrcFile, "\\Resource\\SCPHistoryPluginLang.rsc");
_LIT( KDriveZSpecific, "z:" );

// FORWARD DECLARATIONS
class CSCPParamObject;

// CLASS DECLARATION

/**
*  CSCPSpecificStringsPlugin 

*/


class CSCPSpecificStringsPlugin : public CSCPPlugin
    {
    public: // Constructors, Destructor
		/**
        * 2-phased constructor
        * @return Instance of Plug-in
        */
	    static CSCPSpecificStringsPlugin* NewL();

        /**
        * Destructor
        */
	    virtual ~CSCPSpecificStringsPlugin();

        /**
        * Event handler...
        */		
		void HandleEventL( TInt aID, CSCPParamObject& aParam,CSCPParamObject& aOutParam );
		
		void SetEventHandler( MSCPPluginEventHandler* aHandler );

    private: // Constructors, Destructor
		/**
        * Constructor
        */
	    CSCPSpecificStringsPlugin();

		/**
        * 2nd phase constructor
        */
        void ConstructL();
        
     private: // Own internal methods	
        	TInt CheckSpecificStrings( TDes& aSecuritycode, TBool& aForbiddenSecurityCode );
      		TInt ParseAndStoreL( TDes& aForbiddenSecurityCodes);
      		TInt ParseAndRemoveL( TDes& aForbiddenSecurityCodes );
      		TInt FlushConfigFileL();
      		TInt SetConfigFile (); 
      		HBufC16* LoadResourceLC ( TInt aResId );
     		TInt GetResource();
     		
     		TInt GetSpecificStringsArrayL ( CDesCArrayFlat& array );
     		TInt IsNotValidWord ( TDes& aForbiddenSecurityCodes );
     		void RemoveDuplicateWords( CDesCArrayFlat& array );
     		
     		HBufC* LoadAndFormatResL( TInt aResId,
                                      TInt* aParam1 = NULL, 
                                      TInt* aParam2 = NULL );
        
    private: // Data
        /* A pointer to the event handler object on the server, not owned */
        MSCPPluginEventHandler* iEventHandler;
        TFileName iCfgFilenamepath;
        RFs* iFs; // Eventhandler's session ptr (not owned)
        RResourceFile iRf;
        
    };

#endif // __CSCPSPECIFICSTRINGSPLUGIN_H

// End of File
