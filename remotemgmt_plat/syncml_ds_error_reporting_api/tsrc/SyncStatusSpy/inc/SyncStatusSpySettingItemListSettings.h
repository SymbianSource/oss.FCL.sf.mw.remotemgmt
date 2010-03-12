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

#ifndef SYNCSTATUSSPYSETTINGITEMLISTSETTINGS_H
#define SYNCSTATUSSPYSETTINGITEMLISTSETTINGS_H
			

#include <e32std.h>



// FORWARD DECLARATIONS
class CErrorCodeNotifier;
class CProfileIdNotifier;
class CSyncTypeNotifier;
class CSyncInitiationNotifier;

/**
 * @class	TSyncStatusSpySettingItemListSettings SyncStatusSpySettingItemListSettings.h
 */
class CSyncStatusSpySettingItemListSettings : public CBase 
	{
public:
	// construct and destroy
	static CSyncStatusSpySettingItemListSettings* NewL();
	void ConstructL();
	~CSyncStatusSpySettingItemListSettings();
		
private:
	// constructor
	CSyncStatusSpySettingItemListSettings();

public:
	TInt& IntegerEditor1();
	void SetIntegerEditor1(const TInt& aValue);
	TInt& IntegerEditor2();
	void SetIntegerEditor2(const TInt& aValue);
	TInt& EnumeratedTextPopup2();
	void SetEnumeratedTextPopup2(const TInt& aValue);
	TInt& EnumeratedTextPopup1();
	void SetEnumeratedTextPopup1(const TInt& aValue);

protected:
	TInt iIntegerEditor1;
	TInt iIntegerEditor2;
	TInt iEnumeratedTextPopup2;
	TInt iEnumeratedTextPopup1;
	
	CErrorCodeNotifier* iErrorCodeNotifier;
	CProfileIdNotifier* iProfileIdNotifier;
	CSyncTypeNotifier* iSyncTypeNotifier;
	CSyncInitiationNotifier* iSyncInitiationNotifier;
	
	};
#endif // SYNCSTATUSSPYSETTINGITEMLISTSETTINGS_H
