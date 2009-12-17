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
* Description:  Query dialog.
*
*/



#ifndef CSYNCMLQUERYDIALOG_H
#define CSYNCMLQUERYDIALOG_H

//  INCLUDES
#include <AknQueryDialog.h>



// CLASS DECLARATION

/**
*  Message query with timeout.
*
*  @lib SyncMLNotifier
*  @since Series 60 3.0
*/
NONSHARABLE_CLASS ( CSyncMLQueryDialog ) : public CAknQueryDialog
                                                 
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSyncMLQueryDialog* NewL(TBool& aKeyPress);        
                  
        
        /**
        * Destructor.
        */
        virtual ~CSyncMLQueryDialog();

    public: // Functions from base classes

        
    protected:  // Functions from base classes
        

        /**
        * From CEikDialog: Determines if the dialog can be closed.
        * @param aButtonId Identifier of the button pressed.
        * @return whether to close the dialog or not.
        */
        TBool OkToExitL( TInt aButtonId );

        /**
        * Called by OfferkeyEventL(), gives a change to dismiss the query even with
        * keys different than Enter of Ok.
        * @param aKeyEvent has the key event details
        * @return whether to dismiss the dialog or not.
        */
        TBool NeedToDismissQueryL(const TKeyEvent& aKeyEvent);
        
        
    private:

        /**
        * C++ default constructor.
        */
        CSyncMLQueryDialog(TBool& aKeyPress  );


    private:    // Data
        
         TBool& iKeyPress;
    };

#endif      // CSYNCMLQUERYDIALOG_H   
            
// End of File
