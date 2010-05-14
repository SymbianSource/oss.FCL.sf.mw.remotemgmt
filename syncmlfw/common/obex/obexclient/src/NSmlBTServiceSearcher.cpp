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
* Description:  SyncML Obex client
*
*/


#include "NSmlBTServiceSearcher.h"

//============================================================
// CNSmlBTServiceSearcher definition
//============================================================

//------------------------------------------------------------
// CNSmlBTServiceSearcher::CNSmlBTServiceSearcher()
//------------------------------------------------------------
CNSmlBTServiceSearcher::CNSmlBTServiceSearcher( const TBTConnInfo& aBTConnInfo )
:  iIsDeviceSelectorConnected(EFalse),
   iBTConnInfo( aBTConnInfo )
    {
	iResponse().SetDeviceAddress( aBTConnInfo.iDevAddr );
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::~CNSmlBTServiceSearcher()
//------------------------------------------------------------
CNSmlBTServiceSearcher::~CNSmlBTServiceSearcher()
    {
    if( iIsDeviceSelectorConnected )
        {
/*        iDeviceSelector.CancelNotifier( KDeviceSelectionNotifierUid );
        iDeviceSelector.Close();
 */       
        }
	Finished();
    delete iSdpSearchPattern;
    iSdpSearchPattern = NULL;

    delete iAgent;
    iAgent = NULL;
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::SelectDeviceByDiscoveryL( TRequestStatus& aObserverRequestStatus )
//------------------------------------------------------------
void CNSmlBTServiceSearcher::SelectDeviceByDiscoveryL( TRequestStatus& aObserverRequestStatus )
    {
     if( !iIsDeviceSelectorConnected )
        {
 /*       	
        User::LeaveIfError( iDeviceSelector.Connect() );
        iIsDeviceSelectorConnected = ETrue;
*/        
        }

    //  Request a device selection 
	TBTDeviceClass deviceClass(0 ,0 ,0);
	
    TBTDeviceSelectionParams parameters;
    parameters.SetDeviceClass(deviceClass);
	
    TBTDeviceSelectionParamsPckg pckg(parameters);

	pckg().SetUUID( iBTConnInfo.iServiceClass );

//    iDeviceSelector.StartNotifierAndGetResponse(
//         aObserverRequestStatus, 
//         KDeviceSelectionNotifierUid, 
//         /*selectionFilter*/pckg, 
//         iResponse);


    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::FindServiceL( TRequestStatus& aObserverRequestStatus )
//------------------------------------------------------------
void CNSmlBTServiceSearcher::FindServiceL( TRequestStatus& aObserverRequestStatus )
    {
    if( !iResponse().IsValidBDAddr() )
        {
        User::Leave( KErrNotFound );
        }
			
	iHasFoundService = EFalse;

	// delete any existing agent and search pattern
	delete iSdpSearchPattern;
	iSdpSearchPattern = NULL;

	delete iAgent;
	iAgent = NULL;

	iAgent = CSdpAgent::NewL( *this, BTDevAddr() );

	iSdpSearchPattern = CSdpSearchPattern::NewL();
	iSdpSearchPattern->AddL( iBTConnInfo.iServiceClass ); 

	iAgent->SetRecordFilterL( *iSdpSearchPattern );

	iStatusObserver = &aObserverRequestStatus;

	iAgent->NextRecordRequestL();
	}
//------------------------------------------------------------
// CNSmlBTServiceSearcher::NextRecordRequestComplete( TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount )
//------------------------------------------------------------
void CNSmlBTServiceSearcher::NextRecordRequestComplete( TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount )
    {
    TRAPD( error, NextRecordRequestCompleteL( aError, aHandle, aTotalRecordsCount) );
    if ( error != KErrNone ) 
        {
        Panic( EBTServiceSearcherNextRecordRequestComplete );
        }
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::NextRecordRequestCompleteL( TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount )
//------------------------------------------------------------
void CNSmlBTServiceSearcher::NextRecordRequestCompleteL( TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount )
    {
    if ( aError == KErrEof )
        {
        Finished();
        return;
        }

    if ( aError != KErrNone )
        {
        Finished( aError );
        return;
        }

    if ( aTotalRecordsCount == 0 )
        {
        Finished( KErrNotFound );
        return;
        }

    //  Request its attributes
    iAgent->AttributeRequestL( aHandle, KSdpAttrIdProtocolDescriptorList );
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::AttributeRequestResult()
//------------------------------------------------------------
void CNSmlBTServiceSearcher::AttributeRequestResult(
    TSdpServRecordHandle aHandle, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue)
    {
    TRAPD( error, AttributeRequestResultL( aHandle, aAttrID, aAttrValue ) );
    
    //deleted because ownership has been transferred    
    delete aAttrValue;
    aAttrValue = NULL;
    
    if( error != KErrNone )
        {
        Panic(EBTServiceSearcherAttributeRequestResult);
        }
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::AttributeRequestResultL()
//------------------------------------------------------------
void CNSmlBTServiceSearcher::AttributeRequestResultL(
    TSdpServRecordHandle /*aHandle*/, 
    TSdpAttributeID aAttrID, 
    CSdpAttrValue* aAttrValue)
    {
    
    ASSERT( aAttrID == KSdpAttrIdProtocolDescriptorList );

    TSdpAttributeParser parser( ProtocolList(), *this );

    // Validate the attribute value, and extract the RFCOMM channel
    aAttrValue->AcceptVisitorL( parser );

    if ( parser.HasFinished() )
        {
        // Found a suitable record so change state
        iHasFoundService = ETrue;
        }
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::AttributeRequestComplete()
//------------------------------------------------------------
void CNSmlBTServiceSearcher::AttributeRequestComplete(TSdpServRecordHandle aHandle, TInt aError)
    {
    TRAPD( error, AttributeRequestCompleteL( aHandle, aError ) );
    if( error != KErrNone )
        {
        Panic( EBTServiceSearcherAttributeRequestComplete );
        }
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::AttributeRequestCompleteL()
//------------------------------------------------------------
void CNSmlBTServiceSearcher::AttributeRequestCompleteL(TSdpServRecordHandle /*aHandle*/, TInt aError)
    {
    if ( aError != KErrNone )
        {
        }
    else if( !HasFinishedSearching() )
        {
        // have not found a suitable record so request another
        iAgent->NextRecordRequestL();
        }
    else
        {
        Finished();
        }
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::Finished()
//------------------------------------------------------------
void CNSmlBTServiceSearcher::Finished(TInt aError /* default = KErrNone */)
    {
    if( aError == KErrNone && !HasFoundService() )
        {
        aError = KErrNotFound;
        }
	if ( iStatusObserver )
		{
		if ( *iStatusObserver == KRequestPending )
			{
			User::RequestComplete( iStatusObserver, aError );
			}
		}
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::HasFinishedSearching()
//------------------------------------------------------------
TBool CNSmlBTServiceSearcher::HasFinishedSearching() const
    {
    return EFalse;
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::BTDevAddr()
//------------------------------------------------------------
const TBTDevAddr& CNSmlBTServiceSearcher::BTDevAddr()
    {
    return iResponse().BDAddr();
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::ResponseParams()
//------------------------------------------------------------
const TBTDeviceResponseParams& CNSmlBTServiceSearcher::ResponseParams()
    {
    return iResponse();
    }
//------------------------------------------------------------
// CNSmlBTServiceSearcher::HasFoundService()
//------------------------------------------------------------
TBool CNSmlBTServiceSearcher::HasFoundService() const
    {
    return iHasFoundService;
    }
