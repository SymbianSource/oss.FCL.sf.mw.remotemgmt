/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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


// INCLUDES

#include "DMUtilSession.h"
#include "ACLStorage.h"
#include "DMUtilServer.h"
#include "policymnginternalpskeys.h"
#include "debug.h"

#include "CentRepToolClient.h"


#include <e32svr.h>
#include <centralrepository.h>

#include <coreapplicationuisdomainpskeys.h>
#include <hbsymbianvariant.h>
#include <hbindicatorsymbian.h>


#include <e32property.h>
#include <f32file.h>

#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#endif //RD_MULTIPLE_DRIVEs

// CONSTANTS
#ifndef RD_MULTIPLE_DRIVE
const TUint8 KMMCDriveLetter = 'e';
#endif
_LIT( MDriveColon, ":");
_LIT( KMMCEraseFlagFileName, "fmmc.dat");

_LIT(KDMSettingsIndicatorType, "com.nokia.devicemanagement.settingsindicatorplugin/1.0");
_LIT(KTarmTrustManagementActive, "ManagementActive");
_LIT(KTarmTrustTerminalSecurity, "TerminalSecurity");

// MACROS


// DATA TYPES
// FUNCTION PROTOTYPES
// FORWARD DECLARATIONS

// DMUtilSession.cpp
//

// ----------------------------------------------------------------------------------------
// CDMUtilSession::CDMUtilSession
// ----------------------------------------------------------------------------------------
CDMUtilSession::CDMUtilSession()
	{
	RDEBUG("CDMUtilSession::CDMUtilSession");
	}

// ----------------------------------------------------------------------------------------
// CDMUtilSession::Server
// ----------------------------------------------------------------------------------------
CDMUtilServer& CDMUtilSession::Server()
	{
	return *static_cast<CDMUtilServer*>(const_cast<CServer2*>(CSession2::Server()));
	}

// ----------------------------------------------------------------------------------------
// CDMUtilSession::CreateL
// 2nd phase construct for sessions - called by the CServer framework
// ----------------------------------------------------------------------------------------
void CDMUtilSession::CreateL()
{
	RDEBUG("CDMUtilSession::CreateL");
	
	iACLStorage = CDMUtilServer::ACLStorage();
}

// ----------------------------------------------------------------------------------------
// CDMUtilSession::~CDMUtilSession
// ----------------------------------------------------------------------------------------
CDMUtilSession::~CDMUtilSession()
{
	RDEBUG("CDMUtilSession::~CDMUtilSession");
}

// ----------------------------------------------------------------------------------------
// CDMUtilSession::PerformRFS
// ----------------------------------------------------------------------------------------
TInt CDMUtilSession::PerformRFSL()
{
	RDEBUG("CDMUtilSession: Restory factory setting operation started");
	//RFS tasks in DMUtilServer
	// 1. Update RFS flag status
	// 2. Reset policy management state flags
	
	//update RFS flag
	CRepository* cenrep = CRepository::NewLC( KCRUidPolicyManagementUtilInternalKeys);
	
	TInt currentValue = 0;
	TInt err = cenrep->Get( KRFSCounterKey, currentValue);
	if ( err == KErrNone)
	{
		cenrep->Set( KRFSCounterKey, ++currentValue);
		RProperty::Set( KPolicyMngProperty, KFactoryResetCounter, currentValue);
	}
	
	//reset policy management state flags
	cenrep->Set( KPolicyChangeCounter, 0);
	RProperty::Set( KPolicyMngProperty, KPolicyChangedCounter, 0);
		
	cenrep->Set( KTerminalSecurityStateKey, 0);
	SetIndicatorStateL( cenrep);
	
	CleanupStack::PopAndDestroy( cenrep);
	
	RDEBUG("CDMUtilSession: Restory factory setting operation finished");
	
	
	return KErrNone;
}


// ----------------------------------------------------------------------------------------
// CDMUtilSession::ServiceL
// ----------------------------------------------------------------------------------------
void CDMUtilSession::ServiceL(const RMessage2& aMessage)
{
    TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
}

