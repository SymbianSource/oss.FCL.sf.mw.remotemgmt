/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/


#ifndef SETTING_ENFORCEMENT_INFO_HEADER_
#define SETTING_ENFORCEMENT_INFO_HEADER_


// INCLUDES
#include <e32base.h>
#include <ssl.h>

// CONSTANTS
enum KSettingEnforcements
	{
	EEMailEnforcement = 0,
	ESyncMLEnforcement,
	EDataSyncEnforcement,
	EWLANEnforcement,
	EAPEnforcement,
	EIMEnforcement,
	ECustomization,
	ETerminalSecurity,
	EApplicationManagement,
	EDCMOEnforcement
	};


// FORWARD DECLARATIONS
class CRepository;

// CLASS DECLARATION
class CSettingEnforcementInfo : public CBase
{
	private:
		CSettingEnforcementInfo();
		void ConstructL();
				
	public:	
		IMPORT_C ~CSettingEnforcementInfo();
		
		IMPORT_C static CSettingEnforcementInfo* NewL();
		
		IMPORT_C TInt EnforcementActive( KSettingEnforcements aEnforcementType, TBool& aEnforcementActive);
		IMPORT_C void EnforcementActiveL( const TDesC8& aUri, TInt& aEnforcementActive);
	private:	
		CRepository* iRepository;
};

#endif  //SETTING_ENFORCEMENT_INFO_HEADER_