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
* Description:      TestPnPUtilBlocks.cpp
*
*/






// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "TestPnpUtil.h"
#include <ETELMM.H>
#include <e32std.h>
 


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

const static TInt KMaxLength = 256;
const static TInt KMinLength = 5;


void CTestPnpUtil::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestPnpUtil::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::RunMethodL( 
    CStifItemParser& aItem ) 
    {

   TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        
        ENTRY( "PnPUtilNewL", CTestPnpUtil::PnPUtilNewL ),
        ENTRY( "PnPUtilNewLC", CTestPnpUtil::PnPUtilNewLC ),
        ENTRY( "PnPUtilVersionL", CTestPnpUtil::PnPUtilVersionL ),
        ENTRY( "PnPUtilCreateNewTokenL", CTestPnpUtil::PnPUtilCreateNewTokenL ),
        ENTRY( "PnPUtilGetTokenValidityTimeL", CTestPnpUtil::PnPUtilGetTokenValidityTimeL ),
        ENTRY( "PnPUtilGetTokenValueL", CTestPnpUtil::PnPUtilGetTokenValueL ),
        ENTRY( "PnPUtilCreateNewNonceL", CTestPnpUtil::PnPUtilCreateNewNonceL ),
        ENTRY( "PnPUtilGetNonceValidityTimeL", CTestPnpUtil::PnPUtilGetNonceValidityTimeL ),
        ENTRY( "PnPUtilGetNonceL", CTestPnpUtil::PnPUtilGetNonceL ),
        ENTRY( "PnPUtilGetKeyInfoL", CTestPnpUtil::PnPUtilGetKeyInfoL ),
        ENTRY( "PnPUtilImsiL", CTestPnpUtil::PnPUtilImsiL ),
        ENTRY( "PnPUtilFetchHomeNetworkInfoL", CTestPnpUtil::PnPUtilFetchHomeNetworkInfoL ),
        ENTRY( "PnPUtilFetchNetworkInfoL", CTestPnpUtil::PnPUtilFetchNetworkInfoL ),
        ENTRY( "GetAndSetHomeMccL", CTestPnpUtil::GetAndSetHomeMccL ),
        ENTRY( "GetAndSetHomeMncL", CTestPnpUtil::GetAndSetHomeMncL ),
	ENTRY( "GetAndSetNetworkMccL", CTestPnpUtil::GetAndSetNetworkMccL ),
        ENTRY( "GetAndSetNetworkMncL", CTestPnpUtil::GetAndSetNetworkMncL ),
        ENTRY( "PnPUtilRegisteredInHomeNetworkL", CTestPnpUtil::PnPUtilRegisteredInHomeNetworkL ),
        ENTRY( "PnPUtilOperatorLongNameL", CTestPnpUtil::PnPUtilOperatorLongNameL ),
        ENTRY( "PnPUtilStoreAccessPointL", CTestPnpUtil::PnPUtilStoreAccessPointL ),
        ENTRY( "PnPUtilFormatMncCodeL", CTestPnpUtil::PnPUtilFormatMncCodeL ),
        ENTRY( "ProvNewLC", CTestPnpUtil::ProvNewLC ),
        ENTRY( "PnpSetApplicationUidL", CTestPnpUtil::PnpSetApplicationUidL ),
        ENTRY( "PnpGetApplicationUidL", CTestPnpUtil::PnpGetApplicationUidL ),
        ENTRY( "SetProvAdapvalueL", CTestPnpUtil::SetProvAdapvalueL ),
        ENTRY( "GetProvAdapvalueL", CTestPnpUtil::GetProvAdapvalueL ),
        ENTRY( "LaunchOnlineSupportTestL", CTestPnpUtil::LaunchOnlineSupportTestL ),
        ENTRY( "ConstructUriParamL", CTestPnpUtil::ConstructUriParamL ),

               //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilNewL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//  
TInt CTestPnpUtil::PnPUtilNewL()
{
    CPnpUtilImpl* pnptest = CPnpUtilImpl::NewLC();
    CleanupStack::PopAndDestroy(pnptest);
    
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilNewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 

TInt CTestPnpUtil::PnPUtilNewLC()
{
    CPnpUtilImpl* pnptest = CPnpUtilImpl::NewLC();
    CleanupStack::PopAndDestroy(pnptest);
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilVersionL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CTestPnpUtil::PnPUtilVersionL()
{

    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TBuf<KMaxLength> bufversion;
    TInt err = pnputil -> Version(bufversion);
    
    TBuf<KMinLength> bufversionmin;
    TInt err1 = pnputil -> Version(bufversionmin);
    
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err == KErrNone && err1 == KErrArgument)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilCreateNewTokenL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CTestPnpUtil::PnPUtilCreateNewTokenL()
{


    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TInt timevalid = 0;
    TInt timeinvalid = 999999; 
    
    TInt token;
    TInt err = pnputil -> CreateNewToken(timevalid, token);
    
    TInt tokeninvalid;
    TInt err1 = pnputil -> CreateNewToken(timeinvalid, tokeninvalid);
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err == KErrNone && err1 == KErrArgument)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilGetTokenValidityTimeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
// 
TInt CTestPnpUtil::PnPUtilGetTokenValidityTimeL()
{
    
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    const TInt KTokenValidityTime = 600;
    
    TInt tokenvalidaitytime = pnputil -> GetTokenValidityTime();
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(tokenvalidaitytime == KTokenValidityTime)
    return KErrNone;
    else
    return KErrGeneral;
    
    
}


// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilGetTokenValueL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilGetTokenValueL()
{


    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TInt tokenval;
    TInt timevalid = 600;
    TInt token;
    TInt err = pnputil -> CreateNewToken(timevalid, token);
    
    err = pnputil ->GetTokenValue(tokenval);
    
    CleanupStack::PopAndDestroy(pnputil);
    
    return err;  
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilCreateNewNonceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilCreateNewNonceL()
{


    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();

    TUint32 timeout =0;
    TBuf8<KMaxLength> bufversion;
    TRAPD( err, pnputil -> CreateNewNonceL(timeout, bufversion));
    
    TBuf8<KMinLength> bufversionmin;
    TRAPD (err1 , pnputil -> CreateNewNonceL(timeout, bufversionmin));
    
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err == KErrNone && err1 == KErrArgument)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilGetNonceValidityTimeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilGetNonceValidityTimeL()
{
    
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TRAPD(err, pnputil->GetNonceValidityTimeL());
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err == KErrNotSupported)
    return KErrNone;
    else
    return KErrGeneral;
    
    
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilGetNonceL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilGetNonceL()
{


    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TBuf8<KMaxLength> nonce;
    TRAPD(err, pnputil->GetNonceL(nonce));
    
    TBuf8<KMinLength> nonce1;
    TRAPD(err1, pnputil->GetNonceL(nonce1));
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err==KErrNone && err1==KErrArgument)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilGetKeyInfoL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilGetKeyInfoL()
{

    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TBuf8<KMaxLength> key;
    TRAPD(err, pnputil->GetKeyInfoL(key));
    
    TBuf8<KMinLength> key1;
    TRAPD(err1, pnputil->GetKeyInfoL(key1));
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err == KErrNone && err1 == KErrArgument)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilImsiL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilImsiL()
{
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    RMobilePhone::TMobilePhoneSubscriberId aImsi;
    
    TRAPD(err, pnputil->ImsiL(aImsi));
    
    CleanupStack::PopAndDestroy(pnputil);
    
    return err;
    
}


// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilFetchHomeNetworkInfoL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilFetchHomeNetworkInfoL()
{
    
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TRAPD(err, pnputil->FetchHomeNetworkInfoL());
    
    CleanupStack::PopAndDestroy(pnputil);
    
    return err;
}


// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilFetchNetworkInfoL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilFetchNetworkInfoL()
{
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
        TRAPD(err, pnputil->FetchNetworkInfoL());
    
    CleanupStack::PopAndDestroy(pnputil);
    return err;
 }


// -----------------------------------------------------------------------------
// CTestPnpUtil::GetAndSetHomeMccL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::GetAndSetHomeMccL()
{
    
    
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );

    info.iCountryCode.Copy( _L("244") );
    
    TRAPD(err, pnputil->SetHomeMccL( info.iCountryCode ));
    if(err!=KErrNone)
    return err;
    
    RMobilePhone::TMobilePhoneNetworkCountryCode getmcc;
    
    TRAPD(err1, getmcc = pnputil->HomeMccL());    
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err1!=KErrNone)
    return err1;
    
    if(info.iCountryCode == getmcc)
    return KErrNone;
    else
    return KErrGeneral;
}


// -----------------------------------------------------------------------------
// CTestPnpUtil::GetAndSetHomeMncL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::GetAndSetHomeMncL()
{

    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );

    info.iNetworkId.Copy( _L("05") );
    
    TRAPD(err, pnputil->SetHomeMncL( info.iNetworkId));
    if(err!=KErrNone)
    return err;
    
    
    RMobilePhone::TMobilePhoneNetworkIdentity getmnc;
    TRAP(err, getmnc = pnputil->HomeMncL());    
    
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err!=KErrNone)
    return err;
    
    if(info.iNetworkId == getmnc)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::GetAndSetNetworkMccL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::GetAndSetNetworkMccL()
{

    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();

    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );

    info.iCountryCode.Copy( _L("244") );
    
    TRAPD(err, pnputil->SetNetworkMccL( info.iCountryCode ));
    if(err!=KErrNone)
    return err;
    
    RMobilePhone::TMobilePhoneNetworkCountryCode getmcc;
    TRAP(err, getmcc = pnputil->NetworkMccL());    
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err!=KErrNone)
    return err;
    
    if(info.iCountryCode == getmcc)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::GetAndSetNetworkMncL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::GetAndSetNetworkMncL()
{
    
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );

    info.iNetworkId.Copy( _L("05") );
    
    TRAPD(err, pnputil->SetNetworkMncL( info.iNetworkId));
    if(err!=KErrNone)
    return err;
    
    RMobilePhone::TMobilePhoneNetworkIdentity getmnc;
    TRAP(err, getmnc = pnputil->NetworkMncL());   
    
    CleanupStack::PopAndDestroy(pnputil);
     
    if(err!=KErrNone)
    return err;
    
    if(info.iNetworkId == getmnc)
    return KErrNone;
    else
    return KErrGeneral;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::GetAndSetNetworkMncL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilRegisteredInHomeNetworkL()
{

    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();

    TRAPD(err, pnputil->RegisteredInHomeNetworkL());
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err!=KErrNone)
    return err;
    
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::OperatorLongNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilOperatorLongNameL()
{
    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();

    RMobilePhone::TMobilePhoneNetworkLongName name(KNullDesC);
    TInt err = pnputil->OperatorLongName(name);
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err!=KErrNone || name.Compare(KNullDesC)!=0)
    return err;
    
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::OperatorLongNameL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilStoreAccessPointL()
{

    CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
    
    TUint32 value =0;
    TInt err = pnputil->StoreAccessPoint(value);
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err == KErrNotSupported)
    return KErrNone;
    else
    return err;
}