// ----------------------------------------------------------------------------------------
// CDMUtilSession::DispatchMessageL
// ----------------------------------------------------------------------------------------
void CDMUtilSession::DispatchMessageL(const RMessage2& aMessage)
{
	RDEBUG_2("CDMUtilSession::ServiceL; %d",aMessage.Function());

	switch ( aMessage.Function())
	{
		case ERemoveACL:
		case ESetACL:
		case EAddACL:
		{
			//read URI... 
			//check for aMessage Length if message length is negative
			//then leave the Dispatch MessageL with KErrUnderFlow
			
			TInt minlength = aMessage.GetDesLength(0);
			if(minlength < 0)
			User::Leave(KErrUnderflow);
			
			HBufC8* uri = HBufC8::NewLC( aMessage.GetDesLength(0));
			TPtr8 uriPtr = uri->Des();
			aMessage.ReadL(0, uriPtr, 0);
			
			if ( aMessage.Function() == ERemoveACL )
			{
				RDEBUG("	DispatchMessageL case ERemoveACL");
				TBool restoreDefaults;
				TPckg<TBool> restoreDefaultsPack( restoreDefaults);
				aMessage.ReadL( 1, restoreDefaultsPack, 0);
				
				iACLStorage->RemoveACL( *uri, restoreDefaults);
			}
			else
			{
				//destination
				TACLDestination dest;
				TPckg<TACLDestination> destPack( dest);
				aMessage.ReadL(1, destPack, 0);

				//ACL commands
				TAclCommands command;
				TPckg<TAclCommands> commandPack( command);
				aMessage.ReadL(2, commandPack, 0);
		
				if ( aMessage.Function() == ESetACL)
				{
					RDEBUG("	DispatchMessageL case ESetACL");
					iACLStorage->SetACLForNodeL( *uri, dest, command);
				}
				else
				{
					RDEBUG("	DispatchMessageL case EAddACL");
					iACLStorage->AddACLForNodeL( *uri, dest, command);
				}
			}

			CleanupStack::PopAndDestroy( uri);
		}
		break;
		case EMngSessionCertificate:
		{
			RDEBUG("	DispatchMessageL case EMngSessionCertificate");
			//certificate information
			TCertInfo certInfo;
			TPckg<TCertInfo> certInfoPack( iACLStorage->MngSessionCertificate());
			aMessage.ReadL(0, certInfoPack, 0);	
		}
		break;
		case ENewDMSession:
		{
			TInt minlength = aMessage.GetDesLength(1);
			if(minlength < 0)
			User::Leave(KErrUnderflow);
			
					
			RDEBUG("	DispatchMessageL case ENewDMSession");
			//certificate information
			TCertInfo certInfo;
			TPckg<TCertInfo> certInfoPack( certInfo);
			aMessage.ReadL(0, certInfoPack, 0);
			
			//certificate information
			TPtr8 serverid = HBufC8::NewLC( aMessage.GetDesLength(1))->Des();
			aMessage.ReadL(1, serverid, 0);

			iACLStorage->NewSessionL( certInfo, serverid);
			CleanupStack::PopAndDestroy();	//HBufC8
		}
		break;
		case EFlush:
			{
			RDEBUG("	DispatchMessageL case EFlush");
			iACLStorage->SaveACLL();
			}
		break;
	    case EGetDMSessionCertInfo:
        {
        	RDEBUG("	DispatchMessageL case EGetDMSessionCertInfo");
        	//get cert info
	        TCertInfo certInfo;
	        User::LeaveIfError( Server().ACLStorage()->CertInfo( certInfo ) );
	
			//write cert info to client side
			TPckg<TCertInfo> certInfoPack( certInfo );
			aMessage.WriteL(0, certInfoPack );
        }
	    break;
		case EGetDMSessionServerId:
		{
			RDEBUG("	DispatchMessageL case EGetDMSessionServerId");
			//get server id
			HBufC8 * serverid = Server().GetServerIDL();
			CleanupStack::PushL(  serverid);
			
			//write server id to client side
			aMessage.WriteL(0, *serverid);

			CleanupStack::PopAndDestroy( serverid);
		} 	 
		break;   
		case EUpdatePolicyMngStatusFlags:
	    {
	    	RDEBUG("	DispatchMessageL case EUpdatePolicyMngStatusFlags");
			KPolicyMngStatusFlags policyMngStatus;
			TPckg<KPolicyMngStatusFlags> policyMngStatusPack( policyMngStatus);
			aMessage.ReadL(0, policyMngStatusPack, 0);

	    	UpdateStatusFlagsL( policyMngStatus);
	    }
	    break;	    
		case EPerformDMUtilRFS:
		{
			RDEBUG("	DispatchMessageL case EPerformDMUtilRFS");
			PerformRFSL();
		}
	    break;
		case EMarkMMCWipe:
				{
				MarkMMCWipeL();
				}
		default:
		break;
    }
}


// ----------------------------------------------------------------------------------------
// CDMUtilServer::UpdateStatusFlagsL
// Define publish&subscribe properties
// ----------------------------------------------------------------------------------------

