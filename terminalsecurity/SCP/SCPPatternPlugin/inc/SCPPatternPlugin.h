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

#ifndef __CSCPPATTERNPLUGIN_H
#define __CSCPPATTERNPLUGIN_H (0x1)

// INCLUDES
#include <e32base.h>
#include "SCPPlugin.h"

#include <TerminalControl3rdPartyAPI.h>


// For the Localization
#include <eikenv.h>
#include <bautils.h>
#include <f32file.h>
#include <barsread.h>



// FORWARD DECLARATIONS
class CSCPParamObject;


// CONSTANTS
_LIT(SCPPatternPluginSrcFile, "\\Resource\\SCPPatternPluginLang.rsc");
_LIT( KDriveZ, "z:" );

// The max. value for EPasscodeMaxRepeatedCharacters
const TInt KSCPMaxRepeatAmount( 4 );

// The max. value for EPasscodeMinSpecialCharacters
const TInt KSCPMinSpecialAmount( 255 );

// CLASS DECLARATION

/**
*  CSCPPatternPlugin 

*/
class CSCPPatternPlugin : public CSCPPlugin
    {
    public: // Constructors, Destructor
		/**
        * 2-phased constructor
        * @return Instance of Plug-in
        */
	    static CSCPPatternPlugin* NewL();

        /**
        * Destructor
        */
	    virtual ~CSCPPatternPlugin();

        /**
        * Event handler...
        */		
	    void CSCPPatternPlugin::HandleEventL( TInt aID, CSCPParamObject& aParam,CSCPParamObject& aOutParam );
		
		void SetEventHandler( MSCPPluginEventHandler* aHandler );

    private: // Constructors, Destructor
		/**
        * Constructor
        */
	    CSCPPatternPlugin();

		/**
        * 2nd phase constructor
        */
        void ConstructL();
        
        
     private : // Own internal implementation
     	TBool TooManySameCharsL ( TDes& aParam, TInt  aMaxRepeatedCharacters );
     	TBool consecutivelyCheck ( TDes& aParam );
     	HBufC16* LoadResourceLC( TInt aResId );
     	HBufC* LoadAndFormatResL( TInt aResId, TInt* aParam1 = NULL, TInt* aParam2 = NULL );
     	TInt GetResource();
     	void FormatResourceString(HBufC16 &aResStr);
   
        
        
    private: // Data
        /* A pointer to the event handler object on the server, not owned */
        MSCPPluginEventHandler* iEventHandler;
       
        RFs* iFs; // Eventhandler's session ptr (not owned)
        RResourceFile iRf;
        
    
    };

#endif // __CSCPPATTERNPLUGIN_H

// End of File
