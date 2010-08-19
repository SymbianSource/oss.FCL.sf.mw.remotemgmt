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
* Description:  Example Device Info Extension Data Container Plugin source file.
*
*/

// EXTERNAL INCLUDES
#include <e32base.h>

// CLASS HEADER
#include "exampledevinfextdatacontainerplugin.h"

// -----------------------------------------------------------------------------
// CExampleDevInfExtDataContainerPlugin::CExampleDevInfExtDataContainerPlugin()
// -----------------------------------------------------------------------------
CExampleDevInfExtDataContainerPlugin::CExampleDevInfExtDataContainerPlugin()
    {
    }

// -----------------------------------------------------------------------------
// void CExampleDevInfExtDataContainerPlugin::ConstructL()
// -----------------------------------------------------------------------------
void CExampleDevInfExtDataContainerPlugin::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CExampleDevInfExtDataContainerPlugin* CExampleDevInfExtDataContainerPlugin::NewL()
// -----------------------------------------------------------------------------
CExampleDevInfExtDataContainerPlugin* CExampleDevInfExtDataContainerPlugin::NewL()
    {
    CExampleDevInfExtDataContainerPlugin* self = 
        new (ELeave) CExampleDevInfExtDataContainerPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CExampleDevInfExtDataContainerPlugin::~CExampleDevInfExtDataContainerPlugin()
// -----------------------------------------------------------------------------
CExampleDevInfExtDataContainerPlugin::~CExampleDevInfExtDataContainerPlugin()
    {
    for( TInt i = 0; i < iExtensionArray.Count(); i++ )
        {
        iExtensionArray[ i ].iXValArray.Close();
        }
    iExtensionArray.Close();
    }

// -----------------------------------------------------------------------------
// TInt CExampleDevInfExtDataContainerPlugin::GetExtensionCountL()
// -----------------------------------------------------------------------------
TInt CExampleDevInfExtDataContainerPlugin::GetExtensionCountL()
    {
    return iExtensionArray.Count();
    }

// -----------------------------------------------------------------------------
// const TDesC8& CExampleDevInfExtDataContainerPlugin::GetExtNameL()
// -----------------------------------------------------------------------------
const TDesC8& CExampleDevInfExtDataContainerPlugin::GetExtNameL( TInt aExtIndex )
    {
    if( aExtIndex < 0 || aExtIndex >= iExtensionArray.Count() )
        {
        // Illegal index
        User::Leave( KErrArgument );
        }

    return iExtensionArray[ aExtIndex ].iXNam;
    }

// -----------------------------------------------------------------------------
// TInt CExampleDevInfExtDataContainerPlugin::GetExtValueCountL()
// -----------------------------------------------------------------------------
TInt CExampleDevInfExtDataContainerPlugin::GetExtValueCountL( TInt aExtIndex )
    {
    if( aExtIndex < 0 || aExtIndex >= iExtensionArray.Count() )
        {
        // Illegal index
        User::Leave( KErrArgument );
        }

    return iExtensionArray[ aExtIndex ].iXValArray.Count();
    }

// -----------------------------------------------------------------------------
// const TDesC8& CExampleDevInfExtDataContainerPlugin::GetExtValueL()
// -----------------------------------------------------------------------------
const TDesC8& CExampleDevInfExtDataContainerPlugin::GetExtValueL( TInt aExtIndex, 
    TInt aValueIndex )
    {

    // Check for illegal indices
    if( aExtIndex < 0 || aExtIndex >= iExtensionArray.Count() )
        {
        User::Leave( KErrArgument );
        }
    if( aValueIndex < 0 || 
        aValueIndex >= iExtensionArray[ aExtIndex ].iXValArray.Count() )
        {
        User::Leave( KErrArgument );
        }

    return iExtensionArray[ aExtIndex ].iXValArray[ aValueIndex ];
    }

// -----------------------------------------------------------------------------
// void CExampleDevInfExtDataContainerPlugin::ClearExtensions()
// -----------------------------------------------------------------------------
void CExampleDevInfExtDataContainerPlugin::ClearExtensions()
    {
    for( TInt i = 0; i < iExtensionArray.Count(); i++ )
        {
        iExtensionArray[ i ].iXValArray.Reset();
        }
    iExtensionArray.Reset();
    }

// -----------------------------------------------------------------------------
// void CExampleDevInfExtDataContainerPlugin::InsertExtension()
// -----------------------------------------------------------------------------
void CExampleDevInfExtDataContainerPlugin::InsertExtension(
    const TDesC8& aXNam, const TDesC8& aXVal )
    {
    TInt index = FindExtensionIndex( aXNam );
    if( index != KErrNotFound )
        {
        AddXValToItem( index, aXVal );
        }
    else
        {
        AddNewXValItem( aXNam, aXVal );
        }
    }

// -----------------------------------------------------------------------------
// void CExampleDevInfExtDataContainerPlugin::RemoveExtension()
// -----------------------------------------------------------------------------
void CExampleDevInfExtDataContainerPlugin::RemoveExtension( const TDesC8& aXNam )
    {
    TInt index = FindExtensionIndex( aXNam );
    if( index != KErrNotFound )
        {
        iExtensionArray.Remove( index );    
        }
    }

// -----------------------------------------------------------------------------
// void CExampleDevInfExtDataContainerPlugin::FindExtensionIndex()
// -----------------------------------------------------------------------------
TInt CExampleDevInfExtDataContainerPlugin::FindExtensionIndex( 
    const TDesC8& aXNam )
    {
    TInt index = KErrNotFound;

    for( TInt i = 0; i < iExtensionArray.Count(); i++ )
        {
        TExampleDevInfExtExtensionItem item = iExtensionArray[ i ];
        if( aXNam.Compare( item.iXNam ) == 0 )
            {
            index = i;
            break;
            }
        }
    return index;
    }

// -----------------------------------------------------------------------------
// void CExampleDevInfExtDataContainerPlugin::AddXValToItem()
// -----------------------------------------------------------------------------
void CExampleDevInfExtDataContainerPlugin::AddXValToItem(
        const TInt aIndex, const TDesC8& aXVal )
    {
    ASSERT( aIndex >= 0 );
    ASSERT( aIndex < iExtensionArray.Count() );

    if( aXVal.Length() == 0 )
        {
        // No XVal set, do not insert anything to XVal array
        return;
        }
    else if( aXVal.Length() <= KExtensionElementMaxLength )
        {
        iExtensionArray[ aIndex ].iXValArray.Append( aXVal );
        }
    else
        {
        iExtensionArray[ aIndex ].iXValArray.Append( 
            aXVal.Left( KExtensionElementMaxLength ) );
        }
    }

// -----------------------------------------------------------------------------
// void CExampleDevInfExtDataContainerPlugin::AddNewXValItem()
// -----------------------------------------------------------------------------
void CExampleDevInfExtDataContainerPlugin::AddNewXValItem( 
    const TDesC8& aXNam, const TDesC8& aXVal )
    {    
    TExampleDevInfExtExtensionItem item;

    // Insert XNam
    if( aXNam.Length() <= KExtensionElementMaxLength )
        {
        item.iXNam = aXNam;
        }
    else
        {
        item.iXNam = aXNam.Left( KExtensionElementMaxLength );
        }

    // Insert XVal
    if( aXVal.Length() > 0 )
        {
        if( aXVal.Length() <= KExtensionElementMaxLength )
            {
            item.iXValArray.Append( aXVal );
            }
        else
            {
            item.iXValArray.Append( aXVal.Left( KExtensionElementMaxLength ) );
            }
        }

    // Add to extension array as a new entry
    iExtensionArray.Append( item );
    }

// End of file        
