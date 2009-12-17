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
* Description:  Conversion for private error codes to public syncML errors.
*
*/



#ifndef TNSMLERRORCONVERSION_H
#define TNSMLERRORCONVERSION_H

//  INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  Converts private error code to public SyncML error code.
*
*  @lib SyncMLClientAPI.lib
*/
class TNSmlErrorConversion
    {
    public:
    struct TNSmlErrorMapping
		{
		TInt iErrorCode;
		TInt iSyncMLError;
		};
	
	struct TNSmlErrorMappingRange
		{
		TInt iErrorCodeRangeStart;
		TInt iErrorCodeRangeEnd;
		TInt iSyncMLError;
		};
	
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TNSmlErrorConversion( TInt aErrorCode );

    public: // New functions
    
    	void SetErrorCode( TInt aErrorCode );
    
    	IMPORT_C TInt Convert() const;

    private:	// New functions

        // Prohibit copy constructor
        TNSmlErrorConversion( const TNSmlErrorConversion& );
        // Prohibit assigment operator.
        TNSmlErrorConversion& operator=( const TNSmlErrorConversion& );
    
    protected:  // Data
        TInt iErrorCode;
        static const TNSmlErrorMapping KErrors[];
        static const TNSmlErrorMappingRange KErrorRanges[];
    };

#endif      // TNSMLERRORCONVERSION_H
            
// End of File
