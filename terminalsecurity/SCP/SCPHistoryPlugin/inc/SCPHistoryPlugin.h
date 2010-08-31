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


#ifndef __CSCPHISTORYPLUGIN_H
#define __CSCPHISTORYPLUGIN_H (0x1)

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
const TInt KHistoryCounterParamID = 	999;
const TInt KHistoryItemParamBase = 		1000;

const TInt KPasscodeHistoryBufferMinValue  = 0;
const TInt KPasscodeHistoryBufferMaxValue	= 65535;	

_LIT( KConfigFile,"SCPHistoryConfig.ini" );
_LIT( SCPHistoryPluginSrcFile, "\\Resource\\SCPHistoryPluginLang.rsc");
_LIT( KDriveZ, "z:" );


// FORWARD DECLARATIONS
class CSCPParamObject;

// CLASS DECLARATION

/**
*  CSCPHistoryPlugin 

*/

class CSCPHistoryPlugin : public CSCPPlugin
    {
    public: // Constructors, Destructor
		/**
        * 2-phased constructor
        * @return Instance of Plug-in
        */
	    static CSCPHistoryPlugin* NewL();

        /**
        * Destructor
        */
	    virtual ~CSCPHistoryPlugin();

        /**
        * Event handler...
        */		
		void HandleEventL( TInt aID, CSCPParamObject& aInParam , CSCPParamObject& aOutParam);
		
		void SetEventHandler( MSCPPluginEventHandler* aHandler );

    private: // Constructors, Destructor
		/**
        * Constructor
        */
	    CSCPHistoryPlugin();

		/**
        * 2nd phase constructor
        */
        void ConstructL();
               
    private: // Own internal methods
    
    	TInt GetHistoryArrayL( CDesCArrayFlat& array );
    	TInt GetHistoryItemCount( TInt& aHistoryCount);  
    	TInt SetConfigFile (); 
    	TInt AppendAndWriteSecurityCodeL ( TDes& aSecuritycode );
    	TInt GetResource();
     	TInt GetHistoryCountParamValue();
     	TInt FlushConfigFileL();
    	HBufC16* LoadResourceLC ( TInt aResId );
    	HBufC* LoadAndFormatResL( TInt aResId, TInt* aParam1 = NULL, TInt* aParam2 = NULL );
    	void FormatResourceString(HBufC16 &aResStr);

    private: // Data
        /* A pointer to the event handler object on the server, not owned */
        MSCPPluginEventHandler* iEventHandler;
        TFileName iCfgFilenamepath;
        RFs* iFs; // Eventhandler's session ptr (not owned)
        RResourceFile iRf;
        
    };

#endif // __CSCPHISTORYPLUGIN_H

// End of File
