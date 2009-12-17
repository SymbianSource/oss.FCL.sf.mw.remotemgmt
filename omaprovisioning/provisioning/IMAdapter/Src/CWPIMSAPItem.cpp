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
* Description: 
*     Settings item for wireless village settings, handles also the saving of
*     Settings item. 
*
*/


// INCLUDE FILES
#include "CWPIMSAPItem.h"


#include <cimpssapsettings.h>
#include <cimpssapsettingsstore.h>
#include <cimpssapsettingslist.h>
#include <CWPParameter.h>

// CONSTANTS
const TUint KLengthOfIncrementChars = 4;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CWPIMSAPItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPIMSAPItem* CWPIMSAPItem::NewLC( TIMPSAccessGroup aAccessGroup )
    {
	CWPIMSAPItem* self = new ( ELeave ) CWPIMSAPItem( aAccessGroup );
    CleanupStack::PushL( self );
    self->ConstructL();    
    return self;
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::CWPIMSAPItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPIMSAPItem::CWPIMSAPItem( TIMPSAccessGroup aAccessGroup )
                            : iAccessGroup( aAccessGroup )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWPIMSAPItem::~CWPIMSAPItem()
    {
    delete iWVSAPSettings;    
    delete iSAPStore;
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::ConstructL()
    {
    iWVSAPSettings = CIMPSSAPSettings::NewL();

    // create the SAPSettingsStore    
    TRAPD( err, iSAPStore = CIMPSSAPSettingsStore::NewL() );    
    
    if ( err == KErrCorrupt )
        {        
        // if corrupted, recreation creates also new db.
        iSAPStore = CIMPSSAPSettingsStore::NewL();
        }
    else
        {
        User::LeaveIfError(err);
        }
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::SetSettingsNameL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::SetSettingsNameL(const TDesC& aName)
    {
    iWVSAPSettings->SetSAPNameL( aName );
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::SetUserIDL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::SetUserIDL(const TDesC& aUserId)
    {
    iWVSAPSettings->SetSAPUserIdL( aUserId );
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::SetPasswordL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::SetPasswordL(const TDesC& aPassword)
    {
    iWVSAPSettings->SetSAPUserPasswordL(aPassword);
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::SetSAPURIL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::SetSAPURIL(const TDesC& aURI)
    {
    iWVSAPSettings->SetSAPAddressL( aURI);
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::SetIAPIdL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::SetIAPIdL(TUint32 aUid)
    {
    iWVSAPSettings->SetAccessPoint( aUid );
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::StoreL
// -----------------------------------------------------------------------------
//
TUint32 CWPIMSAPItem::StoreL()
    {
    FLOG( _L( "CWPIMSAPItem::StoreL: Enter" ) );
    TUint32 uid(0);
    // check is there already a server with a same name; if there is, 
    // increment the name. 
    if ( !IsServerNameUniqueL( iWVSAPSettings->SAPName() ) )
        {
        TInt length = iWVSAPSettings->SAPName().Length() + KLengthOfIncrementChars;
        
        HBufC* newName = HBufC::NewLC(length);
        TPtr namePtr = newName->Des();
        namePtr = iWVSAPSettings->SAPName();

        do 
            {
         IncrementNameL(namePtr);
            }
        while ( !IsServerNameUniqueL(namePtr) );
        
        iWVSAPSettings->SetSAPNameL(namePtr);
        
        CleanupStack::PopAndDestroy(newName);
        }

    // store the SAP item
    uid = iSAPStore->StoreNewSAPL( iWVSAPSettings, iAccessGroup );
    FLOG( _L( "CWPIMSAPItem::StoreL: Exit" ) );
    return uid;    
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::DeleteL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::DeleteL( TUint32 aUid )
	{
	TUint32 defaultUid = 0;
	TRAPD( error, iSAPStore->GetDefaultL( defaultUid, iAccessGroup ));
	if ( error != KErrNone )
		{
		// No default found, which is fine.
		// set default to 0 just in case
		defaultUid = 0;
		}
	TUint32 sapCount = iSAPStore->SAPCountL( iAccessGroup );

	if ( aUid == defaultUid &&  sapCount != 1 )
		{
		// SAP to be deleted is default SAP, need to set another to default
		CIMPSSAPSettingsList* list = CIMPSSAPSettingsList::NewLC();
		iSAPStore->PopulateSAPSettingsListL( *list, iAccessGroup );
		TUint32 tempUid = 0;
		for ( TUint32 i = 0; i < sapCount; i++ )
			{
			tempUid = list->UidForIndex( i );
			if ( tempUid != aUid )
				{
				iSAPStore->SetToDefaultL( tempUid, iAccessGroup );
				break;
				}
			}
		CleanupStack::PopAndDestroy( list );
		}

	// currently do not react to error situations
	TRAP( error, iSAPStore->DeleteSAPL( aUid ));
	}

// -----------------------------------------------------------------------------
// CWPIMSAPItem::IsServerNameUniqueL
// -----------------------------------------------------------------------------
//
TBool CWPIMSAPItem::IsServerNameUniqueL( const TDesC& aServerName )
    {
    TBool ret(ETrue);
    // create list for SAP items. 
    CIMPSSAPSettingsList* SAPList = CIMPSSAPSettingsList::NewLC();
    // populate the list with SAP settings
    iSAPStore->PopulateSAPSettingsListL( *SAPList, iAccessGroup );
    
    TInt storedCount = SAPList->MdcaCount();
    
    for( TInt index(0); index < storedCount; index++  ) 
        {
        SAPList->MdcaPoint( index );
        TInt SAPListUid = SAPList->UidForIndex( index );        
        // get list item
        const CIMPSSAPSettingsListItem* item = 
                                        SAPList->ListItemForUid( SAPListUid  );
        // compare is there already a server with same name. 
        if ( item && aServerName == item->Name() )
            {            
            // SAP setting name exists in list.
            ret = EFalse;
            break;
            }
        }
    CleanupStack::PopAndDestroy( SAPList );
    return ret;
    }

// -----------------------------------------------------------------------------
// CWPIMSAPItem::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::SetAsDefaultL( TUint32 aUid, TIMPSAccessGroup aAccessGroup )
    {
    // create the store
    CIMPSSAPSettingsStore* sapStore = CIMPSSAPSettingsStore::NewLC();
    // set SAPItem with aUid as default
    sapStore->SetToDefaultL( aUid, aAccessGroup );
    CleanupStack::PopAndDestroy( sapStore );
    }
 
void CWPIMSAPItem::IncrementNameL(TDes& aName)
    {

	FLOG( _L( "CWPIMSAPItem::IncrementNameL: Enter" ) );
    
    TInt length = aName.Length();
    
    
    // If this fails it means that at least default name should have been assigned
    // to item.
    //__ASSERT_ALWAYS(length > 0, Panic(EMEUNameEmpty));
    TInt newOrdinal = 0;
    TInt index = length-1;
    TInt charValue = aName[index];

    // Check if name ends with ')'
    if (charValue == ')')
        {
        TBool cutBetweenIndexes = EFalse;
        index--;
        TInt multi = 1;
        while ( index >= 0)
            {
            charValue = aName[index];
            TChar ch(charValue);

            if (!ch.IsDigit())
                {
                // There was non-digits inside the brackets.
                if ( charValue == '(')
                    {
                    // We found the start
                    cutBetweenIndexes = ETrue;
                    index--;
                    break;
                    }
                else
                    {
                    // There was a non-digit inside the '(' and ')'
                    // We just append after the original name.
                    break;
                    }
                } // if
            else
                {
                TInt newNum = ch.GetNumericValue();
                if (multi <= 100000000)
                    {
                    // If there's a pattern (007) then this makes it to be (007)(01)
                    if (newNum == 0 && newOrdinal != 0 && multi > 10)
                        {
                        break;
                        }
                    newOrdinal += newNum * multi;
                    multi*=10;
                    }//0010
                else
                    {
                    newOrdinal = 0;
                    break;
                    }
                }
            index--;
            } // while

        // There was correct pattern of (0000) so now we delete it.
        if (cutBetweenIndexes)
            {
            aName.Delete(index+1, length-index);
            }
        else
            {
            // This case is for example 12345) so we get 12345)(01)
            newOrdinal = 0;
            }
        } // if

    // Add one to the ordinal read from the old name
    newOrdinal++;

    // Check that there's enough space to add the ordinal
    TInt maxLength = aName.MaxLength();
    TInt ordinalSpace(4); // Ordinal space comes from (00) so it's 4
    length = aName.Length();
    if (length + ordinalSpace > maxLength)
        {
        aName.Delete(maxLength-ordinalSpace, ordinalSpace);
        }

    // Append the ordinal at the end of the name
    HBufC* old = aName.AllocLC();

    _LIT(KFormatSpecLessTen, "%S(0%d)");
    _LIT(KFormatSpecOverTen, "%S(%d)");
    if (newOrdinal < 10)
        {
        aName.Format(KFormatSpecLessTen, old, newOrdinal);
        }
    else
        {
        aName.Format(KFormatSpecOverTen, old, newOrdinal);
        }
    CleanupStack::PopAndDestroy(old); // old
    
    FLOG( _L( "CWPIMSAPItem::IncrementNameL: Exit" ) );
    }


// -----------------------------------------------------------------------------
// CWPIMSAPItem::IncrementNameL
// -----------------------------------------------------------------------------
//
void CWPIMSAPItem::IncrementNameL(TDes& aName, TInt aMaxLength)
    {
    
    FLOG( _L( "CWPIMSAPItem::IncrementNameL: Enter" ) );
    
    TInt length = aName.Length();

    	
    // If this fails it means that at least default name should have been assigned
    // to item.
    //__ASSERT_ALWAYS(length > 0, Panic(EMEUNameEmpty));
    TInt newOrdinal = 0;
    TInt index = length-1;
    TInt charValue = aName[index];

    // Check if name ends with ')'
    if (charValue == ')')
        {
        TBool cutBetweenIndexes = EFalse;
        index--;
        TInt multi = 1;
        while ( index >= 0)
            {
            charValue = aName[index];
            TChar ch(charValue);

            if (!ch.IsDigit())
                {
                // There was non-digits inside the brackets.
                if ( charValue == '(')
                    {
                    // We found the start
                    cutBetweenIndexes = ETrue;
                    index--;
                    break;
                    }
                else
                    {
                    // There was a non-digit inside the '(' and ')'
                    // We just append after the original name.
                    break;
                    }
                } // if
            else
                {
                TInt newNum = ch.GetNumericValue();
                if (multi <= 100000000)
                    {
                    // If there's a pattern (007) then this makes it to be (007)(01)
                    if (newNum == 0 && newOrdinal != 0 && multi > 10)
                        {
                        break;
                        }
                    newOrdinal += newNum * multi;
                    multi*=10;
                    }//0010
                else
                    {
                    newOrdinal = 0;
                    break;
                    }
                }
            index--;
            } // while

        // There was correct pattern of (0000) so now we delete it.
        if (cutBetweenIndexes)
            {
            aName.Delete(index+1, length-index);
            }
        else
            {
            // This case is for example 12345) so we get 12345)(01)
            newOrdinal = 0;
            }
        } // if

    // Add one to the ordinal read from the old name
    newOrdinal++;

    // Check that there's enough space to add the ordinal
    TInt maxLength = aMaxLength;
    TInt ordinalSpace(4); // Ordinal space comes from (00) so it's 4
    length = aName.Length();
    if (length + ordinalSpace > maxLength)
        {
        aName.Delete(maxLength-ordinalSpace, ordinalSpace);
        }

    // Append the ordinal at the end of the name
    HBufC* old = aName.AllocLC();

    _LIT(KFormatSpecLessTen, "%S(0%d)");
    _LIT(KFormatSpecOverTen, "%S(%d)");
    if (newOrdinal < 10)
        {
        aName.Format(KFormatSpecLessTen, old, newOrdinal);
        }
    else
        {
        aName.Format(KFormatSpecOverTen, old, newOrdinal);
        }
    CleanupStack::PopAndDestroy(old); // old
    
    FLOG( _L( "CWPIMSAPItem::IncrementNameL: Enter" ) );
    }

//  End of File  