void CDMUtilSession::UpdateStatusFlagsL( KPolicyMngStatusFlags aUpdateCommand)
{
	RDEBUG("CDMUtilSession::UpdateStatusFlagsL()");
	//set initial values for properties
	CRepository* cenrep = CRepository::NewLC( KCRUidPolicyManagementUtilInternalKeys);

	switch ( aUpdateCommand )
	{
		case EPolicyChanged:
		{
			RDEBUG("		UpdateStatusFlagsL() case EPolicyChanged");
			TInt currentValue = 0;
			User::LeaveIfError( cenrep->Get( KPolicyChangeCounter, currentValue));
			User::LeaveIfError( cenrep->Set( KPolicyChangeCounter, ++currentValue));
			
			User::LeaveIfError( RProperty::Set( KPolicyMngProperty, KPolicyChangedCounter, currentValue));
		
		}
		break;	
		case ETerminalSecurityActive:
			RDEBUG("		UpdateStatusFlagsL() case ETerminalSecurityActive");
			User::LeaveIfError( cenrep->Set( KTerminalSecurityStateKey, 1));
			SetIndicatorStateL( cenrep);
		break;
		case ETerminalSecurityDeactive:
			RDEBUG("		UpdateStatusFlagsL() case ETerminalSecurityDeactive");
			User::LeaveIfError( cenrep->Set( KTerminalSecurityStateKey, 0));
			SetIndicatorStateL( cenrep);
		break;
		default:
		break;
	}

	
	CleanupStack::PopAndDestroy( cenrep);
}


// ----------------------------------------------------------------------------------------
// CDMUtilServer::UpdateSessionInfoL
// ----------------------------------------------------------------------------------------
void CDMUtilSession::SetIndicatorStateL( CRepository * aCenRep)
{
	RDEBUG8_2("CDMUtilSession::SetIndicatorStateL() 0x%x", aCenRep);
	//get terminal security and management state from centrep
	TInt terminalSecurity = 0;
	TInt err( KErrNone );
	
	CHbIndicatorSymbian *ind = CHbIndicatorSymbian::NewL();
	CleanupStack::PushL(ind);

	if ( !aCenRep)
		{
		CRepository* rep = CRepository::NewLC( KCRUidPolicyManagementUtilInternalKeys);				
		err = rep->Get( KTerminalSecurityStateKey, terminalSecurity);
		RDEBUG8_2("CDMUtilSession::SetIndicatorStateL aCenRep not found ERROR: %d",err);
		CleanupStack::PopAndDestroy( rep);
		}
	else
		{
		err = aCenRep->Get( KTerminalSecurityStateKey, terminalSecurity);
		RDEBUG8_2("CDMUtilSession::SetIndicatorStateL aCenRep ERROR: %d",err);
		}

	//set indicator state...
	if ( CDMUtilServer::iManagementActive )			
		{
		RDEBUG(" iManagementActive is ETrue -> ECoreAppUIsTarmMngActiveIndicatorOn");
		//err = RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsTarmIndicator, ECoreAppUIsTarmMngActiveIndicatorOn);
		
		CHbSymbianVariant* parameter = CHbSymbianVariant::NewL(&KTarmTrustManagementActive,  
                            CHbSymbianVariant::EDes);
		ind->Activate(KDMSettingsIndicatorType, parameter );
		delete parameter;

		}
	else if ( terminalSecurity )
		{
		RDEBUG(" terminalSecurity is ETrue -> ECoreAppUIsTarmTerminalSecurityOnIndicatorOn");
		//err = RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsTarmIndicator, ECoreAppUIsTarmTerminalSecurityOnIndicatorOn);
		
		CHbSymbianVariant* parameter = CHbSymbianVariant::NewL(&KTarmTrustTerminalSecurity,  
                            CHbSymbianVariant::EDes);
		ind->Activate(KDMSettingsIndicatorType, parameter );
		delete parameter;
		}
	else
		{
		RDEBUG(" terminalSecurity is EFalse -> ECoreAppUIsTarmIndicatorsOff");
		//err = RProperty::Set( KPSUidCoreApplicationUIs, KCoreAppUIsTarmIndicator, ECoreAppUIsTarmIndicatorsOff);
		
		ind->Deactivate(KDMSettingsIndicatorType);
		}	
	
	CleanupStack::PopAndDestroy(ind);
	
	if( err != KErrNone )
		{
		RDEBUG8_2("CDMUtilSession::SetIndicatorStateL ERROR: %d",err);
		}
		
	RDEBUG("CDMUtilSession::SetIndicatorStateL() - END");
}


