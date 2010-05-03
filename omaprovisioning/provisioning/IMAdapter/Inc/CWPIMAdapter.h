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
* Description: 
*     Handles wireless village settings in provisioning.
*
*/


#ifndef CWPIMADAPTER_H
#define CWPIMADAPTER_H

// INCLUDES
#include <CWPAdapter.h>
#include <MWPContextExtension.h>
#include <impssapsettings.h>
#include "WPIMUtil.h"

// FORWARD DECLARATIONS
class CWPCharacteristic;
class CWPIMSAPItem;


// CLASS DECLARATION

/**
 * CWPIMAdapter handles wireless village settings.
 *
 * @lib WPIMAdapter
 * @since 2.0
 */ 
class CWPIMAdapter : public CWPAdapter, private MWPContextExtension
	{
	public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
		static CWPIMAdapter* NewL();

        /**
        * Destructor.
        */
		virtual ~CWPIMAdapter();
        
    public : // from CWPAdapter
        
        TInt ItemCount() const;
        const TDesC16& SummaryTitle(TInt aIndex) const;
		const TDesC16& SummaryText(TInt aIndex) const;
        void SaveL( TInt aItem );
        TBool CanSetAsDefault( TInt aItem ) const;
        void SetAsDefaultL( TInt aItem );
        TInt DetailsL( TInt aItem, MWPPairVisitor& aVisitor );
        void VisitL(CWPCharacteristic& aElement);		
		void VisitL(CWPParameter& aElement);		
		void VisitLinkL(CWPCharacteristic& aLink );
        TInt ContextExtension( MWPContextExtension*& aExtension );

    public: // from MWPContextExtension
        const TDesC8& SaveDataL( TInt aIndex ) const;
        void DeleteL( const TDesC8& aSaveData );
		TUint32 Uid() const;

	private:  // New functions

        /**
        * C++ default constructor.
        */
		CWPIMAdapter();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
		void ConstructL();
        
	private:  // Data
        
        // The application id of the current characteristic
        TPtrC iAppID;

        // Current data, owns.
		TData* iCurrentData;

        // The settings items, owns.
		RPointerArray<TData> iDatas;

        // Default name for service access point, owns.
		HBufC* iDefaultName;

        // Title for wireless village settings, owns.
        HBufC* iWVTitle;

        // Id of SAP item to be set as default
        TUint32 iSAPIdForDefault;

    private: // for testing purpose
        friend class T_CWPIMAdapter;
	};

#endif	// CWPIMADAPTER_H
            
// End of File