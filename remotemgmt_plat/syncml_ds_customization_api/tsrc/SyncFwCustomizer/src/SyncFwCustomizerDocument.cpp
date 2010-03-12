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

#include "SyncFwCustomizerDocument.h"
#include "SyncFwCustomizerAppUi.h"

/**
 * @brief Constructs the document class for the application.
 * @param anApplication the application instance
 */
CSyncFwCustomizerDocument::CSyncFwCustomizerDocument( CEikApplication& anApplication )
	: CAknDocument( anApplication )
	{
	}

/**
 * @brief Completes the second phase of Symbian object construction. 
 * Put initialization code that could leave here.  
 */ 
void CSyncFwCustomizerDocument::ConstructL()
	{
	}
	
/**
 * Symbian OS two-phase constructor.
 *
 * Creates an instance of CSyncFwCustomizerDocument, constructs it, and
 * returns it.
 *
 * @param aApp the application instance
 * @return the new CSyncFwCustomizerDocument
 */
CSyncFwCustomizerDocument* CSyncFwCustomizerDocument::NewL( CEikApplication& aApp )
	{
	CSyncFwCustomizerDocument* self = new ( ELeave ) CSyncFwCustomizerDocument( aApp );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

/**
 * @brief Creates the application UI object for this document.
 * @return the new instance
 */	
CEikAppUi* CSyncFwCustomizerDocument::CreateAppUiL()
	{
	return new ( ELeave ) CSyncFwCustomizerAppUi;
	}
				
