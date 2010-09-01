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

#ifndef SYNCFWCUSTOMIZERSETTINGITEMLISTSETTINGS_H
#define SYNCFWCUSTOMIZERSETTINGITEMLISTSETTINGS_H
			
#include <e32std.h>

const int KEdit1MaxLength = 255;
const int KEdit2MaxLength = 255;
const int KEdit3MaxLength = 255;
const int KEdit4MaxLength = 255;

class CRepository;


/**
 * @class	TSyncFwCustomizerSettingItemListSettings SyncFwCustomizerSettingItemListSettings.h
 */
class TSyncFwCustomizerSettingItemListSettings
	{
public:
	// construct and destroy
	static TSyncFwCustomizerSettingItemListSettings* NewL(CRepository& aRepository);
	
private:
	// constructor
	TSyncFwCustomizerSettingItemListSettings(CRepository& aRepository);
    void ConstructL();

public:
	TDes& ServerUrl();
	void SetServerUrl(const TDesC& aValue);
	TInt& OperatorAdapterUid();
	void SetOperatorAdapterUid(const TInt& aValue);
	TInt& ProfileAdapterUid();
	void SetProfileAdapterUid(const TInt& aValue);
	TDes& SwV();
	void SetSwV(const TDesC8& aValue);
	TDes& Mod();
	void SetMod(const TDesC8& aValue);
	TInt& SyncProfileVisibility();
	void SetSyncProfileVisibility(const TInt& aValue);
	TDes& OperatorServerId();
	void SetOperatorServerId(const TDesC8& aValue);

protected:
    CRepository& iRepository; // Not owned
	TBuf<KEdit1MaxLength> iEdit1;
	TInt iSyncAdapterEditor;
	TInt iProfileAdapterUid;
	TBuf<KEdit2MaxLength> iSwV;
	TBuf<KEdit3MaxLength> iMod;
	TInt iSyncProfileVisibility;
	TBuf<KEdit4MaxLength> iOperatorServerId;	
	};

#endif // SYNCFWCUSTOMIZERSETTINGITEMLISTSETTINGS_H
