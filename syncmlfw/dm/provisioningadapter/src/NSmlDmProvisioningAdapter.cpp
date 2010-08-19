/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Main class for handling provisioning Provisioning message
*
*/



// INCLUDE FILES
#include <badesca.h>
#include <commdb.h>
#include <cdbcols.h>			 // CommsDB columname defs
#include <stringresourcereader.h>
#include <barsread.h>
#include <nsmldmprovisioningadapter.rsg>
#include <f32file.h>
#include <bautils.h>
#include <utf.h>
#include <featmgr.h>
#include <barsc.h> 
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <nsmldebug.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <CWPEngine.h>
#include <NSmlPrivateAPI.h>
#include "NSmlDmProvisioningAdapter.h"
#include "NSmlTransportHandler.h"

#include <data_caging_path_literals.hrh>
#define KMINPORT 0
#define KMAXPORT 65536

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::CNSmlDmProvisioningAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CNSmlDmProvisioningAdapter::CNSmlDmProvisioningAdapter()
	:CWPAdapter(), iAuthSecretLimitIndicator(0)
	{
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CNSmlDmProvisioningAdapter::ConstructL()
	{
	iSession.OpenL();
	FeatureManager::InitializeLibL();
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CNSmlDmProvisioningAdapter* CNSmlDmProvisioningAdapter::NewL()
	{
	_DBG_FILE("CNSmlDmProvisioningAdapter::NewL(): begin");
	CNSmlDmProvisioningAdapter* self = new(ELeave) CNSmlDmProvisioningAdapter; 
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	_DBG_FILE("CNSmlDmProvisioningAdapter::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CNSmlDmProvisioningAdapter::~CNSmlDmProvisioningAdapter()
	{
	iProfiles.ResetAndDestroy();
	iProfiles.Close();
	
	delete iTitle;
	iSession.Close();
	FeatureManager::UnInitializeLib();
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CNSmlDmProfileElement::~CNSmlDmProfileElement()
	{
	iVisitParameter = 0;
	
	delete iServerNonce;
	delete iHostAddress;
	delete iPort;
	delete iHTTPUserName;
	delete iHTTPPassword;
	
	delete iDisplayName;
	delete iServerId;
	delete iUsername;
	delete iPassword;
	delete iServerPassword;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
TBool CNSmlDmProvisioningAdapter::CanSetAsDefault(TInt /*aItem*/) const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::SetAsDefaultL
// -----------------------------------------------------------------------------
void CNSmlDmProvisioningAdapter::SetAsDefaultL(TInt /*aItem*/ )
	{
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::ItemCount
// -----------------------------------------------------------------------------
TInt CNSmlDmProvisioningAdapter::ItemCount() const
	{
	return iProfiles.Count();
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::SummaryTitle
// -----------------------------------------------------------------------------
const TDesC16& CNSmlDmProvisioningAdapter::SummaryTitle(TInt /*aIndex*/) const
	{
	return *iTitle;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::SummaryText
// -----------------------------------------------------------------------------
const TDesC16& CNSmlDmProvisioningAdapter::SummaryText(TInt aIndex) const
	{
	return *iProfiles[aIndex]->iDisplayName;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::SaveL
// -----------------------------------------------------------------------------
void CNSmlDmProvisioningAdapter::SaveL(TInt aItem)
	{
		_DBG_FILE("CNSmlDmProvisioningAdapter::SaveL(): begin");
	TInt serverID = 0;
	TInt hostAddress = 0;
	TInt username = 0;

	if(iProfiles[aItem]->iServerId && iProfiles[aItem]->iHostAddress && 
	iProfiles[aItem]->iUsername)
	{
		TPtrC8 serverPtr = iProfiles[aItem]->iServerId->Des();
		TPtrC hostAddressPtr = iProfiles[aItem]->iHostAddress->Des();
		TPtrC8 usernamePtr = iProfiles[aItem]->iUsername->Des();		
		serverID = serverPtr.Compare(KNSmlDMProvisioningNoVal);
		hostAddress = hostAddressPtr.Compare(KNSmlDMProvisioningNoVal16);
		username = usernamePtr.Compare(KNSmlDMProvisioningNoVal);
	}
	else
	{
		User::Leave(KErrGeneral);
	}

	if( ( serverID == 0 ) || ( hostAddress == 0 ) || ( username == 0 ))
	{
		User::Leave(KErrGeneral);
	}
	if (iAuthSecretLimitIndicator)
	{
		iAuthSecretLimitIndicator = 0;
		User::Leave(KErrOverflow);
	}
	
	//check for incorrect port
	//Only port address between 1 to 65536 is allowed. 
	if( iProfiles[aItem]->iPort )
		{
			const TDesC& port = iProfiles[aItem]->iPort->Des();
			TInt len = port.Length();
			if(len > 0)
			{
        HBufC* bufPort = port.AllocL();
        TLex aLex(*bufPort);
        TInt portNum ;
        TInt err = aLex.Val(portNum);
        if(bufPort)
        {
          delete bufPort;
          bufPort = NULL;
        }
	    if(err != KErrNone)
	    {
          User::Leave(KErrGeneral);
	    }
	    if(!((portNum > KMINPORT) && (portNum < KMAXPORT)))
	    {
	      User::Leave(KErrGeneral);       
	    }
		}
	}
	
	TPckgBuf<TUint32> uid;
	
	RSyncMLDevManProfile profile,ProfileToSearch;
	RArray<TSmlProfileId> arr;
  iSession.ListProfilesL( arr, ESmlDevMan );
  TBool ret = EFalse;
  TInt ProfileId = KErrNotFound;
  CleanupClosePushL(arr);   	
	for ( TInt index = 0; index < arr.Count(); index++ )
		{
	    TRAPD( error, ProfileToSearch.OpenL(iSession, arr[index], ESmlOpenRead ) );
	    if ( error == KErrNone )
	        {	            	            	        
    	        if ( ( iProfiles[aItem]->iServerId->Des()).Compare(ProfileToSearch.ServerId() ) == 0 )
    	            {
    	            
                    ret = ETrue;                    
                    ProfileId = (TInt)arr[index];
                    ProfileToSearch.Close();
                    break;
                    }	            		    
	        }
	      ProfileToSearch.Close();  
		}
    CleanupStack::PopAndDestroy( &arr );
	
	if( ret )
		{
		profile.OpenL(iSession, ProfileId , ESmlOpenReadWrite );	
		TInt isprofilelocked=profile.ProfileLocked(EFalse, EFalse);
		if (isprofilelocked == 1)
		 {
		  
      profile.Close(); 
      User::Leave(KErrAccessDenied); 	 	
		 }
		
		}
	else
		{
	profile.CreateL( iSession );
		}
	CleanupClosePushL( profile );
	// ui needs this to be set 0
	profile.SetCreatorId(0);
	
	profile.SetSanUserInteractionL( iProfiles[aItem]->iServerAlertedAction );
	
	if ( iProfiles[aItem]->iDisplayName )
	    {	    
	    profile.SetDisplayNameL( iProfiles[aItem]->iDisplayName->Des() );    
	    }
	
	if ( iProfiles[aItem]->iServerId )
	    {	    
	    profile.SetServerIdL( iProfiles[aItem]->iServerId->Des() );    
	    }
	
    if ( iProfiles[aItem]->iServerPassword )
	    {	    
	    profile.SetServerPasswordL( iProfiles[aItem]->iServerPassword->Des() );    
	    }
		        
	if ( iProfiles[aItem]->iUsername )   
	    {	    
	    profile.SetUserNameL( iProfiles[aItem]->iUsername->Des() );    
	    }
	
	if ( iProfiles[aItem]->iPassword )
	    {	    
	    profile.SetPasswordL( iProfiles[aItem]->iPassword->Des() );    
	    }
		    
	// creates profile -> must be done before opening the connection
	profile.UpdateL();
	
	RSyncMLConnection connection;
	connection.OpenL( profile, KUidNSmlMediumTypeInternet.iUid );
	CleanupClosePushL( connection );
		
	if( iProfiles[aItem]->iVisitParameter && iProfiles[aItem]->iVisitParameter->Data().Length() == uid.MaxLength() )
		{
		uid.Copy( iProfiles[aItem]->iVisitParameter->Data() );
		
		RCmManagerExt  cmmanagerExt;
		cmmanagerExt.OpenL();
		CleanupClosePushL(cmmanagerExt);
		RCmConnectionMethodExt cm;
		cm = cmmanagerExt.ConnectionMethodL( uid());
		CleanupClosePushL( cm );

		TUint apId = 0;
		TRAPD( ERROR, apId = cm.GetIntAttributeL(CMManager::ECmIapId) );
		if( ERROR != KErrNone )
			{
			apId = GetDefaultIAPL();
			}
				
		HBufC8* iapBuf = HBufC8::NewLC( 8 );
		TPtr8 ptrBuf = iapBuf->Des();
		ptrBuf.Num( apId );

		connection.SetPropertyL( KNSmlIAPId, *iapBuf );
		
		CleanupStack::PopAndDestroy( 3 ); //cmmanager,cm, iapBuf
		}
		
	if( iProfiles[aItem]->iHostAddress )
		{
		// see if address contains also port
		TBool portFound = EFalse;
		
		TInt startPos(0);
		if(iProfiles[aItem]->iHostAddress->Find(KNSmlDMProvisioningHTTP)==0)
		    {
		    startPos=KNSmlDMProvisioningHTTP().Length();		    		    
		    }
		else if(iProfiles[aItem]->iHostAddress->Find(KNSmlDMProvisioningHTTPS)==0)
		    {
		    startPos=KNSmlDMProvisioningHTTPS().Length();		    		    
		    }
		TPtrC uriPtr = iProfiles[aItem]->iHostAddress->Mid(startPos);
		
		if(uriPtr.Locate(KNSmlDMColon)!=KErrNotFound)
		    {
			portFound = ETrue;
		    }
	
		if( portFound == EFalse )
			{
			HBufC *uri = 0;
			// port not found from URI -> see if it is given separately				
			if( iProfiles[aItem]->iPort )
				{
				// parse address and port into URI				
				if( CombineURILC( iProfiles[aItem]->iHostAddress->Des(),
							  iProfiles[aItem]->iPort->Des(), uri ) == KErrNone )
					{
					if(iProfiles[aItem]->iHostAddress)
					{
						delete iProfiles[aItem]->iHostAddress;
						iProfiles[aItem]->iHostAddress = NULL;
					}

					iProfiles[aItem]->iHostAddress = uri->Des().AllocL();
					}
				CleanupStack::PopAndDestroy(); // uri		  
				}
			else
				{
				// use default port
				if( CombineURILC( iProfiles[aItem]->iHostAddress->Des(),
							  KNSmlDmDefaultPort(), uri ) == KErrNone )
					{
					if(iProfiles[aItem]->iHostAddress)
					{
						delete iProfiles[aItem]->iHostAddress;
						iProfiles[aItem]->iHostAddress = NULL;
					}

					iProfiles[aItem]->iHostAddress = uri->Des().AllocL();
					}
				CleanupStack::PopAndDestroy(); // uri					
				}
			}
		

		connection.SetServerURIL( ConvertTo8LC( iProfiles[aItem]->iHostAddress->Des() ) );
		CleanupStack::PopAndDestroy(); // ConvertTo8LC
		}
			
	// set authtype, HTTPUserName, HTTPPassword
	if( iProfiles[aItem]->iHTTPUserName ||
	    iProfiles[aItem]->iHTTPPassword )
		{
		connection.SetPropertyL( KNSmlHTTPAuth, KNSmlTrueVal );
		
		if( iProfiles[aItem]->iHTTPUserName )
			{
			connection.SetPropertyL( KNSmlHTTPUsername, iProfiles[aItem]->iHTTPUserName->Des() );
			}
		if( iProfiles[aItem]->iHTTPPassword )
			{
			connection.SetPropertyL( KNSmlHTTPPassword, iProfiles[aItem]->iHTTPPassword->Des() );
			}
		}
	else
		{
		connection.SetPropertyL( KNSmlHTTPAuth, KNSmlFalseVal );
		}
		
	connection.UpdateL();
	CleanupStack::PopAndDestroy(); //connection

	profile.UpdateL();

	if( iProfiles[aItem]->iServerNonce )
		{
		RNSmlPrivateAPI privateApi;
		iProfiles[aItem]->iAuthInfo.iProfileId = profile.Identifier();		
 		iProfiles[aItem]->iAuthInfo.iServerNonce = ConvertTo8LC( iProfiles[aItem]->iServerNonce->Des() ).AllocL();
 		CleanupStack::PopAndDestroy(); //ConvertTo8LC 
 		iProfiles[aItem]->iAuthInfo.iClientNonce = KNullDesC8().AllocL();
 		
 		privateApi.OpenL();
 		CleanupClosePushL( privateApi );
 		TInt err( KErrNone );
 		TRAP(err, privateApi.SetDMAuthInfoL( iProfiles[aItem]->iAuthInfo ));
 		CleanupStack::PopAndDestroy(); // privateApi
		}

	iProfiles[aItem]->iProfileId = profile.Identifier(); 
	if(iInitSession&(0x01<<(aItem+1)))
	    {
    	_DBG_FILE("CNSmlDmProvisioningAdapter::SaveL(): init session");
	    TInt serverIdLen = profile.ServerId().Length();
	    TInt staticPartLen = KNSmlDmStaticAlertMessagePart().Length();
	    HBufC8* alertMessage = HBufC8::NewLC(staticPartLen+3+serverIdLen);
	    TPtr8 alertPtr = alertMessage->Des();
	    TPckgBuf<TUint8> sessionIdLo(1);
	    TPckgBuf<TUint8> sessionIdHi(0);
	    TPckgBuf<TUint8> serverIdLenBuf(serverIdLen);
	    alertPtr.Append(KNSmlDmStaticAlertMessagePart);
	    alertPtr.Append(sessionIdHi);
	    alertPtr.Append(sessionIdLo);
	    alertPtr.Append(serverIdLenBuf);
	    alertPtr.Append(profile.ServerId());
		DBG_ARGS8( alertMessage->Ptr());
	    
		RNSmlPrivateAPI privateApi;
		privateApi.OpenL();
		CleanupClosePushL(privateApi);
		if(!FeatureManager::FeatureSupported( KFeatureIdSyncMlDm112  ))
		{
		privateApi.SendL( alertPtr, ESmlDevMan, ESmlVersion1_2 );	
		}
		else
		{
		privateApi.SendL( alertPtr, ESmlDevMan, ESmlVersion1_1_2 );	
		}
	    CleanupStack::PopAndDestroy(); //privateApi
	    
	    CleanupStack::PopAndDestroy(alertMessage);	    
	    }

	CleanupStack::PopAndDestroy( &profile );

	_DBG_FILE("CNSmlDmProvisioningAdapter::SaveL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::Reserved_1
// -----------------------------------------------------------------------------
TInt CNSmlDmProvisioningAdapter::Reserved_1()
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::Reserved_2
// -----------------------------------------------------------------------------
TInt CNSmlDmProvisioningAdapter::Reserved_2()
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::VisitL
// -----------------------------------------------------------------------------
void CNSmlDmProvisioningAdapter::VisitL( CWPCharacteristic& aCharacteristic )
	{
	_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPCharacteristic): begin");
	if(aCharacteristic.Type() == KWPApplication)
		{
		iState = CNSmlDmProvisioningAdapter::EStateApplication;
		}
	else if(aCharacteristic.Type() == KWPAppAuth)
		{
		iAuthLevel = CNSmlDmProvisioningAdapter::ENone;
		if(iState != CNSmlDmProvisioningAdapter::EStateDmSettings)
			{
			iState = CNSmlDmProvisioningAdapter::EStateNull;
			}
		}
	else if(aCharacteristic.Type() == KWPAppAddr || aCharacteristic.Type() == KWPResource || aCharacteristic.Type() == KWPPort)
		{
		if(iState != CNSmlDmProvisioningAdapter::EStateDmSettings)
			{
			iState = CNSmlDmProvisioningAdapter::EStateNull;
			}
		}
	else
		{
		iState = CNSmlDmProvisioningAdapter::EStateNull;
		}

	if(iState!=CNSmlDmProvisioningAdapter::EStateNull)
		{
		aCharacteristic.AcceptL( *this );
		}
	_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPCharacteristic): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::VisitL
// -----------------------------------------------------------------------------
void CNSmlDmProvisioningAdapter::VisitL( CWPParameter& aParameter)
	{
	_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): begin");
	
	if( iState == CNSmlDmProvisioningAdapter::EStateApplication )
		{
		if( aParameter.ID() == EWPParameterAppID && aParameter.Value().Compare( KNSmlDMProvisioningDMAppIdVal ) == 0 )
			{
			iState = CNSmlDmProvisioningAdapter::EStateDmSettings;
	
			GetTitleL();

			CNSmlDmProfileElement* dmProfileElement = new(ELeave) CNSmlDmProfileElement;
			CleanupStack::PushL( dmProfileElement );
			
			dmProfileElement->iDisplayName = KNSmlDMProvisioningDefName().AllocL();			
			dmProfileElement->iServerAlertedAction = ESmlConfirmSync;			
			
			dmProfileElement->iServerNonce = 0;
			dmProfileElement->iVisitParameter = 0;			

	 		dmProfileElement->iAuthInfo.iAuthPref = ENoAuth;

			iProfiles.AppendL( dmProfileElement );
			CleanupStack::Pop( dmProfileElement );
			}
		else
			{
			iState = CNSmlDmProvisioningAdapter::EStateNull;
			}
		}
	else if( iState == CNSmlDmProvisioningAdapter::EStateDmSettings )
		{
		switch ( aParameter.ID() )
			{
			case EWPParameterName:
			case EWPParameterName1: {
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): Name");
				DBG_ARGS( aParameter.Value().Ptr() );
				
				if ( iProfiles[iProfiles.Count()-1]->iDisplayName )
				    {
				    delete iProfiles[iProfiles.Count()-1]->iDisplayName;
				    iProfiles[iProfiles.Count()-1]->iDisplayName = NULL;				        
				    }
				    
				if(aParameter.Value().Length()>KNSmlDmMaxProfileNameLength)
					{
						TPtrC ptr2 = aParameter.Value().Left(KNSmlDmMaxProfileNameLength);
						iProfiles[iProfiles.Count()-1]->iDisplayName = ptr2.AllocL();	
					}
				else
				iProfiles[iProfiles.Count()-1]->iDisplayName = aParameter.Value().AllocL();								
				/* parameter is NULL set to default value*/	    		
	    		TInt num = iProfiles[iProfiles.Count()-1]->iDisplayName->Length();
	    			
				/*check for white spaces, If all characters are white spaces set to default*/
				TPtr temp = iProfiles[iProfiles.Count()-1]->iDisplayName->Des();
				TInt i;
				for(i=0; i<num; i++)
	    			{			
	    				if(temp[i] != ' ')
	    				break;
	    			}
			    TInt flag =0;					
	    			
    			if(i == num || num == 0 )
    			  flag =1;
    			
    			if(flag)
    			{
    				if ( iProfiles[iProfiles.Count()-1]->iDisplayName )
			    	{
			    	delete iProfiles[iProfiles.Count()-1]->iDisplayName;
			    	iProfiles[iProfiles.Count()-1]->iDisplayName = NULL;
			    	}
			    	iProfiles[iProfiles.Count()-1]->iDisplayName = KNSmlDMProvisioningDefName().AllocL();
    			}
				break;
				}
			case EWPParameterToProxy:	
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): ToProxy");
			case EWPParameterToNapID:
			case EWPParameterToNapID1:
				break;

			case EWPParameterPortNbr:
			case EWPParameterPortNbr1:
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): PortNbr");
				DBG_ARGS( aParameter.Value().Ptr() );
				iProfiles[iProfiles.Count()-1]->iPort = aParameter.Value().AllocL();
				break;

			case EWPParameterAddr:
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): Addr");
				DBG_ARGS( aParameter.Value().Ptr() );
				if(aParameter.Value().Length()<=KNSmlMaxURLLength)
					{
					iProfiles[iProfiles.Count()-1]->iHostAddress = aParameter.Value().AllocL();
					}
				break;

			case EWPParameterAAuthLevel:
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): AuthLevel");

				if( aParameter.Value().Compare( KNSmlDMProvisioningServerAuth ) == 0 )
					{
					iAuthLevel = CNSmlDmProvisioningAdapter::EServer;
					}
				else if( aParameter.Value().Compare( KNSmlDMProvisioningClientAuth ) == 0 )
					{
					iAuthLevel = CNSmlDmProvisioningAdapter::EClient;
					}
				break;

			case EWPParameterAAuthName:
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): AuthName");
				
				if(aParameter.Value().Length()<=KNSmlDmMaxUsernameLength)
				{
				if(iAuthLevel == CNSmlDmProvisioningAdapter::EClient)
					{
					if ( iProfiles[iProfiles.Count()-1]->iServerId == NULL )
					    {					    
					    iProfiles[iProfiles.Count()-1]->iServerId = ConvertTo8LC( aParameter.Value() ).AllocL();
						CleanupStack::PopAndDestroy(); // ConvertTo8LC
					    }
                    else if ( ( iProfiles[iProfiles.Count()-1]->iServerId->Compare( KNullDesC8() ) ) == 0 )
						{
						delete iProfiles[iProfiles.Count()-1]->iServerId;
						iProfiles[iProfiles.Count()-1]->iServerId = NULL;
						
						iProfiles[iProfiles.Count()-1]->iServerId = ConvertTo8LC( aParameter.Value() ).AllocL();
						CleanupStack::PopAndDestroy(); // ConvertTo8LC					    
						}										
					}
					
				else if( iAuthLevel == CNSmlDmProvisioningAdapter::EServer )
					{
					iProfiles[iProfiles.Count()-1]->iUsername = ConvertTo8LC( aParameter.Value() ).AllocL();					
					CleanupStack::PopAndDestroy(); // ConvertTo8LC

					if( iProfiles[iProfiles.Count()-1]->iAuthInfo.iAuthPref == ENoAuth )
						{
						iProfiles[iProfiles.Count()-1]->iAuthInfo.iAuthPref = EBasic;
						}
					}
				else if (iAuthLevel == CNSmlDmProvisioningAdapter::EHttp)
                    {
			        iProfiles[iProfiles.Count()-1]->iHTTPUserName = ConvertTo8LC( aParameter.Value() ).AllocL();
			        CleanupStack::PopAndDestroy(); //ConvertTo8LC
			        }
				}
				break;

			case EWPParameterAAuthSecret:
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): AuthSecret");
				if(aParameter.Value().Length()<=KNSmlDmMaxPasswordLength)
				{
				if( iAuthLevel == CNSmlDmProvisioningAdapter::EClient )
					{
					iProfiles[iProfiles.Count()-1]->iServerPassword = ConvertTo8LC( aParameter.Value() ).AllocL();					
					CleanupStack::PopAndDestroy(); // ConvertTo8LC
					}
				else if ( iAuthLevel == CNSmlDmProvisioningAdapter::EServer )
					{
					iProfiles[iProfiles.Count()-1]->iPassword = ConvertTo8LC( aParameter.Value() ).AllocL();				
					CleanupStack::PopAndDestroy(); // ConvertTo8LC
					}
				else if (iAuthLevel == CNSmlDmProvisioningAdapter::EHttp)
                    {
			        iProfiles[iProfiles.Count()-1]->iHTTPPassword = ConvertTo8LC( aParameter.Value() ).AllocL();
			        CleanupStack::PopAndDestroy(); //ConvertTo8LC
			        }
				}
				else
				iAuthSecretLimitIndicator = 1;
				break;

			case EWPParameterAAuthData:
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): AuthData");
				if( iAuthLevel == CNSmlDmProvisioningAdapter::EServer )
					{
					iProfiles[iProfiles.Count()-1]->iAuthInfo.iAuthPref = EMD5;
					iProfiles[iProfiles.Count()-1]->iServerNonce = aParameter.Value().AllocL();					
					}
				break;

			case EWPParameterAAuthType:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): AuthType");
				DBG_ARGS( aParameter.Value().Ptr() );
				if (( aParameter.Value().Compare( KNSmlDMProvisioningHTTPBasic ) == 0 ) ||
					( aParameter.Value().Compare( KNSmlDMProvisioningHTTPDigest ) == 0 ))
					{
					_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): HTTP");
					iAuthLevel = CNSmlDmProvisioningAdapter::EHttp;
					} 
				break;
				
			case EWPParameterProviderID:
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): ProviderId");
				if(aParameter.Value().Length()<=KNSmlDmServerIdMaxLength)
				{
				iProfiles[iProfiles.Count()-1]->iServerId = ConvertTo8LC( aParameter.Value() ).AllocL();				
				CleanupStack::PopAndDestroy(); // ConvertTo8LC
				}
				break;
				
			case EWPNamedParameter:
   				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): Named parameter");
			    if(aParameter.Name().Compare(KNSmlDMProvisioningINIT)==0)
			        {
    				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): Named parameter (INIT)");
    				iInitSession= iInitSession|(0x01<<iProfiles.Count());
			        }
			    break;
			default:
				break;
			}
		}
	
	_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL( CWPParameter ): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::VisitLinkL
