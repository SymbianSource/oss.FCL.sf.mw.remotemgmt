/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*  
*
*/



#ifndef PMRFSPLUGIN_H__
#define PMRFSPLUGIN_H__

//  INCLUDES
#include <e32base.h>
#include <rfsPlugin.h>

// CLASS DECLARATION

class CPMRfsPlugin : public CRFSPlugin
    {
public:

    /**
    * Two-phased constructor.
	* @param	aInitParams initial parameters
    */
    static CPMRfsPlugin* NewL(TAny* aInitParams);

    /**
    * Destructor.
    */
    virtual ~CPMRfsPlugin();

    /**
    * 
	* @param
    */
    void RestoreFactorySettingsL( const TRfsReason aType );

    /**
    * 
	* @param
    */
    void GetScriptL( const TRfsReason aType, TDes& aPath );

    /**
    * 
	* @param
    */
    void ExecuteCustomCommandL( const TRfsReason aType, TDesC& aCommand );
private:

    /**
    * C++ default constructor.
    */
    CPMRfsPlugin();

    /**
    * Constructor.
    * @param	aInitParams initial parameters
	*/
	CPMRfsPlugin(TAny* aInitParams);

    /**
    * Constructor
    */
    void ConstructL();
    
   
    /*
    * Check if the format MMC card flag is enabled
    */
    TBool IsMMCFormatFlagEnabledL();
    #ifdef RD_MULTIPLE_DRIVE
    TFileName iFileName;
    #endif //RD_MULTIPLE_DRIVE
    };



#endif      // PMRFSPLUGIN_H__

// End of File
