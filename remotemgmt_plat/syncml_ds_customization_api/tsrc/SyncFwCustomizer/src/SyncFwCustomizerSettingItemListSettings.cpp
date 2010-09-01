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
 
#include <e32base.h>
#include <stringloader.h>
#include <barsread.h>
#include <SyncFwCustomizer.rsg>
#include "SyncFwCustomizerSettingItemListSettings.h"
#include <centralrepository.h>
#include <NSmlOperatorDataCRKeys.h>

/**
 * C/C++ constructor for settings data, cannot throw
 */
TSyncFwCustomizerSettingItemListSettings::TSyncFwCustomizerSettingItemListSettings(CRepository& aRepository):
    iRepository( aRepository )
	{
	}

/**
 * Two-phase constructor for settings data
 */
TSyncFwCustomizerSettingItemListSettings* TSyncFwCustomizerSettingItemListSettings::NewL(CRepository& aRepository)
	{
	TSyncFwCustomizerSettingItemListSettings* data = new( ELeave ) TSyncFwCustomizerSettingItemListSettings(aRepository);
	CleanupStack::PushL( data );
	data->ConstructL();
	CleanupStack::Pop( data );
	return data;
	}
	
/**
 *	Second phase for initializing settings data
 */
void TSyncFwCustomizerSettingItemListSettings::ConstructL()
	{
    TBuf<100> tmpDesc;  
    TBuf8<100> tmpDesc8;  
    TInt tmpInt = 0;
    
    iRepository.Get( KNsmlOpDsOperatorSyncServerURL, tmpDesc );
	SetServerUrl( tmpDesc );
		
	iRepository.Get( KNsmlOpDsOperatorAdapterUid, tmpInt );		
	SetOperatorAdapterUid( tmpInt ); 
	
    iRepository.Get( KNsmlOpDsProfileAdapterUid, tmpInt );     
    SetProfileAdapterUid( tmpInt ); 
	
    iRepository.Get( KNsmlOpDsDevInfoSwVValue, tmpDesc8 );
    SetSwV( tmpDesc8 );

    iRepository.Get( KNsmlOpDsDevInfoModValue, tmpDesc8 );
    SetMod( tmpDesc8 );

    iRepository.Get( KNsmlOpDsSyncProfileVisibility, tmpInt );
    SetSyncProfileVisibility( tmpInt );
    
    iRepository.Get( KNsmlOpDsOperatorSyncServerId, tmpDesc8 );
    SetOperatorServerId( tmpDesc8 );    
	}
	
TDes& TSyncFwCustomizerSettingItemListSettings::ServerUrl()
	{
	return iEdit1;
	}

void TSyncFwCustomizerSettingItemListSettings::SetServerUrl(const TDesC& aValue)
	{
	if ( aValue.Length() < KEdit1MaxLength)
		iEdit1.Copy( aValue );
	else
		iEdit1.Copy( aValue.Ptr(), KEdit1MaxLength);
	}

TInt& TSyncFwCustomizerSettingItemListSettings::OperatorAdapterUid()
	{
	return iSyncAdapterEditor;
	}

void TSyncFwCustomizerSettingItemListSettings::SetOperatorAdapterUid(const TInt& aValue)
	{
	iSyncAdapterEditor = aValue;
	}

TInt& TSyncFwCustomizerSettingItemListSettings::ProfileAdapterUid()
	{
	return iProfileAdapterUid;
	}

void TSyncFwCustomizerSettingItemListSettings::SetProfileAdapterUid(const TInt& aValue)
	{
    iProfileAdapterUid = aValue;
	}

TDes& TSyncFwCustomizerSettingItemListSettings::SwV()
	{
	return iSwV;
	}

void TSyncFwCustomizerSettingItemListSettings::SetSwV(const TDesC8& aValue)
	{
	iSwV.Copy( aValue );
	}

TDes& TSyncFwCustomizerSettingItemListSettings::Mod()
	{
	return iMod;
	}

void TSyncFwCustomizerSettingItemListSettings::SetMod(const TDesC8& aValue)
	{
	iMod.Copy( aValue );
	}

TInt& TSyncFwCustomizerSettingItemListSettings::SyncProfileVisibility()
	{
	return iSyncProfileVisibility;
	}

void TSyncFwCustomizerSettingItemListSettings::SetSyncProfileVisibility(const TInt& aValue)
	{
    iSyncProfileVisibility = aValue;
	}

TDes& TSyncFwCustomizerSettingItemListSettings::OperatorServerId()
	{
	return iOperatorServerId;
	}

void TSyncFwCustomizerSettingItemListSettings::SetOperatorServerId(const TDesC8& aValue)
	{
    iOperatorServerId.Copy( aValue );
	}

