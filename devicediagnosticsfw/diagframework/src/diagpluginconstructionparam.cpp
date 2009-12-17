/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Class definition of CDiagPluginConstructionParam
*
*/


// CLASS DECLARATION
#include <DiagPluginConstructionParam.h>

// SYSTEM INCLUDE FILES
#include <badesca.h>            // CDesC16ArrayFlat

// USER INCLUDE FILES

// CONSTANTS
_LIT( KDiagPluginBlankServiceName, "" );

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginConstructionParam* CDiagPluginConstructionParam::NewL(
                                        HBufC*              aServiceProvided,
                                        CDesC16ArrayFlat*   aServicesRequired,
                                        TInt                aOrder,
                                        TUid                aImplUid,
                                        TUid                aParentUid )
    {
    CDiagPluginConstructionParam* self = CDiagPluginConstructionParam::NewLC( 
                                        aServiceProvided,
                                        aServicesRequired,
                                        aOrder,
                                        aImplUid,
                                        aParentUid );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::NewLC()
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginConstructionParam* CDiagPluginConstructionParam::NewLC(
                                        HBufC*              aServiceProvided,
                                        CDesC16ArrayFlat*   aServicesRequired,
                                        TInt                aOrder,
                                        TUid                aImplUid,
                                        TUid                aParentUid )
    {
    CDiagPluginConstructionParam* self = new( ELeave )CDiagPluginConstructionParam(
                                        aServiceProvided,
                                        aServicesRequired,
                                        aOrder,
                                        aImplUid,
                                        aParentUid );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::CDiagPluginConstructionParam
// ---------------------------------------------------------------------------
//
CDiagPluginConstructionParam::CDiagPluginConstructionParam(
        HBufC*              aServiceProvided,
        CDesC16ArrayFlat*   aServicesRequired,
        TInt                aOrder,
        TUid                aImplUid,
        TUid                aParentUid )
    :   iServiceProvided    ( aServiceProvided ),
        iServicesRequired   ( aServicesRequired ),
        iOrder              ( aOrder ),
        iImplUid            ( aImplUid ),
        iParentUid          ( aParentUid )
    {
    }

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::~CDiagPluginConstructionParam
// ---------------------------------------------------------------------------
//
EXPORT_C CDiagPluginConstructionParam::~CDiagPluginConstructionParam()
    {
    delete iServiceProvided;
    iServiceProvided = NULL;

    delete iServicesRequired;
    iServicesRequired = NULL;
    }

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::ServiceProvided
// ---------------------------------------------------------------------------
//
EXPORT_C const TDesC& CDiagPluginConstructionParam::ServiceProvided() const
    {
    if ( iServiceProvided )
        {
        return *iServiceProvided;
        }
    else
        {
        // there is no service name. Because we must return a reference,
        // we should at least return a blank text.
        return KDiagPluginBlankServiceName();
        }
    }

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::ServicesRequired
// ---------------------------------------------------------------------------
//
EXPORT_C const CDesC16ArrayFlat& CDiagPluginConstructionParam::ServicesRequired() const
    {
    return *iServicesRequired;
    }

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::Order
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CDiagPluginConstructionParam::Order() const
    {
    return iOrder;
    }

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::Uid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CDiagPluginConstructionParam::Uid() const
    {
    return iImplUid;
    }

// ---------------------------------------------------------------------------
// CDiagPluginConstructionParam::ParentUid
// ---------------------------------------------------------------------------
//
EXPORT_C TUid CDiagPluginConstructionParam::ParentUid() const
    {
    return iParentUid;
    }

// End of File

