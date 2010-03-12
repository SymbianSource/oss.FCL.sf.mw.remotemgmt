/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface file for DM Native Notifier.
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
