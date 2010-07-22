/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server authentication
*
*/



#include <tconvbase64.h>
#include <e32math.h>
#include "NSmlAuth.h"
#include "nsmlcliagconstants.h"
#include "NSmlAgentBase.h"
#include "nsmlagenttestdefines.h"



// ---------------------------------------------------------
// CNSmlAuth::CNSmlAuth()
// Constructor, nothing special in here.
// ---------------------------------------------------------
//
CNSmlAuth::CNSmlAuth()
	{
	}

// ---------------------------------------------------------
// CNSmlAuth::~CNSmlAuth()
// Destructor
// ---------------------------------------------------------
//
CNSmlAuth::~CNSmlAuth()
	{
	delete iCredential;
	delete iType;
	delete iFormat;
	delete iNonce;
	delete iB64Nonce;
	}

// ---------------------------------------------------------
// CNSmlAuth::NewL()
// Creates new instance of CNSmlAuth. 
// ---------------------------------------------------------
//
EXPORT_C CNSmlAuth* CNSmlAuth::NewL()
	{
	CNSmlAuth* self = new (ELeave) CNSmlAuth();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return( self );
	}
// ---------------------------------------------------------
// CNSmlAuth::SetSyncAgent()
// 
// ---------------------------------------------------------
EXPORT_C void CNSmlAuth::SetSyncAgent( CNSmlAgentBase* aAgent ) 
	{
	iAgent = aAgent;
	}
// ---------------------------------------------------------
// CNSmlAuth::SetCredentialL()
// 
// ---------------------------------------------------------
//
EXPORT_C void CNSmlAuth::SetCredentialL( const TDesC8& aCredential )
	{
	delete iCredential;
	iCredential = NULL;
	iCredential = aCredential.AllocL();
	
	}
// ---------------------------------------------------------
// CNSmlAuth::SetTypeL()
// 
// ---------------------------------------------------------
//	
void CNSmlAuth::SetTypeL( const TDesC8& aType )
	{
	delete iType;
	iType = NULL;
	iType = aType.AllocL();
	}
// ---------------------------------------------------------
// CNSmlAuth::SetFormatL()
// 
// ---------------------------------------------------------
//	
void CNSmlAuth::SetFormatL( const TDesC8& aFormat )
	{
	delete iFormat;
	iFormat = NULL;
	iFormat = aFormat.AllocL();
	}

// ---------------------------------------------------------
// CNSmlAuth::SetAuthRequiredL()
// 
// ---------------------------------------------------------
//	
EXPORT_C void CNSmlAuth::SetAuthRequiredL()
	{
	iAuthState = ENotAuthenticated;
	HBufC* unicodeNonce = iAgent->ServerNonceAllocL();
	CleanupStack::PushL( unicodeNonce );
	iNonce = HBufC8::NewL( unicodeNonce->Length() );
	iNonce->Des().Copy( *unicodeNonce ); 
	CleanupStack::PopAndDestroy(); //unicodeNonce 
	if ( iNonce->Length() == 0 )
		{
		CreateAndSaveNewNonceL();
		}
	}
// ---------------------------------------------------------
// CNSmlAuth::SetChallenced()
// 
// ---------------------------------------------------------
//	
void CNSmlAuth::SetChallenced()
	{
	if ( iAuthState == ENotAuthenticated )
		{
		iAuthState = EAuthChallenced;
		}
	if ( iAuthState == EAuthenticated )
		{
		iAuthState = EChallencedForNext;
		}
	}

// ---------------------------------------------------------
// CNSmlAuth::CreateAndSaveNewNonceL()
// ---------------------------------------------------------
void CNSmlAuth::CreateAndSaveNewNonceL()
	{
	delete iNonce;
	iNonce = NULL;
	iNonce = HBufC8::NewL( 5 );
	_LIT8( KNSmlNonceMask, "%05d" );
	TInt64 seed = User::TickCount();
	TInt nonce = Math::Rand( seed );
	iNonce->Des().Format( KNSmlNonceMask, nonce );
#ifdef __CONSTANT_NONCE
	*iNonce = _L8("nonce");
#endif
	HBufC* unicodeNonce = HBufC::NewLC( iNonce->Length() );
	unicodeNonce->Des().Copy( *iNonce );
	iAgent->SetServerNonceL( *unicodeNonce );
	CleanupStack::PopAndDestroy(); //unicodeNonce 
	}

