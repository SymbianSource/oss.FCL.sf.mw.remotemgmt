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
* Description:  Interface for plugin alert handlers
 *
*/



#ifndef __NSMLSYNCALERTHANDLERIF_H_
#define __NSMLSYNCALERTHANDLERIF_H_

#include <e32base.h>
#include <badesca.h>
#include <bttypes.h>

#include "nsmlsyncalerthandlerif.h"
#include "mnsmlalertobexplugincontext.h"


// CLASS DECLARATION

enum TNSmlAlertResult
		{
		ENSmlErrNone,
		ENSmlErrUndefined,
		ENSmlErrNotConsumed
		};

/**
 * @brief Sync alert handler
 *
 * 
 *
 */
class CNSmlSyncAlertHandler : public CBase
    {
    public:   // Constructors and destructor
    	/**
	* Creates new instance of CNSmlSyncAlertHandler based class.
	* @param aImplementationUid UID of the implementation to be created.
	* @return CNSmlSyncAlertHandler* Newly created instance.
	*/
	inline static CNSmlSyncAlertHandler* NewL( const TUid& aImplementationUid );


   /*
    * @brief Start sync via Alert
    *
    * This virtual function should be implemented by all concrete
    * ECOM implementations. It's typically passed almost directly
    * to a polymorphic DLL that implements the actual
    * alerting. The concrete alert handler can use the resources
    * of Obex Plugin via aContext pointer if desirable.
    *
    * @param aSyncMLMessage    The wbxml message that was received
    * @param aTransport    What transport will we start the synch with?
    * @param aBTAddress	Bluetooth address of peer, if any
    * @param aBTName	 Bluetooth name of peer, if any
    * @param aContext   ObexPlugin resource context
    *                  
    * @return The result status of synch
    */

    virtual TNSmlAlertResult StartSyncL(
        const TDesC8& aSyncMLMessage,
        TInt aTransport,
        TBTDevAddr aBTAddress,
        const TDesC& aBTName, 
        MNSmlAlertObexPluginContext* aContext) = 0;
              
        
    inline virtual ~CNSmlSyncAlertHandler();

    private:
        // ECOM framework requires this ID in object destructor
        TUid iDtor_ID_Key;
    };

#include "nsmlsyncalerthandlerif.inl"

#endif      //

// End of File
