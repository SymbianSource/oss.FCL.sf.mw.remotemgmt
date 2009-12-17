/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDE FILES

#define __INCLUDE_CAPABILITY_NAMES__

#include "IniFileHelper.h"
#include "constants.h"
#include "debug.h"


#include <e32capability.h>
#include <centralrepository.h>
#include <utf.h>


// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

static const TChar KNullDataIndicator = '-';
static const TInt KMaxCapCount = 7;
static const TInt KMaxCapCountSidExist = 3;

// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES


// ==================== LOCAL FUNCTIONS =================
// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSecuritySetting::AddSid()
// -----------------------------------------------------------------------------
//

CSecuritySetting::CSecuritySetting()
{
	iSidExistWr = iSidExistRd = EFalse;
	
	for ( TInt i(0); i < KMaxCapCount; i++)
	{
		iCapWr[i] = iCapRd[i] = TUint8(ECapability_None);
	}
}


// -----------------------------------------------------------------------------
// CSecuritySetting::SecurityString()
// -----------------------------------------------------------------------------
//
TDesC& CSecuritySetting::SecurityString( TDes &aSecurityString)
{
	using namespace IniConstants;

	aSecurityString.Zero();	
	TBuf<30> cap;
	
	TInt capCount = KMaxCapCount;
	
	if ( iSidExistRd)
	{

		//create sid_wr=0x000000 string, where 0x000000 is aSecurityId  
		TBuf< KUidLengthSet> name;
		name.Num( iSidRd.iUid, EDecimal);
	
		aSecurityString.Append( KReadAccessSidString );
		aSecurityString.Append( _L("=") );
		aSecurityString.Append( name );
			aSecurityString.Append(' ');
			
		capCount = KMaxCapCountSidExist;	
	}
	
	for ( TInt i(0); i < capCount; i++)
	{
		if ( iCapRd[ i] != TUint8(ECapability_None) )
		{
			aSecurityString.Append( KReadAccessCapString);
			aSecurityString.Append('=');
			cap.Copy( ConvertCapToStr( iCapRd[ i], iDefaultTypeRd));
			aSecurityString.Append( cap);
			aSecurityString.Append(' ');
		}
	}

	capCount = KMaxCapCount;
	if ( iSidExistWr)
	{

		//create sid_wr=0x000000 string, where 0x000000 is aSecurityId  
		TBuf< KUidLengthSet> name;
		name.Num( iSidWr.iUid, EDecimal);
	
		aSecurityString.Append( KWriteAccessSidString );
		aSecurityString.Append( _L("=") );
		aSecurityString.Append( name );
		aSecurityString.Append(' ');

		capCount = KMaxCapCountSidExist;	
	}

	for ( TInt i(0); i < capCount; i++)
	{
		if ( iCapWr[ i] != TUint8(ECapability_None) )
		{
			aSecurityString.Append( KWriteAccessCapString);
			aSecurityString.Append('=');
			cap.Copy( ConvertCapToStr( iCapWr[ i], iDefaultTypeWr));
			aSecurityString.Append( cap);
			aSecurityString.Append(' ');
		}
	}


	return aSecurityString;
}


// -----------------------------------------------------------------------------
// CSecuritySetting::ContainsSecuritySettings()
// -----------------------------------------------------------------------------
//
TBool CSecuritySetting::ContainsSecuritySettings()
{
	return iSidExistWr || iSidExistRd || iCapWr[0] != TUint8(ECapability_None) || iCapRd[0] != TUint8(ECapability_None);
}


// -----------------------------------------------------------------------------
// CSecuritySetting::CheckAccess()
// -----------------------------------------------------------------------------
//
TBool CSecuritySetting::CheckAccess( const RMessage2& aMessage, TAccessType aAccessType)
{
	TBool retVal = ETrue;
	
	if ( aAccessType == EReadAccess || aAccessType == EAccessBoth)
	{
		TInt capCount = KMaxCapCount;
		if ( iSidExistRd)
		{
			capCount = KMaxCapCountSidExist;
			if ( aMessage.SecureId().iId != iSidRd.iUid)
			{
				return EFalse;
			}
		}
		
		for ( TInt i(0); i < capCount; i++)
		{
			if ( iCapRd[ i] != TUint8(ECapability_None))
			{
				if ( !aMessage.HasCapability( TCapability( iCapRd[ i]), KSuppressPlatSecDiagnosticMagicValue))
				{
					return EFalse;
				}
			}
			
		}
	}
	
	if ( aAccessType == EWriteAccess || aAccessType == EAccessBoth)
	{
		TInt capCount = KMaxCapCount;
		if ( iSidExistWr)
		{
			capCount = KMaxCapCountSidExist;
			if ( aMessage.SecureId().iId != iSidWr.iUid)
			{
				return EFalse;
			}
		}
		
		for ( TInt i(0); i < capCount; i++)
		{
			if ( iCapWr[ i] != TUint8(ECapability_None))
			{
				if ( !aMessage.HasCapability( TCapability( iCapWr[ i]), KSuppressPlatSecDiagnosticMagicValue))
				{
					return EFalse;
				}
			}
			
		}
	}

	return retVal;
}



// -----------------------------------------------------------------------------
// CSecuritySetting::AddSid()
// -----------------------------------------------------------------------------
//
KSettingType CSecuritySetting::Type()
{
	return iType;	
}


