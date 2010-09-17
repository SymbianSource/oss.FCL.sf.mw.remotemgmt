/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  provides interface for application to set UID and 
 *                provisioning adapters to set the settings application ID.
 *                PNPMS client get these values before activating service.
 *                This class can be used for launching online support in different modes
 *
*/


#include <PnpProvUtil.h>
#include <centralrepository.h>
#include "PnpUtilPrivateCRKeys.h"
#include <apgcli.h>
#include <apacmdln.h>

_LIT( KSpace," ");
_LIT( KQuote, "\"" );
_LIT( KServerParam, "s" );
_LIT( KPageParam, "p" );
_LIT( KReasonParam, "r" );
_LIT( KQueryParam, "q" );
_LIT( KAutoStartParam, "a" );
_LIT( KTokenOverrideParam, "t" );
_LIT( KServerPrefix, "www");
_LIT( KPageString, "page1");
_LIT( KHelpquery, "query");

const TUid KServiceHelpUID = { 0x10204338 };

const TInt KBufferMaxLength = 128; 
const TInt KBufferMinLength = 2;


// -----------------------------------------------------------------------------
// CPnpProvUtil::CPnpProvUtil
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPnpProvUtil::CPnpProvUtil()
    {
    }

// -----------------------------------------------------------------------------
// CPnpProvUtil::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPnpProvUtil::ConstructL()
    {
    //No values to assign	
    }

// -----------------------------------------------------------------------------
// CPnpProvUtil::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPnpProvUtil* CPnpProvUtil::NewL()
    {

    CPnpProvUtil* self = NewLC();
    CleanupStack::Pop();

    return self; 
    }

