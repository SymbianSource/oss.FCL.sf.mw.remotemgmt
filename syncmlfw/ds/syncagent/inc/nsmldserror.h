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
* Description:  Error code conversion
*
*/


#ifndef __NSMLDSERROR_H__
#define __NSMLDSERROR_H__

// INCLUDES
#include <e32base.h>
#include "nsmlerror.h"

// CLASS DECLARATION

/**
* Class for error code conversion.
*
* @lib nsmldsagent.lib
*/
class TNSmlDSError : public TNSmlError
	{
	public: // enumerations 
    	// error coding
		enum TNSmlDSAgentErrorCode
			{
			ESmlDSUnsupportedSyncType = 2000,
			ESmlDSLocalDatabaseError = 2002,
			ESmlDSNoValidDatabases = 2003,
			ESmlDSDeviceInfoMissing = 2004,
			ESmlDSDeviceInfoInvalid = 2005,
			ESmlDSDatastoreSourceRefMissing = 2006, 
			ESmlDSDatastoreSourceRefInvalid = 2007, 
			ESmlDSDatastoreRxPrefCTTypeMissing = 2008,
			ESmlDSDatastoreRxCTTypeNotMatching = 2009,
			ESmlDSDatastoreTxPrefCTTypeMissing = 2010,
			ESmlDSDatastoreTxCTTypeNotMatching = 2011,
	        ESmlDSLocalDatabaseChanged = 2012,
	        ESmlDSServerIdNotMatching = 2013
			};
	
	public: // constructors and destructor
		/**
		* C++ constructor.
		*/
		TNSmlDSError();
	
	public: // functions from base classes  
		/**
		* From TNSmlError. Converts error code to a smaller subset of error codes.
		* @return Converted error code.
		*/
		TInt SyncLogMessageCode();
		
		/**
		* From TNSmlError. Returns the error code.
		* @return Error code.
		*/
		TInt SyncLogErrorCode();
	
	private: // functions from base classes
		/**
		* From TNSmlError. Converts some error codes to Symbian error codes.
		*/
		void ErrorCodeConversion();
	};

#endif // __NSMLERROR_H__

// End of File
