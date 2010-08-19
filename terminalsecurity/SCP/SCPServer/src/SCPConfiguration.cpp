/*
* Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of terminalsecurity components
*
*/


// INCLUDE FILES
#include <e32debug.h>
#include <e32base.h>
#include <f32file.h>
#include <SCPParamObject.h>
#include <des.h>

#include "SCPConfiguration.h"
#include "SCPServer.h"
#include <featmgr.h>

#include <dmencryptionutil.h>
// ================= MEMBER FUNCTIONS =======================
	    
        
// C++ default constructor can NOT contain any code, that
// might leave.
//
TSCPConfiguration::TSCPConfiguration( RFs* aRfs )
    : iFsSession( aRfs )
	{
	Dprint( (_L("--> TSCPConfiguration::TSCPConfiguration()") ));
		      	
	Dprint( (_L("<-- TSCPConfiguration::TSCPConfiguration()") ));
	}



// ---------------------------------------------------------
// TSCPConfiguration::Initialize()
// Initialized the configuration path
// 
// Status : Approved
// ---------------------------------------------------------
//
void TSCPConfiguration::Initialize()
    {
    Dprint( (_L("--> TSCPConfiguration::Initialize()") ));
    
    iConfigFileName.Zero();
		
    // Create the server's private path, if it doesn't exist
    iFsSession->CreatePrivatePath( EDriveC );
        
    // Form the configuration file name        
    iFsSession->PrivatePath( iConfigFileName );
    iConfigFileName.Append( KSCPSettingsFileName );      
    }



// ---------------------------------------------------------
// TSCPConfiguration::ReadSetupL()
// Read the configuration settings from the config-file, using
// a paramObject.
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt TSCPConfiguration::ReadSetupL()
    {
    TRAPD( errf, FeatureManager::InitializeLibL() );
		if( errf != KErrNone )
		{
			User::Leave(errf);
		}
		TInt result = KErrNone;
        TAny* KParameters[KTotalParamIDs];
        TInt KParamIDs[KTotalParamIDs];
        TSCPParamType KSCPParamTypes[KTotalParamIDs];
		TInt KNumParams;
		
		
		if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
		{
			TAny* KParameters_WithFlag[] = SCP_PARAMETERS_WithFlag;       
		    const TInt KParamIDs_WithFlag[] = SCP_PARAMIDS_WithFlag;
		    const TSCPParamType KSCPParamTypes_WithFlag[] = SCP_PARAMTYPES_WithFlag;
		    const TInt KNumParams_WithFlag = sizeof( KParameters_WithFlag ) / sizeof( TAny* );
	    	
	    	for (TInt index = 0; index < KNumParams_WithFlag; index++)
	    	{
	    		KParameters[index] = KParameters_WithFlag [index];
	    		KParamIDs[index] = KParamIDs_WithFlag[index];
	    		KSCPParamTypes[index] = KSCPParamTypes_WithFlag[index];
	    	}
	    	KNumParams = KNumParams_WithFlag;
		}
		
		else
		{
			TAny* KParameters_WithOutFlag[] = SCP_PARAMETERS_WithOutFlag;       
		    const TInt KParamIDs_WithOutFlag[] = SCP_PARAMIDS_WithOutFlag;
		    const TSCPParamType KSCPParamTypes_WithOutFlag[] = SCP_PARAMTYPES_WithOutFlag;
		    const TInt KNumParams_WithOutFlag = sizeof( KParameters_WithOutFlag ) / sizeof( TAny* );
	    	
	    	for (TInt index = 0; index < KNumParams_WithOutFlag; index++)
	    	{
	    		KParameters[index] = KParameters_WithOutFlag [index];
	    		KParamIDs[index] = KParamIDs_WithOutFlag[index];
	    		KSCPParamTypes[index] = KSCPParamTypes_WithOutFlag[index];
	    	}
	    	KNumParams = KNumParams_WithOutFlag;
		}
		
    
    CSCPParamObject* params = CSCPParamObject::NewL();
    CleanupStack::PushL( params );

    TFileName configFileName;
    configFileName.Copy( KSCPSettingsFileName );
    
    TRAPD( err, params->ReadFromFileL( iConfigFileName, iFsSession ) );
    
    if ( err != KErrNone )
        {
        FeatureManager::UnInitializeLib();
        CleanupStack::PopAndDestroy( params );
        User::Leave( err );
        }
    
    TInt ret = KErrNone;    
    for ( TInt i = 0; i < KNumParams; i++ )
        { 
        // Set the value according to the type
        switch ( KSCPParamTypes[i] )
           {   
           case ( EParTypeInt ):
               {                                
               ret = params->Get( KParamIDs[i], *(reinterpret_cast<TInt*>( KParameters[i] )) );
               break;
               }
                               
		   case ( EParTypeDesc ):
               { 
               ret = params->Get( KParamIDs[i], *(reinterpret_cast<TDes*>( KParameters[i] )) );
               break;
               } 
               
           default:               
                // No implementation needed           
                break;
           } // switch                 
        
        if ( ret != KErrNone )
            {
            result = ret; // Return the last error, attempting to read all parameters
            Dprint( (_L("TSCPConfiguration::ReadSetupL(): FAILED to get value for %d"), KParamIDs[i] ));
            }
        }
    
    CleanupStack::PopAndDestroy( params ); 
    
    // Decrypt the ISA security code
    TSCPSecCode cryptBuf;
    cryptBuf.Copy( iSecCode );
    Dprint( (_L("--> TSCPConfiguration::ReadSetupL NativeTransform iCryptoCode= %S "), &iCryptoCode ));
    if(EFalse == NativeTransform(EFalse, iCryptoCode, iSecCode))
    {
        Dprint( (_L("--> TSCPConfiguration::ReadSetupL NativeTransform returned false") ));
        // If ECOM decrypt fails after modifying iSecCode. 
        iSecCode.Copy(cryptBuf);
        TransformStringL( EFalse, cryptBuf, iSecCode );        
    }
    else
    {
        Dprint( (_L("--> TSCPConfiguration::ReadSetupL NativeTransform returned true") ));
        Dprint( (_L("--> TSCPConfiguration::ReadSetupL NativeTransform iCryptoCode= %S "), &iCryptoCode ));
        //Dummy code to be written of KSCPCodeMaxLen size, if plugin exist.
       // Dprint((_L("iSecCode.Copy(iCryptoCode.Ptr(), KSCPCodeMaxLen);")));
       // iSecCode.Copy(iCryptoCode.Ptr(), KSCPCodeMaxLen);        
    }
    iSecCode.SetLength( KSCPCodeMaxLen ); // Remove the suffix
       
    FeatureManager::UnInitializeLib();
    return result;       
    }



