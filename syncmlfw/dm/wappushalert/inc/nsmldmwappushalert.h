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
* Description:  Ecom plug-in for x-wap-application:syncml.dm application ID          
*
*/


#ifndef __NSMLDMWAPPUSHALERT_H__
#define __NSMLDMWAPPUSHALERT_H__

// INCLUDES
#include <e32base.h>
#include <cpushhandlerbase.h>

// FORWARD DECLARATIONS
//
class CContentHandlerBase;

// CONSTANTS
//
const TUint KNSmlDMWapPushAlertImplUid = 0x101F6E38;


// CLASS DECLARATION
class CNSmlDMWapPushAlert : public CPushHandlerBase
	{
public:	// constructor and destructor

	static CNSmlDMWapPushAlert* NewL();
	virtual ~CNSmlDMWapPushAlert();

private:  // New functions

	CNSmlDMWapPushAlert();
	void ProcessMessageL(const CPushMessage& aPushMsg);
	
private: // Functions from base classes

	void HandleMessageL(CPushMessage* aPushMsg, TRequestStatus& aStatus);  //from CPushHandlerBase
	void CancelHandleMessage();                                            //from CPushHandlerBase   
	void HandleMessageL(CPushMessage* aPushMsg);                           //from CPushHandlerBase    
    void CPushHandlerBase_Reserved1();                                     //from CPushHandlerBase    
	void CPushHandlerBase_Reserved2();                                     //from CPushHandlerBase

    void DoCancel();       // from CActive
	void RunL();           // from CActive


private:	// Data
    //
	// Used classes
	//
	//CContentHandlerBase*		iContentHandlerPi;
	};

#endif	// __NSMLDMWAPPUSHALERT_H__