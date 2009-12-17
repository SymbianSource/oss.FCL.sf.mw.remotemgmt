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
* Description:  Observer class for BT searchers
*
*/



#ifndef MEXTBTSEARCHEROBSERVER_H
#define MEXTBTSEARCHEROBSERVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class TBTDeviceResponseParams;
class TIrdaSockAddr;

// CLASS DECLARATION

class MExtBTSearcherObserver 
    {

    public:  // Constructors and destructors
        
	virtual ~MExtBTSearcherObserver(){};

    public: // New functions

	/**
	 *	Notify BT device found
	 */
	virtual void BTDeviceFoundL(
		const TBTDeviceResponseParams& aDevResponseParams ) = 0;

	/**
	 *	Notify BT device search error
	 */
	virtual void BTDeviceErrorL( TInt aErr ) = 0;
    };

#endif      // MEXTBTSEARCHEROBSERVER_H   
            
// End of File
