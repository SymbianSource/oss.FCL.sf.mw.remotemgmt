/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Example Device Info Extension Data Container Plugin 
*  header file.
*
*/

#ifndef __EXAMPLEDEVINFEXTDATACONTAINERPLUGIN_H__
#define __EXAMPLEDEVINFEXTDATACONTAINERPLUGIN_H__

// EXTERNAL INCLUDES
#include <nsmldevinfextdatacontainerplugin.h>

// FORWARD DECLARATIONS
class CDesCArrayFlat;

//  CONSTANTS
const TUint KExampleDevInfExtDataContainerImplUid = 0x2002DC7D;
const TInt KExtensionElementMaxLength = 50;

// CLASS DEFINITION
/**
 * Type definition for TExampleDevInfExtExtensionItem, which holds
 * the contents for a single extension (i.e. XNam value + 0..N 
 * XVal values). Note that this type is only for testing purposes,
 * e.g. the maximum length of the XNam/XVal elements is set to 
 * 50 characters.
 */
NONSHARABLE_CLASS( TExampleDevInfExtExtensionItem )
    {
    public:
        TBuf8< KExtensionElementMaxLength > iXNam;
        RArray< TBuf8 < KExtensionElementMaxLength > > iXValArray;
    };


// CLASS DEFINITION
/**
 * CExampleDevInfExtDataContainerPlugin is an example plugin,
 * which implements the CNSmlDevInfExtDataContainerPlugin ECom interface.
 * The example plugin is primarily used for unit test purposes.
 * It allows modifying the XNam & XVal entries stored in the plugin,
 * which in turn makes it possible to test the plugin interface
 * with different configurations.
 * 
 * Note that the maximum length for the XNam & XVal is limited to
 * 50 characters; with longer strings the values become truncated
 * to 50 characters.
 */
class CExampleDevInfExtDataContainerPlugin : 
    public CNSmlDevInfExtDataContainerPlugin
    {

    public:  // Constructor(s) and destructor
        ~CExampleDevInfExtDataContainerPlugin();
        CExampleDevInfExtDataContainerPlugin();
        void ConstructL();
        static CExampleDevInfExtDataContainerPlugin* NewL();
    
    public: // From CNSmlDevInfExtDataContainerPlugin
        TInt GetExtensionCountL();
        const TDesC8& GetExtNameL( TInt aExtIndex );
        TInt GetExtValueCountL( TInt aExtIndex );
        const TDesC8& GetExtValueL( TInt aExtIndex, TInt aValueIndex );

    public:  // New methods
        /**
         * Removes all extension definitions currently configured
         *  to the plugin. After a call to this method, the extension
         *  count is zero.
         */
        void ClearExtensions();
        
        /**
         * Inserts a new extension with the given XNam name and XVal value
         * to the plugin. If there are multiple XVal to be defined per
         * XNam, this method shall be called multiple times with the same
         * XNam value.
         * 
         * @param aXNam XNam value for the extension to be added.
         * @param aXVal XVal value for the extension to be added.
         */
        void InsertExtension( const TDesC8& aXNam, const TDesC8& aXVal );
        
        /**
         * Removes the given extensions from the Plugin. If an extension with
         * the given XNam name is not found, does nothing.
         * 
         * @param aXNam The XNam value for the extension to be removed.
         */
        void RemoveExtension( const TDesC8& aXNam );

    private:  // Private utility methods
        TInt FindExtensionIndex( const TDesC8& aXNam );
        void AddXValToItem( const TInt aIndex, const TDesC8& aXVal );
        void AddNewXValItem( const TDesC8& aXNam, const TDesC8& aXVal );
        
    private:  // Data
        RArray< TExampleDevInfExtExtensionItem > iExtensionArray;

    };
        
#endif // __EXAMPLEDEVINFEXTDATACONTAINERPLUGIN_H__
