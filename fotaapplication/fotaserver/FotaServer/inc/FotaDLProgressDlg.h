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
* Description:   header file fot CFotaDLProgressDlg class
*
*/



#ifndef CFOTADLPROGRESSDLG_H
#define CFOTADLPROGRESSDLG_H

// INCLUDES
#include <AknProgressDialog.h>

// Set to progress bar, when download starts
const TInt  KInitialProgressFinalValue( 100 );

// Constant to identify the type of progress note displayed at that moment
enum {
	EDownloadingNote=0,	//downloading note is shown
	EConnectingNote		//connecting note is shown
};
// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
 *  Dialog observer interface
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
class MDLProgressDlgObserver
    {
public:
    
    /**
    * Called when the dialog is going to be closed.
    *
    * @since S60 v3.1
    * @param aButtonId    Id of the button, which was used to cancel 
    *                       the dialog.
    * @return TBool Can dlg be exited
    */
    virtual TBool HandleDLProgressDialogExitL( TInt aButtonId ) = 0;    
    };


/**
 *  Progress dialog for download.
 *
 *  @lib    fotaserver.exe
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS(CFotaDLProgressDlg) : public CAknProgressDialog
    {

public:

    /**
     * Constructor.
     *
     * @since S60 v3.1
     * @param aSelfPtr              Pointer to self
     * @param aVisibilityDelayOff   Visibility in delay
     * @param aObserver             Observer
     * @return None
     */
    CFotaDLProgressDlg( CEikDialog** aSelfPtr, TBool aVisibilityDelayOff
                        , MDLProgressDlgObserver* aObserver, TInt aNoteType);

    virtual ~CFotaDLProgressDlg();

    /**
     * Offers key event for progress dialog.
     *
     * @since S60 v3.1
     * @param aKeyEvent             Key event
     * @param aType                 Event type
     * @return None
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

public: // from base classes
    

    /**
     * From CEikDialog, respond to softkey inputs.
     *
     * @since S60 v3.1
     * @para aButtonId, type of pressed Button or Softkey
     * @return TBool, ETrue if exit the dialog, otherwise EFalse.
     */
    TBool OkToExitL( TInt aButtonId );

    /**
     * Type of note shown or displayed
     */
    const TInt iNoteType;
private: // data

    /**
     * Progress dialog observer
     */
    MDLProgressDlgObserver* iObserver;

    };

#endif //

// End of file
