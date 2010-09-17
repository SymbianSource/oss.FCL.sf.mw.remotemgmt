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

#include <e32debug.h>
#include <f32file.h>

#include "SCPParamObject.h"
#include "SCPClient.h"

#include "SCPDebug.h"
/*#ifdef _DEBUG
#define __SCP_DEBUG
#endif // _DEBUG

// Define this so the precompiler in CW 3.1 won't complain about token pasting,
// the warnings are not valid
#pragma warn_illtokenpasting off

#ifdef __SCP_DEBUG
#define Dprint(a) RDebug::Print##a
#else
#define Dprint(a)
#endif // _DEBUG*/

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSCPParamObject::CSCPParamObject()
// C++ constructor
//
// Status: Approved
// ----------------------------------------------------------
CSCPParamObject::CSCPParamObject()
	{
	// No implementation required
	}
	

// ----------------------------------------------------------
// CSCPParamObject::NewL()
// Static constructor
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C CSCPParamObject* CSCPParamObject::NewL()
	{	
	CSCPParamObject* self = CSCPParamObject::NewLC();
	CleanupStack::Pop( self );
		
	return self;	
	}
	

// ----------------------------------------------------------
// CSCPParamObject::NewLC()
// Static constructor, leaves object pointer to the cleanup stack.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C CSCPParamObject* CSCPParamObject::NewLC()
	{
	Dprint( (_L("--> CSCPParamObject::NewLC()")) );

	CSCPParamObject* self = new (ELeave) CSCPParamObject();

    CleanupStack::PushL( self );
    self->ConstructL();	
    
    Dprint( (_L("<-- CSCPParamObject::NewLC()") ));
    return self;	
    }

// ----------------------------------------------------------
// CSCPParamObject::ConstructL()
// Symbian 2nd phase constructor
//
// Status: Approved
// ----------------------------------------------------------
void CSCPParamObject::ConstructL()
    {            
	iParamIDs.Reset();
	iParamValues.Reset();
    }


// ----------------------------------------------------------
// CSCPParamObject::GetParamLength()
// Return the length of the given parameter.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::GetParamLength( TInt aParamID )
    {    
    TInt ret = iParamIDs.Find( aParamID );
    if ( ( ret != KErrNotFound ) && ( ret >= 0 ) )
        {
        ret = ( iParamValues[ret]->Des() ).Length();
        } 
        
    return ret;   
    }


// ----------------------------------------------------------
// CSCPParamObject::Get()
// Retrieve the given value from the arrays
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::Get( TInt aParamID, TDes& aValue )
    {
    TInt ret = KErrNone;
    
    // Find the object
    TInt index = iParamIDs.Find( aParamID );
    if ( ( index != KErrNotFound ) && ( index >= 0 ) )
        {
        // Retrieve the value
        if ( aValue.MaxLength() < ( iParamValues[index]->Des() ).Length() )
            {
            ret = KErrOverflow;
            }
        else
            {
            aValue.Copy( iParamValues[index]->Des() );
            }                    
        }
    else
        {
        ret = index;
        }
        
    return ret;
    }
    
// ----------------------------------------------------------
// CSCPParamObject::Get()
// Retrieve the given value from the arrays, trying to 
// convert it to an integer.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::Get( TInt aParamID, TInt& aValue )
    {
    TBuf<KSCPMaxIntLength> convBuf;
    convBuf.Zero();
    
    // An integer should fit into 16chars, if not, it's an error
    TInt ret = Get( aParamID, convBuf );
    
    if ( ret == KErrNone )
        {
        TLex lex( convBuf );
        ret = lex.Val( aValue );
        }        
    
    return ret;
    }    
    
    
    
