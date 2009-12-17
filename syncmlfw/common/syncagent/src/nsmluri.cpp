/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  URI parsing
*
*/


// INCLUDE FILES
#include <nsmlunicodeconverter.h>
#include<nsmlconstantdefs.h>
#include "NSmlURI.h"
#include "nsmlcliagconstants.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlURI::CNSmlURI
// C++ constructor.
// -----------------------------------------------------------------------------
//
CNSmlURI::CNSmlURI()
	{
	iPort = KNSmlDefaultPort;
	}

// -----------------------------------------------------------------------------
// CNSmlURI::NewL
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlURI* CNSmlURI::NewL()
	{
	CNSmlURI* self = CNSmlURI::NewLC();
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::NewL
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlURI* CNSmlURI::NewL( const TDesC& aHostName, TBool aInternet )
	{
	CNSmlURI* self = CNSmlURI::NewLC( aHostName, aInternet );
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::NewL
// Symbian two-phased construtor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlURI* CNSmlURI::NewL( const TDesC8& aHostName, TBool aInternet )
	{
	CNSmlURI* self = CNSmlURI::NewLC( aHostName, aInternet );
	CleanupStack::Pop();
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlURI* CNSmlURI::NewLC()
	{
	CNSmlURI* self = new ( ELeave ) CNSmlURI;
	CleanupStack::PushL( self );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlURI* CNSmlURI::NewLC( const TDesC& aHostName, TBool aInternet )
	{
	CNSmlURI* self = new ( ELeave ) CNSmlURI;
	CleanupStack::PushL( self );
	self->ConstructL( aHostName, aInternet );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::NewLC
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlURI* CNSmlURI::NewLC( const TDesC8& aHostName, TBool aInternet )
	{
	CNSmlURI* self = new ( ELeave ) CNSmlURI;
	CleanupStack::PushL( self );
	self->ConstructL( aHostName, aInternet );
	return self;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::ConstructL
// Symbian second phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlURI::ConstructL( const TDesC& aHostName, TBool aInternet )
	{
	SetHostNameL( aHostName, aInternet );
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::ConstructL
// Symbian second phase constructor.
// -----------------------------------------------------------------------------
//
void CNSmlURI::ConstructL( const TDesC8& aHostName, TBool aInternet )
	{
	SetHostNameL( aHostName, aInternet );
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::~CNSmlURI
// Destructor.
// -----------------------------------------------------------------------------
//
CNSmlURI::~CNSmlURI()
	{
	delete iHostName;
	delete iHostNameWithPort;
	delete iDatabase;
	delete iDatabaseWithoutColon;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::operator=
// Assignment operator. Creates a deep copy of the original object.
// -----------------------------------------------------------------------------
//
EXPORT_C CNSmlURI& CNSmlURI::operator=( const CNSmlURI& aOther )
	{
	if ( aOther.iHostName )
		{
		delete iHostName;
		iHostName = NULL;
		iHostName = aOther.iHostName->AllocL();
		}
		
	if ( aOther.iHostNameWithPort )
		{
		delete iHostNameWithPort;
		iHostNameWithPort = NULL;
		iHostNameWithPort = aOther.iHostNameWithPort->AllocL();
		}
		
	iPort = aOther.iPort;
	
	if ( aOther.iDatabase )
		{
		delete iDatabase;
		iDatabase = NULL; 
		iDatabase = aOther.iDatabase->AllocL();
		}
		
	if ( aOther.iDatabaseWithoutColon )
		{
		delete iDatabaseWithoutColon;
		iDatabaseWithoutColon = NULL;
		iDatabaseWithoutColon = aOther.iDatabaseWithoutColon->AllocL();
		}
		
	iAbsoluteDatabaseURI = aOther.iAbsoluteDatabaseURI;
	
	return *this;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::IsEqualL
// Compares if two URIs are equal.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNSmlURI::IsEqualL( const CNSmlURI& aURI, TBool aIgnoreCase )
	{
	TBool equalURI( EFalse );
	HBufC* thisAbsoluteURI( AbsoluteURILC( this ) );
	HBufC* otherAbsoluteURI( AbsoluteURILC( &aURI ) );
	if( aIgnoreCase )
	{
		if( thisAbsoluteURI->CompareF( otherAbsoluteURI->Des() ) == 0 )
		{
			equalURI = ETrue;
		}
	   
	}
	else
	{
	if ( *thisAbsoluteURI == *otherAbsoluteURI )
		{
		equalURI = ETrue;
		}
		}

	CleanupStack::PopAndDestroy( 2 ); // otherAbsoluteURI, thisAbsoluteURI

	return equalURI;
	}

// -----------------------------------------------------------------------------
// CNSmlURI::SetHostNameL
// Sets the hostname.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlURI::SetHostNameL( const TDesC& aHostName, TBool aInternet )
	{
	delete iHostName;
	iHostName = NULL;
	iHostName = aHostName.AllocL();

	RemoveTrailingSlash( iHostName->Des() );

	if ( aInternet )
		{
		AddHTTPSchemeToHostnameL();
		ExtractPortFromHostnameL();
		}
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::SetHostNameL
// 8-bit variant of SetHostNameL.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlURI::SetHostNameL( const TDesC8& aHostName, TBool aInternet )
	{
	HBufC* hostName;
	NSmlUnicodeConverter::HBufC16InUnicodeLC( aHostName, hostName );
	
	SetHostNameL( *hostName, aInternet );
	
	CleanupStack::PopAndDestroy(); // hostName
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::SetPort
// Sets the port.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlURI::SetPort( TInt aPort )
	{
	iPort = aPort;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::SetDatabaseL
// Sets the database part of URI.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlURI::SetDatabaseL( const TDesC& aDatabase, TBool aRemoveColon )
	{
	delete iDatabase;
	iDatabase = NULL;
	 
  	if ( aDatabase.FindF( KNSmlHttpHeader ) == 0 || aDatabase.FindF( KNSmlHttpsHeader ) == 0 || aDatabase.FindF( KNSmlIMEIHeader ) == 0 )
		{
		iDatabase = HBufC::NewL( aDatabase.Length() );
		
		if ( aDatabase.FindF( *iHostName ) == 0 )
			{
			iAbsoluteDatabaseURI = EFalse;
			*iDatabase = aDatabase.Right( aDatabase.Length() - iHostName->Length() );
			
			if ( iDatabase->Locate( '/' ) == 0 )
				{
				*iDatabase = iDatabase->Right( iDatabase->Length() - 1 );  
				}
			}
		else
			{
			iAbsoluteDatabaseURI = ETrue;
			*iDatabase = aDatabase;
			}
		}
	else
		{
		iAbsoluteDatabaseURI = EFalse;
		
		if ( aDatabase.Length() >= KNSmlAgentRelativeURIPrefix().Length() )
			{
			if ( aDatabase.FindF( KNSmlAgentRelativeURIPrefix ) == 0 )
				{
				iDatabase = aDatabase.AllocL();
				}
			else
				{
				iDatabase = HBufC::NewL( aDatabase.Length() + KNSmlAgentRelativeURIPrefix().Length() );
				iDatabase->Des().Format( KNSmlAgentRelativeURIPrefix );
				iDatabase->Des().Append( aDatabase );
				}
			}
		else if ( aDatabase.Length() == 1 )
			{
			if ( aDatabase[0] == '/' )
				{
				iDatabase = aDatabase.AllocL();
				}
			else
				{
				iDatabase = HBufC::NewL( 1 + KNSmlAgentRelativeURIPrefix().Length() );
				iDatabase->Des().Format( KNSmlAgentRelativeURIPrefix );
				iDatabase->Des().Append( aDatabase );
				}
			}
		else
			{
			iDatabase = HBufC::NewL( 0 );
			}
		}

	delete iDatabaseWithoutColon;
	iDatabaseWithoutColon = NULL;
	
	if ( aRemoveColon )
		{
		if ( iDatabase->Locate( ':' ) == 3 )
			{
			iDatabaseWithoutColon = iDatabase->AllocL();
			iDatabaseWithoutColon->Des().Delete( 3, 1 );
			}
		}
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::SetDatabaseL
// 8-bit variant of SetDatabaseL.
// -----------------------------------------------------------------------------
//
EXPORT_C void CNSmlURI::SetDatabaseL( const TDesC8& aDatabase, TBool aRemoveColon )
	{
	HBufC* database;
	NSmlUnicodeConverter::HBufC16InUnicodeLC( aDatabase, database );
	SetDatabaseL( *database, aRemoveColon ); 
	CleanupStack::PopAndDestroy(); // database
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::HostName
// Returns hostname.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CNSmlURI::HostName()
	{
	TPtrC hostName( *iHostName );
	return hostName;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::HostNameWithPortL
// Returns hostname URI with port number.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CNSmlURI::HostNameWithPortL( TBool aIncludingDefaultPort )
	{
	delete iHostNameWithPort;
	iHostNameWithPort = NULL;
	iHostNameWithPort = HBufC::NewL( iHostName->Length() + 12 + KNSmlHttpHeaderSpace );
	
	if (iPort == KNSmlDefaultPort && !aIncludingDefaultPort )
		{
		*iHostNameWithPort = *iHostName;
		}
	else
		{
		if ( iHostName->FindF( KNSmlHttpHeader() ) != KErrNotFound )  
			{
			iHostNameWithPort->Des() += KNSmlHttpHeader;
			}
		else
			{
			if ( iHostName->FindF( KNSmlHttpsHeader() ) != KErrNotFound )  
				{
				iHostNameWithPort->Des() += KNSmlHttpsHeader;
				}
			}
		iHostNameWithPort->Des() += *IPAllocLC();
		CleanupStack::PopAndDestroy(); 
		iHostNameWithPort->Des() += _L(":");
		TBuf<11> stringPort; 
		stringPort.Num( iPort );
		iHostNameWithPort->Des() += stringPort;
		iHostNameWithPort->Des() += *DocNameAllocLC();
		CleanupStack::PopAndDestroy(); 
		}
		
	TPtrC hostNameWithPort( *iHostNameWithPort );
	return hostNameWithPort;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::HostNameInUTF8AllocLC
// Returns hostname in 8-bit heap buffer.
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CNSmlURI::HostNameInUTF8AllocLC()
	{
	HBufC8* UTF8HostName;
	NSmlUnicodeConverter::HBufC8InUTF8LC( *iHostName, UTF8HostName );
	return UTF8HostName;
	}

// -----------------------------------------------------------------------------
// CNSmlURI::Database
// Returns database.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CNSmlURI::Database()
	{
	TPtrC database( *iDatabase );
	return database;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::DataBaseWithoutRelativePrefix
// Returns database without relative URI prefix.
// -----------------------------------------------------------------------------
//
EXPORT_C TPtrC CNSmlURI::DataBaseWithoutRelativePrefix()
	{
	// database always contains the relative URI prefix
    if ( iDatabase->Length() > KNSmlAgentRelativeURIPrefix().Length() )
    {
    	if ( iDatabase->FindF( KNSmlAgentRelativeURIPrefix ) == 0 )
    	{
		    return (*iDatabase).Mid( KNSmlAgentRelativeURIPrefix().Length() );
    	}
    }
    return (*iDatabase).Mid(0);
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::SyncMLDatabaseAllocLC
// Returns database in a new heap buffer. 
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CNSmlURI::SyncMLDatabaseAllocLC()
	{
	HBufC* shapedURI;
	
	if ( iDatabaseWithoutColon )
		{
		if ( iDatabaseWithoutColon->FindF( KNSmlAgentRelativeURIPrefix ) == 0 )
			{
			shapedURI = iDatabaseWithoutColon->AllocLC();
			}
		else
			{
			shapedURI = HBufC::NewLC( iDatabaseWithoutColon->Length() + KNSmlAgentRelativeURIPrefix.iTypeLength );
			shapedURI->Des() = KNSmlAgentRelativeURIPrefix;
			shapedURI->Des() += *iDatabaseWithoutColon;
			}
		}
	else
		{
		if ( iDatabase )
			{
			if ( iAbsoluteDatabaseURI )
				{
				shapedURI = iDatabase->AllocLC();
				}
			else
				{
				if ( iDatabase->FindF( KNSmlAgentRelativeURIPrefix ) == 0 )
					{
					shapedURI = iDatabase->AllocLC();		
					}
				else
					{
					shapedURI = HBufC::NewLC( iDatabase->Length() + KNSmlAgentRelativeURIPrefix.iTypeLength );
					shapedURI->Des() = KNSmlAgentRelativeURIPrefix;
					shapedURI->Des() += *iDatabase;
					}
				}
			}
		else
			{
			shapedURI = HBufC::NewLC( 0 );
			}
		}	
		
	return shapedURI;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::DatabaseMatchesL
// Compares if two database addresses match.
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CNSmlURI::DatabaseMatchesL( const TDesC& aDatabase )
	{
	HBufC* database( HBufC::NewL( aDatabase.Length() ) );
	
	if( iDatabase->CompareF( aDatabase ) == 0 ) 
	
		{
		return ETrue;
		}

	delete database; 
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlURI::IPAllocLC
// Returns hostname part of the address.
// -----------------------------------------------------------------------------
//
HBufC* CNSmlURI::IPAllocLC()
	{
	HBufC* IP = HBufC::NewLC ( iHostName->Length() + 2 );
	HBufC* docName = HBufC::NewLC ( iHostName->Length() + 2 );
	TPtr ptrIP = IP->Des();
	TPtr ptrDocName = docName->Des();
	ParseURIL( *iHostName, ptrIP, ptrDocName );
	CleanupStack::PopAndDestroy(); // docName
	return IP;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::DocNameAllocLC
// Returns path part of the address.
// -----------------------------------------------------------------------------
//
HBufC* CNSmlURI::DocNameAllocLC()
	{
	HBufC* docName = HBufC::NewLC ( iHostName->Length() + 2 );
	HBufC* IP = HBufC::NewLC ( iHostName->Length() + 2 );
	TPtr ptrDocName = docName->Des();
	TPtr ptrIP = IP->Des();
	ParseURIL( *iHostName, ptrIP, ptrDocName );
	CleanupStack::PopAndDestroy(); // IP
	return docName;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::ParseURILL
// Parses hostname and path components of the address.
// -----------------------------------------------------------------------------
//
void CNSmlURI::ParseURIL( const TDesC& aURI, TDes& aAddress, TDes& aDocName ) const
	{
	const TChar oneSlash( '/' );

	HBufC* URI( aURI.AllocLC() );
	TPtr ptrURI( URI->Des() );
	aAddress.Zero();
	aDocName.Zero();

	TInt httpHeaderPos( ptrURI.FindF( KNSmlHttpHeader() ) );
	TInt httpsHeaderPos( ptrURI.FindF( KNSmlHttpsHeader() ) );
	
	// Skip http:// if found
	if( (httpHeaderPos != KErrNotFound) || (httpsHeaderPos != KErrNotFound) )
		{
		TInt cutLength = (httpHeaderPos == KErrNotFound) ? KNSmlHttpsHeader().Length() : KNSmlHttpHeader().Length();
		ptrURI = ptrURI.Right( ptrURI.Length() - cutLength );
		}

	// Now extract server name and server path
	TInt firstSlash( ptrURI.Locate( oneSlash ) );

	if( firstSlash != KErrNotFound )
		{
		// Slash found -> grab address and rest is servername
		aAddress = ptrURI.Left( firstSlash );
		aDocName = ptrURI.Right( ptrURI.Length() - firstSlash );
		}
	else
		{
		aAddress = ptrURI;
		aDocName.Append( oneSlash );
		}

	aAddress.ZeroTerminate();
	aDocName.ZeroTerminate();

	CleanupStack::PopAndDestroy(); // URI
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::AbsoluteURILC
// Returns absolute URI.
// -----------------------------------------------------------------------------
//
HBufC* CNSmlURI::AbsoluteURILC( const CNSmlURI* aURI ) const
	{
	HBufC* absoluteURI;
	
	if ( aURI->iAbsoluteDatabaseURI )
		{
		absoluteURI = aURI->iDatabase->AllocLC();
		}
	else
		{
		TInt hostNameLength( 0 );
		TInt databaseLength( 0 );
		
		if ( aURI->iHostName )
			{
			hostNameLength = aURI->iHostName->Length();
			}
			
		if ( aURI->iDatabaseWithoutColon )
			{
			databaseLength = aURI->iDatabaseWithoutColon->Length();
			}
		else
			{
			if ( aURI->iDatabase )
				{
				databaseLength = aURI->iDatabase->Length();
				}
			}
			
		absoluteURI = HBufC::NewLC( hostNameLength + KNSmlAgentURIDelimeter.iTypeLength + databaseLength );
		TPtr absoluteURIPtr = absoluteURI->Des();
		
		if ( aURI->iHostName )
			{
			absoluteURIPtr.Format( *aURI->iHostName );
			}
			
		absoluteURIPtr.Append( KNSmlAgentURIDelimeter );
		
		if ( aURI->iDatabaseWithoutColon )
			{
			absoluteURIPtr.Append( *aURI->iDatabaseWithoutColon );
			}
		else
			{
			if ( aURI->iDatabase )
				{
				absoluteURIPtr.Append( RemoveDotSlash( *aURI->iDatabase ) );
				}
			}
		}
		
	return absoluteURI;
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::AddHTTPSchemeToHostnameL
// Adds HTTP prefix to the address if necessary.
// -----------------------------------------------------------------------------
//
void CNSmlURI::AddHTTPSchemeToHostnameL()
	{
	if ( !iHostName )
		{
		return;
		}
		
	if( iHostName->FindF( KNSmlHttpHeader() ) == 0 || 
		iHostName->FindF( KNSmlHttpsHeader() ) == 0 )
		{
		return;
		}
		
	HBufC* tempHostName( HBufC::NewLC( iHostName->Length() + KNSmlHttpHeader.iTypeLength ) );
	tempHostName->Des() = KNSmlHttpHeader;
	tempHostName->Des() += *iHostName;

	delete iHostName;
	iHostName = tempHostName;

	CleanupStack::Pop(); // tempHostName
	}
	
// -----------------------------------------------------------------------------
// CNSmlURI::RemoveTrailingSlash
// Removes a possible trailing slash 
// (e.g. http://myserver.com/sml/ -> http:// myserver.com/sml).
// -----------------------------------------------------------------------------
//
void CNSmlURI::RemoveTrailingSlash( TPtr aString ) const
	{
	if ( aString.Length() > 0 )
		{
		if ( aString[aString.Length() - 1] == '/' )
			{
			aString.Delete( aString.Length() - 1, 1 ); 
			}
		}
	}

// -----------------------------------------------------------------------------
// TPtrC16 CNSmlURI::RemoveDotSlash
// Returns URI without dot and slash (./) in the beginning.
// -----------------------------------------------------------------------------
//
TPtrC16 CNSmlURI::RemoveDotSlash( const TDesC& aURI ) const
	{
	if ( aURI.FindF( KNSmlAgentRelativeURIPrefix ) == 0 )
		{
		return aURI.Right( aURI.Length() - 2 );
		}
	else
		{
		return aURI;
		}
	}

// -----------------------------------------------------------------------------
// CNSmlURI::ExtractPortFromHostname
// Extracts port from hostname (e.g. http://myserver.com:80/syncml).
// -----------------------------------------------------------------------------
//
void CNSmlURI::ExtractPortFromHostnameL()
	{	
	TInt startPos( iHostName->LocateReverseF( ':' ) );
	
	if ( startPos == KErrNotFound )
		{
		return;
		}
		
    TInt endPos( (*iHostName).Mid( startPos ).LocateF( '/' ) );
	
	if ( endPos == KErrNotFound )
		{
		endPos = iHostName->Length() - startPos;
		}
		
	HBufC* tempName( HBufC::NewLC( iHostName->Length() ) );

	tempName->Des().Append( (*iHostName).Left( startPos ) );
	tempName->Des().Append( (*iHostName).Mid( startPos + endPos ) );
	
	TLex lexer( (*iHostName).Mid( startPos + 1, endPos - 1 ) );

	lexer.Val( iPort );
	
	delete iHostName;
	iHostName = tempName;
	
	CleanupStack::Pop(); // tempName
	}

// End of File
