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
* Description:  WPIMUtil is a utility class for reading resource strings.
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <barsc.h>
#include <bautils.h>
#include "WPIMUtil.h"
#include <data_caging_path_literals.hrh>

#include <CWPCharacteristic.h>
#include <cmconnectionmethoddef.h>
#include <cmmanagerext.h>
#include <cmpluginpacketdatadef.h>
#include <commdb.h>
#include "WPAdapterUtil.h"
#include <wpwvadapterresource.rsg>
#include <uri16.h>                // TUriParser8

// CONSTANTS
const TInt KWVLastIllegalCharIndex = 32;
const TInt KNameMaxLength = 30;
const TInt KUserIdMaxLength = 50;
const TInt KPasswordMaxLenth = 50;
const TInt KURIMaxLength = 100;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWPPecAdapter::FindGPRSL
// -----------------------------------------------------------------------------
//
TUint32 WPIMUtil::FindGPRSL( 
                                      RPointerArray<CWPCharacteristic>& aLinks )
	{
	TUint32 iapID=NULL;
	RCmManagerExt  cmmanagerExt;
	cmmanagerExt.OpenL();
	CleanupClosePushL(cmmanagerExt);
	TUint32 bearer = 0;

	
	for( TInt i( 0 ); i < aLinks.Count(); i++ )
	   {
	   CWPCharacteristic* curr = aLinks[i];

	   TPckgBuf<TUint32> uidPckg;
	   for( TInt dataNum( 0 ); curr->Data( dataNum ).Length() == uidPckg.MaxLength(); dataNum++ )
	      {
	      uidPckg.Copy( curr->Data( dataNum ) );
	      RCmConnectionMethodExt cm;
	      cm = cmmanagerExt.ConnectionMethodL( uidPckg() );
	      CleanupClosePushL( cm );
	      bearer = cm.GetIntAttributeL( CMManager::ECmBearerType );
	      if(bearer == KUidPacketDataBearerType)
	        {
	        iapID = cm.GetIntAttributeL(CMManager::ECmIapId);
	        CleanupStack::PopAndDestroy(2);
	        return iapID;
	        }
	      CleanupStack::PopAndDestroy();  // cm         
	      }
	    }
	CleanupStack::PopAndDestroy(); //cmmanagerExt
	
	// This leave is absolutely needed as it pops & destroys 
	// data in CleanupStack
  //  User::Leave( KErrNotFound );
    return iapID;
	}

// -----------------------------------------------------------------------------
// WPIMUtil::HasIllegalChars
// -----------------------------------------------------------------------------
//
TBool WPIMUtil::HasIllegalChars( const TDesC& aDes )
    {
    // check is there any special chars between 00h to 1Fh 
    for (TInt i=0; i< KWVLastIllegalCharIndex ; i++)
        {
        if (aDes.Locate(i) !=KErrNotFound )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// WPIMUtil::IsValid
// -----------------------------------------------------------------------------
//
TBool WPIMUtil::IsValid( TData* aCurrentData )
    {
    TBool ret(ETrue);

    // validate name:
    TPtrC dataItem(aCurrentData->iName);

    // cut the name to its max length. 
    TPtrC newPtr = dataItem.Left(KNameMaxLength);
    aCurrentData->iName.Set(newPtr);
    
    dataItem.Set( aCurrentData->iName );
    
    if ( dataItem.Length() == 0 || WPIMUtil::HasIllegalChars( dataItem ) )
        {
        // set the default name
        TFileName fileName;
        Dll::FileName( fileName );
        HBufC* defaultName = NULL;
        TRAPD( retVal, defaultName = WPAdapterUtil::ReadHBufCL( fileName,
                                                 KWVAdapterName,
                                                 R_QTN_SM_IM_SERVER_DNAME ) );
        CleanupStack::PushL( defaultName );                                     
        if ( ( retVal == KErrNone ) && defaultName )
            {
            aCurrentData->iName.Set(*defaultName);
            }
        CleanupStack::PopAndDestroy( defaultName );
        }

    // cut the name to max length
    // validate URL
    dataItem.Set( aCurrentData->iURL );        
    if ( dataItem.Length() == 0                 ||
         dataItem.Length() > KURIMaxLength      || 
         WPIMUtil::HasIllegalChars( dataItem ) )
        {
        ret = EFalse;
        }
    else 
        {
        TUriParser16 uriParser;
        TInt err = uriParser.Parse( dataItem );

        if (err != KErrNone)
            {
            ret = EFalse;
            }
        }

    if (ret)
        {
        // validate userID    
        dataItem.Set( aCurrentData->iUserID );
        if ( dataItem.Length() > KUserIdMaxLength ||
             WPIMUtil::HasIllegalChars( dataItem ) )
            {
            ret = EFalse;
            }
        }
    if (ret)
        {
        // validate password
 	    dataItem.Set(aCurrentData->iPassword);
        if ( dataItem.Length() > KPasswordMaxLenth )
            {
            ret = EFalse;
            }
        }
    return ret;
    }
    
// -----------------------------------------------------------------------------
// TData::TData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TData::TData()
	{
	iName.Set( KNullDesC );
	iURL.Set( KNullDesC );
	iUserID.Set( KNullDesC );
	iPassword.Set( KNullDesC );
	iSAPId = 0;
	}
	
// -----------------------------------------------------------------------------	
// Destructor
// -----------------------------------------------------------------------------
TData::~TData()
	{
	iLinks.Close();
	}
	
//  End of File  
