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
* Description:  Obex searcher factory
*
*/



#ifndef COBEXSEARCHERFACTORY_H
#define COBEXSEARCHERFACTORY_H

//  INCLUDES
#include <e32base.h>
#include <bttypes.h>
#include "nsmltransport.h"
#include "Btconninfo.h"


// FORWARD DECLARATIONS
class CObexSearcherBase;

// CLASS DECLARATION

class CObexSearcherFactory : public CBase
    {

    public: // New functions
        
		/**
		 *	Creates an obex searcher.
		 */
        static CObexSearcherBase* CreateObexSearcherL(
			TNSmlObexTransport aTransport );

		/**
		 *	Creates a Bluetooth searcher
		 */
        static CObexSearcherBase* CreateBTSearcherL(
			const TBTConnInfo& aBTConnInfo );

    private:

		/**
        * C++ default constructor.
        */
        CObexSearcherFactory();

    };

#endif      // COBEXSEARCHERFACTORY_H   
            
// End of File
