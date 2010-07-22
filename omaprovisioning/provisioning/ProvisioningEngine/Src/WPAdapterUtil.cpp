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
* Description:  WPAdapterUtil is a utility class for reading resource strings.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include <uri16.h>
#include "CWPCharacteristic.h"
#include "WPAdapterUtil.h"
#include "TWPFindAccesspoint.h"
#include <data_caging_path_literals.hrh>
#include "CWPInternetAPDB.h"

#include "ProvisioningDebug.h"
#include <cmmanager.h>
#include <cmmanagerext.h>
#include <cmconnectionmethod.h>
#include <cmconnectionmethodext.h>
#include <cmdestination.h>
#include <cmdestinationext.h>
#include <cmmanagerdef.h>
#include "ProvisioningInternalCRKeys.h"
#include <centralrepository.h>

// LOCAL FUNCTION PROTOTYPES
typedef TBool (*TParseFunc)( TLex& aLex );
LOCAL_C TBool DoOrReverse( TParseFunc aFunc, TLex& aLex );
LOCAL_C TBool Star( TParseFunc aFunc, TLex& aLex );
LOCAL_C TBool CheckIPv6Hex4( TLex& aLex );
LOCAL_C TBool CheckIPv6ColonHex4( TLex& aLex );
LOCAL_C TBool CheckIPv6HexSeq( TLex& aLex );
LOCAL_C TBool CheckIPv6HexPart( TLex& aLex );
LOCAL_C TBool CheckIPv4AddressPart( TLex& aLex );
LOCAL_C TBool CheckIPv4Address( TLex& aLex );
LOCAL_C TBool CheckIPv6Address( TLex& aLex );
//Local functions for Internet Parameter handling
//Get Cenrep key (Operator value)
LOCAL_C void GetCenrepKeyL(TDes8& aValue);

//Get Access Point ID from Database of Internet connection Methods
//or from Internet SNAP
LOCAL_C TUint GetAPIDFromDBorSNAPL(TDesC& aOrig);

//Get the Latest Access Point ID from Internet SNAP
LOCAL_C TUint GetInternetSnapLatestAPIDL(RCmDestinationExt& InternetDestination);

// Get the destination of purpose Internet
LOCAL_C void GetInternetSnapDestinationL(RCmManagerExt& aCmManager, RCmDestinationExt& InternetDestination);

// Set the Internet Access point in Internet SNAP and 
// Database of Internet connection Methods
LOCAL_C void SetAPtoDBandSNAPL(const TUint aAPID, const TDesC& aOriginator);

using namespace CMManager;

/// Default resource file suffix
_LIT( KResourceFileSuffix, ".rsc" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WPAdapterUtil::AccesspointL
// -----------------------------------------------------------------------------
//
EXPORT_C CWPCharacteristic* WPAdapterUtil::AccesspointL( 
                                            CWPCharacteristic& aCharacteristic )
    {
    TWPFindAccesspoint finder;
    aCharacteristic.AcceptL( finder );
    return finder.iNapDef;
    }

// -----------------------------------------------------------------------------
// WPAdapterUtil::ReadHBufCL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* WPAdapterUtil::ReadHBufCL( const TDesC& aDllPath, 
                                           const TDesC& aAdapterName,
                                           TInt aResourceId )
    {
    // Open a file server session
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Open the resource file
    RResourceFile resourceFile;
    FindAndOpenResourceFileLC(fs, aDllPath, aAdapterName, resourceFile);

    TResourceReader resReader;
    resReader.SetBuffer( resourceFile.AllocReadLC( aResourceId ) );
    HBufC* text = resReader.ReadHBufCL();

    CleanupStack::PopAndDestroy(3); // resourceFile, fs, aResourceId

    return text;
    }