// ----------------------------------------------------------------------------------------
// CDMUtilServer::DefineProperties
// Define publish&subscribe properties
// ----------------------------------------------------------------------------------------
void CDMUtilSession::DefinePropertiesL()
{
	RDEBUG("CDMUtilSession::DefinePropertiesL()");
	//define properties...
	TInt err = RProperty::Define( KPolicyMngProperty, KPolicyChangedCounter, RProperty::EInt);
	if ( err != KErrNone && err != KErrAlreadyExists)
	{
		User::Leave( err);
	}
	
	err = RProperty::Define( KPolicyMngProperty, KFactoryResetCounter, RProperty::EInt);
	if ( err != KErrNone &&  err != KErrAlreadyExists)
	{
		User::Leave( err);
	}
	
	//set initial values for properties
	CRepository* cenrep = CRepository::NewLC( KCRUidPolicyManagementUtilInternalKeys);
	
	//policy change counter
	TInt value = 0;
	err = cenrep->Get( KPolicyChangeCounter, value);
	if ( err == KErrNotFound)
	{
		User::LeaveIfError( cenrep->Create( KPolicyChangeCounter, value));
	}
	User::LeaveIfError( RProperty::Set( KPolicyMngProperty, KPolicyChangedCounter, 0));

	//policy change counter
	value = 0;
	err = cenrep->Get( KRFSCounterKey, value);
	if ( err == KErrNotFound)
	{
		User::LeaveIfError( cenrep->Create( KRFSCounterKey, value));
	}
	User::LeaveIfError( RProperty::Set( KPolicyMngProperty, KFactoryResetCounter, 0));
	
	//terminal security active
	err = cenrep->Get( KTerminalSecurityStateKey, value);
	if ( err == KErrNotFound)
	{
		User::LeaveIfError( cenrep->Create( KTerminalSecurityStateKey, 0));
	}

	CDMUtilSession::SetIndicatorStateL( cenrep);

	CleanupStack::PopAndDestroy( cenrep);
}


// ----------------------------------------------------------------------------------------
// CDMUtilSession::ServiceError
// Handle an error from CDMUtilSession::ServiceL()
// ----------------------------------------------------------------------------------------
void CDMUtilSession::ServiceError(const RMessage2& aMessage,TInt aError)
	{
	RDEBUG_2("CDMUtilSession::ServiceError %d",aError);
	CSession2::ServiceError(aMessage,aError);
	}
	
	
	
	
void CDMUtilSession::MarkMMCWipeL()
	{
	RDEBUG("CDMUtilSession::MarkMMCWipeL()");
	RFs rfs;
	User::LeaveIfError( rfs.Connect() );
	CleanupClosePushL( rfs );	
	
	// create private directory to MMC
#ifndef RD_MULTIPLE_DRIVE
	TInt err = rfs.CreatePrivatePath( EDriveE );
	if( err != KErrNone && err != KErrAlreadyExists )
		{
		RDEBUG_2("	**** ERROR, can not create private path on MMC drive: %d", err );
		User::Leave( err );
		}
		
	TFileName fullPath;
	fullPath.Append( KMMCDriveLetter );
	fullPath.Append( MDriveColon );
	
	TFileName privatePath;
	User::LeaveIfError( rfs.PrivatePath( privatePath ) );
	fullPath.Append( privatePath );
	
	fullPath.Append( KMMCEraseFlagFileName );
	
	RFile file;
	// it is ok to leave if the file already exists
	User::LeaveIfError( file.Create( rfs, fullPath, EFileWrite ) );
	file.Close();
#else
	TDriveList driveList;
	TInt driveCount;
	//Get all removeable drive, both physically and logically
	User::LeaveIfError(DriveInfo::GetUserVisibleDrives(
            rfs,  driveList,  driveCount, KDriveAttRemovable ));
    
    TInt max(driveList.Length());
    
    for(TInt i=0; i<max;++i)
    {
    	if (driveList[i])
    	{
    		TUint status;
    		DriveInfo::GetDriveStatus(rfs, i, status);
    		//To make sure the drive is physically removeable not logically removeable	
    		//need to format internal mass memory also. So no need to check the below condition
    	//	if (status & DriveInfo::EDriveRemovable)
    		{
    		
    			TInt err = rfs.CreatePrivatePath( i );
				if( err != KErrNone && err != KErrAlreadyExists )
				{
					RDEBUG_2("	**** ERROR, can not create private path on MMC drive: %d", err );
					User::Leave( err );
				}
				
    			TChar driveLetter;
    			rfs.DriveToChar(i,driveLetter);
    			TFileName fullPath;
				fullPath.Append( driveLetter );
				fullPath.Append( MDriveColon );
				
				TFileName privatePath;
				User::LeaveIfError( rfs.PrivatePath( privatePath ) );
				fullPath.Append( privatePath );
				fullPath.Append( KMMCEraseFlagFileName );
				
				RFile file;
				// it is ok to leave if the file already exists
				User::LeaveIfError( file.Create( rfs, fullPath, EFileWrite ) );
				file.Close();
    		}
    	}
     }

#endif //RD_MULTIPLE_DRIVE	
	RDEBUG("	MMC format marker file created successfully! MMC wipe enabled.");
	CleanupStack::PopAndDestroy( &rfs );
	}
	

// ----------------------------------------------------------------------------------------
// End of file
