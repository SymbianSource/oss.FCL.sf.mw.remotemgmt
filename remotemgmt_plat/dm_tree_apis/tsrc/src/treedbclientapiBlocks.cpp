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



// INCLUDE FILES
#include <e32svr.h>
#include <StifLogger.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <nsmldmtreedbclient.h>

#include <TestScripterInternal.h>
#include <utf.h>

#include "treedbclientapi.h"
#include <nsmldmcallbackserver.h>

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
// Ctreedbclientapi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Ctreedbclientapi::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Ctreedbclientapi::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Ctreedbclientapi::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Ctreedbclientapi::ExampleL ),
        ENTRY( "Createcallbacksession", Ctreedbclientapi::CreatecallbacksessionL ),
        ENTRY( "Destroycallbacksession", Ctreedbclientapi::DestroycallbacksessionL ),
        ENTRY( "Connect", Ctreedbclientapi::ConnectL ),
        ENTRY( "ResourceCount", Ctreedbclientapi::ResourceCountL ),
        ENTRY( "Close", Ctreedbclientapi::CloseL ),
        ENTRY( "GetLuidAlloc", Ctreedbclientapi::GetLuidAllocL ),
        ENTRY( "GetUriSegList", Ctreedbclientapi::GetUriSegListL ),
        ENTRY( "SetServer", Ctreedbclientapi::SetServerL ),
        ENTRY( "AddMappingInfo", Ctreedbclientapi::AddMappingInfoL ),
        ENTRY( "RemoveMappingInfo", Ctreedbclientapi::RemoveMappingInfoL ),
        ENTRY( "RenameMapping", Ctreedbclientapi::RenameMappingL ),
        ENTRY( "UpdateMappingInfo", Ctreedbclientapi::UpdateMappingInfoL ),
        ENTRY( "DeleteAcl", Ctreedbclientapi::DeleteAclL ),
        ENTRY( "UpdateAcl", Ctreedbclientapi::UpdateAclL ),
        ENTRY( "GetAcl", Ctreedbclientapi::GetAclL ),
        ENTRY( "CheckAcl", Ctreedbclientapi::CheckAclL),
        ENTRY( "CheckDynamicAcl", Ctreedbclientapi::CheckDynamicAclL),
        ENTRY( "DefaultACLsToServer", Ctreedbclientapi::DefaultACLsToServerL ),
        ENTRY( "DefaultACLsToChildren", Ctreedbclientapi::DefaultACLsToChildrenL),
        ENTRY( "EraseServerId", Ctreedbclientapi::EraseServerIdL ),
        ENTRY( "StartAtomic", Ctreedbclientapi::StartAtomic),
        ENTRY( "CommitAtomic", Ctreedbclientapi::CommitAtomic ),
        ENTRY( "RollbackAtomic", Ctreedbclientapi::RollbackAtomic),
        

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreedbclientapi::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( Ktreedbclientapi, "treedbclientapi" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Ktreedbclientapi, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Ktreedbclientapi, 
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }
    
// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Ctreedbclientapi::CreatecallbacksessionL( )
    {
    	__UHEAP_MARK;
    	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession();
//    	TInt status = session->Connect();
    	delete session;
    	__UHEAP_MARKEND;
    	return KErrNone;
    }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::DestroycallbacksessionL ()
        {
        	__UHEAP_MARK;
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	        TRAPD (errClose, session->Close());
	        if(errClose)
	        {
	        	delete session;
	        	return errClose;
	        }
	        TRAPD (errDelete, delete session);
	        if(errDelete)
	        {
	        	delete session;
	        	return errDelete;
	        }
	        __UHEAP_MARKEND;
        	return KErrNone;
        }
  
  
// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
      TInt Ctreedbclientapi::ConnectL ()
        {
        	__UHEAP_MARK;
	        RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	        session->Close();
	        delete session;
	        __UHEAP_MARKEND;
        	return KErrNone;
        }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::ResourceCountL ()
        {
        	__UHEAP_MARK;
	        RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
	        
	        TInt result = session->Connect();
	   		if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	        //ResourceCount
	        TInt count = session->ResourceCount();
	        
	        if( 0 == count) 
	        {	        
	        	session->Close();
	        	delete session;
	        	return KErrNone;	
	        }        
	        session->Close();
	        delete session;
	        __UHEAP_MARKEND;
	        return KErrGeneral;	
         }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::CloseL ()
        {
        	__UHEAP_MARK;
   	        RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	        TRAPD( res, session->Close());
	        if(res)
	        {
	        	delete session;
	        	return res;
	        }
	        delete session;
	        __UHEAP_MARKEND;
        	return KErrNone;
        }


// from here


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::GetLuidAllocL()
        {
        // Internet adapter id:	0x101F6DE2
        // Numerical value:		270495202

        __UHEAP_MARK;

			
 
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;

        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	session->Close();
	        	delete session;
	        	return KErrGeneral;
	        }
	        
	        TUint32 apId = 270495202;
	       	_LIT8(KaURI,"./AP");
		  		TBufC8<25> aURI( KaURI );
					HBufC8* getaLuid = NULL;
	        
	        
			getaLuid = session->GetLuidAllocL(apId, aURI);
			if( NULL == getaLuid)
			{
				
				session->Close();
				delete session;
				return KErrGeneral;
			}

		
			delete getaLuid;
			session->Close();
			delete session;
	        
	        __UHEAP_MARKEND;
        	
        	return KErrNone;
        }
        

// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
        TInt Ctreedbclientapi::GetUriSegListL ()
        {
 //        	__UHEAP_MARK;
   	        RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }

	        TUint32 apId = 270495202;
	       	_LIT8(KaURI,"./AP");
		  	TBufC8<25> aURI( KaURI );
			//HBufC8* getaLuid = NULL;
	        
			TInt size = 50;
			//CArrayFixFlat <TSmlDmMappingInfo> aURISegList( size);
			CArrayFixFlat <TSmlDmMappingInfo>* aURISegList = new CArrayFixFlat<TSmlDmMappingInfo> ( size);

	        session->GetUriSegListL( apId, aURI, *aURISegList);
	        
	        TRAPD( res, session->Close());
	        if(res)
	        {
	        	delete session;
	        	return res;
	        }
	        delete aURISegList;
	        delete session;	        
	        
//	        __UHEAP_MARKEND;
        	return KErrNone;
        }
        
  
// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//      
        TInt Ctreedbclientapi::SetServerL (CStifItemParser& aItem)
        {
        	__UHEAP_MARK;

			TPtrC string;
    		TInt err = aItem.GetNextString ( string );
    		if( err )
    		{
    			return KErrNotFound;
    		}
			HBufC8* serverId2 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
	
			CleanupStack::PushL(serverId2);
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	        session->SetServerL(*serverId2);


	        TRAPD( res, session->Close());
	        if(res)
	        {
	        	delete session;
	        	return res;
	        }	        
	        delete session;
	        CleanupStack::PopAndDestroy(); // For ServerId2
	        __UHEAP_MARKEND;
        	return KErrNone;
        }
  
  
// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
      TInt Ctreedbclientapi::AddMappingInfoL (CStifItemParser& aItem )
        {
            TUint adapterId;
		    TPtrC string1;
    		TPtrC string2;
    		TPtrC string3;

            TInt err = aItem.GetNextInt( adapterId );
 		    TInt err1 = aItem.GetNextString( string1 );
    		TInt err2 = aItem.GetNextString( string2 );
    		TInt err3 = aItem.GetNextString( string3 );
    		if( err || err1 || err2)
    		{
    			return KErrNotFound;
    		}
 
		    HBufC8* aURI = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string1);
			HBufC8* aLuid = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string2);
			HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string3);

			RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	       
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	return KErrGeneral;
	        }
	      
	      	session->SetServerL(*serverId);


	        result = session->AddMappingInfoL(adapterId, *aURI, *aLuid);
	        session->Close();
	        delete session;
	        delete aURI;
	        delete aLuid;
	        delete serverId;
	        if (result)
	        {
	        	return result;
	        }	        
	        return KErrNone;
        }

// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::RemoveMappingInfoL (CStifItemParser& aItem)
        {
            TUint adapterId;
		    TPtrC string1;
    		TPtrC string2;
    		TPtrC string3;

            TInt err = aItem.GetNextInt( adapterId );
 		    TInt err1 = aItem.GetNextString( string1 );
    		TInt err2 = aItem.GetNextString( string2 );
    		if( err || err1 || err2)
    		{
    			return KErrNotFound;
    		}
 
		    HBufC8* aURI = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string1);
			HBufC8* aLuid = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string2);
			HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string3);

			RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	       
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	return KErrGeneral;
	        }
	     
	      	session->SetServerL(*serverId);
	        result = session->AddMappingInfoL(adapterId, *aURI, *aLuid);
	        if (result)
	        {
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	return result;
	        }

			TRAPD (ret, result = session->RemoveMappingInfoL(adapterId, *aURI, TRUE));
			if(ret)
			{
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
				return ret;
			}
				
	        session->Close();
	     	if (result)
	        {
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	return result;
	        }
	        delete session;
	        delete aURI;
	        delete aLuid;
	        delete serverId;
	     
	        return KErrNone;
        }

// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::RenameMappingL (CStifItemParser& /*aItem */)
        {

			TUint32 adapterId = 270495202 ;
			_LIT8(KRootnode,"AP/APID001/NAP/NAP001");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );


// We need to change this for each run of RenameMappingsL
			_LIT8(Kreplacenode,"NAP006");
		    TBufC8<25> replacenode( Kreplacenode );
		    		    
		    _LIT8(Kgetnode,"AP/APID001/NAP/NAP006");
		    TBufC8<25> getnode(Kgetnode);
	
			RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	       
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return result;
	        }
	  	    
			TInt error = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			if( error )
			{
				return error;
			}

			TInt ret = session->RenameMappingL( adapterId, rootnode, replacenode);
			
			session->Close();
			delete session;
			if(ret  )
			{				
				return KErrGeneral;
			}
			else
			{				
				return KErrNone;
			}
	}


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::UpdateMappingInfoL (CStifItemParser& /*aItem */)
        {
        	const TInt KGranularity = 8;
		   	TUint32 adapterId = 270495202;
			_LIT8(KRootnode,"AP/APID002");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );
		    
		    _LIT8(KChildnode1,"AP/APID002/NAP");
		    TBufC8<25> childnode1( KChildnode1 );
		    _LIT8(KChildnode1Luid,"2");
		    TBufC8<25> childnode1Luid( KChildnode1Luid );
			
			_LIT8(KChildnode2,"AP/APID002/Px");
		    TBufC8<25> childnode2( KChildnode2 );
		    _LIT8(KChildnode2Luid,"3");
		    TBufC8<25> childnode2Luid( KChildnode2Luid );
			
			_LIT8(KChildnode3,"AP/APID002/Ax");
		    TBufC8<25> childnode3( KChildnode3 );
		    _LIT8(KChildnode3Luid,"4");
		    TBufC8<25> childnode3Luid( KChildnode3Luid );
		    
		    
			RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	       
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
			TInt error = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			TInt error1 = session->AddMappingInfoL( adapterId, childnode1, childnode1Luid );
			TInt error2 = session->AddMappingInfoL( adapterId, childnode2, childnode2Luid );
			TInt error3 = session->AddMappingInfoL( adapterId, childnode3, childnode3Luid );

			if(error || error1 || error2 || error3)
			{
				session->Close();
				delete session;
				return KErrGeneral;
			}
			

		 _LIT8(KChildnode4,"NAP/Px");
		    TBufC8<25> childnode4( KChildnode4 );
		    
			CBufBase *childList = CBufFlat::NewL(16);
		    CleanupStack::PushL(childList);
		    childList->InsertL(0,childnode4);
		    
		    _LIT8(KserverId,"funambool");
		  	TBufC8<25> serverId( KserverId );
		  	
		    session->SetServerL(serverId);
		    
		    session->UpdateMappingInfoL(adapterId, rootnode, *childList); 
		    
		    
		   	CArrayFixFlat<TSmlDmMappingInfo>* aURISegList =	new (ELeave) CArrayFixFlat <TSmlDmMappingInfo> (KGranularity);


			session->GetUriSegListL( adapterId, rootnode, *aURISegList );
			
			TBufC8<25> getchildnode1 = aURISegList->At(0).iURISeg;
			TBufC8<25> getchildnode2 = aURISegList->At(1).iURISeg;

			_LIT8(Knapnode,"NAP");
		    TBufC8<25> napnode( Knapnode );

			_LIT8(KPxnode,"Px");
		    TBufC8<25> Pxnode( KPxnode );

			
			session->Close();
			delete session;
			CleanupStack::PopAndDestroy(); // childList
			if(getchildnode1.Compare(napnode) || getchildnode2.Compare(Pxnode) || aURISegList->Count() != 2)
			{
				delete aURISegList;
			 	return KErrGeneral;		
			}
			delete aURISegList;
		    return KErrNone;
        }
        

// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::DeleteAclL (CStifItemParser& aItem)
        {	

        TUint adapterId;
		    TPtrC string1;
    		TPtrC string2;
    		TPtrC string3;

            TInt err = aItem.GetNextInt( adapterId );
 		    TInt err1 = aItem.GetNextString( string1 );
    		TInt err2 = aItem.GetNextString( string2 );
    		TInt err3 = aItem.GetNextString( string3 );
    		if( err || err1 || err2)
    		{
    			return KErrNotFound;
    		}
 
		    HBufC8* aURI = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string1);
			HBufC8* aLuid = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string2);
			HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string3);

			RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	       
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	return result;	   
	        }
	      
	      	session->SetServerL(*serverId);
	        result = session->AddMappingInfoL(adapterId, *aURI, *aLuid);
	        if (result)
	        {    
	        	session->Close();	    
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	return result;
	        }

			_LIT8(Kacl,"Add=funambool&Get=funambool&Replace=funambool&Delete=funambool&Exec=funambool");
		    TBufC8<100> aACL( Kacl );

			TInt success = session->UpdateAclL(*aURI, aACL);
			
			if(KErrNone != success)
			{
				session->Close();	    
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	return success;	        	
			}
			
			CBufBase *childList = CBufFlat::NewL(16);
	    CleanupStack::PushL(childList);
		
			success = session->DeleteAclL(*aURI);
			TRAPD( ret2, session->GetAclL(*aURI, *childList));
			if(ret2)
			{				
				session->Close();	    
	        	delete session;
	        	delete aURI;
	        	delete aLuid;
	        	delete serverId;
	        	CleanupStack::PopAndDestroy();
	        	return success;	        	
			}
			TInt foundfun = 0;
			if(childList->Size())
			{
				
				TBuf8<18> getacl;
				childList->Read(0, getacl);
				childList->Reset();
			
				foundfun = getacl.Find(*serverId);
			}
			session->Close();
	        delete session;
	        delete aURI;
	        delete aLuid;
	        delete serverId;
			CleanupStack::PopAndDestroy(); // childList
			
			if(foundfun == KErrNotFound) // Check here
				return KErrNone;
			return KErrGeneral;

        }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::UpdateAclL(CStifItemParser& aItem)
        {  
       	    TPtrC string;
		    TInt err = aItem.GetNextString ( string );
		    if( err )
		    {
		    	return KErrNotFound;
		    }
			HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
        	CleanupStack::PushL(session);

			TInt result = session->Connect();
	    if (KErrNone != result)
	    {
	    			CleanupStack::PopAndDestroy(); // session
	        	delete serverId;
	        	return result;
	    }
			session->SetServerL(*serverId);
			
		   	TUint32 adapterId = 270495202 ; // Internet adapter id
			_LIT8(KRootnode,"AP/APID002");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );
			TInt error = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			
			if(error)	     	
			{
				session->Close();
				delete session;
			}
			_LIT8(Kacl,"Add=funambool&Get=funambool");
		    TBufC8<50> aACL( Kacl );
		    
		   	CBufBase *childList = CBufFlat::NewL(16);
		    CleanupStack::PushL(childList);

			session->UpdateAclL(rootnode, aACL);
			session->GetAclL(rootnode, *childList);
		    TBuf8<27>getacl;
		    childList->Read(0, getacl);
		    TInt foundfun = getacl.Find(*serverId);
		    
			session->Close();
		 	CleanupStack::PopAndDestroy(); // childList
			CleanupStack::PopAndDestroy(); // serverId
		    
			if(foundfun != KErrNotFound)
			{
				return KErrNone;	
			}
			else
			{
				return KErrNotFound;
			}
        }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//        
        TInt Ctreedbclientapi::GetAclL(CStifItemParser& aItem)
        {
       	    TPtrC string;
		    TInt err = aItem.GetNextString ( string );
		    if( err )
		    {
		    	return KErrNotFound;
		    }
			HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
        	CleanupStack::PushL(session);

			TInt result = session->Connect();
	    if (KErrNone != result)
	    {
	    			CleanupStack::PopAndDestroy(); // session
	        	delete serverId;
	        	return result;
	    }
			session->SetServerL(*serverId);
			
		   	TUint32 adapterId = 270495202 ; // Internet adapter id
			_LIT8(KRootnode,"AP/APID002");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );
			TInt error = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			
			if(error)	     	
			{
				session->Close();
				CleanupStack::PopAndDestroy(); 
				delete session;
			}
			_LIT8(Kacl,"Add=funambool&Get=funambool");
		    TBufC8<50> aACL( Kacl );
		    
		   	CBufBase *childList = CBufFlat::NewL(16);
		    CleanupStack::PushL(childList);

			session->UpdateAclL(rootnode, aACL);
			error = session->GetAclL(rootnode, *childList);
			if(error)
			{
				session->Close();
		 		CleanupStack::PopAndDestroy(); // childList
				CleanupStack::PopAndDestroy(); 
				return error;
			}
		    TBuf8<27>getacl;
		    childList->Read(0, getacl);
		    TInt foundfun = getacl.Find(*serverId);
		    
			session->Close();
		 	CleanupStack::PopAndDestroy(); // childList
			CleanupStack::PopAndDestroy(); 
		    
			if(foundfun != KErrNotFound)
			{
				return KErrNone;	
			}
			else
			{
				return KErrNotFound;
			}
        }
  
  
// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
      TInt Ctreedbclientapi::CheckAclL(CStifItemParser& aItem)
        {
            TPtrC string;
		    TInt err = aItem.GetNextString ( string );
		    if( err )
		    {
		    	return KErrNotFound;
		    }
			HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
        	CleanupStack::PushL(session);

			TInt result = session->Connect();
	    if (KErrNone != result)
	    {
	    			CleanupStack::PopAndDestroy(); // session
	        	delete serverId;
	        	return result;
	    }
			session->SetServerL(*serverId);
			
		   	TUint32 adapterId = 270495202 ; // Internet adapter id
			_LIT8(KRootnode,"AP/APID002");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );
			TInt error = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			
			_LIT8(Kacl,"Add=funambool&Get=funambool");
		    TBufC8<100> aACL( Kacl );

			session->UpdateAclL(rootnode, aACL);
			TNSmlDmCmdType aCmdType = EAclGet;
			TBool isCMDtype1 = session->CheckAclL( rootnode, aCmdType );
			aCmdType = EAclAdd;
			TBool isCMDtype2 = session->CheckAclL( rootnode, aCmdType );
			aCmdType = EAclReplace;
			TBool isCMDtype3 = session->CheckAclL( rootnode, aCmdType );
			aCmdType = EAclDelete;
			TBool isCMDtype4 = session->CheckAclL( rootnode, aCmdType );

			_LIT8(Kacl1,"Add=hi&Get=hi");
		    TBufC8<100> aACL1( Kacl1 );
		    
		   	session->UpdateAclL(rootnode, aACL1);
			aCmdType = EAclGet;
			TBool isCMDtype5 = session->CheckAclL( rootnode, aCmdType );
			aCmdType = EAclAdd;
			TBool isCMDtype6 = session->CheckAclL( rootnode, aCmdType );

			session->Close();
			CleanupStack::PopAndDestroy();//session
			delete serverId;

			if(isCMDtype1 == 1 && isCMDtype2 == 1 &&isCMDtype3 == 0 &&isCMDtype4 == 0 &&
			   isCMDtype5 == 0 && isCMDtype6 == 0 )
			{
				return KErrNone;
			}
			else
			{
				return KErrGeneral;
			}
		    
        }

// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::CheckDynamicAclL(CStifItemParser& /*aItem */)
        {
// Yet to be covered

/*
        		__UHEAP_MARK;
			TPtrC string;
    		TInt err = aItem.GetNextString ( string );
    		if( err )
    		{
    			return KErrNotFound;
    		}
			HBufC8* serverId2 = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
	
			CleanupStack::PushL(serverId2);
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	        session->SetServerL(*serverId2);
	     //   session->SetServerL(serverId);
	        
//	        CleanupStack::PopAndDestroy();
//	        CleanupStack::Pop();

//EXPORT_C TBool RNSmlDMCallbackSession::CheckDynamicAclL(
//        const TDesC8& aURI, TNSmlDmCmdType aCmdType,   CNSmlDmMgmtTree *aMgmtTree        )

		//class CNSmlDmMgmtTree* iMgmtTree;
//uri, EAclGet, iMgmtTree

			_LIT8 (KURI, "AP/APID001");
			//TDesC8 aURI(KURI);
			TBufC8<25> aURI(KURI);
			
			CNSmlDmMgmtTree* MgmtTree;
			
			TBool ret = session->CheckDynamicAclL(aURI, EAclGet, MgmtTree);
			
	        TRAPD( res, session->Close());
	        if(res)
	        {
	        	delete session;
	        	return res;
	        }	        
	        delete session;
	        CleanupStack::PopAndDestroy(); // For ServerId2
	        __UHEAP_MARKEND;
  */
        	return KErrNone;
        }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::DefaultACLsToServerL(CStifItemParser& /* aItem */)
        {
        
           	TUint32 adapterId = 270495202;
			_LIT8(KRootnode,"AP/APID002");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );
		    
		    _LIT8(KServerID,"funambool");
		    TBufC8<25> serverID( KServerID );
	    	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	        
	      TInt result = session->Connect();
	    	if (KErrNone != result)
	    	{
	    			delete session;
	        	return result;
	    	}
	
		   	session->SetServerL( serverID );

			TInt error = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			if( error )
			{
				//CleanupStack::PopAndDestroy();	
				session->Close();
				return error;
			}
			
			session->DefaultACLsToServerL(rootnode);
			
			CBufBase *childList = CBufFlat::NewL(16);
		    CleanupStack::PushL(childList);
			session->GetAclL(rootnode, *childList);
		    TBuf8<77>getacl;
		    childList->Read(0, getacl);
		    childList->Reset();
		    TInt foundfun = getacl.Find(serverID);

			
		    _LIT8(KServerID1,"Name");
		    TBufC8<25> serverID1( KServerID1 );
		   	session->SetServerL( serverID1 );

			session->DefaultACLsToServerL(rootnode);
			session->GetAclL(rootnode, *childList);

		    TBuf8<62>getacl1;
		    childList->Read(0, getacl1);
		    childList->Reset();
		    TInt foundash = getacl1.Find(serverID1);

			session->Close();			
			CleanupStack::PopAndDestroy();
			
			if(foundfun == KErrNotFound || foundash == KErrNotFound  )
			{
				return KErrNotFound;
			}
			else
			{
				return KErrNone;
			}

     }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::DefaultACLsToChildrenL(CStifItemParser& /* aItem */)
        {
          	TUint32 adapterId = 270495202;
			_LIT8(KRootnode,"AP/APID002");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );
		    
		    _LIT8(KServerID,"funambool");
		    TBufC8<25> serverID( KServerID );
	    	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	        
	      TInt result = session->Connect();
	    	if (KErrNone != result)
	    	{
	    			delete session;
	        	return result;
	    	}	      
	
		   	session->SetServerL( serverID );

			TInt error = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			if( error )
			{				
				session->Close();
				return error;
			}
			
	
			_LIT8(Kacl,"Add=funambool&Get=funambool&Replace=funambool&Delete=funambool&Exec=funambool");
		    TBufC8<100> aACL( Kacl );

			TInt success = session->UpdateAclL( rootnode, aACL);
			
			if(KErrNone != success)
			{
				session->Close();	    
	        	delete session;
	        	return success;	        	
			}
			
	
	
