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
* Description:  ObexSearcherObserver
*
*/


#ifndef MObexSearcherObserver_H
#define MObexSearcherObserver_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TBTDeviceResponseParams;
class TIrdaSockAddr;

// CLASS DECLARATION

class MObexSearcherObserver
    {
    public: // New functions
        
		/**
		 *	Notify device found
		 */
        virtual void HandleDeviceFoundL() = 0;

		/**
		 *	Notify device search error
		 */
        virtual void HandleDeviceErrorL( TInt aErr ) = 0;

		/**
		 *	Notify service found
		 */
        virtual void HandleServiceFoundL() = 0;

		/**
		 *	Notify service search error
		 */
        virtual void HandleServiceErrorL( TInt aErr ) = 0;

        ~MObexSearcherObserver() {};

    protected:

        MObexSearcherObserver() {};

    };

#endif      // MObexSearcherObserver_H   
            
// End of File
