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
* Description:  Observer for ProvisioningSC to be notified about read events
*
*/



#ifndef MWPWIMOBSERVER_H
#define MWPWIMOBSERVER_H

//  INCLUDES


// FORWARD DECLARATIONS


/**
*  @since 2.5
*/
class MWPWimObserver	 
    {
	public:
	
		virtual void ReadCompletedL() = 0 ;

		virtual void ReadCancelledL() = 0;

		virtual void ReadErrorL() = 0;
 
    };

#endif      // CWPBioMsgHandler_H   
            
// End of File