// -----------------------------------------------------------------------------
// CPnpProvUtil::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPnpProvUtil* CPnpProvUtil::NewLC()
    {
    CPnpProvUtil* self = new( ELeave ) CPnpProvUtil;

    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CPnpProvUtil::~CPnpProvUtil()
    {   
    }

// -----------------------------------------------------------------------------
// SetLaunchedApplicationDetailsL
// -----------------------------------------------------------------------------
EXPORT_C void CPnpProvUtil::SetApplicationUidL(TUint32 aUid)
    {
    // Set UID of application into Cenrep Key for identifying the application
    // that triggered PNPMS client for service activation

    TBuf<50> uidval;

    // In case of resetting the cenrep value 	 
    if(aUid == 0)
        uidval= KNullDesC;
    else	
        uidval.Num(aUid, EHex);

    SetCenrepValueL(KCRUidPnpUtil,KPnPProvUtilAppUID, uidval);
    }

// -----------------------------------------------------------------------------
// GetLaunchedApplicationDetailsL
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CPnpProvUtil::GetApplicationUidL()
    {
    // Get UID of application from Cenrep set by SetApplicationUidL
    // Return application UID to PNPMS client for service activation

    TUint32 uidval = 0; 

    TBuf<50> buf;
    GetCenrepValueL(KCRUidPnpUtil,KPnPProvUtilAppUID,buf);

    // If Cenrep value is empty then return UID with 0

    if( buf.Compare(KNullDesC) != 0 ) 
        {
        TLex lex(buf);
        lex.Val(uidval, EHex);

        }

    return uidval;

    }

// -----------------------------------------------------------------------------
// SetProvAdapterAppIdL
// -----------------------------------------------------------------------------
EXPORT_C void CPnpProvUtil::SetProvAdapterAppIdL(TDesC& appID)
    {	

    // This function set Provisioning adapter Application ID
    // for example: Email Provisioning settings: 110,143 


    TBuf<KBufferMaxLength> getValue;
    GetCenrepValueL(KCRUidPnpUtil,KPnPProvUtilProvAdapID, getValue );

    // Passing Null reference indicates clearing the Provisioning application
    // ID keys

    if(appID.Compare(_L(""))==0)
        SetCenrepValueL(KCRUidPnpUtil,KPnPProvUtilProvAdapID, appID );
    else
        {
        // If same adapter requests to set more than one application ID then
        // new application ID will be appended to existing one 
        // for example: Email app can have more than one app ID(POP3 and IMPA4)

        if(getValue.Compare(_L("")))
            {
            getValue.Append(_L(","));
            getValue.Append(appID);	
            }
        else	
            {
            getValue.Append(appID); 
            }

        SetCenrepValueL(KCRUidPnpUtil,KPnPProvUtilProvAdapID, getValue );
        }



    }

// -----------------------------------------------------------------------------
// GetProvAdapterAppIdsL
// -----------------------------------------------------------------------------
EXPORT_C void CPnpProvUtil::GetProvAdapterAppIdsL(RPointerArray<HBufC>& array)
    {

    // Get list of provisioning application IDs set by a particular 
    // provisioning adapter

    TBuf<KBufferMaxLength> getValue;
    GetCenrepValueL(KCRUidPnpUtil,KPnPProvUtilProvAdapID, getValue);

    ParseValueL(getValue, array);

    TBufC<KBufferMinLength> buf(_L(""));
    SetProvAdapterAppIdL(buf);
    }

// -----------------------------------------------------------------------------
// LaunchOnlineSupportL
// -----------------------------------------------------------------------------

EXPORT_C void CPnpProvUtil::LaunchOnlineSupportL(TDesC& aUri,TConnectReason aConnectReason, TStartMode aMode, TBool aTokenOverride , TDesC& aQueryString )
    {

    TBuf<KBufferMaxLength> cmdLineString;

    // Get constructed command line string and pass it as command line argument for starting the application
    ConstructCmdLineStringL(aUri, aConnectReason, aMode, aTokenOverride, aQueryString,cmdLineString );


    //online Support is launched from Command Line Symbian Framework	 

    RApaLsSession appArcSession;
    User::LeaveIfError( appArcSession.Connect() );
    CleanupClosePushL( appArcSession );


    TApaAppInfo info;
    User::LeaveIfError( appArcSession.GetAppInfo( info,KServiceHelpUID  ) );

    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();

    cmdLine->SetExecutableNameL( info.iFullName );

    TBuf8<KBufferMaxLength> params;
    params.Copy( cmdLineString.Left( KBufferMaxLength) );
    cmdLine->SetTailEndL( params );

    TInt err = appArcSession.StartApp( *cmdLine );
    if( err != KErrNone )
        {

        User::Leave( err );
        }
    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy(); // appArcSession.Close();


    }

// -----------------------------------------------------------------------------
// ConstructUriWithPnPUtilL
// -----------------------------------------------------------------------------

EXPORT_C void CPnpProvUtil::ConstructUriWithPnPUtilL(TDes& /*aUri*/, TServers /*aServer*/)
    {
    User::Leave(KErrNotSupported);
    }


// -----------------------------------------------------------------------------
// SetCenrepValueL
// -----------------------------------------------------------------------------
void CPnpProvUtil::SetCenrepValueL(TUid aRepositoryUid,TUint32 aKey,TDesC& aValue)
    {

    // Set cenrep value to key specified as parameter to this function

    CRepository * rep = NULL;
    TInt errorStatus = KErrNone;
    TBuf<KBufferMaxLength> getValue(aValue);

    TRAPD( errVal, rep = CRepository::NewL(aRepositoryUid));

    if(errVal == KErrNone)
        {
        errorStatus =  rep->Set( aKey , getValue );
        }
    else
        {
        errorStatus = errVal;
        }

    if(rep)
        {
        delete rep;
        }


    if(errorStatus != KErrNone )
        {
        User::Leave(errorStatus);
        }


    }

// -----------------------------------------------------------------------------
// GetCenrepValueL
// -----------------------------------------------------------------------------
void CPnpProvUtil::GetCenrepValueL(TUid aRepositoryUid,TUint32 aKey, TDes& aValue)
    {

    // Get cenrep value of key specified as parameter to this function

    CRepository * rep = NULL;
    TInt errorStatus = KErrNone;

    TRAPD( errVal, rep = CRepository::NewL(aRepositoryUid));

    if(errVal == KErrNone)
        {
        errorStatus =  rep->Get( aKey , aValue );
        }
    else
        {
        errorStatus =  errVal;
        }

    if(rep)
        {
        delete rep;
        }


    if(errorStatus != KErrNone )
        {
        User::Leave(errorStatus);
        }

    }

// -----------------------------------------------------------------------------
// ParseValueL 
// -----------------------------------------------------------------------------

void CPnpProvUtil::ParseValueL(TDesC& aValue, RPointerArray<HBufC>& array )
    {

    // Parse buffer with comma as delimiter and return list of items	

    TLex aLex(aValue);

    while(aLex.Peek() != '\x00')
        {

        aLex.Mark();
        while(aLex.Peek()!=',' && aLex.Peek()!='\x00')
            aLex.Inc();

        TPtrC aPtr = aLex.MarkedToken(); 


        HBufC *buf18 = aPtr.AllocL();
        array.AppendL(buf18); 

        if(aLex.Peek()=='\x00') // end of string
            break; 

        aLex.Inc();

        }
    }


// -----------------------------------------------------------------------------
// ConstructCmdLineStringL
// -----------------------------------------------------------------------------

void CPnpProvUtil::ConstructCmdLineStringL(TDesC& aUri,TConnectReason aConnectReason, TStartMode aMode, TBool aTokenOverride, TDesC& aQueryString, TDes& aPnPUtilUri)
    {


    TInt tokennum;
    TBuf<KBufferMaxLength> cmdLineString;

    cmdLineString.Zero();

    cmdLineString.Append( KServerParam );
    cmdLineString.Append( KQuote );
    cmdLineString.Append( KServerPrefix );
    cmdLineString.Append( KQuote );

    cmdLineString.Append( KSpace );
    cmdLineString.Append( KPageParam );
    cmdLineString.Append( KQuote );
    cmdLineString.Append( KPageString );
    cmdLineString.Append( KQuote );

    cmdLineString.Append( KSpace );
    cmdLineString.Append( KReasonParam );
    cmdLineString.AppendNum( aConnectReason );

    // Non interactive mode of settings download is not supported at server side and only interactive mode is supported

    if(aMode == EStartInteractive)
        {
        cmdLineString.Append( KSpace );
        cmdLineString.Append( KAutoStartParam );
        cmdLineString.AppendNum( 0 );

        }
    else
        {
        //Silent mode is not supported by PnPMS server
        User::Leave(KErrNotSupported);
        }

    cmdLineString.Append( KSpace );
    cmdLineString.Append( KTokenOverrideParam );

    if(aTokenOverride == EFalse)
        tokennum = 0;
    else
        tokennum = 1;

    cmdLineString.AppendNum( tokennum );


    //Help Query parameter is not supported by Server so default "query" string will be set

    cmdLineString.Append( KSpace );
    cmdLineString.Append( KQueryParam );
    cmdLineString.Append( KQuote );
    cmdLineString.Append( KHelpquery );
    cmdLineString.Append( KQuote );

    aPnPUtilUri.Copy(cmdLineString);

    }