// ---------------------------------------------------------
// CNSmlAuth::NonceL()
// 
// ---------------------------------------------------------
//
TPtrC8 CNSmlAuth::NonceL()
	{
	if ( iAuthState == EAuthenticated )
		{
		CreateAndSaveNewNonceL();
		}
	TBase64 B64Coder;
	delete iB64Nonce;
	iB64Nonce = NULL;
	iB64Nonce = HBufC8::NewL( iNonce->Length() * 2 + 1 );
	TPtr8 B64NoncePtr( iB64Nonce->Des() );
	User::LeaveIfError( B64Coder.PortableEncode( *iNonce, B64NoncePtr ) );
	return *iB64Nonce;
	}

// ---------------------------------------------------------
// CNSmlAuth::ChallengeNeeded()
// 
// ---------------------------------------------------------
//
TBool CNSmlAuth::ChallengeNeeded() const
	{
	if ( iAuthState == ENotAuthenticated || iAuthState == EAuthenticated )
		{
		return ETrue;
		}
	return EFalse;
	}

// ---------------------------------------------------------
// CNSmlAuth::Challenged()
// 
// ---------------------------------------------------------
//
EXPORT_C TBool CNSmlAuth::Challenged() const
	{
	if ( iAuthState == EAuthChallenced )
		{
		return ETrue;
		}
	return EFalse;
	}

// ---------------------------------------------------------
// CNSmlAuth::StatusCodeL()
// 
// ---------------------------------------------------------
//
TNSmlError::TNSmlSyncMLStatusCode CNSmlAuth::StatusCodeL()
	{
	if ( iAuthState == ENotInUse )
		{
		return TNSmlError::ESmlStatusOK;
		}
	if ( iAuthState == EAuthenticated || iAuthState == EChallencedForNext )
		{
		return TNSmlError::ESmlStatusAuthenticationAccepted;
		}
	TNSmlError::TNSmlSyncMLStatusCode statusCode = TNSmlError::ESmlStatusOK;
	if ( iCredential == NULL || iType == NULL )
		{
		statusCode = TNSmlError::ESmlStatusClientAuthenticationRequired;
		}
	else
		{
		if ( iType->Des() !=  KNSmlAgentAuthMD5 )
			{
			statusCode = TNSmlError::ESmlStatusClientAuthenticationRequired;
			}
		else
			{
            statusCode = CheckCredentialL();
			if ( statusCode == TNSmlError::ESmlStatusAuthenticationAccepted )
				{
				iAuthState = EAuthenticated;
				}
			}
		}	
	if ( iAuthState == EAuthChallenced && statusCode == TNSmlError::ESmlStatusClientAuthenticationRequired )
		{
		statusCode = TNSmlError::ESmlStatusUnauthorized;
		}
	return statusCode;
	}
// ---------------------------------------------------------
// CNSmlAuth::ConstructL()
// ---------------------------------------------------------
//
void CNSmlAuth::ConstructL()
	{
	iAuthState = ENotInUse;
	iType = KNSmlAgentAuthMD5().AllocL();
	iFormat = HBufC8::NewL(0);
	}

// ---------------------------------------------------------
// CNSmlAuth::CheckCredentialL()
// ---------------------------------------------------------
TNSmlError::TNSmlSyncMLStatusCode CNSmlAuth::CheckCredentialL() 
	{
	HBufC8* B64Cred;
	if ( iFormat->Des() != KNSmlAgentBase64Format )
		{
		TBase64 B64Coder;
		B64Cred = HBufC8::NewLC( iCredential->Length() * 2 + 1 );
		TPtr8 B64CredPtr( B64Cred->Des() );
		User::LeaveIfError( B64Coder.PortableEncode( *iCredential, B64CredPtr ) );
		}
	else
		{
		B64Cred = iCredential->AllocLC();
		}
	HBufC8* digest = iAgent->Md5CredentialL( ETrue );
	TNSmlError::TNSmlSyncMLStatusCode statusCode = TNSmlError::ESmlStatusAuthenticationAccepted;
	if ( *digest != *B64Cred )
		{
		statusCode = TNSmlError::ESmlStatusClientAuthenticationRequired;
		}
	CleanupStack::PopAndDestroy(); //B64Cred
	delete digest;
	return statusCode;
	}





