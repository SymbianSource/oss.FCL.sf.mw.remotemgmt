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

#include <badesca.h>
#include <Ecom/ImplementationInformation.h>
#include <e32cmn.h>
#include <f32file.h>
#include <scpclient.h>
#include <PolicyEngineXACML.h>
#include <PolicyEngineClient.h>
#include <CentralRepository.h>
#include "debug.h"
#include "dmcertapiSession.h"
#include "dmcertapiClientServer.h"


#include "dmcert.h"

const TUid KCRUidPolicyManagementUtilInternalKeys = {0x10207843};
const TUint32 KSyncMLSessionCertificate = 0x00000001;

CdmcertapiSession::CdmcertapiSession()
	{
	RDEBUG("CdmcertapiSession::CdmcertapiSession");
	}

CdmcertapiSession::~CdmcertapiSession()
	{
	RDEBUG("CdmcertapiSession::~CdmcertapiSession");
	Server().DropSession();
	}

CdmcertapiServer& CdmcertapiSession::Server()
	{
	return *static_cast<CdmcertapiServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

void CdmcertapiSession::CreateL()
	{
	RDEBUG("CdmcertapiSession::CreateL");
	Server().AddSession();
	}

void CdmcertapiSession::ServiceL(const RMessage2& aMessage)
	{
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
	}

void CdmcertapiSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CdmcertapiSession::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}	

void CdmcertapiSession::DispatchMessageL(const RMessage2& aMessage)
	{
	RDEBUG("CdmcertapiSession::DispatchMessageL");
	TInt ret = KErrNone;
	switch(aMessage.Function())
		{
		case EGetCertificate:
		{
			TInt err = KErrNone;
	
			TCertInfo info ;
			CreateCertificate( info ); 	

			CRepository *re = NULL;
			TRAPD( erx, re = CRepository::NewL ( 
		            KCRUidPolicyManagementUtilInternalKeys ) );
			if (erx == KErrNone )
			{
				TPckg<TCertInfo> pcert( info );
				err = re->Create( KSyncMLSessionCertificate, pcert ) ;
				if ( err == KErrNone )
				{
					//DBG_ARGS8(_S8("Wrote reposotry key %S"), &pcert );
				}
				else
				{
					if ( err == KErrAlreadyExists )
					{
						err = re->Set( KSyncMLSessionCertificate, pcert ) ;
						if ( err != KErrNone )
							{
							//
							}
					}
					else
					{
						//
					}
				}	
				delete re ;
				re = NULL;
			}
			else
			{
				//	DBG_ARGS8(_S8("ERROR Failed to open reposiritry %d"), erx );	
			}
		
		TCertInfo info1 ;
		RDMCert rdm;
		rdm.Get(info1);
		ret = KErrNone;
		}
		break;
		default:
			break;
		}
		TPckgBuf<TInt> retPackage(ret);
		aMessage.WriteL(0,retPackage);
	}
	
void CdmcertapiSession::CreateCertificate( TCertInfo& aCertInfo )
{
	// This is not valid certificate, need to provide/fill aCertInfo with valid certificate
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);

	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);
	aCertInfo.iFingerprint.Append(0x01);

	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	aCertInfo.iSerialNo.Append(0x01); 
	
	aCertInfo.iIssuerDNInfo.iCountry = _L8("FI");
	aCertInfo.iIssuerDNInfo.iOrganizationUnit = _L8("Nokia");
	aCertInfo.iIssuerDNInfo.iOrganization = _L8("Nokia");
	aCertInfo.iIssuerDNInfo.iCommonName = _L8("Nokia");
	aCertInfo.iSubjectDNInfo.iCommonName = _L8("74.125.39.18");				
}
	
// end of file

