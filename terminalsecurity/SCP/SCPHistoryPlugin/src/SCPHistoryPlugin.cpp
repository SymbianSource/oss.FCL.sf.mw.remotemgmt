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
#include <e32base.h>
#include <e32std.h>
#include <ecom/implementationproxy.h>
#include <SCPParamObject.h>

#include "SCPHistoryPlugin.h"
#include <SCPHistoryPluginLang.rsg>
#include "SCP_IDs.h"


// ============================= LOCAL FUNCTIONS  =============================

// ============================= MEMBER FUNCTIONS =============================

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::NewL
// Two-phased contructor
// (static, may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPHistoryPlugin* CSCPHistoryPlugin::NewL()
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::NewL()" ) ) );
	CSCPHistoryPlugin* self = new ( ELeave ) CSCPHistoryPlugin();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
    Dprint ( ( _L( "( 0x%x ) CSCPHistoryPlugin::NewL()" ), self ) );

    return self;
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::CSCPHistoryPlugin
// Constructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPHistoryPlugin::CSCPHistoryPlugin()
   
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::CSCPHistoryPlugin()" ) ) );
    return;
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::ConstructL
// 2nd phase construction
// (may leave)
// Status : Approved
// ----------------------------------------------------------------------------
//
void CSCPHistoryPlugin::ConstructL()
    {        
    Dprint ( ( _L( "CSCPHistoryPlugin::ConstructL()" ) ) );
    return;
    }
    
// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::HandleEvent
// 
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
CSCPParamObject* CSCPHistoryPlugin::HandleEvent( TInt aID, CSCPParamObject& aParam )
	{				
	Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent()" ) ) );
	// Make the ParamObject for success ack, Delete later
	CSCPParamObject* retParams = NULL;
	
	TBool errRaised;
	errRaised = EFalse;
	
	TBool isInvalid = EFalse;
	
	if ( iFs == NULL )
	    {
	    return NULL; // Eventhandler not available
	    }	
	
	// Insert the default security code into the history-buffer if not there yet
    {	        
    TInt errSCF = SetConfigFile ();
	if (errSCF != KErrNone)
	    {		
		return NULL;
		}
		
    TInt historyItemCounter = 0;
    if ( GetHistoryItemCount( historyItemCounter ) != KErrNone )
	    {
	    Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent historyItemCounter = %d" ), historyItemCounter ) );
	    // Hash the security code	
		TBuf<KSCPPasscodeMaxLength> codeBuf;
		TBuf<KSCPMaxHashLength> hashBuf;
		
		codeBuf.Copy( KSCPDefaultEnchSecCode );
		hashBuf.Zero();
		
		iEventHandler->HashInput( codeBuf, hashBuf );		        
        
        CSCPParamObject* historyObject = NULL;
	    TRAPD( err, historyObject = CSCPParamObject::NewL() );
	    if ( err == KErrNone )
	        {
 		    historyObject->Set( KHistoryCounterParamID, 1 );
		    historyObject->Set( KHistoryItemParamBase, hashBuf );
        
            TRAP_IGNORE( historyObject->WriteToFileL( iCfgFilenamepath, iFs ) );
	        }
	    
	    delete historyObject;
	    }
    }
	
	
	// check for Case
    switch ( aID )
        {

        case ( KSCPEventValidate ) :
            {            
           	// Obtain the paramValue
           	Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventValidate" ) ) );
			TInt passhistoryParamValue;
			passhistoryParamValue = GetHistoryCountParamValue();
			Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent passhistoryParamValue = %d" ), passhistoryParamValue ) );
			// if all required bounds are zero, there is nothing to do.
			if ( passhistoryParamValue != 0)
			    {
				// Get the configFile's path.
				// If this fails, there is something badly wrong(Private folder is not there)
				TInt errSCF = SetConfigFile ();
				if (errSCF != KErrNone)
				{
					errRaised = ETrue;
					break; // Break out from Case
				}

				// Get the historyItemCount, If the err is raised, the file is not there
				// This will lead to KSCPEventPasswordChanged event and new history file will
				// be created
				TInt historyItemCounter;
				TInt errHC = GetHistoryItemCount( historyItemCounter );
				Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent historyItemCounter = %d" ), historyItemCounter ) );
				if (errHC != KErrNone)
				{
					errRaised = ETrue;
					break; // Break out from Case
				}

				// continue with the KSCPEventValidate Check

				// Get the password from the paramObject
				TBuf<KSCPPasscodeMaxLength> seccode;
				if ( aParam.Get( KSCPParamPassword, seccode ) != KErrNone )
				{
					// Nothing to do anymore
					Dprint( (_L("CSCPHistoryPlugin::HandleEvent()\
					ERROR: KSCPEventValidate/KSCPParamPassword is != KErrNone") ));
					errRaised = ETrue;
					break; // Break out from Case
				}            
				
				// Hash  the securitycode	
				TBuf<KSCPPasscodeMaxLength> securityhash;
				iEventHandler->HashInput(seccode,securityhash);
			
				// get history
				CDesCArrayFlat* array = NULL;
				TInt errGH = KErrNone;
				
				array = new CDesCArrayFlat(1);			
				if ( array != NULL )
				    {
				    TRAPD( err2, errGH = GetHistoryArrayL( *array ) );
				    if ( err2 != KErrNone )
				        {
				        errGH = err2;
				        }
				    }
				else
				    {
				    errGH = KErrNoMemory;
				    }
			
				// If for some reason err is raised, break out
				// If the Historyonfig file get deleted on the fly ex
				if (errGH != KErrNone)
				    {
					errRaised = ETrue;
					array->Reset();
					delete array;
					break; // Break out from Case
				    }
				TInt correction;
				correction = 0;

				if ( array->Count() >=  passhistoryParamValue )
        {
        correction =  array->Count() - passhistoryParamValue;
        }
				// check for match
				TBuf<KSCPPasscodeMaxLength> arrayItem;
				
				// Set the historyobject
				for (TInt i= 0 + correction; i < array->Count(); i++)
				    {
					arrayItem =  array->MdcaPoint(i);
					if (arrayItem.Compare(securityhash) == KErrNone)
					    {
						// Get the filesystem for Resource
						// If fail, bail out
						TInt errgGR = GetResource();
						if (errgGR != KErrNone)
						    {
							errRaised = ETrue;
							break; // Break out from the For
						    }	

						// Prompt buf, iNote can show only 97 chars,
						// without ... markings.
						HBufC* hbuf = NULL;					
						
						if ( passhistoryParamValue == 1 )
						    {
		                    isInvalid = ETrue;
		                    TRAP_IGNORE(
		                        hbuf = LoadAndFormatResL( R_SET_SEC_CODE_INFO_PREVIOUS );
		                        );
						    }
						else
						    {
		                    isInvalid = ETrue;
		                    TRAP_IGNORE(
		                        hbuf = LoadAndFormatResL( 
		                            R_SET_SEC_CODE_INFO_CHECK, 
		                            &passhistoryParamValue );
		                        );							    
						    }														

                        if ( isInvalid )
    					    {	    							
    				    	// Create the result-object to return
    					    TRAPD( err, retParams  = CSCPParamObject::NewL() );
                            
                            if ( err == KErrNone )
    					        {
        			            retParams->Set( KSCPParamStatus, KErrSCPInvalidCode );
    	    		            retParams->Set( KSCPParamAction, KSCPActionShowUI );
    		    	            retParams->Set( KSCPParamUIMode, KSCPUINote );
    			                
    			                if ( hbuf != NULL )
    			                    {
    			                    TPtr ptr = hbuf->Des();
    			                    retParams->Set( KSCPParamPromptText, ptr );
    			                    delete hbuf;
    			                    }
    					        }
    					    
    					    break;
    					    }
																															
					    } // End of compare IF
				    } // End of For
				    
				// kill the local
				array->Reset();
				delete array;
															
			    } // passhistoryParamValue
			else
			    {
				retParams = NULL;
			    }

			break;
			} // end of KSCPEventValidate
                    
        // Someone has changed the Seccode and I need to include it to history
         case ( KSCPEventPasswordChanged ) :
			{																
			// Get the configFile's path.
			Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventPasswordChanged" ) ) );
			TInt errSCF = SetConfigFile ();
			if (errSCF != KErrNone)
			    {
				errRaised = ETrue;
				break; // Break out from the case
			    }
			
			// Get the password from the paramObject
     		TBuf<KSCPPasscodeMaxLength> securitycode;
            if ( aParam.Get( KSCPParamPassword, securitycode ) != KErrNone )
                {
            	// Nothing to do anymore
               	Dprint( (_L("CSCPHistoryPlugin::HandleEvent()\
               	ERROR: KSCPEventPasswordChanged/KSCPParamPassword is  != KErrNone") ));
            	errRaised = ETrue;
				break; // Break out from the Case
                }          

			// Hash  the securitycode	
			TBuf<KSCPPasscodeMaxLength> securityhash;
			iEventHandler->HashInput(securitycode,securityhash);

			// Get the historyItemCount, If error occures, File is not there yet, Make one
			TInt historyItemCounter;
			TInt errHC = GetHistoryItemCount( historyItemCounter );
			Dprint ( ( _L( "CSCPHistoryPlugin::HandleEvent historyItemCounter = %d" ), historyItemCounter ) );
			if (errHC != KErrNone)
			    {
				// The file does not exist yet (should not happen)
				// Make the ParamObject,  Set the New historyData with count of 1
				CSCPParamObject* historyObject = NULL;
				TRAPD( err, historyObject = CSCPParamObject::NewL() );
		 		if ( err == KErrNone )
		 		    {
			 		historyObject->Set(KHistoryCounterParamID,1);
    				historyObject->Set(KHistoryItemParamBase,securityhash );
                    
                    TRAPD( errWC, historyObject->WriteToFileL( iCfgFilenamepath, iFs ) );
					if ( errWC != KErrNone )
    					{
						Dprint( (_L("CSCPHistoryPlugin::HandleEvent(): WARNING:\
						failed to write plugin configuration: %d"), errWC ));
						errRaised = ETrue;
						break; // Break out from the Case
	    				}
					delete historyObject;
		 		    }
			    }
			// There are passwords avail.
			else
			    {
				// Append the new passwords
				TInt err = KErrNone;
				TRAPD( err2, err = AppendAndWriteSecurityCodeL( securityhash  ) );
				if ( ( err != KErrNone ) || ( err2 != KErrNone ) )
				    {
					errRaised = ETrue;
					break; // Break out from the Case						
				    }										
			    }    					
			break;
			} // end of KSCPEventPasswordChanged
          
    	case ( KSCPEventConfigurationQuery ):
            {            
            Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventConfigurationQuery" ) ) );
        	TInt paramID = -1; 
            // Get the ID from the paramObject      
            if ( aParam.Get( KSCPParamID, paramID ) != KErrNone )
                {
                // Nothing to do anymore
                break;
                }            
                    
            // 1011
            if ( paramID == (RTerminalControl3rdPartySession::EPasscodeHistoryBuffer))
                {
				// OK, we're interested, check that the value is valid
				TRAPD( err, retParams  = CSCPParamObject::NewL() );
				if ( err != KErrNone )
				    {
				    break; // Nothing we can do
				    }

				// All of our params are TInts
				TInt paramValue;
				if ( aParam.Get( KSCPParamValue, paramValue ) != KErrNone )
				    {
					retParams->Set( KSCPParamStatus, KErrGeneral );
					break;
				    }
            	            
					TInt retStatus = KErrNone;
					switch ( paramID )
					    {
				
						case ( RTerminalControl3rdPartySession::EPasscodeHistoryBuffer ):
						    {
							// Bounds are be be
							if ( ( paramValue < KPasscodeHistoryBufferMinValue ) 
							|| ( paramValue > KPasscodeHistoryBufferMaxValue ) )                                 
							    {
								// This is not a valid valuerange
								retStatus = KErrArgument;
						        }     
							           
						    break;				
						    } // end of case EPasscodeHistoryBuffer
					    } // end of switch ( paramID )
								
			        retParams->Set( KSCPParamStatus, retStatus );
                    }
			    else
			        {
				    retParams = NULL;
			        }
			 			 
			 break;	            
	         } //End of KSCPEventConfigurationQuery Case
            
            
          case ( KSCPEventReset ):
              {
              Dprint ( ( _L( "CSCPHistoryPlugin::KSCPEventReset" ) ) );
              // Reset the configuration for this plugin.
              TRAP_IGNORE( FlushConfigFileL() );
              
              break;
              }
                          
          } // End of  switch ( aID )
                             
       // Check if Any errors were raised and handle it
    if (errRaised) 
        {
        if ( retParams != NULL )
            {
            delete retParams;
            }
        retParams = NULL;
        }
       
    return retParams; 
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::SetEventHandler
// SetEventHandler
// 
// Status : Approved
// ----------------------------------------------------------------------------
//    
void CSCPHistoryPlugin::SetEventHandler( MSCPPluginEventHandler* aHandler )
	{
	Dprint ( ( _L( "CSCPHistoryPlugin::SetEventHandler()" ) ) );
	iEventHandler = aHandler;
	
	iFs = &(iEventHandler->GetFsSession());
	}	

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::~CSCPHistoryPlugin
// Destructor
// Status : Approved
// ----------------------------------------------------------------------------
//
CSCPHistoryPlugin::~CSCPHistoryPlugin()
    {
	Dprint( (_L("CSCPHistoryPlugin::~CSCPHistoryPlugin()") ));
    
	iRf.Close();   
	    
	return;
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::GetHistoryItemCount
// GetHistoryItemCount
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPHistoryPlugin::GetHistoryItemCount( TInt& aHistoryCount )
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryItemCount()" ) ) );
    // Make the ParamObject for success ack, 
    CSCPParamObject* historyObject = NULL;
    TRAPD( err, historyObject = CSCPParamObject::NewL() );
    if ( err != KErrNone )
        {
        return err;
        }

    TRAP( err, historyObject->ReadFromFileL( iCfgFilenamepath, iFs ) );
    if (  err != KErrNone  )
        {
    	// Something is wrong with the config file
    	// it is missing, caller will create new file
    	Dprint( (_L("CSCPHistoryPlugin::SetConfigFile:\
    	ERROR: Failed to ReadFromFileL: %d"), err ));
        }
    else
        {
    	historyObject->Get(KHistoryCounterParamID,aHistoryCount);
        }
	Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryItemCount aHistoryCount = %d" ), aHistoryCount ) );
    delete historyObject;
		             
	return err;
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::SetConfigFile
// SetConfigFile
// Status : Approved
// ----------------------------------------------------------------------------
//

TInt CSCPHistoryPlugin::SetConfigFile()
    {	
    Dprint ( ( _L( "CSCPHistoryPlugin::SetConfigFile()" ) ) );
	TRAPD( err, iEventHandler->GetStoragePathL( iCfgFilenamepath ) );
	if ( err != KErrNone )
	    {
		Dprint( (_L("CSCPHistoryPlugin::SetConfigFile:\
		ERROR: Failed to get storage path: %d"), err ));             
		return err;
	    }
	iCfgFilenamepath.Append( KConfigFile );
	return err;
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::AppendSecurityCode
// AppendSecurityCode
// Status : Approved
// ----------------------------------------------------------------------------
//

TInt CSCPHistoryPlugin::AppendAndWriteSecurityCodeL ( TDes& aSecuritycode )
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::AppendAndWriteSecurityCodeL()" ) ) );
	TInt err;
	err = KErrNone;
		
	TInt passhistoryParamValue;
	passhistoryParamValue = GetHistoryCountParamValue();
	Dprint ( ( _L( "CSCPHistoryPlugin::AppendAndWriteSecurityCodeL passhistoryParamValue = %d" ), passhistoryParamValue ) );
	if ( passhistoryParamValue == 0 )
	    {
	    // We must still save the currect password..
	    passhistoryParamValue = 1;
	    }

	// Make the ParamObject
	CSCPParamObject* historyObject = CSCPParamObject::NewL();
	CleanupStack::PushL( historyObject );
	
	// get history
	CDesCArrayFlat*  array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( array );
	
	err = GetHistoryArrayL( *array );
	// If for some reason err is raised, break out
	if (err != KErrNone)
	    {
		array->Reset();
		CleanupStack::PopAndDestroy( array );
		CleanupStack::PopAndDestroy( historyObject );
		return err;
	    }

	TBuf<KSCPPasscodeMaxLength> arrayItem;
	
  	// append the new pass to history to last index.  	
	array->AppendL( aSecuritycode );
	
	TInt correction;
	correction = 0;

	if ( array->Count() >=  passhistoryParamValue )
        {
        correction =  array->Count() - passhistoryParamValue;
        }    

  	// Set the historyobject
	for ( TInt i = 0 + correction; i <  array->Count(); i++ )
		{
        arrayItem =  array->MdcaPoint(i);
        historyObject->Set( KHistoryItemParamBase + i - correction, arrayItem );
        arrayItem.Zero();
		}
					
	// Set the historycount
	historyObject->Set(KHistoryCounterParamID, array->Count() - correction );

	// Write
	TRAP( err, historyObject->WriteToFileL( iCfgFilenamepath, iFs ) );
	if (  err != KErrNone  )
	    {
		Dprint( (_L("CSCPHistoryPlugin::AppendSecurityCode(): WARNING:\
		failed to write plugin configuration: %d"), err )); 
	    }
	
	// Kill the local array
	array->Reset();
	CleanupStack::PopAndDestroy( array );
	
	CleanupStack::PopAndDestroy( historyObject );
	
	return err;	
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::GetHistoryArray
// Reads the historyconfig file and retrieves the history data
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPHistoryPlugin::GetHistoryArrayL ( CDesCArrayFlat& array )
    {   
	Dprint( (_L("CSCPHistoryPlugin::GetHistoryArray()") ));
	
    TBuf<KSCPPasscodeMaxLength> arrayItem;
    TInt historyCount;
	 
	// Make the ParamObject
	CSCPParamObject* historyObject = CSCPParamObject::NewL();
	CleanupStack::PushL( historyObject );

	TRAPD( err, historyObject->ReadFromFileL( iCfgFilenamepath, iFs ) );
	if ( err != KErrNone ) 
	    {
		// Reading from history fails. 
		Dprint( (_L("CSCPHistoryPlugin::GetHistoryArray():\
		failed to read plugin configuration: %d"), err )); 
		CleanupStack::PopAndDestroy( historyObject );
		return err;
	    }

	// Get the historyCount
	historyObject->Get(KHistoryCounterParamID,historyCount);
	Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryArrayL historyCount = %d" ), historyCount ) );
	// Loop them into array
	
	for (TInt i = 0 ; i < historyCount ; i ++)
    	{
		historyObject->Get(KHistoryItemParamBase+i,arrayItem);
		array.AppendL(arrayItem);
		arrayItem.Zero();
	    }

    CleanupStack::PopAndDestroy( historyObject );
	return err;	
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::GetHistoryCountParamValue
// GetHistoryCountParamValue
// Status : Approved
// ----------------------------------------------------------------------------
//

TInt CSCPHistoryPlugin::GetHistoryCountParamValue()
    {
	Dprint( (_L("CSCPHistoryPlugin::GetHistoryCountParamValue()") ));
	// Get required params for bounds for EPasscodeHistoryBuffer
	CSCPParamObject& config = iEventHandler->GetParameters();	

	// These are the dault values for ThisPlugIn's functions
	TInt passcodehistorycount;

	// Get Values with ID's
	if ( config.Get( ( RTerminalControl3rdPartySession::EPasscodeHistoryBuffer), 
	                   passcodehistorycount ) !=  KErrNone )
	    {
	    passcodehistorycount = 0;
	    }	
	Dprint ( ( _L( "CSCPHistoryPlugin::GetHistoryCountParamValue passcodehistorycount = %d" ), passcodehistorycount ) );
	return passcodehistorycount;
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::GetResource
// GetResource
// Status : Approved
// ----------------------------------------------------------------------------
//

TInt CSCPHistoryPlugin::GetResource()
    {
 	Dprint( (_L("CSCPSpecificStringsPlugin::GetResource()") ));
	// The resource has to be loaded manually since it is not an application.
          	
	TFileName resourceFile;
	resourceFile.Append( KDriveZ );
	resourceFile.Append( SCPHistoryPluginSrcFile );
	BaflUtils::NearestLanguageFile( *iFs, resourceFile );
	TRAPD( err, iRf.OpenL( *iFs, resourceFile ) );

	if ( err == KErrNone )
	    {
		TRAP( err, iRf.ConfirmSignatureL() );
    	}          
    
    return err;       
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::LoadResourceLC
// GetResource
// Status : Approved
// ----------------------------------------------------------------------------
//
HBufC16* CSCPHistoryPlugin::LoadResourceLC ( TInt aResId )
    {
	Dprint( (_L("CSCPHistoryPlugin::LoadResourceLC()") ));

	// load the actual resource
    HBufC8* readBuffer = iRf.AllocReadLC( aResId );
    // as we are expecting HBufC16...
    const TPtrC16 ptrReadBuffer( (TText16*) readBuffer->Ptr(),
                                 ( readBuffer->Length() + 1 ) >> 1 );
                                 
    HBufC16* textBuffer=HBufC16::NewL( ptrReadBuffer.Length() );    
    *textBuffer=ptrReadBuffer;
    CleanupStack::PopAndDestroy( readBuffer ); // readBuffer
    CleanupStack::PushL( textBuffer );
  	return textBuffer;
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::FlushConfigFile
// Remove all the other passwords from the file, except the last one (current)
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
TInt CSCPHistoryPlugin::FlushConfigFileL()
    {			
    Dprint ( ( _L( "CSCPHistoryPlugin::FlushConfigFileL()" ) ) );
	// Make the ParamObject
	CSCPParamObject* historyObject = CSCPParamObject::NewL();
	CleanupStack::PushL( historyObject );
	
	// get history
	CDesCArrayFlat*  array = new (ELeave) CDesCArrayFlat(1);
	CleanupStack::PushL( array );
	
	TInt err = GetHistoryArrayL( *array );
	// If for some reason err is raised, break out
	if (err != KErrNone)
	    {
		array->Reset();
		CleanupStack::PopAndDestroy( array );
		CleanupStack::PopAndDestroy( historyObject );
		return err;
	    }
	
	// Set the parameters
	TBuf<KSCPPasscodeMaxLength> arrayItem = array->MdcaPoint( array->Count() - 1 );
	
	historyObject->Set( KHistoryCounterParamID, 1 );			
	historyObject->Set( KHistoryItemParamBase, arrayItem );
	
	// Write
	TRAP( err, historyObject->WriteToFileL( iCfgFilenamepath, iFs ) );
	if (  err != KErrNone  )
	    {
		Dprint( (_L("CSCPHistoryPlugin::AppendSecurityCode(): WARNING:\
		failed to write plugin configuration: %d"), err )); 
	    }
	
	// Kill the local array
	array->Reset();
	CleanupStack::PopAndDestroy( array );
	
	CleanupStack::PopAndDestroy( historyObject );
	
	return err;			
    }


// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::LoadAndFormatResL
// Load the given resouce, and format the string according to the TInt parameters
// if given.
// 
// Status : Approved
// ----------------------------------------------------------------------------
//
HBufC* CSCPHistoryPlugin::LoadAndFormatResL( TInt aResId, TInt* aParam1, TInt* aParam2 )
    {
    Dprint ( ( _L( "CSCPHistoryPlugin::LoadAndFormatResL()" ) ) );
    HBufC16* resource = NULL;
    HBufC* hbuf = NULL;
    
    resource = LoadResourceLC( aResId );
    FormatResourceString (*resource);
    TInt allocLen = 0;
    if ( aParam1 != NULL )
        {
        allocLen += KSCPMaxIntLength;
        }
    if ( aParam2 != NULL )
        {
        allocLen += KSCPMaxIntLength;
        }
                
	hbuf = HBufC::NewL( resource->Length() + allocLen );
	
	if ( ( aParam1 == NULL ) && ( aParam2 == NULL ) )
	    {
	    hbuf->Des().Copy( resource->Des() );
	    }
	else
	    {
	    if ( aParam1 == NULL )
	        {
	        hbuf->Des().Format( resource->Des(), *aParam2 );
	        }
	    else if ( aParam2 == NULL )
	        {
	        hbuf->Des().Format(resource->Des(), *aParam1 );
	        }
	    else
	        {
	        hbuf->Des().Format(resource->Des(), *aParam1, *aParam2 );
	        }	    
	    }
								    
	CleanupStack::PopAndDestroy( resource );
	return hbuf;
    }

// ----------------------------------------------------------------------------
// CSCPHistoryPlugin::FormatResourceString
// The buffer that is passed is formatted to have only %i as a format specifier instead of %N or %0N etc.
// 
// Status : Approved
// ----------------------------------------------------------------------------
//  
void CSCPHistoryPlugin::FormatResourceString(HBufC16 &aResStr)
{
		TInt pos = 0;
		TInt flag = 0;
        TPtr16 bufPtr = aResStr.Des();
        _LIT (mess1, "%N");
        _LIT (mess2, "%i");
        _LIT (mess3, "%0N");
        _LIT (mess4, "%1N");
                              
        while ((pos = bufPtr.Find(mess1)) !=KErrNotFound)
        {
              bufPtr.Replace(pos,2,mess2); 
              flag = 1;
              break;                    
        }
               
        if(flag == 0)
        {
              while ((pos = bufPtr.Find(mess3)) != KErrNotFound)
              {
              		bufPtr.Replace(pos,3,mess2);
              }
               		
              while ((pos = bufPtr.Find(mess4)) != KErrNotFound)
              {
                	bufPtr.Replace(pos,3,mess2);
              }
        }	
}
  
// End of File