// -----------------------------------------------------------------------------
// CTestPnpUtil::PnPUtilFormatMncCodeL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CTestPnpUtil::PnPUtilFormatMncCodeL()
{
   CPnpUtilImpl* pnputil = CPnpUtilImpl::NewLC();
   
    RMobilePhone::TMobilePhoneNetworkInfoV1 info;
    RMobilePhone::TMobilePhoneNetworkInfoV1Pckg infoPckg( info );

    info.iCountryCode.Copy( _L("244") );
    info.iNetworkId.Copy( _L("05") );


    RMobilePhone::TMobilePhoneNetworkIdentity formattedMnc;
    TRAPD(err, pnputil->FormatMncCodeL( info.iCountryCode, info.iNetworkId, formattedMnc ));
    
    CleanupStack::PopAndDestroy(pnputil);
    
    if(err!=KErrNone)
    return err;
    
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CTestPnpUtil::ProvNewLC
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CTestPnpUtil::ProvNewLC()
    {
    CPnpProvUtil *prov = CPnpProvUtil::NewLC();
    CleanupStack::PopAndDestroy(prov);

    return KErrNone;

    }
    
    
// -----------------------------------------------------------------------------
// CTestPnpUtil::PnpSetApplicationUidL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt CTestPnpUtil::PnpSetApplicationUidL()
{
    CPnpProvUtil *prov = CPnpProvUtil::NewLC();
    TUint32 value = 0x100058C5;
    
    prov->SetApplicationUidL(value);

    CleanupStack::PopAndDestroy(prov);

    return KErrNone;
    
}


// -----------------------------------------------------------------------------
// CTestPnpUtil::PnpGetApplicationUidL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt CTestPnpUtil::PnpGetApplicationUidL()
{
    CPnpProvUtil *prov = CPnpProvUtil::NewLC();
    TUint32 value = 0x100058C5;
    TUint32 valuezero = 0;
    
    prov->SetApplicationUidL(value);

    TUint32 retvalue = prov->GetApplicationUidL();
    
    prov->SetApplicationUidL(valuezero);

    TUint32 valueretzero = prov->GetApplicationUidL();
    
    
    CleanupStack::PopAndDestroy(prov);

    if(retvalue == value && valueretzero == valuezero)
     return KErrNone;
    else
     return KErrGeneral;

}

// -----------------------------------------------------------------------------
// CTestPnpUtil::SetProvAdapvalue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//



TInt CTestPnpUtil::SetProvAdapvalueL()
{
    CPnpProvUtil *prov = CPnpProvUtil::NewLC();
    

    _LIT(KNull, "");
    TBuf<2> buf(KNull);

    prov->SetProvAdapterAppIdL(buf);
        
    _LIT(KName1, "name1");
    TBuf<KMinLength> buf1(KName1);
    prov->SetProvAdapterAppIdL(buf1);

    _LIT(KName2, "name2");
    TBuf<KMinLength> buf2(KName2);
    prov->SetProvAdapterAppIdL(buf2);

    CleanupStack::PopAndDestroy(prov);

    return KErrNone;

}


// -----------------------------------------------------------------------------
// CTestPnpUtil::GetProvAdapvalue
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt CTestPnpUtil::GetProvAdapvalueL()
{

    CPnpProvUtil *prov = CPnpProvUtil::NewLC();
    RPointerArray<HBufC> array;

    TInt flag =0;
    
    _LIT(KNull, "");
    TBuf<2> buf(KNull);

    prov->SetProvAdapterAppIdL(buf);
        
    _LIT(KName1, "name1");
    TBuf<KMinLength> buf1(KName1);
    prov->SetProvAdapterAppIdL(buf1);

    _LIT(KName2, "name2");
    TBuf<KMinLength> buf2(KName2);
    prov->SetProvAdapterAppIdL(buf2);

    prov->GetProvAdapterAppIdsL(array);

    if(array.Count() == 2)
    flag = 1;

    array.ResetAndDestroy();
    CleanupStack::PopAndDestroy(prov);

    if(flag == 1)
     return KErrNone;
    else
     return KErrGeneral;     

}


// -----------------------------------------------------------------------------
// CTestPnpUtil::LaunchOnlineSupportTestL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//


TInt CTestPnpUtil::LaunchOnlineSupportTestL()
{

    TBufC<1> Nullval(_L(""));
      
    CPnpProvUtil *prov = CPnpProvUtil::NewLC();
    TRAPD(err, prov->LaunchOnlineSupportL(Nullval, EUserTriggered, 
                     EStartInteractive,  EFalse, Nullval)); 
    CleanupStack::PopAndDestroy(prov);
    
    if(err!=KErrNone)
    return err;
      else
    return KErrNone;

}

// -----------------------------------------------------------------------------
// CTestPnpUtil::ConstructUriParamL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt CTestPnpUtil::ConstructUriParamL()
{
    TBuf<1> Nullval(_L(""));    

    CPnpProvUtil *prov = CPnpProvUtil::NewLC();
    TRAPD(err, prov->ConstructUriWithPnPUtilL(Nullval, EHelpPortal ));

    CleanupStack::PopAndDestroy(prov);
    if(err==KErrNotSupported || err == KErrNone)
    return KErrNone;
    else
    return err;
}





