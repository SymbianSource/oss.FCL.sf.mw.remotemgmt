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
* Description:   header file fot CDevEncProgressDlg class
*
*/



#ifndef __DEVENCPROGRESSDLG_H__
#define __DEVENCPROGRESSDLG_H__

// INCLUDES
#include <stringresourcereader.h>
#include <AknGlobalNote.h>

// Set to progress bar, when download starts
const TInt  KInitialDEProgressFinalValue( 100 );

// FORWARD DECLARATIONS
class CAknGlobalProgressDialog;
// CLASS DECLARATION


/**
 *  Dialog observer interface
 *
 *  @lib    fotaserver
 *  @since  S60 v3.1
 */
class MDEProgressDlgObserver
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
    virtual TBool HandleDEProgressDialogExitL( TInt aButtonId ) = 0;    
    };


/**
 *  Progress dialog for download.
 *
 *  @lib    fotaserver.exe
 *  @since  S60 v3.1
 */
NONSHARABLE_CLASS(CDevEncProgressDlg) : public CActive
    {

public:

    IMPORT_C static CDevEncProgressDlg* NewL(MDEProgressDlgObserver* aObserver, TInt aResource);
    IMPORT_C static CDevEncProgressDlg* NewLC(MDEProgressDlgObserver* aObserver, TInt aResource);

    virtual ~CDevEncProgressDlg();


public: // from base classes
    

    void ShowProgressDialogL();
    
    void UpdateProgressDialogL(TInt aValue, TInt aFinalValue);
    
    void ProgressFinished();

private:
    void ConstructL(TInt aResource);
    
    virtual void RunL();
    
    virtual void DoCancel();
    
    CDevEncProgressDlg(MDEProgressDlgObserver* aObserver);

private: // data

   
    CAknGlobalProgressDialog* iProgressDialog;
    
    HBufC* iStringResource;
    
    
    /**
     * Progress dialog observer. Not owned.
     */
    MDEProgressDlgObserver* iObserver;

    };

#endif // __DEVENCPROGRESSDLG_H__

// End of file