// -----------------------------------------------------------------------------
// WPAdapterUtil::FindAndOpenResourceFileLC
// -----------------------------------------------------------------------------
//
void WPAdapterUtil::FindAndOpenResourceFileLC( RFs& aFs, 
                                               const TDesC& aDllPath, 
                                               const TDesC& aAdapterName, 
                                               RResourceFile& aResFile )
    {
    // Get the drive from DLL file name
    TFileName fileName( aDllPath.Left( KMaxFileName ) );
    TParsePtrC parse( fileName );
    fileName = parse.Drive();

    fileName.Append( KDC_RESOURCE_FILES_DIR );
    fileName.Append( aAdapterName );
    fileName.Append( KResourceFileSuffix );

    // Retrieve the correct suffix
    BaflUtils::NearestLanguageFile( aFs, fileName );

    // Open the file
    aResFile.OpenL( aFs, fileName );
    CleanupClosePushL( aResFile );
    }

// -----------------------------------------------------------------------------
// WPAdapterUtil::CheckURI
// -----------------------------------------------------------------------------
//
EXPORT_C TBool WPAdapterUtil::CheckURI( const TDesC& aUrl )
    {
    TUriParser16 uriParser;
    TInt err = uriParser.Parse( aUrl );
    return err == KErrNone 
        && uriParser.IsPresent(EUriScheme)
        && uriParser.IsPresent(EUriHost);
    }


// -----------------------------------------------------------------------------
// WPAdapterUtil::CheckIPv4
// -----------------------------------------------------------------------------
//
EXPORT_C TBool WPAdapterUtil::CheckIPv4( const TDesC& aIP )
    {
    /*
    * ABNF
    * IPv4address = 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT
    */

    TLex lex( aIP );

    return CheckIPv4Address( lex );
    }

// -----------------------------------------------------------------------------
// WPAdapterUtil::CheckIPv6
// -----------------------------------------------------------------------------
//
EXPORT_C TBool WPAdapterUtil::CheckIPv6( const TDesC& aIP )
    {
    /*
    * ABNF from RFC2373
    * IPv6address = hexpart [ ":" IPv4address ]
    * IPv4address = 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT
    *
    * IPv6prefix  = hexpart "/" 1*2DIGIT
    *
    * hexpart = hexseq | hexseq "::" [ hexseq ] | "::" [ hexseq ]
    * hexseq  = hex4 *( ":" hex4)
    * hex4    = 1*4HEXDIG
    */

    TLex lex( aIP );
    TLex lexipv4( aIP );
    
    if(!CheckIPv4Address( lexipv4 ))
		return CheckIPv6Address( lex );
		else
    return EFalse;
    }

// ----------------------------------------------------
// CheckIPv6Address()
// IPv6address = hexpart [ ":" IPv4address ]
// ----------------------------------------------------
//
LOCAL_C TBool CheckIPv6Address( TLex& aLex )
    {
    TBool result( CheckIPv6HexPart( aLex ) );
    if( aLex.Peek() == '.' )
        {
        while( aLex.Peek() != ':' && aLex.Offset() > 0 )
            {
            aLex.UnGet();
            }
        aLex.Inc();
        result = CheckIPv4Address( aLex );
        }

    return result && aLex.Eos();
    }

// ----------------------------------------------------
// CheckIPv4Address()
// IPv4address = 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT "." 1*3DIGIT
// ----------------------------------------------------
//
LOCAL_C TBool CheckIPv4Address( TLex& aLex )
    {
    return CheckIPv4AddressPart( aLex ) 
        && aLex.Get() == '.'
        && CheckIPv4AddressPart( aLex ) 
        && aLex.Get() == '.'
        && CheckIPv4AddressPart( aLex ) 
        && aLex.Get() == '.'
        && CheckIPv4AddressPart( aLex ) && aLex.Get() == '\0' ;
    }

// ----------------------------------------------------
// CheckIPv4AddressPart()
// 1*3DIGIT
// ----------------------------------------------------
//
LOCAL_C TBool CheckIPv4AddressPart( TLex& aLex )
    {
    TBool result( aLex.Peek().IsDigit() );
    aLex.Inc();
    if( aLex.Peek().IsDigit() )
        {
        aLex.Inc();
        if( aLex.Peek().IsDigit() )
            {
            aLex.Inc();
            }
        }

    return result;
    }