//	EXPORT_C TInt RNSmlDMCallbackSession::DefaultACLsToChildrenL(
//	const TDesC8& aURI,CBufBase& aChildList)
		
			CBufBase *childList = CBufFlat::NewL(16);
		    CleanupStack::PushL(childList);
		    
			TInt ret = session->DefaultACLsToChildrenL(rootnode, *childList);
			if(ret)
			{
				session->Close();
				delete session;
				return ret;
			}
			//CBufBase *childList = CBufFlat::NewL(16);
		    //CleanupStack::PushL(childList);
			session->GetAclL(rootnode, *childList);
		    TBuf8<77>getacl;
		    childList->Read(0, getacl);
		    childList->Reset();
		    TInt foundfun = getacl.Find(serverID);

			
		    _LIT8(KServerID1,"Name");
		    TBufC8<25> serverID1( KServerID1 );
		   	session->SetServerL( serverID1 );
	
			_LIT8(Kacl2,"Add=Name&Get=Name&Replace=Name&Delete=Name&Exec=Name");
		    TBufC8<100> aACL2( Kacl2 );
		
			success = session->UpdateAclL( rootnode, aACL2);
			
			if(KErrNone != success)
			{
				session->Close();	    
	        	delete session;
	        	return success;	        	
			}
			
			session->DefaultACLsToChildrenL(rootnode, *childList);
			session->GetAclL(rootnode, *childList);

		    TBuf8<62>getacl1;
		    childList->Read(0, getacl1);
		    childList->Reset();
		    TInt foundash = getacl1.Find(serverID1);

			session->Close();			
			CleanupStack::PopAndDestroy();
			
			if(foundfun == KErrNotFound || foundash == KErrNotFound  )
			{
				return KErrNotFound;
			}
			else
			{
				return KErrNone;
			}

        }
