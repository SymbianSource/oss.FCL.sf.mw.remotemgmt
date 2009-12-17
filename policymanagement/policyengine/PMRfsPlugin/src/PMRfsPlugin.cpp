/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
*
*/


// INCLUDE FILES
#include "PMRfsPlugin.h"

#include <rfsClient.h>

#include <PolicyEngineClient.h>
#include "DMUtilClient.h"
#include "CentRepToolClient.h"
#include "debug.h"
#ifdef RD_MULTIPLE_DRIVE
#include <driveinfo.h>
#include <pathinfo.h>
#include <s32file.h>
#endif //RD_MULTIPLE_DRIVE

#ifndef RD_MULTIPLE_DRIVE
_LIT( KScriptFormat_E, "z:\\private\\1020783F\\format_e_.txt");
#endif
_LIT( KDMUtilServerPrivatePath, "\\private\\10207843\\" );
_LIT( MDriveColon, ":");
_LIT( KMMCEraseFlagFileName, "fmmc.dat");

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPMRfsPlugin::CPMRfsPlugin()
    {
    RDEBUG("CPMRfsPlugin::CPMRfsPlugin()");
    }

CPMRfsPlugin::CPMRfsPlugin(TAny* /*aInitParams*/)
    {
    RDEBUG("CPMRfsPlugin::CPMRfsPlugin()");
    }

// Destructor
CPMRfsPlugin::~CPMRfsPlugin()
    {
   	RDEBUG("CPMRfsPlugin::~CPMRfsPlugin()");
    }

