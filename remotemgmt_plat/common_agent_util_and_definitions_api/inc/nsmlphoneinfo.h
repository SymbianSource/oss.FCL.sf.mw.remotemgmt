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
* Description:  Phone specific data
*
*/



#ifndef __NSMLPHONEINFO_H
#define __NSMLPHONEINFO_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CNSmlFutureReservation;

// CLASS DECLARATION
class CNSmlPhoneInfo : public CBase 
	{
	public: // enumerations
	enum TPhoneIdType
		{
		EPhoneManufacturer = 0,
		EPhoneModelId,
		EPhoneRevisionId,
		EPhoneSerialNumber
		};
	
	public:  // Constructors and destructor
	IMPORT_C static CNSmlPhoneInfo* NewL(); 
	IMPORT_C static CNSmlPhoneInfo* NewLC(); 
	~CNSmlPhoneInfo();    //from CBase
	
	public:  // new functions
	IMPORT_C void PhoneDataL( TPhoneIdType aIdType, TDes& aId ) const;
	IMPORT_C TPtrC8 SwVersionL();

	private:
	void ConstructL();
	CNSmlPhoneInfo();
	CNSmlPhoneInfo( const CNSmlPhoneInfo& aOther );
	CNSmlPhoneInfo& operator=( const CNSmlPhoneInfo& aOther );
	TBool IsFlagSet( const TUint& aValue, const TUint& aBit ) const;
	
	private: //Data
	HBufC8* iSwVersion;
	// Reserved to maintain binary compability
	CNSmlFutureReservation* iReserved;

	};

#endif // __NSMLPHONEINFO_H
