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

#ifndef SYNCFWCUSTOMIZERAPPLICATION_H
#define SYNCFWCUSTOMIZERAPPLICATION_H

#include <aknapp.h>

const TUid KUidSyncFwCustomizerApplication = { 0x2001FDF4 };

/**
 *
 * @class	CSyncFwCustomizerApplication SyncFwCustomizerApplication.h
 * @brief	A CAknApplication-derived class is required by the S60 application 
 *          framework. It is subclassed to create the application's document 
 *          object.
 */
class CSyncFwCustomizerApplication : public CAknApplication
	{
private:
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();	
	};
			
#endif // SYNCFWCUSTOMIZERAPPLICATION_H		