// -----------------------------------------------------------------------------
// CSecuritySetting::AddSid()
// -----------------------------------------------------------------------------
//
TInt CSecuritySetting::AddSid( TUid aUid)
{
	if ( iSidExistWr)
	{
		return KErrAlreadyExists;
	}

	//enable and set sid
	iSidWr = aUid;
	iSidExistWr = ETrue;
		
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CSecuritySetting::RemoveSid()
// -----------------------------------------------------------------------------
//
TInt CSecuritySetting::RemoveSid()
{
	//disable sid...
	iSidExistWr = EFalse;

	return KErrNone;
}

void CSecuritySetting::CopyCapabilities( CSecuritySetting* aSetting)
{
	//Copy all capabilities from parameter object to this object
	iSidExistWr = aSetting->iSidExistWr;
	iSidWr = aSetting->iSidWr;
	iSidExistRd = aSetting->iSidExistRd;
	iSidRd = aSetting->iSidRd;

	for ( TInt i(0); i < KMaxCapCount; i++)
	{
		iCapRd[i] = aSetting->iCapRd[i];
		iCapWr[i] = aSetting->iCapWr[i];
	}
}


// -----------------------------------------------------------------------------
// CSecuritySetting::RemoveSid()
// -----------------------------------------------------------------------------
//

TInt CSecuritySetting::ReadCaps( TLex& aLex)
{
	//read capablities, format: cap_type=cap_value
	using namespace IniConstants;
	
	TLex lex( aLex);
	lex.SkipSpace();
	TInt err( KErrNone);

	//read until TLex reaches Eos...
	while ( !lex.Eos() && err == KErrNone)
	{	
		//Mark start point
		TLexMark16 mark;
		lex.Mark( mark);
		lex.SkipSpace();

		//get next token, contains capability type
		TPtrC token = lex.NextToken();;
		TPtrC typePtr( token); 
		TPtrC capOrSid;
	
		//format could be "cap_type=..." or "cap_type =...", try to locate '='-mark from token
		TInt index = token.Locate('=');
		
		if ( index >= 0)
		{
			typePtr.Set( token.Left( index));
			token.Set( token.Mid( index + 1));
		} 
		
		//read capabilities from. There could be max 3 caps separated with ','
		TBool capOrSidDetected = EFalse;
		TBool ready = EFalse;
		do
		{
			//read new token if current token is empty
			if ( !token.Length())
			{
				TLexMark subMark;
				lex.Mark( subMark);
		
				token.Set( lex.NextToken());
				
				//if token contains only ',', then get new one
				if ( token.Length() == 1 && token[0] == ',')
				{
					token.Set( lex.NextToken());
				}
				
				//check if capability type changes (e.g. cap_wr -> cap_rd)
				TPtrC ptr( token);
				TInt index = token.Locate('=');
				if ( index >= 0)
				{
					ptr.Set( token.Left(index));
				}
				
				if ( ptr.CompareF( KReadAccessSidString) == 0  ||
					 ptr.CompareF( KReadAccessCapString) == 0  ||
					 ptr.CompareF( KWriteAccessSidString) == 0 ||
					 ptr.CompareF( KWriteAccessCapString) == 0 )
				{
					token.Set( KNullDesC);
					lex.UnGetToMark( subMark);					
					
					if ( !capOrSidDetected)
					{
						return KErrCorrupt;
					}
					
					ready = ETrue;
					continue;
				}
			}
		
			//read capability or SID
			if ( token.Length() )
			{
				//rome ','-mark if exist
				TInt index = token.Locate(',');		
				if ( index >= 0 )
				{
					capOrSid.Set( token.Left( index ));
					token.Set( token.Mid( index + 1));
				}
				else
				{
					capOrSid.Set( token);
					token.Set( KNullDesC);
				}
				
				//and cap or SID found add it to the setting
				if ( capOrSid.Length())
				{
					capOrSidDetected = ETrue;
					err = AddSecurityAttributes( typePtr, capOrSid);
				}
			}
			else
			{
				ready = ETrue;
			}

		} while ( !ready && err == KErrNone);
			
		lex.SkipSpace();
	}
	
	return err;
}


// -----------------------------------------------------------------------------
// CSecuritySetting::AddSecurityAttributes()
// -----------------------------------------------------------------------------
//
TInt CSecuritySetting::AddSecurityAttributes( const TDesC& aType, const TDesC& aValue)
{
	using namespace IniConstants;
	
	TInt err = KErrGeneral;

	if (  KWriteAccessCapString == aType )
	{
		TUint8 cap;
		if ( ConvertStrToCap( aValue, cap, iDefaultTypeWr) != KErrNone )
		{
			return KErrGeneral;
		}
		err = AddWriteCap( cap );
	}
	else if ( KReadAccessCapString == aType )
	{
		TUint8 cap;
		if ( ConvertStrToCap( aValue, cap, iDefaultTypeRd) != KErrNone )
		{
			return KErrGeneral;
		}
		err = AddReadCap( cap );
	}
	else if ( KReadAccessSidString == aType)
	{
		if ( !iSidExistRd )		
		{
			if ( KErrNone == ConvertStrToSid( aValue, iSidRd))
			{
				iSidExistRd = ETrue;
				err = KErrNone;
			}
		}		
	}
	else if ( KWriteAccessSidString == aType)
	{
		if ( !iSidExistWr )		
		{
			if ( KErrNone == ConvertStrToSid( aValue, iSidWr))
			{
				iSidExistWr = ETrue;
				err = KErrNone;
			}
		}
	}
	
	return err;	
}




// -----------------------------------------------------------------------------
// CSecuritySetting::ConvertStrToCab()
// -----------------------------------------------------------------------------
//
TInt CSecuritySetting::ConvertStrToSid( const TDesC& aStr, TUid& aValue)
{
	TLex lex( aStr);
	
	TRadix radix = EDecimal;
	if( lex.Peek()=='0')
	{
		lex.Inc();
		if( lex.Peek().GetLowerCase()=='x')
		{
			lex.Inc();
			radix = EHex;
		}
		else
		{
			lex.UnGet();
		}
	}

	TUint32 value;
	TInt err = lex.Val( value, radix);
	aValue.iUid = value;
	return err;
}


// -----------------------------------------------------------------------------
// CSecuritySetting::ConvertStrToCab()
// -----------------------------------------------------------------------------
//
TInt CSecuritySetting::ConvertStrToCap( const TDesC& aCab, TUint8& aCapValue, KDefaultType& aType)
{
	
	TBuf8<40> cap;
	cap.Append( aCab);
	cap.LowerCase();
	
	if ( cap.CompareF( IniConstants::KAccessAlwaysPass ) == 0)
	{
		aType = EAlwaysPass;
		aCapValue = TUint8(ECapability_None);
		return KErrNone;
	}

	if ( cap.CompareF( IniConstants::KAccessAlwaysFail ) == 0)
	{
		aType = EAlwaysFail;
		aCapValue = TUint8(ECapability_None);
		return KErrNone;
	}


	for ( TInt i(0); i < ECapability_Limit;  i++)	
	{
		TPtrC8 ptr((const TUint8 *) CapabilityNames[i]);
		
		TBool compareResult = ETrue;
		for ( TInt j(0); j < ptr.Length(); j++)
		{
			TChar ptrChar( ptr[j]);
			TChar aCabChar( aCab[j]);
			ptrChar.LowerCase();
			aCabChar.LowerCase();
		
			if ( ptrChar != aCabChar )
			{
				compareResult = EFalse;
				break;
			}
		}
		
	
		if ( compareResult )
		{
			aCapValue = i;
			return KErrNone;
		}
	}
	
	return KErrNotFound;
}


// -----------------------------------------------------------------------------
// CSecuritySetting::ConvertCabToStr()
// -----------------------------------------------------------------------------
//
const TPtrC8 CSecuritySetting::ConvertCapToStr( TUint8 aCab, KDefaultType& aType)
{
	if ( aCab == TUint8(ECapability_None))
	{
		if ( aType == EAlwaysPass)
		{
			return IniConstants::KAccessAlwaysPass;
			}
		else
		{
			return IniConstants::KAccessAlwaysFail;
		}
	}

	return (const TUint8 *)CapabilityNames[ aCab];	
}

// -----------------------------------------------------------------------------
// CSecuritySetting::SetWriteCab()
// -----------------------------------------------------------------------------
//
TInt CSecuritySetting::AddWriteCap( TUint8 aCab)
{
	TInt capCount = KMaxCapCount;
	if ( iSidExistWr)
	{
		capCount = KMaxCapCountSidExist;
	}

	for ( TInt i(0); i < capCount; i++)
	{
		if ( iCapWr[i] == TUint8(ECapability_None) )
		{
			iCapWr[i] = aCab;
			return KErrNone;
		}
	}
	
	return KErrGeneral;
}


// -----------------------------------------------------------------------------
// CSecuritySetting::SetReadeCab()
// -----------------------------------------------------------------------------
//
TInt CSecuritySetting::AddReadCap( TUint8 aCab)
{
 	TInt capCount = KMaxCapCount;
	if ( iSidExistWr)
	{
		capCount = KMaxCapCountSidExist;
	}

	for ( TInt i(0); i < capCount; i++)
	{
		if ( iCapWr[i] == TUint8(ECapability_None) )
		{
			iCapRd[i] = aCab;
			return KErrNone;
		}
	}
	
	return KErrGeneral;
}



// -----------------------------------------------------------------------------
// CSecuritySetting::Set()
// 
// -----------------------------------------------------------------------------
//
void CSecuritySetting::Set(const TDesC8& aDes, ECapTypes aCapType)
{
	TBool *sidExist = &iSidExistRd;
	TUid *sid = &iSidRd;
	TUint8 *cap = iCapRd;
	KDefaultType *defaultType = &iDefaultTypeRd;
	
	if ( aCapType == EWriteCaps )
	{
		sidExist = &iSidExistWr;
		sid = &iSidWr;
		cap = iCapWr;
		defaultType = &iDefaultTypeWr;
	}

	//check that string length match to TSecurity policy size
	if ( aDes.Length() == sizeof( TSecurityPolicy))
	{
		//offsets are coming from TSecurityPolicy class
		TUint8 type = aDes[0];	
		*sidExist = EFalse;	
		*defaultType = EAlwaysPass;
		
		switch ( type )
		{
			case TSecurityPolicy::ETypeFail :
				*defaultType = EAlwaysFail;
			case TSecurityPolicy::ETypePass :
			{
				cap[0] = TUint8(ECapability_None);				
				cap[1] = TUint8(ECapability_None);				
				cap[2] = TUint8(ECapability_None);			
				TInt32* uidRef = (TInt32*) (&cap[4]);
				*uidRef = TInt32(ECapability_None);
			}
			break;
			case TSecurityPolicy::ETypeS3 :
			{
				TInt32* uidRef = (TInt32*) (aDes.Ptr() + 4);
		 		sid->iUid = *uidRef;
				*sidExist = ETrue;	
			}
			//break is not needed -> caps are read in next case statement
			case TSecurityPolicy::ETypeC3 :
			{
				for ( TInt i(0); i < 3; i++)
				{
					cap[i] = aDes[i+1];
				}
			}
			break;
			case TSecurityPolicy::ETypeC7 :
			{
				for ( TInt i(0); i < KMaxCapCount; i++)
				{
					cap[i] = aDes[i+1];
				}
			}
			break;
			default:
			{
				for ( TInt i(0); i < KMaxCapCount; i++)
				{
					cap[i] = TUint8(ECapability_None);
				}

				*sidExist = EFalse;
			}
			break;
		}
	}
	else
	{
		for ( TInt i(0); i < KMaxCapCount; i++)
		{
			cap[i] = TUint8(ECapability_None);
			*defaultType = EAlwaysPass;
		}
		*sidExist = EFalse;
	}	
}


// -----------------------------------------------------------------------------
// CSecuritySetting::Package()
// 
// -----------------------------------------------------------------------------
//
void CSecuritySetting::Package( TDes8& aPackage, ECapTypes aCapType) const
{
	const TBool *sidExist = &iSidExistRd;
	const TUid *sid = &iSidRd;
	const TUint8 *cap = iCapRd;
	const KDefaultType *defaultType = &iDefaultTypeRd;
	
	if ( aCapType == EWriteCaps )
	{
		sidExist = &iSidExistWr;
		sid = &iSidWr;
		cap = iCapWr;
		defaultType = &iDefaultTypeWr;	
	}

	TUint8 type = TSecurityPolicy::ETypePass;	
	
	if ( *defaultType == EAlwaysFail)
	{
		type = TSecurityPolicy::ETypeFail;	
	}
	 
	
	TUint8 * package = const_cast<TUint8*>( aPackage.Ptr());

	for ( TInt i(0); i < KMaxCapCount; i++)
	{
		package[i+1]= cap[i];
	}

	if ( *sidExist )
	{
		TInt32* uidRef = (TInt32*) (package + 4);
		*uidRef = sid->iUid;
	
		type = TSecurityPolicy::ETypeS3;
	}
	else if ( cap[0] != TUint8(ECapability_None))
	{
		if ( cap[3] != TUint8(ECapability_None))
		{
			type = TSecurityPolicy::ETypeC7;
		}
		else
		{
			type = TSecurityPolicy::ETypeC3;
		}
	}



	package[0] = type;
}


// -----------------------------------------------------------------------------
// CSecuritySetting::SetWr()
// 
// -----------------------------------------------------------------------------
//
void CSecuritySetting::SetWr(const TDesC8& aDes)
{
	Set( aDes, EWriteCaps);
}

// -----------------------------------------------------------------------------
// CSecuritySetting::PackageWr()
// -----------------------------------------------------------------------------
//
void CSecuritySetting::PackageWr( TDes8& aPackage) const
{
	Package( aPackage, EWriteCaps);
}

// -----------------------------------------------------------------------------
// CSecuritySetting::SetRd()
// -----------------------------------------------------------------------------
//
void CSecuritySetting::SetRd(const TDesC8& aDes)
{
	Set( aDes, EReadCaps);
}

// -----------------------------------------------------------------------------
// CSecuritySetting::PackageRd()
// -----------------------------------------------------------------------------
//
void CSecuritySetting::PackageRd( TDes8& aPackage) const
{
	Package( aPackage, EReadCaps);
}



// -----------------------------------------------------------------------------
// CSecuritySetting::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CSecuritySetting::ExternalizeL(RWriteStream& aStream) const
{
	//write read- and write-security policies
	TBuf8< sizeof(TSecurityPolicy)> buf;
	buf.SetLength( sizeof(TSecurityPolicy));
	PackageRd( buf);
	aStream << buf;
	PackageWr( buf);
	aStream << buf;
}

// -----------------------------------------------------------------------------
// CSecuritySetting::InternalizeL()
// -----------------------------------------------------------------------------
//
void CSecuritySetting::InternalizeL(RReadStream& aStream)
{

	//read read-security policies
	HBufC8* readPolicy = HBufC8::NewLC( aStream, 5000) ;
	SetRd( *readPolicy);
	CleanupStack::PopAndDestroy( readPolicy) ;


	//read write-security policies
	HBufC8* writePolicy = HBufC8::NewLC( aStream, 5000) ;
	SetWr( *writePolicy);
	CleanupStack::PopAndDestroy( writePolicy) ;
}




// -----------------------------------------------------------------------------
// CRangeMeta::CRangeMeta()
// -----------------------------------------------------------------------------
//
CRangeMeta::CRangeMeta()
{
}


// -----------------------------------------------------------------------------
// CRangeMeta::CRangeMeta()
// -----------------------------------------------------------------------------
//

CRangeMeta::CRangeMeta( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask, TUint32 aMeta)
	: iStart( aRangeStart), iEnd( aRangeEnd), iMask( aMask), iMeta( aMeta)
{
	if ( iMask )
	{
		iType = EMaskSetting;
	}
	else
	{
		iType = ERangeSetting;
	}
}

// -----------------------------------------------------------------------------
// CRangeMeta::Type()
// -----------------------------------------------------------------------------
//
KSettingType CRangeMeta::Type()
{
	return iType;
}


// -----------------------------------------------------------------------------
// CRangeMeta::Start()
// -----------------------------------------------------------------------------
//
TUint32 CRangeMeta::Start() const
{
	return iStart;
}

// -----------------------------------------------------------------------------
// CRangeMeta::End()
// -----------------------------------------------------------------------------
//
TUint32 CRangeMeta::End() const
{
	return iEnd;	
}

// -----------------------------------------------------------------------------
// CRangeMeta::Mask()
// -----------------------------------------------------------------------------
//
TUint32 CRangeMeta::Mask() const
{
	return iMask;
}



// -----------------------------------------------------------------------------
// CRangeMeta::NewL()
// -----------------------------------------------------------------------------
//
CRangeMeta* CRangeMeta::NewL( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask, TUint32 aMeta)
{
	CRangeMeta * self = new (ELeave) CRangeMeta( aRangeStart, aRangeEnd, aMask, aMeta);
	
	return self;
}


// -----------------------------------------------------------------------------
// CRangeMeta::NewL()
// -----------------------------------------------------------------------------
//
CRangeMeta* CRangeMeta::NewL( TLex& aSecurityString)
{
	CRangeMeta * setting = new (ELeave) CRangeMeta();
	
	if ( KErrNone != setting->ReadRangeSetting( aSecurityString))
	{
		if ( KErrNone != setting->ReadMaskSetting( aSecurityString))
		{
			delete setting;
			setting = 0;
		}
	}
	
	
	return setting;	
}

// -----------------------------------------------------------------------------
// CRangeMeta::ReadSetting()
// -----------------------------------------------------------------------------
//
TInt CRangeMeta::ReadRangeSetting( TLex& aSecurityString)
{
	//Read range meta setting, format: 0x1234 0x1234 0x1234 (start end meta) 
	//mask meta setting, format: 0x1234 mask=0x1234 0x01234 (value mask meta)
	//set mark to start point
	TLexMark16 mark;
	aSecurityString.SkipSpaceAndMark( mark);
	
	//read next token
	TPtrC token = aSecurityString.NextToken();
	
	//shoud be a number
	TLex firstValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iStart, firstValue ) || !firstValue.Eos())
	{
		//and if not, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}

	//skip spaces and get next token		
	aSecurityString.SkipSpace();
	token.Set( aSecurityString.NextToken());
	
	//masksetting??
	TInt index = token.FindC( IniConstants::KMask);
	
	if ( index >= 0 )
	{
		//should be in a following format mask=0x1234
		//locate '='
		index = token.Locate('=');
		
		if ( index >= 0 )
		{
			//if '=' included in first token

			if ( index == ( token.Length() - 1))
			{
				token.Set( aSecurityString.NextToken());
			}
			else
			{
				token.Set( token.Mid( index + 1));
			}
		}
		else
		{
			//next token contains '='-mark
			
			token.Set( aSecurityString.NextToken());
			
			index = token.Locate('=');
			if ( index >= 0 && token.Length() > 1)
			{
				token.Set( token.Mid( index + 1));				
			}
			else
			{
				token.Set( aSecurityString.NextToken());		
			}
		}
	}
	
	//also next token should be number
	TLex secondValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iEnd, secondValue) || !secondValue.Eos())
	{
		//and if not, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//skip spaces and get next token		
	aSecurityString.SkipSpace();
	token.Set( aSecurityString.NextToken());
	
	//also next token should be number
	TLex thirdValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iMeta, thirdValue) || !thirdValue.Eos())
	{
		//and if not, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	return KErrNone;	
}


