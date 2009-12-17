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
* Description: 
*     Declares visitor interface for Provisioning details data.
*
*/


#ifndef MWPPAIRVISITOR_H
#define MWPPAIRVISITOR_H

// FORWARD DECLARATIONS
class TDesC16;

// CLASS DECLARATION

/**
 * MWPNameValueVisitor has to be implemented by details visitors.
 */ 
class MWPPairVisitor
	{
	public:
		/**
		* Called for each detail line found.
		* @since 2.0
		* @param aName The title of the line
		* @param aValue The value of the line
		*/
		virtual void VisitPairL(const TDesC& aName, const TDesC& aValue) = 0;
		
	};

#endif /* MWPPAIRVISITOR_H*/