// ----------------------------------------------------------
// CSCPParamObject::Set()
// Insert or replace the parameter value of aParamID with 
// aValue.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::Set( TInt aParamID, TDes& aValue )
    {
    TBool isNewParam = ETrue;
    TInt ret = KErrNone;
    
    // Create a buffer for the value
    HBufC* newValue = NULL;
    TRAPD( err, newValue = HBufC::NewL( aValue.Length() ) );        
    
    if ( err != KErrNone )
        {
        return err;
        }    
    
    TPtr newValPtr = newValue->Des(); 
    newValPtr.Copy( aValue );

    if ( err != KErrNone )
        {
        Dprint( (_L("CSCPParamObject::Set(): ERROR: \
            HBufC Allocation failed") ));
        ret = err;
        }
                
    // Check if we've got this ID already         
    for ( TInt i = 0; i < iParamIDs.Count(); i++ )
        {
        if ( iParamIDs[i] == aParamID )
            {            
            // Remove the old entry
            HBufC* tmpValue = iParamValues[ i ];            
            iParamValues.Remove( i );
            delete tmpValue;
                
            iParamValues.InsertL( newValue, i );
            isNewParam = EFalse;
            }
        }
        
    if ( isNewParam )
        {
        // Create a new value
        TInt err = KErrNone;
        TRAP(err, iParamIDs.AppendL( aParamID ));
        if(err != KErrNone)
            {
            delete newValue;
            ret = err;
            }
        else 
            {
            TRAP(err, iParamValues.AppendL( newValue ));
            if(err != KErrNone)
                {
                delete newValue;
                //Rollback append.
                iParamIDs.Remove(iParamIDs.Count()-1);
                ret = err;
                }
            }
        }

    return ret;
    }    
    
// ----------------------------------------------------------
// CSCPParamObject::Set()
// Convert the int into a descriptor and propagate to Set.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::Set( TInt aParamID, TInt aValue )
    {
    // Convert the value
    TBuf<KSCPMaxIntLength> convBuf;
    convBuf.Zero();    
    convBuf.AppendNum( aValue );
    
    return Set( aParamID, convBuf );
    }  
    
    
// ----------------------------------------------------------
// CSCPParamObject::Unset()
// Remove the given ID along with its value from the arrays.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::Unset( TInt aParamID )
    {
    TInt ret = KErrNotFound;
    
    // Check if we've got this ID
    for ( TInt i = 0; i < iParamIDs.Count(); i++ )
        {
        if ( iParamIDs[i] == aParamID )
            {            
            // Remove the entry
            HBufC* tmpValue = iParamValues[ i ];            
            iParamValues.Remove( i );
            delete tmpValue;                        
                       
            iParamIDs.Remove( i );
            ret = KErrNone;
            }
        }
        
    return ret;
    }
        
      
// ----------------------------------------------------------
// CSCPParamObject::Reset()
// Reset the ID and value arrays.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::Reset()
    {
    iParamIDs.Reset();
	iParamValues.ResetAndDestroy();
	iFailedPolicyIDs.Reset();	
	return KErrNone;
    }
   
    
// ----------------------------------------------------------
// CSCPParamObject::Parse()
// Parse a previously compiled buffer, and add the parameters
// found to the arrays.
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::Parse( TDes8& aBuffer )
    {        
    TInt ret = KErrNone;
    
    TInt intSize = sizeof( TInt );
    
#ifdef UNICODE
    TInt charSize = 2;
#else
    TInt charSize = 1;
#endif // __UNICODE    
        
    if ( aBuffer.Length() == 0 )
        {
        // OK, nothing to parse
        return KErrNone;
        }
    
    if ( aBuffer.Length() < intSize )
        {
        return KErrArgument;
        }    
        
    // The number of parameters is the first TInt in the buffer
    TInt numParams = *(reinterpret_cast<TInt*>( &aBuffer[0] ) );
        
    TInt curOffset = ( 1 + numParams * 2 ) * intSize; 
    
    if ( aBuffer.Length() < curOffset )
        {
        // The buffer must not be shorter than the header
        return KErrArgument;
        }    
        
    for ( TInt i = 0; i < numParams; i++ )
        { 
        // 2 ints per param, offset 1 = ID, offset 2 = length       
        TInt curID = *(reinterpret_cast<TInt*>( &aBuffer[ (i * 2 + 1) * intSize ]) );
        TInt curLen = *(reinterpret_cast<TInt*>( &aBuffer[ (i * 2 + 2) * intSize ]) );
        
#ifdef UNICODE
        TPtr curDes( reinterpret_cast<TUint16*>(&aBuffer[ curOffset ]), curLen/charSize );
#else
        TPtr curDes( &aBuffer[ curOffset ], curLen/charSize );
#endif // __UNICODE                                 
        curDes.SetLength( curDes.MaxLength() );
        
        curOffset += curLen;
        
        if ( aBuffer.Length() < curOffset )
            {        
            ret =  KErrArgument;
            break;
            } 
        
        ret = Set( curID, curDes );
        
        if ( ret != KErrNone )
            {
            break;
            }
        }
    
    return ret;
    }



