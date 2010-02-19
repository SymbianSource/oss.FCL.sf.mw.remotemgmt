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
* Description: M class for the Dm notifier interface.
*     
*/


#ifndef DMNATIVENOTIFIERINTERFACE_H_
#define DMNATIVENOTIFIERINTERFACE_H_

#include <e32base.h>    // CBase  // TBuf
#include <ecom/ecom.h>

// The Ecom interface uid
const TUid KNotifierInterfaceUid =  {0x2002BC4D};

//Return types based on the user key press
enum 
    {
    ESyncMLDlgRespKeyNone = -3, 
    ESyncMLDlgRespKeyOk = -2, // all LSK
    ESyncMLDlgRespKeyOthers = -1, //all call-termination key press fall here.
    ESyncMLDlgRespKeyCancel= 0
    };

//Constants for showing the note
enum TDmNoteType
{
    EPrivacyPolicy = 0
};

class MDMNativeNotifierInterface
    {
public:
    virtual TInt ShowDialogL(TDmNoteType aNote)=0;
    };



#endif /* DMNATIVENOTIFIERINTERFACE_H_ */
