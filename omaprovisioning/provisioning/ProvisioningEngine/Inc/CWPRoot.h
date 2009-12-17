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
* Description:  Root for WAP Provisioning document.
*
*/


#ifndef CWPROOT_H
#define CWPROOT_H


// INCLUDES
#include <e32base.h>
#include "MWPBuilder.h"
#include "MWPVisitor.h"

// FORWARD DECLARATIONS
class CWPCharacteristic;
class CDesC16Array;

//  CLASS DEFINITION

/**
*  Root for the contents of a WAP Provisioning document.
*
*  @lib ProvisioningEngine
*  @since 2.0
*/
class CWPRoot : public CBase, public MWPBuilder, private MWPVisitor 
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CWPRoot* NewL();
        
        /**
        * Two-phased constructor.
        */
        static CWPRoot* NewLC();
        
        /**
        * Destructor.
        */
        ~CWPRoot();
        
    public: // New functions

        /**
        * Accepts a visitor for the contents of the document.
        * @param aVisitor Visitor is called for each element in the root of the document.
        */
        void AcceptL(MWPVisitor& aVisitor);
        
        /**
        * Loads a WAP Provisioning document from a stream.
        * @param aStream Stream to read the document from.
        */
        void InternalizeL(RReadStream& aStream);
        
        /**
        * Stores a WAP Provisioning document in a stream.
        * @param aStream Stream to write the document to.
        */
        void ExternalizeL(RWriteStream& aStream) const;

        /**
        * Insert an element to the root of the document.
        * @param aElement Element to insert. The ownership is transferred to CWPRoot.
        */
        void InsertL( CWPCharacteristic* aElement );

        /**
        * Creates links from logical proxy ids to logical 
        * proxies and access point ids to access points.
        * Call this before walking through the document data.
        */
        void CreateLinksL();

    public: // From MWPBuilder

        void StartCharacteristicL( TInt aType );
        void StartCharacteristicL( const TDesC& aName );
        void EndCharacteristicL();
        void ParameterL( TInt aID, const TDesC& aValue );
        void ParameterL( const TDesC& aName, const TDesC& aValue );
        
    private: // From MWPVisitor
        
        void VisitL(CWPCharacteristic& aCharacteristic);
        void VisitL(CWPParameter& aParameter);
        void VisitLinkL(CWPCharacteristic& aCharacteristic );

    private:
        
        /**
        * C++ default constructor.
        */
        CWPRoot();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Data
        /// Characteristics in the root of the document. Owns.
        CArrayPtr<CWPCharacteristic>* iContents;
        
        /// Stack for MWPBuilder
        RPointerArray<CWPCharacteristic> iStack;

        /// Needed proxy IDs. Owns.
        CDesC16Array* iNeededIDs;

        /// Proxy IDs. Owns.
        CDesC16Array* iProviderIDs;

        /// Logical proxies. Owns.
        RPointerArray<CWPCharacteristic> iProviders;
        
        /// Characteristics needing proxy.
        RPointerArray<CWPCharacteristic> iNeeders;

        /// Stack
        CWPCharacteristic* iCharStack;
    };

#endif  //  CWPROOT_H
