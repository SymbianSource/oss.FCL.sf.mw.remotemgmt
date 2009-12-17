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


#ifndef REQUEST_CONTEXT_HEADER_
#define REQUEST_CONTEXT_HEADER_


// INCLUDES
#include <e32base.h>
#include <ssl.h>


class CElementBase;

// CLASS DECLARATION
enum TResponseValue
{
	EResponseDeny = 0,
	EResponsePermit = 1,
	EResponseNotApplicable = 2,
	EResponseIndeterminate = 3
};

class TRequestContext
{
	public:	
		IMPORT_C TRequestContext();
		IMPORT_C ~TRequestContext();
	
		IMPORT_C void AddSubjectAttributeL( const TDesC8& aAttributeId, const TCertInfo& aCertInfo);
		IMPORT_C void AddSubjectAttributeL( const TDesC8& aAttributeId, const TUid& aSecureId);
		IMPORT_C void AddSubjectAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType);
		IMPORT_C void AddActionAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType);
		IMPORT_C void AddResourceAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType);
		IMPORT_C void AddEnvironmentAttributeL( const TDesC8& aAttributeId, const TDesC8& aAttributeValue, const TDesC8& aDataType);
		
		const TDesC8 & RequestDescription();
	private:	
		HBufC8 * iRequestDescription;
		void ReAllocL( TInt aAdditionalLength);
};

class TResponse
{
	public:
		IMPORT_C void SetResponseValue(TResponseValue aValue);
		IMPORT_C TResponseValue GetResponseValue();
	private:
		TResponseValue iValue;
};

#endif