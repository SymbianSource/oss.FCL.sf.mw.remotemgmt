/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  dmencryptionutil Encryption/Decryption Interface 
*
*/


#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>
#include <ecom/ecomresolverparams.h>

/**
 * C++ Destructor
 */
// Inline functions
CDMEncryptionUtil::~CDMEncryptionUtil()
	{
        // Destroy any instance variables and then
        // inform the framework that this specific 
        // instance of the interface has been destroyed.
        REComSession::DestroyedImplementation(iDtor_ID_Key);
	}
	
/**
 * Cleans up the ECOM plugin array
 * @param aUid , implementation Uid
 * @return Instance of CDMEncryptionUtil
 */
void CleanupEComArray(TAny* aArray)
    {
        (static_cast<RImplInfoPtrArray*> (aArray))->ResetAndDestroy();
        (static_cast<RImplInfoPtrArray*> (aArray))->Close();
    }

/**
* Create instance of CDMEncryptionUtil, this NewL always loads only the secenv hardware encryption plugin.
* @param Nil
* @return Instance of CDMEncryptionUtil
*/
inline CDMEncryptionUtil* CDMEncryptionUtil::NewL()
    {
        RImplInfoPtrArray infoArray;
        // Note that a special cleanup function is required to reset and destroy
        // all items in the array, and then close it.
        TCleanupItem cleanup(CleanupEComArray, &infoArray);
        CleanupStack::PushL(cleanup);        
        
        //Only ROM plugins are filtered & listed.
       TEComResolverParams resolverParams;
       _LIT8(KOperationName,"extdmencryptionutil");
       
       resolverParams.SetDataType(KOperationName());
       resolverParams.SetWildcardMatch(ETrue);     // Allow wildcard matching
       TRAPD(terr, REComSession::ListImplementationsL(KDMEncryptionUtilInterfaceUid, resolverParams,KRomOnlyResolverUid, infoArray));
       REComSession::FinalClose();
       if(terr)
       {
           User::Leave(terr);
       }
       if(infoArray.Count() > 1 || (0 == infoArray.Count()))
       {
           User::Leave(KErrBadName);
       }
       TUid retUid = infoArray[0]->ImplementationUid();
       CleanupStack::PopAndDestroy(); //cleanup
    
       TAny* ptr = REComSession::CreateImplementationL( retUid, _FOFF(CDMEncryptionUtil, iDtor_ID_Key));
       CDMEncryptionUtil* self = reinterpret_cast<CDMEncryptionUtil*>( ptr );
      
       return self;
    }

/**
* Create instance of CDMEncryptionUtil, this NewL always loads the plugin which matches with the default_data.
* @param aData default_data
* @return Instance of CDMEncryptionUtil
*       Leaves with KErrBadName: if the default_data exists more than 1.                  
*/
inline CDMEncryptionUtil* CDMEncryptionUtil::NewL(const TDesC8& aData)
    {
        RImplInfoPtrArray infoArray;
        // Note that a special cleanup function is required to reset and destroy
        // all items in the array, and then close it.
        TCleanupItem cleanup(CleanupEComArray, &infoArray);
        CleanupStack::PushL(cleanup);        
        
        //Only ROM plugins are filtered & listed.
       TEComResolverParams resolverParams;
       resolverParams.SetDataType(aData);
       resolverParams.SetWildcardMatch(ETrue);     // Allow wildcard matching
       TRAPD(terr, REComSession::ListImplementationsL(KDMEncryptionUtilInterfaceUid, resolverParams,KRomOnlyResolverUid, infoArray));
       REComSession::FinalClose();
       if(terr)
       {
           User::Leave(terr);
       }
       if(infoArray.Count() > 1)
       {
           User::Leave(KErrBadName);
       }
       TUid retUid = infoArray[0]->ImplementationUid();
       CleanupStack::PopAndDestroy(); //cleanup
    
       TAny* ptr = REComSession::CreateImplementationL( retUid, _FOFF(CDMEncryptionUtil, iDtor_ID_Key));
       CDMEncryptionUtil* self = reinterpret_cast<CDMEncryptionUtil*>( ptr );
      
       return self;
    }