// -----------------------------------------------------------------------------
void CNSmlDmProvisioningAdapter::VisitLinkL( CWPCharacteristic& aParameter)
	{
	_DBG_FILE("CNSmlDmProvisioningAdapter::VisitLinkL(): begin");
	
	if( ( ( aParameter.Type()==KWPNapDef ) || ( aParameter.Type()==KWPPxLogical ) ) &&
		( iState == CNSmlDmProvisioningAdapter::EStateDmSettings ) )
		{
		if( iProfiles.Count() > 0 )
			{
			iProfiles[iProfiles.Count()-1]->iVisitParameter = &aParameter;
			}
		}
	_DBG_FILE("CNSmlDmProvisioningAdapter::VisitLinkL(): end");
	}


// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::ContextExtension
// -----------------------------------------------------------------------------
TInt CNSmlDmProvisioningAdapter::ContextExtension( MWPContextExtension*& aContextExtension )
	{
	_DBG_FILE("CNSmlDmProvisioningAdapter::ContextExtension(): begin");
	aContextExtension = this;
	_DBG_FILE("CNSmlDmProvisioningAdapter::ContextExtension(): end");
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::SaveDataL
// -----------------------------------------------------------------------------
const TDesC8& CNSmlDmProvisioningAdapter::SaveDataL( TInt aIndex ) const
	{
	return iProfiles[aIndex]->iProfileId;
	}


// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::DeleteL
// -----------------------------------------------------------------------------
void CNSmlDmProvisioningAdapter::DeleteL( const TDesC8& aSaveData)
	{
	_DBG_FILE("CNSmlDmProvisioningAdapter::DeleteL(): begin");
	TPckgBuf<TInt> uid;
	uid.Copy( aSaveData );
	iSession.DeleteProfileL( uid() );
	_DBG_FILE("CNSmlDmProvisioningAdapter::DeleteL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::Uid
// -----------------------------------------------------------------------------
TUint32 CNSmlDmProvisioningAdapter::Uid() const
	{
	_DBG_FILE("CNSmlDmProvisioningAdapter::Uid(): begin");
	return KNSmlDmProvisioningAdapterImplUid;
	}

// -----------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::DetailsL( )
// -----------------------------------------------------------------------------
TInt CNSmlDmProvisioningAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/ )
	{
	return KErrNotSupported;
	}

// -------------------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::GetDefaultIAPL()
// Get the default NAPId 
// -------------------------------------------------------------------------------------
TInt CNSmlDmProvisioningAdapter::GetDefaultIAPL()
	{
	TInt iapId = KErrNotFound; 

	CCommsDatabase* database = CCommsDatabase::NewL();
	CleanupStack::PushL( database );

	CCommsDbTableView* tableView = database->OpenTableLC( TPtrC( IAP ) );

	TInt errorCode = tableView->GotoFirstRecord();
		
	if ( errorCode == KErrNone ) 
		{
		TUint32	value;
		tableView->ReadUintL( TPtrC( COMMDB_ID ), value );
		iapId = value;
		}

	CleanupStack::PopAndDestroy( 2 ); // database, tableView

	return iapId;
	}

// ---------------------------------------------------------
// CNSmlDmProvisioningAdapter::GetTitleL
// ---------------------------------------------------------
void CNSmlDmProvisioningAdapter::GetTitleL()
	{
	if( iTitle == 0 )
		{
		RFs	fs;
		User::LeaveIfError( fs.Connect() );
		CleanupClosePushL( fs );

		TFileName fileName;
		TParse parse;
		parse.Set( KNSmlDmPovisioningDirAndResource, &KDC_RESOURCE_FILES_DIR, NULL );
		fileName = parse.FullName();

		RResourceFile resourceFile; 
		BaflUtils::NearestLanguageFile( fs, fileName );
		resourceFile.OpenL( fs, fileName );
		CleanupClosePushL( resourceFile );

		HBufC8* dataBuffer = resourceFile.AllocReadLC( R_SYNCMLDM_PROVISIONING_TITLE );

		TResourceReader reader; 
		reader.SetBuffer( dataBuffer ); 
		iTitle = reader.ReadHBufC16L(); 
		CleanupStack::PopAndDestroy( 3 ); //fs, resourcefile, databuffer
		}
	}

// ------------------------------------------------------------------------------------------------
// CNSmlDmProvisioningAdapter::CombineURILC()
// Combines address and port to URI 
// ------------------------------------------------------------------------------------------------
TInt CNSmlDmProvisioningAdapter::CombineURILC( const TDesC& aAddr, const TDesC& aPort, HBufC*& aRealURI )
	{
	TInt offset = 0;
	TInt i = 0;
	if( aAddr.Find( KNSmlDMProvisioningHTTP ) == 0 )
		{
		offset = KNSmlDMProvisioningHTTP().Length();
		}
	else if( aAddr.Find( KNSmlDMProvisioningHTTPS ) == 0 )
		{
		offset = KNSmlDMProvisioningHTTPS().Length();
		}
		
	// after skipping double slashes seek next single slash
	for( i = offset; i < aAddr.Length(); i++ )
	    {
	    if ( aAddr[i] == KNSmlDMUriSeparator )
	        {
	        break;
	        }
	    }
	
	aRealURI = HBufC::NewLC( aAddr.Length() + aPort.Length() + 1 );
	TPtr uriPtr = aRealURI->Des();

	uriPtr.Copy( aAddr.Ptr(), i );
	uriPtr.Append( KNSmlDMColon );
	uriPtr.Append( aPort );
	uriPtr.Append( aAddr.Right( aAddr.Length() - i ) );
		
	return KErrNone;
	}
// ---------------------------------------------------------
//		CNSmlDmProvisioningAdapter::ConvertTo8LC()
//		Converts string value to 8-bit
// ---------------------------------------------------------
TDesC8& CNSmlDmProvisioningAdapter::ConvertTo8LC( const TDesC& aSource )
	{
	HBufC8* buf = HBufC8::NewLC( aSource.Length()*2 );
	TPtr8 bufPtr = buf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
	}
	
// ---------------------------------------------------------
//		CNSmlDmProvisioningAdapter::ConvertTo8L()
//		Converts string value to 8-bit
// ---------------------------------------------------------
TDesC8& CNSmlDmProvisioningAdapter::ConvertTo8L( const TDesC& aSource )
	{
	HBufC8* buf = HBufC8::NewL( aSource.Length()*2 );
	TPtr8 bufPtr = buf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
	}


//  End of File  
