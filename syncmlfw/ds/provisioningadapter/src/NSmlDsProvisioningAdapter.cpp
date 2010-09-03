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
* Description:  DS Settings Provisioning Adapter
*
*/



// INCLUDE FILES
#include <badesca.h>
#include <commdb.h>
#include <cdbcols.h>			 // CommsDB columname defs
#include <utf.h>
#include <stringresourcereader.h>
#include <barsread.h>
#include <nsmldsprovisioningadapter.rsg>
#include <bautils.h>
#include <barsc.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <centralrepository.h> //For central Repository
#include <nsmloperatordatacrkeys.h> // KCRUidOperatorDatasyncInternalKeys
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmldsconstants.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <CWPEngine.h>
#include "NSmlDsProvisioningAdapter.h"
#include "implementationinformation.h"
#include "NSmlTransportHandler.h"
#include <WPAdapterUtil.h>

_LIT( KInternetString, "INTERNET" );
_LIT( KXVcardMimeType, "text/x-vcard");
const TInt KMaxValueLength = 255;

#include <data_caging_path_literals.hrh>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::CNSmlDsProvisioningAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CNSmlDsProvisioningAdapter::CNSmlDsProvisioningAdapter() :
    CWPAdapter(), iToNapIDInternetIndicator(EFalse)
	{
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CNSmlDsProvisioningAdapter::ConstructL()
	{
	iSession.OpenL();
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CNSmlDsProvisioningAdapter* CNSmlDsProvisioningAdapter::NewL()
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::NewL(): begin");
	CNSmlDsProvisioningAdapter* self = new(ELeave) CNSmlDsProvisioningAdapter; 
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	_DBG_FILE("CNSmlDsProvisioningAdapter::NewL(): end");
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CNSmlDsProvisioningAdapter::~CNSmlDsProvisioningAdapter()
	{
	iProfiles.ResetAndDestroy();
	iProfiles.Close();
	
	delete iTitle;
	iSession.Close();
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CNSmlDataProviderElement::~CNSmlDataProviderElement()
	{
	delete iRemoteDBUri;
	delete iLocalDBUri;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CNSmlDsProfileElement::~CNSmlDsProfileElement()
	{
	iDataProvElement.ResetAndDestroy();
	iDataProvElement.Close();
	iVisitParameter = 0;
	
	delete iHostAddress;
	delete iPort;
	delete iHTTPUserName;
	delete iHTTPPassword;
	
	delete iDisplayName;		
	delete iServerId;
    delete iUsername;
	delete iPassword;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
TBool CNSmlDsProvisioningAdapter::CanSetAsDefault(TInt /*aItem*/) const
	{
	return EFalse;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::SetAsDefaultL
// -----------------------------------------------------------------------------
void CNSmlDsProvisioningAdapter::SetAsDefaultL(TInt /*aItem*/ )
	{
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::ItemCount
// -----------------------------------------------------------------------------
TInt CNSmlDsProvisioningAdapter::ItemCount() const
	{
	return iProfiles.Count();
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::SummaryTitle
// -----------------------------------------------------------------------------
const TDesC16& CNSmlDsProvisioningAdapter::SummaryTitle(TInt /*aIndex*/) const
	{
	return *iTitle;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::SummaryText
// -----------------------------------------------------------------------------
const TDesC16& CNSmlDsProvisioningAdapter::SummaryText(TInt aIndex) const
	{
	return *iProfiles[aIndex]->iDisplayName;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::SaveL
// -----------------------------------------------------------------------------
void CNSmlDsProvisioningAdapter::SaveL(TInt aItem)
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::SaveL(): begin");
	TPckgBuf<TUint32> uid;
	
	RSyncMLDataSyncProfile profile;
	TBool ret = EFalse;
	
	if(iProfiles[aItem]->iServerId != NULL)
	{
	RSyncMLDataSyncProfile ProfileToSearch;
	RArray<TSmlProfileId> arr;
    iSession.ListProfilesL( arr, ESmlDataSync );
    
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
		}
	else
		{
	profile.CreateL( iSession );
		}
	CleanupClosePushL( profile );	
	}
	else
	{
		
		profile.CreateL( iSession );
		CleanupClosePushL( profile );	
	}
	
	// ui needs this to be set 0
	profile.SetCreatorId(0);//iProfiles[aItem]->iProfile.SetCreatorId(0);
	profile.SetSanUserInteractionL( iProfiles[aItem]->iServerAlertedAction );
	
	if ( iProfiles[aItem]->iDisplayName )
	    {
	    profile.SetDisplayNameL( iProfiles[aItem]->iDisplayName->Des() );    
	    }
	
	if ( iProfiles[aItem]->iServerId )
	    {
	    profile.SetServerIdL( iProfiles[aItem]->iServerId->Des() );    
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
		
    if ( (iProfiles[aItem]->iVisitParameter
            && iProfiles[aItem]->iVisitParameter->Data().Length() == uid.MaxLength() )
            || iToNapIDInternetIndicator)
		{
        TUint apId = 0;
        TInt ERROR = KErrNone;
        TInt err1 = KErrNone;

        if (iToNapIDInternetIndicator)
            {
            //Get Access Point from DB or SNAP
            TRAP(err1, apId = WPAdapterUtil::GetAPIDL());
            }

        else
            {
            uid.Copy(iProfiles[aItem]->iVisitParameter->Data() );

            RCmManagerExt  cmmanagerExt;
		    cmmanagerExt.OpenL();
		    CleanupClosePushL(cmmanagerExt);
		    RCmConnectionMethodExt cm;
		    cm = cmmanagerExt.ConnectionMethodL( uid());
		    CleanupClosePushL( cm );

            TRAP( ERROR, apId = cm.GetIntAttributeL(CMManager::ECmIapId) );
            CleanupStack::PopAndDestroy(2); //cmmanager,cm
            }

        //Get default access point in failure of getting AP
        if (ERROR != KErrNone || err1 != KErrNone)
			{
			apId = GetDefaultIAPL();
			}
		
		HBufC8* iapBuf = HBufC8::NewLC( 8 );
		TPtr8 ptrBuf = iapBuf->Des();
		ptrBuf.Num( apId );
		
		connection.SetPropertyL( KNSmlIAPId, *iapBuf );
			
        CleanupStack::PopAndDestroy(); //iapBuf	
		}
		
	if( iProfiles[aItem]->iHostAddress )
		{
		// see if address contains also port
		TBool portFound = EFalse;
		TInt startPos(0);
		if(iProfiles[aItem]->iHostAddress->Find(KNSmlDsProvisioningHTTP)==0)
		    {
		    startPos=KNSmlDsProvisioningHTTP().Length();		    		    
		    }
		else if(iProfiles[aItem]->iHostAddress->Find(KNSmlDsProvisioningHTTPS)==0)
		    {
		    startPos=KNSmlDsProvisioningHTTPS().Length();		    		    
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
							  KNSmlDsDefaultPort(), uri ) == KErrNone )
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
	CleanupStack::PopAndDestroy(); // connection
	
	
		
	
	
	if(iProfiles[aItem]->iProtocolVersion == ESmlVersion1_1_2 || 
			iProfiles[aItem]->iProtocolVersion == ESmlVersion1_2 )
	{
		profile.SetProtocolVersionL(iProfiles[aItem]->iProtocolVersion);
	}

	
	
	
	
		
	TInt iDataProvElementCount = iProfiles[aItem]->iDataProvElement.Count();
	_DBG_FILE("CNSmlDsProvisioningAdapter::SaveL(): Resource For loop: before Start");
	for ( TInt i = 0; i < iDataProvElementCount; i++ )
		{
        _DBG_FILE(
                "CNSmlDsProvisioningAdapter::SaveL(): Resource For loop: In");
		RSyncMLDataProvider dataProvider;

        TRAPD(error, dataProvider.OpenL(iSession,
                iProfiles[aItem]->iDataProvElement[i]->iUid));
		if (!error)
		    {
		    CleanupClosePushL(dataProvider);
            if (ret)
                {
                RArray<TSmlTaskId> providers;
                profile.ListTasksL(providers);
                TInt dataprovcount = providers.Count();

                for (TInt i = 0; i < dataprovcount; i++)
                    {
                    TSmlTaskId taskID = providers[i];
                    RSyncMLTask task;
                    task.OpenL(profile, taskID);
                    CleanupClosePushL(task);

                    if (dataProvider.Identifier() == task.DataProvider())
                        {
                        profile.DeleteTaskL(taskID);
                        CleanupStack::PopAndDestroy();
                        break;
                        }

                    CleanupStack::PopAndDestroy();

                    }
                providers.Reset();
                providers.Close();

                }
            HBufC* localDB = dataProvider.DefaultDataStoreName().AllocLC();
            _DBG_FILE("CNSmlDsProvisioningAdapter::SaveL(): DB value");
            DBG_ARGS(
                    iProfiles[aItem]->iDataProvElement[i]->iRemoteDBUri->Des().Ptr());

            RSyncMLTask task;
            task.CreateL(
                    profile,
                    iProfiles[aItem]->iDataProvElement[i]->iUid,
                    iProfiles[aItem]->iDataProvElement[i]->iRemoteDBUri->Des(),
                    localDB->Des());
            CleanupClosePushL(task);
            TRAPD(err, task.UpdateL());
            if (err != KErrAlreadyExists && err != KErrNone)
                {
                User::Leave(err);
                }
            CleanupStack::PopAndDestroy(3); // task, localDB, dataProvider
		    }
		
		}
	_DBG_FILE("CNSmlDsProvisioningAdapter::SaveL(): Resource For loop: after End");
				
	profile.UpdateL();
	iProfiles[aItem]->iProfileId = profile.Identifier();
	CleanupStack::PopAndDestroy( &profile );
	_DBG_FILE("CNSmlDsProvisioningAdapter::SaveL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::Reserved_1
// -----------------------------------------------------------------------------
TInt CNSmlDsProvisioningAdapter::Reserved_1()
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::Reserved_2
// -----------------------------------------------------------------------------
TInt CNSmlDsProvisioningAdapter::Reserved_2()
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::VisitL
// -----------------------------------------------------------------------------
void CNSmlDsProvisioningAdapter::VisitL( CWPCharacteristic& aCharacteristic )
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPCharacteristic): begin");
	if(aCharacteristic.Type() == KWPApplication)
		{
		iState = CNSmlDsProvisioningAdapter::EStateApplication;
		}
	else if(aCharacteristic.Type() == KWPAppAuth)
		{
		iAuthLevel = CNSmlDsProvisioningAdapter::ENone;
		if(iState == CNSmlDsProvisioningAdapter::EStateDsSettings)
			{
			iState = CNSmlDsProvisioningAdapter::EStateAppAuth;
			}
		else
			{
			iState = CNSmlDsProvisioningAdapter::EStateNull;
			}
		}
	else if(aCharacteristic.Type() == KWPResource)
		{
		if(iState != CNSmlDsProvisioningAdapter::EStateNull )
			{
			iState = CNSmlDsProvisioningAdapter::EStateResource;
			}
		else
			{
			iState = CNSmlDsProvisioningAdapter::EStateNull;
			}
		}
	else if(aCharacteristic.Type() == KWPAppAddr || aCharacteristic.Type() == KWPPort)
		{
		if( iState != CNSmlDsProvisioningAdapter::EStateNull )
			{
			iState = CNSmlDsProvisioningAdapter::EStateDsSettings;
			}
		}
	else
		{
		iState = CNSmlDsProvisioningAdapter::EStateNull;
		}

	if(iState != CNSmlDsProvisioningAdapter::EStateNull)
		{
		aCharacteristic.AcceptL( *this );
		}
	_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPCharacteristic): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::VisitL
// -----------------------------------------------------------------------------
void CNSmlDsProvisioningAdapter::VisitL( CWPParameter& aParameter)
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): begin");
	TInt num;
	TInt flag =0;
	
	
	if( iState == CNSmlDsProvisioningAdapter::EStateApplication )
		{
		if( aParameter.ID() == EWPParameterAppID && aParameter.Value().Compare( KNSmlDsProvisioningDMAppIdVal ) == 0 )
			{
			iState = CNSmlDsProvisioningAdapter::EStateDsSettings;

			GetTitleL();

			CNSmlDsProfileElement* dsProfileElement = new(ELeave) CNSmlDsProfileElement;
			CleanupStack::PushL( dsProfileElement );
			
			dsProfileElement->iDisplayName = KNSmlDsProvisioningDefName().AllocL();			
			dsProfileElement->iServerAlertedAction = ESmlConfirmSync;			

			dsProfileElement->iVisitParameter = 0;

			iProfiles.AppendL( dsProfileElement );	
			
			iProfiles[iProfiles.Count()-1]->iProtocolVersion = ESmlVersion1_2;
			CleanupStack::Pop( dsProfileElement );
			}
		else
			{
			iState = CNSmlDsProvisioningAdapter::EStateNull;
			}
		}
	else if( iState == CNSmlDsProvisioningAdapter::EStateDsSettings )
		{
		switch ( aParameter.ID() )
			{
			case EWPParameterName:
			case EWPParameterName1:
			{
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): Name");
				DBG_ARGS( aParameter.Value().Ptr() );
				
				if ( iProfiles[iProfiles.Count()-1]->iDisplayName )
				    {
				    delete iProfiles[iProfiles.Count()-1]->iDisplayName;
				    iProfiles[iProfiles.Count()-1]->iDisplayName = NULL;
				    }
					/*parameter exceeding 50 characters truncate to 50 and save the item*/
					if(aParameter.Value().Length()>50)
					{
						TPtrC ptr2 = aParameter.Value().Left(50);
						iProfiles[iProfiles.Count()-1]->iDisplayName = ptr2.AllocL();	
					}
					else
				iProfiles[iProfiles.Count()-1]->iDisplayName = aParameter.Value().AllocL();				
					
	    			
					/* parameter is NULL set to default value*/	    		
	    			num = iProfiles[iProfiles.Count()-1]->iDisplayName->Length();
	    			
					
					TInt i;
					/*check for white spaces, If all characters are white spaces set to default*/
					TPtr temp = iProfiles[iProfiles.Count()-1]->iDisplayName->Des();
					for(i=0; i<iProfiles[iProfiles.Count()-1]->iDisplayName->Length(); i++)
	    			{
	    				
	    				
	    				if(temp[i] != ' ')
	    				break;
	    			}
	    			
	    			if(i == num || num == 0 )
	    			  flag =1;
	    			
	    			if(flag)
	    			{
	    				if ( iProfiles[iProfiles.Count()-1]->iDisplayName )
				    	{
				    	delete iProfiles[iProfiles.Count()-1]->iDisplayName;
				    	iProfiles[iProfiles.Count()-1]->iDisplayName = NULL;
				    	}
				    	iProfiles[iProfiles.Count()-1]->iDisplayName = KNSmlDsProvisioningDefName().AllocL();
	    			}
			}
				break;
			
			case EWPParameterToProxy:	
				_DBG_FILE("CNSmlDmProvisioningAdapter::VisitL(CWPParameter): ToProxy");
			case EWPParameterToNapID:
			case EWPParameterToNapID1:
                        {
                        TPtrC ptrToNapID(aParameter.Value());
                        if(ptrToNapID.CompareF(KInternetString)==0)
                        iToNapIDInternetIndicator = ETrue;
                        }
				break;

			case EWPParameterPortNbr:
			case EWPParameterPortNbr1:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): PortNbr");
				DBG_ARGS( aParameter.Value().Ptr() );
				iProfiles[iProfiles.Count()-1]->iPort = aParameter.Value().AllocL();
				break;

			case EWPParameterAddr:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): Addr");
				DBG_ARGS( aParameter.Value().Ptr() );
				iProfiles[iProfiles.Count()-1]->iHostAddress = aParameter.Value().AllocL();
				break;

			case EWPParameterProviderID:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): ProviderId");
				DBG_ARGS( aParameter.Value().Ptr() );
				if(aParameter.Value().Length()<=150)
				{
				iProfiles[iProfiles.Count()-1]->iServerId = ConvertTo8LC( aParameter.Value() ).AllocL();				
				CleanupStack::PopAndDestroy(); // ConvertTo8LC
			  }
				break;
				
			case EWPParameterAProtocol:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): AProtocol:Version");
				DBG_ARGS( aParameter.Value().Ptr() );
				//TPtrC ptr(aParameter.Value());
				if(aParameter.Value().Compare(KNSmlVersion112)==0)
				{
					iProfiles[iProfiles.Count()-1]->iProtocolVersion = ESmlVersion1_1_2;	
				}
				
				else 
				if(aParameter.Value().Compare(KNSmlVersion11)==0)
				{
					iProfiles[iProfiles.Count()-1]->iProtocolVersion = ESmlVersion1_1_2;	
				}
				
				else
				{
					iProfiles[iProfiles.Count()-1]->iProtocolVersion = ESmlVersion1_2;	
				}
				
				
				
				break;
			default:
				break;
			}
		
		}
	else if( iState == CNSmlDsProvisioningAdapter::EStateAppAuth )
		{
		switch ( aParameter.ID() )
			{
			case EWPParameterAAuthType:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): AuthType");
				DBG_ARGS( aParameter.Value().Ptr() );
				if (( aParameter.Value().Compare( KNSmlDSProvisioningHTTPBasic ) == 0 ) ||
					( aParameter.Value().Compare( KNSmlDSProvisioningHTTPDigest ) == 0 ))
					{
					_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): HTTP");
					iAuthLevel = CNSmlDsProvisioningAdapter::EHttp;
					} 
				break;

			case EWPParameterAAuthName:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): AuthName");
				DBG_ARGS( aParameter.Value().Ptr() );
				if(aParameter.Value().Length()<=80)
				{
				if ( iAuthLevel == CNSmlDsProvisioningAdapter::EHttp )
					{
					iProfiles[iProfiles.Count()-1]->iHTTPUserName = ConvertTo8LC( aParameter.Value() ).AllocL();
					CleanupStack::PopAndDestroy(); // ConvertTo8LC
					}
				else
					{
					iProfiles[iProfiles.Count()-1]->iUsername = ConvertTo8LC( aParameter.Value() ).AllocL();					
					CleanupStack::PopAndDestroy(); // ConvertTo8LC
					}
				}
				break;

			case EWPParameterAAuthSecret:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): AuthSecret");
				DBG_ARGS(aParameter.Value().Ptr());
				if(aParameter.Value().Length()<=22)
				{
					if ( iAuthLevel == CNSmlDsProvisioningAdapter::EHttp )
					{
					iProfiles[iProfiles.Count()-1]->iHTTPPassword = ConvertTo8LC( aParameter.Value() ).AllocL();
					CleanupStack::PopAndDestroy(); // ConvertTo8LC
					}
				else
					{
					iProfiles[iProfiles.Count()-1]->iPassword = ConvertTo8LC( aParameter.Value() ).AllocL();					
					CleanupStack::PopAndDestroy(); // ConvertTo8LC
					}
				iState = CNSmlDsProvisioningAdapter::EStateDsSettings;
			 }
				break;
			default:
				break;
			}
		}

	else if( iState == CNSmlDsProvisioningAdapter::EStateResource )
		{
		TInt iDataProvElementCount = iProfiles[iProfiles.Count()-1]->iDataProvElement.Count(); 
		if( iDataProvElementCount == 0 )
			{
			CNSmlDataProviderElement* dataProviderElement = new(ELeave) CNSmlDataProviderElement;
			CleanupStack::PushL( dataProviderElement );
			iProfiles[iProfiles.Count()-1]->iDataProvElement.AppendL( dataProviderElement );
			CleanupStack::Pop( dataProviderElement );
			}
		else if ( (iProfiles[iProfiles.Count()-1]->iDataProvElement[iDataProvElementCount-1]->iRemoteDBUri != 0) &&
			 	  (iProfiles[iProfiles.Count()-1]->iDataProvElement[iDataProvElementCount-1]->iUid != 0) &&
			 	  ( ( aParameter.ID() == EWPParameterURI ) || ( aParameter.ID() == EWPParameterAAccept ) ) )
		{
			// another resource element recognized
			CNSmlDataProviderElement* dataProviderElement = new(ELeave) CNSmlDataProviderElement;
			CleanupStack::PushL( dataProviderElement );
			iProfiles[iProfiles.Count()-1]->iDataProvElement.AppendL( dataProviderElement );
			CleanupStack::Pop( dataProviderElement );
		}
		
		switch ( aParameter.ID() )
			{
			case EWPParameterName:
			case EWPParameterName1:
				break;

			case EWPParameterURI:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): RemoteDBURI");
				DBG_ARGS( aParameter.Value().Ptr() );
				if(aParameter.Value().Length() <= 125)
				{
				iProfiles[iProfiles.Count()-1]->iDataProvElement[iProfiles[iProfiles.Count()-1]->iDataProvElement.Count()-1]->iRemoteDBUri =
						aParameter.Value().AllocL();
				}
				break;

			case EWPParameterAAccept:
				_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): AAccept (mime)");
				StoreAttributesL( aParameter.Value() );
				iState = CNSmlDsProvisioningAdapter::EStateDsSettings;
				break;
			default:
				break;
			}
		}
		
	_DBG_FILE("CNSmlDsProvisioningAdapter::VisitL(CWPParameter): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::VisitLinkL
// -----------------------------------------------------------------------------
void CNSmlDsProvisioningAdapter::VisitLinkL( CWPCharacteristic& aParameter)
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::VisitLinkL(): begin");
	
	if( ( ( aParameter.Type()==KWPNapDef ) || ( aParameter.Type()==KWPPxLogical ) ) &&
		( iState != CNSmlDsProvisioningAdapter::EStateNull ) )
		{
		if( iProfiles.Count() > 0 )
			{
			iProfiles[iProfiles.Count()-1]->iVisitParameter = &aParameter;
			}
		}
	_DBG_FILE("CNSmlDsProvisioningAdapter::VisitLinkL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::ContextExtension
// -----------------------------------------------------------------------------
TInt CNSmlDsProvisioningAdapter::ContextExtension( MWPContextExtension*& aContextExtension )
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::ContextExtension(): begin");
	aContextExtension = this;
	_DBG_FILE("CNSmlDsProvisioningAdapter::ContextExtension(): end");
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::SaveDataL
// -----------------------------------------------------------------------------
const TDesC8& CNSmlDsProvisioningAdapter::SaveDataL( TInt aIndex ) const
	{
	return iProfiles[aIndex]->iProfileId;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::DeleteL
// -----------------------------------------------------------------------------
void CNSmlDsProvisioningAdapter::DeleteL( const TDesC8& aSaveData)
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::DeleteL(): begin");
	TPckgBuf<TInt> uid;
	uid.Copy( aSaveData );
	iSession.DeleteProfileL( uid() );
	_DBG_FILE("CNSmlDsProvisioningAdapter::DeleteL(): end");
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::Uid
// -----------------------------------------------------------------------------
TUint32 CNSmlDsProvisioningAdapter::Uid() const
	{
	_DBG_FILE("CNSmlDsProvisioningAdapter::Uid(): begin");
	return KNSmlDsProvisioningAdapterImplUid;
	}

// -----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::DetailsL( )
// -----------------------------------------------------------------------------
TInt CNSmlDsProvisioningAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor*/ )
	{
	return KErrNotSupported;
	}

// -------------------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::GetDefaultIAPL()
// Gets the default NAPId 
// -------------------------------------------------------------------------------------
TInt CNSmlDsProvisioningAdapter::GetDefaultIAPL()
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

// ------------------------------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::CombineURILC()
// Combines address and port to URI 
// ------------------------------------------------------------------------------------------------
TInt CNSmlDsProvisioningAdapter::CombineURILC( const TDesC& aAddr, const TDesC& aPort, HBufC*& aRealURI )
	{
	TInt offset = 0;
	TInt i = 0;
	if( aAddr.Find( KNSmlDsProvisioningHTTP ) == 0 )
		{
		offset = KNSmlDsProvisioningHTTP().Length();
		}
	else if( aAddr.Find( KNSmlDsProvisioningHTTPS ) == 0 )
		{
		offset = KNSmlDsProvisioningHTTPS().Length();
		}
		
	// after skipping double slashes seek next single slash
	for( i = offset; i < aAddr.Length(); i++ )
		{
		if( aAddr[i] == KNSmlDMUriSeparator )
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
// CNSmlDsProvisioningAdapter::StoreAttributesL
// ---------------------------------------------------------
void CNSmlDsProvisioningAdapter::StoreAttributesL( const TDesC& aType )
    {
    _DBG_FILE("CNSmlDsProvisioningAdapter::StoreAttributesL(): begin");
    
    TInt iDataProvElementCount = iProfiles[iProfiles.Count()-1]->iDataProvElement.Count()-1;
	// Leave if aType cannot be assigned
    if( ( aType.Length() > 0 ) &&
        ( iProfiles[iProfiles.Count()-1]->iDataProvElement[iDataProvElementCount]->iRemoteDBUri ) )
        {
        TBool dataProvIdFoundInZ = FALSE;
        TSmlDataProviderId firstDataProvIdFound = 0;
        TSmlDataProviderId uidFound = 0;

        TBool doSearch = ETrue;
        if ( aType.FindF( KXVcardMimeType ) != KErrNotFound )
            {
            if ( IsOperatorProfile( *iProfiles[iProfiles.Count()-1] ) )
                {
                const CNSmlDsProfileElement& profile = *iProfiles[iProfiles.Count()-1];
                StoreOperatorUrlL( *profile.iHostAddress );
                
                // Do not make a search through adapter implementations
                doSearch = EFalse;
                uidFound = OperatorAdapterUid();
                if ( !uidFound )
                    {
                    // If OperatorAdapterUid returns 0, do a search
                    doSearch = ETrue;
                    }
                }
            }
		// look through every implementation adapter until one found
		// which supports MIME type in question
		
		// The first one located in ROM is chosen. If none found in ROM then
		// the first adapter found is chosen.

		HBufC8 *type = HBufC8::NewLC(aType.Size());
		TPtr8 typePtr = type->Des();
		CnvUtfConverter::ConvertFromUnicodeToUtf8( typePtr, aType);

		// get list of dataproviderIds
		RImplInfoPtrArray implArray;
		CleanupStack::PushL( PtrArrCleanupItemRArr( CImplementationInformation, &implArray ) );
		TUid ifUid = { KNSmlDSInterfaceUid };
		REComSession::ListImplementationsL( ifUid, implArray );
		
        if ( doSearch )
            {
            TInt countProviders = implArray.Count();
            for( TInt i = 0; i < countProviders; i++ )
                {
                CImplementationInformation* implInfo = implArray[i];

                RSyncMLDataProvider dataProvider;
                dataProvider.OpenL( iSession, implInfo->ImplementationUid().iUid );
                CleanupClosePushL( dataProvider );

                TInt mimeTypeCount = dataProvider.MimeTypeCount();
                for( TInt j = 0; j < mimeTypeCount; j++ )
                    {
                    HBufC* mimeType = dataProvider.MimeType( j ).AllocLC();
                    TPtrC8 convMimeType = ConvertTo8LC( *mimeType );
                    if( typePtr.Find( convMimeType ) == 0)
                        {
                        // MIME type in question was found
                        uidFound = implInfo->ImplementationUid().iUid;

                        if( firstDataProvIdFound == 0 )
                            {
                            // save the first in case of none found from ROM
                            firstDataProvIdFound = uidFound;
                            }
					
                        // check whether the provider is located in ROM (drive Z)
                        if( implInfo->Drive() == EDriveZ )
                            {
                            dataProvIdFoundInZ = TRUE;
                            }
                        }
				
                    CleanupStack::PopAndDestroy(2); // mimetype, ConvertTo8LC

                    if( uidFound )
                        {
                        break;
                        }
                    }
				
                CleanupStack::PopAndDestroy(); // dataProvider
			
                if ( dataProvIdFoundInZ )
                    {
                    break;
                    }
                else
                    {
                    uidFound = firstDataProvIdFound;
                    }
                }
            }
        
		REComSession::FinalClose();
		CleanupStack::PopAndDestroy( 2 ); // type, implArray

		if( uidFound )
			{
			iProfiles[iProfiles.Count()-1]->iDataProvElement[iDataProvElementCount]->iUid = uidFound;
			}		
        }
	
	_DBG_FILE("CNSmlDsProvisioningAdapter::StoreAttributesL(): end");
	}

// ---------------------------------------------------------
// CNSmlDsProvisioningAdapter::GetTitleL
// ---------------------------------------------------------
void CNSmlDsProvisioningAdapter::GetTitleL()
	{
	if( iTitle == 0 )
		{
		RFs	fs;
		User::LeaveIfError( fs.Connect() );
		CleanupClosePushL( fs );

		TFileName fileName;
		TParse parse;
		parse.Set( KNSmlDsPovisioningDirAndResource, &KDC_RESOURCE_FILES_DIR, NULL );
		fileName = parse.FullName();

		RResourceFile resourceFile;
		BaflUtils::NearestLanguageFile( fs, fileName );
		resourceFile.OpenL( fs, fileName );
		CleanupClosePushL( resourceFile );

		HBufC8* dataBuffer = resourceFile.AllocReadLC( R_SYNC_PROVISIONING_TITLE );
			
		TResourceReader reader;
		reader.SetBuffer( dataBuffer ); 
		iTitle = reader.ReadHBufC16L(); 
		CleanupStack::PopAndDestroy( 3 ); //fs, resourcefile, databuffer
		}
	}

// ---------------------------------------------------------
//		CNSmlDsProvisioningAdapter::ConvertTo8LC()
//		Converts string value to 8-bit
// ---------------------------------------------------------
TDesC8& CNSmlDsProvisioningAdapter::ConvertTo8LC( const TDesC& aSource )
	{
	HBufC8* buf = HBufC8::NewLC( aSource.Length()*2 );
	TPtr8 bufPtr = buf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
	}

// ---------------------------------------------------------
//		CNSmlDsProvisioningAdapter::ConvertTo8L()
//		Converts string value to 8-bit
// ---------------------------------------------------------
TDesC8& CNSmlDsProvisioningAdapter::ConvertTo8L( const TDesC& aSource )
	{
	HBufC8* buf = HBufC8::NewL( aSource.Length()*2 );
	TPtr8 bufPtr = buf->Des();
	CnvUtfConverter::ConvertFromUnicodeToUtf8( bufPtr, aSource );

    return *buf;
	}

//-----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::IsOperatorProfile
// 
//-----------------------------------------------------------------------------
//
TBool CNSmlDsProvisioningAdapter::IsOperatorProfile( const CNSmlDsProfileElement& aProfile )
    {
    TBuf8<KMaxValueLength> value;
    CRepository* rep = NULL;
    TRAPD ( err, rep = CRepository::NewL( KCRUidOperatorDatasyncInternalKeys ) );
    if ( err == KErrNone )
        {
        rep->Get( KNsmlOpDsOperatorSyncServerId, value );
        delete rep;
        }
    
    if ( aProfile.iServerId )
    	{
        if ( value.Compare( *aProfile.iServerId ) == 0 )
            {
            return ETrue;
            }
    	}
    return EFalse;
    }

//-----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::OperatorAdapterUid
// 
//-----------------------------------------------------------------------------
//
TInt CNSmlDsProvisioningAdapter::OperatorAdapterUid()
    {
    TInt value = 0;
    CRepository* rep = NULL;
    TRAPD ( err, rep = CRepository::NewL( KCRUidOperatorDatasyncInternalKeys ) );
    if ( err == KErrNone )
        {
        rep->Get( KNsmlOpDsOperatorAdapterUid, value );
        delete rep;
        }
    return value;
    }

//-----------------------------------------------------------------------------
// CNSmlDsProvisioningAdapter::StoreOperatorUrlL
// 
//-----------------------------------------------------------------------------
//
void CNSmlDsProvisioningAdapter::StoreOperatorUrlL( const TDesC& aUrl )
	{
	CRepository* rep = NULL;
	TRAPD ( err, rep = CRepository::NewL( KCRUidOperatorDatasyncInternalKeys ) );
	if ( err == KErrNone )
		{
		CleanupStack::PushL( rep );
		User::LeaveIfError( rep->Set( KNsmlOpDsOperatorSyncServerURL, aUrl ));
		CleanupStack::PopAndDestroy( rep );
		}
	}

//  End of File  
