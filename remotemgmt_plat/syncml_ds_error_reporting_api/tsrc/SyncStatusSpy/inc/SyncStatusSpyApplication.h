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

#ifndef SYNCSTATUSSPYAPPLICATION_H
#define SYNCSTATUSSPYAPPLICATION_H


#include <aknapp.h>



const TUid KUidSyncStatusSpyApplication = { 0x2001FDF3 };

/**
 *
 * @class	CSyncStatusSpyApplication SyncStatusSpyApplication.h
 * @brief	A CAknApplication-derived class is required by the S60 application 
 *          framework. It is subclassed to create the application's document 
 *          object.
 */
class CSyncStatusSpyApplication : public CAknApplication
	{
private:
	TUid AppDllUid() const;
	CApaDocument* CreateDocumentL();
	
	};
			
#endif // SYNCSTATUSSPYAPPLICATION_H		