// ---------------------------------------------------------
// NewL
// ---------------------------------------------------------
//
CPMRfsPlugin* CPMRfsPlugin::NewL(TAny* aInitParams)
    {
    RDEBUG("CPMRfsPlugin::NewL()");
    CPMRfsPlugin* self = new (ELeave) CPMRfsPlugin(aInitParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CPMRfsPlugin::ConstructL()
    {
    RDEBUG("CPMRfsPlugin::ConstructL()");
    }

void CPMRfsPlugin::RestoreFactorySettingsL( const TRfsReason aType )
    {
    RDEBUG("CPMRfsPlugin::RestoreFactorySettingsL()");
    
    if( aType == ENormalRfs )
    	{
    	//Perform DMUtil RFS
    	RDEBUG("	-> CPMRfsPlugin: Perform DMUtil RFS ... ");
    	RDMUtil dmutil;
		User::LeaveIfError( dmutil.Connect() );
		CleanupClosePushL( dmutil );
    	User::LeaveIfError( dmutil.PerformRFS() );
		CleanupStack::PopAndDestroy( &dmutil );
		RDEBUG("	-> CPMRfsPlugin: Perform DMUtil RFS ... DONE!");
    	
    	//Perform CentRepToolClient RFS
    	RDEBUG("	-> CPMRfsPlugin: Perform CentRepToolClient RFS ... ");
		RCentRepTool centRepTool;
		User::LeaveIfError( centRepTool.Connect() );
		CleanupClosePushL( centRepTool );
		User::LeaveIfError( centRepTool.PerformRFS() );
		CleanupStack::PopAndDestroy( &centRepTool );
		RDEBUG("	-> CPMRfsPlugin: Perform CentRepToolClient RFS ... DONE!");
		
		//Perform PolicyEngine RFS
		RDEBUG("	-> CPMRfsPlugin: Perform PolicyEngine RFS ... ");
		RPolicyEngine policyEngine;
		User::LeaveIfError( policyEngine.Connect() );
		CleanupClosePushL( policyEngine );
		User::LeaveIfError( policyEngine.PerformRFS() );
		CleanupStack::PopAndDestroy( &policyEngine );
		RDEBUG("	-> CPMRfsPlugin: Perform PolicyEngine RFS ... DONE!");
    	}
    	
    RDEBUG("CPMRfsPlugin::RestoreFactorySettingsL() ... DONE!");
	}

void CPMRfsPlugin::GetScriptL( const TRfsReason /*aType*/, TDes& aPath )
    {
    RDEBUG("CPMRfsPlugin::GetScriptL()");
    
        
    if( IsMMCFormatFlagEnabledL() )
        {
        RDEBUG("CPMRfsPlugin::GetScriptL() - Format MMC flag enabled");
    #ifndef RD_MULTIPLE_DRIVE
        aPath.Copy( KScriptFormat_E );
    #else
    	aPath.Copy( iFileName);
    	
    #endif
        }
    else
        {
        RDEBUG("CPMRfsPlugin::GetScriptL() - Format MMC flag disabled");
        }
    }

void CPMRfsPlugin::ExecuteCustomCommandL( const TRfsReason /*aType*/,
                                        TDesC& /*aCommand*/ )
    {
    RDEBUG("CPMRfsPlugin::ExecuteCustomCommandL()");
    }
     


TBool CPMRfsPlugin::IsMMCFormatFlagEnabledL()
	{
	RDEBUG("CPMRfsPlugin::IsMMCFormatFlagEnabledL()");
	
	RFs rfs;
	User::LeaveIfError( rfs.Connect() );
	CleanupClosePushL( rfs );	
#ifndef RD_MULTIPLE_DRIVE
	TFileName fullPath;
	fullPath.Append( _L("e") );
	fullPath.Append( MDriveColon );	
	fullPath.Append( KDMUtilServerPrivatePath );
	fullPath.Append( KMMCEraseFlagFileName );
		
	RFile file;
	TInt err = file.Open( rfs, fullPath, EFileRead );
	file.Close();
	
	CleanupStack::PopAndDestroy( &rfs );
	
	if( err == KErrNone )
		{
		RDEBUG("CPMRfsPlugin::IsMMCFormatFlagEnabled() - returned: ETrue");
		return ETrue;
		}
	
	RDEBUG_2("CPMRfsPlugin::IsMMCFormatFlagEnabled() - returned: EFalse: %d)", err);
	return EFalse;
#else
	TDriveList driveList;
	TInt driveCount;
	TBool fileReplace = EFalse;
	TInt err = KErrNone;
	
	//Get all removeable drive, both physically and logically
	User::LeaveIfError(DriveInfo::GetUserVisibleDrives(
            rfs,  driveList,  driveCount, KDriveAttRemovable ));
    
    TInt max(driveList.Length());
    
    RFileWriteStream file;
    for(TInt i=0; i<max;++i)
    {
    	if (driveList[i])
    	{
    		TUint status;
    		DriveInfo::GetDriveStatus(rfs, i, status);
    		//To make sure the drive is physically removeable not logically removeable	
    		//need to format internal mass memory also. So no need to check the below condition
    		//if (status & DriveInfo::EDriveRemovable)
    		{
    			TChar driveLetter;
    			rfs.DriveToChar(i,driveLetter);
				
				TFileName fullPath;
				fullPath.Append( driveLetter );
				fullPath.Append( MDriveColon );
				fullPath.Append( KDMUtilServerPrivatePath );
				fullPath.Append( KMMCEraseFlagFileName );
				
				RFile flagfile;
				err = flagfile.Open( rfs, fullPath, EFileRead );
				flagfile.Close();

				if( err == KErrNone )
				{
					if (!fileReplace)
					{
						User::LeaveIfError(file.Temp(rfs, _L("C:\\"),iFileName, EFileWrite));	
						file.WriteInt16L(65279);
						CleanupClosePushL(file);  
							
						fileReplace = ETrue;
						RDEBUG("CPMRfsPlugin::IsMMCFormatFlagEnabled() - File created on private path");
					}
			
					file.WriteL(_L("FORMAT"));
					file.WriteL(_L(" "));
					TBuf<4> DriveLetterBuf;
					DriveLetterBuf.Append(driveLetter);
					file.WriteL(DriveLetterBuf);
					file.WriteL(_L(":"));
					file.WriteL(_L("\r\n"));
					
				}
					
			}
    	}
     }
     
   	if (fileReplace)
   	{
   		file.CommitL();
   		CleanupStack::PopAndDestroy(); // pop file
   	}
		
	CleanupStack::PopAndDestroy( &rfs );	
     if (fileReplace)
     {
     	RDEBUG("CPMRfsPlugin::IsMMCFormatFlagEnabled() - returned: ETrue:");
     	return ETrue;
     }
     else
     {
     	RDEBUG_2("CPMRfsPlugin::IsMMCFormatFlagEnabled() - returned: EFalse: %d)", err);
     	return EFalse;
     }
    

#endif //RD_MULTIPLE_DRIVE
	}
// End of file