// ---------------------------------------------------------
// TSCPConfiguration::WriteSetupL()
// Fill the configuration values into a paramObject and write
// it to disk.
// 
// Status : Approved
// ---------------------------------------------------------
//
TInt TSCPConfiguration::WriteSetupL()
    {        
    TRAPD( errf, FeatureManager::InitializeLibL() );
		if( errf != KErrNone )
		{
			User::Leave(errf);
		}
		TInt result = KErrNone;
		
		
		
        TAny* KParameters[KTotalParamIDs];
        TInt KParamIDs[KTotalParamIDs];
        TSCPParamType KSCPParamTypes[KTotalParamIDs];
		TInt KNumParams;
		
		
		if(FeatureManager::FeatureSupported(KFeatureIdSapDeviceLockEnhancements))
		{
			TAny* KParameters_WithFlag[] = SCP_PARAMETERS_WithFlag;       
		    const TInt KParamIDs_WithFlag[] = SCP_PARAMIDS_WithFlag;
		    const TSCPParamType KSCPParamTypes_WithFlag[] = SCP_PARAMTYPES_WithFlag;
		    const TInt KNumParams_WithFlag = sizeof( KParameters_WithFlag ) / sizeof( TAny* );
	    	
	    	for (TInt index = 0; index < KNumParams_WithFlag; index++)
	    	{
	    		KParameters[index] = KParameters_WithFlag [index];
	    		KParamIDs[index] = KParamIDs_WithFlag[index];
	    		KSCPParamTypes[index] = KSCPParamTypes_WithFlag[index];
	    	}
	    	KNumParams = KNumParams_WithFlag;
		}
		
		else
		{
			TAny* KParameters_WithOutFlag[] = SCP_PARAMETERS_WithOutFlag;       
		    const TInt KParamIDs_WithOutFlag[] = SCP_PARAMIDS_WithOutFlag;
		    const TSCPParamType KSCPParamTypes_WithOutFlag[] = SCP_PARAMTYPES_WithOutFlag;
		    const TInt KNumParams_WithOutFlag = sizeof( KParameters_WithOutFlag ) / sizeof( TAny* );
	    	
	    	for (TInt index = 0; index < KNumParams_WithOutFlag; index++)
	    	{
	    		KParameters[index] = KParameters_WithOutFlag [index];
	    		KParamIDs[index] = KParamIDs_WithOutFlag[index];
	    		KSCPParamTypes[index] = KSCPParamTypes_WithOutFlag[index];
	    	}
	    	KNumParams = KNumParams_WithOutFlag;
		}
    // Write the parameters by the configured IDs to the paramObject, and write this to disk.       
    CSCPParamObject* params = CSCPParamObject::NewL();
    CleanupStack::PushL( params );  
    
    // Encrypt the ISA security code
    TSCPSecCode cryptBuf;
    cryptBuf.Copy( iSecCode );
    cryptBuf.Append( KSCPCryptSuffix ); // 5 chars for the code + suffix
    if(EFalse == NativeTransform(ETrue, cryptBuf, iCryptoCode)) 
    {
        cryptBuf.FillZ();
        cryptBuf.Zero();
        cryptBuf.Copy( iSecCode );
        cryptBuf.Append( KSCPCryptSuffix ); // 5 chars for the code + suffix
        Dprint( (_L("--> TSCPConfiguration::WriteSetupL NativeTransform returned false") ));
        TransformStringL( ETrue, cryptBuf, iSecCode );
        
    }
    else
    {
        iSecCode.FillZ();
        iSecCode.Zero();
        
        Dprint( (_L("--> TSCPConfiguration::WriteSetupL NativeTransform returned true") ));
        Dprint( (_L("--> TSCPConfiguration::WriteSetupL NativeTransform iCryptoCode= %S "), &iCryptoCode ));
    }
    TInt ret = KErrNone;    
    for ( TInt i = 0; i < KNumParams; i++ )
        { 
        // Set the value according to the type
        switch ( KSCPParamTypes[i] )
           {   
           case ( EParTypeInt ):
               {                                
               ret = params->Set( KParamIDs[i], *(reinterpret_cast<TInt*>( KParameters[i] )) );
               break;
               }
                               
		   case ( EParTypeDesc ):
               { 
               ret = params->Set( KParamIDs[i], *(reinterpret_cast<TDes*>( KParameters[i] )) );
               break;
               } 
      
           default:               
                // No implementation needed           
                break;
           } // switch                 
        
        if ( ret != KErrNone )
            {
            result = ret; // Return the last error, attempting to read all parameters
            Dprint( (_L("TSCPConfiguration::WriteSetupL(): FAILED to set value for %d"), KParamIDs[i] ));
            }
        }  
        
    // Reset the ISA security code
    iSecCode.Copy( cryptBuf );
    iSecCode.SetLength( KSCPCodeMaxLen );
           
    params->WriteToFileL( iConfigFileName, iFsSession );           
     
    CleanupStack::PopAndDestroy( params );  
    
    
    FeatureManager::UnInitializeLib();
    return result;    
    }
    
    

