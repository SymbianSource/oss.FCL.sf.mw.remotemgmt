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
* Description:  Multiple context manager.
*
*/


//  INCLUDE FILES
#include "CWPBindingContextManager.h"
#include <d32dbms.h>
#include <sysutil.h>
#include "MWPContextExtension.h"
#include "MWPContextExtensionArray.h"
#include "WPEngineDebug.h"
#include "ProvisioningDebug.h"


// CONSTANTS
/// Size of the database grows 16 bytes plus the actual data. Tested empirically.
const TInt KNewSaveDataSizeEstimate = 16;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPBindingContextManager::CWPBindingContextManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPBindingContextManager::CWPBindingContextManager()
	{
	}

// -----------------------------------------------------------------------------
// CWPBindingContextManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPBindingContextManager::ConstructL()
	{
    CWPMultiContextManager::ConstructL();
	}

// -----------------------------------------------------------------------------
// CWPBindingContextManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPBindingContextManager* CWPBindingContextManager::NewL()
	{
	CWPBindingContextManager* self = NewLC();
	CleanupStack::Pop();
	
	return self;
	}

// -----------------------------------------------------------------------------
// CWPBindingContextManager::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPBindingContextManager* CWPBindingContextManager::NewLC()
	{
	CWPBindingContextManager* self = new( ELeave ) CWPBindingContextManager;
	
	CleanupStack::PushL( self );
	self->ConstructL();
	
	return self;
	}

// Destructor
CWPBindingContextManager::~CWPBindingContextManager()
	{
	}

// -----------------------------------------------------------------------------
// CWPBindingContextManager::DeleteContextL
// -----------------------------------------------------------------------------
//
void CWPBindingContextManager::DeleteContextL( MWPContextExtensionArray& aArray, 
                                              TUint32 aUid )
	{
    CheckDatabaseL();

    // Make sure that all context data has been deleted
    while( DeleteContextDataL( aArray, aUid ) )
    	{
    	}

    // Delete the actual context
    CWPMultiContextManager::DeleteContextL( aArray, aUid );
    }

// -----------------------------------------------------------------------------
// CWPBindingContextManager::DeleteContextDataL
// -----------------------------------------------------------------------------
//
TBool CWPBindingContextManager::DeleteContextDataL( 
    MWPContextExtensionArray& aArray, TUint32 aUid )
	{
    CheckDatabaseL();

    // Call DeleteL() for all related adapter extensions
    RDbTable table;
    OpenLC( KDbTableData, table );
    CDbColSet* colset = table.ColSetL();
    TInt adapterIdCol( colset->ColNo( KDbColumnDataAdapterId ) );
    TInt saveDataCol( colset->ColNo( KDbColumnDataSaveData ) );
    delete colset;
    
    User::LeaveIfError( table.SetIndex( KDbIndexDataContextId ) );
    TBool found( table.SeekL( TDbSeekKey( TUint( aUid ) ) ) );
    if( found )
        {
        table.GetL();
        TUint32 adapterId( table.ColUint32( adapterIdCol ) );
        TPtrC8 saveData( table.ColDes8( saveDataCol ) );
        
        for( TInt i( 0 ); i < aArray.MwpceCount(); i++ )
            {
            if( aArray.MwpcePoint( i ).Uid() == adapterId )
                {
                DVA( "DeleteContextDataL: Deleting data using adapter %08x", 
                    adapterId );
                aArray.MwpcePoint( i ).DeleteL( saveData );
                DVA( "DeleteContextDataL: Deleted data using adapter %08x", 
                    adapterId );
                }
            }
        table.DeleteL();
        }

    CleanupStack::PopAndDestroy(); // table
    return found;
    }

// -----------------------------------------------------------------------------
// CWPBindingContextManager::ContextDataCountL
// -----------------------------------------------------------------------------
//
TInt CWPBindingContextManager::ContextDataCountL( TUint32 aUid )
	{
	FLOG( _L( "[Provisioning] CWPBindingContextManager::ContextDataCountL:" ) );
    CheckDatabaseL();

    // Call DeleteL() for all related adapter extensions
    RDbTable table;
    OpenLC( KDbTableData, table, RDbRowSet::EReadOnly );
    CDbColSet* colset = table.ColSetL();
    TInt contextId( colset->ColNo( KDbColumnDataContextId ) );
    delete colset;
    
    User::LeaveIfError( table.SetIndex( KDbIndexDataContextId ) );
    table.SeekL( TDbSeekKey( TUint( aUid ) ) );
    TInt found( 0 );
    while( table.AtRow() && 
        (table.GetL(), table.ColUint32( contextId ) == aUid ) )
        {
        found++;
        table.NextL();
        }
    FTRACE(RDebug::Print(_L("[Provisioning] CWPBindingContextManager::ContextDataCountL count (%d)"), found));
    CleanupStack::PopAndDestroy(); // table
    return found;
    }

// -----------------------------------------------------------------------------
// CWPBindingContextManager::SaveL
// -----------------------------------------------------------------------------
//
void CWPBindingContextManager::SaveL( 
    MWPContextExtension& aExtension, TInt aItem )
	{
	FLOG( _L( "[Provisioning] CWPBindingContextManager::SaveL:" ) );
	
    CheckDatabaseL();

    // Grab save data only if a context is active
    if( CurrentContextL() != KWPMgrUidNoContext )
        {
        FLOG( _L( "[Provisioning] CWPBindingContextManager::SaveL: 1" ) );
        TPtrC8 saveData( aExtension.SaveDataL( aItem ) );
        if( saveData != KNullDesC8 )
            {
            if( SysUtil::FFSSpaceBelowCriticalLevelL( 
                NULL, KNewSaveDataSizeEstimate+saveData.Length() ) )
                {
                User::Leave( KErrDiskFull );
                }

            RDbTable table;
            OpenLC( KDbTableData, table );

            CDbColSet* colset = table.ColSetL();
            TInt contextIdCol( colset->ColNo( KDbColumnDataContextId ) );
            TInt adapterIdCol( colset->ColNo( KDbColumnDataAdapterId ) );
            TInt saveDataCol( colset->ColNo( KDbColumnDataSaveData ) );
            delete colset;

            DatabaseBeginLC();

            InsertBeginLC( table );
            table.SetColL( contextIdCol, CurrentContextL() );
            table.SetColL( adapterIdCol, aExtension.Uid() );
            table.SetColL( saveDataCol, saveData );
            InsertCommitLP( table );
            
            DatabaseCommitLP();
            CleanupStack::PopAndDestroy(); // table
            }
        }
    FLOG( _L( "[Provisioning] CWPBindingContextManager::SaveL: done" ) );
	}

//  End of File  
