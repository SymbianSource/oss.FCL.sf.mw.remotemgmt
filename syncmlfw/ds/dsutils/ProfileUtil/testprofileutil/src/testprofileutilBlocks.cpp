/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "testprofileutil.h"
//SyncML headers
#include <SyncMLDef.h>
#include <SyncMLClient.h>
#include <SyncMLClientDS.h>
#include <SyncMLClientDM.h>
#include <SyncMLTransportProperties.h>
#include <commdb.h> 
#include "nsmlconstants.h"

#include "NsmlProfileUtil.h"




// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;
const TInt TPropertyIntenetAccessPoint = 0;
const TSmlServerAlertedAction 	KDSProfileSAN 		= ESmlConfirmSync;
const TSmlProtocolVersion 		KDSProfileProtocol 	= ESmlVersion1_1_2;
const TInt 						KDSProfileCreatorId = 20;
const TInt						KTaskCreatorId		= 270495197;
const TSmlSyncType				KTaskSyncType		= ESmlSlowSync;
const TBool						KTaskEnabled		= ETrue;

const TInt KBufSize32 = 32;

//for Intelli Sync server details
_LIT (KDSRemoteProfileDisplayName, "Intellisync");
_LIT8(KDSProfileUserName, 		"nokia6" );
_LIT8(KDSProfilePassword, 		"ashwini" );
_LIT8(KDSProfileServerId, 		"None" );
_LIT8(KDSProfileServerURL, 	"http://82.77.123.84/services/syncml:80" );
_LIT8(KDSAccessPoint, 		        "Sonera GPRS" );
_LIT(KTaskServerDataSource, 		"./Contact/Unfiled" );
_LIT(KTaskClientContactDataSource,"./C:Contacts.cdb");
_LIT(KTaskDisplayName,			"Contacts" );

_LIT( KNetMon,"\\netmon.exe" );



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Ctestprofileutil::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctestprofileutil::Delete() 
    {
           iProfiles.Close();
          
    }

