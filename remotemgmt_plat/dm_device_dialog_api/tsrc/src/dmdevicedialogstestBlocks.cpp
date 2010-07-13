/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: This file contains testclass implementation.
*
*/

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <StifTestInterface.h>
#include "dmdevicedialogstest.h"
#include <dmdevdialogclient.h>
#include <nsmlprivateapi.h>
#include <centralrepository.h>
#include <devmaninternalcrkeys.h>
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
// Cdmdevicedialogstest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cdmdevicedialogstest::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// Cdmdevicedialogstest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cdmdevicedialogstest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Example", Cdmdevicedialogstest::ExampleL ),
        ENTRY( "ShowOnlyPkgZeroNoteL", Cdmdevicedialogstest::ShowOnlyPkgZeroNoteL ),
        ENTRY("ShowPkgZeroandConnectNoteL",Cdmdevicedialogstest::ShowPkgZeroandConnectNoteL),
        ENTRY("DismissConnectNoteL",Cdmdevicedialogstest::DismissConnectNoteL),
        ENTRY("ShowConnectNoteAgainL",Cdmdevicedialogstest::ShowConnectNoteAgainL),
        ENTRY("ShowDisplayAlertL",Cdmdevicedialogstest::ShowDisplayAlertL),
        ENTRY("ShowConfirmationAlertL",Cdmdevicedialogstest::ShowConfirmationAlertL),
        ENTRY("CancelPkgZeroNoteL",Cdmdevicedialogstest::CancelPkgZeroNoteL),
        ENTRY("SilentPkgZeroL",Cdmdevicedialogstest::SilentPkgZeroL),
                ENTRY("DefaultProfilePkgZeroL",Cdmdevicedialogstest::DefaultProfilePkgZeroL),
        
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// Cdmdevicedialogstest::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cdmdevicedialogstest::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
/*    _LIT( Kdmdevicedialogstest, "dmdevicedialogstest" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, Kdmdevicedialogstest, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, Kdmdevicedialogstest, 
                                KParam, i, &string );
        i++;
        }*/

    RDmDevDialog DmDevdialog;
    TInt err = DmDevdialog.OpenL();
    if(err)
        return err;
    DmDevdialog.Close();
    return KErrNone;

    }

TInt Cdmdevicedialogstest::ShowOnlyPkgZeroNoteL( CStifItemParser& aItem )
    {
    RDebug::Print(_L("Cdmdevicedialogstest::ShowOnlyPkgZeroNoteL"));
    RDmDevDialog DmDevdialog;
    
    TInt err = DmDevdialog.OpenL();
    RDebug::Print(_L("Cdmdevicedialogstest::ShowOnlyPkgZeroNoteL after openL"));
    if (err)
        return err;
    TRequestStatus status = KRequestPending;
    TInt ProfileId = 1000001;
    TInt Jobid = 0;
    TInt uimode = 0;
    //TBuf<100> name(_L("Server name 123"));
    TPckgBuf<TInt> iResBuf;
    DmDevdialog.LaunchPkgZero(ProfileId, Jobid, uimode, iResBuf, status);
    User::After(1000000 * 6);
    //User::WaitForRequest(status); //working
    err = DmDevdialog.CancelPkgZeroNote();
    //Different tests to be performed
    //Close immediately
    DmDevdialog.Close(); //See what happens to server whether it is closed or not
    //Close later
    /*if (status.Int() == KErrNone || status.Int() == KErrCancel)
        return KErrNone;
    else
        return KErrNotFound;*/
    return err;
    }
TInt Cdmdevicedialogstest::SilentPkgZeroL( CStifItemParser& aItem )
    {
    RDebug::Print(_L("Cdmdevicedialogstest::SilentPkgZeroL"));
    RDmDevDialog DmDevdialog;
    
    TInt err = DmDevdialog.OpenL();
    RDebug::Print(_L("Cdmdevicedialogstest::SilentPkgZeroL after openL"));
    if (err)
        return err;
    TRequestStatus status = KRequestPending;
    TInt ProfileId = 1000001;
    TInt Jobid = 0;
    TInt uimode = 1;
    TBuf<100> name(_L("Silent server"));
    TPckgBuf<TInt> iResBuf;
    DmDevdialog.LaunchPkgZero(ProfileId, Jobid, uimode, iResBuf, status);
   // User::After(1000000 * 6);
    User::WaitForRequest(status); //working
    //err = DmDevdialog.CancelPkgZeroNote();
    //Different tests to be performed
    //Close immediately
    DmDevdialog.Close(); //See what happens to server whether it is closed or not
    //Close later
    if (status.Int() == KErrNone || status.Int() == KErrCancel)
        return KErrNone;
    else
        return KErrNotFound;
    return err;
    }