// ---------------------------------------------------------
// TSCPConfiguration::GetKey()
// Returns the server's UID reformatted as the key.
// 
// Status : Approved
// ---------------------------------------------------------
//
void TSCPConfiguration::GetKeyL( TDes8& aKey )
    {
    TInt64 key = KSCPServerUid.iUid;
    key = ( key << 32 ) + KSCPServerUid.iUid;
    
    TUint8* keyPtr = const_cast<TUint8*>( aKey.Ptr() );
    *( reinterpret_cast<TInt64*>( keyPtr ) ) = key;
    
    aKey.SetLength( KSCPDesBlockSize );
    
    TBuf8<KSCPDesBlockSize> modBuf;
    modBuf.Copy( KSCPEncryptionKeyMod );
    
    TInt i = 0;
    for ( i = 0; i < KSCPDesBlockSize; i++ )
        {
        aKey[i] = aKey[i] ^ modBuf[i];
        }    
    }
    
    
    
// ---------------------------------------------------------
// TSCPConfiguration::TransformStringL()
// Transforms the given buffer using DES encryption/decryption
// and stores the result to aOutput
// 
// Status : Approved
// ---------------------------------------------------------
//
void TSCPConfiguration::TransformStringL( TBool aEncrypt, TDes& aInput, TDes& aOutput )
    {
#ifdef UNICODE
    // Reinterpret the input and output as 8-bit data
    TUint16* inputPtr = const_cast<TUint16*>( aInput.Ptr() ); 
    TPtrC8 inputData( reinterpret_cast<TUint8*>(inputPtr), aInput.Length()*2 );
    
    TUint16* outputPtr = const_cast<TUint16*>( aOutput.Ptr() ); 
    TPtr8 outputData( reinterpret_cast<TUint8*>(outputPtr), 0, aOutput.MaxLength()*2 );
        
#else // !UNICODE
    TPtrC8 inputData = aInput;
    TPtr8 outputData = aOutput;
#endif  // UNICODE             
    
    TBuf8<KSCPDesBlockSize> desKey;
    GetKeyL( desKey );
    
    CBlockTransformation* transformer = NULL;
    if ( aEncrypt )
        {
        transformer = CDESEncryptor::NewL( desKey, EFalse );
        }
    else
        {
        transformer = CDESDecryptor::NewL( desKey, EFalse );
        }
    CleanupStack::PushL( transformer );
    
    aOutput.FillZ();
    aOutput.Zero();
    
    // 8 bytes of data per block
    TBuf8<KSCPDesBlockSize> desBlock;          
        
    TInt index = 0;
    TInt copySize;
    while ( index < inputData.Length() - 1 )
        {
        desBlock.FillZ(); // If the data is not an exact multiple of 8, pad with 0s
        desBlock.Zero();
        
        // Copy either the remaining data or a DES block
        if ( ( inputData.Length() - index ) < KSCPDesBlockSize )
            {
            copySize = inputData.Length() - index;
            }
        else
            {
            copySize = KSCPDesBlockSize;
            }
            
        desBlock.Copy( inputData.Mid( index, copySize ) );
        index += copySize;
        desBlock.SetLength( KSCPDesBlockSize );
        
        transformer->Transform( desBlock );
                        
        // Append the transformed block to the output
        outputData.Append( desBlock );                
        }
            
    CleanupStack::PopAndDestroy( transformer );
        
#ifdef UNICODE
    aOutput.SetLength( outputData.Length() / 2 );
#endif // UNICODE       
    }
