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
* Description:  TWPIMVisitor crawls a logical proxy
*
*/


#ifndef TWPIMVISITOR_H
#define TWPIMVISITOR_H

// INCLUDE FILES
#include <e32base.h>

// CLASS DECLARATION

/**
 * TWPIMVisitor crawls a logical proxy and finds the corresponding 
 * NAPDEF.
 *
 * @lib WPWAPAdapter
 * @since 2.0
 */ 
class TWPIMVisitor : public MWPVisitor
	{
	public:

        /**
        * C++ default constructor.
		*/
		TWPIMVisitor();

	public: // From MWPVisitor
		void VisitL( CWPParameter& /*aElement*/ );
		void VisitL( CWPCharacteristic& aElement );
		void VisitLinkL( CWPCharacteristic& aLink );

	public: // Data

		// The NAPDEF characteristic found
		CWPCharacteristic* iNapDef;
		// The name of the access point
		TPtrC iName;
	};


#endif	// TWPIMVISITOR_H
            
// End of File