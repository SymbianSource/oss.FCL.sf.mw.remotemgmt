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
* Description:  Basic Diagnostics Test Result Detail Item class
*
*/


#ifndef DIAGRESULTDETAILBASICITEM_H
#define DIAGRESULTDETAILBASICITEM_H

// INCLUDES
#include <e32base.h>                     //  CBase

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;

/**
*  Diagnostics Result Database Detail  Item Classe.
*
*  This  class stores information needed for each entry in
*  CDiagResultDetailBasic.
*
*  @since S60 v5.0
*/
class CDiagResultDetailBasicItem : public CBase
    {
public: // Data Types
    enum TFieldType 
        {
        ETypeInt    = 0,    // data is TInt type
        ETypeDes16,         // data is TDesC16 type
        ETypeDes8,          // data is TDesC8 type
        };

public: // Constructors
    /**
    * Two-phase constructor.
    *
    * @param aFieldId   - Field Id.
    * @param aFieldName - Textual description of the field.
    * @param aValue     - TInt value.
    */
    static CDiagResultDetailBasicItem* NewL( TInt aFieldId,
                                             const TDesC8& aFieldName,
                                             TInt aValue );

    /**
    * Two-phase constructor.
    *
    * @param aFieldId   - Field Id.
    * @param aFieldName - Textual description of the field.
    * @param aValue     - TDesC8 Text value.
    */
    static CDiagResultDetailBasicItem* NewL( TInt aFieldId,
                                             const TDesC8& aFieldName,
                                             const TDesC8& aValue );

    /**
    * Two-phase constructor.
    *
    * @param aFieldId   - Field Id
    * @param aFieldName - Textual description of the field.
    * @param aValue     - TDesC16 Text value.
    */
    static CDiagResultDetailBasicItem* NewL( TInt aFieldId,
                                                const TDesC8& aFieldName,
                                                const TDesC16& aValue );

    /**
    * Two-phase constructor with RReadStream
    *
    * @param aReadStream - stream to internalize from
    */
    static CDiagResultDetailBasicItem* NewL( RReadStream& aReadStream );

    /**
    * Destructor
    */
    ~CDiagResultDetailBasicItem();

    /**
    * Get the field Id.
    *
    * @return Field Id.
    */
    TInt FieldId() const;

    /**
    * Get the name of the field. 
    *
    * @return Name of the field.
    */
    const TDesC8& FieldName() const;

    /**
    * Get the type of the field.
    *
    * @return Type of the field. 
    */
    TFieldType Type() const;

    /**
    * Get value. If invalid aFieldId is passed, or type does not 
    * match, function will leave with KErrArgument. 
    *
    * @param aValue - Output paramater. The data will be stored
    *   here.
    * @param KErrArgument if type does not match
    */
    TInt GetValue( TInt& aValue ) const;
    TInt GetValue( TPtrC8& aValue ) const;
    TInt GetValue( TPtrC16& aValue ) const;

    /**
    * Externalize to a stream.
    *
    */
    void ExternalizeL( RWriteStream& aWriteStream ) const;

private:  
    /**
    * Constructor 
    *
    */
    CDiagResultDetailBasicItem();

    /**
    * Internalize from stream.
    *
    * @param aReadStream - stream to read from.
    */
    void InternalizeL( RReadStream& aReadStream );


private: // Private Data Types
    
    typedef union 
        {
        HBufC16*    iBuffer16;
        HBufC8*     iBuffer8;
        TInt        iInt;
        } TValue;

private: // Data
    TFieldType  iFieldType;
    TInt        iFieldId;
    TValue      iValue;
    HBufC8*     iFieldName;
    };

#endif // DIAGRESULTDETAILBASICITEM_h

// End of File

