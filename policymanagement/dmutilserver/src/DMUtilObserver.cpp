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


// INCLUDES

#include "DMUtilObserver.h"
#include "DMUtilActiveObserver.h"
#include "PMUtilInternalCRKeys.h"
#include "policymnginternalpskeys.h"
#include "debug.h"

#include <s32file.h>
#include <centralrepository.h>
#include <e32property.h>


// CONSTANTS

_LIT( KPolicyChangeCounterIni, "pcounter.ini");
_LIT( KRFSCounterIni, "rfscounter.ini");

// MACROS
// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

namespace ObserverUtils
{
	TBool CheckValueL( const TDesC& aFile, const TUint32& aCentRepKey)
	{
		TBool retVal = EFalse;
		CRepository* repository = CRepository::NewLC( KCRUidPolicyManagementUtilInternalKeys);
					
		//get counter value from central repository			
		TInt counterPreValue(0);
		TInt counterRealValue(0);
		User::LeaveIfError( repository->Get( aCentRepKey, counterRealValue));			
		CleanupStack::PopAndDestroy( repository);
					
		//create binary presentation for values
		TPckg<TInt> realCounterPack( counterRealValue);			
		TPckg<TInt> preCounterPack( counterPreValue);			
				
		//Open file	
		RFs rfs;
		User::LeaveIfError( rfs.Connect());			
		CleanupClosePushL( rfs);
		
		RFile file;
		CleanupClosePushL( file);
		TBuf<100> fullName;
		User::LeaveIfError( rfs.PrivatePath( fullName));
		fullName.Append( aFile);	
		
		TInt err = file.Open( rfs, fullName, EFileRead|EFileWrite);
		
		if ( err == KErrPathNotFound)
		{
			//create private path if not exists
			User::LeaveIfError( rfs.CreatePrivatePath( EDriveC));
			err = KErrNotFound;
		}
		
		if ( err == KErrNotFound)
		{
			//create file with current counter value
			User::LeaveIfError( file.Create( rfs, fullName, EFileRead|EFileWrite));			
			User::LeaveIfError( file.Write( realCounterPack));
			User::LeaveIfError( file.Flush());
		}
		else
		{
			//read pre-counter value
			User::LeaveIfError( err);
			User::LeaveIfError( file.Read( preCounterPack));
			
			
			if ( counterPreValue != counterRealValue)
			{
				//if counter values has changed, changes return value and update pre-value...
				retVal = ETrue;
				TInt offset(0);
				User::LeaveIfError( file.Seek( ESeekStart, offset));
				User::LeaveIfError( file.Write( realCounterPack));
				User::LeaveIfError( file.Flush());
			}
			 	
		}
	
		//close sessions				
		CleanupStack::PopAndDestroy( 2, &rfs);			
		
		return retVal;
	}
}

// -----------------------------------------------------------------------------
// TDMUtilPassiveObserver::IsPolicyChangedL
// -----------------------------------------------------------------------------
//

EXPORT_C TBool TDMUtilPassiveObserver::IsPolicyChangedL()
{
	return ObserverUtils::CheckValueL( KPolicyChangeCounterIni, KPolicyChangeCounter);		
}

// -----------------------------------------------------------------------------
// TDMUtilStaticObserver::IsRFSPerformedL
// -----------------------------------------------------------------------------
//

EXPORT_C TBool TDMUtilPassiveObserver::IsRFSPerformedL()
{
	return ObserverUtils::CheckValueL( KRFSCounterIni, KRFSCounterKey );		
}


// -----------------------------------------------------------------------------
// CDMUtilActiveObserver::CDMUtilActiveObserver
// -----------------------------------------------------------------------------
//

CDMUtilActiveObserver::CDMUtilActiveObserver()
	: iActiveObserver(0)
{
	
}

// -----------------------------------------------------------------------------
// CDMUtilActiveObserver::~CDMUtilActiveObserver
// -----------------------------------------------------------------------------
//

CDMUtilActiveObserver::~CDMUtilActiveObserver()
{
	delete iActiveObserver;
}

