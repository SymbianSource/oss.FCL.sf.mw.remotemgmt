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
* Description:  Utility functions for plug-ins
*
*/


#ifndef DIAGPLUGINBASEUTILS_H
#define DIAGPLUGINBASEUTILS_H

// INCLUDES

// FORWARD DECLARATIONS
class RConeResourceLoader;

// CONSTANTS

/**
*  Diagnostics Plugin Base Utility Functions
*
*   Collection of various useful functions.
*
*  @since S60 v5.0
*/

namespace DiagPluginBaseUtils
    {
void OpenResourceFileL( const TDesC& aResourceFileName,
                        RConeResourceLoader& aResourceLoader,
                        RFs& aFsSession );
    }   // namespace DiagPluginBaseUtils

#endif // DIAGPLUGINBASEUTILS_H

// End of File

