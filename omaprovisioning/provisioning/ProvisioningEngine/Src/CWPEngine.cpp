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
* Description:  Engine class for Provisioning.
*
*/


//  INCLUDE FILES
#include "CWPEngine.h"
#include <badesca.h>
#include "CWPWbxmlParser.h"
#include "CWPEngine.pan"
#include "CWPRoot.h"
#include "CWPValidator.h"
#include "CWPAdapterManager.h"
#include "MWPContextManager.h"
#include "WPContextManagerFactory.h"
#include "WPEngineDebug.h"
#include "CWPPushMessage.h"
#include "ProvisioningDebug.h"

// CONSTANTS
const TInt KStreamVersion = 1;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPEngine::CWPEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPEngine::CWPEngine():iAccessDenied(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CWPEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPEngine::ConstructL()
    {
    FLOG( _L( "[ProvisioningEngine] CWPEngine::ConstructL" ) );
    
    iRoot = CWPRoot::NewL();
    // Create a context manager
    iManager = WPContextManagerFactory::CreateL();
    FLOG( _L( "[ProvisioningEngine] CWPEngine::ConstructL done" ) );        
    }

// -----------------------------------------------------------------------------
// CWPEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPEngine* CWPEngine::NewL()
    {
    CWPEngine* self = NewLC();
    CleanupStack::Pop();
    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPEngine::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CWPEngine* CWPEngine::NewLC()
    {
    CWPEngine* self = new( ELeave ) CWPEngine;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPEngine::~CWPEngine()
    {
    FLOG( _L( "[ProvisioningEngine] CWPEngine::~CWPEngine" ) );
    delete iManager;
    delete iAdapters;
    delete iRoot;
    }

// -----------------------------------------------------------------------------
// CWPEngine::RestoreL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::RestoreL( CStreamStore& aStore, TStreamId aId )
    {
    FLOG( _L( "[ProvisioningEngine] CWPEngine::RestoreL" ) );
    
    RStoreReadStream instream;
    instream.OpenLC(aStore,aId);
    InternalizeL(instream);
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CWPEngine::StoreL
// -----------------------------------------------------------------------------
//
EXPORT_C TStreamId CWPEngine::StoreL( CStreamStore& aStore ) const
    {
    FLOG( _L( "[ProvisioningEngine] CWPEngine::StoreL" ) );
    
    RStoreWriteStream stream;
    TStreamId id=stream.CreateLC(aStore);
    ExternalizeL(stream);
    stream.CommitL();
    CleanupStack::PopAndDestroy();
    return id;
    }

// -----------------------------------------------------------------------------
// CWPEngine::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::InternalizeL( RReadStream& aStream )
    {
    if( aStream.ReadInt32L() != KStreamVersion )
        {
        User::Leave( KErrNotSupported );
        }

    iRoot->InternalizeL( aStream );
    CreateLinksL();
    }

// -----------------------------------------------------------------------------
// CWPEngine::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( KStreamVersion );
    iRoot->ExternalizeL( aStream );
    }


// -----------------------------------------------------------------------------
// CWPEngine::AcceptL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::AcceptL( MWPVisitor& aVisitor )
    {
    iRoot->AcceptL( aVisitor );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ImportDocumentL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::ImportDocumentL( const TDesC8& aDocument )
    {
    FLOG( _L( "[ProvisioningEngine] CWPEngine::ImportDocumentL begin" ) );
    
    #ifdef _DEBUG
    FHex(aDocument);
    #endif
    
    CWPWbxmlParser* parser = CWPWbxmlParser::NewL();
    CleanupStack::PushL( parser );
    parser->ParseL( aDocument, *iRoot );
    CleanupStack::PopAndDestroy(); // parser

    CWPValidator* validator = CWPValidator::NewLC();
    iRoot->AcceptL( *validator );
    CleanupStack::PopAndDestroy();

    CreateLinksL();
    FLOG( _L( "[ProvisioningEngine] CWPEngine::ImportDocumentL end" ) );

    }

// -----------------------------------------------------------------------------
// CWPEngine::PopulateL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::PopulateL()
    {
    FLOG( _L( "[ProvisioningEngine] CWPEngine::PopulateL" ) );

    // First remove the old adapters
    delete iAdapters;
    iAdapters = NULL;

    // Create the adapters
    AdapterManagerL().PopulateL( *iRoot );

    FLOG( _L( "[ProvisioningEngine] CWPEngine::PopulateL done" ) );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ItemCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPEngine::ItemCount() const
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPEngine::ItemCount: (%d)"), iAdapters->ItemCount()));

    return iAdapters->ItemCount();
    }

// -----------------------------------------------------------------------------
// CWPEngine::SummaryTitle
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC16& CWPEngine::SummaryTitle( TInt aIndex ) const
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPEngine::SummaryTitle: (%d)"), aIndex));

    return iAdapters->SummaryTitle(aIndex);
    }

// -----------------------------------------------------------------------------
// CWPEngine::SummaryText
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC16& CWPEngine::SummaryText( TInt aIndex ) const
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPEngine::SummaryText: (%d)"), aIndex));    

    return iAdapters->SummaryText(aIndex);
    }

// -----------------------------------------------------------------------------
// CWPEngine::DetailsL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPEngine::DetailsL( TInt aIndex, MWPPairVisitor& aVisitor )
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPEngine::DetailsL: (%d)"), aIndex));

    return AdapterManagerL().DetailsL(aIndex, aVisitor);
    }

// -----------------------------------------------------------------------------
// CWPEngine::SaveL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::SaveL( TInt aIndex )
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPEngine::SaveL: (%d)"), aIndex));
    
    AdapterManagerL().SaveL( *iManager, aIndex );
    }