// ----------------------------------------------------------
// CSCPParamObject::GetBuffer()
// Form a buffer containing the stored parameters
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C TInt CSCPParamObject::GetBuffer( HBufC8*& aBuffer )
    { 
    // Calculate the space required
    // The format is: 
    // <numparams> + <numparams>*(<paramid><paramlen>) + 
    // <numparams>*<paramvalue>
    TInt intSize = sizeof( TInt );
    
#ifdef UNICODE
    TInt charSize = 2;
#else
    TInt charSize = 1;
#endif // __UNICODE    

    TInt headerSpace = ( 1 + iParamIDs.Count() * 2) * intSize;
    TInt dataSpace = 0;
    
    TInt i;
    for ( i = 0; i < iParamValues.Count(); i++ )
        {
        // Add the size of each value
        dataSpace += ( iParamValues[i]->Des() ).Length() * charSize;
        }
    TInt spaceNeeded = headerSpace + dataSpace; 
    
    HBufC8* buffer = NULL;
    
    TRAPD( ret, buffer = HBufC8::NewL( spaceNeeded ) );        
    if ( ret != KErrNone )
        {
        Dprint( (_L("--> CSCPParamObject::GetBuffer(): Allocation FAILED") ));
        }
    else
        {                          
        TPtr8 bufPtr = buffer->Des();
        TUint8* theBuffer = const_cast<TUint8*>( bufPtr.Ptr() );
        
        // Header
        *(reinterpret_cast<TInt*>( &theBuffer[0] )) = iParamIDs.Count();    
    
        for ( i = 0; i < iParamIDs.Count(); i++ )
            {
            // 2 ints per param, offset 1 = ID, offset 2 = length
            *(reinterpret_cast<TInt*>( &theBuffer[ (i * 2 + 1) * intSize ] ))
                = iParamIDs[i]; // ID
            *(reinterpret_cast<TInt*>( &theBuffer[ (i * 2 + 2) * intSize ] ))
                = ( iParamValues[i]->Des() ).Length() * charSize; // length                    
            }
    
        TInt curOffset = headerSpace;    
    
        // Copy the values after the header
        for ( i = 0; i < iParamIDs.Count(); i++ )
            {
            TAny* target = &theBuffer[ curOffset ];
            TPtr sourceDes = iParamValues[i]->Des();
            const TAny* source = static_cast<const TAny*>( sourceDes.Ptr() );
        
            memcpy( target, source, sourceDes.Length() * charSize );
            
            curOffset += sourceDes.Length() * charSize;
            }
        
        bufPtr.SetLength( curOffset );
        aBuffer = buffer;
        }
    
    return ret;
    }
                                         

      
// ----------------------------------------------------------
// CSCPParamObject::WriteToFile()
// Write the output from GetBuffer to a file
//
// Status: Approved
// ----------------------------------------------------------
//
EXPORT_C void CSCPParamObject::WriteToFileL( TDesC& aFilename, RFs* aRfs /*= NULL*/ )
    {
    HBufC8* paramBuffer;
    
    TInt ret = GetBuffer( paramBuffer );
    if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPPluginEventHandler::WritePluginConfigurationL(): FAILED \
            to get the configuration buffer: %d"), ret ));
        User::Leave( ret );
        }
    CleanupStack::PushL( paramBuffer );
            
    RFs* fsSessionPtr;
    RFs fsSession;
    
    if ( aRfs != NULL )
        {
        fsSessionPtr = aRfs;
        }
    else
        {
        User::LeaveIfError( fsSession.Connect() );
        CleanupClosePushL( fsSession );
        fsSessionPtr = &fsSession;
        }
        
    TPtr8 paramDes = paramBuffer->Des();        
       
    RFile configFile;
       
    // Make sure that the target directory exists
    ret = fsSessionPtr->MkDirAll( aFilename );
    if ( ( ret != KErrNone ) && ( ret != KErrAlreadyExists ) )
        {
        Dprint( (_L("CSCPPluginEventHandler::WritePluginConfigurationL(): FAILED \
            to create the target directory: %d"), ret ));
        User::Leave( ret );
        }    
          
    // Create or replace the existing file
    ret = configFile.Replace( *fsSessionPtr, 
            aFilename, 
            EFileShareAny | EFileWrite
        );
        
    if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPPluginEventHandler::WritePluginConfigurationL():\
            FAILED to replace configuration file: %d"), ret ));                   
        User::Leave( ret );
        }
            
    CleanupClosePushL( configFile );
        
    // Write the data
    ret = configFile.Write( paramDes );    
                
    if ( aRfs != NULL )
        {
        CleanupStack::PopAndDestroy(); // configFile
        }
    else
        {
        CleanupStack::PopAndDestroy( 2 ); // configFile - fsSession  
        }              
    
    CleanupStack::PopAndDestroy( paramBuffer );    
    
    if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPPluginEventHandler::WritePluginConfigurationL():\
            FAILED to write configuration file: %d"), ret ));
        User::Leave( ret );
        }                                                        
    }
        
       
