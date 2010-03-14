/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Defines the class which will load the DMNotifier plugin and would service the caller.
*     
*/

#ifndef DMNATIVENOTIFIER_H
#define DMNATIVENOTIFIER_H


#include <DMNativeNotifierInterface.h>

enum{
    EDMDialogNotAccepted = 0,
    EDMDialogAccepted = 1,
    EDMDialogActive = 2
};
class CDMNativeNotifier : public CBase, public MDMNativeNotifierInterface
{
 public:
    
    /**
    * Two-phased constructor.
    */
    static CDMNativeNotifier* NewL(TUid aOperationId);
	
    /**
    * Destructor.
    */
	virtual ~CDMNativeNotifier();

 public:
	/**
	* Function to list all the implementations
	*/
	static void ListImplementationsL(RImplInfoPtrArray& aImplInfoArray);

 private:
	// Instance identifier key
	TUid iDtor_ID_Key;
};

#include "DMNativeNotifier.inl"

#endif  // DMNATIVENOTIFIER_H

//End of file
