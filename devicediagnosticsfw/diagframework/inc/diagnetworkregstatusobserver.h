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
* Description:  This is the header for the Diagnostics Network registration
*                 status observer interface
*
*/


#ifndef DIAGNETWORKREGSTATUSOBSERVER_H
#define DIAGNETWORKREGSTATUSOBSERVER_H

/**
* Diagnostics Plugin Network registration status observer interface
*   
* This is an abstract class which plugins must derive and implement to receive
* call-backs from the Diagnostics Network registration status observer.
*/
class MDiagNetworkRegStatusObserver
    {
public:
    /**
    * This function is used to inform the intial network registration status
    *
    * @param aRegistered. 
    *  ETrue :The device is currently registered on the network
    *  EFalse:The device is currently not registered on the network.
    */
    virtual void InitialNetworkRegistrationStatusL( const TBool& aRegistered ) = 0;
    
    /**
    * This interface is used to inform the clients of any changes in the network registration
    * status
    *
    * @param aRegistered
    * ETrue :The device has now changed to registered status.
    * EFalse:The device has now lost network coverage.
    */    
    virtual void NetworkRegistrationStatusChangeL( const TBool& aRegistered ) = 0;
    };

#endif // DIAGNETWORKREGSTATUSOBSERVER_H

//End of file