// -----------------------------------------------------------------------------
// Ctestprofileutil::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctestprofileutil::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "OpenSyncSession",Ctestprofileutil::OpenSyncSessionL ),
        ENTRY( "CreateDSProfile",Ctestprofileutil::CreateDSProfileL ),
        ENTRY( "SetDSRemoteProfileData",Ctestprofileutil::SetRemoteDSProfileDataL),
        ENTRY( "UpdateDSProfile",Ctestprofileutil::UpdateDSProfileL ),
        ENTRY( "CloseSyncSession",Ctestprofileutil::CloseSyncSession ),
        ENTRY( "OpenDSProfileById",Ctestprofileutil::OpenDSProfileByIdL ),
        ENTRY( "CreateConnection",Ctestprofileutil::CreateConnectionL ),
        ENTRY( "UpdateConnection",Ctestprofileutil::UpdateConnectionL ),
        ENTRY( "OpenConnection",Ctestprofileutil::OpenConnectionL ),
        ENTRY( "CreateTask",Ctestprofileutil::CreateTaskL),
        ENTRY( "SetTaskData",Ctestprofileutil::SetTaskDataL ),
        ENTRY( "UpdateTask",Ctestprofileutil::UpdateTaskL ),
        ENTRY( "CloseTask",Ctestprofileutil::CloseTask ),
        ENTRY( "CloseDSProfile",Ctestprofileutil::CloseDSProfile ),
        ENTRY( "CloseConnection",Ctestprofileutil::CloseConnection ),
        ENTRY( "WriteAndReadCenrepKeys",Ctestprofileutil::ReadAndWriteCenrepKeysL),
        ENTRY( "ProfileUtilCheck",Ctestprofileutil::ReadSettingsDBL),
        ENTRY( "GoingtoOffline",Ctestprofileutil::OffLineL),
        ENTRY( "GoingtoGeneral",Ctestprofileutil::GeneralL),
        ENTRY( "StartSync",Ctestprofileutil::SyncL),        
        ENTRY( "LaunchAutoRestart",Ctestprofileutil::LaunchAutoRestartL),
        ENTRY( "CallDelay",Ctestprofileutil::CallDelay),
        ENTRY( "Example", Ctestprofileutil::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctestprofileutil::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctestprofileutil::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktestprofileutil, "testprofileutil" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Ktestprofileutil, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Ktestprofileutil, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
//------------------------------------------------------------------------------
//Ctestprofileutil::OpenSyncSessionL
//opens the syncml session
//-------------------------------------------------------------------------------
//
TInt Ctestprofileutil::OpenSyncSessionL( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.OpenL();
	iSyncSession.ShareAuto(); // share automatically with other threads (notifiers)
	return KErrNone;
	}
	
//------------------------------------------------------------------------------
//Ctestprofileutil::UpdateDSProfileL
//updates the ds profile with new values
//-------------------------------------------------------------------------------
//	
TInt Ctestprofileutil::UpdateDSProfileL( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.UpdateL();
	iProfileId = iDSProfile.Identifier();
	return KErrNone;
	}
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateDSProfileL
//creates the Ds profile
//-------------------------------------------------------------------------------
//	
TInt Ctestprofileutil::CreateDSProfileL( CStifItemParser& aItem )
	{
	TInt protocol = 0;
	aItem.GetNextInt( protocol );
	
	iDSProfile.CreateL( iSyncSession );
	iDSProfile.SetProtocolVersionL((TSmlProtocolVersion) protocol );
	return KErrNone;
	}
	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::CreateDSProfileL
//sets the values for the remote ds profile
//-------------------------------------------------------------------------------
//	
TInt Ctestprofileutil::SetRemoteDSProfileDataL( CStifItemParser& aItem )
	{
	 TInt nextId=0;
	 aItem.GetNextInt(nextId);
	 if(nextId==2)
	 {
		iDSProfile.SetCreatorId( KDSProfileCreatorId);
		iDSProfile.SetDisplayNameL( KDSRemoteProfileDisplayName);
		iDSProfile.SetUserNameL( KDSProfileUserName);
		iDSProfile.SetPasswordL(  KDSProfilePassword);
    	iDSProfile.SetServerIdL( KDSProfileServerId);
		iDSProfile.SetSanUserInteractionL( KDSProfileSAN );
		iDSProfile.SetProtocolVersionL( KDSProfileProtocol );
	 }

	
	return KErrNone;
	}

//------------------------------------------------------------------------------
//Ctestprofileutil::ListProfilesL
//list all the profiles and stores the profil-id in profiles list
//-------------------------------------------------------------------------------
//		
void Ctestprofileutil::ListProfilesL( TSmlUsageType type )
	{
	TRAPD( err, iSyncSession.ListProfilesL( iProfiles, type ) );
	if ( err == KErrNotSupported ) // ds sync not supported, use dummy values
		{
		iProfiles.Append( 1 );
		}
	}
	

//------------------------------------------------------------------------------
//Ctestprofileutil::OpenDSProfileByIdL
//opens the connection
//-------------------------------------------------------------------------------
//
TInt Ctestprofileutil::OpenDSProfileByIdL( CStifItemParser& aItem )
	{
	TInt id = -2;
	aItem.GetNextInt( id );
	ListProfilesL( ESmlDataSync );
	if ( id == -2 )
		{
		iDSProfile.OpenL( iSyncSession, iProfileId, ESmlOpenReadWrite );
		}
	else if (id==-1)
	    {
	     TSmlProfileId profileId;
	     TInt i;
	     for (i=0;i<iProfiles.Count() ;++i)
	     {
	       	
	       profileId=OpenDSProfileByNameL(iProfiles[i]);
	      if(profileId!=KErrNone)
	       {
	      	iDSProfile.OpenL( iSyncSession, profileId, ESmlOpenReadWrite );
	      	break;
	       }
		    
	     	
	      }
	     
		}
	else
		{
		iDSProfile.OpenL( iSyncSession, id, ESmlOpenReadWrite );
		}
	
	return KErrNone;
	}


//------------------------------------------------------------------------------
//converts the string
//-------------------------------------------------------------------------------
//    
    
 HBufC* StrConversion(const TDesC& aDefaultText)
{
	return aDefaultText.AllocLC();
}


//------------------------------------------------------------------------------
//copies the string
//-------------------------------------------------------------------------------
//
void StrCopy(TDes8& aTarget, const TDesC& aSource)
    {
	TInt len = aTarget.MaxLength();
    if(len < aSource.Length()) 
	    {
		aTarget.Copy(aSource.Left(len));
		return;
	    }
	aTarget.Copy(aSource);
    }
	
	
void StrCopy(TDes& aTarget, const TDesC& aSource)
    {
	TInt len = aTarget.MaxLength();
    if(len < aSource.Length()) 
	    {
		aTarget.Copy(aSource.Left(len));
		return;
	    }
	aTarget.Copy(aSource);
    }

//------------------------------------------------------------------------------
//Ctestprofileutil::OpenDSProfileByNameL
//opens the profile based on the id 
//-------------------------------------------------------------------------------
//	
TSmlProfileId Ctestprofileutil::OpenDSProfileByNameL( TSmlProfileId aProfileId/*,TDesC& aProfilename */)
	{
	RSyncMLDataSyncProfile prof;
	prof.OpenL( iSyncSession, aProfileId, ESmlOpenReadWrite );
    HBufC* hBuf=StrConversion(prof.DisplayName());
    TPtrC ptr=hBuf->Des();
		   	  if(ptr.Compare(KDSRemoteProfileDisplayName) == 0	)
		   	  {     
		   	  		prof.Close();
		   	  		CleanupStack::PopAndDestroy(hBuf);
		   	  		return(aProfileId);
		   	  }
	prof.Close();
	CleanupStack::PopAndDestroy(hBuf);
	return KErrNone;
	}



//------------------------------------------------------------------------------
//gets the Id corresponding to the Access point passed
//-------------------------------------------------------------------------------
//		
	
TInt AccessPointIdL(TDesC8& aBuf)
{
	CCommsDatabase *database = CCommsDatabase::NewL();
	TUint32 aId ;
    TInt retVal;
    CleanupStack::PushL(database);
    CCommsDbTableView*  checkView;
    checkView = database->OpenViewMatchingTextLC(TPtrC(IAP),TPtrC(COMMDB_NAME), aBuf);
    TInt error = checkView->GotoFirstRecord();
    if (error == KErrNone)
        {
         //Get the IAP ID 
         checkView->ReadUintL(TPtrC(COMMDB_ID), aId);
         retVal = aId;
        }
           	
    CleanupStack::PopAndDestroy(2);    
    return retVal;     
}
//------------------------------------------------------------------------------
//gets the connection property name
//-------------------------------------------------------------------------------
//	
void GetConnectionPropertyNameL(RSyncMLSession &aSyncsession,TDes8& aText, TInt aPropertyPos)
	{
	//
	// at the moment RSyncMLTransport is only needed for internet connection settings
	//
	RSyncMLTransport transport;
	transport.OpenL(aSyncsession, KUidNSmlMediumTypeInternet.iUid);  // no ICP call
	CleanupClosePushL(transport);
	
	const CSyncMLTransportPropertiesArray&  arr = transport.Properties();
	
    //__ASSERT_DEBUG(arr.Count()>aPropertyPos, TUtil::Panic(KErrGeneral));
    	
	const TSyncMLTransportPropertyInfo& info = arr.At(aPropertyPos);
	aText = info.iName;
	
	CleanupStack::PopAndDestroy(&transport);
	}	


//------------------------------------------------------------------------------
//Ctestprofileutil::UsageType
//opens the connection
//-------------------------------------------------------------------------------
//
TSmlUsageType Ctestprofileutil::UsageType( CStifItemParser& aItem )
	{
	TInt type = -1;
	aItem.GetNextInt( type );
	
	switch ( type )
		{
		case 0:
			return ESmlDataSync;
			break;
		case 1:
			return ESmlDevMan;
			break;
		default:
			User::Leave( KErrArgument );
			break;
		}
		
	return ESmlDataSync;
	}



	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::SetConnectionProfileL
//
//-------------------------------------------------------------------------------
//	
void Ctestprofileutil::SetConnectionProfileL( CStifItemParser& aItem )
	{
	TSmlUsageType type = UsageType( aItem );
	if ( type == ESmlDataSync ) 
		iConnectionProfile = &iDSProfile;
	else 
		User::Leave( KErrArgument );
	}
	
	
//------------------------------------------------------------------------------
//Ctestprofileutil::OpenConnectionL
//opens the connection
//-------------------------------------------------------------------------------
//		
	
TInt Ctestprofileutil::OpenConnectionL( CStifItemParser& aItem )
	{
	
///	TInt type=-1;
//	aItem.GetNextInt(type);
	SetConnectionProfileL(aItem);
	
	RArray<TSmlTransportId> conns;
	iConnectionProfile->ListConnectionsL( conns );
	CleanupClosePushL( conns );
	
	iConnectionId = conns[0];
	iConnection.OpenL( *iConnectionProfile, iConnectionId );

	
	CleanupStack::PopAndDestroy(); // conns
	
	return KErrNone;
	}
	

//------------------------------------------------------------------------------
//Ctestprofileutil::CreateConnectionL
//opens the connection
//-------------------------------------------------------------------------------
//	

TInt Ctestprofileutil::CreateConnectionL( CStifItemParser& aItem )
	{
		TInt id = 0;
		TInt aIapId=0;
		aItem.GetNextInt( id );
		iConnectionId = id;

		iConnection.CreateL( *iConnectionProfile, id );
		iConnection.SetServerURIL(KDSProfileServerURL);
		
		
	/*	TBuf<KBufSize32> buf;
		TBuf8<KBufSize32> key;
		TBuf8<KBufSize32> value;
		TBuf8<KBufSize32> value2;
		TBuf8<KBufSize32> value3;
		_LIT(KDollar,"$");
		TBuf<10> trial;
		trial.Copy(_L(" "));

		TPtrC string;
		aItem.GetNextString ( string );
		TInt pos=string.Find(KDollar);
		StrCopy(value2,string);
		StrCopy(value3,trial);
		value2.Replace(pos,1,value3);
		//get the id corresponding to the access point
		//if the accesspoint has to be hardcoded in the code 
		//	TBuf<10> test;
		//	test.Copy(_L("AccessPointName"));
		//	aItem.GetNextString(test);
		//	aIapId=AccessPointIdL(string);
		aIapId=AccessPointIdL(value2);

		buf.Num(aIapId);
		StrCopy(value, buf);
		GetConnectionPropertyNameL(iSyncSession,key, TPropertyIntenetAccessPoint);
		iConnection.SetPropertyL(key, value);*/

		
	    return KErrNone;
	}


//------------------------------------------------------------------------------
//Ctestprofileutil::CreateTaskL
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt Ctestprofileutil::CreateTaskL( CStifItemParser& aItem )
	{
	TInt dataProvider = 0;
	aItem.GetNextInt( dataProvider );
	
	if ( dataProvider == 0 ) // not given as parameter, get from session
		{
		RArray<TSmlDataProviderId> adapters;
		CleanupClosePushL( adapters );
		iSyncSession.ListDataProvidersL( adapters );
		dataProvider = adapters[0];
		CleanupStack::PopAndDestroy(); // adapters
		}
	
	iDataProviderId = dataProvider;
	
	iTask.CreateL( iDSProfile, dataProvider, KTaskServerDataSource, KTaskClientContactDataSource );
	
	return KErrNone;
	}
	
//------------------------------------------------------------------------------
//CClientAPIAdvanced::UpdateTaskL
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt Ctestprofileutil::UpdateTaskL( CStifItemParser& /*aItem*/ )
	{
	iTask.UpdateL();
	iTaskId = iTask.Identifier();
	return KErrNone;
	}	
//------------------------------------------------------------------------------
//Ctestprofileutil::UpdateConnectionL
//opens the connection
//-------------------------------------------------------------------------------
//
TInt Ctestprofileutil::UpdateConnectionL( CStifItemParser& /*aItem*/ )
	{
	iConnection.UpdateL();
	return KErrNone;
	}

	
//------------------------------------------------------------------------------
//Ctestprofileutil::SetTaskDataL
//opens the connection
//-------------------------------------------------------------------------------
//
TInt Ctestprofileutil::SetTaskDataL( CStifItemParser& /*aItem*/ )
	{
	iTask.SetCreatorId( KTaskCreatorId );
	iTask.SetEnabledL( KTaskEnabled );
	return KErrNone;
	}
	
//------------------------------------------------------------------------------
//Ctestprofileutil::CloseTask
//opens the connection
//-------------------------------------------------------------------------------
//
TInt Ctestprofileutil::CloseTask( CStifItemParser& /*aItem*/ )
	{
	iTask.Close();
	return KErrNone;
	}

//------------------------------------------------------------------------------
//Ctestprofileutil::Closes Connection
//opens the connection
//-------------------------------------------------------------------------------
//	
TInt Ctestprofileutil::CloseConnection( CStifItemParser& /*aItem*/ )
	{
	iConnection.Close();
	return KErrNone;
	}
	

//------------------------------------------------------------------------------
//Ctestprofileutil::CloseDSProfile
//opens the connection
//-------------------------------------------------------------------------------
//
TInt Ctestprofileutil::CloseDSProfile( CStifItemParser& /*aItem*/ )
	{
	iDSProfile.Close();
	return KErrNone;
	}

//------------------------------------------------------------------------------
//Ctestprofileutil::CloseSyncSession
//opens the connection
//-------------------------------------------------------------------------------
//		
TInt Ctestprofileutil::CloseSyncSession( CStifItemParser& /*aItem*/ )
	{
	iSyncSession.Close();
	return KErrNone;
	}
    

//------------------------------------------------------------------------------
//Ctestprofileutil::ReadSettingsDBL
//Reads the profile settings from the DB
//-------------------------------------------------------------------------------
//		
TInt Ctestprofileutil::ReadSettingsDBL( CStifItemParser& /*aItem*/ )
{
	
		CNsmlProfileUtil* profileUtil = CNsmlProfileUtil::NewLC();	
		profileUtil->InternalizeFromSettingsDBL(iProfileId);
	    profileUtil->ExternalizeToCenrepL();
	    
	    CNsmlProfileUtil* profileUtil1 = CNsmlProfileUtil::NewLC();
	    profileUtil1->InternalizeFromCenrepL();
	    
	    CNsmlProfileUtil* profileUtil2 = CNsmlProfileUtil::NewL();
	    profileUtil2->InternalizeFromSettingsDBL(profileUtil1->ProfileId());
	    TInt chk=profileUtil2->IsSame(*profileUtil1);
	    
	    if(!chk)
	       User::Leave(KErrCorrupt);
	    
	    CleanupStack::PopAndDestroy(profileUtil1);
	    CleanupStack::PopAndDestroy(profileUtil);
   

    	return KErrNone;
}



//------------------------------------------------------------------------------
//Ctestprofileutil::ReadAndWriteCenrepKeysL
//Read and writes the cenrep key value
//-------------------------------------------------------------------------------
//		
TInt Ctestprofileutil::ReadAndWriteCenrepKeysL( CStifItemParser& /*aItem*/ )
{
   	CNsmlProfileUtil* profileUtil = CNsmlProfileUtil::NewLC();
   	TBool chk;
   	profileUtil->WriteInterruptFlagL(-1);
   	profileUtil->IsValidResumeL(chk);
   	if(chk)
   	{
   		//try setting to otherthan -1
   		profileUtil->WriteInterruptFlagL(1);
   		profileUtil->IsValidResumeL(chk);
   		if(chk)
   		  User::Leave(KErrCorrupt);
   	}
   	else
   		User::Leave(KErrCorrupt);
   	 	
   	TInt accesspoint;
   	profileUtil->WriteAccessPointIdL(5);
   	profileUtil->AccessPointIdL(accesspoint);
   	if(accesspoint!=5)
   	   	User::Leave(KErrCorrupt);
   		   
   	CleanupStack::PopAndDestroy(profileUtil);
   	
   	return KErrNone;

}
//------------------------------------------------------------------------------
//Ctestprofileutil::ReadAndWriteCenrepKeysL
//Phone goes to OffLine mode
//-------------------------------------------------------------------------------
//		


TInt Ctestprofileutil::OffLineL( CStifItemParser& /*aItem*/ )
{
    CRepository* repository = CRepository::NewL(KCRUidCoreApplicationUIs);
    TInt err = repository->Set(KCoreAppUIsNetworkConnectionAllowed, 
                                        ECoreAppUIsNetworkConnectionNotAllowed);
    
    if(repository)
       delete repository;
    
    User::LeaveIfError(err);
   	return KErrNone;

}

//------------------------------------------------------------------------------
//Ctestprofileutil::ReadAndWriteCenrepKeysL
//Phone goes to General mode
//-------------------------------------------------------------------------------
//		


TInt Ctestprofileutil::GeneralL( CStifItemParser& /*aItem*/ )
{
    CRepository* repository = CRepository::NewL(KCRUidCoreApplicationUIs);
    TInt err = repository->Set(KCoreAppUIsNetworkConnectionAllowed, 
                                            ECoreAppUIsNetworkConnectionAllowed);
    if(repository)
         delete repository;
    User::LeaveIfError(err);
    
    
   	return KErrNone;

}


//------------------------------------------------------------------------------
//Ctestprofileutil::ReadAndWriteCenrepKeysL
//Read and writes the cenrep key value
//-------------------------------------------------------------------------------
//		


TInt Ctestprofileutil::SyncL( CStifItemParser& aItem )
{
   	TInt param = 0;
	aItem.GetNextInt( param );

    CSync *sync = CSync::NewL(param);

	CActiveScheduler::Start();

   	return KErrNone;

}

//------------------------------------------------------------------------------
//Ctestprofileutil::LaunchAutoRestartL
//Launches the Auto-restart
//-------------------------------------------------------------------------------
//

TInt Ctestprofileutil::LaunchAutoRestartL( CStifItemParser& aItem )
{

  RProcess rp;
  TInt err = rp.Create(KNetMon,KNullDesC);
  User::LeaveIfError(err);
  rp.Resume();
     
  return KErrNone;	
}
// ========================== OTHER EXPORTED FUNCTIONS =========================
// None


//------------------------------------------------------------------------------
//Ctestprofileutil::LaunchAutoRestartL
//Launches the Auto-restart
//-------------------------------------------------------------------------------
//

TInt Ctestprofileutil::CallDelay( CStifItemParser& aItem )
{

  	User::After(1000);
	return KErrNone;
     
}
//  [End of File] - Do not remove
