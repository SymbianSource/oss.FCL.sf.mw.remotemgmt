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
* Description:  Document structure validator.
*
*/


#ifndef CWPVALIDATOR_H
#define CWPVALIDATOR_H

// INCLUDES
#include <e32base.h>
#include <s32std.h>
#include "MWPVisitor.h"

// FORWARD DECLARATIONS
class MWPElement;
struct TMapping;

// CLASS DECLARATION

/**
*  CWPValidator is document structure validator.
*
*  @lib ProvisioningEngine
*  @since 2.0
 */ 
class CWPValidator : public CBase, public MWPVisitor
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPValidator* NewL();

        /**
        * Two-phased constructor.
        */
        static CWPValidator* NewLC();

        /**
        * Destructor.
        */
        ~CWPValidator();

    public: // From MWPVisitor
        
        void VisitL(CWPCharacteristic& aCharacteristic);
        void VisitL(CWPParameter& aParameter);
        void VisitLinkL(CWPCharacteristic& aCharacteristic );

    private:
        /**
        * Checks if the current element can contain an element
        * of type aTo and retrieves the new validating table.
        * @param aTo The element to check
        */
        void MapToL( MWPElement& aTo );
        TBool Rangecheck(const TInt toType);

    private:
        /**
        * C++ default constructor.
        */
        CWPValidator();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:
        /// Currently possible mappings
        const TMapping* iCurrentMapping;

        /// Currently handled element
        const MWPElement* iCurrentElement;
        TBool iFlag;	
    };

#endif /* CWPENGINE_H */