TInt Cdmdevicedialogstest::DefaultProfilePkgZeroL( CStifItemParser& aItem )
    {
    RDebug::Print(_L("Cdmdevicedialogstest::DefaultProfilePkgZeroL"));
    // Device manager key UID
    const TUid KCRUidNSmlDMSyncApp = {0x101f6de5};
    // CenRep keys for default dm profile used in the
    // firmware update over the air.
    const TUint32 KNSmlDMDefaultFotaProfileKey = 0x00000002;
    TInt ProfileId = 1000001;
    CRepository* centrep = NULL;
    TRAPD( err, centrep = CRepository::NewL( KCRUidNSmlDMSyncApp ) );
    RDebug::Print(_L("Cdmdevicedialogstest::DefaultProfilePkgZeroL cenrep created"));
    User::LeaveIfError( err );
    RDebug::Print(_L("Cdmdevicedialogstest::DefaultProfilePkgZeroL cenrep fine"));
    centrep->Set( KNSmlDMDefaultFotaProfileKey, ProfileId );
    RDebug::Print(_L("Cdmdevicedialogstest::DefaultProfilePkgZeroL cenrep set fine"));
    delete centrep;
    
    RDmDevDialog DmDevdialog;
    
    err = DmDevdialog.OpenL();
    RDebug::Print(_L("Cdmdevicedialogstest::DefaultProfilePkgZeroL after openL"));
    if (err)
        return err;
    TRequestStatus status = KRequestPending;
    
    TInt Jobid = 0;
    TInt uimode = 0;
    TBuf<100> name(_L("non silent def"));
   TPckgBuf<TInt> iResBuf;
    DmDevdialog.LaunchPkgZero(ProfileId, Jobid, uimode, iResBuf, status);
   // User::After(1000000 * 6);
    User::WaitForRequest(status); //working
    //err = DmDevdialog.CancelPkgZeroNote();
    //Different tests to be performed
    //Close immediately
    DmDevdialog.Close(); //See what happens to server whether it is closed or not
    //Close later
    if (status.Int() == KErrNone || status.Int() == KErrCancel)
        return KErrNone;
    else
        return KErrNotFound;
    return err;
    }

TInt Cdmdevicedialogstest::ShowPkgZeroandConnectNoteL( CStifItemParser& aItem )
{
RNSmlPrivateAPI privateApi;
    TPtrC8 messageBody(_L8("test"));
    
                TRAPD(openErr, privateApi.OpenL());
                
                if ( openErr != KErrNone )
                    {
                    return openErr;
                    }
                    
                TRAPD(err,privateApi.SendL( messageBody, ESmlDevMan, ESmlVersion1_2 ));                
                privateApi.Close();
                return err;
}

TInt Cdmdevicedialogstest::ShowConnectNoteAgainL( CStifItemParser& aItem )
    {

    RDebug::Print(_L("Cdmdevicedialogstest::ShowConnectNoteAgainL"));
        RDmDevDialog DmDevdialog;
        
        TInt err = DmDevdialog.OpenL();
        RDebug::Print(_L("Cdmdevicedialogstest::ShowConnectNoteAgainL after openL"));
        if (err)
            return err;
        TInt Status(KErrNotFound);
       err = DmDevdialog.ShowConnectDialog();
      
           DmDevdialog.Close();
           return err;                 
    }

TInt Cdmdevicedialogstest::CancelPkgZeroNoteL( CStifItemParser& aItem )
    {

    RDebug::Print(_L("Cdmdevicedialogstest::CancelPkgZeroNoteL"));
        RDmDevDialog DmDevdialog;
        
        TInt err = DmDevdialog.OpenL();
        RDebug::Print(_L("Cdmdevicedialogstest::CancelPkgZeroNoteL after openL"));
        if (err)
            return err;
        TInt Status(KErrNotFound);
       err = DmDevdialog.CancelPkgZeroNote();
      
           DmDevdialog.Close();
           return err;                 
    }
TInt Cdmdevicedialogstest::DismissConnectNoteL( CStifItemParser& aItem )
    {
    RDebug::Print(_L("Cdmdevicedialogstest::DismissConnectNoteL"));
        RDmDevDialog DmDevdialog;
        
        TInt err = DmDevdialog.OpenL();
        RDebug::Print(_L("Cdmdevicedialogstest::ShowOnlyPkgZeroNoteL after openL"));
        if (err)
            return err;
        TInt Status(KErrNotFound);
       err = DmDevdialog.IsPkgZeroConnectNoteShown(Status);
       if (err)
           {
           DmDevdialog.Close();
           return err;
           }
       if(Status == 1)
           {
        err = DmDevdialog.DismissConnectDialog();
       /* if (err)
            {
            DmDevdialog.Close();
            return err;
            }*/
        }
       else
           return KErrNone;
        //Different tests to be performed
        //Close immediately
        DmDevdialog.Close(); //See what happens to server whether it is closed or not
        return err;
        //Close later
        
    }
TInt Cdmdevicedialogstest::IsConnectNoteShownL( CStifItemParser& aItem )
    {
    return KErrNone;
    }
TInt Cdmdevicedialogstest::ShowDisplayAlertL( CStifItemParser& aItem )
    {
    RDmDevDialog DmDevdialog;
    TInt err = DmDevdialog.OpenL();
    if(err)
        return err;
    
    TRequestStatus status = KRequestPending;
TBuf<100> ServerMsg(_L("Information alert...."));
    DmDevdialog.ShowDisplayAlert(ServerMsg,status);

  
    User::WaitForRequest(status);

DmDevdialog.Close();
    return KErrNone;
    }
TInt Cdmdevicedialogstest::ShowConfirmationAlertL( CStifItemParser& aItem )
    {
    RDmDevDialog DmDevdialog;
    TInt err = DmDevdialog.OpenL();
    if(err)
        return err;
    
    TRequestStatus status = KRequestPending;
TBuf<100> ServerMsg(_L("Confirmation alert...."));

    TInt timeout = 30; // dummy
    TBuf<30> header; // dummy
    DmDevdialog.ShowConfirmationAlert(timeout,header,ServerMsg,status);
       
   
    User::WaitForRequest(status);

DmDevdialog.Close();
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// Cdmdevicedialogstest::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt Cdmdevicedialogstest::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
