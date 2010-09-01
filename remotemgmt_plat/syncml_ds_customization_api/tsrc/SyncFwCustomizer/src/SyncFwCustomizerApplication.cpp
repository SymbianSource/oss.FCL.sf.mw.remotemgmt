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

#include "SyncFwCustomizerApplication.h"
#include "SyncFwCustomizerDocument.h"

#ifdef EKA2
#include <eikstart.h>
#endif

/**
 * @brief Returns the application's UID (override from CApaApplication::AppDllUid())
 * @return UID for this application (KUidSyncFwCustomizerApplication)
 */
TUid CSyncFwCustomizerApplication::AppDllUid() const
	{
	return KUidSyncFwCustomizerApplication;
	}

/**
 * @brief Creates the application's document (override from CApaApplication::CreateDocumentL())
 * @return Pointer to the created document object (CSyncFwCustomizerDocument)
 */
CApaDocument* CSyncFwCustomizerApplication::CreateDocumentL()
	{
	return CSyncFwCustomizerDocument::NewL( *this );
	}

#ifdef EKA2

/**
 *	@brief Called by the application framework to construct the application object
 *  @return The application (CSyncFwCustomizerApplication)
 */	
LOCAL_C CApaApplication* NewApplication()
	{
	return new CSyncFwCustomizerApplication;
	}

/**
* @brief This standard export is the entry point for all Series 60 applications
* @return error code
 */	
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}
	
#else 	// Series 60 2.x main DLL program code

/**
* @brief This standard export constructs the application object.
* @return The application (CSyncFwCustomizerApplication)
*/
EXPORT_C CApaApplication* NewApplication()
	{
	return new CSyncFwCustomizerApplication;
	}

/**
* @brief This standard export is the entry point for all Series 60 applications
* @return error code
*/
GLDEF_C TInt E32Dll(TDllReason /*reason*/)
	{
	return KErrNone;
	}

#endif // EKA2
