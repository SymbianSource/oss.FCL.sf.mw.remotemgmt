/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  SyncML HTTP client
*
*/


#include "nsmlhttpclient.h"
#include "nsmlerror.h"
#include <featmgr.h>
#include <nsmloperatorerrorcrkeys.h>
#include <nsmloperatordatacrkeys.h> // KCRUidOperatorDatasyncInternalKeys
#include <centralrepository.h> 

//CONSTANTS
const TInt KErrorCodeRangeFirst = 400;
const TInt KErrorCodeRangeLast = 516;

//Fix to Remove the Bad Compiler Warnings
#ifndef __WINS__
// This lowers the unnecessary compiler warning (armv5) to remark.
// "Warning:  #174-D: expression has no effect..." is caused by 
// DBG_ARGS8 macro in no-debug builds.
#pragma diag_remark 174
#endif


//============================================================
// Class CHttpEventHandler
//============================================================

//------------------------------------------------------------
// CHttpEventHandler::CHttpEventHandler()
// constructor
//------------------------------------------------------------
CHttpEventHandler::CHttpEventHandler()
	{
	}
//------------------------------------------------------------
// CHttpEventHandler::ConstructL( CNSmlHTTP* aAgent )
// 2-phase constructor
//------------------------------------------------------------
void CHttpEventHandler::ConstructL( CNSmlHTTP* aAgent )
	{
	FeatureManager::InitializeLibL();
	iAgent = aAgent;
 
    CRepository* rep = NULL;
    rep = CRepository::NewL( KCRUidOperatorDatasyncInternalKeys );
    CleanupStack::PushL( rep );
    rep->Get( KNsmlOpDsHttpErrorReporting, iErrorReportingEnabled );
    CleanupStack::PopAndDestroy( rep );
 
    iRepositorySSC = CRepository::NewL( KCRUidOperatorDatasyncErrorKeys );
	}
//------------------------------------------------------------
// CHttpEventHandler::~CHttpEventHandler()
// destructor
//------------------------------------------------------------
CHttpEventHandler::~CHttpEventHandler()
	{
    FeatureManager::UnInitializeLib();		
    delete iRepositorySSC;
	}
//------------------------------------------------------------
// CHttpEventHandler::NewLC()
//
//------------------------------------------------------------
CHttpEventHandler* CHttpEventHandler::NewLC()
	{
	CHttpEventHandler* me = new(ELeave)CHttpEventHandler();
	CleanupStack::PushL( me );
	return me;
	}
//------------------------------------------------------------
// CHttpEventHandler::NewL()
//
//------------------------------------------------------------
CHttpEventHandler* CHttpEventHandler::NewL()
	{
	CHttpEventHandler* me = NewLC();
	CleanupStack::Pop( me );
	return me;
	}
