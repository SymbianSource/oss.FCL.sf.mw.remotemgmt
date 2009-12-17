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



#ifndef INIFILEHELPER_HEADER_
#define INIFILEHELPER_HEADER_

// INCLUDES

#include <e32base.h>
#include <f32file.h> 
#include <s32strm.h> 
#include "CentRepToolClientServer.h"

// CONSTANTS

const TUint32 KBackupBitMask = 0x01000000;	//from the most significiant byte the least significiant bit defines is backup on or off

enum KSettingType
{
	EIndividualSetting = 0,
	EDefaultMeta,
	ERangeMetaSetting,
	ERangeMaskSetting,
	ERangeSetting,
	EMaskSetting,
};

enum KDefaultType
{
	EAlwaysPass,
	EAlwaysFail
};


// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS
// CLASS DECLARATION

class CSecuritySetting : public CBase
{
	public:
		//constructor
		CSecuritySetting();

		//
		TDesC& SecurityString( TDes &aSecurityString);
		virtual TDesC& SettingDefinition( TDes &aSecurityString) = 0;	//subclasses defines setting part
		
		//cap modifieng
		TInt AddSid( TUid aUid);
		TInt RemoveSid();
		TInt AddWriteCap( TUint8 aCab);
		TInt AddReadCap( TUint8 aCab);
		
		//copy function
		void CopyCapabilities( CSecuritySetting* aSetting);
		
		//compatible functions with TSecurityPolicy Set and Package functions
		void SetWr(const TDesC8& aDes);
		void PackageWr( TDes8& aPackage) const;
		void SetRd(const TDesC8& aDes);
		void PackageRd( TDes8& aPackage) const;

		//streaming functions
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);
		
		//util
		TBool ContainsSecuritySettings();
		KSettingType Type();
		
		TBool CheckAccess( const RMessage2& aMessage, TAccessType aAccessType);
	protected:
		TInt ReadCaps( TLex& aLex);	
		TInt AddSecurityAttributes( const TDesC& aType, const TDesC& aValue);
	private:
		TInt ConvertStrToCap( const TDesC& aCab, TUint8& aCapValue, KDefaultType& aType);
		TInt ConvertStrToSid( const TDesC& aStr, TUid& iSid);
		const TPtrC8 ConvertCapToStr( TUint8 aCab, KDefaultType& aType);

		enum ECapTypes
		{
			EWriteCaps,
			EReadCaps
		};

		void Set(const TDesC8& aDes, ECapTypes aCapType);
		void Package( TDes8& aPackage, ECapTypes aCapType) const;

	protected:
		KSettingType iType;
		KDefaultType iDefaultTypeWr;
		KDefaultType iDefaultTypeRd;

	private:
		TBool iSidExistWr;
		TUid iSidWr;
		TUint8 iCapWr[7];
		TBool iSidExistRd;
		TUid iSidRd;
		TUint8 iCapRd[7];
		
		friend class CSecuritySetting;
};

class CRangeSetting : public CSecuritySetting
{
	public:
		CRangeSetting(); 				
		CRangeSetting( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask);

		static CRangeSetting* NewL( TLex& aSecurityString); 				
		static CRangeSetting* NewL( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask); 				

		virtual TDesC& SettingDefinition( TDes &aSecurityString);	
		
		TUint32 Start() const;
		TUint32 End() const;
		TUint32 Mask() const;
		
		//streaming functions
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);
	protected:
	private:
		TDesC& RangeSettingDefinition( TDes &aSecurityString);	
		TDesC& MaskSettingDefinition( TDes &aSecurityString);	
		TInt ReadRangeSetting( TLex& aSecurityString);
		TInt ReadMaskSetting( TLex& aSecurityString);
	
		TUint32 iStart;
		TUint32 iEnd;
		TUint32 iMask;
		
		friend class CRepositorySession;
		friend class CRepositoryContent;
};

