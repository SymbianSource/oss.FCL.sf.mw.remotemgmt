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
* Description:  Info class for BT connection
*
*/



#ifndef TBTCONNINFO_H
#define TBTCONNINFO_H

//  INCLUDES
#include <e32base.h>
#include <bttypes.h>

/**
 *	Bluetooth connection info
 */
class TBTConnInfo
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        IMPORT_C TBTConnInfo();
		
        /**
        * Destructor.
        */
        IMPORT_C virtual ~TBTConnInfo();

    public: // New functions

    public:     // Data

		// Device address
 		TBTDevAddr iDevAddr;

		// Service class UUID
		TUUID iServiceClass;
    };

#endif      // TBTCONNINFO_H   
            
// End of File