// -----------------------------------------------------------------------------
// CDMUtilActiveObserver::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CDMUtilActiveObserver* CDMUtilActiveObserver::NewL( TPolicyManagementEventTypes aType)
{
	CDMUtilActiveObserver* self = new (ELeave) CDMUtilActiveObserver();
	CleanupStack::PushL( self);
	self->iActiveObserver = CActiveObserver::NewL( aType);
	
	CleanupStack::Pop( self);
	return self;
}
	
// -----------------------------------------------------------------------------
// CDMUtilActiveObserver::SubscribeEventL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDMUtilActiveObserver::SubscribeEventL( MActiveCallBackObserver* aDynamicCallBackObserver)
{
	iActiveObserver->SubscribeEventL( aDynamicCallBackObserver);
}

// -----------------------------------------------------------------------------
// CDMUtilActiveObserver::UnSubscribeEventL
// -----------------------------------------------------------------------------
//
EXPORT_C void CDMUtilActiveObserver::UnSubscribeEvent()
{
	iActiveObserver->UnSubscribeEvent();
}

// -----------------------------------------------------------------------------
// CActiveObserver::CActiveObserver
// -----------------------------------------------------------------------------
//

CActiveObserver::CActiveObserver( TPolicyManagementEventTypes aType) 
	: CActive(0), type( aType), iSubscribed( 0)
{
	
}

// -----------------------------------------------------------------------------
// CActiveObserver::~CActiveObserver
// -----------------------------------------------------------------------------
//

CActiveObserver::~CActiveObserver()
{
	iProperty.Close();
	iSubcribers.Close();
	Deque();
}

// -----------------------------------------------------------------------------
// CActiveObserver::NewL
// -----------------------------------------------------------------------------
//

CActiveObserver* CActiveObserver::NewL( TPolicyManagementEventTypes aType)
{
	CActiveObserver* self = new (ELeave) CActiveObserver( aType);
	return self;
}
	
// -----------------------------------------------------------------------------
// CActiveObserver::SubscribeEventL
// -----------------------------------------------------------------------------
//
void CActiveObserver::SubscribeEventL( MActiveCallBackObserver* aDynamicCallBackObserver)
{
	//add subsribers info to list
	iSubcribers.AppendL( aDynamicCallBackObserver);
	
	if ( !iSubscribed)
	{
		//subscribe event 
		TUint32 key = SubscribeKey( type);
		iProperty.Attach( KPolicyMngProperty, key);
		iProperty.Subscribe( iStatus);
	
		//init....
		CActiveScheduler::Add( this);
		SetActive();
	}
	
	iSubscribed = ETrue;
}


// -----------------------------------------------------------------------------
// CActiveObserver::UnSubscribeEventL
// -----------------------------------------------------------------------------
//
TUint32 CActiveObserver::SubscribeKey( TPolicyManagementEventTypes aType)
{
	TUint32 retVal = 0;

	switch ( aType)
	{
		case EPolicyChangedEvent:
			retVal = KPolicyChangedCounter;
		break;
		default:
		break;
	}
	
	return retVal;
}


// -----------------------------------------------------------------------------
// CActiveObserver::UnSubscribeEventL
// -----------------------------------------------------------------------------
//
void CActiveObserver::UnSubscribeEvent()
{
	DoCancel();
	iSubscribed = EFalse;
}


// -----------------------------------------------------------------------------
// CActiveObserver::UnSubscribeEventL
// -----------------------------------------------------------------------------
//
void CActiveObserver::RunL()
{	
	//notify subscribers....
	for ( TInt i(0); i < iSubcribers.Count(); i++)
	{
		iSubcribers[i]->EventLaunchedL();
	}

	//subscribe again...	
	SetActive();
	iProperty.Subscribe( iStatus);	
}

TInt CActiveObserver::RunError ( TInt /*aError*/ )
    {    
    return KErrNone;
    }	
// -----------------------------------------------------------------------------
// CActiveObserver::DoCancel
// -----------------------------------------------------------------------------
//
void CActiveObserver::DoCancel()
{
	iProperty.Close();
	iSubcribers.Reset();
}










