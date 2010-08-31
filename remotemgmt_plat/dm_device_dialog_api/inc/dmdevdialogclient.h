/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of Dm device dialog client
*
*/

#ifndef __DMDEVDIALOG_H__
#define __DMDEVDIALOG_H__

#include <e32std.h>
#include <e32base.h>

class RDmDevDialog : public RSessionBase
	{
	public:
		/**
		 * Launches Dm device dialog server
		 * @param None
		 * @return None
		 */
		IMPORT_C TInt OpenL();
		
		/**
		 * Close server connection
		 * @param None
		 * @return None
		 */
		IMPORT_C void Close();
		
		/**
		 * Asynchronous call to launche package zero device dialog		 
		 * @param aProfileId, ProfileId of the DM server
		 * @param ajobId, Id of the DM job created due to package zero
		 * @param aUimode, Server alert ui mode type
		 * @param aResponse, Response from device dialog
		 * @param aStatus, Status variable set after request completion
		 * @return None
		 */				  
		IMPORT_C void LaunchPkgZero( const TInt& aProfileId, const TInt& ajobId, 
		  const TInt& aUimode, TDes8& aResponse,TRequestStatus& aStatus);  					
	
	
        /**
         * Synchronous call to check whether server alert 
         * connect note shown or not.       
         * @param aConectNoteShown, Status of the server alert connect note         
         * @return None.
         */		
		IMPORT_C TInt IsPkgZeroConnectNoteShown(TInt& aConectNoteShown);
		
        /**
         * Synchronous call to dismiss connect dialog. 
         * Ongoing DM session won't stop.       
         * @param None.         
         * @return None.		
		 */
		IMPORT_C TInt DismissConnectDialog();
		
        /**
         * Synchronous call to Show connect dialog, if connect dialog dismissed 
         * earlier
         * @param None         
         * @return None.        
         */		
		IMPORT_C TInt ShowConnectDialog();
		
        /**
         * Asynchronous call to Show Display(1100) alert dialog          
         * @param aNoteDisplayText, Text to be displayed on the note  
         * @param aStatus, Status variable set after request completion       
         * @return None.        
         */		
		IMPORT_C void ShowDisplayAlert(const TDesC& aNoteDisplayText,TRequestStatus& aStatus);
		
        /**
         * Asynchronous call to Show Confirmation(1101) alert dialog
         * @param aTimeout, Timeout of the dialog. 
         * @param aNoteDisplayText, Text to be displayed on the note           
         * @param aNoteHeader, Text to be displayed as dialog header  
         * @param aStatus, Status variable set after request completion       
         * @return None.        
         */
		IMPORT_C void ShowConfirmationAlert(const TInt& aTimeout,const TDesC& aNoteHeader,
		        const TDesC& aNoteDisplayText,TRequestStatus& aStatus);
		        		
        /**
         * Cancels earlier package zero note, if user doesn't respond
         * @param None.      
         * @return None.        
         */
		IMPORT_C TInt CancelPkgZeroNote();        
};

#endif