class CDefaultSetting : public CSecuritySetting
{
	public:
		//constructors
		CDefaultSetting();
		static CDefaultSetting* NewL( TLex& aSecurityString); 				
		
		//
		virtual TDesC& SettingDefinition( TDes &aSecurityString);
		
		//streaming functions
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);
	protected:
	private:
		TInt ReadSetting( TLex& aSecurityString);
};

class CIndividualSetting : public CSecuritySetting
{
	public:
		//constructors and destructors
		CIndividualSetting();
		CIndividualSetting( const TUint32& aSettingId);
		~CIndividualSetting();
		static CIndividualSetting* NewL( TLex& aSecurityString); 				
		
		
		
		//
		virtual TDesC& SettingDefinition( TDes &aSecurityString);

		//streaming	for setting data	
		void ExternalizeDataL(RWriteStream& aStream) const;
		void InternalizeDataL(RReadStream& aStream);

		//streaming	for setting data	
		void ExternalizePlatSecL(RWriteStream& aStream) const;
		void InternalizePlatSecL(RReadStream& aStream);

		
		//compare functions
		static TInt CompareElements( CIndividualSetting const& aSetting1, CIndividualSetting const& aSetting2);
	protected:
	private:
		void ReadSettingL( TLex& aSecurityString);
		
	private:
		//type definitions
		enum TSettingType { EInt, EReal, EString, EString8, EBinaryType};
	
		//setting values
		TPtrC iData;
		TUint32 iSettingId;
		TUint32 iMeta;
		
		union TSettingValue { TInt iIntegerValue; TReal iRealValue; HBufC8* iStringValue; } iSettingValue;
		TSettingType iSettingType;
		
		friend class CRepositorySession;
		friend class CRepositoryContent;
};

class CRangeMeta : public CBase
{
	public:
		CRangeMeta(); 				
		CRangeMeta( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask, TUint32 aMeta); 				

		static CRangeMeta* NewL( TLex& aSecurityString); 				
		static CRangeMeta* NewL( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask, TUint32 aMeta); 				

		TUint32 Start() const;
		TUint32 End() const;
		TUint32 Mask() const;
		KSettingType Type();
		
		//streaming functions
		void ExternalizeL(RWriteStream& aStream) const;
		void InternalizeL(RReadStream& aStream);
	protected:
	private:
		TInt ReadRangeSetting( TLex& aSecurityString);
		TInt ReadMaskSetting( TLex& aSecurityString);

		KSettingType iType;

	
		TUint32 iStart;
		TUint32 iEnd;
		TUint32 iMask;
		TUint32 iMeta;
		
		friend class CRepositorySession;
		friend class CRepositoryContent;
};


class TIniFileHelper 
{
	public:
		TIniFileHelper();
	
		//read and write functions
		HBufC* ReadFileL( RFile& aFile);
		TInt StartWrite( RFile& aFile);
		TInt WriteToFile( const TDesC& aContent);
		TInt LineFeed();
		TInt FinishWrite();
		
		//static helper functions
		static void NextLine( TPtrC& aContent, TPtrC& aNextLine);
		static TInt ReadNumber( TInt32& aNumber, TLex& aLex);
		static TInt ReadUNumber( TUint32& aNumber, TLex& aLex);
		static TInt ReadNumber( TReal& aNumber, TLex& aLex);
		static TInt CompareElements( CRangeSetting const& aElement1, CRangeSetting const& aElement2);
		static TInt CompareElements( CRangeMeta const& aElement1, CRangeMeta const& aElement2);
		
		static TInt StringReadL( TLex& aLex, HBufC8*& aString);
		static TInt String8ReadL( TLex& aLex, HBufC8*& aString);
		static TInt BinaryReadL( TLex& aLex, HBufC8*& aString);


	private:
		//member functions		
	private:
		//attributes
		RFile* iFile;
		TInt iSize;
		
#ifdef _DEBUG
		RFs iDebugFs;
		RFile iDebugFile;
#endif //_DEBUG
};


#endif 