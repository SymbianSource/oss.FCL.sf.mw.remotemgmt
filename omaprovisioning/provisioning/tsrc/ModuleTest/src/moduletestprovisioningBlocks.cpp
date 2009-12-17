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
 * Description:        Implementation of Module Test Automation Code for 
 *  Provisioning  
 *
 */

// INCLUDE FILES
#include <e32svr.h>
#include <e32def.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "moduletestprovisioning.h"
#include <PushMessage.h>
#include <MWPPhone.h>
#include "CWPBootstrap.h"
#include "CWPAdapter.h"
#include "ProvisioningUIDs.h"
#include <S32FILE.h>
#include <S32STOR.h>
#include <S32STD.h>
#include <CWPParameter.h>
#include <CWPCharacteristic.h>
#include <WPAdapterUtil.h>
#include "MWPBuilder.h"
#include <WPWVAdapterResource.rsg>
#include "ProvisioningInternalCRKeys.h"
#include <centralrepository.h>
#include <commdb.h>
#include <ApAccessPointItem.h>
#include <ApDataHandler.h>


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
// Cmoduletestprovisioning::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void Cmoduletestprovisioning::Delete()
    {

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::RunMethodL(CStifItemParser& aItem)
    {

    TStifFunctionInfo const
            KFunctions[] =
                {
                // Copy this line for every implemented function.
                        // First string is the function name used in TestScripter script file.
                        // Second is the actual implementation member function. 
                        // Copy this line for every implemented function.
                        // First string is the function name used in TestScripter script file.
                        // Second is the actual implementation member function. 
                        ENTRY("Example", Cmoduletestprovisioning::ExampleL),
                        ENTRY("Create",
                                Cmoduletestprovisioning::CreateandDeleteL),
                        ENTRY("CreateNewL",
                                Cmoduletestprovisioning::CreateandDeleteNewL),
                        ENTRY("Import",
                                Cmoduletestprovisioning::ImportDocumentL),
                        ENTRY("Populate", Cmoduletestprovisioning::PopulateL),
                        ENTRY("ItemCount",
                                Cmoduletestprovisioning::ItemCountL),
                        ENTRY("InternAndExtern",
                                Cmoduletestprovisioning::InternAndExternL),
                        ENTRY("Save", Cmoduletestprovisioning::SaveL),
                        ENTRY("Summary",
                                Cmoduletestprovisioning::SummaryTitleandTextL),
                        ENTRY("SetAsDefault",
                                Cmoduletestprovisioning::SetAsDefaultL),
                        ENTRY("StoreAndRestore",
                                Cmoduletestprovisioning::StoreAndRestoreL),
                        ENTRY("Details", Cmoduletestprovisioning::DetailsL),
                        ENTRY("CreateContext",
                                Cmoduletestprovisioning::CreateContextL),
                        ENTRY("DeleteContext",
                                Cmoduletestprovisioning::DeleteContextL),
                        ENTRY("ContextDataCount",
                                Cmoduletestprovisioning::ContextDataCountL),
                        ENTRY("ContextUids",
                                Cmoduletestprovisioning::ContextUidsL),
                        ENTRY("ContextName",
                                Cmoduletestprovisioning::ContextNameL),
                        ENTRY("ContextTPS",
                                Cmoduletestprovisioning::ContextTPSL),
                        ENTRY("ContextProxies",
                                Cmoduletestprovisioning::ContextProxiesL),
                        ENTRY("Context", Cmoduletestprovisioning::ContextL),
                        ENTRY("Builder", Cmoduletestprovisioning::BuilderL),
                        ENTRY("CurrentContext",
                                Cmoduletestprovisioning::CurrentContextL),
                        ENTRY("CreateparameterNewL",
                                Cmoduletestprovisioning::CreateparameterNewL),
                        ENTRY("CreateparameterNewLC",
                                Cmoduletestprovisioning::CreateparameterNewL),
                        ENTRY("CSetandGetparamID",
                                Cmoduletestprovisioning::CSetandGetparamIDL),
                        ENTRY("CSetandGetparamName",
                                Cmoduletestprovisioning::CSetandGetparamNameL),
                        ENTRY(
                                "CSetandGetparamValue",
                                Cmoduletestprovisioning::CSetandGetparamValueL),
                        ENTRY(
                                "InternAndExternparam",
                                Cmoduletestprovisioning::InternAndExternparamL),
                        ENTRY("GetparamType",
                                Cmoduletestprovisioning::GetparamTypeL),
                        ENTRY(
                                "InsertCharacteristic",
                                Cmoduletestprovisioning::InsertCharacteristicL),
                        ENTRY(
                                "InsertLinkCharacteristic",
                                Cmoduletestprovisioning::InsertLinkCharacteristicL),
                        ENTRY(
                                "SetandGetCharactername",
                                Cmoduletestprovisioning::SetandGetCharacternameL),
                        ENTRY(
                                "SetDataCharacteristic",
                                Cmoduletestprovisioning::SetDataCharacteristicL),
                        ENTRY(
                                "SetIndexDataCharacteristic",
                                Cmoduletestprovisioning::SetIndexDataCharacteristicL),
                        ENTRY(
                                "DeleteAllDataCharacteristic",
                                Cmoduletestprovisioning::DeleteAllDataCharacteristicL),
                        ENTRY(
                                "GetParamValuecharactristic",
                                Cmoduletestprovisioning::GetParamValuecharactristicL),
                        ENTRY("Accesspoint",
                                Cmoduletestprovisioning::AccesspointL),
                        ENTRY("ReadBuf", Cmoduletestprovisioning::ReadHBufCL),
                        ENTRY("CheckURI", Cmoduletestprovisioning::CheckURI),
                        ENTRY("CheckIPV6", Cmoduletestprovisioning::CheckIPV6),
                        ENTRY("CheckIPV4", Cmoduletestprovisioning::CheckIPV4),
                        ENTRY("CWPAdapter",
                                Cmoduletestprovisioning::CWPAdapterL),
                        ENTRY("DeleteContextData",
                                Cmoduletestprovisioning::DeleteContextDataL),
                        ENTRY("ContextExistsForUid",
                                Cmoduletestprovisioning::ContextExistsForUidL),
                        ENTRY("ContextExistsForTPS",
                                Cmoduletestprovisioning::ContextExistsForTPSL),
                        ENTRY("ExternparamL",
                                Cmoduletestprovisioning::ExternparamL),
                        ENTRY(
                                "RegisterContextObserver",
                                Cmoduletestprovisioning::RegisterContextObserverL),
                        ENTRY(
                                "UnRegisterContextObserver",
                                Cmoduletestprovisioning::UnRegisterContextObserverL),
        				ENTRY("SetAPDetails",
                                Cmoduletestprovisioning::SetAPDetailsL),
                        ENTRY("GetAPID", Cmoduletestprovisioning::GetAPIDL),
                };

    const TInt count = sizeof(KFunctions) / sizeof(TStifFunctionInfo);

    return RunInternalL(KFunctions, count, aItem);

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateandDeleteL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CreateandDeleteL()
    {
    CWPEngine* engine = CWPEngine::NewLC();
    CleanupStack::PopAndDestroy();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateandDeleteNewL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CreateandDeleteNewL()
    {
    CWPEngine* engine = CWPEngine::NewL();
    delete engine;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::SetUpEngineL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void Cmoduletestprovisioning::SetUpEngineL()
    {
    iEngine = CWPEngine::NewL();
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ImportDocumentL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::ImportDocumentL(CStifItemParser& aItem)
    {
    TPtrC string;
    TInt error = aItem.GetNextString(string);
    if (error != KErrNone)
        {
        return error;

        }
    HBufC8* buf8 = GetfilecontentsL(string);
    TPtr8 buf8Ptr = buf8->Des();

    CWPEngine* engine = CWPEngine::NewLC();
    TRAPD(err, engine->ImportDocumentL(buf8Ptr));

    CleanupStack::PopAndDestroy();//engine
    delete buf8;//buf

    if (err == KErrNone)
        return KErrNone;

    else
        return err;

    }
// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateandDeleteL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::PopulateL()
    {
    SetUpEngineL();
    TRAPD(err, iEngine->PopulateL());

    if (err != KErrNone)
        return err;

    else
        return KErrNone;
    }
// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ImportL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ImportL(CStifItemParser& aItem)
    {
    TPtrC string;

    TInt error = aItem.GetNextString(string);
    if (error != KErrNone)
        {
        return error;

        }

    HBufC8 *buf8 = GetfilecontentsL(string);
    TPtr8 buf8Ptr = buf8->Des();

    iEngine = CWPEngine::NewL();
    TRAPD(err, iEngine->ImportDocumentL(buf8Ptr));

    delete buf8;
    if (err != KErrNone)
        return err;
    else
        return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ItemCountL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ItemCountL(CStifItemParser& aItem)
    {
    TInt err = ImportL(aItem);

    TRAPD(err1, iEngine->PopulateL());

    if (err1 != KErrNone || err != KErrNone)
        return err1;

    TInt count = iEngine->ItemCount();

    if (count == 0)
        {

        return KErrGeneral;
        }
    else
        {
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::SaveL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::SaveL(CStifItemParser& aItem)
    {

    TInt err = ImportL(aItem);
    TBool flag = ETrue;
    TInt errSave = KErrNone;

    TRAPD(err1, iEngine->PopulateL());

    if (err1 != KErrNone || err != KErrNone)
        {
        return err1;
        }

    TInt count = iEngine->ItemCount();

    for (TInt i = 0; i < count; i++)
        {
        TRAP(errSave, iEngine->SaveL(i));
        if (errSave != KErrNone)
            {

            flag = EFalse;
            break;
            }
        }

    if (flag == EFalse)
        {

        return errSave;
        }
    else
        {
        return KErrNone;
        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::SummaryTitleandTextL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::SummaryTitleandTextL(CStifItemParser& aItem)
    {
    TInt err = ImportL(aItem);

    TRAPD(err1, iEngine->PopulateL());

    if (err1 != KErrNone || err != KErrNone)
        {
        return err1;
        }

    TInt count = iEngine->ItemCount();

    for (TInt i = 0; i < count; i++)
        {
        TBuf<30> ibuf1(iEngine->SummaryTitle(i));
        TBuf<30> ibuf2(iEngine->SummaryText(i));
        TestModuleIf().Printf(i, ibuf1, ibuf2);

        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::InternAndExternL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::InternAndExternL(CStifItemParser& aItem)
    {

    CStreamStore* store = CBufStore::NewLC(5000);
    TBuf8<1000> buf1;
    TInt err = ImportL(aItem);
    RStoreWriteStream writeStream;
    TStreamId id(writeStream.CreateLC(*store));

    iEngine->ExternalizeL(writeStream);
    writeStream.CommitL();
    CleanupStack::PopAndDestroy();

    RStoreReadStream instream;
    instream.OpenLC(*store, id);

    iEngine->InternalizeL(instream);

    CleanupStack::PopAndDestroy();//instream
    CleanupStack::PopAndDestroy();//store

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::SetAsDefaultL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::SetAsDefaultL(CStifItemParser& aItem)
    {

    TInt err = ImportL(aItem);

    TRAPD(err1, iEngine->PopulateL());

    if (err1 != KErrNone || err != KErrNone)
        {
        return err1;
        }

    TInt count = iEngine->ItemCount();

    for (TInt i = 0; i < count; i++)
        {

        TRAPD(errSave, iEngine->SaveL(i));
        if (errSave == KErrNone)
            {

            if (iEngine->CanSetAsDefault(i))
                {
                TRAPD(errdefset, iEngine->SetAsDefaultL(i));
                if (errdefset)
                    {
                    return errdefset;
                    }
                }

            }

        else
            {
            return errSave;
            }
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::StoreAndRestoreL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::StoreAndRestoreL(CStifItemParser& aItem)
    {

    TInt err = ImportL(aItem);
    CStreamStore* store = CBufStore::NewLC(5000);
    TStreamId id(iEngine->StoreL(*store));
    iEngine->RestoreL(*store, id);
    CleanupStack::PopAndDestroy();

    if (err != KErrNone)
        return err;
    else
        return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::DetailsL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::DetailsL(CStifItemParser& aItem)
    {

    TInt err = ImportL(aItem);

    TRAPD(err1, iEngine->PopulateL());

    if (err1 != KErrNone || err != KErrNone)
        {
        return err1;
        }

    MWPPairVisitor *visitor = NULL;

    TInt error = iEngine->DetailsL(0, *visitor);

    if (error == KErrNotSupported)
        return KErrNone;
    else
        return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateContextL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::CreateContextL(CStifItemParser& aItem)
    {

    TBuf<30> tps;

    _LIT(KProxy, "PROXY_TEST");
    _LIT(KTPS, "TPS");
    _LIT(KName, "TEST_CASE_CONTEXT");

    TBuf<256> proxies(KProxy);
    TBuf<256> Names(KName);
    TBuf<256> Tps(KTPS);

    iProxies->InsertL(0, proxies);

    iEngine = CWPEngine::NewL();

    TUint32 origUid1 = iEngine -> ContextExistsL(Tps);
    //TRAPD(err,iEngine -> ContextExistsL(Tps));

    if (origUid1)
        {
        TUint32 contextuid = iEngine -> ContextL(Tps);
        iEngine -> DeleteContextL(contextuid);
        }

    TUint32 origUid(iEngine -> CreateContextL(Names, Tps, *iProxies));

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::DeleteContextL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::DeleteContextL(CStifItemParser& aItem)
    {

    TUint32 origUid(ContextcreateL());

    iEngine -> DeleteContextL(origUid);

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateDataCountL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextDataCountL()
    {

    TUint32 origUid(ContextcreateL());

    TInt DataCount = iEngine->ContextDataCountL(origUid);

    iEngine -> DeleteContextL(origUid);

    if (DataCount == 0)
        return KErrNone;
    else
        return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ContextL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextL(CStifItemParser& aItem)
    {
    TBuf<30> tps;
    TPtrC string;
    TUint32 origUid, retUid;
    TBool Failed = EFalse;
    _LIT(KProxy, "PROXY_TEST");
    _LIT(KTPS, "TPS");
    _LIT(KName, "TEST_CASE_CONTEXT");
    TBuf<256> proxiesdata(KProxy);
    TBuf<256> Names(KName);
    TBuf<256> Tps(KTPS);
    CDesC16Array* proxies;
    proxies = new (ELeave) CDesC16ArrayFlat(1);
    CleanupStack::PushL(proxies);
    proxies->InsertL(0, proxiesdata);

    TInt error = aItem.GetNextString(string);
    if (error != KErrNone)
        {
        return error;

        }

    iEngine = CWPEngine::NewL();

    if (iEngine -> ContextExistsL(string))
        {
        retUid = iEngine -> ContextL(string);
        if (retUid == 0)
            Failed = ETrue;
        }
    else
        {
        origUid = iEngine -> CreateContextL(Names, Tps, *proxies);

        //origUid = ContextcreateL();

        retUid = iEngine -> ContextL(string);
        if (retUid != origUid)
            Failed = ETrue;
        }

    CleanupStack::PopAndDestroy();

    if (Failed)
        return KErrGeneral;
    else
        return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ContextcreateL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TUint32 Cmoduletestprovisioning::ContextcreateL()
    {
    TBuf<30> tps;
    TPtrC string;

    TBuf<256> proxies(KProxy);
    TBuf<256> Names(KName);
    TBuf<256> Tps(KTPS);

    iProxies->InsertL(0, proxies);

    iEngine = CWPEngine::NewL();
    if (iEngine -> ContextExistsL(Tps))
        return (iEngine -> ContextL(Tps));

    TUint32 origUid(iEngine -> CreateContextL(Names, Tps, *iProxies));

    return origUid;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateDataCountL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextUidsL()
    {

    TUint32 origUid(ContextcreateL());

    CArrayFix<TUint32>* array = iEngine->ContextUidsL();
    CleanupStack::PushL(array);

    TUint32 retrUid(array->At(0));

    iEngine -> DeleteContextL(origUid);

    CleanupStack::PopAndDestroy(); // array


    if (origUid != retrUid)
        return KErrGeneral;
    else
        return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ContextNameL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextNameL()
    {

    TUint32 origUid(ContextcreateL());
    TInt ErrorNum;

    CArrayFix<TUint32>* array = iEngine->ContextUidsL();
    CleanupStack::PushL(array);

    TUint32 retrUid(array->At(0));

    HBufC* name = iEngine->ContextNameL(retrUid);
    CleanupStack::PushL(name);

    if (*name == KName)
        ErrorNum = KErrNone;
    else
        ErrorNum = KErrGeneral;

    CleanupStack::PopAndDestroy(); //  name

    iEngine -> DeleteContextL(origUid);

    CleanupStack::PopAndDestroy(); // array


    return ErrorNum;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateTPSL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextTPSL()
    {

    TUint32 origUid(ContextcreateL());
    TInt ErrorNum;

    CArrayFix<TUint32>* array = iEngine->ContextUidsL();
    CleanupStack::PushL(array);

    TUint32 retrUid(array->At(0));

    HBufC* tps = iEngine->ContextTPSL(retrUid);
    CleanupStack::PushL(tps);

    if (*tps == KTPS)
        ErrorNum = KErrNone;
    else
        ErrorNum = KErrGeneral;

    CleanupStack::PopAndDestroy(); //  name

    iEngine -> DeleteContextL(origUid);

    CleanupStack::PopAndDestroy(); // array


    return ErrorNum;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ContextProxiesL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextProxiesL()
    {

    TUint32 origUid(ContextcreateL());
    TInt ErrorNum;

    CArrayFix<TUint32>* array = iEngine->ContextUidsL();
    CleanupStack::PushL(array);

    TUint32 retrUid(array->At(0));

    CDesCArray *proxies = iEngine->ContextProxiesL(retrUid);
    CleanupStack::PushL(proxies);

    if (proxies->MdcaPoint(0) == KProxy)
        ErrorNum = KErrNone;
    else
        ErrorNum = KErrGeneral;

    CleanupStack::PopAndDestroy(); //  name

    iEngine -> DeleteContextL(origUid);

    CleanupStack::PopAndDestroy(); // array

    return ErrorNum;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::GetfilecontentsL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* Cmoduletestprovisioning::GetfilecontentsL(TPtrC aString)
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    RFile file;

    TInt err = file.Open(fs, aString, EFileShareAny);
    if (err != KErrNone)
        {
        CleanupStack::PopAndDestroy();//fs
        User::LeaveIfError(err);
        }

    CleanupClosePushL(file);
    TInt size;
    file.Size(size);
    HBufC8 *buf8 = HBufC8::NewLC(size);
    TPtr8 buf8Ptr = buf8->Des();
    file.Read(buf8Ptr);
    CleanupStack::Pop(buf8);
    CleanupStack::PopAndDestroy();//file
    CleanupStack::PopAndDestroy();//fs
    return buf8;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::ExampleL(CStifItemParser& aItem)
    {

    // Print to UI
    _LIT(Ktestengineapis, "testengineapis");
    _LIT(KExample, "In Example");
    TestModuleIf().Printf(0, Ktestengineapis, KExample);
    // Print to log file
    iLog->Log(KExample);

    TInt i = 0;
    TPtrC string;
    _LIT(KParam, "Param[%i]: %S");
    while (aItem.GetNextString(string) == KErrNone)
        {
        TestModuleIf().Printf(i, Ktestengineapis, KParam, i, &string);
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::BuilderL()
    {
    MWPBuilder* builder;
    CWPEngine* engine = CWPEngine::NewLC();
    *builder = engine -> Builder();
    CleanupStack::PopAndDestroy();//fs
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CurrentContextL()
    {
    TUint32 contextuid = ContextcreateL();

    iEngine->SetCurrentContextL(contextuid);

    TUint32 retUid = iEngine->CurrentContextL();

    if (contextuid == retUid)
        {
        return KErrNone;
        }
    else
        {
        return KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateparameterNewL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CreateparameterNewL()
    {
    CWPParameter* param = CWPParameter::NewL();
    delete param;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateparameterNewL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CreateparameterNewLC()
    {
    CWPParameter* param = CWPParameter::NewLC();
    CleanupStack::PopAndDestroy();
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamID
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CSetandGetparamIDL()
    {
    CWPParameter* param = CWPParameter::NewLC();
    TInt aID = 10;
    param -> SetID(aID);
    TInt getID = param ->ID();
    if (aID != getID)
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;

        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CSetandGetparamNameL()
    {
    CWPParameter* param = CWPParameter::NewLC();
    _LIT(KParamName, "name");
    TBufC<10> paramname(KParamName);
    param -> SetNameL(paramname);
    TBufC<10> getparamname;
    getparamname = param -> Name();

    CWPParameter* param1 = CWPParameter::NewLC();

    TBufC<10> getparamname1;
    getparamname1 = param1 -> Name();

    if (getparamname.Compare(KParamName) && getparamname1.Compare(KNullDesC))
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        return KErrNone;

        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CSetandGetparamValueL()
    {
    CWPParameter* param = CWPParameter::NewLC();
    _LIT(KParamValue, "Value");
    TBufC<10> paramvalue(KParamValue);
    param -> SetValueL(paramvalue);
    TBufC<10> getparamvalue;
    getparamvalue = param -> Value();

    CWPParameter* param1 = CWPParameter::NewLC();

    TBufC<10> getparamvalue1;
    getparamvalue1 = param1 -> Value();

    if (getparamvalue.Compare(KParamValue) && getparamvalue1.Compare(
            KNullDesC))
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        return KErrNone;

        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::InternAndExternparamL()
    {

    CStreamStore* store = CBufStore::NewLC(5000);
    RStoreWriteStream aStream;
    TStreamId id(aStream.CreateLC(*store));

    TInt aID = 10;
    _LIT(KParamName, "name");
    _LIT(KParamValue, "Value");
    TBufC<10> paramname(KParamName);
    TBufC<10> paramvalue(KParamValue);
    const TInt KBitsPerWord = 16;

    HBufC* pname = paramname.AllocL();
    HBufC* pvalue = paramvalue.AllocL();

    //RWriteStream aStream;

    aStream.WriteInt32L(aID);

    // Lengths packed into one 32-bit word for compatibility
    TUint length(0);
    if (pvalue)
        {
        length += pvalue->Length();
        }
    if (pname)
        {
        length += pname->Length() << KBitsPerWord;
        }

    aStream.WriteUint32L(length);

    aStream << *pvalue;

    if (pname)
        {
        aStream << *pname;
        }

    aStream.CommitL();
    CleanupStack::PopAndDestroy();

    RStoreReadStream aReadstream;

    aReadstream.OpenLC(*store, id);
    CWPParameter* param = CWPParameter::NewLC();

    //param->ExternalizeL( aStream );

    //RReadStream aReadstream;
    param -> InternalizeL(aReadstream);

    TBufC<10> getparamvalue;
    getparamvalue = param -> Value();

    TBufC<10> getparamname;
    getparamname = param -> Name();

    TInt getID = param ->ID();

    if (getparamname.Compare(KParamName)
            && getparamvalue.Compare(KParamValue) && getID != aID)
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        delete pname;
        delete pvalue;
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        delete pname;
        delete pvalue;
        return KErrNone;
        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::GetparamTypeL()
    {
    CWPParameter* param = CWPParameter::NewLC();
    TInt type = param->Type();
    if (type == 1)
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::InsertCharacteristicL()
    {

    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);
    CWPParameter* param = CWPParameter::NewL();
    TRAPD(err, characteristic -> InsertL(param));
    if (err)
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::InsertLinkCharacteristicL()
    {

    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);
    CWPCharacteristic* element = CWPCharacteristic::NewL(KWPPxLogical);
    TRAPD(err, characteristic -> InsertLinkL(*element));
    if (err)
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;
        }
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::SetandGetCharacternameL()
    {

    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);
    _LIT(KCharacterName, "name");
    TBufC<10> charactername(KCharacterName);
    characteristic -> SetNameL(charactername);
    TBufC<10> getcharactermname;
    getcharactermname = characteristic -> Name();

    if (getcharactermname.Compare(KCharacterName))
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;

        }
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::SetDataCharacteristicL()
    {
    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);
    _LIT8(KCharacterdata, "data");
    TBufC8<10> characterdata(KCharacterdata);
    characteristic -> SetDataL(characterdata);
    TBufC8<10> getcharactermdata;
    getcharactermdata = characteristic -> Data();

    if (getcharactermdata.Compare(KCharacterdata))
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;

        }
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::SetIndexDataCharacteristicL()
    {

    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);
    _LIT8(KCharacterdata1, "data");
    _LIT8(KCharacterdata2, "Ashwin");
    _LIT8(KCharacterdata3, "Harsha");
    TBufC8<10> characterdata1(KCharacterdata1);
    characteristic -> SetDataL(characterdata1);

    TBufC8<10> characterdata2(KCharacterdata2);
    characteristic -> SetDataL(characterdata2, 1);

    TBufC8<10> characterdata3(KCharacterdata3);
    characteristic -> SetDataL(characterdata3, 2);

    TBufC8<10> getcharactermdata1;
    getcharactermdata1 = characteristic -> Data();

    TBufC8<10> getcharactermdata2;
    getcharactermdata2 = characteristic -> Data(1);

    TBufC8<10> getcharactermdata3;
    getcharactermdata3 = characteristic -> Data(2);

    if (getcharactermdata1.Compare(KCharacterdata1)
            && getcharactermdata2.Compare(KCharacterdata2)
            && getcharactermdata3.Compare(KCharacterdata3))
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;

        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::DeleteAllDataCharacteristicL()
    {

    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);
    _LIT8(KCharacterdata1, "data");
    _LIT8(KCharacterdata2, "Ashwin");
    _LIT8(KCharacterdata3, "Harsha");
    TBufC8<10> characterdata1(KCharacterdata1);
    characteristic -> SetDataL(characterdata1);

    TBufC8<10> characterdata2(KCharacterdata2);
    characteristic -> SetDataL(characterdata2, 1);

    TBufC8<10> characterdata3(KCharacterdata3);
    characteristic -> SetDataL(characterdata3, 2);

    characteristic -> DeleteAllData();

    TBufC8<10> getcharactermdata1;
    getcharactermdata1 = characteristic -> Data();

    TBufC8<10> getcharactermdata2;
    getcharactermdata2 = characteristic -> Data(1);

    TBufC8<10> getcharactermdata3;
    getcharactermdata3 = characteristic -> Data(2);

    if (getcharactermdata1.Compare(KNullDesC8) && getcharactermdata2.Compare(
            KNullDesC8) && getcharactermdata3.Compare(KNullDesC8))
        {
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        return KErrNone;

        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::GetParamValuecharactristicL()
    {
    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);

    CWPParameter* param = CWPParameter::NewL();
    TInt aID = 10;
    param -> SetID(aID);
    _LIT(KParamValue, "Value");
    TBufC<10> paramvalue(KParamValue);
    param -> SetValueL(paramvalue);

    characteristic -> InsertL(param);
    CArrayFix<TPtrC>* aParameters = new (ELeave) CArrayFixFlat<TPtrC> (1);
    CleanupStack::PushL(aParameters);

    characteristic->ParameterL(aID, aParameters);

    if (aParameters->At(0) == paramvalue)
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        return KErrNone;
        }
    else
        {
        CleanupStack::PopAndDestroy();
        CleanupStack::PopAndDestroy();
        return KErrGeneral;
        }
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CSetandGetparamName
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::AccesspointL()
    {
    CWPCharacteristic* characteristic = CWPCharacteristic::NewLC(KWPNapDef);
    CWPCharacteristic* characteristic1 = WPAdapterUtil::AccesspointL(
            *characteristic);

    CleanupStack::PopAndDestroy();
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ReadBufC
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::ReadHBufCL(CStifItemParser& aItem)
    {

#if ( defined (__WINS__) || defined (__WINSCW) ) // this different on hw
    _LIT( KWVAdapterName, "WPWVAdapterResource" );
#else
    _LIT(KWVAdapterName, "WPWVAdapter");
#endif

    _LIT(KDLLName, "Z:\\sys\\bin\\WPWVAdapter.dll");
    TFileName fileName(KDLLName);

    HBufC* text = WPAdapterUtil::ReadHBufCL(fileName, KWVAdapterName,
            R_QTN_SM_IM_SERVER_DNAME);

    if (text == NULL)
        return KErrGeneral;
    else
        return KErrNone;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CheckURI
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CheckURI(CStifItemParser& aItem)
    {

    _LIT(KURI, "http://123.34.56.67/");
    TBuf<30> buf(KURI);
    TPtrC string;
    TBool ValidURI;

    ValidURI = WPAdapterUtil::CheckURI(buf);

    if (ValidURI)
        return KErrNone;
    else
        return KErrGeneral;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CheckIPV6
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::CheckIPV6(CStifItemParser& aItem)
    {

    TPtrC string;
    TBool ValidIPV6;
    TInt error = aItem.GetNextString(string);
    if (error != KErrNone)
        {
        return error;
        }

    ValidIPV6 = WPAdapterUtil::CheckIPv6(string);

    if (ValidIPV6)
        return KErrNone;
    else
        return KErrGeneral;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CheckIPV4
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::CheckIPV4(CStifItemParser& aItem)
    {

    TPtrC string;
    TBool ValidIPV4;
    TInt error = aItem.GetNextString(string);
    if (error != KErrNone)
        {
        return error;
        }

    ValidIPV4 = WPAdapterUtil::CheckIPv4(string);

    if (ValidIPV4)
        return KErrNone;
    else
        return KErrGeneral;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CWPAdapterL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CWPAdapterL()
    {
    TBool Success = ETrue;

    CWPEngine* engine = CWPEngine::NewLC();
    engine->PopulateL();
   	CWPAdapter *ladapter = NULL;
   	
   	
   	ladapter = CWPAdapter::NewL(TUid::Uid(0x101F84D9));
    
   	if(ladapter==NULL)
    	Success=EFalse;
   	
   	delete ladapter;   
    CleanupStack::PopAndDestroy();//engine
    if (Success)
        return KErrNone;
    else
        return KErrGeneral;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::DeleteContextDataL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::DeleteContextDataL()
    {

    TUint32 origUid(ContextcreateL());

    TInt Data = iEngine->DeleteContextDataL(origUid);

    iEngine -> DeleteContextL(origUid);
                                   
               

    if (Data == 0)
        return KErrNone;
    else
        return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ContextExistsForUidL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextExistsForUidL()
    {

    TUint32 origUid(ContextcreateL());

    TBool Exists = iEngine->ContextExistsL(origUid);

    iEngine -> DeleteContextL(origUid);

               

    if (Exists == EFalse)
        return KErrGeneral;
    else
        return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ContextExistsForTPSL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ContextExistsForTPSL(CStifItemParser& aItem)
    {

    TBuf<45> buf(23);
    TPtrC string;
    TInt error = aItem.GetNextString(string);
    if (error != KErrNone)
        {
        return error;

        }

    TUint32 origUid(ContextcreateL());

    TBool Exists = iEngine->ContextExistsL(string);

    iEngine -> DeleteContextL(origUid);

               

    if (Exists == EFalse)
        return KErrGeneral;
    else
        return KErrNone;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::ExternparamL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

TInt Cmoduletestprovisioning::ExternparamL()
    {

    // CONSTANTS
    /// Number of bits in half-word
    const TInt KBitsPerWord = 16;
    /// Mask to extract a half-word
    const TInt KWordMask = 0xffff;
    CWPParameter* param = CWPParameter::NewLC();
    TInt aID = 10;
    param -> SetID(aID);

    _LIT(KParamName, "name");
    TBufC<10> paramname(KParamName);
    param -> SetNameL(paramname);

    _LIT(KParamValue, "Value");
    TBufC<10> paramvalue(KParamValue);
    param -> SetValueL(paramvalue);

    CStreamStore* store = CBufStore::NewLC(5000);
    RStoreWriteStream aStream;
    TStreamId id(aStream.CreateLC(*store));

    param->ExternalizeL(aStream);

    CleanupStack::PopAndDestroy();

    RStoreReadStream aStream1;

    aStream1.OpenLC(*store, id);

    TInt parameterID(aStream1.ReadInt32L());
    TUint length(aStream1.ReadInt32L());
    // Lengths packed into one 32-bit word for compatibility
    TInt valueLength(length & KWordMask);
    TInt nameLength((length >> KBitsPerWord) & KWordMask);
    HBufC* value = HBufC::NewL(aStream1, valueLength);
    TPtr ptrvalue = value->Des();

    HBufC* name = NULL;
    if (nameLength > 0)
        {
        name = HBufC::NewL(aStream1, nameLength);
        }

    TPtr ptrname = name->Des();

    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy();

    if (ptrname.Compare(KParamName) && ptrvalue.Compare(KParamValue)
            && parameterID != aID)
        {
        delete value;
        delete name;
        return KErrGeneral;
        }
    else
        {
        delete value;
        delete name;
        return KErrNone;
        }

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::SetUpContextObserver
// Create an object of Context Observer
// -----------------------------------------------------------------------------
//
void Cmoduletestprovisioning::SetUpContextObserverL(TInt data)
    {
    iObserver = CWPContextObserver::NewL(data);
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::RegisterContextObserver
// Register Context Observer
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::RegisterContextObserverL()
    {
    TInt err = KErrNone;
    SetUpContextObserverL(1);
    SetUpEngineL();
    TRAP(err, iEngine->RegisterContextObserverL(iObserver));
    return err;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::UnRegisterContextObserver
// Unregister Context Observer
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::UnRegisterContextObserverL()
    {
    TInt err = KErrNone;

    SetUpContextObserverL(1);
    SetUpEngineL();
    TRAP(err, iEngine->RegisterContextObserverL(iObserver));
    if (err == KErrNone)
        {
        err = iEngine->UnregisterContextObserver(iObserver);
        }

    return err;
    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::CreateSetAPL
// Create and Set AP
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::CreateSetAPL(TPtrC& aOrig)
    {
    TInt err = KErrNone;
    CRepository* rep = NULL;

    rep = CRepository::NewL(KCRUidOMAProvisioningLV);
    CleanupStack::PushL(rep);

    err = rep->Set(KOMAProvOriginatorContent, aOrig);

    if (err == KErrNone)
        {
        TInt wapId = 0;

        CApAccessPointItem* newItem = CApAccessPointItem::NewLC();

        CCommsDatabase* commsDb = CCommsDatabase::NewL();
        CleanupStack::PushL(commsDb);

        CApDataHandler* lAPHandler = CApDataHandler::NewLC(*commsDb);
        wapId = lAPHandler->CreateFromDataL(*newItem);

        WPAdapterUtil::SetAPDetailsL(wapId);
        CleanupStack::PopAndDestroy(3); // lAPHandler, commsDb, newItem
        }

    CleanupStack::PopAndDestroy(); //rep

    return err;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::SetAPDetails
// Set AP details
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::SetAPDetailsL(CStifItemParser& aItem)
    {
    TInt err = KErrNone;
    TPtrC orig;

    err = aItem.GetNextString(orig);

    if (err == KErrNone)
        {
        CreateSetAPL(orig);
        }
    return err;

    }

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::GetAPIDL
// Get AP ID from DB or SNAP
// -----------------------------------------------------------------------------
//
TInt Cmoduletestprovisioning::GetAPIDL(CStifItemParser& aItem)
    {
    TInt err = KErrNone;
    TPtrC orig;

    err = aItem.GetNextString(orig);

    if (err == KErrNone)
        {
        CreateSetAPL(orig);
        }
    
    WPAdapterUtil::GetAPIDL();

    return err;
    }
    

// -----------------------------------------------------------------------------
// Cmoduletestprovisioning::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
 TInt Cmoduletestprovisioning::?member_function(
 CItemParser& aItem )
 {

 ?code

 }
 */

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  End of File
