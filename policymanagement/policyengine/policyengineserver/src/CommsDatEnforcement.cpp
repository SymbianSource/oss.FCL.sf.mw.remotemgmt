/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of policymanagement components
*
*/


// INCLUDE FILES


#include <commdb.h>

#include "CommsDatEnforcement.h"
#include "XACMLconstants.h"
#include "debug.h"
#include "PolicyEngineServer.h"
#include "PolicyStorage.h"

#include <protectdb.h>

#include <commsdattypesv1_1.h>
#include <WlanCdbCols.h>
#include <d32dbms.h>
#include <metadatabase.h>

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES
// CONSTANTS

_LIT( KCommsDatEnformentPanic, "CommsDat enforcement panic");


//Repository UIDs

const TUid TCommsDatRepository = { 0xCCCCCC00 };
const TUint32 KCDMaskRecordType = 0x7f800000;

const TUint32 KWLANServiceExtRecord			  = 0x05000000; 

const TPtrC KWLANServiceExtTable( _S("WLANServiceExtensionTable"));
const TPtrC KWLANServiceTable( _S("WLANServiceTable"));
const TPtrC KWLANDeviceTable( _S("WLANDeviceTable"));
const TPtrC KWLANSecondarySSID( _S("SecondarySSID"));
const TPtrC KDestinationNetwork( _S("DestinationNetwork"));


