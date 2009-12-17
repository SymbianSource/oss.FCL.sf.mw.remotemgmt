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
* Description: Implementation of policymanagement components
*
*/


#ifndef CONSTANTS_HEADER__
#define CONSTANTS_HEADER__

namespace IniConstants
{
	const TUint8 KPersistsVersion = 0;


	_LIT( KCentRepToolPanic, "CentRepToolPanic");

	const TInt KFileOpenAttemps = 20;
	const TInt KFileOpenPeriod = 50;
	
	const TInt KSettingNameLength = 10;
	const TUint8 KSettingEndMark = 10;
	const TInt KUidLengthRep = 8;
	const TInt KUidLengthSet = 10;
	const TInt KAccessStringLength = 6;
	const TInt KPathLength = 80;
	const TInt KFileNameLength = 12;

	const TUint16 KUcs2Bom = 0xfeff;

	const TPtrC KReadAccessSidString( _L( "sid_rd"));
	const TPtrC KReadAccessCapString( _L( "cap_rd"));
	const TPtrC KWriteAccessSidString( _L( "sid_wr"));
	const TPtrC KWriteAccessCapString( _L( "cap_wr"));
	const TPtrC8 KAccessAlwaysPass( _L8( "alwayspass"));
	const TPtrC8 KAccessAlwaysFail( _L8( "alwaysfail"));
	
	_LIT(KTypeInt, "int");
	_LIT(KTypeReal, "real");
	_LIT(KTypeString, "string");
	_LIT(KTypeString8, "string8");
	_LIT(KTypeBinary, "binary");



	_LIT( KRepositoryFileExtensionTxt, ".txt");
	_LIT( KRepositoryFileExtensionCre, ".cre");
	
	//for construction backup...
	_LIT( KRepositoryFileExtensionTmp, ".tmp");
	
	
	_LIT( KDefaultMetaSection, "[defaultmeta]");
	const TInt KDefaultMetaStringLength = 13; 

	_LIT( KOwnerSection, "[owner]");
	const TInt KOwnerStringLength = 7; 

	
	_LIT( KPlatSecSection, "[platsec]");
	const TInt KPlatSecStringLength = 9; 
	_LIT( KMainSection, "[main]");
	_LIT( KMask, "mask");
	const TInt KMainSectionStringLength = 6; 
	
	_LIT( KDriveC, "C:\\private\\10202be9\\persists\\");
	_LIT( KDriveZ, "Z:\\private\\10202be9\\");

	_LIT( KBackupPath, "C:\\private\\10207819\\");
	_LIT( KTempFile, "rep.tmp");





////FROM CENTREP 


const TInt KPrivatePathLen = 25; //20 for path + 5 for wildcard
_LIT(KRepositoryFilesWildcard, "*.txt");


_LIT(KSignature, "cenrep");
LOCAL_D const TInt KSignatureLen = 6;
_LIT(KVersion, "version");
LOCAL_D const TInt KVersionLen = 7;
LOCAL_D const TUint KCurrentVersion = 1;

LOCAL_D const TInt KMaxTypeLen = 6;
LOCAL_D const TChar KNullDataIndicator = '-';

// PlatSec identifiers in the ini file
const TInt KPlatSecSectionLen = 9;
const TInt KMainSectionLen = 6;
//const TInt KIniFileSectionLen = MAX(KPlatSecSectionLen,KMainSectionLen);
_LIT(KMaskString, "mask");
const TInt KMaskLen = 4;
//_LIT(KReadAccessSidString, "sid_rd");
//_LIT(KReadAccessCapString, "cap_rd");
//_LIT(KWriteAccessSidString, "sid_wr");
//_LIT(KWriteAccessCapString, "cap_wr");
//_LIT(KAccessAlwaysPass, "alwayspass");

const TInt KMaxAccessTypeLen = 6; // could do max of _LITs above
const TInt KMaxCapabilityStringLen = 20; // longest capability string from CapabilityNames is 15

	
}

#endif