//------------------------------------------------------------
// CHttpEventHandler::MHFRunL( RHTTPTransaction aTransaction, 
// const THTTPEvent& aEvent )
//
//------------------------------------------------------------
void CHttpEventHandler::MHFRunL( 
    RHTTPTransaction aTransaction, 
    const THTTPEvent& aEvent )
	{
	TRequestStatus* iStatusForAgent = &this->iAgent->iStatus;
	
	switch ( aEvent.iStatus )
		{
        case THTTPEvent::EGotResponseHeaders:
			{
			DBG_FILE_CODE( aEvent.iStatus, 
                    _S8("EGotResponseHeaders...") );
//tarm start
if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
{
			this->iAgent->SaveCertificateL( aTransaction );
}
//tarm end
			// HTTP response headers have been received. 
			// We can determine now if there is
			// going to be a response body to save.
			RHTTPResponse resp = aTransaction.Response();

			// check that content-type is appropriate
			RStringPool strPool = aTransaction.Session().StringPool();
			RHTTPHeaders hdr = resp.GetHeaderCollection();
			RStringF contentType = strPool.OpenFStringL( KSmlContentType );
			THTTPHdrVal value;
			hdr.GetField( contentType, 0, value );
			
			RStringF fieldValStr = strPool.StringF( value.StrF() ); 
			const TDesC8& contentTypeStr = fieldValStr.DesC();
			
			TInt status = resp.StatusCode();
			DBG_FILE_CODE( status, _S8("Received HTTP status...") );
			
			DBG_ARGS8(_S8("CHttpEventHandler::MHFRunL() contentTypeStr: %S"), &contentTypeStr );
			
			if ( contentTypeStr != KSmlContentTypeDS 
				&& contentTypeStr != KSmlContentTypeDM )
				{
                if( this->iAgent->iSession == ESyncMLDSSession )
                    {
                    if( iErrorReportingEnabled && ( ( status >= KErrorCodeRangeFirst ) 
                            && ( status <= KErrorCodeRangeLast ) ) )
                        {
                        iRepositorySSC->Set( KNsmlOpDsSyncErrorCode, status );
                        }
                    }  

				//Error fix for BPSS-7H7H5S				
				DBG_FILE( _S8("CHttpEventHandler::MHFRunL() There is a mismatch in the Content Type") );
				
				status = CNSmlHTTP::SetErrorStatus( status );
				
				if (status == resp.StatusCode() )
				{
					DBG_FILE( _S8("Error in Communication string is set"));
					status = TNSmlError::ESmlCommunicationInterrupted;
				}
					
				// content mismatch
				aTransaction.Close();
				User::RequestComplete( iStatusForAgent, status);
				
				break;
				}

			if(this->iAgent->iSession == ESyncMLDSSession)
			    {
    	         RStringF serverContentEncoding = strPool.OpenFStringL( KSmlContentEncodingType );
    	         THTTPHdrVal serverContentEncodingValue;
    	         if(hdr.GetField( serverContentEncoding, 0, serverContentEncodingValue ) != KErrNotFound)	         
    	             {
        	         RStringF fieldServerContentEncodingValStr = strPool.StringF( serverContentEncodingValue.StrF() ); 
        	         const TDesC8& serverContentEncodingStr = fieldServerContentEncodingValStr.DesC();
        	         if( serverContentEncodingStr == KSmlContentDeflate)
        	             {
        	             this->iAgent->iServerContentEncoding = CNSmlHTTP::ExptDeflate;
        	             }
    	             }
    
    			 RStringF serverAcceptEncoding = strPool.OpenFStringL( KSmlAcceptEncodingType );
    	         THTTPHdrVal serverAcceptEncodingValue;
    	         if(hdr.GetField( serverAcceptEncoding, 0, serverAcceptEncodingValue )  != KErrNotFound )	         
    	             {
        	         RStringF fieldServerAcceptEncodingValStr = strPool.StringF( serverAcceptEncodingValue.StrF() ); 
        	         const TDesC8& serverAcceptEncodingStr = fieldServerAcceptEncodingValStr.DesC();
        	         if( serverAcceptEncodingStr.Find(KSmlContentDeflate) != KErrNotFound)
        	             {
        	             this->iAgent->iServerAcceptEncoding = CNSmlHTTP::ExptDeflate;
        	             }
    	             }
			    }
			
			if ( status == 200 )
				{
				this->iAgent->iDocumentLength = 0;
				}
			else if ( resp.HasBody() 
					&& ( status >= 200 ) 
					&& ( status < 300 ) 
					&& ( status != 204 ) )
				{
				TInt dataSize = resp.Body()->OverallDataSize();
				this->iAgent->iDocumentLength = dataSize;
				DBG_FILE_CODE( dataSize, _S8("Incoming packet length...") );
				}
			else
				{
				// canceling
				aTransaction.Close();
				DBG_FILE_CODE( status, _S8("Adding offset to http status...") );
				status = CNSmlHTTP::SetErrorStatus( status );
				DBG_FILE_CODE( status, _S8("Complete with internal \
				                http status value ...") );
				User::RequestComplete( iStatusForAgent, status );
				} 
			delete this->iAgent->iData;
			this->iAgent->iData = NULL;
			this->iAgent->iData = HBufC8::NewL( this->iAgent->iMaxMsgSize );
			} break;
        case THTTPEvent::EGotResponseBodyData:
            {
			DBG_FILE_CODE( aEvent.iStatus, _S8("EGotResponseBodyData...") );
			// Get the body data supplier
			iRespBody = aTransaction.Response().Body();
			RHTTPResponse resp = aTransaction.Response();
			TInt respDataSize = resp.Body()->OverallDataSize();
			DBG_FILE_CODE( respDataSize, _S8("number of bytes received...") );
			
			TPtrC8 bodyData;
			iRespBody->GetNextDataPart( bodyData );

			TPtr8 iDataPtr( this->iAgent->iData->Des() );
			if (respDataSize > this->iAgent->iMaxMsgSize)
				{	
				User::LeaveIfError( TNSmlError::ESmlStatusSizeMismatch );
				}
			else
				{
				iDataPtr.Append( bodyData );	
				}			

			// Done with that bit of body data
			iRespBody->ReleaseData();
			} break;
        case THTTPEvent::EResponseComplete:
            {
			DBG_FILE_CODE( aEvent.iStatus, _S8("EResponseComplete...") );
			} break;
        case THTTPEvent::ESucceeded:
            {
			DBG_FILE_CODE( aEvent.iStatus, _S8("ESucceeded...") );
			aTransaction.Close();
            User::RequestComplete( iStatusForAgent, KErrNone ); 
            } break;
        case THTTPEvent::EFailed:
            {
			DBG_FILE_CODE( aEvent.iStatus, _S8("EFailed...") );
			DBG_FILE_CODE( this->iAgent->iPreemptRequest, _S8("was this  prempted , is it > 0 so ignore ") );
			if(this->iAgent->iPreemptRequest==0)
			{
			aTransaction.Close();
            User::RequestComplete( iStatusForAgent, aEvent.iStatus );    	
			}
			else
			{
			 aTransaction.Close();
			 this->iAgent->iPreemptRequest--;	
			}
            } break;
		case THTTPEvent::ERedirectedPermanently:
			{
			DBG_FILE_CODE( aEvent.iStatus, _S8("ERedirectedPermanently...") );
			} break;
		case THTTPEvent::ERedirectedTemporarily:
			{
			DBG_FILE_CODE( aEvent.iStatus, _S8("ERedirectedTemporarily...") );
			} break;
       case  KServerUntrusted:
		    {
		    	if(FeatureManager::FeatureSupported(KFeatureIdSapPolicyManagement))
		    	{
		    DBG_FILE_CODE( aEvent.iStatus, _S8("Euntrustedcert") );	
		    aTransaction.Close();
            User::RequestComplete( iStatusForAgent, TNSmlError::ESmlUntrustedCert );
          }
		    }break;
        default:
            {
			if ( aEvent.iStatus < 0 )
				{
				DBG_FILE_CODE( aEvent.iStatus, _S8("error...") );
				}
            } break;
        }
	}
//------------------------------------------------------------
// CHttpEventHandler::MHFRunError( TInt /*aError*/, 
// RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/ )
//
//------------------------------------------------------------
TInt CHttpEventHandler::MHFRunError( 
    TInt /*aError*/, 
    RHTTPTransaction /*aTransaction*/, 
    const THTTPEvent& /*aEvent*/ )
	{
	return KErrNone;
	}