// ----------------------------------------------------
// CheckIPv6HexPart()
// hexpart = hexseq | hexseq "::" [ hexseq ] | "::" [ hexseq ]
// ----------------------------------------------------
//
LOCAL_C TBool CheckIPv6HexPart( TLex& aLex )
    {
    TBool hexSeq( DoOrReverse( CheckIPv6HexSeq, aLex ) );
    TBool doubleSemiColon( EFalse );
    if( aLex.Peek() == ':' )
        {
        aLex.Inc();
        if( aLex.Get() == ':' )
            {
            doubleSemiColon = ETrue;
            DoOrReverse( CheckIPv6HexSeq, aLex );
            }
        }

    return hexSeq || doubleSemiColon;
    }

// ----------------------------------------------------
// CheckIPv6HexSeq()
// hexseq  = hex4 *( ":" hex4)
// ----------------------------------------------------
//
LOCAL_C TBool CheckIPv6HexSeq( TLex& aLex )
    {
    TBool result( CheckIPv6Hex4( aLex ) );
    if( result )
        {
        Star( CheckIPv6ColonHex4, aLex );
        }
    return result;
    }

// ----------------------------------------------------
// CheckIPv6ColonHex4()
// ":" hex4
// Used as a helper to give to Star()
// ----------------------------------------------------
//
LOCAL_C TBool CheckIPv6ColonHex4( TLex& aLex )
    {
    return aLex.Get() == ':'
        && CheckIPv6Hex4( aLex );
    }

// ----------------------------------------------------
// CheckIPv6Hex4()
// hex4    = 1*4HEXDIG
// ----------------------------------------------------
//
LOCAL_C TBool CheckIPv6Hex4( TLex& aLex )
    {
    _LIT( KHexDigits, "0123456789ABCDEF" );

    TPtrC hexDigits( KHexDigits );
    TBool foundOne( EFalse );
    while( hexDigits.LocateF( aLex.Peek() ) != KErrNotFound )
        {
        foundOne = ETrue;
        aLex.Inc();
        }

    return foundOne;
    }

// ----------------------------------------------------
// DoOrReverse()
// Calls aFunc. If it fails, the lexer is returned to 
// the state before calling DoOrReverse().
// ----------------------------------------------------
//
LOCAL_C TBool DoOrReverse( TParseFunc aFunc, TLex& aLex )
    {
    TLexMark mark;
    aLex.Mark( mark );
    if( (*aFunc)(aLex) )
        {
        return ETrue;
        }
    else
        {
        aLex.UnGetToMark( mark );
        return EFalse;
        }
    }

// ----------------------------------------------------
// Star()
// Calls aFunc repeatedly until aFunc returns EFalse.
// The lexer is left to the state where it was after
// the last succesful aFunc call. Returns ETrue if
// at least one call to aFunc succeeded.
// ----------------------------------------------------
//
LOCAL_C TBool Star( TParseFunc aFunc, TLex& aLex )
    {
    TBool hasMore( ETrue );
    TBool hasOne( EFalse );
    while( hasMore )
        {
        if( DoOrReverse( aFunc, aLex ) )
            {
            hasOne = ETrue; 
            }
        else
            {
            hasMore = EFalse;
            }
        }

    return hasOne;
    }

// -----------------------------------------------------------------------------
// WPAdapterUtil::SetAPDetailsL
// -----------------------------------------------------------------------------

