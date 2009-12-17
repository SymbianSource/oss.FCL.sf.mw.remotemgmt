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
* Description:  Base class for WAP setting items.
*
*/


#ifndef TWPFINDACCESSPOINT_H
#define TWPFINDACCESSPOINT_H

// INCLUDE FILES
#include "MWPVisitor.h"
#include <e32base.h>

// CLASS DECLARATION

/**
 * TWPFindAccesspoint crawls a logical proxy and finds the corresponding 
 * access point.
 *
 * @lib ProvisioningEngine
 * @since 2.0
 */ 
class TWPFindAccesspoint : public MWPVisitor
    {
    public:
        /**
        * C++ default constructor.
        */
        TWPFindAccesspoint();

    public: // From MWPVisitor
        void VisitL( CWPParameter& /*aElement*/ );
        void VisitL( CWPCharacteristic& aElement );
        void VisitLinkL( CWPCharacteristic& aLink );

    public: // Data
        /// The access point found
        CWPCharacteristic* iNapDef;
    };


#endif  // TWPFINDACCESSPOINT_H
            
// End of File
