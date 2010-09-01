/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Diagnostics Detail Test Result Interface
*
*/


#ifndef DIAGRESULTDETAIL_H
#define DIAGRESULTDETAIL_H

// INCLUDES
#include <e32cmn.h>         // RBuf

// FORWARD DECLARATIONS
class CGulIcon;


/**
*  Diagnostics Framework Test Result Details
*
*  This interface allows application to query addtional information about
*  Diagnostics Test Result.  
*
*  @since S60 v5.0
*/
class MDiagResultDetail 
    {
public:
    /** 
    * Possible Output Formats.
    */
    enum TOutputFormat
        {
        EGetMoreInfo,           // Human readable text format.
        ECorrectionUrl,         // URL for correction
        ECorrectionUrlCaption   // Caption for the correction URL.
        };

    /**
    * Get list of supported output
    *
    * @param aOutputList   - Returns supported output formats
    */
    virtual void GetSupportedOutput( RArray<TOutputFormat>& aOutputList ) const = 0;

    /**
    * Check if a given output format is supported.
    *
    * @param aOutputFormat - Possible output formate.
    * @return ETrue if supported. EFalse otherwise.
    */
    virtual TBool IsOutputSupported( TOutputFormat aOutputFormat ) const = 0;

    /**
    * Output current result to a specified format.
    *
    * @param aFormat   Format of desired output.
    * @param aBuffer   Output buffer.
    */
    virtual void GetOutputL( TOutputFormat aFormat, RBuf& aBuffer ) const = 0;

    /**
    * Create an icon. Icon can be created based on the results of the test.
    *
    * @return An icon that matches the result of the test.
    **/
    virtual CGulIcon* CreateIconL() const = 0;

    /**
    * Externalize to CBufFlat. This will be how the data will be stored in the 
    * CDiagResultsDatabaseItem
    *
    * @return CBufFlat containing the externalized data. Ownership is passed 
    *   to the caller.
    **/
    virtual CBufFlat* ExternalizeToBufferL() const = 0;

    /**
    * C++ Virtual destructor. Virtual destructor must be defined to make sure
    * that correct destructor is called. 
    *
    **/
    virtual ~MDiagResultDetail() {};
    };

#endif // DIAGRESULTDETAIL_H

// End of File