// -----------------------------------------------------------------------------
// CWPEngine::CanSetAsDefault
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CWPEngine::CanSetAsDefault( TInt aIndex ) const
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPEngine::CanSetAsDefault: (%d)"), aIndex));

    return iAdapters->CanSetAsDefault(aIndex);
    }

// -----------------------------------------------------------------------------
// CWPEngine::SetAsDefaultL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::SetAsDefaultL( TInt aIndex )
    {
    FTRACE(RDebug::Print(_L("[Provisioning] CWPEngine::SetAsDefault: (%d)"), aIndex));
    
    AdapterManagerL().SetAsDefaultL(aIndex);
    }

// -----------------------------------------------------------------------------
// CWPEngine::CreateLinksL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::CreateLinksL()
    {
    iRoot->CreateLinksL();
    }

// -----------------------------------------------------------------------------
// CWPEngine::Builder
// -----------------------------------------------------------------------------
//
EXPORT_C MWPBuilder& CWPEngine::Builder() const
    {
    return *iRoot;
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextManager
// -----------------------------------------------------------------------------
//
MWPContextManager& CWPEngine::ContextManager() const
    {
    FLOG( _L( "[ProvisioningEngine] CWPEngine::ContextManager" ) );
    return *iManager;
    }

// -----------------------------------------------------------------------------
// CWPEngine::AdapterManager
// -----------------------------------------------------------------------------
//
CWPAdapterManager& CWPEngine::AdapterManagerL() 
    {
    if( !iAdapters )
        {
        iAdapters = CWPAdapterManager::NewL();
        }

    return *iAdapters;
    }

// -----------------------------------------------------------------------------
// CWPEngine::CreateContextL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CWPEngine::CreateContextL( const TDesC& aName, 
                                            const TDesC& aTPS, 
                                            const MDesC16Array& aProxies )
    {
    return iManager->CreateContextL( aName, aTPS, aProxies );
    }

// -----------------------------------------------------------------------------
// CWPEngine::DeleteContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::DeleteContextL( TUint32 aUid )
    {
    iManager->DeleteContextL( AdapterManagerL(), aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::DeleteContextDataL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CWPEngine::DeleteContextDataL( TUint32 aUid )
    {
    return iManager->DeleteContextDataL( AdapterManagerL(), aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextDataCountL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPEngine::ContextDataCountL( TUint32 aUid )
    {
    return iManager->ContextDataCountL( aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextUidsL
// -----------------------------------------------------------------------------
//
EXPORT_C CArrayFix<TUint32>* CWPEngine::ContextUidsL()
    {
    return iManager->ContextUidsL();
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextNameL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CWPEngine::ContextNameL( TUint32 aUid )
    {
    return iManager->ContextNameL( aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextTPSL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CWPEngine::ContextTPSL( TUint32 aUid )
    {
    return iManager->ContextTPSL( aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextProxiesL
// -----------------------------------------------------------------------------
//
EXPORT_C CDesCArray* CWPEngine::ContextProxiesL( TUint32 aUid )
    {
    return iManager->ContextProxiesL( aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextExistsL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CWPEngine::ContextExistsL( TUint32 aUid )
    {
    return iManager->ContextExistsL( aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextExistsL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CWPEngine::ContextExistsL( const TDesC& aTPS )
    {
    return iManager->ContextExistsL( aTPS );
    }

// -----------------------------------------------------------------------------
// CWPEngine::ContextL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CWPEngine::ContextL( const TDesC& aTPS )
    {
    return iManager->ContextL( aTPS );
    }

// -----------------------------------------------------------------------------
// CWPEngine::CurrentContextL
// -----------------------------------------------------------------------------
//
EXPORT_C TUint32 CWPEngine::CurrentContextL()
    {
    return iManager->CurrentContextL();
    }

// -----------------------------------------------------------------------------
// CWPEngine::SetCurrentContextL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::SetCurrentContextL( TUint32 aUid )
    {
    iManager->SetCurrentContextL( aUid );
    }

// -----------------------------------------------------------------------------
// CWPEngine::RegisterContextObserverL
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::RegisterContextObserverL( 
                                                MWPContextObserver* aObserver )
    {
    iManager->RegisterContextObserverL( aObserver );
    }

// -----------------------------------------------------------------------------
// CWPEngine::SetCurrentContextL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPEngine::UnregisterContextObserver( 
                                                MWPContextObserver* aObserver )
    {
    return iManager->UnregisterContextObserver( aObserver );
    }

// -----------------------------------------------------------------------------
// CWPEngine::SetAccessDenied
// -----------------------------------------------------------------------------
//
EXPORT_C void CWPEngine::SetAccessDenied(TBool aStatus)
    {
    iAccessDenied = aStatus;
    }

// -----------------------------------------------------------------------------
// CWPEngine::GetAccessDenied
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CWPEngine::GetAccessDenied()
    {
    return iAccessDenied;
    }
        
//  End of File  