// ----------------------------------------------------------
// CSCPParamObject::ReadFromFile()
// Fetch the configuration from disk
//
// Status: Approved
// ---------------------------------------------------------- 
//
EXPORT_C void CSCPParamObject::ReadFromFileL( TDesC& aFilename, RFs* aRfs /*= NULL*/ )
    {    
    TInt ret;
      
    RFs* fsSessionPtr;
    RFs fsSession;
    
    if ( aRfs != NULL )
        {
        fsSessionPtr = aRfs;
        }
    else
        {
        User::LeaveIfError( fsSession.Connect() );
        CleanupClosePushL( fsSession );
        fsSessionPtr = &fsSession;
        }           
    
    RFile configFile;        
    
    ret = configFile.Open( *fsSessionPtr, 
                aFilename, 
                EFileShareAny | EFileRead 
            );
    
    if ( ret != KErrNone )
        {                                    
        Dprint( (_L("CSCPParamObject::ReadFromFileL(): Failed \
            to open config file: %d"), ret ));        
        User::Leave( ret );                    
        }
            
    CleanupClosePushL( configFile );
    
    TInt size;
    ret = configFile.Size( size );
    if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPParamObject::ReadFromFile(): Failed \
            to get config file size: %d"), ret ));
        User::Leave( ret );
        }        
    
    HBufC8* paramBuf = HBufC8::NewL( size );
    CleanupStack::PushL( paramBuf );
    
    TPtr8 paramDes = paramBuf->Des();
    ret = configFile.Read( paramDes );
    if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPParamObject::ReadFromFile(): Failed \
            read configuration data: %d"), ret ));
        User::Leave( ret );
        }        
    
    // Parse the parameters from the buffer
    ret = Parse( paramDes );            
    
    CleanupStack::PopAndDestroy( paramBuf );
    
    if ( aRfs != NULL )
        {
        CleanupStack::PopAndDestroy(); // configFile
        }
    else
        {
        CleanupStack::PopAndDestroy( 2 ); // configFile - fsSession  
        }          
    
	if ( ret != KErrNone )
        {
        Dprint( (_L("CSCPParamObject::ReadFromFile(): Failed \
            to parse buffer for parameters: %d"), ret ));
        User::Leave( ret );
        }    
    }

EXPORT_C TInt CSCPParamObject::AddtoFailedPolices( TInt aFailedpolicyID )
    {
    return iFailedPolicyIDs.Append(aFailedpolicyID);
    }
	
EXPORT_C const RArray<TInt>& CSCPParamObject::GetFailedPolices( )
    {
    return iFailedPolicyIDs;
    }
// ----------------------------------------------------------
// CSCPParamObject::~CSCPParamObject()
// Destructor
//
// Status: Approved
// ----------------------------------------------------------
EXPORT_C CSCPParamObject::~CSCPParamObject()
	{
	Dprint( (_L("--> CSCPParamObject::~CSCPParamObject()") ));
	
	Reset();
	
	Dprint( (_L("<-- CSCPParamObject::~CSCPParamObject()") ));
	}

// End of file

