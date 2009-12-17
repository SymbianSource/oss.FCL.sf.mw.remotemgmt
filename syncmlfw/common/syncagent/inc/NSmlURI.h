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
* Description:  URI storage and parsing.
*
*/


#ifndef __NSMLURI_H
#define __NSMLURI_H

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CNSmlFutureReservation;

// CLASS DECLARATION

/**
* Class for database URI handling.
*
* @lib nsmlagentcommon.lib
*/
class CNSmlURI : public CBase 
	{
	public: // constructors and destructor
		/**
		* Symbian two-phased constructor.
		*/
		IMPORT_C static CNSmlURI* NewL();
		
		/**
		* Symbian two-phased constructor.
		* @param aHostName Hostname.
		* @param aInternet If ETrue then HTTP address prefix is added.
		*/
		IMPORT_C static CNSmlURI* NewL( const TDesC& aHostName, TBool aInternet = EFalse ); 
		
		/**
		* Symbian two-phased constructor.
		* @param aHostName Hostname.
		* @param aInternet If ETrue then HTTP address prefix is added.
		*/
		IMPORT_C static CNSmlURI* NewL( const TDesC8& aHostName, TBool aInternet = EFalse );

		/**
		* Symbian two-phased constructor.
		*/
		IMPORT_C static CNSmlURI* NewLC();

		/**
		* Symbian two-phased constructor.
		* @param aHostName Hostname.
		* @param aInternet If ETrue then HTTP address prefix is added.
		*/
		IMPORT_C static CNSmlURI* NewLC( const TDesC& aHostName, TBool aInternet = EFalse );
	
		/**
		* Symbian two-phased constructor.
		* @param aHostName Hostname.
		* @param aInternet If ETrue then HTTP address prefix is added.
		*/
		IMPORT_C static CNSmlURI* NewLC( const TDesC8& aHostName, TBool aInternet = EFalse );
	
		/**
		* Destructor.
		*/
		~CNSmlURI();
		
	public: // new functions
		/**
		* Assignment operator. Makes a deep copy of the given object.
		* @param aOther The object to be copied.
		* @return Target object.
		*/
		IMPORT_C CNSmlURI& operator=( const CNSmlURI& aOther );
		
		/**
		* Compares if two objects are identical.
		* @param aURI The object to be compared against.
		* @return ETrue if the addresses are equal.
		*/
		IMPORT_C TBool IsEqualL( const CNSmlURI& aURI, TBool aIgnoreCase = EFalse );
		
		/**
		* Sets the hostname.
		* @param aHostName New hostname.
		* @param aInternet If ETrue then HTTP address prefix is added.
		*/
		IMPORT_C void SetHostNameL( const TDesC& aHostName, TBool aInternet = EFalse );

		/**
		* Sets the hostname.
		* @param aHostName New hostname.
		* @param aInternet If ETrue then HTTP address prefix is added.
		*/
		IMPORT_C void SetHostNameL( const TDesC8& aHostName, TBool aInternet = EFalse );
		
		/**
		* Sets the port number.
		* @param aPort Port number to be assigned.
		*/
		IMPORT_C void SetPort( TInt aPort );
		
		/**
		* Sets the database part of the address.
		* @param aDatabase Database address.
		* @param aRemoveColon If ETrue then colon is removed from the address 
		* e.g. c:\system\calendar becomes c\system\calendar.
		*/
		IMPORT_C void SetDatabaseL( const TDesC& aDatabase, TBool aRemoveColon = EFalse );

		/**
		* Sets the database part of the address.
		* @param aDatabase Database address.
		* @param aRemoveColon If ETrue then colon is removed from the address 
		* e.g. c:\system\calendar becomes c\system\calendar.
		*/
		IMPORT_C void SetDatabaseL( const TDesC8& aDatabase, TBool aRemoveColon = EFalse );

		/**
		* Returns hostname part of the address.
		* @return Hostname.
		*/
		IMPORT_C TPtrC HostName();
		
		/**
		* Allocates a new 8-bit heap buffer and inserts the hostname to it. The allocated
		* buffer is pushed to the cleanup stack and returned.
		* @return Hostname in 8-bit heap buffer.
		*/
		IMPORT_C HBufC8* HostNameInUTF8AllocLC();
		
		/**
		* Returns a hostname with port number.
		* @param aIncludingDefaultPort If ETrue then even the default port (80) is included.
		* @return Hostname containing the port number.
		*/
		IMPORT_C TPtrC HostNameWithPortL( TBool aIncludingDefaultPort = EFalse );
		
		/**
		* Returns the database part of the address.
		* @return Database part of the address.
		*/
		IMPORT_C TPtrC Database();
		
		/**
		* Allocates a new heap buffer and returns it. The buffer contains database part of the
		* address
		* @return Database part of the address.
		*/
		IMPORT_C HBufC* SyncMLDatabaseAllocLC();

		/**
		* Compares database parts of two addresses.
		* @return ETrue if database part are the same.
		*/
		IMPORT_C TBool DatabaseMatchesL( const TDesC& aDatabase );
		
		/**
		* Returns the database part of the address without relative URI prefix.
		* @return Database part of the address.
		*/
		IMPORT_C TPtrC DataBaseWithoutRelativePrefix();

	private: // constructors
		/**
		* Symbian second phase constructor.
		* @param aHostName Hostname part of the address.
		* @param If ETrue then HTTP address prefix is added.
		*/		
		void ConstructL( const TDesC& aHostName, TBool aInternet = EFalse );

		/**
		* Symbian second phase constructor.
		* @param aHostName Hostname part of the address.
		* @param If ETrue then HTTP address prefix is added.
		*/		
		void ConstructL( const TDesC8& aHostName, TBool aInternet = EFalse );
		
		/**
		* C++ constructor.
		*/
		CNSmlURI();
		
		/**
		* Copy constructor. Makes a deep copy of the original object.
		* @param aOther Original object.
		*/
		CNSmlURI( const CNSmlURI& aOther );
	
	private: // new functions
		/**
		* Extracts hostname part of the address (i.e. possible HTTP prefix and path
		* components are omitted.
		* @return Hostname part of the address.
		*/
		HBufC* IPAllocLC();
		
		/**
		* Extracts path component ofm the address.
		* @return Path component of the address.
		*/
		HBufC* DocNameAllocLC();
		
		/**
		* Parses address and extracts both hostname and path components.
		* @param aURI URI to be parsed.
		* @param aAddress On return contains hostname.
		* @param aDocName On return contains path.
		*/
		void ParseURIL( const TDesC& aURI, TDes& aAddress, TDes& aDocName ) const;
	
		/**
		* Returns a new heap descriptor containing absolute address constructed
		* from the given URI.
		* @param aURI The URI used to address construction.
		* @return The constructed absolute URI.
		*/
		HBufC* AbsoluteURILC( const CNSmlURI* aURI ) const;
	
		/**
		* Adds HTTP addressing prefix (http://) to the hostname part of the address.
		*/
		void AddHTTPSchemeToHostnameL();
	
		/**
		* Removes a possible trailing slash. 
		* (e.g. http://myserver.com/sml/ -> http:// myserver.com/sml).
		* @param aString String from where the slash is removed.
		*/
		void RemoveTrailingSlash( TPtr aString ) const;
	
		/**
		* Returns URI without dot and slash (./) in the beginning.
		* @return URI without leading dot and slash.
		*/
		TPtrC16 RemoveDotSlash(const TDesC& aURI) const;
	
		/**
		* Extracts port from hostname (e.g. http://myserver.com:80/syncml). The iPort
		* member variable is set to the extracted value.
		*/
		void ExtractPortFromHostnameL();
		
	private: // data
		// hostname part of the address
		HBufC* iHostName;
		// hostname part of the address with port number
		HBufC* iHostNameWithPort;
		// port number
		TInt iPort;
		// database part of the address
		HBufC* iDatabase;
		// database part of the address, colon removed
		HBufC* iDatabaseWithoutColon;
		// flag to indicate if the address is absolute
		TBool iAbsoluteDatabaseURI;
		// reserved to maintain binary compability
		CNSmlFutureReservation* iReserved;
	};

#endif // __NSMLURI_H

// End of File
