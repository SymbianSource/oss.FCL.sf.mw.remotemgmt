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
* Description:  Collection of utility functions useful for plugins
*
*/


// SYSTEM INCLUDE FILES
#include <bautils.h>                        // BaflUtils
#include <ConeResLoader.h>                  // RConeResourceLoader
#include <f32file.h>                        // RFs
#include <data_caging_path_literals.hrh>    // KDC_RESOURCE_FILES_DIR
#include <DiagFrameworkDebug.h>             // LOGSTRING

// USER INCLUDE FILES
#include "diagpluginbaseutils.h"        // CDiagPluginUtils

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// DiagPluginBaseUtils::OpenResourceFileL()
// ---------------------------------------------------------------------------
//
void DiagPluginBaseUtils::OpenResourceFileL(
        const TDesC& aResourceFileName,
        RConeResourceLoader& aResourceLoader,
        RFs& aFsSession )
    {
    // Find the resource file:
    TParse parse;
    parse.Set( aResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL );
    TFileName fileName( parse.FullName() );

    // Get language of resource file:
    BaflUtils::NearestLanguageFile( aFsSession, fileName );

    LOGSTRING( "CDiagPluginBaseUtils::OpenLocalizedResourceFileL: FileName:" )
    LOGTEXT( fileName )

    // Open resource file:
    TRAPD( err, aResourceLoader.OpenL( fileName ) )

#ifdef _DEBUG
    if ( err != KErrNone )
        {
        // if it is a debug build, and it fails to load the resource, try 
        // again with C: as base directory.
        // This allows installation of plug-ins as SIS file without changing
        // code in the plug-in itself.
        _LIT( KCDriveName, "c" );
        fileName.Replace( 0, 1, KCDriveName );

        LOGSTRING( "CDiagPluginBaseUtils::OpenLocalizedResourceFileL: Retry:" )
        LOGTEXT( fileName )
        TRAP( err, aResourceLoader.OpenL( fileName ) )
        }
#endif // _DEBUG

    if ( err != KErrNone )
        {
        LOGSTRING2( "CDiagPluginBaseUtils::OpenLocalizedResourceFileL:"
            L" Failed err = %d", err )
        User::Leave( err );
        }

    LOGSTRING( "CDiagPluginBaseUtils::OpenLocalizedResourceFileL: Resource Opened" )
    }

// End of File

