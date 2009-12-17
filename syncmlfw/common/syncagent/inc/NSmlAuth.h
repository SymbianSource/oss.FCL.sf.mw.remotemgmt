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



#ifndef __NSMLAUTH_H
#define __NSMLAUTH_H

#include <e32base.h>
#include "nsmlerror.h"

class CNSmlAgentBase;
class CNSmlFutureReservation;


class CNSmlAuth : public CBase 
	{
	public:  // New functions
	IMPORT_C static CNSmlAuth* NewL();
	~CNSmlAuth();    //from CBase
	IMPORT_C void SetSyncAgent( CNSmlAgentBase* aAgent ); 
	IMPORT_C void SetCredentialL( const TDesC8& aCredential );
	void SetTypeL( const TDesC8& aType );
	void SetFormatL( const TDesC8& aFormat );
	IMPORT_C void SetAuthRequiredL();
	void SetChallenced();
	void CreateAndSaveNewNonceL();
	TPtrC8 NonceL();
	TBool ChallengeNeeded() const;
	IMPORT_C TBool Challenged() const;
	TNSmlError::TNSmlSyncMLStatusCode StatusCodeL();
	
	private:
	void ConstructL();
	CNSmlAuth();
	CNSmlAuth( const CNSmlAuth& aOther );
	CNSmlAuth& operator=( const CNSmlAuth& aOther );
	TNSmlError::TNSmlSyncMLStatusCode CheckCredentialL();
	
	// Private variables
	private:
	enum TAuthState
		{
		ENotInUse,
		ENotAuthenticated,
		EAuthChallenced,
		EAuthenticated,
		EChallencedForNext
		};
	
	TAuthState iAuthState;
	HBufC8* iCredential;
	HBufC8* iType;
	HBufC8* iFormat;
	HBufC8* iNonce;
	HBufC8* iB64Nonce;
	
	CNSmlAgentBase* iAgent;
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;
	};

#endif // __NSMLAUTH_H