EXPORT_C void WPAdapterUtil::SetAPDetailsL(TUint aAPId)
    {
    FLOG( _L( "[WPAdapterUtil] WPAdapterUtil::SetAPDetailsL:" ) );

    TBuf8<KOriginatorMaxLength> Orig;

    //Get Originator from the Cenrep Key
    GetCenrepKeyL(Orig);
    //Convert to TBuf
    TBuf<KOriginatorMaxLength> Orig1;
    Orig1.Copy(Orig);

    RCmManagerExt cmmanagerExt;
    cmmanagerExt.OpenL();
    CleanupClosePushL(cmmanagerExt);
    RCmConnectionMethodExt cm;
    cm = cmmanagerExt.ConnectionMethodL( aAPId );
    CleanupClosePushL( cm );
    TUint apId = 0;
    TRAPD( ERROR, apId = cm.GetIntAttributeL(CMManager::ECmIapId) );
    User::LeaveIfError(ERROR);

    //Set AP to SNAP and DB
    TRAPD(err,SetAPtoDBandSNAPL(apId, Orig1));
    User::LeaveIfError(err);

    CleanupStack::PopAndDestroy(2); //cm, cmmanagerExt 

    FLOG( _L( "[Provisioning] WPAdapterUtil::SetAPDetailsL: done" ) );

    }

// -----------------------------------------------------------------------------
// WPAdapterUtil::GetAPIDL
// -----------------------------------------------------------------------------
EXPORT_C TUint WPAdapterUtil::GetAPIDL()
    {
    FLOG( _L( "[Provisioning] WPAdapterUtil::GetAPIDL:" ) );

    TBuf8<KOriginatorMaxLength> Orig;

    //Get Originator from the P&S Key
    GetCenrepKeyL(Orig);

    //Convert to TBuf
    TBuf<KOriginatorMaxLength> Orig1;
    Orig1.Copy(Orig);

    TUint apid;
    //Get the APID from DB or SNAP
    apid = GetAPIDFromDBorSNAPL(Orig1);

    FLOG( _L( "[Provisioning] WPAdapterUtil::GetAPIDL:done" ) );
    return apid;

    }

