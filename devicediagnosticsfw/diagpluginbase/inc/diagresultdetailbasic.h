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
* Description:  Basic Diagnostics Test Result Detail class
*
*/


#ifndef DIAGRESULTDETAILBASIC_H
#define DIAGRESULTDETAILBASIC_H

// INCLUDES
#include <e32base.h>                    // CBase
#include <DiagResultDetail.h>           // MDiagResultDetail
#include <DiagResultsDatabaseItem.h>    // CDiagResultsDatabaseItem::TResult

// FORWARD DECLARATIONS
class CDiagResultDetailBasicItem;


// CONSTANTS
const TInt KDiagResultDetailBasicVersion  = 1;   // verion of data format.

/**
*  Diagnostics Result Database Detail  class.
*
*  This  class provides  implementation of MDiagResultDetail 
*  with commonly used methods.
*
*  @since S60 v5.0
*/
class CDiagResultDetailBasic : public CBase,
                               public MDiagResultDetail
    {
public:
    /**
    * Constructor.
    *
    * @param aResult - Test Result
    */
    IMPORT_C CDiagResultDetailBasic( 
                    CDiagResultsDatabaseItem::TResult aResult );

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDiagResultDetailBasic();

    /**
    * Externalize to a stream
    *
    * @param aWriteStream - stream to write to.
    */
    IMPORT_C void ExternalizeL( RWriteStream& aWriteStream ) const;

    /**
    * Update result value
    * 
    * @param aResult - Test result
    */
    IMPORT_C void SetResult( CDiagResultsDatabaseItem::TResult aResult );

protected: // Interface for Derived class
    /**
    * Set data to a field.  This function sets a value to a field.
    * If data already exists at the same field id, new data will 
    * over write the old value at the same field.
    *
    * @param aFieldId - Unique ID to identify the data.
    * @param aFieldName - Name of the field. This only for a 
    *   reference.
    * @param aValue - Value to set. This can be a TInt,
    *   const TDesC8& or const TDesC16&.
    */
    IMPORT_C void SetValueL( TInt aFieldId,
                             const TDesC8& aFieldName,
                             TInt aValue );
    IMPORT_C void SetValueL( TInt aFieldId,
                             const TDesC8& aFieldName,
                             const TDesC8& aValue );
    IMPORT_C void SetValueL( TInt aFieldId,
                             const TDesC8& aFieldName,
                             const TDesC16& aValue );

    /**
    * Get value. If invalid aFieldId is passed, or type does not 
    * match, function will leave with KErrArgument. 
    *
    * @param aFieldId - Unique id that identifies the data. Same
    *   value used in SetValueL function.
    * @param aValue - Output paramater. The data will be stored
    *   here.
    * @return KErrNone if successful. If not found KErrNotFound.
    *   If type does not match, KErrArgument.
    */
    IMPORT_C TInt GetValue( TInt aFieldId, TInt& aValue ) const;
    IMPORT_C TInt GetValue( TInt aFieldId, TPtrC8& aValue ) const;
    IMPORT_C TInt GetValue( TInt aFieldId, TPtrC16& aValue ) const;

public: // from MDiagResultDetail
    /**
    * Get list of supported output
    *
    * @param aOutputList   - Returns supported output formats
    */
    IMPORT_C virtual void GetSupportedOutput(
                RArray<TOutputFormat>& aOutputList ) const;

    /**
    * Check if a given output format is supported.
    *
    * @param aOutputFormat - Possible output formate.
    * @return ETrue if supported. EFalse otherwise.
    */
    IMPORT_C virtual TBool IsOutputSupported( TOutputFormat aOutputFormat ) const;

    /**
    * Output current result to a specified format.
    *
    * @param aFormat   Format of desired output.
    * @param aBuffer   Output buffer.
    */
    IMPORT_C virtual void GetOutputL( TOutputFormat aFormat, RBuf& aBuffer ) const;

    /**
    * Create an icon. Icon can be created based on the results of the test.
    *
    * @return An icon that matches the result of the test.
    **/
    IMPORT_C virtual CGulIcon* CreateIconL() const;

    /**
    * Externalize to a CBufFlat. 
    *   @see MDiagResultDetail::ExternalizeToBufferL
    */
    IMPORT_C CBufFlat* ExternalizeToBufferL() const;

protected:
    /**
    * BaseConstructL
    *
    * @param aReadStream - Initialize from a stream.
    */
    IMPORT_C void BaseConstructL( RReadStream& aReadStream );

    /**
    * BaseConstructL
    *
    * @param aBuffer - Buffer to initialize from
    */
    IMPORT_C void BaseConstructL( const CBufFlat& aBuffer );


private:  // internal methods
    /**
    * InternalizeL
    *
    * @param aReadStream - Initialize from a stream.
    */
    void InternalizeL( RReadStream& aReadStream );

    /**
    * Insert a new item. This function will delete the old item if item with 
    * same field id already existsed.
    *
    * @param aNewItem - new item to set
    */
    void SetItemL( CDiagResultDetailBasicItem* aNewItem );

    /**
    * Find item by field id
    *
    * @param - Field id to look up.
    * @return - Index in iFields array if look up is successful.
    *   Return value is negative if look up fails.
    */
    TInt FindItem( TInt aFieldId ) const;


private: // DATA
    /**
    * Test result.
    */
    CDiagResultsDatabaseItem::TResult            iResult;
    
    /**
    * List of fields it contains.
    *   All items are owned by this class.
    */
    RPointerArray<CDiagResultDetailBasicItem>    iFields;
    };



#endif // DIAGRESULTDETAILBASIC_H

// End of File