_LIT8( KAPURI, "AP");


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::CCommsDatEnforcement()
// -----------------------------------------------------------------------------
//
CCommsDatEnforcement::CCommsDatEnforcement()
	{
	RDEBUG("CCommsDatEnforcement::CCommsDatEnforcement()");
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::~CCommsDatEnforcement()
// -----------------------------------------------------------------------------
//
CCommsDatEnforcement::~CCommsDatEnforcement()
	{
	RDEBUG("CCommsDatEnforcement::~CCommsDatEnforcement()");
	
	if( iSession )
        {
        iSession->Close();
        delete iSession; 
        }
	
	iMaskList.Close();	
	iTableList.Close();	
	iCommsDatEnforcement.Close();
	iCentRepServer.Close();	
	iDMUtil.Close();
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::ConstructL()
// -----------------------------------------------------------------------------
//		
void CCommsDatEnforcement::ConstructL()
	{
	RDEBUG("CCommsDatEnforcement::ConstructL()");	
	//no implementation needed
    iSession = CMDBSession::NewL( CMDBSession::LatestVersion() );
	}

// -----------------------------------------------------------------------------
// CCommsDatEnforcement::NewL()
// -----------------------------------------------------------------------------
//		
CCommsDatEnforcement* CCommsDatEnforcement::NewL( const TDesC8& aEnforcementId )
	{
	RDEBUG("CCommsDatEnforcement::NewL()");
	CCommsDatEnforcement* self = 0;

	if( aEnforcementId == PolicyLanguage::Constants::EAPEnforcementPolicy )
		{
		RDEBUG("	-> EAPEnforcementPolicy");
		self = new ( ELeave ) CCommsDatEnforcement();	
		self->ConstructL();
		CleanupStack::PushL( self );
		self->iMaskList.AppendL( KCDTIdWAPAccessPointRecord );
		self->iTableList.AppendL( TPtrC(KCDTypeNameWAPAccessPoint ));
		self->iMaskList.AppendL( KCDTIdWAPSMSBearerRecord );
		self->iTableList.AppendL( TPtrC(KCDTypeNameWAPSMSBearer) );
		self->iMaskList.AppendL( KCDTIdWAPIPBearerRecord );
		self->iTableList.AppendL( TPtrC(KCDTypeNameWAPIPBearer) );
		self->iMaskList.AppendL( KCDTIdIAPRecord );
		self->iTableList.AppendL( TPtrC(KCDTypeNameIAP) );
		self->iSettingType = EAPEnforcement;
		CleanupStack::Pop( self );
		}
	else if( aEnforcementId == PolicyLanguage::Constants::EWLANEnforcementPolicy )
		{
		RDEBUG("	-> EWLANEnforcementPolicy");
		self = new ( ELeave ) CCommsDatEnforcement();	
		self->ConstructL();	
		CleanupStack::PushL( self );
		self->iMaskList.AppendL( KWLANServiceExtRecord );
		self->iTableList.AppendL( KWLANServiceExtTable );

		TUint32 wlanServiceTableId = self->GetRecordIdL( KWLANServiceTable );
		self->iMaskList.AppendL( wlanServiceTableId );
		self->iTableList.AppendL( KWLANServiceTable );
		
		TUint32 wlanDeviceTableId = self->GetRecordIdL( KWLANDeviceTable );
		self->iMaskList.AppendL( wlanDeviceTableId );
		self->iTableList.AppendL( KWLANDeviceTable );
		
		TUint32 wlanSSIDTableId = self->GetRecordIdL( KWLANSecondarySSID );
		self->iMaskList.AppendL( wlanSSIDTableId );
		self->iTableList.AppendL( KWLANSecondarySSID );
		
		TUint32 wlanDestinationNetworkTableId = self->GetRecordIdL( KDestinationNetwork );
		self->iMaskList.AppendL( wlanDestinationNetworkTableId );
		self->iTableList.AppendL( KDestinationNetwork );	

		self->iSettingType = EWLANEnforcement;
		CleanupStack::Pop( self );
		}

	return self;
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::ValidEnforcementElement()
// -----------------------------------------------------------------------------
//
TBool CCommsDatEnforcement::ValidEnforcementElement( const TDesC8& aEnforcementId )
	{
	RDEBUG("CCommsDatEnforcement::ValidEnforcementElement()");
	if ( aEnforcementId == PolicyLanguage::Constants::EAPEnforcementPolicy )
		{
		RDEBUG("	-> valid EAPEnforcementPolicy");
		return ETrue;
		} 
	else if ( aEnforcementId == PolicyLanguage::Constants::EWLANEnforcementPolicy )
		{
		RDEBUG("	-> valid EWLANEnforcementPolicy");
		return ETrue;	
		}
	
	return EFalse;
	}

	
// -----------------------------------------------------------------------------
// CCommsDatEnforcement::InitEnforcement()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::InitEnforcementL( TRequestStatus& aRequestStatus )
	{
	RDEBUG("CCommsDatEnforcement::InitEnforcementL()");
	//set restore flag
	if ( iAccessControlList->Count() )
		{
		iRestore = EFalse;
		}
	else
		{
		iRestore = ETrue;
		iInitState++;
		}

	//in first phase open connections to centreptool
	if ( iInitState == 0 || iRestore )
		{
		RDEBUG("	-> Opening connections ... ");
		User::LeaveIfError( iCentRepServer.Connect() );
		User::LeaveIfError( iCommsDatEnforcement.Open( TCommsDatRepository , iCentRepServer ) );
		User::LeaveIfError( iDMUtil.Connect());
		RDEBUG("	-> Opening connections ... DONE!");
		}
	
	//init each session in own cycle....
	switch ( iInitState )
		{
		case 0:
			{
			RDEBUG("CCommsDatEnforcement: Protect AP tables ... ");
			CCommsDatabaseProtect* dbprotect = CCommsDatabaseProtect::NewL();
			
			//add protection for GS
			for ( TInt i( 0 ); i < iTableList.Count(); i++ )
				{
				TInt err = dbprotect->ProtectTable( iTableList[ i ] );
				RDEBUG_2("CCommsDatEnforcement: Protection status %d", err );
				}			
			
			delete dbprotect;
			dbprotect = NULL;	
			
			// enable all WLAN AP locks
		    if(iSettingType == EWLANEnforcement)
		    {
				LockWLANAccessPointsL( ETrue );
		    }
				
			//compelete request
			TRequestStatus * status = &aRequestStatus;
			User::RequestComplete( status, KErrNone );
			}
		break;
		case 1:
			{
			RDEBUG("CCommsDatEnforcement: Init commsDat enforcement session");
			iCommsDatEnforcement.InitSession( aRequestStatus );
			}
		break;
		default:
			{
			RDEBUG("**** CCommsDatEnforcement PANIC, invalid switch-case!");
			User::Panic( KCommsDatEnformentPanic, KErrAbort );
			}
		break;
		}
	
	iInitState++;
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::InitReady()
// -----------------------------------------------------------------------------
//
TBool CCommsDatEnforcement::InitReady()
	{
	RDEBUG("CCommsDatEnforcement::InitReady()");
	return ( iInitState > 1 );
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::InitReady()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::DoEnforcementL( TRequestStatus& aRequestStatus )
	{
	RDEBUG("CCommsDatEnforcement::DoEnforcementL()");
	if( !iRestore )
		{
		//if there any number subject which have exclusively right for setting, give access only for DM client
		for ( TInt i( 0 ); i < iMaskList.Count(); i++ )
			{
			RDEBUG_3("	making enforcement: %d/%d", i, iMaskList.Count() );
			User::LeaveIfError( iCommsDatEnforcement.SetSIDWRForMask( iMaskList[ i ], KCDMaskRecordType, KDMClientUiD));
			User::LeaveIfError( iCommsDatEnforcement.RemoveBackupFlagForMask( iMaskList[ i ], KCDMaskRecordType));
			CPolicyStorage::PolicyStorage()->ActivateEnforcementFlagL( iSettingType );	
			}
		
		//ACL...
		RDEBUG("	making ACL modifications for enforcement ... ");
		User::LeaveIfError( iDMUtil.SetMngSessionCertificate( SessionCertificate() ) );
		User::LeaveIfError( iDMUtil.AddACLForNode( KAPURI, EForChildrens, EACLDelete ) );
		User::LeaveIfError( iDMUtil.AddACLForNode( KAPURI, EForNode, EACLGet ) );
		User::LeaveIfError( iDMUtil.SetACLForNode( KAPURI, EForNode, EACLAdd ) );
		RDEBUG("	making ACL modifications for enforcement ... DONE!");
		}
	else
		{
		//Clear default settings
		for ( TInt i( 0 ); i < iMaskList.Count(); i++ )
			{
			RDEBUG_3("	clearing default settings: %d/%d", i, iMaskList.Count() );
			User::LeaveIfError( iCommsDatEnforcement.RestoreMask( iMaskList[ i ], KCDMaskRecordType ));
			User::LeaveIfError( iCommsDatEnforcement.RestoreBackupFlagForMask( iMaskList[ i ], KCDMaskRecordType));
			CPolicyStorage::PolicyStorage()->DeactivateEnforcementFlagL( iSettingType );
			}

		//ACL...
		RDEBUG("	removing ACL modifications for enforcement ... ");
		User::LeaveIfError( iDMUtil.RemoveACL( KAPURI, ETrue ) );
		RDEBUG("	removing ACL modifications for enforcement ... DONE!");

		}
	
	iEnforcementState++;
	TRequestStatus * status = &aRequestStatus;
	User::RequestComplete( status, KErrNone );
	}

// -----------------------------------------------------------------------------
// CCommsDatEnforcement::EnforcementReady()
// -----------------------------------------------------------------------------
//
TBool CCommsDatEnforcement::EnforcementReady()
	{
	RDEBUG("CCommsDatEnforcement::EnforcementReady()");
	return iEnforcementState > 0;
	}
	
// -----------------------------------------------------------------------------
// CCommsDatEnforcement::FinishEnforcementL()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::FinishEnforcementL( TBool aFlushSettings)
	{
	RDEBUG("CCommsDatEnforcement::FinishEnforcementL()");
	//Close sessions
	if( aFlushSettings )
		{
		iCommsDatEnforcement.Flush();
		iDMUtil.Flush();
		}
	
	iCommsDatEnforcement.Close();

	//Close centrep server...
	iCentRepServer.Close();
	
	iDMUtil.Close();

		
	if( iRestore && aFlushSettings )
		{
		RDEBUG("PolicyEngineServer: Remove AP table Protection");
	
		CCommsDatabaseProtect* dbprotect = CCommsDatabaseProtect::NewL();
		CleanupStack::PushL( dbprotect );
		
		TRAP_IGNORE( LockWLANAccessPointsL( EFalse ) );
		//remove protection for GS
		for ( TInt i( 0 ); i < iTableList.Count(); i++ )
			{
			TInt err = dbprotect->UnProtectTable( iTableList[ i ] );
			RDEBUG_2("PolicyEngineServer: Protection status %d", err );
			}			
	//Condition when AP + WLAN are enforced and AP is being removed
	//GS should still show lock icons for WLANs
	      RDbRowSet::TAccess checkAccessType = RDbRowSet::EReadOnly ;
		   	TRAP_IGNORE( checkAccessType =  dbprotect->GetTableAccessL(TPtrC(WLAN_SERVICE)));
    		switch(checkAccessType)
    		{
    			case RDbRowSet::EReadOnly :
				 	TRAP_IGNORE(LockWLANAccessPointsL( ETrue ));
				default :	break;
    		}
		CleanupStack::PopAndDestroy( dbprotect );
		}
	}
				
// -----------------------------------------------------------------------------
// CCommsDatEnforcement::AccessRightList()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::AccessRightList( RAccessControlList& aAccessControlList)
	{
	RDEBUG("CCommsDatEnforcement::AccessRightList()");
	iAccessControlList = &aAccessControlList;
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::ResetEnforcementL()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::ResetEnforcementL()
	{
	RDEBUG("CCommsDatEnforcement::ResetEnforcementL()");
	
	User::LeaveIfError( iCentRepServer.Connect() );
	User::LeaveIfError( iCommsDatEnforcement.Open( TCommsDatRepository , iCentRepServer ) );
	User::LeaveIfError( iDMUtil.Connect());

	TRequestStatus request;
	iCommsDatEnforcement.InitSession( request );
	User::WaitForRequest( request);
	
		for ( TInt i( 0 ); i < iMaskList.Count(); i++ )
			{
			User::LeaveIfError( iCommsDatEnforcement.RestoreMask( iMaskList[ i ], KCDMaskRecordType ));
			User::LeaveIfError( iCommsDatEnforcement.RestoreBackupFlagForMask( iMaskList[ i ], KCDMaskRecordType));
			}

	//ACL...
	User::LeaveIfError( iDMUtil.RemoveACL( KAPURI, ETrue ) );


	iCommsDatEnforcement.Flush();
	iCommsDatEnforcement.Close();
	iCentRepServer.Close();	
	iDMUtil.Close();
	}


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::LockWLANAccessPointsL()
// -----------------------------------------------------------------------------
//
void CCommsDatEnforcement::LockWLANAccessPointsL( TBool aLockValue )
	{
	RDEBUG_2("CCommsDatEnforcement::LockAccessPoint( %d )", aLockValue );
	
	//Get WLAN service table and get ServiceID--> which is nothing but IAP ID and lock that record

	//TBool ret = EFalse;
	TUint32 apIAPID = 0;
		
    CCommsDbTableView*  checkView;
	CCommsDatabase* commsDataBase = CCommsDatabase::NewL();
	CleanupStack::PushL( commsDataBase );
    checkView = commsDataBase->OpenTableLC(TPtrC(IAP));
   	RDEBUG("		-> After opening IAP table ");
   	TBuf<KCommsDbSvrMaxFieldLength> serviceType;
    TInt error = checkView->GotoFirstRecord();
    RDEBUG("		-> After going to first record ");
    while (error == KErrNone)
        {
        RDEBUG("		-> KERRNONE ");
       		// Get the ID and check for service type
       	checkView->ReadTextL(TPtrC(IAP_SERVICE_TYPE), serviceType);
        if(serviceType == TPtrC(LAN_SERVICE))
            {
               	checkView->ReadUintL(TPtrC(COMMDB_ID), apIAPID);
               		RDEBUG_2("	->found %d WLAN AP. being protected or unprotected", apIAPID );
               	if(aLockValue)
               	{
               	((CCommsDbProtectTableView*)checkView)->ProtectRecord();
               	RDEBUG("		-> WLAN AP protected successfully!");	
               	}
               	else
               	{
               		((CCommsDbProtectTableView*)checkView)->UnprotectRecord();
               		RDEBUG("		-> WLAN AP UN protected successfully!");
               	}
               	
            }
            error = checkView->GotoNextRecord();
            
        }
    CleanupStack::PopAndDestroy(); // checkView

    CleanupStack::PopAndDestroy( commsDataBase );	


	}	


// -----------------------------------------------------------------------------
// CCommsDatEnforcement::GetRecordId()
// -----------------------------------------------------------------------------
//	
TUint32 CCommsDatEnforcement::GetRecordIdL( const TDesC& aTableName )
	{
	RDEBUG_2("looking rentrep record id for table: %S", &aTableName );
	TMDBElementId tableRecordId = 0;
	CMDBGenericRecord* tempUserDefinedRecord = static_cast<CMDBGenericRecord*>(CCDRecordBase::RecordFactoryL(0));
    CleanupStack::PushL(tempUserDefinedRecord);
    
    tempUserDefinedRecord->InitializeL(aTableName, NULL);
	tempUserDefinedRecord->LoadL(*iSession);
	
    // Get the Id that we're interested in...
    tableRecordId = tempUserDefinedRecord->TableId();
	RDEBUG_2("	found tableRecordId: %08x", tableRecordId );
	CleanupStack::PopAndDestroy(tempUserDefinedRecord);

    // ..and validate it.
    if ((tableRecordId & KCDMaskShowRecordType) < KCDInitialUDefRecordType)
    	{
	   	RDEBUG("Error validating tableRecordId");
        User::Leave(KErrNotFound);
        }

	return tableRecordId;
	}

