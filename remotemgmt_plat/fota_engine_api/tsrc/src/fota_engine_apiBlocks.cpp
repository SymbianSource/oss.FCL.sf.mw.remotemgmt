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
* Description: definition of dm constants/exported methods
* 	This is part of remotemgmt_plat.
*
*/



// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <e32des16.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "fota_engine_api.h"

#include <EIKENV.H>
// for FOTA

#include <SyncMLClientDM.h>
#include <nsmlconstants.h>
#include <fotaengine.h>

#include <SyncMLDef.h>
#include <SyncMLClient.h>

#include "FotaIPCTypes.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Cfota_engine_api::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cfota_engine_api::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cfota_engine_api::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cfota_engine_api::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cfota_engine_api::ExampleL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove
        
        ENTRY( "DownloadUpdateL", Cfota_engine_api::DownloadUpdateL),
        
        ENTRY( "RFotaEngineSessionCons", Cfota_engine_api::RFotaEngineSessionCons),
        ENTRY( "OpenL", Cfota_engine_api::OpenL),
        ENTRY( "Close", Cfota_engine_api::CloseL),
        ENTRY( "DownloadL", Cfota_engine_api::DownloadL),
        ENTRY( "IsPackageStoreSizeAvailable", Cfota_engine_api::IsPackageStoreSizeAvailableL),
        ENTRY( "GetState", Cfota_engine_api::GetStateL),
        ENTRY( "GetResult", Cfota_engine_api::GetResultL),
        ENTRY( "CurrentVersion", Cfota_engine_api::CurrentVersionL),
        ENTRY( "GetUpdatePackageIds", Cfota_engine_api::GetUpdatePackageIdsL),
        ENTRY( "Version", Cfota_engine_api::VersionL),
        ENTRY( "GenericAlertSentL", Cfota_engine_api::GenericAlertSentL),
        ENTRY( "ScheduledUpdateL", Cfota_engine_api::ScheduledUpdateL),
				ENTRY( "TryResumeDownload", Cfota_engine_api::TryResumeDownloadL),

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cfota_engine_api::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cfota_engine_api::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Kfota_engine_api, "fota_engine_api" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Kfota_engine_api, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kfota_engine_api, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cfota_engine_api::DownloadUpdateL
// DownloadUpdateL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cfota_engine_api::DownloadUpdateL(  CStifItemParser& aItem )
    {
	RSyncMLSession syncSession;
	syncSession.OpenL();
	CleanupClosePushL(syncSession);
	
	RSyncMLDevManProfile DMProfile;
	DMProfile.CreateL( syncSession );
	CleanupClosePushL( DMProfile);

    DMProfile.SetDisplayNameL(_L("####´51"));
    DMProfile.SetServerIdL(_L8("ServerID51"));
    DMProfile.SetCreatorId(85);
    DMProfile.SetPasswordL(_L8("Password"));
    DMProfile.SetServerPasswordL(_L8("ServerPassword"));
    DMProfile.SetUserNameL(_L8("Username"));
	
	DMProfile.UpdateL();
		
	RSyncMLConnection connection;
	connection.OpenL( DMProfile, KUidNSmlMediumTypeInternet.iUid );
	CleanupClosePushL( connection );
	TPtrC stringHostaddress;
  if( aItem.GetNextString ( stringHostaddress ) == KErrNone )
  { 
    	HBufC16* newaddr =  stringHostaddress.AllocL();     	  	
			TBuf8<100> hostAddress((newaddr->Des()).Collapse());
			connection.SetServerURIL( hostAddress );
			delete newaddr;
	}

		
    TSmlProfileId ProfileId = DMProfile.Identifier();
	
	CleanupStack::PopAndDestroy(); // connection
	CleanupStack::PopAndDestroy(); // DMProfile
	CleanupStack::PopAndDestroy(); // syncSession
		
    RFotaEngineSession session;
    session.OpenL();

  TInt PkgId = 1;
    
    TBuf8<10> PkgName;
    PkgName.Copy(_L8("zkg"));

    TBuf8<10> PkgVersion;
    PkgVersion.Copy(_L8("1.0"));
			TPtrC stringPkgUrl;
    	if( aItem.GetNextString ( stringPkgUrl ) == KErrNone )
    	{ 
    	HBufC16* newurl =  stringHostaddress.AllocL();     	  	
			TBuf8<100> PkgURL((newurl->Des()).Collapse());
			delete newurl;
	
	TInt err = session.DownloadAndUpdate(PkgId,PkgURL,ProfileId,PkgName,PkgVersion); 
	
	   
    session.Close();
    if(err != KErrNone )
    {
    	return err;
    }    
	}
	return KErrNone;
    }
    
  
