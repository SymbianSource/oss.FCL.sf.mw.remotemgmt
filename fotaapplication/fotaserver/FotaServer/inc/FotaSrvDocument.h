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
* Description:   Header file for CFotaSrvDocument
*
*/




#ifndef CFOTASRVDOCUMENT_H
#define CFOTASRVDOCUMENT_H

// INCLUDES
#include <aknapp.h>
#include <AknDoc.h>
#include <eikenv.h>

// FORWARD DECLARATIONS
class CEikAppUi;
class CEikEnv;
class CNSmlDMSyncProfileList;

// CLASS DECLARATION

/**
 *  CFotaSrvDocument application class.
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
class CFotaSrvDocument : public CAknDocument
    {
public: 

    /**
    * Two-phased constructor.
    */
    static CFotaSrvDocument* NewL(CAknApplication& aApp);

    /**
    * Destructor.
    */
    virtual ~CFotaSrvDocument();

public: // New functions

	/**
	* Getter fo CEikAppUi class
	* @param    None
	* @return   Pointer to the CEikAppUi
	*/
	CEikAppUi* AppUi() const { return iAppUi; }

private:


  	/**
	* Constructor
	* @param    aApp Used by akndocument
	* @return   None
	*/
    CFotaSrvDocument( CEikApplication& aApp ): CAknDocument( aApp ) { }


    void ConstructL();


  	/**
	* From CEikDocument, create CFotaSrvUi "App UI" object.
	* @param    None
	* @return   UI
	*/
    CEikAppUi* CreateAppUiL();

    
private:

    /**
     * Reference to eikonenv
     * 
     */
	//CEikonEnv*                          iEikEnv;
    };

#endif

// End of File