// ---------------------------------------------------------
// TSCPConfiguration::NativeTransform (TBool aEncrypt, TDes& aInput, TDes& aOutput)
// Transforms the given aInput buffer using the plugin Encryption/Decryption,
// and stores the result to aOutput
// 
// Status : New
// ---------------------------------------------------------
//
TBool TSCPConfiguration::NativeTransform (TBool aEncrypt, TDes& aInput, TDes& aOutput)
{
    Dprint( (_L("--> TSCPConfiguration::NativeTransform()") ));
    TInt err(KErrNone);
    TBool result(ETrue);


        CDMEncryptionUtil* eUtil;
        Dprint( (_L("TSCPConfiguration::NativeTransform(), calling TRAP(err, eUtil = CDMEncryptionUtil::NewL());  ") ));

        TRAP(err, eUtil = CDMEncryptionUtil::NewL());
        Dprint( (_L("TSCPConfiguration::NativeTransform(), after TRAP(err, eUtil = CDMEncryptionUtil::NewL());  err= %d"), err ));
        if(err)
        {
            result = EFalse;
            return result;
        }
        CleanupStack::PushL(eUtil);
        if(aEncrypt)
        {
            Dprint( (_L(" TSCPConfiguration::NativeTransform()Encrypting...") ));            
            TRAP(err, eUtil->EncryptL( aInput, aOutput));
            Dprint( (_L("TSCPConfiguration::NativeTransform(), after Encrypting err= %d"), err ));
        }
        else
        {
            Dprint( (_L(" TSCPConfiguration::NativeTransform()Decrypting...") ));            
            TRAP(err, eUtil->DecryptL( aInput, aOutput));            
            Dprint( (_L("TSCPConfiguration::NativeTransform(), after Decrypting err= %d"), err ));
        }
        CleanupStack::PopAndDestroy(); //eUtil
        if(err)
        {
            Dprint( (_L(" TSCPConfiguration::NativeTransform() CDMEncryptionUtil::ListImplementationsL failed") ));
            result = EFalse;  
        }
        else
        {
            Dprint( (_L(" TSCPConfiguration::NativeTransform() CDMEncryptionUtil::ListImplementationsL success") ));
            result = ETrue;            
        }

    Dprint( (_L("TSCPConfiguration::NativeTransform(): result = %d"), result ));
    Dprint( (_L("<-- TSCPConfiguration::NativeTransform()") ));
    return result;
}

//  End of File  

