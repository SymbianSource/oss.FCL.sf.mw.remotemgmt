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
*     Provisioning context list container
*
*/


#ifndef CWPCXCONTAINER_H
#define CWPCXCONTAINER_H

// INCLUDES
#include <aknview.h>
#include <bamdesca.h>
   
// FORWARD DECLARATIONS
class CAknSingleStyleListBox;
class CWPEngine;

// CLASS DECLARATION

/**
*  CWPCxDocument  container control class.
*  
*/
class CWPCxContainer : public CCoeControl, 
    private MCoeControlObserver
    {
    public: // Constructors and destructor

        /**
        * C++ constructor
        */
        CWPCxContainer( CWPEngine& aEngine, CAknView& aAppView );
        
        /**
        * Symbian default constructor.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL(const TRect& aRect);

        /**
        * Destructor.
        */
        ~CWPCxContainer();

    public: // from CCoeControl

        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
        void HandleResourceChange( TInt aType );

    public: // new methods

        /**
        * Returns the UID of the currently selected context.
        * @return Context UID
        */
        TUint32 CurrentContext() const;

        /**
        * Returns the UID of the currently selected context.
        * @return Context UID
        */
        TPtrC CurrentContextName() const;

        /**
        * Returns number of contexts in list.
        * @return Number of contexts
        */
        TInt ContextCount() const;

        /**
        * Updates the list of contexts.
        */
        void UpdateContextsL();

    private: // from CCoeControl

        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void GetHelpContext( TCoeHelpContext& aContext ) const;

    private: // from MCoeControlObserver
       
        void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);
        
    private: //data

        /// The list box. Owns.
        CAknSingleStyleListBox* iListBox;

        /// Provisioning engine. Refs.
        CWPEngine& iEngine;

        /// List of context uids. Owns.
        CArrayFix<TUint32>* iCxUids;

        /// List of visible lines. Owns.
        CDesCArray* iLines;

        /// The application view
        CAknView& iAppView;
    };

#endif

// End of File