// -----------------------------------------------------------------------------
// Cfota_engine_api::RFotaEngineSessionCons
// RFotaEngineSessionCons test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::RFotaEngineSessionCons(  CStifItemParser& /* aItem */)
    {
        RFotaEngineSession session;
    	return KErrNone;
    }
    
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::OpenL
// OpenL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::OpenL(  CStifItemParser& /* aItem */ )
    {
    
    	RFotaEngineSession session;
    	TRAPD(err, session.OpenL());
    	if(err)
    	{
    		return err;
    	}
    	session.Close();
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::Close
// Close test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::CloseL(  CStifItemParser& /* aItem */ )
    {    
    	RFotaEngineSession session;
    	session.OpenL();
    	session.Close();
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::DownloadL
// DownloadL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::DownloadL(  CStifItemParser& aItem )
    {
    
		RSyncMLSession syncSession;
		syncSession.OpenL();
		CleanupClosePushL(syncSession);
		
		RSyncMLDevManProfile DMProfile;
		DMProfile.CreateL( syncSession );
		CleanupClosePushL( DMProfile);

	    DMProfile.SetDisplayNameL(_L("####´52"));
	    DMProfile.SetServerIdL(_L8("ServerID52"));
	    DMProfile.SetCreatorId(85);
	    DMProfile.SetPasswordL(_L8("Password"));
	    DMProfile.SetServerPasswordL(_L8("ServerPassword"));
	    DMProfile.SetUserNameL(_L8("Username"));
		
		DMProfile.UpdateL();
			
		RSyncMLConnection connection;
		connection.OpenL( DMProfile, KUidNSmlMediumTypeInternet.iUid );
		CleanupClosePushL( connection );
		TPtrC stringHostaddress;
    if( aItem.GetNextString ( stringHostaddress ) == KErrNone )
    { 
    	HBufC16* newaddr =  stringHostaddress.AllocL();     	  	
			TBuf8<100> hostAddress((newaddr->Des()).Collapse());
			connection.SetServerURIL( hostAddress );
			delete newaddr;
		}
			
	  TSmlProfileId ProfileId = DMProfile.Identifier();
			
		CleanupStack::PopAndDestroy(); // connection
		CleanupStack::PopAndDestroy(); // DMProfile
		CleanupStack::PopAndDestroy(); // syncSession
		
		RFotaEngineSession session;
	    session.OpenL();

 	    TInt PkgId = 1;
			TPtrC stringPkgUrl;
    	if( aItem.GetNextString ( stringPkgUrl ) == KErrNone )
    	{ 
    	HBufC16* newurl =  stringHostaddress.AllocL();     	  	
			TBuf8<100> PkgURL((newurl->Des()).Collapse());
			delete newurl;
			
			    	
	    TBuf8<10> PkgName;
	    PkgName.Copy(_L8("zkg"));

	    TBuf8<10> PkgVersion;
	    PkgVersion.Copy(_L8("1.0"));

			TInt err = session.Download(PkgId,PkgURL,ProfileId,PkgName,PkgVersion); 	
		 
	    session.Close();  
			if(KErrNone != err)
			{
				return err;
			}
			}
    	return KErrNone;    	
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::IsPackageStoreSizeAvailable
// IsPackageStoreSizeAvailable test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::IsPackageStoreSizeAvailableL(  CStifItemParser& /* aItem */)
    {

   		RFotaEngineSession session;
	    session.OpenL();

		TInt size = 1024;
		TBool result;
		result = session.IsPackageStoreSizeAvailable(size);
		session.Close();
		if( FALSE == result)
		{
			return KErrGeneral;
		}
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::GetState
// GetState test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::GetStateL(  CStifItemParser& /* aItem */)
    {
    	RFotaEngineSession session;
	    session.OpenL();

		RFotaEngineSession::TState result = session.GetState(-1);
		session.Close();
		if( RFotaEngineSession::EIdle != result)
		{
			return KErrGeneral;
		}
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::GetResult
// GetResult test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::GetResultL(  CStifItemParser& /* aItem */ )
    {
    	RFotaEngineSession session;
	    session.OpenL();

		TInt result = session.GetResult(-1);
		session.Close();
		if( -1 != result)
		{
			return KErrGeneral;
		}
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::CurrentVersion
// CurrentVersion test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::CurrentVersionL(  CStifItemParser& /* aItem */)
    {
	   	RFotaEngineSession session;
	    session.OpenL();
   
   		TBuf<100> SWVersion;
   
		TInt result = session.CurrentVersion( SWVersion);
		session.Close();
		if( KErrNone != result)
		{
			return KErrGeneral;
		}
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::GetUpdatePackageIds
// GetUpdatePackageIds test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::GetUpdatePackageIdsL(  CStifItemParser& /* aItem */)
    {    
    	RFotaEngineSession session;
	    session.OpenL();
   
   		TBuf<500> packID;
   		TInt result = session.GetUpdatePackageIds( packID);
		session.Close();
		if( KErrNone != result)
		{
			return KErrGeneral;
		}
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::Version
// Version test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::VersionL(  CStifItemParser& /* aItem */ )
    {
     	RFotaEngineSession session;
	    session.OpenL();

//		TVersion version = 

//		No src file consists of RFotaEngineSession::Version ?
//		session.Version();
		session.Close();
	
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::GenericAlertSentL
// GenericAlertSentL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::GenericAlertSentL(  CStifItemParser& /* aItem */)
    {
    	RFotaEngineSession session;
    	session.OpenL();
    	TInt packageID = -1;
    	TRAPD(err, session.GenericAlertSentL(packageID));
    	session.Close();
    	if(err)
    	{
    		return err;
    	}
    	return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::ScheduledUpdateL
// ScheduledUpdateL test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
    TInt Cfota_engine_api::ScheduledUpdateL(  CStifItemParser& /* aItem */)
    {		

    	RFotaEngineSession fotaengine;

		TRAPD(error,fotaengine.OpenL());
		if(error)
		  {
				iLog->Log(_L("CFMSInterruptAob::LaunchFotaScheduleUpdateL()- error in opening the fota engine"));
			  	fotaengine.Close();
			  	return error;

		  }
		TFotaScheduledUpdate sched(-1 ,-1 );   //trigger the Fota remainder dialog here..

	  TPckg<TFotaScheduledUpdate>    p(sched);

	  TRAPD(err, fotaengine.ScheduledUpdateL(sched));

	  fotaengine.Close();
	  if(err)
	  {
			return err;
	  }
	  return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// Cfota_engine_api::TryResumeDownload
// TryResumeDownload test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TInt Cfota_engine_api::TryResumeDownloadL(  CStifItemParser& aItem )
    {		

    	RSyncMLSession syncSession;
		syncSession.OpenL();
		CleanupClosePushL(syncSession);
		
		RSyncMLDevManProfile DMProfile;
		DMProfile.CreateL( syncSession );
		CleanupClosePushL( DMProfile);

	    DMProfile.SetDisplayNameL(_L("####´53"));
	    DMProfile.SetServerIdL(_L8("ServerID53"));
	    DMProfile.SetCreatorId(85);
	    DMProfile.SetPasswordL(_L8("Password"));
	    DMProfile.SetServerPasswordL(_L8("ServerPassword"));
	    DMProfile.SetUserNameL(_L8("Username"));	

		DMProfile.UpdateL();
			
		RSyncMLConnection connection;
		connection.OpenL( DMProfile, KUidNSmlMediumTypeInternet.iUid );
		CleanupClosePushL( connection );
		TPtrC stringHostaddress;
    if( aItem.GetNextString ( stringHostaddress ) == KErrNone )
    { 
    	HBufC16* newaddr =  stringHostaddress.AllocL();     	  	
			TBuf8<100> hostAddress((newaddr->Des()).Collapse());
			connection.SetServerURIL( hostAddress );
			delete newaddr;
		}
			
	    TSmlProfileId ProfileId = DMProfile.Identifier();
			
		CleanupStack::PopAndDestroy(); // connection
		CleanupStack::PopAndDestroy(); // DMProfile
		CleanupStack::PopAndDestroy(); // syncSession
		
	  session.OpenL();

 	    TInt PkgId = 1;

	    
	    TBuf8<10> PkgName;
	    PkgName.Copy(_L8("zkg"));

	    TBuf8<10> PkgVersion;
	    PkgVersion.Copy(_L8("1.0"));
		TPtrC stringPkgUrl;
    	if( aItem.GetNextString ( stringPkgUrl ) == KErrNone )
    	{ 
    	HBufC16* newurl =  stringHostaddress.AllocL();     	  	
			TBuf8<100> PkgURL((newurl->Des()).Collapse());
			delete newurl;
		TInt err = session.Download(PkgId,PkgURL,ProfileId,PkgName,PkgVersion);
		if(err)
		{
			iLog->Log(_L("CFMSInterruptAob::TryResumeDownloadL()- download error"));
		  
		  	return err;
		}
	}
	  User::After(5000000*6);
		
		session.Close();
		RWsSession iWsSession;
		TInt err = iWsSession.Connect();
		if(err)
		{
			iLog->Log(_L("CFMSInterruptAob::TryResumeDownloadL()- Window server session error"));		  
		  return err;
		}
		TApaTask(CCoeEnv::Static()->WsSession());
		TApaTaskList tasklist(iWsSession);
		TApaTask task=tasklist.FindApp(TUid::Uid(0x101F6DE5));
		if(task.Exists())
		{
			task.KillTask();
		}
		iWsSession.Close();
		User::After(5000000*3);
		session.OpenL();

    session.TryResumeDownload();
		User::After(5000000*3);
	
   	return KErrNone;
    }
   
// Cfota_engine_api::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cfota_engine_api::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
