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

#ifndef SYNCFWCUSTOMIZERDOCUMENT_H
#define SYNCFWCUSTOMIZERDOCUMENT_H

#include <akndoc.h>
		
class CEikAppUi;

/**
* @class	CSyncFwCustomizerDocument SyncFwCustomizerDocument.h
* @brief	A CAknDocument-derived class is required by the S60 application 
*           framework. It is responsible for creating the AppUi object. 
*/
class CSyncFwCustomizerDocument : public CAknDocument
	{
public: 
	// constructor
	static CSyncFwCustomizerDocument* NewL( CEikApplication& aApp );

private: 
	// constructors
	CSyncFwCustomizerDocument( CEikApplication& aApp );
	void ConstructL();
	
public: 
	// from base class CEikDocument
	CEikAppUi* CreateAppUiL();
	};

#endif // SYNCFWCUSTOMIZERDOCUMENT_H
