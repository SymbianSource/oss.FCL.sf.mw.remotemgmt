/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Ecom plug-in for x-wap-application:push.syncml application ID          
*
*/


#ifndef __NSMLDSWAPPUSHALERT_H__
#define __NSMLDSWAPPUSHALERT_H__

// INCLUDES
#include <e32base.h>
#include <cpushhandlerbase.h>

// FORWARD DECLARATIONS
//
class CContentHandlerBase;

// CONSTANTS
//
const TUint KNSmlDSWapPushAlertImplUid = 0x101F7C59;


// CLASS DECLARATION
class CNSmlDSWapPushAlert : public CPushHandlerBase
	{
public:	// constructor and destructor

	static CNSmlDSWapPushAlert* NewL();
	virtual ~CNSmlDSWapPushAlert();

private:  // New functions

	CNSmlDSWapPushAlert();
	void ProcessMessageL(const CPushMessage& aPushMsg);
	
private: // Functions from base classes

	void HandleMessageL(CPushMessage* aPushMsg, TRequestStatus& aStatus);  //from CPushHandlerBase
	void CancelHandleMessage();                                            //from CPushHandlerBase   
	void HandleMessageL(CPushMessage* aPushMsg);                           //from CPushHandlerBase    
    void CPushHandlerBase_Reserved1();                                     //from CPushHandlerBase    
	void CPushHandlerBase_Reserved2();                                     //from CPushHandlerBase

    void DoCancel();       // from CActive
	void RunL();           // from CActive

	};

#endif	// __NSMLDSWAPPUSHALERT_H__