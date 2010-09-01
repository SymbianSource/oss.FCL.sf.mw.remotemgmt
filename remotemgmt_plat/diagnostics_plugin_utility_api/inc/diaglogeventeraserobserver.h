/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the header for the Diagnostics Logs Event Eraser
*                 Observer class, which clients implement to receive events
*                 from the Event Eraser.
*
*/


#ifndef DIAGLOGEVENTERASEROBSERVER_H
#define DIAGLOGEVENTERASEROBSERVER_H

// Forward Declarations
class CLogEvent;

/**
* Diagnostics Plugin Log Event Eraser Observer
*   
* This is a pure virtual class that clients of the Event Eraser must implement
* to receive callbacks from the Log Event Eraser.
*/
class MDiagLogEventEraserObserver
    {

public: // New functions

    /**
    * This function is called to check if the log event needs to be deleted.
    *
    * @param aEvent The event to check.
    * @return ETrue if the log event should be deleted, EFalse otherwise.
    */
    virtual TBool IsEventToBeDeleted( const CLogEvent& aEvent ) = 0;
    
    /**
    * This function is called to notify that the erase operation is complete.
    *
    * @param aError Error code to indicate erase failure, or KErrNone.
    */           
    virtual void CompleteEventEraseL( TInt aError ) = 0;

    };

#endif // DIAGLOGEVENTERASEROBSERVER_H
