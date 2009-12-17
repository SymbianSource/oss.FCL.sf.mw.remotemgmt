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
* Description:  DS server alert handler
*
*/


// INCLUDE FILES
//system includes
#include <s32mem.h>
#include <SyncMLDef.h>
#include <implementationproxy.h>	// TImplementationProxy definition
#include <nsmlconstants.h>
#include <nsmldebug.h>
#include <nsmlunicodeconverter.h>
#include <NSmlPrivateAPI.h>

#include "WBXMLSyncMLParser.h"
#include "WBXMLParser.h"
#include "WBXMLParserError.h"
#include "smlmetinfdtd.h"
#include "NSmlURI.h"
#include "nsmldsalert.h"
#include "nsmldssettings.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CNSmlDSAlert::CNSmlDSAlert()
// Constructor
// ---------------------------------------------------------
//
CNSmlDSAlert::CNSmlDSAlert()
    {
	}


// ---------------------------------------------------------
// CNSmlDSAlert::NewL()
// Creates new instance of CNSmlDSAlert. 
// Does not leave instance pointer to CleanupStack.
// ---------------------------------------------------------
//
CNSmlDSAlert* CNSmlDSAlert::NewL()
    {
	CNSmlDSAlert* self = new (ELeave) CNSmlDSAlert();
	return self;
    }

// ---------------------------------------------------------
// CNSmlDSAlert::~CNSmlDSAlert()
// Destructor
// ---------------------------------------------------------
//
CNSmlDSAlert::~CNSmlDSAlert()
    {
	}

// ---------------------------------------------------------
// CNSmlDSAlert::StartSync( const TDesC8& aSyncMLMessage, TInt aTransport, TBTDevAddr aBTAddress,
//    						const TDesC& aBTName, MNSmlAlertObexPluginContext* aPluginContext )
// Both standards supported (1.1 and 1.2)
// ---------------------------------------------------------
//
	TNSmlAlertResult CNSmlDSAlert::StartSyncL(
    const TDesC8& aSyncMLMessage,
    TInt aTransport,
    TBTDevAddr /* aBTAddress */,
    const TDesC& /* aBTName */,
    MNSmlAlertObexPluginContext* /* aPluginContext */)
 	{
	DBG_FILE(_S8("CNSmlDSAlert::StartSync begins"));
	// Handling 1.2 Notification package
	//==================================
	TSmlProtocolVersion protocol(ESmlVersion1_2);
		
	if ( ! Is12Package( aSyncMLMessage ) )
		{
		// Handling 1.1 Package0
		//======================
		TRAPD( parseErr, ParseMessageL( aSyncMLMessage ) );
		if ( ( parseErr != KErrNone ) || (iPacketShouldGoToMdo) )
			{
			// It may be MDO package
			return TNSmlAlertResult( ENSmlErrNotConsumed );
			}
		
		protocol = ESmlVersion1_1_2;
		
		}
	
	RNSmlPrivateAPI privateApi;
	TRAPD(openErr, privateApi.OpenL());
	
	if ( openErr != KErrNone )
		{
		return ENSmlErrUndefined;
		}
	
	const TUid transportId = { aTransport };
 	
	TRAPD(connErr, privateApi.SendL(aSyncMLMessage, ESmlDataSync, 
	        protocol, transportId ));
	
	if ( connErr != KErrNone )
		{
		privateApi.Close();
		return ENSmlErrUndefined;
		}
		
	privateApi.Close();
	
	return ENSmlErrNone;
	}



//
//  Private member functions
//

// ---------------------------------------------------------
// CNSmlDSAlert::Is12Package( const TDesC8& aTriggerMessage ) const
// 
// ---------------------------------------------------------
TBool CNSmlDSAlert::Is12Package( const TDesC8& aTriggerMessage ) const
	{
	if ( aTriggerMessage.Length() > KNSmlDSAlertVersionPos )
		{
		TInt version;
		version = ((TUint8) aTriggerMessage[ KNSmlDSAlertVersionPos ]) << 8;
		version |= (TUint8) aTriggerMessage[ KNSmlDSAlertVersionPos + 1 ];
		version = version >> 6;
		if ( version == KNSmlDSAlertVersion )
			{
			return ETrue;
			}
		}
	return EFalse;
	}


// ---------------------------------------------------------
// CNSmlDSAlert::ParseMessageL( const TDesC8& aSyncMLMessage )
// 
// ---------------------------------------------------------
void CNSmlDSAlert::ParseMessageL( const TDesC8& aSyncMLMessage )
    {
	DBG_FILE(_S8("CNSmlDSAlert::ParseMessageL begins"));
    CWBXMLSyncMLDocHandler* docHandler = CWBXMLSyncMLDocHandler::NewL( this );
	CleanupStack::PushL( docHandler );
	CWBXMLParser* parser = CWBXMLParser::NewL();
	CleanupStack::PushL( parser );
	parser->SetDocumentHandler( docHandler );
	parser->SetExtensionHandler( docHandler );
	RDesReadStream readStream;
	readStream.Open( aSyncMLMessage );
	CleanupClosePushL ( readStream );
	parser->SetDocumentL( readStream );
	TWBXMLParserError ret = KWBXMLParserErrorOk;
	while ( ret == KWBXMLParserErrorOk )
		{
	    ret = parser->ParseL();
		}
	CleanupStack::PopAndDestroy( 3 ); //readStream, parser, docHandler
	DBG_FILE(_S8("CNSmlDSAlert::ParseMessageL ends"));
	}


