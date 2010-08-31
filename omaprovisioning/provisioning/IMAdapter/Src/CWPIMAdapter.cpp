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
* Description:  Handles wireless village settings in provisioning.
*
*/



// INCLUDE FILES
#include "CWPIMAdapter.h"   // own header
#include <e32base.h>
#include <CWPAdapter.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpwvadapterresource.rsg>

#include <commdb.h>
#include "WPAdapterUtil.h"
#include "CWPIMSAPItem.h"
#include "ProvisioningDebug.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPIMAdapter::CWPIMAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPIMAdapter::CWPIMAdapter() : CWPAdapter()
	{
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPIMAdapter::ConstructL()
	{
	FLOG( _L( "[Provisioning] CWPIMAdapter::ConstructL:" ) );
	
    TFileName fileName;
    Dll::FileName( fileName );
    iWVTitle = WPAdapterUtil::ReadHBufCL( fileName, KWVAdapterName, R_QTN_OP_HEAD_IMSETTINGS );

    iSAPIdForDefault = KNotSet;
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPIMAdapter* CWPIMAdapter::NewL()
	{
	CWPIMAdapter* self = new(ELeave) CWPIMAdapter; 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPIMAdapter::~CWPIMAdapter()
	{
	delete iCurrentData;    
    delete iWVTitle;
	for ( TInt i = 0; i < iDatas.Count(); i++ )
		{
		delete iDatas[i];
		}
	iDatas.Close();
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CWPIMAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor */ )
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::SummaryCount
// -----------------------------------------------------------------------------
//
TInt CWPIMAdapter::ItemCount() const
	{
	TInt count(0);
	for(TInt i=0;i<iDatas.Count();i++)
	    {
	    if(iDatas[i]->iProvLocation == EIMPSIMAccessGroup)
	        {
	        count++;
	        }
	    }
	return count;
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CWPIMAdapter::SummaryTitle(TInt /*aIndex*/) const
	{
	return *iWVTitle;
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CWPIMAdapter::SummaryText(TInt aIndex) const
	{
	return iDatas[aIndex]->iName;
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CWPIMAdapter::SaveL( TInt aIndex )
	{
	FLOG( _L( "[Provisioning] CWPIMAdapter::SaveL" ) );
	
	for(TInt i=0;i<iDatas.Count();i++)
	    {
	    if(iDatas[i]->iProvLocation != EIMPSIMAccessGroup)
	        {
	        delete iDatas[i];
	        iDatas.Remove(i);
	        i--;
	        }
	    }
	if ( ( iDatas.Count() == 0 ) ||  ( aIndex > iDatas.Count() - 1 ) )
	    {
	    FLOG( _L( "[Provisioning] CWPIMAdapter::SaveL No data" ) );
	    return;
	    }
	
    if ( iDatas[aIndex]->iProvLocation == EIMPSIMAccessGroup )	
	    {
        //data item at given index
        TData* data = iDatas[aIndex];
        // create save item
        CWPIMSAPItem* sapItem = CWPIMSAPItem::NewLC( EIMPSIMAccessGroup );
        
        // set data to save item
        sapItem->SetUserIDL( data->iUserID );
        sapItem->SetPasswordL( data->iPassword );
        sapItem->SetSAPURIL( data->iURL );
        sapItem->SetSettingsNameL( data->iName );

    	// Find a proper GPRS access point
        TUint32 iapId(KMaxTUint32);
        if(data->iLinks.Count())
    	    {
            iapId = WPIMUtil::FindGPRSL( data->iLinks );
            }
    	// set the IAP id as part of save item.
    	sapItem->SetIAPIdL( iapId );
           
        // store SAP item
        TUint32 SAPuid = sapItem->StoreL();
    	data->iSAPId = SAPuid;

        // if there is no id for default Service Access Point, set it. 
        if (iSAPIdForDefault == KNotSet)
            {
            iSAPIdForDefault = SAPuid;
            }
        
        CleanupStack::PopAndDestroy( sapItem ); 
        FLOG( _L( "[Provisioning] CWPIMAdapter::SaveL Done" ) );
	    }
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPIMAdapter::CanSetAsDefault(TInt aIndex) const
	{
    // the first one is set as default if exists.
    return ( iSAPIdForDefault && ( aIndex == 0 ) );
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPIMAdapter::SetAsDefaultL( TInt aIndex )
	{
    if ( aIndex == 0 && iSAPIdForDefault != KNotSet )
        {
        // set the first item saved as a default     
        CWPIMSAPItem::SetAsDefaultL( iSAPIdForDefault, EIMPSIMAccessGroup );
        }
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPIMAdapter::VisitL( CWPCharacteristic& aCharacteristic )
	{
	switch( aCharacteristic.Type() )
		{
		case KWPApplication:
			iCurrentData = new(ELeave) TData;
            
            aCharacteristic.AcceptL( *this );

			if( (iAppID == KIMAppID1 || 
			     iAppID == KIMAppID2 ) && WPIMUtil::IsValid(iCurrentData) )
				{
				FLOG( _L( "[Provisioning] CWPIMAdapter::VisitL App ID match" ) );
				iCurrentData->iProvLocation = EIMPSIMAccessGroup;
				User::LeaveIfError( iDatas.Append( iCurrentData ) );
				}
			else
				{
				delete iCurrentData;
				}

			iCurrentData = NULL;
			break;
		
		case KWPAppAddr: // fallthrough
        case KWPAppAuth:
			if( iCurrentData )
				{                
				aCharacteristic.AcceptL( *this );
				}
			break;
                
        default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPIMAdapter::VisitL( CWPParameter& aParameter)
	{
    	
	switch( aParameter.ID() )
		{
        case EWPParameterAppID:
			iAppID.Set( aParameter.Value() );
			break;

		case EWPParameterName:
            if (iCurrentData->iName == KNullDesC)
                {
                iCurrentData->iName.Set( aParameter.Value() );
                }
 			break;

		case EWPParameterAddr:
            if (iCurrentData->iURL == KNullDesC)
                {            
			    iCurrentData->iURL.Set( aParameter.Value() );
                }
			break;

		case EWPParameterAAuthName:
            if (iCurrentData->iUserID == KNullDesC)
                {            
			    iCurrentData->iUserID.Set( aParameter.Value() );
                }
			break;

		case EWPParameterAAuthSecret:
            if (iCurrentData->iPassword == KNullDesC)
                {            
			    iCurrentData->iPassword.Set( aParameter.Value() );
                }
			break;
        // Here case 0 are handled the IM and Presence parameters that are extensions  
        // to OMA Client Provisioning parameter set.
        case 0:
            {
            FLOG( _L( "[Provisioning] CWPIMAdapter::VisitL Case 0" ) );
            TInt val = aParameter.Name().Match( KServicesName );
            if ( val >= 0 )
                {
                FLOG( _L( "[Provisioning] CWPIMAdapter::VisitL  Services" ) );
                val = aParameter.Value().Match( KIMName );
                if ( val >= 0 )
                    {
                    FLOG( _L( "[Provisioning] CWPIMAdapter::VisitL  Services IM settings" ) );
    	            iCurrentData->iProvLocation = EIMPSIMAccessGroup;
                    }
                else 
                    {
                    FLOG( _L( "[Provisioning] CWPIMAdapter::VisitL  Services NO IM settings" ) );
                    // settings not for us...
                    iCurrentData->iProvLocation = EIMPSNoAccessGroup;
                    }
                }
            }
            break;			
        default:
            break;
        }
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::VisitLinkL
// -----------------------------------------------------------------------------
//
void CWPIMAdapter::VisitLinkL( CWPCharacteristic& aLink )
	{
  	switch( aLink.Type() )
		{
		case KWPNapDef: // fallthrough
		case KWPPxLogical:
			{
			User::LeaveIfError( iCurrentData->iLinks.Append( &aLink ) );
   			break;
			}
		default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::ContextExtension
// -----------------------------------------------------------------------------
//
TInt CWPIMAdapter::ContextExtension( MWPContextExtension*& aExtension )
	{
    aExtension = this;
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::SaveDataL
// -----------------------------------------------------------------------------
//
const TDesC8& CWPIMAdapter::SaveDataL( TInt aIndex ) const
	{
	return iDatas[aIndex]->iSAPId;
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::DeleteL
// -----------------------------------------------------------------------------
//
void CWPIMAdapter::DeleteL( const TDesC8& aSaveData )
	{
	TPckgBuf<TUint32> pckg;
    pckg.Copy( aSaveData );

	CWPIMSAPItem* sapItem = CWPIMSAPItem::NewLC( EIMPSIMAccessGroup );
	sapItem->DeleteL( pckg() );
	CleanupStack::PopAndDestroy( sapItem );   
	}

// -----------------------------------------------------------------------------
// CWPIMAdapter::Uid
// -----------------------------------------------------------------------------
//
TUint32 CWPIMAdapter::Uid() const
	{
    return iDtor_ID_Key.iUid;
	}

//  End of File