// till here


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::EraseServerIdL( CStifItemParser& aItem)
        {  
	     	TUint32 adapterId = 270495202;
			_LIT8(KRootnode,"AP/APID002");
		    TBufC8<25> rootnode( KRootnode );
			_LIT8(KRootnodeLuid,"1");
		    TBufC8<25> rootnodeLuid( KRootnodeLuid );
		    
		    TPtrC string;
    		TInt err = aItem.GetNextString ( string );
    		if( err )
    		{
    			return KErrNotFound;
    		}
			HBufC8* serverId = CnvUtfConverter::ConvertFromUnicodeToUtf8L(string);
	
			CleanupStack::PushL(serverId);
        	
        	RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession;
   	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	CleanupStack::PopAndDestroy();
	        	return KErrGeneral;
	        }
	        session->SetServerL(*serverId);
	        TInt error  = session->AddMappingInfoL( adapterId, rootnode, rootnodeLuid );
			
			if( error  )
			{
		    	session->Close();
		    	delete session;
				CleanupStack::PopAndDestroy();
				return error;
			}
			CBufBase *childList = CBufFlat::NewL(16);
		    CleanupStack::PushL(childList);

		    _LIT8(KServerID1,"Name");
		    TBufC8<25> serverID1( KServerID1 );

		    _LIT8(KServerID,"funambool");
		    TBufC8<25> serverID( KServerID );


			_LIT8(Kacl,"Add=funambool&Get=funambool&Replace=Name&Delete=Name");
		    TBufC8<100> aACL( Kacl );
		    session->UpdateAclL(rootnode, aACL);
		   
			session->GetAclL(rootnode, *childList);
		    TBuf8<56>getacl;
		    //TBuf8<100>getacl;
		    childList->Read(0, getacl);
		    childList->Reset();
		    
		    TInt foundfun = getacl.Find(serverID);
		    TInt foundash = getacl.Find(serverID1);
		    
		    if( foundfun == KErrNotFound || foundash == KErrNotFound  )
		    {
		    	session->Close();
		    	delete session;
				CleanupStack::PopAndDestroy();
				CleanupStack::PopAndDestroy();
				return KErrNotFound;		    	
		    }

			session->EraseServerIdL( serverID1 );

			session->GetAclL(rootnode, *childList);
		    TBuf8<27>getacl1;
		    childList->Read(0, getacl1);
		    childList->Reset();
		    
		    foundfun = getacl1.Find(serverID);
		   	foundash = getacl1.Find(serverID1);
		   
		    if( foundfun == KErrNotFound || foundash != KErrNotFound)
		    {
				session->Close();
		    	delete session;
				CleanupStack::PopAndDestroy();
				CleanupStack::PopAndDestroy();
				return KErrNotFound;
		    	
		    }
			session->EraseServerIdL( serverID );
			session->GetAclL(rootnode, *childList);
			
			if(childList->Size())
			{		
			
			    TBuf8<18>getacl2;
			    childList->Read(0, getacl2);
			    childList->Reset();

			    foundfun = getacl2.Find(serverID);
			   	foundash = getacl2.Find(serverID1);
				
				session->Close();
			    delete session;
				CleanupStack::PopAndDestroy();
				CleanupStack::PopAndDestroy();
					
			    if( foundfun != KErrNotFound || foundash != KErrNotFound)
			    {
					return KErrNotFound;
			    	
			    }
			}
			session->Close();
			delete session;
			CleanupStack::PopAndDestroy();
			CleanupStack::PopAndDestroy();
			
			return KErrNone;
        }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::StartAtomic()
        {
 //		   	__UHEAP_MARK;
    		RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession();
	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	
			TInt ret = session->StartAtomic();
			if(KErrNone != ret)
			{
				delete session;
				return ret;
			}
	        TRAPD( res, session->Close());
	        if(res)
	        {
	        	delete session;
	        	return res;
	        }	        
    		delete session;
   // 		__UHEAP_MARKEND;
    		return KErrNone;
         }

// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::CommitAtomic()
        {
 //		   	__UHEAP_MARK;
    		RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession();
	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	
			TInt ret = session->CommitAtomic();
			if(KErrNone != ret)
			{
				delete session;
				return ret;
			}
	        TRAPD( res, session->Close());
	        if(res)
	        {
	        	delete session;
	        	return res;
	        }	        
    		delete session;
   // 		__UHEAP_MARKEND;
    		return KErrNone;
        }


// -----------------------------------------------------------------------------
// Ctreedbclientapi::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
        TInt Ctreedbclientapi::RollbackAtomic()
        {
        	 //		   	__UHEAP_MARK;
    		RNSmlDMCallbackSession* session = new RNSmlDMCallbackSession();
	        
	        TInt result = session->Connect();
	        if (KErrNone != result)
	        {
	        	delete session;
	        	return KErrGeneral;
	        }
	
			TInt ret = session->RollbackAtomic();
			if(KErrNone != ret)
			{
				delete session;
				return ret;
			}
	        TRAPD( res, session->Close());
	        if(res)
	        {
	        	delete session;
	        	return res;
	        }	        
    		delete session;
   // 		__UHEAP_MARKEND;
    		return KErrNone;      
        }

// -----------------------------------------------------------------------------
// Ctreedbclientapi::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Ctreedbclientapi::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
