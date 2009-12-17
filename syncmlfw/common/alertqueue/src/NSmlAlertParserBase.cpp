/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Alert queue 
*
*/


#include "NSmlAlertQueue.h"

// ---------------------------------------------------------
// CNSmlMessageParserBase(CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
// Constructor
// ---------------------------------------------------------
CNSmlMessageParserBase::CNSmlMessageParserBase(CSmlAlertInfo& aAlertInfo, CSyncMLHistoryPushMsg& aHistoryInfo )
: iAlertInfo(aAlertInfo), iHistoryInfo(aHistoryInfo), iMessagePtr( 0, NULL, 0) 
	{ 
	
	}

// ---------------------------------------------------------
// CNSmlMessageParserBase::~CNSmlMessageParserBase()
// Destructor
// ---------------------------------------------------------
CNSmlMessageParserBase::~CNSmlMessageParserBase()
	{
	delete iMessage;
	iFoundProfiles.ResetAndDestroy();
	iFoundProfiles.Close();
	}
// ---------------------------------------------------------
// CNSmlMessageParserBase::CreateBufferL( TInt aSize )
// Creates buffer of specified size
// ---------------------------------------------------------
void CNSmlMessageParserBase::CreateBufferL( TInt aSize )
	{
	delete iMessage;
	iMessage = NULL;
	iMessage = HBufC8::NewL( aSize );
	}

// ---------------------------------------------------------
// CNSmlMessageParserBase::Message() 
// Returns pointer to the buffer
// ---------------------------------------------------------
TPtr8& CNSmlMessageParserBase::Message() 
	{ 
	iMessagePtr.Set( iMessage->Des() ); 
	return iMessagePtr;
	}

// ---------------------------------------------------------
// CNSmlMessageParserBase::CheckLengthL( TInt aPos )
// Returns pointer to the buffer
// ---------------------------------------------------------
void CNSmlMessageParserBase::CheckLengthL( TInt aPos )
	{
	if ( Message().Length() < aPos )
		{
		User::Leave( KErrCorrupt );
		}
	}

// ---------------------------------------------------------
// CNSmlMessageParserBase::DoMessageCopyLC()
// Returns pointer to the buffer. Buffer is leaved on the cleanup stack.
// ---------------------------------------------------------
TPtrC8 CNSmlMessageParserBase::DoMessageCopyLC()
	{
	HBufC8* message = HBufC8::NewLC( Message().Size() );
	message->Des().Copy( Message() );
	return message->Des();
	}
	
//End of File