// ---------------------------------------------------------
// CNSmlDSAlert::PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, HBufC*& aUnicodeData ) const
// Converts data in Pcdata structure (used by xml parser)
// to Unicode 
// ---------------------------------------------------------
void CNSmlDSAlert::PCDATAToUnicodeLC( const SmlPcdata_t& aPcdata, HBufC*& aUnicodeData ) const
	{
	if ( !aPcdata.content )
		{
		aUnicodeData = NULL;
		aUnicodeData = HBufC::NewLC( 0 );
		}
	TPtr8 pcdata( (TUint8*) aPcdata.content, aPcdata.length, aPcdata.length );
	TrimRightSpaceAndNull( pcdata );
	NSmlUnicodeConverter::HBufC16InUnicodeLC( pcdata, aUnicodeData );    
	}

/// ---------------------------------------------------------
// CNSmlDSAlert::TrimRightSpaceAndNull( TDes8& aDes ) const
// Trims right spaces and zero terminator (NULL) 
// ---------------------------------------------------------
void CNSmlDSAlert::TrimRightSpaceAndNull( TDes8& aDes ) const
	{
	aDes.TrimRight();
	if ( aDes.Length() > 0 )
		{
		if ( aDes[aDes.Length() - 1] == NULL )
			{
			aDes.SetLength( aDes.Length() - 1 );
			}	
		}
	}

//
// Callback functions implementation
//
// ---------------------------------------------------------
// CNSmlDSAlert::smlAlertCmdFuncL
// Alert command from server.  
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlAlertCmdFuncL( SmlAlertPtr_t /*aAlert*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlGetCmdFuncL
// Get command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlGetCmdFuncL( SmlGetPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlPutCmdFuncL
// Put command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlPutCmdFuncL( SmlPutPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlResultsCmdFuncL
// Results command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlResultsCmdFuncL( SmlResultsPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlStatusCmdFuncL
// Status command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlStatusCmdFuncL( SmlStatusPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlStartMessageFuncL
// SyncHdr from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlStartMessageFuncL( SmlSyncHdrPtr_t aSyncHdr )
	{
	
	if ( !aSyncHdr || !aSyncHdr->source || !aSyncHdr->source->locURI  ) 
		{
		User::Leave( KErrGeneral );
		}

    if ( ( aSyncHdr->meta ) &&
		 ( aSyncHdr->meta->content ) &&
         ( aSyncHdr->meta->contentType == SML_PCDATA_EXTENSION ) && 
         ( aSyncHdr->meta->extension == SML_EXT_METINF ) )
        {
        SmlMetInfMetInf_t* metInf;
        metInf = (SmlMetInfMetInf_t*) aSyncHdr->meta->content;
        if ( metInf->emi )
            {
            TPtr8 emi( ( TUint8* ) metInf->emi->data->content,
                       metInf->emi->data->length,
                       metInf->emi->data->length );
            TrimRightSpaceAndNull( emi );

            if ( emi.Match(KMdoAgentSupportedVersion) != KErrNotFound )
                {
                // the sync message should go to mdo, instead of ds
                iPacketShouldGoToMdo = ETrue;
                return 0;
                }
            }
        }
        
  	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlEndMessageFuncL
// End of SyncML message was reached
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlEndMessageFuncL( Boolean_t /*aFinal*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlStartSyncFuncL
// Sync command from server
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlStartSyncFuncL( SmlSyncPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlEndSyncFuncL
// End of Sync command was reached
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlEndSyncFuncL()
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlStartAtomicFuncL
// Atomic command from server, Atomic is not supported
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlStartAtomicFuncL( SmlAtomicPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlEndAtomicFuncL
// End of Atomic command was reached, Atomic is not supported
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlEndAtomicFuncL()
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlStartSequenceFuncL
// Sequence command from server, Sequence is not supported
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlStartSequenceFuncL( SmlSequencePtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlEndSequenceFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlEndSequenceFuncL()
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlAddCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlAddCmdFuncL( SmlAddPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlCopyCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlCopyCmdFuncL( SmlCopyPtr_t /*aContent*/)
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlDeleteCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlDeleteCmdFuncL( SmlDeletePtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlExecCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlExecCmdFuncL( SmlExecPtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlMapCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlMapCmdFuncL( SmlMapPtr_t /*aContent*/ )   
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlReplaceCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlReplaceCmdFuncL( SmlReplacePtr_t /*aContent*/ )
	{
	return 0;
	}
// ---------------------------------------------------------
// CNSmlDSAlert::smlSearchCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlSearchCmdFuncL( SmlSearchPtr_t /*aContent*/ )
	{
	return 0;
	}

//1.2 CHANGES: Move command
// ---------------------------------------------------------
// CNSmlDSAlert::smlMoveCmdFuncL
// 
// ---------------------------------------------------------
Ret_t CNSmlDSAlert::smlMoveCmdFuncL( SmlMovePtr_t /*aContent*/ )
	{
	return 0;
	}
//Changes end


// ------------------------------------------------------------------------------------------------
// Implementationproxy for ECOM
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
// Constants for implementation proxy
// ------------------------------------------------------------------------------------------------

#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY(aUid, aFuncPtr)	{{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] = 
    {
		IMPLEMENTATION_PROXY_ENTRY(KNSmlDSAlertImplUid, CNSmlDSAlert::NewL)
    };

// ------------------------------------------------------------------------------------------------
// TImplementationProxy* ImplementationGroupProxy
// Implementation proxy
// ------------------------------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
	_DBG_FILE("CNSmlDSAlert::ImplementationGroupProxy(): begin");
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
	_DBG_FILE("CNSmlDSAlert::ImplementationGroupProxy(): end");
    return ImplementationTable;
	}

//  End of File  
