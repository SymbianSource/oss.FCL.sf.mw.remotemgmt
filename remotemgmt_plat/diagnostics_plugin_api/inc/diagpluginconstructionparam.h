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
* Description:  Diagnostics Plug-in Construction Parameter
*
*/


#ifndef DIAGPLUGINCONSTRUCTIONPARAM_H
#define DIAGPLUGINCONSTRUCTIONPARAM_H

// INCLUDES
#include <e32base.h>            // CBase

// FORWARD DECLARATIONS
class CDesC16ArrayFlat;

/**
* Parameters that are needed to construct a Diagnostics plug-in.
*
* @since S60 v5.0
**/
NONSHARABLE_CLASS( CDiagPluginConstructionParam ) : public CBase
    {
public:

    /**
    * Two-phased constructors.
    *
    * @param    aServiceProvided - Name of the service provided.
    *           Ownership is transferred to CDiagPluginConstructionParam.
    * @param    aServicesRequired - Array of services required.
    *           Ownership is transferred to CDiagPluginConstructionParam.
    * @param    aOrder - Order that this plug-in should appear in
    *           in parent suite.
    * @param    aImplUid - Implementation Uid of the plug-in.
    * @return   new instance of CDiagPluginConstructionParam*
    **/
    IMPORT_C static CDiagPluginConstructionParam* NewL(
                                        HBufC*              aServiceProvided,
                                        CDesC16ArrayFlat*   aServicesRequired,
                                        TInt                aOrder,
                                        TUid                aImplUid,
                                        TUid                aParentUid );
    IMPORT_C static CDiagPluginConstructionParam* NewLC(
                                        HBufC*              aServiceProvided,
                                        CDesC16ArrayFlat*   aServicesRequired,
                                        TInt                aOrder,
                                        TUid                aImplUid,
                                        TUid                aParentUid );

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CDiagPluginConstructionParam();

    /**
    * Get Name of the service provived.
    *
    * @return Name of the service provided.
    */
    IMPORT_C const TDesC& ServiceProvided() const;

    /**
    * Get array of services required for plug-in.
    *
    * @return Array of services required for plug-in.
    */
    IMPORT_C const CDesC16ArrayFlat& ServicesRequired() const;

    /**
    * Get the order this plug-in should appear in parent.
    *
    * @return Order that this plug-in should appear in parent.
    */
    IMPORT_C TInt Order() const;

    /**
    * Uid of plug-in.
    *
    * @return Uid of plug-in specified in RSS file.
    */
    IMPORT_C TUid Uid() const;

    /**
    * Parent Uid of plug-in.
    *
    * @return Parent Uid of plug-in specified in RSS file.
    */
    IMPORT_C TUid ParentUid() const;

private:    
    
    /**
    * C++ Constructor.
    **/
    CDiagPluginConstructionParam( HBufC*              aServiceProvided,
                                  CDesC16ArrayFlat*   aServicesRequired,
                                  TInt                aOrder,
                                  TUid                aImplUid,
                                  TUid                aParentUid );
    
    
private:    // data

    /**
    * iServiceProvided - Buffer containing service name
    * Owns.
    */
    HBufC* iServiceProvided;

    /**
    * iServicesRequired - Array containing list of dependency list.
    * Owns
    */
    CDesC16ArrayFlat* iServicesRequired;

    /**
    * iOrder - Order in parent list.
    */
    TInt iOrder;

    /**
    * iImplUid - Uid of the plug-in
    */
    TUid iImplUid;

    /**
    * iParentUid - Uid of the parent suite plug-ins
    */
    TUid iParentUid;
    };

#endif // DIAGPLUGINCONSTRUCTIONPARAM_H

// End of File