// -----------------------------------------------------------------------------
//GetCenrepKeyL
// -----------------------------------------------------------------------------
//	
LOCAL_C void GetCenrepKeyL(TDes8& aValue)
    {
    FLOG( _L( "[Provisioning] WPAdapterUtil GetCenrepKeyL" ) );
    CRepository* rep = NULL;
    TInt errorStatus = KErrNone;

    TRAPD( errVal, rep = CRepository::NewL(KCRUidOMAProvisioningLV));

    if(errVal == KErrNone)
        {
        errorStatus = rep->Get( KOMAProvOriginatorContent ,aValue );
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
    FLOG( _L( "[Provisioning] WPAdapterUtil GetCenrepKeyL: done" ) );
    }

// -----------------------------------------------------------------------------
// GetAPIDFromDBorSNAPL
// -----------------------------------------------------------------------------
// 
LOCAL_C TUint GetAPIDFromDBorSNAPL(TDesC& aOrig)
    {
    FLOG( _L( "[Provisioning] WPAdapterUtil GetAPIDFromDBorSNAPL" ) );

    RArray<TAccessPointItem> AccessPointItem;
    RArray<TAccessPointItem> AccessPointItem2;
    CWPInternetAPDB *db = CWPInternetAPDB::NewLC();
    RCmConnectionMethodExt connection;

    RCmManagerExt cmManager;
    cmManager.OpenL();
    CleanupClosePushL( cmManager );

    if (aOrig.Length())
        {
        //Read the Accesspoint data from DB, for the same originator
        db->ReadDbItemsForOrigL(AccessPointItem, aOrig);

        }

    //Read the Accesspoint data from DB, for all originators
    db->ReadDbItemsL(AccessPointItem2);

    TInt APOrigCount = AccessPointItem.Count();
    TInt APCount = AccessPointItem2.Count();
    //Valid APID of same originator
    TBool APIDofSameOrig = EFalse;
    //Valid APID of same originator
    TBool APIDofOtherOrig = EFalse;

    //HBufC *AccessPointName = NULL;

    TUint apid = 0;
    TUint apid1;

    if (APOrigCount)
        {
        TInt i;

        for ( i = APOrigCount - 1; i >= 0; i-- )
            {
            apid1 = AccessPointItem[i].iAPID;
            TRAPD(err, connection = cmManager.ConnectionMethodL(apid1));
            if (err == KErrNone)
                {
                CleanupClosePushL( connection );

                if (AccessPointItem[i].iBearerType == connection.GetIntAttributeL(ECmBearerType))
                    {
                    //Valid access point of same originator is found
                    APIDofSameOrig = ETrue;
                    apid = apid1;
                    CleanupStack::PopAndDestroy();
                    break;
                    }

                CleanupStack::PopAndDestroy();
                }

            }

        //No valid access points of same originator
        //Get Access Point which is of other originator
        if(!APIDofSameOrig)
            {

            TInt j;
            TBool InvalidAPIDofOrig;
            for (i = APCount - 1; i >= 0; i-- )
                {
                apid1 = AccessPointItem2[i].iAPID;

                for (InvalidAPIDofOrig = EFalse, j = APOrigCount - 1; j >= 0; j-- )
                    {
                    if(apid1 == AccessPointItem[j].iAPID)
                        {
                        InvalidAPIDofOrig = ETrue;
                        break;
                        }
                    }

                TRAPD(err, connection = cmManager.ConnectionMethodL(apid1));
                if (err == KErrNone)
                    {
                    CleanupClosePushL( connection );
                    if (!j && !InvalidAPIDofOrig &&
                            AccessPointItem2[i].iBearerType == connection.GetIntAttributeL(ECmBearerType))

                        {
                        //Valid access point of Other originator is found
                        APIDofOtherOrig = ETrue;
                        apid = apid1;
                        CleanupStack::PopAndDestroy(); //connection
                        break;
                        }
                    CleanupStack::PopAndDestroy(); //connection
                    }

                }
            }
        }

    //No access points of same originator and valid 
    //Get Access Point of other originator
    if (APCount && !APIDofSameOrig && !APIDofOtherOrig)
        {
        for (TInt i = APCount - 1; i >= 0; i-- )
            {
            apid1 = AccessPointItem2[i].iAPID;
            TRAPD(err, connection = cmManager.ConnectionMethodL(apid1));

            if (err == KErrNone)
                {
                CleanupClosePushL( connection );
                if ( AccessPointItem2[i].iBearerType == connection.GetIntAttributeL(ECmBearerType))
                    {
                    APIDofOtherOrig = ETrue;
                    apid = apid1;
                    CleanupStack::PopAndDestroy(); //connection
                    break;
                    }

                CleanupStack::PopAndDestroy(); //connection
                }
            }

        }

    //No valid access points of same originator and other originators
    //Get Access Point from Internet SNAP
    if (!APIDofSameOrig && !APIDofOtherOrig)
        {
        //Implementation to get the latest APID from Internet SNAP
        RCmDestinationExt InternetDestination;
        TRAPD(err, GetInternetSnapDestinationL(cmManager,InternetDestination ));
        if (err == KErrNone)
            {
            CleanupClosePushL( InternetDestination );
            FLOG(_L(" CleanupClosePushL done" ));
            //SIM provisioned, DM provisioned or user created access point 
            apid = GetInternetSnapLatestAPIDL(InternetDestination);
            CleanupStack::PopAndDestroy(); // InternetDestination
            }

        }

    CleanupStack::PopAndDestroy(2); //cmManager and db
    AccessPointItem.Reset();
    AccessPointItem2.Reset();

    FLOG( _L( "[Provisioning] WPAdapterUtil GetAPIDFromDBorSNAPL: done" ) );
    return apid;
    }

// -----------------------------------------------------------------------------
// GetInternetSnapLatestAPIDL
// -----------------------------------------------------------------------------
//
LOCAL_C TUint GetInternetSnapLatestAPIDL(RCmDestinationExt& InternetDestination)
    {
    FLOG( _L( "[Provisioning] WPAdapterUtil GetInternetSnapLatestAPIDL" ) );
    TInt APSNAPCount = InternetDestination.ConnectionMethodCount();
    TUint apid = 0;

    RCmConnectionMethodExt connection;

    //Return Connection method 
    if (APSNAPCount)
        {

        connection = InternetDestination.ConnectionMethodL(APSNAPCount - 1);
        CleanupClosePushL( connection );
        apid = connection.GetIntAttributeL( ECmIapId );
        CleanupStack::PopAndDestroy();

        }

    FLOG( _L( "[Provisioning] WPAdapterUtil GetInternetSnapLatestAPIDL: done" ) );
    return apid;
    }

// -----------------------------------------------------------------------------
// GetInternetSnapDestinationL
// -----------------------------------------------------------------------------
//
LOCAL_C void GetInternetSnapDestinationL(RCmManagerExt& aCmManager,RCmDestinationExt& aDestination )
    {
    FLOG( _L( "[Provisioning] WPAdapterUtil GetInternetSnapDestinationL" ) );
    RArray<TUint32> destinationIds;

    aCmManager.AllDestinationsL( destinationIds );
    const TInt destinationCount = destinationIds.Count();

    RCmDestinationExt destination;

    TInt counter;

    for ( counter = 0; counter < destinationCount; counter++ )
        {
        destination = aCmManager.DestinationL(destinationIds[counter] );
        CleanupClosePushL( destination );
        TSnapPurpose purpose;
        purpose = (TSnapPurpose) (destination.MetadataL(CMManager::ESnapMetadataPurpose ));

        if (purpose == ESnapPurposeInternet)
            {
            aDestination = destination;
            CleanupStack::PopAndDestroy(); // destination
            break;

            }
        CleanupStack::PopAndDestroy(); // destination

        }

    destinationIds.Reset(); // destinationIds

    if (counter == destinationCount )
        {
        User::Leave(KErrNotFound);
        }

    FLOG( _L( "[Provisioning] WPAdapterUtil GetInternetSnapDestinationL: done" ) );

    }

// -----------------------------------------------------------------------------
// SetAPtoDBandSNAPL
// -----------------------------------------------------------------------------
//	
LOCAL_C void SetAPtoDBandSNAPL(const TUint aAPID, const TDesC& aOriginator)
    {
    FLOG( _L( "[Provisioning] WPAdapterUtil SetAPtoDBandSNAPL" ) );
    TInt ERROR;
    RCmManagerExt cmManager;
    TRAPD(err,cmManager.OpenL());
    User::LeaveIfError(err);
    CleanupClosePushL( cmManager ); // CS:1


    RCmConnectionMethodExt connection = cmManager.ConnectionMethodL( aAPID );
    CleanupClosePushL( connection ); // CS:2

    //Implementation to update the DB file with Access Point, Bearer Type
    // and Originator
    const TUint32 BearerType = connection.GetIntAttributeL(ECmBearerType);

    CWPInternetAPDB *db = CWPInternetAPDB::NewLC(); // CS:3

    TInt Index;
    TRAPD(err1, db->SaveToDatabaseL(Index, aAPID, BearerType, aOriginator ));
    User::LeaveIfError(err1);

    RCmDestinationExt InternetDestination;

    TRAP(ERROR, GetInternetSnapDestinationL(cmManager,InternetDestination ));

    if (ERROR == KErrNone)
        {

        CleanupClosePushL( InternetDestination ); // CS:4	

        TRAP(ERROR, InternetDestination.AddConnectionMethodL( connection ));
        if (ERROR == KErrNone)
            {
            TRAP(ERROR, InternetDestination.UpdateL());
            }

        CleanupStack::PopAndDestroy();

        }

    if (ERROR != KErrNone)
        {
        db->DeleteFromDatabaseL(aAPID);
        User::LeaveIfError(ERROR);
        }

    CleanupStack::PopAndDestroy();
    CleanupStack::PopAndDestroy( &connection );
    CleanupStack::PopAndDestroy();

    FLOG( _L( "[Provisioning] WPAdapterUtil SetAPtoDBandSNAPL: done" ) );
    }
//  End of File  
