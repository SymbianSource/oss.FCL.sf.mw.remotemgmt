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
#include "CWPPecAdapter.h"   // own header
#include <e32base.h>
#include <CWPCharacteristic.h>
#include <CWPParameter.h>
#include <wpwvadapterresource.rsg>

#include <commdb.h>
#include "WPAdapterUtil.h"
#include "CWPIMSAPItem.h"
#include "ProvisioningDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPPecAdapter::CWPPecAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWPPecAdapter::CWPPecAdapter()
	{
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWPPecAdapter::ConstructL()
	{
	FLOG( _L( "[Provisioning] CWPPecAdapter::ConstructL:" ) );
	
    TFileName fileName;
    Dll::FileName( fileName );
    iWVTitle = WPAdapterUtil::ReadHBufCL( fileName,
                                          KWVAdapterName,
                                          R_QTN_OP_HEAD_IMSETTINGS );

    iSAPIdForDefault = KNotSet;
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWPPecAdapter* CWPPecAdapter::NewL()
	{
	CWPPecAdapter* self = new(ELeave) CWPPecAdapter; 
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CWPPecAdapter::~CWPPecAdapter()
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
// CWPPecAdapter::DetailsL
// -----------------------------------------------------------------------------
//
TInt CWPPecAdapter::DetailsL( TInt /*aItem*/, MWPPairVisitor& /*aVisitor */ )
	{
	return KErrNotSupported;
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::SummaryCount
// -----------------------------------------------------------------------------
//
TInt CWPPecAdapter::ItemCount() const
	{
	TInt count(0);
	for(TInt i=0;i<iDatas.Count();i++)
	    {
	    if(iDatas[i]->iProvLocation == EIMPSPECAccessGroup)
	        {
	        count++;
	        }
	    }
	return count;
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::SummaryTitle
// -----------------------------------------------------------------------------
//
const TDesC16& CWPPecAdapter::SummaryTitle(TInt /*aIndex*/) const
	{
	return *iWVTitle;
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::SummaryText
// -----------------------------------------------------------------------------
//
const TDesC16& CWPPecAdapter::SummaryText(TInt aIndex) const
	{
	return iDatas[aIndex]->iName;
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::SaveL
// -----------------------------------------------------------------------------
//
void CWPPecAdapter::SaveL( TInt aIndex )
	{
	FLOG( _L( "[Provisioning] CWPPecAdapter::SaveL" ) );
	
	
	for(TInt i=0;i<iDatas.Count();i++)
	    {
	    if(iDatas[i]->iProvLocation != EIMPSPECAccessGroup)
	        {
	        delete iDatas[i];
	        iDatas.Remove(i);
	        i--;
	        }
	    }

	if ( ( iDatas.Count() == 0 ) ||  ( aIndex > iDatas.Count() - 1 ) )
	    {
	    FLOG( _L( "[Provisioning] CWPPecAdapter::SaveL No data" ) );
	    return;
	    }
	
    if ( iDatas[aIndex]->iProvLocation == EIMPSPECAccessGroup )	
	    {
        //data item at given index
        TData* data = iDatas[aIndex];
        // create save item
        CWPIMSAPItem* sapItem = CWPIMSAPItem::NewLC( EIMPSPECAccessGroup );
        
        // set data to save item
        sapItem->SetUserIDL( data->iUserID );
        sapItem->SetPasswordL( data->iPassword );
        sapItem->SetSAPURIL( data->iURL );
        sapItem->SetSettingsNameL( data->iName );

    	// Find a proper GPRS access point
        TUint32 iapId = WPIMUtil::FindGPRSL( data->iLinks );
    	

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
        FLOG( _L( "[Provisioning] CWPPecAdapter::SaveL Done" ) );
	    }
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::CanSetAsDefault
// -----------------------------------------------------------------------------
//
TBool CWPPecAdapter::CanSetAsDefault( TInt aIndex ) const
	{
    // the first one is set as default if exists.
    return ( iSAPIdForDefault && ( aIndex == 0 ) );
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::SetAsDefaultL
// -----------------------------------------------------------------------------
//
void CWPPecAdapter::SetAsDefaultL( TInt aIndex )
	{
    if ( aIndex == 0 && iSAPIdForDefault != KNotSet )
        {
        // set the first item saved as a default     
        CWPIMSAPItem::SetAsDefaultL( iSAPIdForDefault, EIMPSPECAccessGroup );
        }
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPPecAdapter::VisitL( CWPCharacteristic& aCharacteristic )
	{
	switch( aCharacteristic.Type() )
		{
		case KWPApplication:
		    {
			iCurrentData = new(ELeave) TData;
            
            aCharacteristic.AcceptL( *this );

			if( (iAppID == KIMAppID1 ||
			     iAppID == KIMAppID2 ) &&
			     WPIMUtil::IsValid( iCurrentData ) )
				{
				FLOG( _L( "[Provisioning] CWPPecAdapter::VisitL App id match" ) );
				User::LeaveIfError( iDatas.Append( iCurrentData ) );
				}
			else
				{
				delete iCurrentData;
				}
			iCurrentData = NULL;
		    }
			break;
		
		case KWPAppAddr: // fallthrough
        case KWPAppAuth:
            {
			if( iCurrentData )
				{                
				aCharacteristic.AcceptL( *this );
				}
            }
			break;
                
        default:
			break;
		}
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::VisitL
// -----------------------------------------------------------------------------
//
void CWPPecAdapter::VisitL( CWPParameter& aParameter )
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
            FLOG( _L( "[Provisioning] CWPPecAdapter::VisitL Case 0" ) );
            TInt val = aParameter.Name().Match( KServicesName );
            if ( val >= 0 )
                {
                FLOG( _L( "[Provisioning] CWPPecAdapter::VisitL Services" ) );
                val = aParameter.Value().Match( KPresenceName );
                if ( val >= 0 )
                    {
                    FLOG( _L( "[Provisioning] CWPPecAdapter::VisitL Pec" ) );
    	            iCurrentData->iProvLocation = EIMPSPECAccessGroup;
                    }
                else 
                    {
                    FLOG( _L( "[Provisioning] CWPPecAdapter::VisitL NO Pec" ) );
                    // settings not for this adapter
                    iCurrentData->iProvLocation = EIMPSNoAccessGroup;
                    }
                }
            break;
            }			
        default:
            break;
        }
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::VisitLinkL
// -----------------------------------------------------------------------------
//
void CWPPecAdapter::VisitLinkL( CWPCharacteristic& aLink )
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
// CWPPecAdapter::ContextExtension
// -----------------------------------------------------------------------------
//
TInt CWPPecAdapter::ContextExtension( MWPContextExtension*& aExtension )
	{
    aExtension = this;
    return KErrNone;
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::SaveDataL
// -----------------------------------------------------------------------------
//
const TDesC8& CWPPecAdapter::SaveDataL( TInt aIndex ) const
	{
	return iDatas[aIndex]->iSAPId;
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::DeleteL
// -----------------------------------------------------------------------------
//
void CWPPecAdapter::DeleteL( const TDesC8& aSaveData )
	{
	TPckgBuf<TUint32> pckg;
    pckg.Copy( aSaveData );

	CWPIMSAPItem* sapItem = CWPIMSAPItem::NewLC( EIMPSPECAccessGroup );
	sapItem->DeleteL( pckg() );
	CleanupStack::PopAndDestroy( sapItem );
	}

// -----------------------------------------------------------------------------
// CWPPecAdapter::Uid
// -----------------------------------------------------------------------------
//
TUint32 CWPPecAdapter::Uid() const
	{
    return iDtor_ID_Key.iUid;
	}

//  End of File
