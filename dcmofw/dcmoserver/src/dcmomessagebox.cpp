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
* Description:  DCMO Message Box 
*
*/


#include "dcmomessagebox.h"
#include "dcmoserver.h"
#include "dcmodebug.h"

TBool CDCMOMessageBox::iMsgBoxClosed = EFalse;

// --------------------------------------------------------------------------
// CDCMOMessageBox* CDCMOMessageBox::NewL( )
// --------------------------------------------------------------------------
//
CDCMOMessageBox* CDCMOMessageBox::NewL( )
	{
		RDEBUG("CDCMOMessageBox::NewL begin");
		CDCMOMessageBox* self = new (ELeave) CDCMOMessageBox( );
  	RDEBUG("CDCMOMessageBox::NewL end");
		return self;		
	}

// --------------------------------------------------------------------------
// CDCMOMessageBox::CDCMOMessageBox( )
// --------------------------------------------------------------------------
//
CDCMOMessageBox::CDCMOMessageBox( ) 
	{
		iMessageBox = NULL;
		RDEBUG("CDCMOMessageBox::constructor");
	}

// --------------------------------------------------------------------------
// CDCMOMessageBox::~CDCMOMessageBox()
// --------------------------------------------------------------------------
//
CDCMOMessageBox::~CDCMOMessageBox()
	{
		RDEBUG("CDCMOMessageBox::~CDCMOMessageBox begin");   
		if( iMessageBox )
		{
			delete iMessageBox ;
    	iMessageBox = NULL; 
		}	
		RDEBUG("CDCMOMessageBox::~CDCMOMessageBox end");    
  }

// --------------------------------------------------------------------------
// TBool CDCMOMessageBox::IsMsgBoxClosed( )
// --------------------------------------------------------------------------
//
TBool CDCMOMessageBox::IsMsgBoxClosed()
	{
		return iMsgBoxClosed;
	}

// --------------------------------------------------------------------------
// void CDCMOMessageBox::SetMsgBoxStatus( TBool aStatus )
// --------------------------------------------------------------------------
//
void CDCMOMessageBox::SetMsgBoxStatus(TBool aStatus)
{
		iMsgBoxClosed = aStatus;
}
	
// --------------------------------------------------------------------------
// void CDCMOMessageBox::ShowNotifierL( TDesC& aString )
// --------------------------------------------------------------------------
//
void CDCMOMessageBox::ShowMessageL( TDesC& aString )
{
		RDEBUG("CDCMOMessageBox::ShowMessageL() : Begin");
		if( !iMessageBox )
			iMessageBox = CHbDeviceMessageBoxSymbian::NewL();			
		iMessageBox->SetTimeout(0); //HbPopup::NoTimeout); 
		iMessageBox->SetDismissPolicy(0); //HbPopup::NoDismiss
		iMessageBox->SetTextL(aString);
		iMessageBox->SetObserver(this);
		iMessageBox->ShowL();		
		RDEBUG("CDCMOMessageBox::ShowMessageL() : End");	
}

void CDCMOMessageBox::MessageBoxClosed(const CHbDeviceMessageBoxSymbian */*aMessageBox*/,
     CHbDeviceMessageBoxSymbian::TButtonId /*aButton*/)
 {
  		 delete	iMessageBox;
  		 iMessageBox = NULL;
    	 iMsgBoxClosed = ETrue;	
    	 if( CDCMOServer::iSessionCount == 0)
    		CActiveScheduler::Stop();
 }
	
//  End of File