// -----------------------------------------------------------------------------
// CRangeSetting::ReadMaskSetting()
// -----------------------------------------------------------------------------
//

TInt CRangeMeta::ReadMaskSetting( TLex& aSecurityString)
{
	using namespace IniConstants;
	//Read mask (format = 0x1234 mask=01234 caps....

	//Skip spaces and set start mark
	TLexMark16 mark;
	aSecurityString.SkipSpaceAndMark( mark);
	
	//read first token value
	TPtrC token = aSecurityString.NextToken();
	TLex value( token);
	
	//token should be number
	if ( KErrNone != TIniFileHelper::ReadUNumber( iStart, value) || !value.Eos())
	{
		//abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//next token(s) should contain mask definition	
	aSecurityString.SkipSpace();
	token.Set( aSecurityString.NextToken());
	
	//try found the mask identifier
	TInt index = token.FindF( KMask);
	if ( index == KErrNotFound )
	{
		//and if not found, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//locate '='-mark, it could be part current token or in next token
	index = token.Locate('=');
	
	if ( index == KErrNotFound)
	{
		//get next token and try locate '='-mark
		token.Set( aSecurityString.NextToken());
		index = token.Locate('=');

		if ( index == KErrNotFound )
		{
			//and if not found, abort evaluation and restore lex position
			aSecurityString.UnGetToMark( mark);
			return KErrGeneral;
		}
	}
	else
	{
		token.Set( token.Mid(4));
	}
	
	//if token length is more than 1, it must contain also mask-value
	if ( token.Length() > 1 )
	{	
		//remove '='-mark
		token.Set( token.Mid(1));
	}
	else
	{
		//get next token, it must contain mask-value
		token.Set( aSecurityString.NextToken());
	}

	//read mask value
	TLex maskValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iMask, maskValue) || !maskValue.Eos())
	{
		//if fails, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//skip spaces and get next token		
	aSecurityString.SkipSpace();
	token.Set( aSecurityString.NextToken());
	
	//also next token should be number
	TLex thirdValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iMeta, thirdValue) || !thirdValue.Eos())
	{
		//and if not, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CRangeMeta::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CRangeMeta::ExternalizeL(RWriteStream& aStream) const
{
	aStream << iStart;
	aStream << iEnd;
	aStream << iMask;
	aStream << iMeta;
}

// -----------------------------------------------------------------------------
// CRangeMeta::InternalizeL()
// -----------------------------------------------------------------------------
//
void CRangeMeta::InternalizeL(RReadStream& aStream)
{
	aStream >> iStart;
	aStream >> iEnd;
	aStream >> iMask;
	aStream >> iMeta;	
	
	if ( iMask )
	{
		iType = EMaskSetting;
	}
	else
	{
		iType = ERangeSetting;
	}	

	RDEBUG_4( "CentRepTool: Range metadata %d %d %d ", iStart, iEnd, iMeta);

}

// -----------------------------------------------------------------------------
// CRangeSetting::CRangeSetting()
// -----------------------------------------------------------------------------
//

CRangeSetting::CRangeSetting()
{
}

// -----------------------------------------------------------------------------
// CRangeSetting::CRangeSetting()
// -----------------------------------------------------------------------------
//

CRangeSetting::CRangeSetting( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask)
	: iStart( aRangeStart), iEnd( aRangeEnd), iMask( aMask)
{
	if ( iMask )
	{
		iType = EMaskSetting;
	}
	else
	{
		iType = ERangeSetting;
	}
}



// -----------------------------------------------------------------------------
// CRangeSetting::NewL()
// -----------------------------------------------------------------------------
//
CRangeSetting* CRangeSetting::NewL( TLex& aSecurityString)
{
	CRangeSetting * setting = new (ELeave) CRangeSetting();
	
	if ( KErrNone != setting->ReadRangeSetting( aSecurityString))
	{
		if ( KErrNone != setting->ReadMaskSetting( aSecurityString))
		{
			delete setting;
			setting = 0;
		}
	}
	
	return setting;	
}


// -----------------------------------------------------------------------------
// CRangeSetting::Start()
// -----------------------------------------------------------------------------
//
TUint32 CRangeSetting::Start() const
{
	return iStart;
}

// -----------------------------------------------------------------------------
// CRangeSetting::End()
// -----------------------------------------------------------------------------
//
TUint32 CRangeSetting::End() const
{
	return iEnd;
}


// -----------------------------------------------------------------------------
// CRangeSetting::Mask()
// -----------------------------------------------------------------------------
//
TUint32 CRangeSetting::Mask() const
{
	return iMask;
}


// -----------------------------------------------------------------------------
// CRangeSetting::NewL()
// -----------------------------------------------------------------------------
//
CRangeSetting* CRangeSetting::NewL( TUint32 aRangeStart, TUint32 aRangeEnd, TUint32 aMask)
{
	return  new (ELeave) CRangeSetting( aRangeStart, aRangeEnd, aMask);
}



// -----------------------------------------------------------------------------
// CRangeSetting::NewL()
// -----------------------------------------------------------------------------
//
TInt CRangeSetting::ReadRangeSetting( TLex& aSecurityString)
{
	//Read range setting, format: 0x1234 0x1234 caps
	//set mark to start point
	TLexMark16 mark;
	aSecurityString.SkipSpaceAndMark( mark);
	
	//read next token
	TPtrC token = aSecurityString.NextToken();
	
	//shoud be a number
	TLex firstValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iStart, firstValue ) || !firstValue.Eos())
	{
		//and if not, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}

	//skip spaces and get next token		
	aSecurityString.SkipSpace();
	token.Set( aSecurityString.NextToken());
	
	//also next token should be number
	TLex secondValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iEnd, secondValue) || !secondValue.Eos())
	{
		//and if not, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//Ok, range setting, read caps
	if ( KErrNone != ReadCaps( aSecurityString))
	{
		//if fails, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CRangeSetting::ReadMaskSetting()
// -----------------------------------------------------------------------------
//

TInt CRangeSetting::ReadMaskSetting( TLex& aSecurityString)
{
	using namespace IniConstants;
	//Read mask (format = 0x1234 mask=01234 caps....

	//Skip spaces and set start mark
	TLexMark16 mark;
	aSecurityString.SkipSpaceAndMark( mark);
	
	//read first token value
	TPtrC token = aSecurityString.NextToken();
	TLex value( token);
	
	//token should be number
	if ( KErrNone != TIniFileHelper::ReadUNumber( iStart, value) || !value.Eos())
	{
		//abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//next token(s) should contain mask definition	
	aSecurityString.SkipSpace();
	token.Set( aSecurityString.NextToken());
	
	//try found the mask identifier
	TInt index = token.FindF( KMask);
	if ( index == KErrNotFound )
	{
		//and if not found, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//locate '='-mark, it could be part current token or in next token
	index = token.Locate('=');
	
	if ( index == KErrNotFound)
	{
		//get next token and try locate '='-mark
		token.Set( aSecurityString.NextToken());
		index = token.Locate('=');

		if ( index == KErrNotFound )
		{
			//and if not found, abort evaluation and restore lex position
			aSecurityString.UnGetToMark( mark);
			return KErrGeneral;
		}
	}
	else
	{
		token.Set( token.Mid(4));
	}
	
	//if token length is more than 1, it must contain also mask-value
	if ( token.Length() > 1 )
	{	
		//remove '='-mark
		token.Set( token.Mid(1));
	}
	else
	{
		//get next token, it must contain mask-value
		token.Set( aSecurityString.NextToken());
	}

	//read mask value
	TLex maskValue( token);
	if ( KErrNone != TIniFileHelper::ReadUNumber( iMask, maskValue) || !maskValue.Eos())
	{
		//if fails, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	//Ok, range setting, read caps
	if ( KErrNone != ReadCaps( aSecurityString))
	{
		//if fails, abort evaluation and restore lex position
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}	
	
	return KErrNone;
}


// -----------------------------------------------------------------------------
// CRangeSetting::SettingDefinition()
// -----------------------------------------------------------------------------
//

TDesC& CRangeSetting::SettingDefinition( TDes &aSecurityString)
{
	if ( Type() == EMaskSetting )
	{
		return MaskSettingDefinition( aSecurityString);
	}
	
	return RangeSettingDefinition( aSecurityString);
}


// -----------------------------------------------------------------------------
// CRangeSetting::RangeSettingDefinition()
// -----------------------------------------------------------------------------
//
TDesC& CRangeSetting::RangeSettingDefinition( TDes &aSecurityString)
{
	aSecurityString.Zero();

	//create range string	
	aSecurityString.Append(_L("0x"));
	aSecurityString.AppendNum( iStart, EHex);
	aSecurityString.Append(' ');
	aSecurityString.Append(_L("0x"));
	aSecurityString.AppendNum( iEnd, EHex);
	aSecurityString.Append(' ');
	
	return aSecurityString;	
}


// -----------------------------------------------------------------------------
// CRangeSetting::MaskSettingDefinition()
// -----------------------------------------------------------------------------
//
TDesC& CRangeSetting::MaskSettingDefinition( TDes &aSecurityString)
{
	aSecurityString.Zero();

	//create range string	
	aSecurityString.Append(_L("0x"));
	aSecurityString.AppendNum( iStart, EHex);
	aSecurityString.Append(' ');
	aSecurityString.Append( IniConstants::KMask);
	aSecurityString.Append(_L("=0x"));
	aSecurityString.AppendNum( iMask, EHex);
	aSecurityString.Append(' ');
	
	return aSecurityString;		
}


// -----------------------------------------------------------------------------
// CRangeSetting::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CRangeSetting::ExternalizeL(RWriteStream& aStream) const
{
	//read values
	aStream << iStart;
	aStream << iEnd;
	aStream << iMask;
	
	//read security policies
	CSecuritySetting::ExternalizeL( aStream);

#ifdef _DEBUG	
	TBuf<1000> buf( KNullDesC);		
	const_cast<CRangeSetting*>(this)->SecurityString( buf);


	if ( iType == ERangeSetting)
	{
		RDEBUG_4("CentRepTool: Range %x => %x - %S", iStart, iEnd, &buf);
	}
	else
	{
		RDEBUG_4("CentRepTool: Mask %x (mask=%x) - %S", iStart, iMask, &buf);
	}
#endif		

}


// -----------------------------------------------------------------------------
// CRangeSetting::InternalizeL()
// -----------------------------------------------------------------------------
//
void CRangeSetting::InternalizeL(RReadStream& aStream)
{
	//read values
	aStream >> iStart;
	aStream >> iEnd;
	aStream >> iMask;
	
	if ( iEnd)
	{
		iType = ERangeSetting;
	}
	else
	{
		iType = EMaskSetting;
	}

	
	//read security policies
	CSecuritySetting::InternalizeL( aStream);
	
#ifdef _DEBUG	
	TBuf<1000> buf(KNullDesC);		
	SecurityString( buf);

	if ( iType == ERangeSetting)
	{
		RDEBUG_4("CentRepTool: Range %x => %x - %S", iStart, iEnd, &buf);
	}
	else
	{
		RDEBUG_4("CentRepTool: Mask %x (mask=%x) - %S", iStart, iMask, &buf);
	}
#endif		
}






// -----------------------------------------------------------------------------
// CDefaultSetting::CDefaultSetting()
// -----------------------------------------------------------------------------
//
CDefaultSetting::CDefaultSetting()
{
	
}


// -----------------------------------------------------------------------------
// CDefaultSetting::NewL()
// -----------------------------------------------------------------------------
//

CDefaultSetting* CDefaultSetting::NewL( TLex& aSecurityString) 
{
	CDefaultSetting * setting = new (ELeave) CDefaultSetting();
	
	if ( KErrNone != setting->ReadSetting( aSecurityString))
	{
		delete setting;
		setting = 0;
	}
	
	return setting;	
}


// -----------------------------------------------------------------------------
// CDefaultSetting::ReadSetting()
// -----------------------------------------------------------------------------
//

TInt CDefaultSetting::ReadSetting( TLex& aSecurityString)
{
	//No setting specific part -> read caps
	TLexMark16 mark;
	aSecurityString.Mark( mark);

	if ( KErrNone != ReadCaps( aSecurityString))
	{
		aSecurityString.UnGetToMark( mark);
		return KErrGeneral;
	}
	
	return KErrNone;
}

// -----------------------------------------------------------------------------
// CDefaultSetting::NewL()
// -----------------------------------------------------------------------------
//


TDesC& CDefaultSetting::SettingDefinition( TDes &aSecurityString)
{
	aSecurityString = KNullDesC;
	return aSecurityString;
}

// -----------------------------------------------------------------------------
// CDefaultSetting::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CDefaultSetting::ExternalizeL(RWriteStream& aStream) const
{
	CSecuritySetting::ExternalizeL( aStream);
	
}
		
// -----------------------------------------------------------------------------
// CDefaultSetting::InternalizeL()
// -----------------------------------------------------------------------------
//
void CDefaultSetting::InternalizeL(RReadStream& aStream)
{
	CSecuritySetting::InternalizeL( aStream);
	
#ifdef _DEBUG	
	TBuf<500> buf( KNullDesC);		
	SecurityString( buf);
	RDEBUG_2("CentRepTool: Security policy %S", &buf);
#endif		
}


// -----------------------------------------------------------------------------
// CIndividualSetting::CIndividualSetting()
// -----------------------------------------------------------------------------
//
CIndividualSetting::CIndividualSetting()
	: CSecuritySetting()
{
		
}


// -----------------------------------------------------------------------------
// CIndividualSetting::CIndividualSetting()
// -----------------------------------------------------------------------------
//
CIndividualSetting::CIndividualSetting( const TUint32& aSettingId)
	: CSecuritySetting(), iSettingId( aSettingId)
{
		
}


// -----------------------------------------------------------------------------
// CIndividualSetting::~CIndividualSetting()
// -----------------------------------------------------------------------------
//
CIndividualSetting::~CIndividualSetting()
{
	if ( iSettingType == EString)
	{
		delete iSettingValue.iStringValue;
	}
}


// -----------------------------------------------------------------------------
// CIndividualSetting::NewL()
// -----------------------------------------------------------------------------
//

CIndividualSetting* CIndividualSetting::NewL( TLex& aSecurityString) 
{
	//Create setting
	CIndividualSetting * setting = new (ELeave) CIndividualSetting();
	CleanupStack::PushL( setting);
	
	//read individual setting info
	setting->ReadSettingL( aSecurityString);
	
	CleanupStack::Pop( setting);
	
	//return created object
	return setting;	
}





// -----------------------------------------------------------------------------
// CIndividualSetting::ReadSetting()
// -----------------------------------------------------------------------------
//
void CIndividualSetting::ReadSettingL( TLex& aSecurityString)
{
	//Mark original point
	iData.Set( aSecurityString.Remainder());
	
	//Read tokens,,,
	TLex idLex( aSecurityString.NextToken());		//id
	
	if ( KErrNone != TIniFileHelper::ReadUNumber( iSettingId, idLex))
	{
		User::Leave( KErrCorrupt);
	}
	
	TPtrC type = aSecurityString.NextToken();	//type

	//read settings type	
	if ( type.CompareF( IniConstants::KTypeInt ) == 0)
	{
		iSettingType = EInt;	
	}
	else if ( type.CompareF( IniConstants::KTypeReal ) == 0)
	{
		iSettingType = EReal;	
	}
	else if ( type.CompareF( IniConstants::KTypeString ) == 0 )
	{
		iSettingType = EString;	
	}
	else if ( type.CompareF( IniConstants::KTypeString8 ) == 0 )
	{
		iSettingType = EString8;	
	}
	else if ( type.CompareF( IniConstants::KTypeBinary ) == 0)
	{
		iSettingType = EBinaryType;	
	}
	else
	{
		User::Leave( KErrCorrupt);
	}
	
	
	TInt err = KErrNone;
	aSecurityString.SkipSpace();

	
	//read setting value
	switch ( iSettingType)
	{
		case EInt :
		{
			TLex valueLex( aSecurityString.NextToken());
			TInt32 value;
			err = TIniFileHelper::ReadNumber( value, valueLex);	
			iSettingValue.iIntegerValue = value;
		}
		break;
		case EReal :
		{
			TLex valueLex( aSecurityString.NextToken());
			err = TIniFileHelper::ReadNumber( iSettingValue.iRealValue, valueLex);	
		}
		break;
		case EString:
		{
			err = TIniFileHelper::StringReadL( aSecurityString, iSettingValue.iStringValue);	
		}
		break;
		case EString8:
		{
			err = TIniFileHelper::String8ReadL( aSecurityString, iSettingValue.iStringValue);	
			iSettingType = EString;
		}
		break;
		case EBinaryType:
		{
			err = TIniFileHelper::BinaryReadL( aSecurityString, iSettingValue.iStringValue);	
			iSettingType = EString;
		}
		break;
		default:
			//not possible case....
		break;
	}

	if ( err != KErrNone)
	{
		User::Leave( KErrCorrupt);
	}

	//Meta data
	TLex lexMeta( aSecurityString.NextToken());
	err = TIniFileHelper::ReadUNumber( iMeta, lexMeta);
	// if no metadata found -> will return KErrGeneral
	if( err == KErrGeneral )
		{
		iMeta = 0;
		}
	else
		{
		User::LeaveIfError( err ); // will leave if KErrOverflow, otherwise won't leave
		}
	
	//set iData to point data area of setting
	iData.Set( iData.Left( aSecurityString.Offset()));
	
	//Ok, read caps
	if ( KErrNone != ReadCaps( aSecurityString))
	{
		User::Leave( KErrCorrupt);
	}
}

// -----------------------------------------------------------------------------
// CIndividualSetting::SettingDefinition()
// -----------------------------------------------------------------------------
//

TDesC& CIndividualSetting::SettingDefinition( TDes& /*aSecurityString*/)
{
	return iData;
}



// -----------------------------------------------------------------------------
// CIndividualSetting::ExternalizeL()
// -----------------------------------------------------------------------------
//
void CIndividualSetting::ExternalizeDataL(RWriteStream& aStream) const
{
	//set setting id and meta data value
	aStream << iSettingId;
	aStream << iMeta;	
	
	//setting type
	TUint8 temp = static_cast<TUint8>(iSettingType);
	aStream << temp;	
	
#ifdef _DEBUG	
	//buffer for security string
	TBuf<500> buf( KNullDesC);		
	const_cast<CIndividualSetting*>(this)->SecurityString( buf);
#endif		
	
	//write value
	switch ( iSettingType)
	{
		case EInt:
		{
			TInt32 value = iSettingValue.iIntegerValue;
			aStream << value;
			//RDEBUG_4("CentRepTool: Int meta: %u value: %u - %S", iMeta, value, &buf);
		}			
		break;
		case EReal:
		{
			aStream << iSettingValue.iRealValue;
			//RDEBUG_4("CentRepTool: Real meta: %u value: %e - %S", iMeta, iSettingValue.iRealValue, &buf);
		}
		break;
		case EString :
		{
			aStream << *(iSettingValue.iStringValue);
			//RDEBUG_4("CentRepTool: String meta: %u value: %S  - %S", iMeta, iSettingValue.iStringValue, &buf);
		}
		break ;
		default:
		break;
	}	
}

// -----------------------------------------------------------------------------
// CIndividualSetting::InternalizeL()
// -----------------------------------------------------------------------------
//
void CIndividualSetting::InternalizeDataL(RReadStream& aStream)
{
	//get setting meta data value
	aStream >> iMeta;

	//read setting type
	TUint8 temp;
	aStream >> temp;
	iSettingType = static_cast<TSettingType>(temp);

#ifdef _DEBUG	
	//buffer for security string
	TBuf<500> buf( KNullDesC);		
	SecurityString( buf);
#endif	
	
	//read value
	switch ( iSettingType)
	{
		case EInt:
		{
			TInt32 value;
			aStream >> value ;
			iSettingValue.iIntegerValue =value;
			//RDEBUG_4("CentRepTool: Int: meta: %u value:  %d - %S", iMeta, value, &buf);
		}			
		break;
		case EReal:
		{
			aStream >> iSettingValue.iRealValue;
			//RDEBUG_4("CentRepTool: Real: meta: %u value %e - %S", iMeta, iSettingValue.iRealValue, &buf);
		}
		break;
		case EString :
		{
			iSettingValue.iStringValue = HBufC8::NewL ( aStream, NCentralRepositoryConstants::KMaxBinaryLength);
			//RDEBUG_4("CentRepTool: String: meta: %u value %S  - %S", iMeta, iSettingValue.iStringValue, &buf);
		}
		break ;
		default:
		break;
	}			
	
}

// -----------------------------------------------------------------------------
// CIndividualSetting::ExternalizePlatSecL()
// -----------------------------------------------------------------------------
//
void CIndividualSetting::ExternalizePlatSecL(RWriteStream& aStream) const
{
	TUint32 dummyValue(0);

	aStream << iSettingId;
	aStream << dummyValue;
	aStream << dummyValue;
	
	CSecuritySetting::ExternalizeL( aStream);
}


// -----------------------------------------------------------------------------
// CIndividualSetting::InternalizePlatSecL()
// -----------------------------------------------------------------------------
//
void CIndividualSetting::InternalizePlatSecL(RReadStream& aStream)
{
	TUint32 dummyValue(0);

	aStream >> iSettingId;
	aStream >> dummyValue;
	aStream >> dummyValue;

	CSecuritySetting::InternalizeL( aStream);
}

// -----------------------------------------------------------------------------
// CIndividualSetting::CompareElements()
// -----------------------------------------------------------------------------
//
TInt CIndividualSetting::CompareElements( CIndividualSetting const& aSetting1, CIndividualSetting const& aSetting2)
{
	if ( aSetting1.iSettingId == aSetting2.iSettingId) return 0;
	return ( aSetting1.iSettingId < aSetting2.iSettingId ? -1 : 1 );
}


// -----------------------------------------------------------------------------
// TIniFileHelper::ReadNumber()
// -----------------------------------------------------------------------------
//

TInt TIniFileHelper::ReadNumber( TInt32& aNumber, TLex& aLex)
{
	return aLex.Val( aNumber);	
}


// -----------------------------------------------------------------------------
// TIniFileHelper::ReadUNumber()
// -----------------------------------------------------------------------------
//

TInt TIniFileHelper::ReadUNumber( TUint32& aNumber, TLex& aLex)
{
	TRadix radix = EDecimal;

	if( aLex.Peek()=='0')
	{
		aLex.Inc();
		if( aLex.Peek().GetLowerCase()=='x')
		{
			aLex.Inc();
			radix = EHex;
		}
		else
		{
			aLex.UnGet();
		}
	}

	return aLex.Val( aNumber, radix);	
}


// -----------------------------------------------------------------------------
// TIniFileHelper::ReadNumber()
// -----------------------------------------------------------------------------
//

TInt TIniFileHelper::ReadNumber( TReal& aNumber, TLex& aLex)
{
	return aLex.Val( aNumber);	
}

// -----------------------------------------------------------------------------
// TIniFileHelper::TIniFileHelper()
// -----------------------------------------------------------------------------
//

TIniFileHelper::TIniFileHelper()
{
}


// -----------------------------------------------------------------------------
// TIniFileHelper::ReadFileL()
// -----------------------------------------------------------------------------
//

HBufC* TIniFileHelper::ReadFileL(RFile& aFile)
{
	TInt size;
	
	//seek to start of the file
	TInt seekOffset(0);
	aFile.Seek( ESeekStart, seekOffset);
	
	User::LeaveIfError( aFile.Size(size));
	
	if(size<4)
	{
		User::Leave(KErrCorrupt);
	}

	TInt len = size/2-1;
	
	HBufC16* buf = HBufC16::NewL(len);
	
	TPtr16 ptr16 = buf->Des();
	TPtr8 ptr8( (TUint8*)ptr16.Ptr(), 0, 2);
	
	User::LeaveIfError(aFile.Read( ptr8, 2));


	if( *ptr16.Ptr() != IniConstants::KUcs2Bom)
	{
		User::Leave(KErrCorrupt);	
	}

	ptr8.Set((TUint8*)ptr16.Ptr(), 0, size-2);
	User::LeaveIfError(aFile.Read(ptr8));
	ptr16.SetLength(len);

	return buf;
}		



// -----------------------------------------------------------------------------
// TIniFileHelper::StartWriteL()
// -----------------------------------------------------------------------------
//

TInt TIniFileHelper::StartWrite( RFile& aFile)
{
	iFile = &aFile;
	iSize = 0;

	TBuf<1> buf;
	buf.Append( IniConstants::KUcs2Bom);

	TPtrC8 ptr8Ucs2Bom((TUint8*)buf.Ptr(), buf.Length()*2);

	//In debug builds debug file is created, file is used in testing
#ifdef _DEBUG
	_LIT( KCentRepTestFile, "C:\\centreptestfile.txt");
	
	TInt e = iDebugFs.Connect();
	if( e != KErrNone )
		{
		return e;
		}
	
	TInt err = iDebugFile.Open( iDebugFs, KCentRepTestFile, EFileWrite);
	if ( err == KErrNotFound )
	{
		err = iDebugFile.Create( iDebugFs, KCentRepTestFile, EFileWrite);
	}

	iDebugFile.Write( 0, ptr8Ucs2Bom);
	
#endif //_DEBUG

	return aFile.Write( 0, ptr8Ucs2Bom);
}

// -----------------------------------------------------------------------------
// TIniFileHelper::WriteToFileL()
// -----------------------------------------------------------------------------
//
TInt TIniFileHelper::WriteToFile( const TDesC& aContent)
{

	TPtrC content = aContent;
	iSize += content.Length() * 2;
	
	TPtrC8 ptr8((TUint8*)content.Ptr(), content.Length()*2);

#ifdef _DEBUG
	iDebugFile.Write( ptr8);
#endif //_DEBUG
	
	return iFile->Write( ptr8);
}

// -----------------------------------------------------------------------------
// TIniFileHelper::LineFeed()
// -----------------------------------------------------------------------------
//
TInt TIniFileHelper::LineFeed()
{
	TBuf<2> buf;
	buf.Append( 0x0d);
	buf.Append( 0x0a);

	return WriteToFile(buf);
}

// -----------------------------------------------------------------------------
// TIniFileHelper::FinishWrite()
// -----------------------------------------------------------------------------
//
TInt TIniFileHelper::FinishWrite()
{
	TInt err = iFile->SetSize( iSize + 2);
	
	if ( err == KErrNone )
	{
		err = iFile->Flush();
	}

#ifdef _DEBUG
	iDebugFile.SetSize( iSize + 2);
	iDebugFile.Flush();
	
	iDebugFile.Close();
	iDebugFs.Close();
#endif //_DEBUG

	
	return err;
}

// -----------------------------------------------------------------------------
// TIniFileHelper::NextLine()
// -----------------------------------------------------------------------------

void TIniFileHelper::NextLine( TPtrC& aContent, TPtrC& aNextLine)
{
	if ( aContent.Length())
	{
		//remove extra marks from begin of line
		while ( aContent[0] == 10 || aContent[0] == 13 || aContent[0] == ' ')
		{
			aContent.Set( aContent.Mid( 1));
			
			if ( !aContent.Length())
			{
				break;
			}
		}

		//get a next line and remove comment lines
		do
		{
			TInt index = aContent.Locate( 10);
			if ( index != KErrNotFound)
			{
				aNextLine.Set( aContent.Left( index));
				aContent.Set( aContent.Mid( index + 1));
			}
			else
			{
				aNextLine.Set( aContent);
				aContent.Set( KNullDesC);
				break;
			}
			
			if ( !aNextLine.Length())
			{
				break;
			}
		} while ( aNextLine[0] == '#');
		
		
		//remove extra mark from end of string 
		while ( aNextLine.Length() && aNextLine [aNextLine.Length() - 1] == 13)
		{
			aNextLine.Set( aNextLine.Left( aNextLine.Length() - 1));
		}
	};
}

// -----------------------------------------------------------------------------
// TIniFileHelper::CompareElements()
// -----------------------------------------------------------------------------

TInt TIniFileHelper::CompareElements( CRangeSetting const& aElement1, CRangeSetting const& aElement2)
{
	if ( aElement1.Start() == aElement2.Start()) return 0;
	return ( aElement1.Start() < aElement2.Start() ? -1 : 1 );
}


// -----------------------------------------------------------------------------
// TIniFileHelper::CompareElements()
// -----------------------------------------------------------------------------

TInt TIniFileHelper::CompareElements( CRangeMeta const& aElement1, CRangeMeta const& aElement2)
{
	if ( aElement1.Start() == aElement2.Start()) return 0;
	return ( aElement1.Start() < aElement2.Start() ? -1 : 1 );
}

// -----------------------------------------------------------------------------
// TIniFileHelper::ReadStringL()
// -----------------------------------------------------------------------------

TInt TIniFileHelper::StringReadL( TLex& aLex, HBufC8*& aString)
{
	aLex.Mark();

	//locate ' or "
	TChar c( aLex.Peek()); 
	TChar quote ( 0);
	if(c=='\'' || c=='\"')
	{
		aLex.SkipAndMark(1);
		quote = c;
	}

	TBool complete( EFalse);

	TInt len(0);
	for(len=0;!aLex.Eos();len++)
	{
		c = aLex.Get();


		if( quote ? c==quote : c.IsSpace() || aLex.Eos() )
		{
			complete = ETrue;
			break;
		}

		if(c=='\\')
		{
			aLex.Get();
		}
	}

	if(!complete || len> NCentralRepositoryConstants::KMaxUnicodeStringLength)
	{
		return KErrCorrupt;
	}

	aString = HBufC8::NewL(len*2);
	TPtr8 ptr8 = aString->Des();
	ptr8.SetLength(len*2);
	TPtr16 ptr16((TUint16*)ptr8.Ptr(), len, len);
	
	aLex.UnGetToMark();

	_LIT(KSpecialChars, "abfnrvt0");
	static TUint8 specialChars[] = { '\a', '\b', '\f', '\n', '\r', '\v', '\t', '\0' };
	for(TInt i=0;i<len;i++)				
	{
		c = aLex.Get();

		if(c=='\\')
		{
			c = aLex.Get();
			TInt i = KSpecialChars().Locate(c);
			if(i>=0)
				c = specialChars[i];
		}
		ptr16[i] = (TUint16)c;
	}	

	if(quote)
	{
		aLex.Inc(); 
	}
	
	return KErrNone;
}


// -----------------------------------------------------------------------------
// TIniFileHelper::ReadString8L()
// -----------------------------------------------------------------------------
TInt TIniFileHelper::String8ReadL( TLex& aLex, HBufC8*& aString)
{
	aLex.Mark();

	TChar c( aLex.Peek());
	TChar quote( 0);
	if( c=='\'' || c=='\"')
	{
		aLex.SkipAndMark(1);
		quote = c;
	}

	TBool complete( EFalse);

	TInt len(0);
	for(len=0;!aLex.Eos();len++)
	{
		c = aLex.Get();

		if(quote ? c==quote : c.IsSpace())
		{
			complete = ETrue;
			break;
		}

		if(c=='\\')
		{
			aLex.Get();
		}
	}

	if(!complete || len> NCentralRepositoryConstants::KMaxUnicodeStringLength)
	{
		return KErrCorrupt;
	}

	aString = HBufC8::NewLC(len*2);

	HBufC16* tempBuffer = HBufC16::NewLC(len);
	
	TPtr16 ptr16 = tempBuffer->Des();
	TPtr8 ptr8 = aString->Des();
	ptr8.SetLength(len*2);
	
	aLex.UnGetToMark();

	_LIT(KSpecialChars, "abfnrvt0");
	static TUint8 specialChars[] = { '\a', '\b', '\f', '\n', '\r', '\v', '\t', '\0' };
	for(TInt i=0;i<len;i++)				
	{
		c = aLex.Get();

		if ( c=='\\')
		{
			c = aLex.Get();
			TInt i = KSpecialChars().Locate(c);
			if(i>=0)
				c = specialChars[i];
		}

		ptr16.Append(c);
		
	}
		
	const TInt returnValue = CnvUtfConverter::ConvertFromUnicodeToUtf8(ptr8, ptr16);
	if (returnValue==CnvUtfConverter::EErrorIllFormedInput)
	{
		CleanupStack::PopAndDestroy(tempBuffer);
		CleanupStack::PopAndDestroy(aString);			
        return KErrCorrupt;
	}
    else if(returnValue<0)
    {
        User::Leave(KErrGeneral);
    }
   
	CleanupStack::PopAndDestroy(tempBuffer);
	CleanupStack::Pop(aString);	

	if(quote)
	{
		aLex.Inc();  
	}
	
	return KErrNone;
}

// -----------------------------------------------------------------------------
// TIniFileHelper::ReadBinaryL()
// -----------------------------------------------------------------------------
TInt TIniFileHelper::BinaryReadL( TLex& aLex, HBufC8*& aString)
{
	aLex.Mark();
	aLex.SkipCharacters();
	TInt len( aLex.TokenLength());
	aLex.UnGetToMark();

	if(len==1 && aLex.Peek() == KNullDataIndicator)
	{
		aLex.Get();
		aString = HBufC8::NewL(0);
		TPtr8 ptr8 = aString->Des();
		ptr8.SetLength(0);
		return KErrNone;
	}

	if( len> NCentralRepositoryConstants::KMaxBinaryLength*2 || len%2)
	{
		delete aString;
		return KErrCorrupt;
	}
		
	len /= 2;
	aString = HBufC8::NewL(len);
	TPtr8 ptr8 = aString->Des();
	ptr8.SetLength(len);

	TBuf<2> buf(2);
	for(TInt i=0;i<len;i++)
	{
		buf[0] = (TUint8)aLex.Get();
		buf[1] = (TUint8)aLex.Get();
		TLex lex(buf);
		if(lex.Val(ptr8[i], EHex)!=KErrNone)
		{
			delete aString;	
			return KErrCorrupt;
		}
	}
	return KErrNone;
}
