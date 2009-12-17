/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/
 

#include <mmf/common/mmfcontrollerpluginresolver.h>
#include "TrustSession.h"
#include "TrustClientServer.h"
#include <badesca.h>
#include "debug.h"
#include <e32cmn.h>
#include <f32file.h>

CTrustSession::CTrustSession()
	{
	RDEBUG("CTrustSession::CTrustSession");
	}


CTrustSession::~CTrustSession()
	{
	RDEBUG("CTrustSession::~CTrustSession");
	Server().DropSession();
	}

CTrustServer& CTrustSession::Server()
	{
	return *static_cast<CTrustServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// ----------------------------------------------------------------------------------------
// CTrustSession::CreateL
// 2nd phase construct for sessions - called by the CServer framework
// ----------------------------------------------------------------------------------------
void CTrustSession::CreateL()
	{
	RDEBUG("CTrustSession::CreateL");
	Server().AddSession();
	
	}


void CTrustSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}

void CTrustSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CTrustSession::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}
	

void CTrustSession::DispatchMessageL(const RMessage2& aMessage)
	{
	
	switch(aMessage.Function())
		{
		case EAddTrust:
			{
				TCertInfo certInfo;
				TPckg<TCertInfo> certInfoPckg(certInfo);
				aMessage.ReadL(1,certInfoPckg);
				TInt err = Server().AddSessionTrustL(certInfo);
				TPckg<TInt> result(err);
				aMessage.WriteL(0,result);
			}
			break;
		
		case EGetElementList:
			{
				RElementIdArray array;
				CleanupClosePushL( array);
				TElementType type = EPolicySets;
				
				TPckgBuf<TInt> EleTypePkg(type);
				aMessage.ReadL(1,EleTypePkg);
				type =(TElementType) EleTypePkg();
				
				TInt err = Server().GetElementListL(type,array);
				
				TPckgBuf<TInt> result(err);
			
				TInt count = array.Count();
				
				aMessage.WriteL(0,result);
				CleanupStack::PopAndDestroy();
			}
			break;
			
		case EGetElement:
			{
			    HBufC8* buffer = HBufC8::NewLC( 300 );
			    TPtr8 ptr( buffer->Des() );
			    aMessage.ReadL( 0, ptr );	
			  	TElementInfo elementInfo(ptr);
			  	TPtrC8 ptrNullDes = elementInfo.GetDescription();  //just to get the code coverage
				User::LeaveIfError( Server().GetElementL(elementInfo));
				
				TPtrC8 ptrDes = elementInfo.GetDescription();
				TPtrC8 ptrID = elementInfo.GetElementId();
							
				for ( TInt i = 0; i < elementInfo.GetChildElementArray().Count(); i++)
				{
				    RDEBUG_2("CTrustSession::DispatchMessageL Child element ids %S", elementInfo.GetChildElementArray()[i]);
				}
				CleanupStack::PopAndDestroy(buffer);
			}
			break;
			
		case EGetXACMLDescription:
			{
				HBufC8* buffer = HBufC8::NewLC( 300 );
			    TPtr8 ptr( buffer->Des() );
			    aMessage.ReadL( 0, ptr );	
			  	TElementInfo elementInfo(ptr);
			  	TPtrC8 ptrNullXACML(elementInfo.GetXACML());  //just to get the code coverage
				User::LeaveIfError( Server().GetXACMLDescriptionL(elementInfo));
				
				TPtrC8 ptrXACML(elementInfo.GetXACML());
//				RDEBUG_2("CTrustSession::DispatchMessageLGetXACML %S", ptrXACML);
				
				CleanupStack::PopAndDestroy(buffer);
			}
			break;
   	
		default:
			break;
		}
	}
