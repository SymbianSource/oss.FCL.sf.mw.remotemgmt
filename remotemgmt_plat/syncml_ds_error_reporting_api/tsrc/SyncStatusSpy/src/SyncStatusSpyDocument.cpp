/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "SyncStatusSpyDocument.h"
#include "SyncStatusSpyAppUi.h"

/**
 * @brief Constructs the document class for the application.
 * @param anApplication the application instance
 */
CSyncStatusSpyDocument::CSyncStatusSpyDocument( CEikApplication& anApplication )
	: CAknDocument( anApplication )
	{
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here.  
 */ 
void CSyncStatusSpyDocument::ConstructL()
	{
	}
	
/**
 * Symbian OS two-phase constructor.
 *
 * Creates an instance of CSyncStatusSpyDocument, constructs it, and
 * returns it.
 *
 * @param aApp the application instance
 * @return the new CSyncStatusSpyDocument
 */
CSyncStatusSpyDocument* CSyncStatusSpyDocument::NewL( CEikApplication& aApp )
	{
	CSyncStatusSpyDocument* self = new ( ELeave ) CSyncStatusSpyDocument( aApp );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * @brief Creates the application UI object for this document.
 * @return the new instance
 */	
CEikAppUi* CSyncStatusSpyDocument::CreateAppUiL()
	{
	return new ( ELeave ) CSyncStatusSpyAppUi;
	}
				
