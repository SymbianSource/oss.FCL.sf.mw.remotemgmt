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
* Description:  Pure virtual interface
*
*/


#ifndef __NSMLDMAGENTOBSERVER_H
#define __NSMLDMAGENTOBSERVER_H

#include <e32std.h>


// DS Agent observer interface
class MNSmlDMAgentObserver
	{
	// Public members
	public:
	enum TNSmlDMAgentPhase 
		{
		EConnecting,
		EProcessing,
		ESending,
		EDisconnecting
		};
	virtual TBool NotifyPhaseBeginL( TNSmlDMAgentPhase aPhase ) = 0;
	virtual TBool NotifyPhaseEndL() = 0;
//	virtual void NotifyDisplayAlertL( const TDesC& aData, TInt aMindt, TInt aMaxdt ) = 0;
//	virtual TBool NotifyConfirmationAlertL( const TDesC& aData, TInt aMindt, TInt aMaxdt ) = 0;
	};

#endif // __NSMLDMAGENTOBSERVER_H
